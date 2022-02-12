#!/bin/sh
# Extract traffic out of nginx logs, generate HTML
# Requirements: GNU grep / BSD grep / any grep with -o
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
        <meta name="description" content="The Universal racing game protocol">
</head>
<body>
<h2>Recent requests to pages on this site:</h2>'

printf "%s" "$OUTFILE" > "$OUTPATH"/traffic.html
IFS='\n'
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
echo '</body>' >> "$OUTPATH"/traffic.html
