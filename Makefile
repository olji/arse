LIB=libarse.so
TEST=arsetest
CC=gcc
CFLAGS=-g -Wfatal-errors -Wpedantic -Wall -fPIC
SOURCES=$(wildcard src/*.c)
TSOURCES=$(wildcard testing/*.c)
DEBUG=-DDEBUG
PWD=$(shell pwd)
OBJS=$(SOURCES:.c=.o)
DEPS=$(OBJS:.o=.d)

.PHONY: clean test

$(TEST): $(LIB) $(TSOURCES) fullname.c
	$(CC) -L$(PWD) -Wl,-rpath=$(PWD) -g $(DEBUG) -Isrc/ $(TSOURCES) $(LIB) -o $@
	$(CC) -L$(PWD) -Wl,-rpath=$(PWD) -g $(DEBUG) -Isrc/ fullname.c $(LIB) -o fullname 

$(LIB): $(OBJS)
	gcc --shared -o $(LIB) $(OBJS)

release:
	make clean
	make DEBUG=""

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -MMD -MP -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS) $(LIB)

-include $(DEPS)
