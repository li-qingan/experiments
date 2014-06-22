
LEVEL=..
include $(LEVEL)/makefile.config

LDFLAGS= -ldwarf -lelf
CFLAGS= -O2 -g

all: getSymbol
.PHONY: all

getSymbol: getSymbol.cpp tool.cpp tool.h
	$(CXX) $(CFLAGS) getSymbol.cpp tool.cpp -o $(BIN_PATH)/getSymbol  $(LDFLAGS)
