BIN=arse
CC=gcc
CFLAGS=-g -Wfatal-errors -Wpedantic -Wall
SOURCES=$(wildcard src/*.c)
DEBUG=-DDEBUG
OBJS=$(SOURCES:.c=.o)
DEPS=$(OBJS:.o=.d)

.PHONY: clean

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

release:
	make clean
	make DEBUG=""

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -MMD -MP -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS) $(BIN)

-include $(DEPS)
