#include "csi.h"
#include <stdio.h>
#include <stdlib.h>

namespace {

void destroy() {
    fprintf(stderr, "Destroy tool\n");
}

}

extern "C" {

void __csi_init(const char * const name) {
    fprintf(stderr, "Initialize tool, name=%s\n", name);
    atexit(destroy);
}

void __csi_unit_init(const char * const file_name,
                     const instrumentation_counts_t counts) {
    fprintf(stderr, "Initialize unit id %s, %lu basic blocks.\n", file_name, counts.num_bb);
}

void __csi_before_load(const uint64_t csi_id,
                       const void *addr,
                       const uint32_t num_bytes,
                       const uint64_t prop) {
    fprintf(stderr, "Before load %lu %p (%d bytes) prop=%lu\n",
            csi_id, addr, num_bytes, prop);
}

void __csi_after_load(const uint64_t csi_id,
                      const void *addr,
                      const uint32_t num_bytes,
                      const uint64_t prop) {
    fprintf(stderr, "After load %lu %p (%d bytes) prop=%lu\n",
            csi_id, addr, num_bytes, prop);
}

void __csi_before_store(const uint64_t csi_id,
                        const void *addr,
                        const uint32_t num_bytes,
                        const uint64_t prop) {
    fprintf(stderr, "Before store %lu %p (%d bytes) prop=%lu\n",
            csi_id, addr, num_bytes, prop);
}

void __csi_after_store(const uint64_t csi_id,
                       const void *addr,
                       const uint32_t num_bytes,
                       const uint64_t prop) {
    fprintf(stderr, "After store %lu %p (%d bytes) prop=%lu\n",
            csi_id, addr, num_bytes, prop);
}

void __csi_func_entry(const uint64_t csi_id,
                      const void * const function,
                      const void * const return_addr,
                      const char * const func_name) {
    fprintf(stderr, "Func entry, %lu %p %p %s\n",
            csi_id, function, return_addr, func_name);
}

void __csi_func_exit(const uint64_t csi_id,
                     const void * const function,
                     const void * const return_addr,
                     const char * const func_name) {
    fprintf(stderr, "Func exit %lu %p %p %s\n",
            csi_id, function, return_addr, func_name);
}

void __csi_bb_entry(const uint64_t csi_id) {
    fprintf(stderr, "Basic block entry %lu\n", csi_id);
}

void __csi_bb_exit(const uint64_t csi_id) {
    fprintf(stderr, "Basic block exit %lu\n", csi_id);
}

void __csi_before_callsite(uint64_t csi_id, uint64_t func_id) {
    fprintf(stderr, "Before callsite %lu to %lu\n", csi_id, func_id);
}


} // extern "C"

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
