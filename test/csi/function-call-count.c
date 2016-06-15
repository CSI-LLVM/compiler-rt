// RUN: %clang_csi_toolc %tooldir/null-tool.c -o %T/null-tool.o
// RUN: %clang_csi_toolc %tooldir/function-call-count-tool.c -o %T/tool.o
// RUN: %link_csi %T/tool.o %T/null-tool.o -o %T/tool.o
// RUN: %clang_csi_c %s -o %t.o
// RUN: %clang_csi %t.o %T/tool.o %csirtlib -o %t
// RUN: %run %t | FileCheck %s

#include <stdio.h>

int main(int argc, char **argv) {
  printf("One call.\n");
  printf("Two calls.\n");
  // CHECK: num_function_calls = 2
  return 0;
}
