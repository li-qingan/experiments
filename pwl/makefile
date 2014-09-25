#use "make" command to build this experimental setup tools

LEVEL=.

include $(LEVEL)/makefile.config


MAKE=make

.PHONY : all getSymbol1 allocLib1 defaultAllocator1 optiAllocator1

all: getSymbol1 allocLib1 defaultAllocator1 optiAllocator1
	@echo "finished"

getSymbol1:
	@echo "==========="
	$(MAKE) -C getSymbol
	
allocLib1:
	@echo "==========="
	$(MAKE) -C allocLib
	
defaultAllocator1: allocLib1
	@echo "==========="
	$(MAKE) -C defaultAllocator
	
optiAllocator1: allocLib1
	@echo "==========="
	$(MAKE) -C optiAllocator

clean:	
	rm allocLib/*.o

	

