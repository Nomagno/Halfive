# FIRST STAGE

function lgsub(e,r,i){ gsub(e,r,i); return i; }
function esc_str(str) { gsub(/[\\.^$(){}\[\]|*+?]/, "\\\\&", str); return str; }
function alen(a,i,k) { for(i in a) k++; return k; }

{
	L[NR]=$0;
}

# \043 is the '#' symbol in octal
# /^#d /: '#d MACRO,VALUE' is a macro definition. Store MACRO and VALUE
# !/^#d /: In each non-macro line, substitute MACRO with VALUE
/^\043d /  {
	split(L[NR], SPLIT, ",");
	MACROS[lgsub("^#d ", "", esc_str(SPLIT[1]))]=esc_str(SPLIT[2]);
	L[NR]="ERASE";
}
!/^\043d / {
	for(m in MACROS){
		L[NR]=lgsub(m, MACROS[m], L[NR]);
	};
}

# Filter macros definitions out
END {
	len=alen(L);
	c=1;
	while(c <= len){
		if(L[c] != "ERASE") {
			print L[c];
		}
		c++;
	}
}
