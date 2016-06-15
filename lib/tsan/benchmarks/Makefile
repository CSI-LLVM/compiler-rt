.SUFFIXES:

TARGETS := mini_bench_local mini_bench_shared start_many_threads mini_bench_increments

default: $(TARGETS)

TSAN_DIR=..

INSTRUMENTATION ?= none

CC=$(COMPILER_HOME)/bin/clang
CXX=$(COMPILER_HOME)/bin/clang++

CFLAGS_Instrumentation_none =
CFLAGS_Instrumentation_tsan = -fsanitize=thread

CFLAGS += ${CFLAGS_Instrumentation_${INSTRUMENTATION}}
CFLAGS += -O3 -emit-llvm
LDFLAGS += -B/usr/lib/gold-ld -O3 -flto -ldl -lpthread -lrt -lm -Wl,--whole-archive $(TSAN_DIR)/rtl/libtsan.a -Wl,--no-whole-archive

TOOL=

%.o : %.cc
	$(CXX) -c $(CFLAGS) $^ -o obj/$@

% : %.o
	$(CXX) $(LDFLAGS) obj/$^ $(TOOL) -o $@

.PHONY : clean
clean:
	-rm -f $(TARGETS) *.o
