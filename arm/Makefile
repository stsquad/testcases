#
# Makefile to build testcases
#

HOST=$(shell uname -m)

ifeq ($(HOST),armv7l)
	# Only set these if not already done
	CC ?= gcc
	LD ?= ld
else
	CC = arm-linux-gnueabihf-gcc
	LD = arm-linux-gnueabihf-ld
endif

CFLAGS=-g3 -static

SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

.PHONY: all
all: $(PROGS)


.PHONY: clean
clean:
	rm -f $(PROGS)
