#ifndef __CSIRT_H__
#define __CSIRT_H__

#include "../toolkit/csi.h"

EXTERN_C

// Front-end data (FED) table accessors.
char *__csi_fed_func_get_filename(const uint64_t func_id);
int32_t __csi_fed_func_get_line_number(const uint64_t func_id);

char *__csi_fed_func_exit_get_filename(const uint64_t func_exit_id);
int32_t __csi_fed_func_exit_get_line_number(const uint64_t func_exit_id);

char *__csi_fed_bb_get_filename(const uint64_t bb_id);
int32_t __csi_fed_bb_get_line_number(const uint64_t bb_id);

char *__csi_fed_callsite_get_filename(const uint64_t callsite_id);
int32_t __csi_fed_callsite_get_line_number(const uint64_t callsite_id);

char *__csi_fed_load_get_filename(const uint64_t load_id);
int32_t __csi_fed_load_get_line_number(const uint64_t load_id);

char *__csi_fed_store_get_filename(const uint64_t store_id);
int32_t __csi_fed_store_get_line_number(const uint64_t store_id);

bool __csirt_callsite_target_unknown(uint64_t csi_id, uint64_t func_id);

// Relation table accessors.
uint64_t __csi_rel_bb_to_func(uint64_t bb_id);
range_t __csi_rel_func_to_bb(uint64_t func_id);

EXTERN_C_END

#endif  // __CSIRT_H__
