
LEVEL=..
include $(LEVEL)/makefile.config

CXXFLAGS= -O2 -g
ifdef DEBUG
CXXFLAGS=-DDEBUG -O0 -g
endif

LDFLAGS=-L$(BIN_PATH) -lmemory

optiAllocator: optiAllocator.cpp
	$(CXX) $(CXXFLAGS) optiAllocator.cpp -o $(BIN_PATH)/optiAllocator $(LDFLAGS)
	 
