
LEVEL=..
include $(LEVEL)/makefile.config

CXXFLAGS= -O2 -g
ifdef DEBUG
CXXFLAGS=-DDEBUG -O0 -g
endif

LDFLAGS=-L$(BIN_PATH) -lmemory

optiAllocator: optiAllocator.cpp $(BIN_PATH)/libmemory.a
	$(CXX) $(CXXFLAGS) optiAllocator.cpp -o optiAllocator $(LDFLAGS)
	cp optiAllocator $(BIN_PATH)
	 
