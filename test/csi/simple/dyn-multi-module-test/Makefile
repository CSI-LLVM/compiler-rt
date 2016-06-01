TOOLKIT = ../../toolkit
CSIRT = ../../csirt
CSIRT_OBJ = $(CSIRT)/csirt.o

# Toolkit rules go before including 'common.mk' so that they are
# preferentially matched.
$(TOOLKIT)/%.o: $(TOOLKIT)/%.cpp
	make -C $(TOOLKIT) $*

# By default link with the null tool.  To use another tool, do
# e.g. 'make foo TOOL=print_tool'
TOOL ?= null_tool
TOOL_OBJ = $(patsubst %,$(TOOLKIT)/%.o,$(TOOL))

#COV_FLAGS = -fprofile-instr-generate -fcoverage-mapping
COV_FLAGS=

include ../../common.mk

$(CSIRT_OBJ):
	make -C $(CSIRT)

OBJECTS=a.o b.o libtest.so

default: main

main: main.o $(CSIRT_OBJ) $(OBJECTS) $(TOOL_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -fcsi -flto $(COV_FLAGS) $^ -o $@ -L. -ltest

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(EXTRA) $(COV_FLAGS) -fcsi -emit-llvm $^ -o $@

lib.o: lib.cpp
	$(CXX) $(CXXFLAGS) -c $(EXTRA) -fPIC $(COV_FLAGS) -fcsi -emit-llvm $^ -o $@

libtest.so: lib.o $(TOOL_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(EXTRA) -Wl,-soname,libtest.so -fcsi -flto -shared -o $@ $^

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $(EXTRA) -fcsi -emit-llvm $(COV_FLAGS) $^ -o $@

clean:
	-rm -f main *.o *.s *.ll *.bc *.so
	make -C $(TOOLKIT) clean
	make -C $(CSIRT) clean
