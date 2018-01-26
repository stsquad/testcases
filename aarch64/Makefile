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
CFLAGS=-g3 -O3 -static

ifneq ("7", $(patsubst .*, $(GCC_VERSION), ""))
SRCS=$(filter-out fminmaxv.c, $(wildcard *.c))
else
SRCS=$(wildcard *.c)
endif

OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

# Compiler variants
PROGS+=vector-benchmark-tree-vec
vector-benchmark-tree-vec: vector-benchmark.c
	$(CC) $(CFLAGS) -g3 -O3 -static -ftree-vectorize $< -o $@

PROGS+=vector-benchmark-funroll
vector-benchmark-funroll: vector-benchmark.c
	$(CC) $(CFLAGS) -g3 -O3 -static -funroll-loops $< -o $@

GCC8 := $(shell command -v gcc-8 2> /dev/null)
ifndef GCC8
$(info "skipping SVE enabled build")
else
PROGS+=vector-benchmark-sve
vector-benchmark-sve: vector-benchmark.c
	gcc-8 $(CFLAGS) -g3 -O3 -static -march=armv8.2-a+sve $< -o $@
endif

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

