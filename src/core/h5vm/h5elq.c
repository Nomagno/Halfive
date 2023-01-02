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

#include <halfive/h5req.h>
#include <halfive/h5stdlib.h>
#include <halfive/h5vm/h5elq.h>
#include <halfive/h5vm/h5vm.h>

#ifdef H5ELQ_DEBUG
#include <stdio.h> /*Debug*/
#endif

#define ISUPPERCASEHEXDIGIT(x)                                                 \
    (x == '0' || x == '1' || x == '2' || x == '3' || x == '4' || x == '5' ||   \
     x == '6' || x == '7' || x == '8' || x == '9' || x == 'A' || x == 'B' ||   \
     x == 'C' || x == 'D' || x == 'E' || x == 'F')

#define ISVARIABLEDIGIT(x)                                                     \
    (x == 'a' || x == 'b' || x == 'c' || x == 'd' || x == 'e' || x == 'f' ||   \
     x == 'g' || x == 'h' || x == 'i' || x == 'j' || x == 'k' || x == 'l' ||   \
     x == 'm' || x == 'n' || x == 'o' || x == 'p' || x == 'q' || x == 'r' ||   \
     x == 's' || x == 't' || x == 'u' || x == 'v' || x == 'w' || x == 'x' ||   \
     x == 'y' || x == 'z' || x == '?')

#define CMPSTR(x, str) (h5strcmp(x, str) == 0)

#define ISSTANDARDSYNTAX(x)                                                    \
    (CMPSTR(x, "car") || CMPSTR(x, "cdr") || CMPSTR(x, "cons") ||              \
     CMPSTR(x, "define") || CMPSTR(x, "assign") || CMPSTR(x, "nil?") ||        \
     CMPSTR(x, "atom?") || CMPSTR(x, "proc?") || CMPSTR(x, "eq?") ||           \
     CMPSTR(x, "num?") || CMPSTR(x, "add") || CMPSTR(x, "sub") ||              \
     CMPSTR(x, "and") || CMPSTR(x, "or") || CMPSTR(x, "xor") ||                \
     CMPSTR(x, "shift") || CMPSTR(x, "set") || CMPSTR(x, "halt") ||            \
     CMPSTR(x, "if") || CMPSTR(x, "begin") || CMPSTR(x, "lambda") ||           \
     CMPSTR(x, "apply"))

#define GETSTANDARDSYNTAX(x, y)                                                \
    do {                                                                       \
	if (CMPSTR(x, "car")) {                                                \
	    y = ELQ_PROC_CAR;                                                  \
	} else if (CMPSTR(x, "begin")) {                                       \
	    y = ELQ_SYNTAX_BEGIN;                                              \
	} else if (CMPSTR(x, "cdr")) {                                         \
	    y = ELQ_PROC_CDR;                                                  \
	} else if (CMPSTR(x, "cons")) {                                        \
	    y = ELQ_PROC_CONS;                                                 \
	} else if (CMPSTR(x, "nil?")) {                                        \
	    y = ELQ_PROC_ISNIL;                                                \
	} else if (CMPSTR(x, "num?")) {                                        \
	    y = ELQ_PROC_ISNUM;                                                \
	} else if (CMPSTR(x, "atom?")) {                                       \
	    y = ELQ_PROC_ISATOM;                                               \
	} else if (CMPSTR(x, "proc?")) {                                       \
	    y = ELQ_PROC_ISPROC;                                               \
	} else if (CMPSTR(x, "eq?")) {                                         \
	    y = ELQ_PROC_ISEQ;                                                 \
	} else if (CMPSTR(x, "apply")) {                                       \
	    y = ELQ_PROC_APPLY;                                                \
	} else if (CMPSTR(x, "add")) {                                         \
	    y = ELQ_PROC_ADD;                                                  \
	} else if (CMPSTR(x, "sub")) {                                         \
	    y = ELQ_PROC_SUB;                                                  \
	} else if (CMPSTR(x, "and")) {                                         \
	    y = ELQ_PROC_AND;                                                  \
	} else if (CMPSTR(x, "or")) {                                          \
	    y = ELQ_PROC_OR;                                                   \
	} else if (CMPSTR(x, "xor")) {                                         \
	    y = ELQ_PROC_XOR;                                                  \
	} else if (CMPSTR(x, "shift")) {                                       \
	    y = ELQ_PROC_SHIFT;                                                \
	} else if (CMPSTR(x, "set")) {                                         \
	    y = ELQ_PROC_SET;                                                  \
	} else if (CMPSTR(x, "halt")) {                                        \
	    y = ELQ_PROC_HALT;                                                 \
	} else if (CMPSTR(x, "lambda")) {                                      \
	    y = ELQ_SYNTAX_LAMBDA;                                             \
	} else if (CMPSTR(x, "define")) {                                      \
	    y = ELQ_SYNTAX_DEFINE;                                             \
	} else if (CMPSTR(x, "assign")) {                                      \
	    y = ELQ_SYNTAX_ASSIGN;                                             \
	} else if (CMPSTR(x, "if")) {                                          \
	    y = ELQ_SYNTAX_IF;                                                 \
	}                                                                      \
    } while (0)

