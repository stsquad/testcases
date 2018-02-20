#
# Makefile to build testcases
#

# Only set these if not already done
CC ?= gcc
LD ?= ld
CFLAGS=-g3

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)
UNIT_TEMPLATES=$(wildcard *.service.template)
UNITS=$(UNIT_TEMPLATES:.template=)

USER=$(shell whoami)

SUBDIRS := $(wildcard */.)
SUBDIRSCLEAN=$(addsuffix -clean,$(SUBDIRS))

# C files are currently build with default rules
# which is a single stage compile and link

.PHONY: all
all: $(PROGS) $(UNITS) $(SUBDIRS)

.PHONY: clean
clean: $(SUBDIRSCLEAN)
	rm -f $(PROGS)
	rm -f $(OBJS)
	rm -f $(UNITS)

%-clean: %
	$(MAKE) -C $< clean

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

# Building systemd units
%.service: %.service.template Makefile
	sed -e 's|XXX_TEST_ROOT_XXX|'"${PWD}"'|' $< > $@
ifeq ($(user),root)
	ln -f -s ${PWD}/$@ /etc/systemd/system
else
	@echo "Can't install $@ when not root"
endif
