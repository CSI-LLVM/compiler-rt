#ifndef __CSI_H__
#define __CSI_H__

#include <stdint.h>

#ifdef __cplusplus
#define EXTERN_C extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C
#define EXTERN_C_END
#include <stdbool.h> // for C99 bool type
#endif

#define WEAK __attribute__((weak))

// API function signatures
EXTERN_C

typedef uint64_t csi_id_t;

typedef struct {
  uint64_t num_bb;
  uint64_t num_callsite;
  uint64_t num_func;
  uint64_t num_func_exit;
  uint64_t num_load;
  uint64_t num_store;
} instrumentation_counts_t;

typedef struct {
  csi_id_t first_id;
  csi_id_t last_id;
} range_t;

WEAK void __csi_init();

WEAK void __csi_unit_init(const char * const file_name,
                          const instrumentation_counts_t counts);

WEAK void __csi_before_load(const csi_id_t load_id,
                            const void *addr,
                            const uint32_t num_bytes,
                            const uint64_t prop);

WEAK void __csi_after_load(const csi_id_t load_id,
                           const void *addr,
                           const uint32_t num_bytes,
                           const uint64_t prop);

WEAK void __csi_before_store(const csi_id_t store_id,
                             const void *addr,
                             const uint32_t num_bytes,
                             const uint64_t prop);

WEAK void __csi_after_store(const csi_id_t store_id,
                            const void *addr,
                            const uint32_t num_bytes,
                            const uint64_t prop);

WEAK void __csi_func_entry(const csi_id_t func_id);

WEAK void __csi_func_exit(const csi_id_t func_exit_id,
                          const csi_id_t func_id);

WEAK void __csi_bb_entry(const csi_id_t bb_id);

WEAK void __csi_bb_exit(const csi_id_t bb_id);

WEAK void __csi_before_callsite(csi_id_t callsite_id, csi_id_t func_id);
WEAK void __csi_after_callsite(csi_id_t callsite_id, csi_id_t func_id);

typedef struct {
    int32_t line_number;
    char *filename;
} source_loc_t;

// Front-end data (FED) table accessors.
source_loc_t __csi_fed_get_func(const csi_id_t func_id);
source_loc_t __csi_fed_get_func_exit(const csi_id_t func_exit_id);
source_loc_t __csi_fed_get_bb(const csi_id_t bb_id);
source_loc_t __csi_fed_get_callsite(const csi_id_t callsite_id);
source_loc_t __csi_fed_get_load(const csi_id_t load_id);
source_loc_t __csi_fed_get_store(const csi_id_t store_id);

bool __csirt_callsite_target_unknown(uint64_t csi_id, uint64_t func_id);

// Relation table accessors.
csi_id_t __csi_rel_bb_to_func(const csi_id_t bb_id);
range_t __csi_rel_func_to_bb(const csi_id_t func_id);

EXTERN_C_END

#endif