#define _DIR_RIGHT 1
#define _DIR_LEFT 1
H5Elq_Node *H5Elq_appendNode(H5Elq_Node *parent, _Bool direction,
			     H5Elq_Node child, H5Elq_NodeHeap *heap)
{
    heap->mempool[heap->poolindex] = child;
    if (parent != NULL) {
	if (direction) {
	    parent->right = &heap->mempool[heap->poolindex];
	}
	if (!direction) {
	    parent->left = &heap->mempool[heap->poolindex];
	}
	heap->mempool[heap->poolindex].parent = parent;
    } else {
	heap->mempool[heap->poolindex].parent = NULL;
    }
    heap->poolindex += 1;
    return &heap->mempool[heap->poolindex - 1];
}

#define STACKMEM 64
typedef struct {
    size_t index;
    H5Elq_Node *data[STACKMEM];
} H5Elq_Stack;
#undef STACKMEM

void H5Elq_push(H5Elq_Stack *stack, H5Elq_Node *val)
{
    stack->index += 1;
    stack->data[stack->index] = val;
    return;
}

H5Elq_Node *H5Elq_pop(H5Elq_Stack *stack)
{
    H5Elq_Node *retval = stack->data[stack->index];
    stack->index -= 1;
    return retval;
}

/*
sed command to sanizitize a program so the parser accepts it:

sed 's/(/ (/g;
s/[[:space:]]*[[:space:]]/ /g;
s/^ //g;
s/ )/)/g;'
*/
H5Elq_Node *H5Elq_parse(char *in, H5Elq_NodeHeap *nodeheap)
{
    H5Elq_Stack forkstack = {0};
    H5Elq_Node *root = H5Elq_appendNode(
	NULL, _DIR_LEFT, (H5Elq_Node){.type = ELQ_EMPTY}, nodeheap);
    H5Elq_Node *currnode = root;
    char tmpstring[24];
    unsigned k;

    char lastchar;

#ifdef H5ELQ_DEBUG
    unsigned i = 0; /*DEBUG*/
#endif
    while (*in != '\0') {
	switch (*in) {
	case ' ':
	case '(':
	    if (lastchar == '(' || lastchar == ' ') {
		H5Elq_push(&forkstack, currnode->parent);
	    }
	    currnode = H5Elq_appendNode(
		currnode, _DIR_LEFT, (H5Elq_Node){.type = ELQ_EMPTY}, nodeheap);
	    break;
	case ')':
	    currnode->type = ELQ_NIL;
	    currnode =
		H5Elq_appendNode(H5Elq_pop(&forkstack), _DIR_RIGHT,
				 (H5Elq_Node){.type = ELQ_EMPTY}, nodeheap);
	    break;
	default:
	    if (ISUPPERCASEHEXDIGIT(*in)) {
		currnode->type = ELQ_INT;
		if (ISUPPERCASEHEXDIGIT(*(in + 1))) {
		    currnode->valScalar =
			h5strtoul((char[]){*in, *(in + 1), 0}, NULL, 16);
		    in += 1;
		} else {
		    currnode->valScalar = h5strtoul((char[]){*in, 0}, NULL, 16);
		}
	    } else if (ISVARIABLEDIGIT(*in)) {
		k = 0;
		while (ISVARIABLEDIGIT(*in)) {
		    tmpstring[k] = *in;
		    in += 1;
		    k += 1;
		}
		tmpstring[k] = '\0';
		if (ISSTANDARDSYNTAX(tmpstring)) {
		    currnode->type = ELQ_PROC;
		    GETSTANDARDSYNTAX(tmpstring, currnode->valScalar);
		} else {
		    currnode->type = ELQ_VAR;
		    for (int j = 0; j < 24; j++) {
			currnode->valSymbol[j] = tmpstring[j];
		    }
		}
		in -= 1;
	    } else if (*in == '%') {
		currnode->type = ELQ_LIT_NIL;
	    }
	    currnode =
		H5Elq_appendNode(currnode->parent, _DIR_RIGHT,
				 (H5Elq_Node){.type = ELQ_EMPTY}, nodeheap);
	    currnode =
		H5Elq_appendNode(currnode->parent, _DIR_RIGHT,
				 (H5Elq_Node){.type = ELQ_EMPTY}, nodeheap);
	    break;
	}
#ifdef H5ELQ_DEBUG
	printf("COUNTER: %u\nCHARACTER: %c\n LASTCHAR: %c\n\n", i, *in,
	       lastchar); /*DEBUG*/
	i += 1;           /*DEBUG*/
#endif

	lastchar = *in;
	in += 1;
    }
    return root;
}
#undef _DIR_LEFT
#undef _DIR_RIGHT

