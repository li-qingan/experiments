
LEVEL=.

include $(LEVEL)/makefile.config


MAKE=make

all: getSymbol1 allocLib1 defaultAllocator1 optiAllocator1
	@echo "finished"

getSymbol1:
	@echo "==========="
	$(MAKE) -C getSymbol
	
allocLib1:
	@echo "==========="
	$(MAKE) -C allocLib
	
defaultAllocator1: allocLib
	@echo "==========="
	$(MAKE) -C defaultAllocator
	
optiAllocator1: allocLib
	@echo "==========="
	$(MAKE) -C optiAllocator

	

