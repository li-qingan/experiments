
LEVEL=..
include $(LEVEL)/makefile.config

LDFLAGS= -ldwarf -lelf
CFLAGS= -O2 -g

getSymbol: getSymbol.cpp tool.cpp tool.h
	$(CXX) $(CFLAGS) getSymbol.cpp tool.cpp -o getSymbol  $(LDFLAGS)
	cp getSymbol $(BIN_PATH)
