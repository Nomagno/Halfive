function alen(a,i,k) { for(i in a) k++; return k; }
BEGIN { FS=">"; }
/^>.*> / { LABELS[$2]=NR; sub("^>.*> ", "", $0); }
{ LINES[NR]=$0; }
END {
	for(v in LABELS){ print v " " LABELS[v]; };
	print "----";
	l=alen(LINES); c=1;
	while(c <= l){ print LINES[c]; c++; }
}

# All lines starting with '>LABEL> ' are label definitions.
# /^>.*> /: Store LABEL along with the line number
