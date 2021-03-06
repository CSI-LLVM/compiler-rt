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

/**
 * Unless a type requires bitwise operations (e.g., property lists), we use
 * signed integers. We don't need the extra bit of data, and using unsigned
 * integers can lead to subtle bugs. See
 * http://www.soundsoftware.ac.uk/c-pitfall-unsigned
 */

typedef int64_t csi_id_t;

#define UNKNOWN_CSI_ID ((csi_id_t)-1)

typedef struct {
  csi_id_t num_bb;
  csi_id_t num_callsite;
  csi_id_t num_func;
  csi_id_t num_func_exit;
  csi_id_t num_load;
  csi_id_t num_store;
} instrumentation_counts_t;

// Property bitfields.

typedef struct {
  // Pad struct to 64 total bits.
  uint64_t _padding : 64;
} func_prop_t;

typedef struct {
  // Pad struct to 64 total bits.
  uint64_t _padding : 64;
} func_exit_prop_t;

typedef struct {
  // Pad struct to 64 total bits.
  uint64_t _padding : 64;
} bb_prop_t;

typedef struct {
  // The call is indirect.
  unsigned is_indirect : 1;
  // Pad struct to 64 total bits.
  uint64_t _padding : 63;
} call_prop_t;

typedef struct {
  // The alignment of the load.
  unsigned alignment : 8;
  // The loaded address is in a vtable.
  unsigned is_vtable_access : 1;
  // The loaded address points to constant data.
  unsigned is_constant : 1;
  // The loaded address is on the stack.
  unsigned is_on_stack : 1;
  // The loaded address cannot be captured.
  unsigned may_be_captured : 1;
  // The loaded address is read before it is written in the same basic block.
  unsigned is_read_before_write_in_bb : 1;
  // Pad struct to 64 total bits.
  uint64_t _padding : 51;
} load_prop_t;

typedef struct {
  // The alignment of the store.
  unsigned alignment : 8;
  // The stored address is in a vtable.
  unsigned is_vtable_access : 1;
  // The stored address points to constant data.
  unsigned is_constant : 1;
  // The stored address is on the stack.
  unsigned is_on_stack : 1;
  // The stored address cannot be captured.
  unsigned may_be_captured : 1;
  // Pad struct to 64 total bits.
  uint64_t _padding : 52;
} store_prop_t;

WEAK void __csi_init();

WEAK void __csi_unit_init(const char * const file_name,
                          const instrumentation_counts_t counts);

WEAK void __csi_func_entry(const csi_id_t func_id, const func_prop_t prop);

WEAK void __csi_func_exit(const csi_id_t func_exit_id,
                          const csi_id_t func_id, const func_exit_prop_t prop);

WEAK void __csi_bb_entry(const csi_id_t bb_id, const bb_prop_t prop);

WEAK void __csi_bb_exit(const csi_id_t bb_id, const bb_prop_t prop);

WEAK void __csi_before_call(const csi_id_t call_id, const csi_id_t func_id,
                            const call_prop_t prop);

WEAK void __csi_after_call(const csi_id_t call_id, const csi_id_t func_id,
                           const call_prop_t prop);

WEAK void __csi_before_load(const csi_id_t load_id,
                            const void *addr,
                            const int32_t num_bytes,
                            const load_prop_t prop);

WEAK void __csi_after_load(const csi_id_t load_id,
                           const void *addr,
                           const int32_t num_bytes,
                           const load_prop_t prop);

WEAK void __csi_before_store(const csi_id_t store_id,
                             const void *addr,
                             const int32_t num_bytes,
                             const store_prop_t prop);

WEAK void __csi_after_store(const csi_id_t store_id,
                            const void *addr,
                            const int32_t num_bytes,
                            const store_prop_t prop);

// This struct is mirrored in ComprehensiveStaticInstrumentation.cpp,
// FrontEndDataTable::getSourceLocStructType.
typedef struct {
    char *name;
    // TODO(ddoucet): Why is this 32 bits?
    int32_t line_number;
    char *filename;
} source_loc_t;

// Front-end data (FED) table accessors.
source_loc_t const * __csi_get_func_source_loc(const csi_id_t func_id);
source_loc_t const * __csi_get_func_exit_source_loc(const csi_id_t func_exit_id);
source_loc_t const * __csi_get_bb_source_loc(const csi_id_t bb_id);
source_loc_t const * __csi_get_callsite_source_loc(const csi_id_t call_id);
source_loc_t const * __csi_get_load_source_loc(const csi_id_t load_id);
source_loc_t const * __csi_get_store_source_loc(const csi_id_t store_id);

// Load property:
#define CSI_PROP_LOAD_READ_BEFORE_WRITE_IN_BB 0x1

EXTERN_C_END

#endif
