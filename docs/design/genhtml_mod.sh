#!/bin/sh
rm_ext() {
	basename $(echo $1 | sed 's/\..*$//g')
}

rm -rf html/
mkdir html
for i in $(ls md/*.md); do
	pandoc --ascii -f markdown-auto_identifiers -t html $i |
# FOR ../GENHTML.SH, MEANS FOR THE MAIN WEBSITE'S GENERATION NEEDS AND MAINLY SPECS I GUESS, INSTEAD OF THIS LINE:
	cat "css_""$(rm_ext $i)" - > "html/""$(rm_ext $i)"".html"
# USE THIS:
#   cat "$1/""css_""$(rm_ext $i)" - > "$1/""$(rm_ext $i)"".html"
# PLEASE REREAD THIS AND THINK ABOUT IT BEFORE APPLYING BLINDLY, I DO NOT REMEMBER THE RATIONALE
done
