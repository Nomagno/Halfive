#!/bin/sh
rep=$(grep '^#d' "$1" | sed 's/^#d //')
f=$(cat "$1" | sed 's|;.*$||; /^$/d; /#d/d')

for i in $rep; do
p1=$(echo "$i" | cut -d',' -f1)
p2=$(echo "$i" | cut -d',' -f2)
f=$(printf "$f" | sed "s/$p1/$p2/")
done

printf "%s\n" "$f"