void H5Elq_generateCode(const H5Elq_Node *ast, H5VM_GeneralMemory *program)
{
    /*To be implemented*/
}

#ifdef H5ELQ_DEBUG
int main(void)
{
    char in[1024];
    printf("Input S-Expression:\n");
    fgets(in, sizeof(in), stdin);

    H5Elq_NodeHeap nodeheap = {0};
    H5Elq_Node *root = H5Elq_parse(in, &nodeheap);

    for (unsigned i = 0; i < 32; i++) {
	printf("INDEX %u: %X, %s, %u, %p, PARENT: %p\n", i,
	       nodeheap.mempool[i].valScalar, nodeheap.mempool[i].valSymbol,
	       nodeheap.mempool[i].type, (void *)&nodeheap.mempool[i],
	       (void *)nodeheap.mempool[i].parent);
    }

    int c;
    printf("TYPE: %s - VAL: %X - STRING: %s - "
	   "- HASPARENT: %u - HASLEFT: %u - HASRIGHT: %u"
	   "- CYCLIC: %u"
	   "- PPTR: %p - LPTR: %p - RPTR: %p\n",
	   (root->type == ELQ_NIL)
	       ? "NIL"
	       : ((root->type == ELQ_INT)
		      ? "INT"
		      : ((root->type == ELQ_VAR)
			     ? "VAR"
			     : ((root->type == ELQ_PROC) ? "PROC" : "EMPTY"))),
	   root->valScalar, root->valSymbol, (root->parent != NULL),
	   (root->left != NULL), (root->right != NULL),
	   root->parent != NULL &&
	       (root->parent == root->left || root->parent == root->right),
	   (void *)root->parent, (void *)root->left, (void *)root->right);
    while ((c = getchar()) != EOF) {
	printf(
	    "TYPE: %s - VAL: %X - STRING: %s - "
	    "- HASPARENT: %u - HASLEFT: %u - HASRIGHT: %u"
	    "- CYCLIC: %u"
	    "- PPTR: %p - LPTR: %p - RPTR: %p\n",
	    (root->type == ELQ_NIL)
		? "NIL"
		: ((root->type == ELQ_INT)
		       ? "INT"
		       : ((root->type == ELQ_VAR)
			      ? "VAR"
			      : ((root->type == ELQ_PROC) ? "PROC" : "EMPTY"))),
	    root->valScalar, root->valSymbol, (root->parent != NULL),
	    (root->left != NULL), (root->right != NULL),
	    root->parent != NULL &&
		(root->parent == root->left || root->parent == root->right),
	    (void *)root->parent, (void *)root->left, (void *)root->right);
	switch (c) {
	case 'P':
	    root = root->parent;
	    break;
	case 'L':
	    root = root->left;
	    break;
	case 'R':
	    root = root->right;
	    break;
	default:
	    break;
	}
    }
}
#endif

/*Execution example:
(... ((lambda (x) (add (car (cons 4 7)) x)) 6) ...)

allocate 6;
create stack frame;
set var x to address of 6;
JUMP to anonymous procedure;
	allocate 4;
	allocate 7;
	create stack frame;
	set var x to address of 4;
	set var y to address 7;
	JUMP to cons;
		allocate (4 . 7);
		set return field of previous frame to address of (4 . 7);
		RETURN to anonymous procedure;
	destroy stack frame;
	destroy 4;
	destroy 7;
	allocate copy of (4 . 7);
	destroy return field object (4 . 7);
	create stack frame;
	set var x to address of (4 . 7);
	JUMP to car;
		allocate 4;
		set return field of previous stack frame to address of 4;
		RETURN to anonymous procedure;
	destroy stack frame;
	allocate copy of 4;
	destroy return field object 4;
	destroy (4 . 7);
	allocate stack frame;
	set var x to address of 4;
	set var y to address of 6;
	JUMP to add;
		allocate 10;
		set previous stack frame return field to address of 10;
		RETURN to anonymous procedure;
	destroy 4;
	destroy 6;
	allocate copy of 10;
	destroy return field value 10;
	set previous stack frame return field to address of 10;
	RETURN to [CALLER]
[CALLER HAPPILY CONTINUES WITH ITS EXECUTION]
*/
