#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../toolkit/csi.h"

// ------------------------------------------------------------------------
// Front end data (FED) table structures.
// ------------------------------------------------------------------------

// A FED table is a flat list of FED entries, indexed by a CSI
// ID. Each FED table has its own private ID space.
typedef struct {
    int64_t num_entries;
    source_loc_t const * entries;
} fed_table_t;

// A FED "collection" is a list of FED tables of a particular type
// (e.g. basic block FEDs, function FEDs, etc). There is exactly one
// collection per type. When units are initialized, their FED tables
// are added to the collection of other FED tables of the same
// type. This process occurs in __csirt_unit_init(), which is before
// execution passes to the tool.
typedef struct {
    int64_t total_num_entries;
    int64_t num_fed_tables;
    int64_t capacity;
    fed_table_t *tables;
} fed_collection_t;

// One FED collection per type of FED table that we maintain across
// all units.
typedef enum {
    FED_COLL_BASICBLOCK,
    FED_COLL_FUNCTIONS,
    FED_COLL_FUNCTION_EXIT,
    FED_COLL_CALLSITE,
    FED_COLL_LOAD,
    FED_COLL_STORE,
    NUM_FED_TYPES // Must be last
} fed_type_t;

// ------------------------------------------------------------------------
// Globals
// ------------------------------------------------------------------------

// Starting number of FED tables allocated in a FED collection. The
// actual number of FED tables is equal to the number of units, but it
// can be slightly more efficient to over-allocate by a constant
// factor fewer times versus allocating exactly more times.
static const int DEFAULT_NUM_FED_TABLES = 128;

// The list of FED collections. This is indexed by a value of
// 'fed_type_t'.
static fed_collection_t *fed_collections = NULL;

// Initially false, set to true once the first unit is initialized,
// which results in the FED collections list being initialized.
static bool fed_collections_initialized = false;

// Initially false, set to true once the first unit is initialized,
// which results in the __csi_init() function being called.
static bool csi_init_called = false;

// ------------------------------------------------------------------------
// Private function definitions
// ------------------------------------------------------------------------

// Initialize the FED collections list, indexed by a value of type
// fed_type_t. This is called once, by the first unit to
// load.
static void initialize_fed_collections() {
    fed_collections = (fed_collection_t *)malloc(NUM_FED_TYPES * sizeof(fed_collection_t));
    for (unsigned i = 0; i < NUM_FED_TYPES; i++) {
        fed_collection_t coll;
        coll.total_num_entries = 0;
        coll.num_fed_tables = 0;
        coll.capacity = 0;
        coll.tables = NULL;
        fed_collections[i] = coll;
    }
    fed_collections_initialized = true;
}

// Ensure that the FED collection of the given type has enough memory
// allocated to add one more FED table to it.
static void ensure_fed_collection_capacity(fed_type_t fed_type) {
    bool need_realloc = false;
    if (!fed_collections_initialized) {
        initialize_fed_collections();
    }
    fed_collection_t *coll = &fed_collections[fed_type];
    if (coll->num_fed_tables == coll->capacity) {
        if (coll->tables) {
            coll->capacity *= 2;
        } else {
            coll->capacity = DEFAULT_NUM_FED_TABLES;
        }
        need_realloc = true;
    }
    if (need_realloc) {
        coll->tables = (fed_table_t *)realloc(coll->tables,
                                              coll->capacity * sizeof(fed_table_t *));
    }
}

// Add a new FED table to the FED collection of the given type. The
// new FED table is passed as a pointer to its list of entries
// 'fed_entries'. The number of entries in 'fed_entries' is
// 'num_entries'.
static inline void add_fed_table(fed_type_t fed_type, int64_t num_entries, source_loc_t const * fed_entries) {
    ensure_fed_collection_capacity(fed_type);
    fed_collection_t *coll = &fed_collections[fed_type];

    fed_table_t new_table;
    new_table.num_entries = num_entries;
    new_table.entries = fed_entries;
    coll->tables[coll->num_fed_tables++] = new_table;
}

