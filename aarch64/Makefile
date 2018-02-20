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

HAVE_CC := $(shell command -v $(CC) 2> /dev/null)

GCC8 := $(shell command -v gcc-8 2> /dev/null)

ifneq ("7", $(patsubst .*, $(GCC_VERSION), ""))
SRCS=$(filter-out fminmaxv.c, $(wildcard *.c))
else
SRCS=$(wildcard *.c)
endif

OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

# Additional programs with different
PROGS+=vector-benchmark-tree-vec
PROGS+=vector-benchmark-funroll

ifndef GCC8
$(info "skipping SVE enabled build")
else
PROGS+=vector-benchmark-sve
endif

.PHONY: all
ifndef HAVE_CC
all:
    $(info "skipping build for AArch64 binaries")
else
all: $(PROGS)
endif

# Compiler variants
vector-benchmark-tree-vec: vector-benchmark.c
	$(CC) $(CFLAGS) -g3 -O3 -static -ftree-vectorize $< -o $@ -lm

vector-benchmark-funroll: vector-benchmark.c
	$(CC) $(CFLAGS) -g3 -O3 -static -funroll-loops $< -o $@ -lm

vector-benchmark: vector-benchmark.c
	$(CC) $(CFLAGS) -g3 -O3 -static $< -o $@ -lm

GCC8 := $(shell command -v gcc-8 2> /dev/null)
ifndef GCC8
$(info "skipping SVE enabled build")
else
PROGS+=vector-benchmark-sve
vector-benchmark-sve: vector-benchmark.c
	gcc-8 $(CFLAGS) -g3 -O3 -static -march=armv8.2-a+sve $< -o $@ -lm
endif

.PHONY: clean
clean:
	rm -f $(PROGS)
	rm -f $(OBJS)

# Special cases
fminmaxv: CFLAGS+=-march=armv8.2-a+fp16

