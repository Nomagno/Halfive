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

/*Halfive Eloquent Lisp (designed to compile to H5VM) utilities*/

#ifndef H5ELQ_H
#define H5ELQ_H
#include <halfive/h5req.h>
#include <halfive/h5vm/h5vm.h>

enum H5Elq_Type {
	H5ELQ_EMPTY	  = 0,
	H5ELQ_NIL	  = 1,
	H5ELQ_INT	  = 2,
	H5ELQ_PROC	  = 3,
	H5ELQ_SYNTAX  = 4,
	H5ELQ_VAR	  = 5,
	H5ELQ_LIT_NIL = 6,
};

enum H5Elq_Proc_Syntax {
	H5ELQ_PROC_CAR		= 0,
	H5ELQ_PROC_CDR		= 1,
	H5ELQ_PROC_CONS		= 2,
	H5ELQ_PROC_ISNIL	= 3,
	H5ELQ_PROC_ISNUM	= 4,
	H5ELQ_PROC_ISATOM	= 5,
	H5ELQ_PROC_ISPROC	= 6,
	H5ELQ_PROC_ISEQ		= 7,
	H5ELQ_PROC_ADD		= 8,
	H5ELQ_PROC_SUB		= 9,
	H5ELQ_PROC_AND		= 10,
	H5ELQ_PROC_OR		= 11,
	H5ELQ_PROC_XOR		= 12,
	H5ELQ_PROC_SHIFT	= 13,
	H5ELQ_PROC_SET		= 14,
	H5ELQ_PROC_HALT		= 15,
	H5ELQ_SYNTAX_IF		= 16,
	H5ELQ_SYNTAX_DEFINE = 17,
	H5ELQ_SYNTAX_BEGIN	= 18,
	H5ELQ_SYNTAX_LAMBDA = 19,
	H5ELQ_SYNTAX_ASSIGN = 20,
	H5ELQ_PROC_APPLY	= 21
};

typedef struct H5Elq_Node {
	enum H5Elq_Type type;
	h5uchar valScalar;
	char valSymbol[24];
	struct H5Elq_Node *parent;
	struct H5Elq_Node *left;
	struct H5Elq_Node *right;
} H5Elq_Node;

#define POOLMEM 1024
typedef struct {
	size_t poolindex;
	H5Elq_Node mempool[POOLMEM];
} H5Elq_NodeHeap;
#undef POOLMEM

H5Elq_Node *H5Elq_parse(const char *in, H5Elq_NodeHeap *nodeheap);
void H5Elq_generateCode(const H5Elq_Node *ast, H5VM_VirtualMachine *program);
#endif
