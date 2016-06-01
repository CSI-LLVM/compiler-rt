#include "csi.h"
#include "../csirt/csirt.h"
#include <stdio.h>

static inline void print_call(const char *const msg,
                              source_loc_t source_loc) {
    printf("%s:%d -- %s\n",
           source_loc.filename,
           source_loc.line_number,
           msg);
}

void __csi_init() {
    printf("__csi_init\n");
}

void __csi_unit_init(const char *const file_name,
                     const uint64_t num_inst) {
    printf("__csi_unit_init: %s, %lu inst\n", file_name, num_inst);
}

void __csi_before_load(const csi_id_t load_id,
                       const void *addr,
                       const uint32_t num_bytes,
                       const uint64_t prop) {
    print_call("__csi_before_load", __csi_fed_get_load(load_id));
}

void __csi_after_load(const csi_id_t load_id,
                      const void *addr,
                      const uint32_t num_bytes,
                      const uint64_t prop) {
    print_call("__csi_after_load", __csi_fed_get_load(load_id));
}

void __csi_before_store(const csi_id_t store_id,
                        const void *addr,
                        const uint32_t num_bytes,
                        const uint64_t prop) {
    print_call("__csi_before_store", __csi_fed_get_store(store_id));
}

void __csi_after_store(const csi_id_t store_id,
                       const void *addr,
                       const uint32_t num_bytes,
                       const uint64_t prop) {
    print_call("__csi_after_store", __csi_fed_get_store(store_id));
}

void __csi_func_entry(const csi_id_t func_id) {
    print_call("__csi_func_entry", __csi_fed_get_func(func_id));
}

void __csi_func_exit(const csi_id_t func_exit_id,
                     const csi_id_t func_id) {
    print_call("__csi_func_exit", __csi_fed_get_func_exit(func_exit_id));
}

void __csi_bb_entry(const csi_id_t bb_id) {
    print_call("__csi_bb_entry", __csi_fed_get_bb(bb_id));
}

void __csi_bb_exit(const csi_id_t bb_id) {
    print_call("__csi_bb_exit", __csi_fed_get_bb(bb_id));
}

void __csi_before_callsite(const csi_id_t callsite_id, const csi_id_t func_id) {
    print_call("__csi_before_callsite", __csi_fed_get_callsite(callsite_id));
    if (__csirt_callsite_target_unknown(callsite_id, func_id)) {
        printf("  (unknown target)\n");
    } else {
        source_loc_t func = __csi_fed_get_func(func_id);
        printf("  target: %s:%d\n", func.filename, func.line_number);
    }
}

void __csi_after_callsite(const csi_id_t callsite_id, const csi_id_t func_id) {
    print_call("__csi_after_callsite", __csi_fed_get_callsite(callsite_id));
    if (__csirt_callsite_target_unknown(callsite_id, func_id)) {
        printf("  (unknown target)\n");
    } else {
        source_loc_t func = __csi_fed_get_func(func_id);
        printf("  target: %s:%d\n", func.filename, func.line_number);
    }
}
