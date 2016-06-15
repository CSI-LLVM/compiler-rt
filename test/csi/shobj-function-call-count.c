// RUN: %clang_csi_toolc %tooldir/null-tool.c -o %T/null-tool.o
// RUN: %clang_csi_toolc %tooldir/function-call-count-tool.c -o %T/tool.o
// RUN: %link_csi %T/tool.o %T/null-tool.o -o %T/tool.o
// RUN: %clang_csi_c -fPIC %supportdir/libtest.c -o %T/libtest.o
// RUN: %clang_csi -Wl,-soname,libtest.so -shared %T/libtest.o %T/tool.o -o %T/libtest.so
// RUN: %clang_csi_c %s -o %t.o
// RUN: %clang_csi -Wl,-rpath,%T -L %T %t.o %T/tool.o -ltest %csirtlib -o %t
// RUN: %run %t | FileCheck %s

#include <stdio.h>
#include "support/libtest.h"

int main(int argc, char **argv) {
  printf("One call.\n");
  printf("Two calls.\n");
  libtest();
  // CHECK: num_function_calls = 4
  return 0;
}
