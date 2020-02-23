CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -lpthread

SRC = $(shell pwd)
CLONE_DIR = /home/Guddu/Util

OBJECTS = $(addsuffix .o, $(basename $(wildcard *.cpp)))
HEADERS = $(wildcard *.h)

LIB_SOURCES = $(wildcard $(LIB_SRC_DIR)/*.h) $(wildcard $(LIB_SRC_DIR)/*.cpp)

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


default: target

%.o : %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $^

target: $(OBJECTS)
	ld -r -o $(TARGET) *.o

update:
	$(MAKE) -C $(CLONE_DIR) CLONE=$(SRC) -f Makefile.mk update

clean:
	rm -rf $(OBJECTS) $(TARGET)