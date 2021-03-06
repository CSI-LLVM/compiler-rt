#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "csi.h"

// Compile-time assert the property structs are 64 bits.
static_assert(sizeof(func_prop_t) == 8, "Size of func_prop_t is not 64 bits.");
static_assert(sizeof(func_exit_prop_t) == 8, "Size of func_exit_prop_t is not 64 bits.");
static_assert(sizeof(bb_prop_t) == 8, "Size of bb_prop_t is not 64 bits.");
static_assert(sizeof(call_prop_t) == 8, "Size of call_prop_t is not 64 bits.");
static_assert(sizeof(load_prop_t) == 8, "Size of load_prop_t is not 64 bits.");
static_assert(sizeof(store_prop_t) == 8, "Size of store_prop_t is not 64 bits.");

#define CSIRT_API __attribute__((visibility("default")))

// ------------------------------------------------------------------------
// Front end data (FED) table structures.
// ------------------------------------------------------------------------

// A FED table is a flat list of FED entries, indexed by a CSI
// ID. Each FED table has its own private ID space.
typedef struct {
    int64_t num_entries;
    source_loc_t *entries;
} fed_table_t;

// Types of FED tables that we maintain across all units.
typedef enum {
    FED_TYPE_BASICBLOCK,
    FED_TYPE_FUNCTIONS,
    FED_TYPE_FUNCTION_EXIT,
    FED_TYPE_CALLSITE,
    FED_TYPE_LOAD,
    FED_TYPE_STORE,
    NUM_FED_TYPES // Must be last
} fed_type_t;

// ------------------------------------------------------------------------
// Globals
// ------------------------------------------------------------------------

// The list of FED tables. This is indexed by a value of
// 'fed_type_t'.
static fed_table_t *fed_tables = NULL;

// Initially false, set to true once the first unit is initialized,
// which results in the FED list being initialized.
static bool fed_tables_initialized = false;

// Initially false, set to true once the first unit is initialized,
// which results in the __csi_init() function being called.
static bool csi_init_called = false;

// ------------------------------------------------------------------------
// Private function definitions
// ------------------------------------------------------------------------

// Initialize the FED tables list, indexed by a value of type
// fed_type_t. This is called once, by the first unit to load.
static void initialize_fed_tables() {
    fed_tables = (fed_table_t *)malloc(NUM_FED_TYPES * sizeof(fed_table_t));
    assert(fed_tables != NULL);
    for (unsigned i = 0; i < NUM_FED_TYPES; i++) {
        fed_table_t table;
        table.num_entries = 0;
        table.entries = NULL;
        fed_tables[i] = table;
    }
    fed_tables_initialized = true;
}

// Ensure that the FED table of the given type has enough memory
// allocated to add a new unit's entries.
static void ensure_fed_table_capacity(fed_type_t fed_type, int64_t num_new_entries) {
    if (!fed_tables_initialized) {
        initialize_fed_tables();
    }
    fed_table_t *table = &fed_tables[fed_type];
    int64_t total_num_entries = table->num_entries + num_new_entries;
    if (total_num_entries > 0) {
        table->entries = (source_loc_t *)realloc(table->entries,
                                                 total_num_entries * sizeof(source_loc_t));
        table->num_entries = total_num_entries;
        assert(table->entries != NULL);
    }
}

// Add a new FED table of the given type.
static inline void add_fed_table(fed_type_t fed_type, int64_t num_entries, source_loc_t const * fed_entries) {
    ensure_fed_table_capacity(fed_type, num_entries);
    fed_table_t *table = &fed_tables[fed_type];
    csi_id_t base = table->num_entries - num_entries;
    for (csi_id_t i = 0; i < num_entries; i++) {
        table->entries[base + i] = fed_entries[i];
    }
}

// The unit-local counter pointed to by 'fed_id_base' keeps track of
// that unit's "base" ID value of the given type (recall that there is
// a private ID space per FED type). The "base" ID value is the global
// ID that corresponds to the unit's local ID 0. This function stores
// the correct value into a unit's base ID.
static inline void update_ids(fed_type_t fed_type, int64_t num_entries, csi_id_t *fed_id_base) {
    fed_table_t *table = &fed_tables[fed_type];
    // The base ID is the current number of FED entries before adding
    // the new FED table.
    *fed_id_base = table->num_entries - num_entries;
}

// Return the FED entry of the given type, corresponding to the given
// CSI ID.
static inline source_loc_t const * get_fed_entry(fed_type_t fed_type, const csi_id_t csi_id) {
    // TODO(ddoucet): threadsafety
    fed_table_t *table = &fed_tables[fed_type];
    if (csi_id < table->num_entries) {
        assert(table->entries != NULL);
        return &table->entries[csi_id];
    } else {
        return NULL;
    }
}

// ------------------------------------------------------------------------
// External function definitions, including CSIRT API functions.
// ------------------------------------------------------------------------

EXTERN_C

// Not used at the moment
// __thread bool __csi_disable_instrumentation;

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
CSIRT_API void __csirt_unit_init(const char * const name,
                       unit_fed_table_t *unit_fed_tables,
                       __csi_init_callsite_to_functions callsite_to_func_init) {
    // Make sure we don't instrument things in __csi_init or __csi_unit init.
    // __csi_disable_instrumentation = true;

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

    // Reset disable flag.
    // __csi_disable_instrumentation = false;
}

CSIRT_API source_loc_t const * __csi_get_func_source_loc(const csi_id_t func_id) {
    return get_fed_entry(FED_TYPE_FUNCTIONS, func_id);
}

CSIRT_API source_loc_t const * __csi_get_func_exit_source_loc(const csi_id_t func_exit_id) {
    return get_fed_entry(FED_TYPE_FUNCTION_EXIT, func_exit_id);
}

CSIRT_API source_loc_t const * __csi_get_bb_source_loc(const csi_id_t bb_id) {
    return get_fed_entry(FED_TYPE_BASICBLOCK, bb_id);
}

CSIRT_API source_loc_t const * __csi_get_callsite_source_loc(const csi_id_t callsite_id) {
    return get_fed_entry(FED_TYPE_CALLSITE, callsite_id);
}

CSIRT_API source_loc_t const * __csi_get_load_source_loc(const csi_id_t load_id) {
    return get_fed_entry(FED_TYPE_LOAD, load_id);
}

CSIRT_API source_loc_t const * __csi_get_store_source_loc(const csi_id_t store_id) {
    return get_fed_entry(FED_TYPE_STORE, store_id);
}

EXTERN_C_END
