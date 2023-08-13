#!/bin/sh
rm_ext() {
	basename $(echo $1 | sed 's/\..*$//g')
}

rm -rf html/
mkdir html
for i in $(ls md/*.md); do
	pandoc --ascii -f markdown-auto_identifiers -t html $i |
	cat "css_""$(rm_ext $i)" - > "html/""$(rm_ext $i)"".html"
done
