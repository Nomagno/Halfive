#!/bin/sh

#Copyright Nomagno 2021, 2022, 2023
#
#Permission is hereby granted, free of charge, to any person obtaining
#a copy of this software and associated documentation files (the "Software"),
#to deal in the Software without restriction, including without limitation
#the rights to use, copy, modify, merge, publish, distribute, sublicense,
#and/or sell copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice (including the next
#paragraph) shall be included in all copies or substantial portions of the
#Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
#IN THE SOFTWARE.

# Generate RSS feed from files in directory
# Requirements:
# POSIX sh
# POSIX sed
# POSIX grep
# POSIX ls
# POSIX printf

TEMPLATE='<head>
<link rel="icon" href="halfive_favicon.png" type="image/icon type">
<style type="text/css">
body{
line-height:1.6;
font-size:18px;
color:#444;
padding:0
10px}
h1,h2,h3{
line-height:1.2
}
</style>
	<title>_TITLE</title>
	<meta name="description" content="The Universal Racing Game"">
</head>'

#!/bin/sh
rm_ext() {
	basename $(echo $1 | sed 's/\..*$//g')
}

rm -rf html/
mkdir html
for i in $(ls "$1"/*.md); do
	pandoc --ascii -f markdown-auto_identifiers -t html "$i" |
	printf "%s\n%s" "$TEMPLATE" "$(cat -)" | sed "s/_TITLE/""$(rm_ext $i)/g" > "$1/""$(rm_ext $i)"".html"
done
