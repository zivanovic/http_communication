#############################################################
#
#	Author: Zoran Zivanovic
#	Project: http communication
#	Version:0.1
#
#############################################################

CC = gcc

DBG_FLEGS=-g -ggdb -O0 -Wall
 
CFLAGS = -I./include

CFLAGS+= $(DBG_FLEGS)
SRC= $(wildcard ./src/*.c) 
OBJS=$(SRC:.c=.o)

all: clean library test

library: $(OBJS)
	$(CC) $(CFLEGS) $(OBJS) -shared -o lib/libHttp.so
	cd test; make;cd -;
	
test:
	cd test; make;cd -;

clean:
	cd bin;rm *;cd -;
	cd lib;rm *; cd -;
	cd src;rm *.o; cd;
 
 