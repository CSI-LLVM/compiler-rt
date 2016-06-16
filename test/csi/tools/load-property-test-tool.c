#include <stdlib.h>
#include <stdio.h>
#include "csi.h"

static int num_loads = 0, num_read_before_writes = 0;

void report() {
    printf("num_loads = %d\n", num_loads);
    printf("num_read_before_writes = %d\n", num_read_before_writes);
}

void __csi_init() {
    num_loads = num_read_before_writes = 0;
    atexit(report);
}

void __csi_before_load(const csi_id_t load_id, const void *addr,
                       const int32_t num_bytes, const uint64_t prop) {
    num_loads++;
    if (prop & CSI_PROP_LOAD_READ_BEFORE_WRITE_IN_BB) num_read_before_writes++;
}
