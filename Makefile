CC:=gcc
LD:=ld

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)
UNIT_TEMPLATES=$(wildcard *.service.template)
UNITS=$(UNIT_TEMPLATES:.template=)

USER=$(shell whoami)

.PHONY: all
all: $(PROGS) $(UNITS)

.PHONY: clean
clean:
	@echo "We have $(SRCS), $(OBJS) and $(PROGS)"
	rm -f $(PROGS)
	rm -f $(OBJS)
	rm -f $(UNITS)

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
