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

GCC_VERSION=$(shell gcc -dumpversion)
CFLAGS=-g3 -O3 -ftree-vectorize -fopt-info

ifneq ("7", $(patsubst .*, $(GCC_VERSION), ""))
SRCS=$(filter-out fminmaxv.c, $(wildcard *.c))
else
SRCS=$(wildcard *.c)
endif

OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

.PHONY: all
all: $(PROGS)

.PHONY: clean
clean:
	rm -f $(PROGS)

# Special cases
fminmaxv: CFLAGS+=-march=armv8.2-a+fp16
