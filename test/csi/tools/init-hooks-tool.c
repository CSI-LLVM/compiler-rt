#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "csi.h"

static const char * const basename(const char * const path) {
    const char * const slash = strrchr(path, '/');
    if (slash == NULL) {
        return path;
    } else {
        return slash + 1;
    }
}

void __csi_init() {
    printf("In __csi_init\n");
}

void __csi_unit_init(const char * const file_name) {
    printf("Unit init '%s'\n", basename(file_name));
}
