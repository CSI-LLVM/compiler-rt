#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <stack>
#include "csi.h"

namespace {

std::set<csi_id_t> *called_functions = nullptr;
std::stack<csi_id_t> *callstack = nullptr;
csi_id_t total_num_functions = 0;
int indent_level = 0;

void report() {
    fprintf(stderr, "\n============= Demo tool report =============\n");
    fprintf(stderr, "Executed %lu/%ld functions.\n", called_functions->size(), total_num_functions);
    for (csi_id_t id = 0; id < total_num_functions; id++) {
        if (called_functions->count(id) == 0) {
            fprintf(stderr, "  Function ID %ld at %s:%d was not executed.\n", id,
                    __csi_fed_get_func(id)->filename,
                    __csi_fed_get_func(id)->line_number);
        }
    }
    fprintf(stderr, "\n");
    delete called_functions;
    delete callstack;
}

void init() {
    called_functions = new std::set<csi_id_t>();
    callstack = new std::stack<csi_id_t>();
    atexit(report);
}

void indent() {
    const char indention[] = "  ";
    for (int i = 0; i < indent_level; i++) {
        fprintf(stderr, indention);
    }
}

}

extern "C" {

void __csi_init() {
    fprintf(stderr, "Initializing the tool.\n");
    init();
}

void __csi_unit_init(const char * const file_name,
                     const instrumentation_counts_t counts) {
    fprintf(stderr, "Initialize unit '%s': %lu basic blocks, %lu callsites, %lu functions, %lu function exits, %lu loads, %lu stores.\n", file_name, counts.num_bb, counts.num_callsite, counts.num_func, counts.num_func_exit, counts.num_load, counts.num_store);
    total_num_functions += counts.num_func;
}

void __csi_before_load(const csi_id_t load_id,
                       const void *addr,
                       const int32_t num_bytes,
                       const uint64_t prop){
    indent();
    fprintf(stderr, "__csi_before_load: address %p, %d bytes (%s:%d)\n",
            addr, num_bytes,
            __csi_fed_get_load(load_id)->filename,
            __csi_fed_get_load(load_id)->line_number);
}

void __csi_after_load(const csi_id_t load_id,
                      const void *addr,
                      const int32_t num_bytes,
                      const uint64_t prop){
    indent();
    fprintf(stderr, "__csi_after_load: address %p, %d bytes (%s:%d)\n",
            addr, num_bytes,
            __csi_fed_get_load(load_id)->filename,
            __csi_fed_get_load(load_id)->line_number);
}

void __csi_before_store(const csi_id_t store_id,
                        const void *addr,
                        const int32_t num_bytes,
                        const uint64_t prop) {
    indent();
    fprintf(stderr, "__csi_before_store: address %p, %d bytes (%s:%d)\n",
            addr, num_bytes,
            __csi_fed_get_store(store_id)->filename,
            __csi_fed_get_store(store_id)->line_number);
}

void __csi_after_store(const csi_id_t store_id,
                       const void *addr,
                       const int32_t num_bytes,
                       const uint64_t prop) {
    indent();
    fprintf(stderr, "__csi_after_store: address %p, %d bytes (%s:%d)\n",
            addr, num_bytes,
            __csi_fed_get_store(store_id)->filename,
            __csi_fed_get_store(store_id)->line_number);
}

void __csi_func_entry(const csi_id_t func_id) {
    indent();
    fprintf(stderr, "__csi_func_entry: Entering function ID %ld (%s:%d)\n",
            func_id,
            __csi_fed_get_func(func_id)->filename,
            __csi_fed_get_func(func_id)->line_number);
    indent_level++;
    called_functions->insert(func_id);
    if (!callstack->empty()) assert(callstack->top() == func_id);
}

void __csi_func_exit(const csi_id_t func_exit_id,
                     const csi_id_t func_id) {
    indent_level--;
    indent();
    fprintf(stderr, "__csi_func_exit: Exited function ID %ld (%s:%d)\n",
            func_id,
            __csi_fed_get_func_exit(func_exit_id)->filename,
            __csi_fed_get_func_exit(func_exit_id)->line_number);
    if (!callstack->empty()) assert(callstack->top() == func_id);
}

void __csi_bb_entry(const csi_id_t bb_id) {
    indent();
    fprintf(stderr, "__csi_bb_entry: %s:%d\n",
            __csi_fed_get_bb(bb_id)->filename,
            __csi_fed_get_bb(bb_id)->line_number);
    indent_level++;
}

void __csi_bb_exit(const csi_id_t bb_id) {
    indent_level--;
    indent();
    fprintf(stderr, "__csi_bb_exit: %s:%d\n",
            __csi_fed_get_bb(bb_id)->filename,
            __csi_fed_get_bb(bb_id)->line_number);
}

void __csi_before_call(const csi_id_t callsite_id, const csi_id_t func_id) {
    indent();
    fprintf(stderr, "__csi_before_call: Calling function ID ");
    if (__csirt_is_callsite_target_unknown(callsite_id, func_id)) {
        fprintf(stderr, "<unknown> ");
    } else {
        callstack->push(func_id);
        fprintf(stderr, "%ld (%s:%d) ", func_id,
                __csi_fed_get_func(func_id)->filename,
                __csi_fed_get_func(func_id)->line_number);
    }
    fprintf(stderr, "from %s:%d\n",
            __csi_fed_get_callsite(callsite_id)->filename,
            __csi_fed_get_callsite(callsite_id)->line_number);
}

void __csi_after_call(const csi_id_t callsite_id, const csi_id_t func_id) {
    indent();
    fprintf(stderr, "__csi_after_call: After call to function ID ");
    if (__csirt_is_callsite_target_unknown(callsite_id, func_id)) {
        fprintf(stderr, "<unknown> ");
    } else {
        callstack->pop();
        fprintf(stderr, "%ld ", func_id);
    }
    fprintf(stderr, "\n");
}

} // extern "C"
