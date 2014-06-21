LIBS=
CFLAGS= -Wall -I/usr/include -I. -O -g
CC1=gcc
LIB_PATH=../bin

install: objects
	ar crs $(LIB_PATH)/memory.a $(objects)
objects: mm.o memlib.o
	
mm.o: memlib.h mm.h 
memlib.o: memlib.h


