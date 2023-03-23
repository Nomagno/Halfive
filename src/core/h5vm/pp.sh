#!/bin/sh

# Inserts other files in place of '#i /path/to/file' lines
includepp(){
	output=
	IFS='
'

	while read -r line; do
		case "$line" in
		'#i'*)
			output="$output
$(cat "$(printstr "$line" | sed 's|^#i ||g')")"
		;;
		*)
			output="$output
$line"
		;;
		esac
	done < "$1"
	printf "%s\n" "$output"
}

includepp "$1"           |
	sed 's/^[ \t]*//g'   |
	sed 's/\;.*//g'      |
	sed '/^\s*$/d'       |
	awk -f pp_stage1.awk |
	sed 's/__/\n/g'      |
	sed 's/^[ \t]*//g'   |
	awk -f pp_stage2.awk | awk -f pp_stage3.awk
