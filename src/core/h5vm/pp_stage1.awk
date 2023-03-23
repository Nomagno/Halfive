function lsub(e,r,i){ sub(e,r,i); return i; }
function esc_str(str) { gsub(/[\\.^$(){}\[\]|*+?]/, "\\\\&", str); return str; }
BEGIN { FS=","; }
/^\043d /  { MACROS[lsub("^#d ","",esc_str($1))]=esc_str($2); }
!/^\043d / { l=$0; for(m in MACROS){ l=lsub(m, MACROS[m], l); }; print l; }
# \043 is the '#' symbol in octal
# /^#d /: '#d MACRO,VALUE' is a macro definition. Store MACRO and VALUE
# !/^#d /: In each non-macro line, substitute MACRO with VALUE
