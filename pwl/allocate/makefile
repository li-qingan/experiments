LIBS=
CFLAGS= -Wall -I/usr/include -I. -c -O -g
CC1=gcc
LIB_PATH=../bin

install: objects
	ar crs $(LIB_PATH)/memory.a $(objects)
objects: MemoryAllocator.o aux.o

	
MemoryAllocator.o: MemoryAllocator.h MemoryAllocator.cpp 
	$(CC1) $(CFLAGS) MemoryAllocator.cpp -o MemoryAllocator.o

aux.o: aux.h aux.cpp
	$(CC1) $(CFLAGS) aux.cpp -o aux.o
