#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "csirt.h"

EXTERN_C
typedef struct {
    int32_t line_number;
    char *filename;
} fed_entry;
EXTERN_C_END

static const int DEFAULT_NUM_FED_TABLES = 128;
static bool csi_init_called = false;

// A FED table is a flat list of FED entries.
typedef struct {
    uint64_t num_entries;
    fed_entry *entries;
} fed_table;

// A FED collection is a list of FED tables of a particular type, one per unit.
typedef struct {
    uint64_t total_num_entries;
    uint64_t num_fed_tables;
    uint64_t capacity;
    fed_table *tables;
} fed_collection;

// One FED collection type per type of FED table that we maintain across all units.
typedef enum {
    FED_COLL_BASICBLOCK,
    FED_COLL_FUNCTIONS,
    FED_COLL_FUNCTION_EXIT,
    FED_COLL_CALLSITE,
    FED_COLL_LOAD,
    FED_COLL_STORE,
    NUM_FED_COLLS // Must be last
} fed_collection_type;

static fed_collection *fed_collections = NULL;
static bool fed_collections_initialized = false;

// A rel table is a flat list of IDs.  This is used to describe
// one-to-one ID relationships (e.g. basic block to function ID, as a
// basic block can only belong to one function).
typedef struct {
    uint64_t num_ids;
    uint64_t *ids;
} rel_table;

// A rel "range" table is a flat list of ID ranges. This is used
// to describe one-to-many ID relationships (e.g. function ID to basic
// block ID range, as a function can contain more than one basic
// block).
typedef struct {
    uint64_t num_ranges;
    range_t *ranges;
} rel_range_table;

// Function signature for the CSI-generated function that updates all
// relation tables when a unit is loaded.
typedef void (*__csi_init_rel_tables_t)(rel_table *rel_bb_to_func, rel_range_table *rel_func_to_bb);

static rel_table rel_bb_to_func;
static rel_range_table rel_func_to_bb;

static void initialize_fed_collections() {
    fed_collections = (fed_collection *)malloc(NUM_FED_COLLS * sizeof(fed_collection));
    for (unsigned i = 0; i < NUM_FED_COLLS; i++) {
        fed_collection coll;
        coll.total_num_entries = 0;
        coll.num_fed_tables = 0;
        coll.capacity = 0;
        coll.tables = NULL;
        fed_collections[i] = coll;
    }
    fed_collections_initialized = true;
}

static void ensure_fed_collection_capacity(fed_collection_type fed_type) {
    bool need_realloc = false;
    if (!fed_collections_initialized) {
        initialize_fed_collections();
    }
    fed_collection *coll = &fed_collections[fed_type];
    if (coll->num_fed_tables == coll->capacity) {
        if (coll->tables) {
            coll->capacity *= 2;
        } else {
            coll->capacity = DEFAULT_NUM_FED_TABLES;
        }
        need_realloc = true;
    }
    if (need_realloc) {
        coll->tables = (fed_table *)realloc(coll->tables,
                                            coll->capacity * sizeof(fed_table *));
    }
}

static inline void add_fed_table(fed_collection_type fed_type, uint64_t num_entries, fed_entry *fed_entries) {
    ensure_fed_collection_capacity(fed_type);
    fed_collection *coll = &fed_collections[fed_type];

    fed_table new_table;
    new_table.num_entries = num_entries;
    new_table.entries = fed_entries;
    coll->tables[coll->num_fed_tables++] = new_table;
}

static inline void update_ids(fed_collection_type fed_type, uint64_t num_entries, uint64_t *fed_id_base) {
    fed_collection *coll = &fed_collections[fed_type];
    *fed_id_base = coll->total_num_entries;
    coll->total_num_entries += num_entries;
}

static inline fed_entry *get_fed_entry(fed_collection_type fed_type, uint64_t csi_id) {
    // TODO(ddoucet): threadsafety
    uint64_t sum = 0;
    fed_collection *coll = &fed_collections[fed_type];
    for (uint64_t i = 0; i < coll->num_fed_tables; i++) {
        fed_table *table = &coll->tables[i];
        if (csi_id < sum + table->num_entries)
            return &table->entries[csi_id - sum];
        sum += table->num_entries;
    }

    fprintf(stderr, "ERROR: Unable to find FED entry for csi_id %lu\n", csi_id);
    exit(-1);
}

static inline void realloc_rel_table(rel_table *table, uint64_t num_entries) {
    table->num_ids += num_entries;
    table->ids = (uint64_t *)realloc(table->ids,
                                     table->num_ids * sizeof(uint64_t));
}

static inline void realloc_rel_range_table(rel_range_table *table, uint64_t num_entries) {
    table->num_ranges += num_entries;
    table->ranges = (range_t *)realloc(table->ranges,
                                       table->num_ranges * sizeof(range_t));
}

static inline void realloc_rel_tables(uint64_t num_bb_to_func_rel_entries,
                                      uint64_t num_func_to_bb_rel_entries) {
    realloc_rel_table(&rel_bb_to_func, num_bb_to_func_rel_entries);
    realloc_rel_range_table(&rel_func_to_bb, num_func_to_bb_rel_entries);
}

EXTERN_C

