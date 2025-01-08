CC=gcc-11
CFLAGS=-std=c18 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ temp*

.PHONY: test clean
