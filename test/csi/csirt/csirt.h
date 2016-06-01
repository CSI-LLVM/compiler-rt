#ifndef __CSIRT_H__
#define __CSIRT_H__

#include "../toolkit/csi.h"

EXTERN_C

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

#endif  // __CSIRT_H__
