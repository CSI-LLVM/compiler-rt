// RUN: %clang_csi_toolc %tooldir/null-tool.c -o %T/null-tool.o
// RUN: %clang_csi_toolc %tooldir/function-call-count-tool.c -o %T/tool.o
// RUN: %link_csi %T/tool.o %T/null-tool.o -o %T/tool.o
// RUN: %clang_csi_c %s -o %t.o
// RUN: %clang_csi_c %supportdir/a.c -o %t.a.o
// RUN: %clang_csi_c %supportdir/b.c -o %t.b.o
// RUN: %clang_csi %t.o %t.a.o %t.b.o %T/tool.o %csirtlib -o %t
// RUN: %run %t | FileCheck %s

#include <stdio.h>

#include "support/a.h"

int main(int argc, char **argv) {
  printf("One call.\n");
  printf("Two calls.\n");
  a();
  // Calls are: main + a + b + one printf each.
  // CHECK: num_function_calls = 6
  return 0;
}
