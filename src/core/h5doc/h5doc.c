/*
Copyright Nomagno 2021, 2022, 2023

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

#include <halfive/h5req.h>
#include <halfive/h5doc/h5doc.h>

#define TOP_LAYER UINT16_MAX
unsigned H5DOC_Parse(const char *input, unsigned tok_size, H5DOC_Token *toks);

struct H5DOC_Stack {
	size_t stackptr;
	size_t parents[16]; /*Keep track of parents for 'childnum' stat
			  (if you're indenting more than 16 levels
			  deep please redesign your document format)*/
};

void H5DOC_push(struct H5DOC_Stack *stack, size_t data)
{
	if (stack->stackptr == SIZE_MAX) {
		stack->stackptr = 0;
	} else if (stack->stackptr < ((sizeof stack->parents) - 1)) {
		stack->stackptr++;
	}
	stack->parents[stack->stackptr] = data;
	return;
}

size_t H5DOC_pop(struct H5DOC_Stack *stack)
{
	if (stack->stackptr != 0) {
		stack->stackptr--;
		return stack->parents[stack->stackptr];
	} else {
		stack->stackptr = SIZE_MAX; /*Uninitialize the stack pointer*/
		return SIZE_MAX;
	}
}

unsigned H5DOC_increaseChildnum(
	struct H5DOC_Stack *stack, unsigned tok_size, H5DOC_Token *toks)
{
	if (stack->stackptr != SIZE_MAX) {
		for (size_t i = stack->stackptr; 1; i--) {
			if (stack->parents[i] < tok_size) {
				toks[stack->parents[i]].childnum += 1;
			} else {
				return 1; /*Out of bounds*/
			}
			if (i == 0)
				return 0; /*Successful*/
		}
	} else {
		return 0; /*Stackptr is uninitialized*/
	}
	return 1; /*Impossible control path*/
}

unsigned H5DOC_parse(const char *input, unsigned tok_size, H5DOC_Token *toks)
{
	struct H5DOC_Stack stack = (struct H5DOC_Stack){SIZE_MAX, {0}};
	int previndent			 = -1;
	int currindent			 = 0;
	_Bool endreached		 = 0;
	H5DOC_Type prevtype		 = H5DOC_SEC;

	size_t i = 0, /*String iterator*/
		j	 = 0; /*Token iterator*/
	while (j < tok_size) {
		switch (input[i]) {
		case 0x0A: /*Newline, delimits separation between different tables or
				  different key/value sets*/
			currindent = 0;			/*Rotate indentation*/
			prevtype   = H5DOC_SEC; /*Base case*/
			break;

		case 0x20:				  /*Space, delimits keys and values*/
			prevtype = H5DOC_KEY; /*If there's a space, well, for all intents
						 and purposes last one was a key*/
			break;

		case 0x09: /*Tab, determines parent-child relationships*/
			currindent += 1;
			break;

		case '\0': /*End of string, delimits the last token*/
			endreached = 1;
			break;

		case 0x5F: /*Underscore, at the start of the token it means its a
				  table*/
			if (currindent > previndent) { /*If indentation is strictly greater,
							  push self as next child*/
				H5DOC_increaseChildnum(&stack, tok_size, toks);
				H5DOC_push(&stack, j);
			} else if (currindent < previndent) {
				for (int i = 0; i < (previndent - currindent); i++) {
					H5DOC_pop(
						&stack); /*Indentation is smaller, so pop until last
							   known parent is reached, then push self*/
				}
				H5DOC_increaseChildnum(&stack, tok_size, toks);
				H5DOC_push(&stack, j);
			} else if (currindent == previndent) {
				H5DOC_pop(&stack); /*Indentation is same, so pop once, then push
							  self*/
				H5DOC_increaseChildnum(&stack, tok_size, toks);
				H5DOC_push(&stack, j);
			}
			previndent = currindent;

			toks[j].type		 = H5DOC_SEC; /*Section*/
			prevtype			 = H5DOC_SEC;
			toks[j].string_start = i;
			while (1) { /*Skip until string end*/
				if (input[i + 1] == '\n') {
					break;
				} else if (input[i + 1] == '\0') {
					endreached = 1;
					break;
				} else {
					i++;
				}
			}
			toks[j].string_end = i;
			j++;
			break;

		default: /*Any other character, at the start of the token it means its a
				key or a value*/
			if (prevtype !=
				H5DOC_KEY) { /*Last one was not a key (was a section)*/
				toks[j].type = H5DOC_KEY;
				prevtype	 = H5DOC_KEY;

				if (currindent > previndent) { /*If indentation is strictly
										  greater, push self as next child*/
					H5DOC_increaseChildnum(&stack, tok_size, toks);
					H5DOC_push(&stack, j);
				} else if (currindent < previndent) {
					for (int i = 0; i < (previndent - currindent); i++) {
						H5DOC_pop(
							&stack); /*Indentation is smaller, so pop until last
								   known parent is reached, then push self*/
					}
					H5DOC_increaseChildnum(&stack, tok_size, toks);
					H5DOC_push(&stack, j);
				} else if (currindent == previndent) {
					H5DOC_pop(&stack); /*Indentation is same, so pop once, then
							  push self*/
					H5DOC_increaseChildnum(&stack, tok_size, toks);
					H5DOC_push(&stack, j);
				}

			} else if (prevtype == H5DOC_KEY) { /*Last one was a key*/
				H5DOC_increaseChildnum(
					&stack, tok_size, toks); /*Increase key parent count*/
				toks[j].type = H5DOC_VAL;
				prevtype	 = H5DOC_VAL;
			}
			previndent = currindent;

			toks[j].string_start = i;
			while (1) { /*Skip until string end*/
				if (input[i + 1] == ' ' || input[i + 1] == '\n') {
					break;
				} else if (input[i + 1] == '\0') {
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
		if (endreached) {
			break;
		}
	}
	if (endreached)
		return j; /*Number of tokens read*/
	else
		return 0; /*Insufficient space*/
}

/*
#include <halfive/code_setup.h>
#include <stdio.h>
#include <string.h>

int main(void){
	char doc[256] = "_ONE\n\tkey val1 val2 val3\n\tkey2 val1 val2
val3\n\t_SUBONE\n\t\tsubkey val0 val1 val2"; H5DOC_Token toks[128] = {0};
	unsigned tokensread = H5DOC_Parse(doc, sizeof toks, toks);
	for (unsigned long i = 0; i < tokensread; i++){
		char substr[20] = {0};
		strncpy(substr, doc+(toks[i].string_start), toks[i].string_end -
toks[i].string_start + 1); maybe_printf("TOK %lu:\n\tSTR...: [%s]\n\tTYPE: %s\n\t"
			   "START.: %lu\n\tEND...: %lu\n\t"
			   "CHILDS: %u\n", i, substr,
			   (toks[i].type == H5DOC_SEC) ? "SECTION" : ((toks[i].type
== H5DOC_KEY) ? "KEY" : "VALUE"), toks[i].string_start, toks[i].string_end,
			   toks[i].childnum);
	}
	return 0;
}
*/

/*
EXAMPLE OF VALID H5DOC

_ONE\n\tkey val1 val2 val3\n\tkey2 val1 val2 val3\n\t_SUBONE\n\t\tsubkey val0
val1 val2
*/
