
LEVEL=..
include $(LEVEL)/makefile.config

CXXFLAGS= -c -O2

ifdef DEBUG
CXXFLAGS=-DDEBUG -c -O0 -g
endif

objects= MemoryAllocator.o tool.o

all: $(objects)
	ar crs $(BIN_PATH)/libmemory.a $(objects)	
		
MemoryAllocator.o: MemoryAllocator.h MemoryAllocator.cpp 
	$(CXX) $(CXXFLAGS) $(GCC_MICRO) MemoryAllocator.cpp -o MemoryAllocator.o

tool.o: tool.h tool.cpp MemoryAllocator.h
	$(CXX) $(CXXFLAGS)  $(GCC_MICRO) tool.cpp -o tool.o
