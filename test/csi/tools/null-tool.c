#include "csi.h"

WEAK void __csi_init() {}

WEAK void __csi_unit_init(const char * const file_name,
                          const instrumentation_counts_t counts) {}

WEAK void __csi_before_load(const csi_id_t load_id,
                            const void *addr,
                            const int32_t num_bytes,
                            const csi_prop_t prop) {}

WEAK void __csi_after_load(const csi_id_t load_id,
                           const void *addr,
                           const int32_t num_bytes,
                           const csi_prop_t prop) {}

WEAK void __csi_before_store(const csi_id_t store_id,
                             const void *addr,
                             const int32_t num_bytes,
                             const csi_prop_t prop) {}

WEAK void __csi_after_store(const csi_id_t store_id,
                            const void *addr,
                            const int32_t num_bytes,
                            const csi_prop_t prop) {}

WEAK void __csi_func_entry(const csi_id_t func_id, const csi_prop_t prop) {}

WEAK void __csi_func_exit(const csi_id_t func_exit_id,
                          const csi_id_t func_id, const csi_prop_t prop) {}

WEAK void __csi_bb_entry(const csi_id_t bb_id, const csi_prop_t prop) {}

WEAK void __csi_bb_exit(const csi_id_t bb_id, const csi_prop_t prop) {}

WEAK void __csi_before_call(csi_id_t callsite_id, csi_id_t func_id,
                            const csi_prop_t prop) {}

WEAK void __csi_after_call(csi_id_t callsite_id, csi_id_t func_id,
                           const csi_prop_t prop) {}
