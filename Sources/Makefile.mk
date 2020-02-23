SRC = $(shell pwd)
PARENT_DIR := $(SRC)/..
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -lpthread

ifeq ($(shell uname),Linux)
	OS = linux
endif
ifeq ($(shell uname | head -c 6),CYGWIN)
	OS = Cygwin
endif

LIB_EXT = so 
ifeq ($(LIB_TYPE),shared)
   TARGET = SoC-$(OS).$(LIB_EXT)
else
   TARGET = SoC-$(OS).a
endif


LINKDIR = Library

OBJECTS = $(addsuffix .o, $(basename $(wildcard *.cpp)))
HEADERS = $(wildcard *.h)

SUB_DIR = $(patsubst %,$(PARENT_DIR)/%,$(LINKDIR))
EXT_OBJECTS := $(patsubst %,$(PARENT_DIR)/%/*o,$(LINKDIR))

default : target

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -I. $(patsubst %,-I$(PARENT_DIR)/%,$(LINKDIR)) $<

$(OBJECTS) : $(HEADERS) $(patsubst %,$(PARENT_DIR)/%/*.h,$(LINKDIR)) 

target: $(OBJECTS)
	ld -r -o $(TARGET) *.o
clean:
	rm -rf $(OBJECTS) $(TARGET)
