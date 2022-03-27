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

# Turn HWVM machine code in ASCII form into instructions. The machine code has
# to follow this format format:
#    F        F   FFFF   FFFF
# mask   opcode   arg1   arg2
# Which is the same format as the binary machine code, but with padding on the 
# variable length instructions if necessary

mask(){
	IN=$(cat)

	if [ "$IN" = B  ]; then
		echo 1
		echo 1
		echo 1
	elif [ "$IN" = A  ]; then
		echo 1
		echo 1
		echo 0
	elif [ "$IN" = 9  ]; then
		echo 1
		echo 0
		echo 1
	elif [ "$IN" = 3  ]; then
		echo 0
		echo 1
		echo 1
	elif [ "$IN" = 2  ]; then
		echo 0
		echo 1
		echo 0
	elif [ "$IN" = 1  ]; then
		echo 0
		echo 0
		echo 1
	fi
}

instruction() {
	IN=$(cat)
	if [ "$IN" = '0' ]; then
		printf halt
	elif [ "$IN" = '1' ]; then
		printf nop
	elif [ "$IN" = '2' ]; then
		printf set
	elif [ "$IN" = '3' ]; then
		printf jmp
	elif [ "$IN" = '4' ]; then
		printf jcz
	elif [ "$IN" = '5' ]; then
		printf add
	elif [ "$IN" = '6' ]; then
		printf sub
	elif [ "$IN" = '7' ]; then
		printf and
	elif [ "$IN" = '8' ]; then
		printf or
	elif [ "$IN" = '9' ]; then
		printf xor
	elif [ "$IN" = 'A' ]; then
		printf rot
	elif [ "$IN" = 'B' ]; then
		printf cmp
	elif [ "$IN" = 'C' ]; then
		printf func
	elif [ "$IN" = 'D' ]; then
		printf ret
	elif [ "$IN" = 'E' ]; then
		printf call
	elif [ "$IN" = 'F' ]; then
		printf jcnz
	fi
}

COUNT=0

ISPTR=0
IS2_LIT=0
IS1_LIT=0
IFS=' 
'

for word in $(cat); do
	if [ "$COUNT" = 0  ]; then
		MASK=$(printf "%s" "$word" | mask)
		ISPTR=$(printf "%s\n" "$MASK" | head -n 1)
		IS2_LIT=$(printf "%s\n" "$MASK" | head -n 2 | tail -n 1)
		IS1_LIT=$(printf "%s\n" "$MASK" | tail -n 1)
		COUNT=1
	elif [ "$COUNT" = 1  ]; then
		printf "%s" "$word" | instruction
		COUNT=2
	elif [ "$COUNT" = 2  ]; then
		printf ' '
		if [ "$IS2_LIT" = 1 ] && [ "$ISPTR" != 1 ]; then
			printf '='
		elif [ "$IS2_LIT" = 1 ] && [ "$ISPTR" = 1 ]; then
			printf '&'
		fi
		printf '%s' "$word"
		COUNT=3
	elif [ "$COUNT" = 3  ]; then
		printf ' '
		if [ "$IS1_LIT" = 1 ] && [ "$ISPTR" != 1 ]; then
			printf '='
		elif [ "$IS1_LIT" = 1 ] && [ "$ISPTR" = 1 ]; then
			printf '&'
		fi
		printf '%s\n' "$word"
		COUNT=0
	fi	

done
