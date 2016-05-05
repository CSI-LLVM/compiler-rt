include ../common.mk
CC=$(COMPILER_HOME)/bin/clang
#LINK= $(COMPILER_HOME)/bin/llvm-link

default: csirt.o

csirt.o: csirt.c
	$(CC) $(CFLAGS) -c -g -O3 $< -o $@

clean:
	rm -f csirt.o
