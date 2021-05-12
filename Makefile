# Basic Makefile

### Compilers
CC  = gcc
CXX = g++

### Machine name
NAME = $(shell uname -n)

DEBUG_LEVEL    = -g
EXTRA_CCFLAGS  = -std=c++11 -Wall
CPPFLAGS       = $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)
CCFLAGS        = $(CPPFLAGS)

RM = rm -f
MV = mv

### ROOT
ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS   := $(shell root-config --libs)

### RAT
RATLIBS  := -L$(RATROOT)/lib -lRATEvent

### BOOST
ifeq ($(NAME), 'nino')
BOOSTCFLAGS := -I/data/snoplus/home/zsoldos/.local/boost-1.71.0
BOOSTLIBS   := -L/data/snoplus/home/zsoldos/.local/boost-1.71.0/lib -lboost_system -lboost_filesystem
else
BOOSTCFLAGS := -I/usr/include/boost
BOOSTLIBS   := -lboost_system -lboost_filesystem
endif

### #### ###
CPPFLAGS  += -Iinclude $(ROOTCFLAGS) -I$(RATROOT)/include
CPPFLAGS  +=  $(BOOSTCFLAGS)

EXTRALIBS  = $(ROOTLIBS)
EXTRALIBS += $(RATLIBS)
EXTRALIBS += $(BOOSTLIBS)

.PHONY: all clean 
.DEFAULT_GOAL = ReadFile

help:
	@grep -h -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'

all: ReadFile

ReadFile:
	$(CXX) $(CPPFLAGS) ReadFile.cc -o $@ $(EXTRALIBS)

ReadThread:
	$(CXX) $(CPPFLAGS) ReadThread.cc -o $@ $(EXTRALIBS)

clean:
	$(RM) ReadFile ReadThread
