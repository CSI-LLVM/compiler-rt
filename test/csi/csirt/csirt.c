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

static const int DEFAULT_FED_TABLE_LENGTH = 128;
static bool csi_init_called = false;

// The sum of the FED size for each unit initialized. When a unit is
// initialized, we update their fed_id_base to the current value of
// total_fed_size, then add their FED size to total_fed_size, giving
// each tix a globally unique set of csi_id values.
static uint64_t total_fed_size = 0;

static uint64_t num_fed_tables = 0, fed_table_pointers_capacity = 0;
static fed_entry **fed_table_pointers = NULL;
static uint64_t *list_size = NULL;

static inline void add_fed_table(uint64_t num_entries, fed_entry *fed_entries) {
    if (num_fed_tables == fed_table_pointers_capacity) {
        if (fed_table_pointers)
            fed_table_pointers_capacity *= 2;
        else
            fed_table_pointers_capacity = DEFAULT_FED_TABLE_LENGTH;
    }

    fed_table_pointers = (fed_entry **)realloc(fed_table_pointers,
                                               fed_table_pointers_capacity * sizeof(fed_entry *));
    list_size = (uint64_t *)realloc(list_size,
                                    fed_table_pointers_capacity * sizeof(uint64_t));

    fed_table_pointers[num_fed_tables] = fed_entries;
    list_size[num_fed_tables] = num_entries;

    num_fed_tables++;
}

static inline void update_ids(uint64_t num_entries, uint64_t *fed_id_base) {
    *fed_id_base = total_fed_size;
    total_fed_size += num_entries;
}

static inline fed_entry *get_fed_entry(uint64_t csi_id) {
    // TODO(ddoucet): threadsafety
    uint64_t sum = 0;
    for (uint64_t i = 0; i < num_fed_tables; i++) {
        if (csi_id < sum + list_size[i])
            return &fed_table_pointers[i][csi_id - sum];
        sum += list_size[i];
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
