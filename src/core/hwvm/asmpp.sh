#!/bin/sh

#Copyright Nomagno 2021, 2022

#Redistributions, modified or unmodified, in whole or in part, must retain
#applicable notices of copyright or other legal privilege, these conditions, and
#the following license terms and disclaimer.  Subject to these conditions, each
#holder of copyright or other legal privileges, author or assembler, and
#contributor of this work, henceforth "licensor", hereby grants to any person
#who obtains a copy of this work in any form:

#1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
#and/or otherwise deal in the licensed material without restriction.

#2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
#patent license to make, have made, provide, transfer, import, use, and/or
#otherwise deal in the licensed material without restriction, for any and all
#patents held by such licensor and necessarily infringed by the form of the work
#upon distribution of that licensor's contribution to the work under the terms
#of this license.

#NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
#OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
#LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
#AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
#COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
#LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
#OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
#WORK.

# Preprocess Half-World Assembly

#REQUIREMENTS:
#POSIX sh
#POSIX printf
#POSIX grep
#POSIX cut
#POSIX cat
#POSIX sed
#POSIX awk

helpmsg='USAGE: asmpp.sh OPTION
   -m   Read macro+assembly from stdin, output hwassembly
   -b   Read ascii-machinecode from stdin, output hwassembly
   -h   Print this message

macro format:
   #d MACRO,MEANING

ascii-machinecode format:
   Where each X is a hexadecimal digit, and 
   each block is separated by a single space:

      X        X   XXXX   XXXX
   mask   opcode   arg1   arg2
'

. ../utils.sh

rep=$(grep '^#d' "$1" | sed 's/^#d //g; s/ /|/g')
f=$(cat "$1" | sed 's|;.*$||g; /#d /d')

rep=$(echo $rep | stac)

for i in $rep; do
p1=$(printf '%s\n' "$i" | cut -d',' -f1 | sed 's/|/ /g; s/\&/\\&/g')
p2=$(printf '%s\n' "$i" | cut -d',' -f2 | sed 's/|/ /g; s/\&/\\&/g')
f=$(printf '%s '"$f" | sed "s/$p1/$p2/g")
done

printf '%s\n' "$f" | sed 's/__/\n/g;' | awk 'NF' | sed 's/^[[:blank:]]*//g'
