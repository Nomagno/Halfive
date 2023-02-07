#!/bin/sh

#Copyright Nomagno 2021, 2022, 2023

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

# Preprocess Halfive Assembly

# REQUIREMENTS:
# POSIX sh
# POSIX printf
# POSIX echo
# POSIX grep
# POSIX sed
# POSIX cut
# POSIX tr
# POSIX dd
# POSIX cat
# POSIX expr

. ../utils.sh # Import script with the stac 'line-by-line reversal' function

# Number file in hexadecimal
numberhex(){
	decfile=$(printstr_n "$1" | nl -ba -w6 -v0 -s: -nln)
	IFS='
'
	printstr_n "$decfile" | while read -r line; do
		decnum=$(printstr "$line" | head -c 6)
		hexnum=$(printhex "$(printstr "$decnum" | trim)")
		hexline=$(printstr "$line" | sed "s/^....../$hexnum/g")
		printstr_n "$hexline"
	done
}

# Takes file, inserts other files in place of '#i /path/to/file' lines, saves to temp file
includepp(){
	file2=
	IFS='
'
	tmp=$(mktemp)

	while read -r line; do
		case "$line" in
		'#i'*)
			file2="$file2
$(cat "$(printstr "$line" | sed 's|^#i ||g')")"
		;;
		*)
			file2="$file2
$line"
		;;
		esac
	done < "$1"
	printstr_n "$file2" > "$tmp"
	printstr_n "$tmp"
}

# Preprocesses macros defined as '#d macro,meaning', '__' means newline
asmpp(){
	f=$(sed 's|;.*$||g; /#d /d' < "$1")
	rep=$(grep '^#d' "$1" |
	      sed 's/^#d //g; s/ /|/g')
	rep=$(echo $rep | stac) # Reverse $rep line-by-line

	# For each line in $rep, get the macro name and meaning
	for i in $rep; do
		p1=$(printstr_n "$i" |
		     cut -d',' -f1 |
		     sed 's/|/ /g; s/\*/\\*/g')
		p2=$(printstr_n "$i" |
		     cut -d',' -f2 |
		     sed 's/|/ /g; s/\*/\\*/g')

		# For each line in $rep, scan the entire file and replace the macro name for its macro meaning
		f=$(printstr "$f " |
		    sed "s/$p1/$p2/g")
	done
	# Replace __ with newline, eliminate indentation and whitespace padding
	f=$(printstr_n "$f" |
	    sed 's/__/\n/g; /^[[:space:]]*$/d;' |
	    sed 's/^[ \t]*//g')

	# This is not pretty, but it works and is not too egregious
	# Labels <FOO> -> every occurrance of >FOO< is replaced with (+1/-1) absolute of the
	# instruction number >FOO< appears in minus absolute of the instruction number
	# <FOO> appears in

	hexfile=$(numberhex "$f")
	hexrep=$(printstr_n "$hexfile" | grep '^.*:<.*>' | cut -d'>' -f1)

	IFS='
'
	for i in $hexrep; do
		labelhexnum=$(printstr "$i" | cut -d':' -f1 | sed 's/ //g')
		labelhexstr=$(printstr "$i" | cut -d'<' -f2)
		f=
		for j in $hexfile; do
			currhexnum=$(printstr_n "$j" | cut -d':' -f1)
			currdiff=$(weirdabs $((0x$labelhexnum - 0x$currhexnum)))
			replacedstring=$(printstr_n "$j" |
				sed "s/>$labelhexstr</=$currdiff/g")
			f=$(printf '%s\n%s' "$f" "$replacedstring")
		done
		f=$(printstr_n "$f" | sed "s/<$labelhexstr> *//g") # FAILURE POINT
		hexfile=$f
	done
	# Replace __ with newline, eliminate indentation and whitespace padding + numbering
	f=$(printstr_n "$f" |
	    sed 's/__/\n/g; /^[[:space:]]*$/d;' |
	    sed 's/^[ \t]*//g; s/ *$//g;' | cut -d':' -f2)
	printstr_n "$f"

}

# Preprocess for includes
tmp2=$(includepp "$1")

# Preprocess for macros
asmpp "$tmp2"

# Remove tmpfile
rm "$tmp2"
