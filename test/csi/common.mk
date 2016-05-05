# -*- mode: makefile-gmake; -*-

# Create your own myconfig.mk in the smae directory that contains this file 
# to include COMPILER_HOME
TEST_MK_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(TEST_MK_DIR)/myconfig.mk

BASIC_CFLAGS = -g -O0 -W -Wall -Werror -Wno-unused-parameter
BASIC_CXXFLAGS=$(BASIC_CFLAGS)
CFLAGS = $(BASIC_CFLAGS) -std=c11 # -flto
CXXFLAGS = $(BASIC_CXXFLAGS) -std=c++11 # -flto

LDFLAGS += -fuse-ld=gold
LDLIBS = -lrt -ldl -lpthread # -flto

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++


.PHONY : default clean

default : $(TARGETS)

# Each C source file will have a corresponding file of prerequisites.
# Include the prerequisites for each of our C source files.
-include $(OBJ:.o=.d)

# This rule generates a file of prerequisites (i.e., a makefile)
# called name.d from a C source file name.c.
%.d : CFLAGS += -MM -MP
%.d : %.c
	@set -e; rm -f $@; \
	$(CC) $(CFLAGS) -MF $@.$$$$ $<; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# This rule generates a file of prerequisites (i.e., a makefile)
# called name.d from a CPP source file name.cpp.
%.d : CXXFLAGS += -MM -MP
%.d : %.cpp
	@set -e; rm -f $@; \
	$(CXX) $(CXXFLAGS) -MF $@.$$$$ $<; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $<
