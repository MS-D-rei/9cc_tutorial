CC=gcc-11
CFLAGS=-std=c18 -g -static

9cc: main.c error_at.c
	$(CC) -o 9cc main.c error_at.c

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.0 *~ temp*

.PHONY: test clean
