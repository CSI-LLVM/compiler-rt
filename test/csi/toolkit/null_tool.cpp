#include "csi.h"

EXTERN_C

WEAK void __csi_init() {}

WEAK void __csi_unit_init(const char * const file_name,
                          const instrumentation_counts_t counts) {}

WEAK void __csi_before_load(const csi_id_t load_id,
                            const void *addr,
                            const uint32_t num_bytes,
                            const uint64_t prop) {}

WEAK void __csi_after_load(const csi_id_t load_id,
                           const void *addr,
                           const uint32_t num_bytes,
                           const uint64_t prop) {}

WEAK void __csi_before_store(const csi_id_t store_id,
                             const void *addr,
                             const uint32_t num_bytes,
                             const uint64_t prop) {}

WEAK void __csi_after_store(const csi_id_t store_id,
                            const void *addr,
                            const uint32_t num_bytes,
                            const uint64_t prop) {}

WEAK void __csi_func_entry(const csi_id_t func_id) {}

WEAK void __csi_func_exit(const csi_id_t func_exit_id,
                          const csi_id_t func_id) {}

WEAK void __csi_bb_entry(const csi_id_t bb_id) {}

WEAK void __csi_bb_exit(const csi_id_t bb_id) {}

WEAK void __csi_before_callsite(csi_id_t callsite_id, csi_id_t func_id) {}
WEAK void __csi_after_callsite(csi_id_t callsite_id, csi_id_t func_id) {}

EXTERN_C_END
