CC=gcc-11
CFLAGS=-std=c18 -g -static

9cc: main.c
	$(CC) -o 9cc main.c

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ temp*

.PHONY: test clean
