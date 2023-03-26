# FIRST STAGE

function lsub(e,r,i){ sub(e,r,i); return i; }
function esc_str(str) { gsub(/[\\.^$(){}\[\]|*+?]/, "\\\\&", str); return str; }
function alen(a,i,k) { for(i in a) k++; return k; }

BEGIN {
	lineCountForLabelProcessing=0;
}

{
	L[NR]=$0;
}

# \043 is the '#' symbol in octal
# /^#d /: '#d MACRO,VALUE' is a macro definition. Store MACRO and VALUE
# !/^#d /: In each non-macro line, substitute MACRO with VALUE
/^\043d /  {
	split(L[NR], SPLIT, ",");
	MACROS[lsub("^#d ", "", esc_str(SPLIT[1]))]=esc_str(SPLIT[2]);
	L[NR]="ERASE";
}
!/^\043d / {
	for(m in MACROS){
		L[NR]=lsub(m, MACROS[m], L[NR]);
	};
	lineCountForLabelProcessing+=1;
}

# All lines starting with '>LABEL> ' are label definitions.
# /^>.*> /: Store LABEL along with the line number
/^>.*> / {
	split(L[NR], SPLIT, ">");
	LABELS[SPLIT[2]]=lineCountForLabelProcessing;
	sub("^>.*> ", "", L[NR]);
}

# END: print all label definitions and their line numbers, then the file so far.
# This will be  processed by the second stage (pp_2.awk)
END {
	for(v in LABELS){
		print v " " LABELS[v];
	};
	print "----";
	len=alen(L);
	c=1;
	while(c <= len){
		if(L[c] != "ERASE") {
			print L[c];
		}
		c++;
	}
}
