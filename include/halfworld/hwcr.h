#ifndef HWCOROUTE_H
#define HWCOROUTE_H

#define HWCR_YIELD(n) frame->pc +=1; return n;
#define HWCR_RETURN(n) frame->pc = 0; HWCR_Reset(frame->data); return n;

typedef struct {
	unsigned pc;
	void *data;
} HWCR_Frame;

void HWCR_Reset(void *data);
#endif
