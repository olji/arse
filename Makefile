LIB=libarse.so
CC=gcc
CFLAGS=-g -Wfatal-errors -Wpedantic -Wall -fPIC
SOURCES=$(wildcard src/*.c)
TSOURCES=$(wildcard testing/*.c)
DEBUG=-DDEBUG
PWD=$(shell pwd)
OBJS=$(SOURCES:.c=.o)
TOBJS=$(TSOURCES:.c=.o)
DEPS=$(OBJS:.o=.d)

.PHONY: clean

test: $(LIB)
	$(CC) -L$(PWD) -Wl,-rpath=$(PWD) -g $(DEBUG) -Isrc/ $(TSOURCES) $(LIB) -o $@
$(LIB): $(OBJS)
	gcc --shared -o $(LIB) $(OBJS)

release:
	make clean
	make DEBUG=""

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -MMD -MP -c $< -o $@

clean:
	$(RM) $(OBJS) $(TOJBS) $(DEPS) $(LIB)

-include $(DEPS)
