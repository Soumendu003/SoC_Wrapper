SRC = $(shell pwd)
PARENT_DIR := $(SRC)/..
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -lpthread

LINKDIR = Util

OBJECTS = $(addsuffix .o, $(basename $(wildcard *.cpp)))

ifeq ($(shell uname),Linux)
	OS = linux
endif
ifeq ($(shell uname | head -c 6),CYGWIN)
	OS = Cygwin
endif

TARGET := SoC-$(OS)

SUB_DIR = $(patsubst %,$(PARENT_DIR)/%,$(LINKDIR))
EXT_OBJECTS := $(patsubst %,$(PARENT_DIR)/%/*o,$(LINKDIR))

default : target

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -I. $(patsubst %,-I$(PARENT_DIR)/%,$(LINKDIR)) $<

$(OBJECTS) : $(HEADERS) $(patsubst %,$(PARENT_DIR)/%/*.h,$(LINKDIR)) 

target: $(OBJECTS) library
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(EXT_OBJECTS) 

clean:
	rm -f $(TARGET) $(OBJECTS)


library:
	for p in $(SUB_DIR) ; do\
		$(MAKE) -C $$p -f Makefile.mk ; \
	done