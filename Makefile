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

.PHONY: clean test install uninstall

$(TEST): $(LIB) $(TSOURCES)
	$(CC) -L$(PWD) -Wl,-rpath=$(PWD) -g $(DEBUG) -Isrc/ $(TSOURCES) $(LIB) -o $@

$(LIB): $(OBJS)
	gcc --shared -o $(LIB) $(OBJS)

release:
	make clean
	make DEBUG=""

install: 
	cp $(LIB) /usr/lib/
	cp src/arse.h /usr/include/

uninstall:
	rm -i /usr/lib/$(LIB) /usr/include/arse.h

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -MMD -MP -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS) $(LIB)

-include $(DEPS)
