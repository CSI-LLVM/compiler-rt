// RUN: %clang_csi_toolc %tooldir/null-tool.c -o %T/null-tool.o
// RUN: %clang_csi_toolc %tooldir/function-call-count-tool.c -o %T/tool.o
// RUN: %link_csi %T/null-tool.o %T/tool.o -o %T/tool.o
// RUN: %clang_csi_c %s -o %t.o
// RUN: %clang_csi %t.o %T/tool.o %csirtlib -o %t
// RUN: %run %t

#include <stdio.h>

int main(int argc, char **argv) {
  printf("Hello world.\n");
  return 0;
}
