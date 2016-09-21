CC:=gcc
LD:=ld

SRCS=$(wilcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)
UNIT_TEMPLATES=$(wildcard *.service.template)
UNITS=$(UNIT_TEMPLATES:.template=)

USER=$(shell whoami)

all: $(PROGS) $(UNITS)

# Building C files
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%: %.o
	$(LD) -o $@ $<

# Building systemd units
%.service: %.service.template Makefile
	sed -e 's|XXX_TEST_ROOT_XXX|'"${PWD}"'|' $< > $@
ifeq ($(user),root)
	ln -f -s ${PWD}/$@ /etc/systemd/system
else
	@echo "Can't install $@ when not root"
endif
