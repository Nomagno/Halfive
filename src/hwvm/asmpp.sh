#!/bin/sh
#REQUIRES UNIX/POSIX COMMANDS 'm4' and 'sed'
m4 "$1" | sed 's/;.*$//; /^$/d'
