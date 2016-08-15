#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "csi.h"

static int num_function_calls = 0;
static std::vector<int> *global_vector = NULL;

void report() {
    printf("num_function_calls = %d\n", num_function_calls);
    if (global_vector) delete global_vector;
}

extern "C" {

void __csi_init() {
    num_function_calls = 0;
    global_vector = new std::vector<int>();
    atexit(report);
}

void __csi_before_call(const csi_id_t call_id, const csi_id_t func_id,
                       const uint64_t prop) {
    if (__csi_disable_instrumentation) return;
    __csi_disable_instrumentation = true;

    global_vector->push_back(call_id);
    num_function_calls++;

    __csi_disable_instrumentation = false;
}

}
