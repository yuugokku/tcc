#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./tcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"
    
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 30 "6+6+6+6+6"
assert 41 " 12 + 34 - 5"
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 2 '10 + 4*-2'
assert 1 '4 == 4'
assert 1 '1 + 10 == 15 - 4'
assert 0 '5 == 0'
assert 1 '5 != 1'
assert 2 '(3 == 3) + (2 == 2)'
assert 1 '5 > -4'
assert 1 '5 >= 5'
assert 5 '(+5 >= -2) * 5'

echo OK
