#include "csi.h"

extern "C" {


WEAK void __csi_init(const char * const name) {}
WEAK void __csi_unit_init(const char * const file_name,
                          const instrumentation_counts_t counts) {}

WEAK void __csi_before_load(const uint64_t csi_id, const void *addr, const uint32_t num_bytes, const uint64_t prop) {}
WEAK void __csi_after_load(const uint64_t csi_id, const void *addr, const uint32_t num_bytes, const uint64_t prop) {}
WEAK void __csi_before_store(const uint64_t csi_id, const void *addr, const uint32_t num_bytes, const uint64_t prop) {}
WEAK void __csi_after_store(const uint64_t csi_id, const void *addr, const uint32_t num_bytes, const uint64_t prop) {}

WEAK void __csi_func_entry(const uint64_t csi_id,
                           const void * const function,
                           const void * const return_addr,
                           const char * const func_name) {}
WEAK void __csi_func_exit(const uint64_t csi_id,
                          const void * const function,
                          const void * const return_addr,
                          const char * const func_name) {}

WEAK void __csi_bb_entry(const uint64_t csi_id) {}
WEAK void __csi_bb_exit(const uint64_t csi_id) {}

WEAK void __csi_before_callsite(uint64_t csi_id, uint64_t func_id) {}

/* // WEAK void __csi_init(csi_info_t info) {}
// WEAK void __csi_module_init(csi_module_info_t info) {}
WEAK void __csi_init(uint32_t num_modules) {}
WEAK void __csi_unit_init(uint32_t module_id, uint64_t num_basic_blocks) {}
// WEAK void __csi_before_load(void *addr, int num_bytes, csi_acc_prop_t prop) {}
// WEAK void __csi_after_load(void *addr, int num_bytes, csi_acc_prop_t prop) {}
// WEAK void __csi_before_store(void *addr, int num_bytes, csi_acc_prop_t prop) {}
// WEAK void __csi_after_store(void *addr, int num_bytes, csi_acc_prop_t prop) {}
WEAK void __csi_before_load(void *addr, int num_bytes, unsigned unused, bool unused2, bool unused3, bool read_before_write_in_bb) {}
WEAK void __csi_after_load(void *addr, int num_bytes, unsigned unused, bool unused2, bool unused3, bool read_before_write_in_bb) {}
WEAK void __csi_before_store(void *addr, int num_bytes, unsigned unused, bool unused2, bool unused3, bool read_before_write_in_bb) {}
WEAK void __csi_after_store(void *addr, int num_bytes, unsigned unused, bool unused2, bool unused3, bool read_before_write_in_bb) {}

WEAK void __csi_func_entry(void *function, void *parentReturnAddr, char *funcName) {}
WEAK void __csi_func_exit() {}
// WEAK void __csi_bb_entry(csi_id_t id) {}
WEAK void __csi_bb_entry(uint32_t module_id, uint64_t id) {}
WEAK void __csi_bb_exit() {} */

} // extern "C"
