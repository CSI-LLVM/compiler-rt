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

WEAK void __csi_init(const char * const name);

typedef struct {
  uint64_t num_bb;
  uint64_t num_callsite;
  uint64_t num_func;
  uint64_t num_func_exit;
  uint64_t num_load;
  uint64_t num_store;
} instrumentation_counts_t;

typedef struct {
  uint64_t first_id;
  uint64_t last_id;
} range_t;

WEAK void __csi_unit_init(const char * const file_name,
                          const instrumentation_counts_t counts);

WEAK void __csi_before_load(const uint64_t csi_id,
                            const void *addr,
                            const uint32_t num_bytes,
                            const uint64_t prop);

WEAK void __csi_after_load(const uint64_t csi_id,
                           const void *addr,
                           const uint32_t num_bytes,
                           const uint64_t prop);

WEAK void __csi_before_store(const uint64_t csi_id,
                             const void *addr,
                             const uint32_t num_bytes,
                             const uint64_t prop);

WEAK void __csi_after_store(const uint64_t csi_id,
                            const void *addr,
                            const uint32_t num_bytes,
                            const uint64_t prop);

WEAK void __csi_func_entry(const uint64_t csi_id,
                           const void * const function,
                           const void * const return_addr,
                           const char * const func_name);

WEAK void __csi_func_exit(const uint64_t csi_id,
                          const void * const function,
                          const void * const return_addr,
                          const char * const func_name);

WEAK void __csi_bb_entry(const uint64_t csi_id);

WEAK void __csi_bb_exit(const uint64_t csi_id);

WEAK void __csi_before_callsite(uint64_t csi_id, uint64_t func_id);

EXTERN_C_END

#endif
