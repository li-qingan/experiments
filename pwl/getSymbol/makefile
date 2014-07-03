
LEVEL=..
include $(LEVEL)/makefile.config

LDFLAGS= -ldwarf -lelf

CFLAGS= -g -O2

ifdef DEBUG
CFLAGS=-DDEBUG -g -O0 
endif

getSymbol: getSymbol.cpp tool.cpp tool.h
	$(CXX) $(CFLAGS) getSymbol.cpp tool.cpp -o getSymbol  $(LDFLAGS)
	cp getSymbol $(BIN_PATH)
