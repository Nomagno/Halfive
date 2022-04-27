#ifndef HWCOROUTE_H
#define HWCOROUTE_H

#define HWCR_YIELD(n) frame->pc +=1; return n;
#define HWCR_RETURN(n) *frame = (HWCR_Frame){0}; return n;

typedef struct {
	unsigned pc;
	void *data;
} HWCR_Frame;
#endif
