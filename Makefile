CC:=gcc
LD:=ld

SRCS=$(wilcard *.c)
OBJS=$(SRCS:.c=.o)
PROGS=$(OBJS:.o=)

all: $(PROGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%: %.o
	$(LD) -o $@ $<

