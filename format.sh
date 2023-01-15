#!/bin/sh
# NEEDS clang-format

IFS='
'

list=$(find . | grep "\.[hc]$")

for i in $list; do
	clang-format "$i" > "$i"2
	mv "$i"2 "$i"
done
