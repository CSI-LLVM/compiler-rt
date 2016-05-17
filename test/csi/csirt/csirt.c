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

typedef struct {
    uint64_t num_entries;
    fed_entry *entries;
} fed_table;

typedef struct {
    uint64_t total_num_entries;
    uint64_t num_fed_tables;
    uint64_t capacity;
    fed_table *tables;
} fed_table_collection;

static fed_table_collection fed_collection;
static bool fed_collection_initialized = false;

static inline void add_fed_table(uint64_t num_entries, fed_entry *fed_entries) {
    bool need_realloc = false;
    if (!fed_collection_initialized || fed_collection.num_fed_tables == fed_collection.capacity) {
        if (fed_collection.tables) {
            fed_collection.capacity *= 2;
        } else {
            fed_collection.capacity = DEFAULT_NUM_FED_TABLES;
        }
        need_realloc = true;
        fed_collection_initialized = true;
    }
    if (need_realloc) {
        fed_collection.tables = (fed_table *)realloc(fed_collection.tables,
                                                     fed_collection.capacity * sizeof(fed_table *));
    }
    fed_table new_table;
    new_table.num_entries = num_entries;
    new_table.entries = fed_entries;
    fed_collection.tables[fed_collection.num_fed_tables++] = new_table;
}

static inline void update_ids(uint64_t num_entries, uint64_t *fed_id_base) {
    *fed_id_base = fed_collection.total_num_entries;
    fed_collection.total_num_entries += num_entries;
}

static inline fed_entry *get_fed_entry(uint64_t csi_id) {
    // TODO(ddoucet): threadsafety
    uint64_t sum = 0;
    for (uint64_t i = 0; i < fed_collection.num_fed_tables; i++) {
        fed_table *table = &fed_collection.tables[i];
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
                       fed_entry *fed_entries) {
    // TODO(ddoucet): threadsafety
    if (!csi_init_called) {
        // TODO(ddoucet): what to call this with?
        __csi_init("TODO: give the actual name here");
        csi_init_called = true;
    }

    add_fed_table(num_entries, fed_entries);
    update_ids(num_entries, fed_id_base);
    __csi_unit_init(name, num_entries);
}

// TODO(ddoucet): why does inlining these functions cause a crash?
char *__csirt_get_filename(const uint64_t csi_id) {
    return get_fed_entry(csi_id)->filename;
}

int32_t __csirt_get_line_number(const uint64_t csi_id) {
    return get_fed_entry(csi_id)->line_number;
}

bool __csirt_callsite_target_unknown(uint64_t csi_id, uint64_t func_id) {
    return func_id == 0xffffffffffffffff;
}

EXTERN_C_END
