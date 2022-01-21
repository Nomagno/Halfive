/*
Copyright Nomagno 2021

Redistributions, modified or unmodified, in whole or in part, must retain
applicable notices of copyright or other legal privilege, these conditions, and
the following license terms and disclaimer.  Subject to these conditions, each
holder of copyright or other legal privileges, author or assembler, and
contributor of this work, henceforth "licensor", hereby grants to any person
who obtains a copy of this work in any form:

1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
and/or otherwise deal in the licensed material without restriction.

2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
patent license to make, have made, provide, transfer, import, use, and/or
otherwise deal in the licensed material without restriction, for any and all
patents held by such licensor and necessarily infringed by the form of the work
upon distribution of that licensor's contribution to the work under the terms
of this license.

NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
WORK.*/

#include <halfworld/hwreq.h>
#include <halfworld/hwdoc/hwdoc.h>

int HWDOC_Parse(const unsigned char *input, int tok_size, HWDOC_Token *toks);

int hwdoc_parse(const unsigned char *input, int tok_size, HWDOC_Token *toks)
{
	int i = 0,           /*Character iterator var*/
	    j = 0,           /*Token iterator var*/
	    t = 0,           /*Temporary storate for iterator vars*/
	    errno = 0,       /*Store error number*/
	    c = 1,           /*Continue boolean*/
	    itr = 0,         /*Indentation track boolean;*/
	    curr_indent = 0, /*Store current indentation level*/
	    last_parent[10] = {
		0} /*Index of tokens and their indentation levels*/;

	toks[0].type = SEC;       /*First token always section*/
	toks[0].string_start = 0; /*Set start of first token*/
	while (c) {
		switch (input[i]) {
		case 0x20: /*Space always occurs before VAL token*/
			t = j;
			j += 1;
			while ((t >= 0) && (toks[t].type == VAL))
				t -= 1;     /*Look for closest key*/
			toks[j].type = VAL; /*Set type of current token*/
			toks[j - 1].string_end =
			    i - 1; /*Set end of previous token*/
			toks[j].string_start =
			    i + 1;          /*Set start of current token*/
			toks[j].parent = t; /*Set closest key as */
			break;
		case 0x0A: /*Falltrough newline to htab*/
		case 0x09: /*Only two separator patterns are valid:
			    - Single newline FOO\nBAR,
			    - Single newline followed by any number of htabs
			    FOO\n\t\t...BAR*/
			if (input[i + 1] ==
			    0x09) /*Not last separator in sequence, abort*/
				break;
			curr_indent = 0;
			t = i;
			j += 1;
			while ((t >= 0) && ((itr = (input[t] == 0x09)) ||
					    (input[t] == 0x0A))) {
				curr_indent += itr;
				t -= 1; /*Look for closest non-separator*/
			}
			last_parent[curr_indent] =
			    j; /*Annotate this token as last one on this indent
				  level*/
			toks[j].parent =
			    last_parent[(curr_indent == 0)
					    ? 0
					    : curr_indent -
						  1] /*Get parent token*/;
			toks[j - 1].string_end =
			    t; /*Set end of previous token*/
			toks[j].string_start = i + 1;
			toks[j].type = (input[i + 1] == 0x5F) ? SEC : KEY;
			break;
		case 0x00:
			toks[j].string_end = i - 1; /*Terminate previous token*/
			errno = j; /*Return number of tokens read*/
			c = 0;     /*Stop execution*/
			break;
		default:
			break;
		}
		i += 1;
		if (j >= tok_size) {
			errno = -1; /*Insufficient tokens error*/
			c = 0;      /*Stop execution*/
		}
	}
	return errno;
}

/*
EXAMPLE OF VALID HWDOC

_ONE\n\tkey val1 val2 val3\n\tkey2 val1 val2 val3\n\t_SUBONE\n\t\tsubkey val0
val1 val2
*/
