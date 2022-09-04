#ifndef HWCOROUTE_H
#define HWCOROUTE_H

#define HWCR_YIELD(n) do { frame->pc +=1; return n; } while(0)
#define HWCR_RETURN(n) do { frame->pc = 0; HWCR_Reset(frame->data); return n; } while(0)

typedef struct {
	unsigned pc;
	void *data;
} HWCR_Frame;

void HWCR_Reset(void *data);
#endif
