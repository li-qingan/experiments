
LEVEL=..
include $(LEVEL)/makefile.config

CXXFLAGS= -O2 -g
ifdef DEBUG
CXXFLAGS=-DDEBUG -O0 -g
endif

LDFLAGS=-L$(BIN_PATH) -lmemory


defaultAllocator: defaultAllocator.cpp $(BIN_PATH)/libmemory.a
	$(CXX) $(CXXFLAGS) defaultAllocator.cpp -o defaultAllocator $(LDFLAGS)
	cp defaultAllocator $(BIN_PATH)
