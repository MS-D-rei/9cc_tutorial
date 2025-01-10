#!/bin/bash

# if want to debug, run `bash -x test.sh`

assert() {
    input="$1"
    expected="$2"

    ./9cc "$input" > temp.s
    cc -o temp temp.s
    ./temp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
    	echo "$input => $expected expected, but got $actual"
	exit 1
    fi
}

assert "0+0;" 0
assert "20+5-4;" 21
assert " 20 + 5 - 4;" 21
assert "5+6* 7;" 47
assert "5*(9-6);" 15
assert "(3+5)/2;" 4

# Unary
assert "-2 + 6;" 4
assert "+6 - 1;" 5

# Multiletter operator
assert "1 == 1;" 1
assert "1 != 0;" 1
assert "1 != 1;" 0
assert "1+1 == 2;" 1
assert "(1+1)*2 == 4;" 1
assert "(1+1)*2 < 4;" 0
assert "(1+1)*2 <= 4;" 1
assert "(1+1)*2 > 4;" 0
assert "(1+1)*2 >= 4;" 1

# 1 char local variable
assert "a=2; b=4; a+b;" 6

echo "Test end"
