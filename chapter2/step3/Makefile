CC=gcc-11
CFLAGS=-std=c18 -g -static

9cc: tokenizer.c
	$(CC) -o 9cc tokenizer.c

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ temp*

.PHONY: test clean
