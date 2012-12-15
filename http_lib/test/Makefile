#############################################################
#
#	Author: Zoran Zivanovic
#	Project: http communication
#	Version:0.1
#
#############################################################

CC=gcc
DBG_FLEGS=-g -ggdb -O0 -Wall
CFLAGS= -I../include
CFLAGS+= $(DBG_FLEGS)

LDFLAGS= -L../lib -lHttp


SRC= $(wildcard ./*.c) 
OBJS=$(SRC:.c=.o)

all: test

test: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS)  $(OBJS) -o ../bin/test_http

clean:
	rm *.o;
	cd ../bin;rm test_http; cd -;
	
