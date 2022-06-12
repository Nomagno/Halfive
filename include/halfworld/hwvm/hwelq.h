#include <halfworld/hwreq.h>
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

enum HWElq_Type {
	ELQ_EMPTY = 0,
	ELQ_NIL = 1,
	ELQ_INT = 2,
	ELQ_PROC = 3,
	ELQ_SYNTAX = 4,
	ELQ_VAR = 5,
	ELQ_LIT_NIL = 6,
};

enum HWElq_Proc_Syntax {
	ELQ_PROC_CAR = 0,
	ELQ_PROC_CDR = 1,
	ELQ_PROC_CONS = 2,
	ELQ_PROC_BIND = 3,
	ELQ_PROC_ISNIL = 4,
	ELQ_PROC_ISATOM = 5,
	ELQ_PROC_ISPROC = 6,
	ELQ_PROC_ISEQ = 7,
	ELQ_PROC_ADD = 8,
	ELQ_PROC_SUB = 9,
	ELQ_PROC_AND = 10,
	ELQ_PROC_OR = 11,
	ELQ_PROC_XOR = 12,
	ELQ_PROC_ROT = 13,
	ELQ_PROC_SET = 14,
	ELQ_PROC_HALT = 15,
	ELQ_SYNTAX_IF = 16,
	ELQ_SYNTAX_BEGIN = 17,
	ELQ_SYNTAX_LAMBDA = 18
};

typedef struct HWElq_Node{
	enum HWElq_Type type;
	hwuchar valScalar;
	char valSymbol[24];
	struct HWElq_Node *parent;
	struct HWElq_Node *left;
	struct HWElq_Node *right;
} HWElq_Node;

#define STACKMEM 64
typedef struct {
	size_t index;
	HWElq_Node *data[STACKMEM];
} HWElq_Stack;
#undef STACKMEM

#define POOLMEM 1024
typedef struct {
	size_t poolindex;
	HWElq_Node mempool[POOLMEM];
} HWElq_NodeHeap;
#undef POOLMEM

HWElq_Node *HWElq_Parse(char *in, HWElq_NodeHeap *nodeheap);