void __csirt_unit_init(const char * const name,
                       uint64_t num_func_entries,
                       uint64_t *fed_func_id_base,
                       fed_entry *fed_func_entries,
                       uint64_t num_func_exit_entries,
                       uint64_t *fed_func_exit_id_base,
                       fed_entry *fed_func_exit_entries,
                       uint64_t num_bb_entries,
                       uint64_t *fed_bb_id_base,
                       fed_entry *fed_bb_entries,
                       uint64_t num_callsite_entries,
                       uint64_t *fed_callsite_id_base,
                       fed_entry *fed_callsite_entries,
                       uint64_t num_load_entries,
                       uint64_t *fed_load_id_base,
                       fed_entry *fed_load_entries,
                       uint64_t num_store_entries,
                       uint64_t *fed_store_id_base,
                       fed_entry *fed_store_entries,
                       uint64_t num_bb_to_func_rel_entries,
                       uint64_t num_func_to_bb_rel_entries,
                       __csi_init_rel_tables_t rel_table_init) {
    // TODO(ddoucet): threadsafety
    if (!csi_init_called) {
        // TODO(ddoucet): what to call this with?
        __csi_init("TODO: give the actual name here");
        csi_init_called = true;
    }

    add_fed_table(FED_COLL_FUNCTIONS, num_func_entries, fed_func_entries);
    update_ids(FED_COLL_FUNCTIONS, num_func_entries, fed_func_id_base);

    add_fed_table(FED_COLL_FUNCTION_EXIT, num_func_exit_entries, fed_func_exit_entries);
    update_ids(FED_COLL_FUNCTION_EXIT, num_func_exit_entries, fed_func_exit_id_base);

    add_fed_table(FED_COLL_BASICBLOCK, num_bb_entries, fed_bb_entries);
    update_ids(FED_COLL_BASICBLOCK, num_bb_entries, fed_bb_id_base);

    add_fed_table(FED_COLL_CALLSITE, num_callsite_entries, fed_callsite_entries);
    update_ids(FED_COLL_CALLSITE, num_callsite_entries, fed_callsite_id_base);

    add_fed_table(FED_COLL_LOAD, num_load_entries, fed_load_entries);
    update_ids(FED_COLL_LOAD, num_load_entries, fed_load_id_base);

    add_fed_table(FED_COLL_STORE, num_store_entries, fed_store_entries);
    update_ids(FED_COLL_STORE, num_store_entries, fed_store_id_base);

    realloc_rel_tables(num_bb_to_func_rel_entries, num_func_to_bb_rel_entries);
    rel_table_init(&rel_bb_to_func, &rel_func_to_bb);

    instrumentation_counts_t counts;
    counts.num_bb = num_bb_entries;
    counts.num_callsite = num_callsite_entries;
    counts.num_func = num_func_entries;
    counts.num_func_exit = num_func_exit_entries;
    counts.num_load = num_load_entries;
    counts.num_store = num_store_entries;
    __csi_unit_init(name, counts);
}

// TODO(ddoucet): why does inlining these functions cause a crash?
char *__csi_fed_func_get_filename(const uint64_t func_id) {
    return get_fed_entry(FED_COLL_FUNCTIONS, func_id)->filename;
}

int32_t __csi_fed_func_get_line_number(const uint64_t func_id) {
    return get_fed_entry(FED_COLL_FUNCTIONS, func_id)->line_number;
}

char *__csi_fed_func_exit_get_filename(const uint64_t func_exit_id) {
    return get_fed_entry(FED_COLL_FUNCTION_EXIT, func_exit_id)->filename;
}

int32_t __csi_fed_func_exit_get_line_number(const uint64_t func_exit_id) {
    return get_fed_entry(FED_COLL_FUNCTION_EXIT, func_exit_id)->line_number;
}

char *__csi_fed_bb_get_filename(const uint64_t bb_id) {
    return get_fed_entry(FED_COLL_BASICBLOCK, bb_id)->filename;
}

int32_t __csi_fed_bb_get_line_number(const uint64_t bb_id) {
    return get_fed_entry(FED_COLL_BASICBLOCK, bb_id)->line_number;
}

char *__csi_fed_callsite_get_filename(const uint64_t callsite_id) {
    return get_fed_entry(FED_COLL_CALLSITE, callsite_id)->filename;
}

int32_t __csi_fed_callsite_get_line_number(const uint64_t callsite_id) {
    return get_fed_entry(FED_COLL_CALLSITE, callsite_id)->line_number;
}

char *__csi_fed_load_get_filename(const uint64_t load_id) {
    return get_fed_entry(FED_COLL_LOAD, load_id)->filename;
}

int32_t __csi_fed_load_get_line_number(const uint64_t load_id) {
    return get_fed_entry(FED_COLL_LOAD, load_id)->line_number;
}

char *__csi_fed_store_get_filename(const uint64_t store_id) {
    return get_fed_entry(FED_COLL_STORE, store_id)->filename;
}

int32_t __csi_fed_store_get_line_number(const uint64_t store_id) {
    return get_fed_entry(FED_COLL_STORE, store_id)->line_number;
}

bool __csirt_callsite_target_unknown(uint64_t csi_id, uint64_t func_id) {
    return func_id == 0xffffffffffffffff;
}

uint64_t __csi_rel_bb_to_func(uint64_t bb_id) {
    return rel_bb_to_func.ids[bb_id];
}

range_t __csi_rel_func_to_bb(uint64_t func_id) {
    return rel_func_to_bb.ranges[func_id];
}


EXTERN_C_END
