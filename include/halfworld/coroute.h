#ifndef COROUTE_H
#define COROUTE_H

#define START switch(stack->pc) {
#define END }
#define C(n) case n
#define DEF(n) default
#define DATA stack->data
#define YIELD(n) stack->pc +=1; return n;
#define RETURN(n) *stack = (Stack){0}; return n;

typedef struct {
	unsigned pc;
	void *data;
} Stack;
#endif