// The unit-local counter pointed to by 'fed_id_base' keeps track of
// that unit's "base" ID value of the given type (recall that there is
// a private ID space per FED type). The "base" ID value is the global
// ID that corresponds to the unit's local ID 0. This function stores
// the correct value into a unit's base ID.
static inline void update_ids(fed_type_t fed_type, int64_t num_entries, csi_id_t *fed_id_base) {
    fed_collection_t *coll = &fed_collections[fed_type];
    // The base ID is the current number of FED entries so far
    *fed_id_base = coll->total_num_entries;
    coll->total_num_entries += num_entries;
}

// Return the FED entry of the given type, corresponding to the given
// CSI ID.
static inline source_loc_t const * get_fed_entry(fed_type_t fed_type, const csi_id_t csi_id) {
    // TODO(ddoucet): threadsafety
    csi_id_t sum = 0;
    fed_collection_t *coll = &fed_collections[fed_type];
    for (csi_id_t i = 0; i < coll->num_fed_tables; i++) {
        fed_table_t *table = &coll->tables[i];
        if (csi_id < sum + table->num_entries)
            return &table->entries[csi_id - sum];
        sum += table->num_entries;
    }

    return NULL;
}

// ------------------------------------------------------------------------
// External function definitions, including CSIRT API functions.
// ------------------------------------------------------------------------

EXTERN_C

typedef struct {
    int64_t num_entries;
    csi_id_t *id_base;
    source_loc_t const * entries;
} unit_fed_table_t;

// Function signature for the function (generated by the CSI compiler
// pass) that updates the callsite to function ID mappings.
typedef void (*__csi_init_callsite_to_functions)();

static inline instrumentation_counts_t compute_inst_counts(unit_fed_table_t *unit_fed_tables) {
    instrumentation_counts_t counts;
    int64_t *base = (int64_t *)&counts;
    for (unsigned i = 0; i < NUM_FED_TYPES; i++)
        *(base + i) = unit_fed_tables[i].num_entries;
    return counts;
}

// A call to this is inserted by the CSI compiler pass, and occurs
// before main().
void __csirt_unit_init(const char * const name,
                       unit_fed_table_t *unit_fed_tables,
                       __csi_init_callsite_to_functions callsite_to_func_init) {
    // TODO(ddoucet): threadsafety
    if (!csi_init_called) {
        __csi_init();
        csi_init_called = true;
    }

    // Add all FED tables from the new unit
    for (unsigned i = 0; i < NUM_FED_TYPES; i++) {
        add_fed_table(i, unit_fed_tables[i].num_entries, unit_fed_tables[i].entries);
        update_ids(i, unit_fed_tables[i].num_entries, unit_fed_tables[i].id_base);
    }

    // Initialize the callsite -> function mappings. This must happen
    // after the base IDs have been updated.
    callsite_to_func_init();

    // Call into the tool implementation.
    __csi_unit_init(name, compute_inst_counts(unit_fed_tables));
}

source_loc_t const * __csi_get_func_source_loc(const csi_id_t func_id) {
    return get_fed_entry(FED_COLL_FUNCTIONS, func_id);
}

source_loc_t const * __csi_get_func_exit_source_loc(const csi_id_t func_exit_id) {
    return get_fed_entry(FED_COLL_FUNCTION_EXIT, func_exit_id);
}

source_loc_t const * __csi_get_bb_source_loc(const csi_id_t bb_id) {
    return get_fed_entry(FED_COLL_BASICBLOCK, bb_id);
}

source_loc_t const * __csi_get_callsite_source_loc(const csi_id_t callsite_id) {
    return get_fed_entry(FED_COLL_CALLSITE, callsite_id);
}

source_loc_t const * __csi_get_load_source_loc(const csi_id_t load_id) {
    return get_fed_entry(FED_COLL_LOAD, load_id);
}

source_loc_t const * __csi_get_store_source_loc(const csi_id_t store_id) {
    return get_fed_entry(FED_COLL_STORE, store_id);
}

bool __csirt_is_callsite_target_unknown(const csi_id_t callsite_id, const csi_id_t func_id) {
    return func_id == -1;
}

EXTERN_C_END
