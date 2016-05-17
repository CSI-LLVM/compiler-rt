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
typedef enum { FED_COLL_GLOBAL, FED_COLL_FUNCTIONS, NUM_FED_COLLS } fed_collection_type;

static fed_collection *fed_collections = NULL;
static bool fed_collections_initialized = false;

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

EXTERN_C

void __csirt_unit_init(const char * const name,
                       uint64_t num_entries,
                       uint64_t *fed_id_base,
                       fed_entry *fed_entries,
                       uint64_t num_func_entries,
                       uint64_t *fed_func_id_base,
                       fed_entry *fed_func_entries) {
    // TODO(ddoucet): threadsafety
    if (!csi_init_called) {
        // TODO(ddoucet): what to call this with?
        __csi_init("TODO: give the actual name here");
        csi_init_called = true;
    }

    add_fed_table(FED_COLL_GLOBAL, num_entries, fed_entries);
    update_ids(FED_COLL_GLOBAL, num_entries, fed_id_base);

    add_fed_table(FED_COLL_FUNCTIONS, num_func_entries, fed_func_entries);
    update_ids(FED_COLL_FUNCTIONS, num_func_entries, fed_func_id_base);

    __csi_unit_init(name, num_entries);
}

// TODO(ddoucet): why does inlining these functions cause a crash?
char *__csirt_get_filename(const uint64_t csi_id) {
    return get_fed_entry(FED_COLL_GLOBAL, csi_id)->filename;
}

int32_t __csirt_get_line_number(const uint64_t csi_id) {
    return get_fed_entry(FED_COLL_GLOBAL, csi_id)->line_number;
}

char *__csi_fed_func_get_filename(const uint64_t func_id) {
    return get_fed_entry(FED_COLL_FUNCTIONS, func_id)->filename;
}

int32_t __csi_fed_func_get_line_number(const uint64_t func_id) {
    return get_fed_entry(FED_COLL_FUNCTIONS, func_id)->line_number;
}

bool __csirt_callsite_target_unknown(uint64_t csi_id, uint64_t func_id) {
    return func_id == 0xffffffffffffffff;
}

EXTERN_C_END
