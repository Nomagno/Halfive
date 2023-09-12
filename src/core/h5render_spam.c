#ifndef H5RENDER_H
#error this must not be compiled by itself!
#endif

void H5Render_mapCharToSymbol(char inchar, char *outarray, size_t n) {
	switch (inchar) {
	case 'a':
		h5strncpy(outarray, "a", n-1);
		break;
	case 'b':
		h5strncpy(outarray, "b", n-1);
		break;
	case 'c':
		h5strncpy(outarray, "c", n-1);
		break;
	case 'd':
		h5strncpy(outarray, "d", n-1);
		break;
	case 'e':
		h5strncpy(outarray, "e", n-1);
		break;
	case 'f':
		h5strncpy(outarray, "f", n-1);
		break;
	case 'g':
		h5strncpy(outarray, "g", n-1);
		break;
	case 'h':
		h5strncpy(outarray, "h", n-1);
		break;
	case 'i':
		h5strncpy(outarray, "i", n-1);
		break;
	case 'j':
		h5strncpy(outarray, "j", n-1);
		break;
	case 'k':
		h5strncpy(outarray, "k", n-1);
		break;
	case 'l':
		h5strncpy(outarray, "l", n-1);
		break;
	case 'm':
		h5strncpy(outarray, "m", n-1);
		break;
	case 'n':
		h5strncpy(outarray, "n", n-1);
		break;
	case 'o':
		h5strncpy(outarray, "o", n-1);
		break;
	case 'p':
		h5strncpy(outarray, "p", n-1);
		break;
	case 'q':
		h5strncpy(outarray, "q", n-1);
		break;
	case 'r':
		h5strncpy(outarray, "r", n-1);
		break;
	case 's':
		h5strncpy(outarray, "s", n-1);
		break;
	case 't':
		h5strncpy(outarray, "t", n-1);
		break;
	case 'u':
		h5strncpy(outarray, "u", n-1);
		break;
	case 'v':
		h5strncpy(outarray, "v", n-1);
		break;
	case 'w':
		h5strncpy(outarray, "w", n-1);
		break;
	case 'x':
		h5strncpy(outarray, "x", n-1);
		break;
	case 'y':
		h5strncpy(outarray, "y", n-1);
		break;
	case 'z':
		h5strncpy(outarray, "z", n-1);
		break;
	case '0':
		h5strncpy(outarray, "zero", n-1);
		break;
	case '1':
		h5strncpy(outarray, "one", n-1);
		break;
	case '2':
		h5strncpy(outarray, "two", n-1);
		break;
	case '3':
		h5strncpy(outarray, "three", n-1);
		break;
	case '4':
		h5strncpy(outarray, "four", n-1);
		break;
	case '5':
		h5strncpy(outarray, "five", n-1);
		break;
	case '6':
		h5strncpy(outarray, "six", n-1);
		break;
	case '7':
		h5strncpy(outarray, "seven", n-1);
		break;
	case '8':
		h5strncpy(outarray, "eight", n-1);
		break;
	case '9':
		h5strncpy(outarray, "nine", n-1);
		break;
	case '#':
		h5strncpy(outarray, "hash", n-1);
		break;
	case ';':
		h5strncpy(outarray, "semicolon", n-1);
		break;
	case '=':
		h5strncpy(outarray, "equal", n-1);
		break;
	case '*':
		h5strncpy(outarray, "asterisk", n-1);
		break;
	case '_':
		h5strncpy(outarray, "underscore", n-1);
		break;
	case ',':
		h5strncpy(outarray, "comma", n-1);
		break;
	case '!':
		h5strncpy(outarray, "exclamation", n-1);
		break;
	case '?':
		h5strncpy(outarray, "question", n-1);
		break;
	case '[':
		h5strncpy(outarray, "bracket_l", n-1);
		break;
	case ']':
		h5strncpy(outarray, "bracket_r", n-1);
		break;
	case '(':
		h5strncpy(outarray, "paren_l", n-1);
		break;
	case ')':
		h5strncpy(outarray, "paren_r", n-1);
		break;
	case '<':
		h5strncpy(outarray, "lesser", n-1);
		break;
	case '>':
		h5strncpy(outarray, "greater", n-1);
		break;
	case '{':
		h5strncpy(outarray, "special1", n-1);
		break;
	case '}':
		h5strncpy(outarray, "special2", n-1);
		break;
	case '`':
		h5strncpy(outarray, "special3", n-1);
		break;
	case '&':
		h5strncpy(outarray, "special4", n-1);
		break;
	case '^':
		h5strncpy(outarray, "special5", n-1);
		break;
	case '+':
		h5strncpy(outarray, "special6", n-1);
		break;
	case '-':
		h5strncpy(outarray, "special7", n-1);
		break;
	case '\'':
		h5strncpy(outarray, "special8", n-1);
		break;
	case '%':
		h5strncpy(outarray, "red", n-1);
		break;
	case '\\':
		h5strncpy(outarray, "green", n-1);
		break;
	case '|':
		h5strncpy(outarray, "blue", n-1);
		break;
	case '/':
		h5strncpy(outarray, "black", n-1);
		break;
	case '~':
		h5strncpy(outarray, "white", n-1);
		break;
	case ' ':
		h5strncpy(outarray, "space", n-1);
		break;
			
	}
}
