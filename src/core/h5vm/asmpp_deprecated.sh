#!/bin/sh

# !!!!!!!!! THIS IS DEPRECATED, USE THE NEW PREPROCESSOR pp.sh
# !!!!!!!!! THIS IS DEPRECATED, USE THE NEW PREPROCESSOR pp.sh
# !!!!!!!!! THIS IS DEPRECATED, USE THE NEW PREPROCESSOR pp.sh

printstr()    { printf '%s' "$1";                    } # Print string
printstr_n()  { printf '%s\n' "$1";                  } # Print string with trailing newline
printhex()    { printf '%x\n' "$1" | tr 'a-z' 'A-Z'; } # Print hexadecimal
weirdabs(){
	if [ "$(($1 < 0))" = 1 ]; then echo "$(($1 + 1))" | sed 's/-//g'
	else echo "$(($1 - 1))" | sed 's/-//g'
	fi
}
trim(){ cat | sed 's/ *$//g' }
# Reverse line-by-line, WEIRD
stac(){
	words=$(cat); size='-1'; count='-1'; IFS=' '
	for _ in $words; do size=$((size + 1)); done
	for _ in $words; do
		count=$((count + 1))
		opposite_count=$((size - count + 1))
		printstr "$(printstr_n "$words" | cut -d' ' -f"$opposite_count") "
	done
	echo
}

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
