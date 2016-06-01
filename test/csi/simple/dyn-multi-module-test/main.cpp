#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "a.h"
#include "b.h"
#include "lib.h"

/*
 * This file defines:
 *      main(), c() and d()
 * This file directly calls:
 *      a(), b(), c(), lib() and sometimes d()
 *
 * Some of those functions call each other. So the entire call sequence is:
 *
 *      main -> a -> b -> foo
 *                     -> lib
 *      main -> b -> foo
 *                -> lib
 *      main -> c
 *      main -> lib
 *      main -> d (sometimes)
 */

static int c() {
    printf("In c.\n");
    return 3;
}

static int d() {
    printf("In d.\n");
    return 4;
}

int main() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srandom(tv.tv_sec + tv.tv_usec/1000000);
    long r = random();
    int result = 0;

    printf("In main.\n");
    result += a() + b() + c() + lib();
    if (r < RAND_MAX/2) {
        result += d();
    }

    printf("Result was %d\n", result);
    return 0;
}
