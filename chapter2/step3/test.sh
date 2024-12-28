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

assert 0+0 0
assert 20+5-4 21
assert " 20 + 5 - 4" 21

echo "Test End"
