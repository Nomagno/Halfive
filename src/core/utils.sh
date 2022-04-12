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

# POSIX shell util collection

# Requirements:
# POSIX sh
# POSIX cat
# POSIX tr
# POSIX dd
# POSIX cut
# POSIX expr
# POSIX printf
# POSIX echo


stac(){
	words=$(cat)
	size='-1'
	count='-1'

	IFS=' '
	for i in $words; do
		size=$((size + 1))
	done	
	for i in $words; do
		count=$((count + 1))
		opposite_count=$((size - count + 1))
		printf "%s " "$(printf '%s\n' "$words" | cut -d' ' -f"$opposite_count")"
	done
	echo
}

# $1 - random byte stream (e.g. /dev/urandom)
# $2 - character count of the key (e.g. 10)
genkey(){
	(LC_CTYPE=C tr -dc 'A-Z' |\
	 LC_CTYPE=C dd bs=1 count="$2" 2>/dev/null) < "$1"
	echo	
}
