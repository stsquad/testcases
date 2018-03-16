#
# Makefile to build testcases
#

# Only set these if not already done
CC ?= gcc
LD ?= ld
CFLAGS=-g3 -static $(shell pkg-config --cflags glib-2.0)

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

LDLIBS += -lm $(shell pkg-config --libs glib-2.0) -lpthread
#CFLAGS += -lm

.PHONY: all
all: $(PROGS) Makefile
