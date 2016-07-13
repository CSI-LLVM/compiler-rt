#include "csi.h"

#define CSIRT_API __attribute__((visibility("default")))

// ------------------------------------------------------------------------
// Globals
// ------------------------------------------------------------------------

// Initially false, set to true once the first unit is initialized,
// which results in the __csi_init() function being called.
static bool csi_init_called = false;

// ------------------------------------------------------------------------
// External function definitions, including CSIRT API functions.
// ------------------------------------------------------------------------

EXTERN_C

// A call to this is inserted by the CSI compiler pass, and occurs
// before main().
CSIRT_API void __csirt_unit_init(const char * const name) {
    // TODO: threadsafety?
    if (!csi_init_called) {
        __csi_init();
        csi_init_called = true;
    }

    // Call into the tool implementation.
    __csi_unit_init(name);
}

EXTERN_C_END
