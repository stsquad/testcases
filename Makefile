CC:=gcc
LD:=ld

SRCS=$(wilcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)
UNIT_TEMPLATES=$(wildcard *.service.template)
UNITS=$(UNIT_TEMPLATES:.template=)

all: $(PROGS) $(UNITS)

# Building C files
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%: %.o
	$(LD) -o $@ $<

# Building systemd units
%.service: %.service.template
	sed -e 's|XXX_TEST_ROOT_XXX|'"${PWD}"'|' $< > $@
	ln -s ${PWD}/$@ /etc/systemd/system
