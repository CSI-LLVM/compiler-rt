// RUN: %clang_csi_toolc %tooldir/null-tool.c -o %t-null-tool.o
// RUN: %clang_csi_toolc %tooldir/init-hooks-tool.c -o %t-tool.o
// RUN: %link_csi %t-tool.o %t-null-tool.o -o %t-tool.o
// RUN: %clang_csi_c %s -o %t.o
// RUN: %clang_csi %t.o %t-tool.o %csirtlib -o %t
// RUN: %run %t | FileCheck %s

#include <stdio.h>

int main(int argc, char **argv) {
  printf("A.\n");
  printf("B.\n");
  // CHECK: In __csi_init
  // CHECK: Unit init 'init-hooks.c'
  return 0;
}
