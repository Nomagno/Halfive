/*
Copyright Nomagno 2021, 2022, 2023

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

/*Halfive Eloquent Lisp (designed to compile to H5VM) utilities*/

#ifndef H5ELQ_H
#define H5ELQ_H
#include <halfive/h5req.h>
#include <halfive/h5vm/h5vm.h>

enum H5Elq_Type {
    ELQ_EMPTY = 0,
    ELQ_NIL = 1,
    ELQ_INT = 2,
    ELQ_PROC = 3,
    ELQ_SYNTAX = 4,
    ELQ_VAR = 5,
    ELQ_LIT_NIL = 6,
};

enum H5Elq_Proc_Syntax {
    ELQ_PROC_CAR = 0,
    ELQ_PROC_CDR = 1,
    ELQ_PROC_CONS = 2,
    ELQ_PROC_ISNIL = 3,
    ELQ_PROC_ISNUM = 4,
    ELQ_PROC_ISATOM = 5,
    ELQ_PROC_ISPROC = 6,
    ELQ_PROC_ISEQ = 7,
    ELQ_PROC_ADD = 8,
    ELQ_PROC_SUB = 9,
    ELQ_PROC_AND = 10,
    ELQ_PROC_OR = 11,
    ELQ_PROC_XOR = 12,
    ELQ_PROC_SHIFT = 13,
    ELQ_PROC_SET = 14,
    ELQ_PROC_HALT = 15,
    ELQ_SYNTAX_IF = 16,
    ELQ_SYNTAX_DEFINE = 17,
    ELQ_SYNTAX_BEGIN = 18,
    ELQ_SYNTAX_LAMBDA = 19,
    ELQ_SYNTAX_ASSIGN = 20,
    ELQ_PROC_APPLY = 21
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

H5Elq_Node *H5Elq_parse(char *in, H5Elq_NodeHeap *nodeheap);
void H5Elq_generateCode(const H5Elq_Node *ast, H5VM_GeneralMemory *program);
#endif
