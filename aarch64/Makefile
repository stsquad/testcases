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
CFLAGS=-g3 -O3 -ftree-vectorize -fopt-info -static

ifneq ("7", $(patsubst .*, $(GCC_VERSION), ""))
SRCS=$(filter-out fminmaxv.c, $(wildcard *.c))
else
SRCS=$(wildcard *.c)
endif

OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

HAVE_CC := $(shell command -v $(CC) 2> /dev/null)

.PHONY: all
ifndef HAVE_CC
all:
    $(info "skipping build for AArch64 binaries")
else
all: $(PROGS)
endif

.PHONY: clean
clean:
	rm -f $(PROGS)
	rm -f $(OBJS)

# Special cases
fminmaxv: CFLAGS+=-march=armv8.2-a+fp16
