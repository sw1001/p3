#
# Sufficient Lineage makefile
#
# History: Written  by Tim Mattson, August 2010
#          Modified by Tom Deakin and Simon McIntosh-Smith, October 2012
#          Modified by Tom Deakin, July 2013
# 		   Modified by Chenyuan, July 2019
#

ifndef CPPC
	CPPC=g++
endif

CCFLAGS=-O3 -ffast-math -std=c++0x -U__STRICT_ANSI__

LIBS = -lm -lOpenCL -fopenmp

COMMON_DIR = ../../Cpp_common

INC = -I $(COMMON_DIR)

MMUL_OBJS = main.o Load.o DNF.o Suff.o Influ.o Change.o Para.o 
EXEC = query

# Check our platform and make sure we define the APPLE variable
# and set up the right compiler flags and libraries
PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	CPPC = clang++
	CCFLAGS += -stdlib=libc++
	LIBS = -lm -framework OpenCL
endif

all: $(EXEC)

query: $(MMUL_OBJS)
	$(CPPC) $(MMUL_OBJS) $(CCFLAGS) $(LIBS) -o $(EXEC)

.c.o:
	$(CPPC) -c $< $(CCFLAGS) -o $@

.cpp.o:
	$(CPPC) -c $< $(CCFLAGS) $(INC) -o $@

Load.o:  Load.h
 
DNF.o:  DNF.h

Suff.o:  Suff.h Para.h

Influ.o:  Influ.h Suff.h Para.h

Change.o:  Change.h Influ.h Suff.h Para.h

Para.o:  Para.h

main.o:  Load.h DNF.h Suff.h Influ.h Change.h Para.h

clean:
	rm -f $(MMUL_OBJS) $(EXEC)
