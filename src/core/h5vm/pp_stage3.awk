function abs(n){ return (n < 0)?(-n):n; }
BEGIN { FS=" "; state=0; L=0; }
(state == 0) { LABELS[$1]=$2; if($1 == "----"){ state=1; next; } }
(state == 1) { L+=1; }
(state == 1) && !/<.*</ { print $0 }
(state == 1) &&  /<.*</ {
	split($0, SPLIT, "<");
	sub("<.*<", "="abs(LABELS[SPLIT[2]]-L)-1, $0);
	print $0;
}
# All statements of the form '<LABEL<' are label references.
# There is at most one per line.
# Replace each label reference with abs(LABELS[label] - NR) - 1
