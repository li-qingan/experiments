
LEVEL=..
include $(LEVEL)/makefile.config

CXXFLAGS= -O2 -g
ifdef DEBUG
CXXFLAGS=-DDEBUG -O0 -g
endif

LDFLAGS=-L$(BIN_PATH) -lmemory

all: defaultAllocator

defaultAllocator: defaultAllocator.cpp
	$(CXX) $(CXXFLAGS) defaultAllocator.cpp -o $(BIN_PATH)/defaultAllocator $(LDFLAGS)
