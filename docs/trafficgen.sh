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

# Extract traffic out of nginx logs, generate HTML

# Requirements: 
# POSIX sh
# GNU grep / BSD grep / any grep with -o
# POSIX cat
# POSIX sed
# POSIX awk
# POSIX find
# POSIX cut
# POSIX printf
# POSIX read

# Variables that need to be exported:
# OUTPATH=/path/to/webserver/root/
# LOGPATH=/path/to/access.log

INLIST=$(cat "$LOGPATH" | grep -o '"GET /.* HTTP...."' |\
sed 's/"GET //g; s/ HTTP...."//g' | \
grep -e "^/$" -e "README" -e "LICENSE" -e "\.md" -e "\.xml" -e "\.html")

OUTLIST=$(printf '%s' "$INLIST" | awk -F : '{for(i=1; i<=NF; ++i) tab[$i]++} 
END {for(t in tab)
{printf "%c%s;%d", sep, t, tab[t]; 
sep = "\n"};
printf "\n"}')

OUTFILE='<head>
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
        <title>Halfive Traffic</title>
        <meta name="description" content="The Universal Racing Game">
</head>
<body>
<h2>Recent visits:</h2>
<h3>>Rotated every monday</h3>'

printf "%s" "$OUTFILE" > "$OUTPATH"/traffic.html
IFS='
'
printf '%s\n' "$OUTLIST" | while read -r tok; do
	var1=$(printf '%s\n' "$tok" | cut -d';' -f1)
	var2=$(printf '%s\n' "$tok" | cut -d';' -f2)
	find "$OUTPATH" | grep -q "$var1"
	returnVal="$?"
	if [ $returnVal -eq 0 ]
	then
	outvar="<p>$var1 - $var2 visits</p>"
	printf "%s\n" "$outvar" >> "$OUTPATH"/traffic.html
	fi
done
printf '</body>\n' >> "$OUTPATH"/traffic.html
