#ifndef __CSIRT_H__
#define __CSIRT_H__

#include "../toolkit/csi.h"

EXTERN_C

char *__csirt_get_filename(const uint64_t csi_id);
int32_t __csirt_get_line_number(const uint64_t csi_id);

char *__csi_fed_func_get_filename(const uint64_t func_id);
int32_t __csi_fed_func_get_line_number(const uint64_t func_id);

char *__csi_fed_func_exit_get_filename(const uint64_t func_exit_id);
int32_t __csi_fed_func_exit_get_line_number(const uint64_t func_exit_id);

char *__csi_fed_bb_get_filename(const uint64_t bb_id);
int32_t __csi_fed_bb_get_line_number(const uint64_t bb_id);

char *__csi_fed_callsite_get_filename(const uint64_t callsite_id);
int32_t __csi_fed_callsite_get_line_number(const uint64_t callsite_id);

bool __csirt_callsite_target_unknown(uint64_t csi_id, uint64_t func_id);

EXTERN_C_END

#endif  // __CSIRT_H__
