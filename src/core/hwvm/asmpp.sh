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


# Takes file, inserts other files in place of '#i /path/to/file' lines, saves to temp file
includepp(){
	file=$(cat "$1")
	file2=
	IFS='
'
	tmp=$(mktemp)
	
	for line in $(printf '%s\n' "$file"); do
		if echo "$line" | grep -q '^#i'; then
			file2="$file2
$(cat "$(printf "%s" "$line" | sed 's|^#i ||g')")"
		else
			file2="$file2
$line"
		fi
	done
	printf '%s\n' "$file2" > "$tmp"
	printf '%s\n' "$tmp"
}

# Preprocesses macros defined as '#d macro,meaning', '__' means newline
asmpp(){
	. ../utils.sh # Import script with the stac 'line-by-line reversal' function
	f=$(sed 's|;.*$||g; /#d /d' < "$1") # File without comments and without the macro definions
	rep=$(grep '^#d' "$1" | sed 's/^#d //g; s/ /|/g') # Keep only macros, without the '#d ', and with spaces escaped
	rep=$(echo $rep | stac) # Reverse $rep line-by-line
	for i in $rep; do
		p1=$(printf '%s\n' "$i" | cut -d',' -f1 | sed 's/|/ /g; s/\&/\\&/g') # For each line in $rep, get the macro name
		p2=$(printf '%s\n' "$i" | cut -d',' -f2 | sed 's/|/ /g; s/\&/\\&/g') # For each line in $rep, get the macro meaning
		f=$(printf '%s ' "$f" | sed "s/$p1/$p2/g") # For each line in $rep, scan the entire file and replace the macro name for its macro meaning
	done
	f=$(printf '%s\n' "$f" | sed 's/__/\n/g; /^[[:space:]]*$/d;' | sed 's/^[ \t]*//g') # Replace __ with newline, eliminate indentation and whitespace padding
	printf '%s\n' "$f" # Print final file
}

tmp2=$(includepp "$1") # Preprocess for includes
asmpp "$tmp2" # Preprocess for macros
rm "$tmp2" # Remove tmpfile
