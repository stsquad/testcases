# Only set these if not already done
CC ?= gcc
LD ?= ld
CFLAGS=-g3

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

.PHONY: all
all: $(PROGS)

.PHONY: clean
clean:
	rm -f $(PROGS)
	rm -f $(OBJS)
