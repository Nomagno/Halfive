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
        <title>Half-World Traffic</title>
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
