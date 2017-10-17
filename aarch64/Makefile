#
# Makefile to build testcases
#

HOST=$(shell uname -m)

ifeq ($(HOST),aarch64)
	# Only set these if not already done
	CC ?= gcc
	LD ?= ld
else
	CC = aarch64-linux-gnu-gcc
	LD = aarch64-linux-gnu-gcc
endif

CFLAGS=-g3 -O3 -ftree-vectorize -fopt-info -fno-unroll-loops -fno-peel-loops

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

.PHONY: all
all: $(PROGS)

.PHONY: clean
clean:
	rm -f $(PROGS)
