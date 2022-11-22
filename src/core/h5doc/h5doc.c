/*
Copyright Nomagno 2021, 2022

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

#include <halfive/h5doc/h5doc.h>
#include <halfive/h5req.h>

#define TOP_LAYER UINT16_MAX
unsigned H5DOC_Parse(const char *input, unsigned tok_size, H5DOC_Token *toks);

struct H5DOC_Stack {
	size_t stackptr;
	size_t parents[16]; /*Keep track of parents for 'childnum' stat
	                      (if you're indenting more than 16 levels
	                      deep please redesign your document format)*/
};

void H5DOC_Push(struct H5DOC_Stack *stack, size_t data) {
	if (stack->stackptr == SIZE_MAX) {
		stack->stackptr = 0;
	} else if (stack->stackptr < ((sizeof stack->parents) - 1)) {
		stack->stackptr++;
	}
	stack->parents[stack->stackptr] = data;
	return;
}

size_t H5DOC_Pop(struct H5DOC_Stack *stack) {
	if (stack->stackptr != 0) {
		stack->stackptr--;
		return stack->parents[stack->stackptr];
	} else {
		stack->stackptr = SIZE_MAX; /*Uninitialize the stack pointer*/
		return SIZE_MAX;
	}
}

unsigned H5DOC_IncreaseChildnum(struct H5DOC_Stack *stack, unsigned tok_size, H5DOC_Token *toks){
	if (stack->stackptr != SIZE_MAX) {
		for (size_t i = stack->stackptr; 1; i--){
			if (stack->parents[i] < tok_size){
				toks[stack->parents[i]].childnum += 1;
			} else {
				return 1; /*Out of bounds*/
			}
			if (i == 0) return 0; /*Successful*/
		}
	} else {
		return 0; /*Stackptr is uninitialized*/
	}
	return 1; /*Impossible control path*/

}

unsigned H5DOC_Parse(const char *input, unsigned tok_size, H5DOC_Token *toks) {
	struct H5DOC_Stack stack = (struct H5DOC_Stack){SIZE_MAX, {0}};
	int previndent = -1;
	int currindent = 0;
	_Bool endreached = 0;
	H5DOC_Type prevtype = H5DOC_SEC;
	int returnval = 0;

	size_t i = 0, /*String iterator*/
	       j = 0; /*Token iterator*/
	while (j < tok_size) {
		switch (input[i]) {
		case 0x0A: /*Newline, delimits separation between different tables or different key/value sets*/
			currindent = 0; /*Rotate indentation*/
			prevtype = H5DOC_SEC; /*Base case*/
			break;

		case 0x20: /*Space, delimits keys and values*/
			prevtype = H5DOC_KEY; /*If there's a space, well, for all intents and purposes last one was a key*/
			break;

		case 0x09: /*Tab, determines parent-child relationships*/
			currindent += 1;
			break;

		case '\0': /*End of string, delimits the last token*/
			endreached = 1;
			break;

		case 0x5F: /*Underscore, at the start of the token it means its a table*/
			if (currindent > previndent) { /*If indentation is strictly greater, push self as next child*/
				returnval = H5DOC_IncreaseChildnum(&stack, tok_size, toks);
				H5DOC_Push(&stack, j);
			} else if (currindent < previndent) {
				for (int i = 0; i < (previndent - currindent); i++){
					H5DOC_Pop(&stack); /*Indentation is smaller, so pop until last known parent is
					                     reached, then push self*/
				}
				returnval = H5DOC_IncreaseChildnum(&stack, tok_size, toks);
				H5DOC_Push(&stack, j);
			} else if (currindent == previndent) {
				H5DOC_Pop(&stack); /*Indentation is same, so pop once, then push self*/
				returnval = H5DOC_IncreaseChildnum(&stack, tok_size, toks);
				H5DOC_Push(&stack, j);
			}
			previndent = currindent;

			toks[j].type = H5DOC_SEC; /*Section*/
			prevtype = H5DOC_SEC;
			toks[j].string_start = i;
			while (1) { /*Skip until string end*/
				if (input[i+1] == '\n') {
					break;
				} else if (input[i+1] == '\0') {
					endreached = 1;
					break;
				} else {
					i++;
				}
			}
			toks[j].string_end = i;
			j++;
			break;

		default: /*Any other character, at the start of the token it means its a key or a value*/
			if (prevtype != H5DOC_KEY) { /*Last one was not a key (was a section)*/
				toks[j].type = H5DOC_KEY;
				prevtype = H5DOC_KEY;

				if (currindent > previndent) { /*If indentation is strictly greater, push self as next child*/
					returnval = H5DOC_IncreaseChildnum(&stack, tok_size, toks);
					H5DOC_Push(&stack, j);
				} else if (currindent < previndent) {
					for (int i = 0; i < (previndent - currindent); i++){
						H5DOC_Pop(&stack); /*Indentation is smaller, so pop until last known parent is
						                     reached, then push self*/
					}
					returnval = H5DOC_IncreaseChildnum(&stack, tok_size, toks);
					H5DOC_Push(&stack, j);
				} else if (currindent == previndent) {
					H5DOC_Pop(&stack); /*Indentation is same, so pop once, then push self*/
					returnval = H5DOC_IncreaseChildnum(&stack, tok_size, toks);
					H5DOC_Push(&stack, j);
				}

			} else if (prevtype == H5DOC_KEY) { /*Last one was a key*/
				returnval = H5DOC_IncreaseChildnum(&stack, tok_size, toks); /*Increase key parent count*/
				toks[j].type = H5DOC_VAL;
				prevtype = H5DOC_VAL;
			}
			previndent = currindent;

			toks[j].string_start = i;
			while (1) { /*Skip until string end*/
				if (input[i+1] == ' ' || input[i+1] == '\n') {
					break;
				} else if (input[i+1] == '\0') {
					endreached = 1;
					break;
				} else {
					i++;
				}
			}
			toks[j].string_end = i;
			j++;
			break;
		}
		i++;
		if (endreached){
			break;
		}
	}
	if (endreached) return j; /*Number of tokens read*/
	else return 0; /*Insufficient space*/
}

/*
#include <stdio.h>
#include <string.h>

int main(void){
	char doc[256] = "_ONE\n\tkey val1 val2 val3\n\tkey2 val1 val2 val3\n\t_SUBONE\n\t\tsubkey val0 val1 val2";
	H5DOC_Token toks[128] = {0};
	unsigned tokensread = H5DOC_Parse(doc, sizeof toks, toks);
	for (unsigned long i = 0; i < tokensread; i++){
		char substr[20] = {0};
		strncpy(substr, doc+(toks[i].string_start), toks[i].string_end - toks[i].string_start + 1);
		printf("TOK %lu:\n\tSTR...: [%s]\n\tTYPE: %s\n\t"
		       "START.: %lu\n\tEND...: %lu\n\t"
		       "CHILDS: %u\n", i, substr,
		       (toks[i].type == H5DOC_SEC) ? "SECTION" : ((toks[i].type == H5DOC_KEY) ? "KEY" : "VALUE"),
		       toks[i].string_start, toks[i].string_end,
		       toks[i].childnum);
	}
	return 0;
}
*/

/*
EXAMPLE OF VALID H5DOC

_ONE\n\tkey val1 val2 val3\n\tkey2 val1 val2 val3\n\t_SUBONE\n\t\tsubkey val0 val1 val2
*/
