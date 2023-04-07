# SECOND STAGE

function alen(a,i,k) { for(i in a) k++; return k; }

{
	L[NR]=$0;
}

# All lines starting with '>LABEL> ' are label definitions.
# /^>.*> /: Store LABEL along with the line number
/^>.*> / {
	split(L[NR], SPLIT, ">");
	LABELS[SPLIT[2]]=NR;
	gsub("^>.*> ", "", L[NR]);
}

# END: print all label definitions and their line numbers, then the file so far.
# This will be  processed by the third stage (pp_3.awk)
END {
	for(v in LABELS){
		print v " " LABELS[v];
	};
	print "----";
	len=alen(L);
	c=1;
	while(c <= len){
		print L[c];
		c++;
	}
}
