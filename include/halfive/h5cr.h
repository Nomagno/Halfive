#ifndef H5COROUTE_H
#define H5COROUTE_H

#define H5CR_YIELD(n) do { frame->pc +=1; return n; } while(0)
#define H5CR_RETURN(n) do { frame->pc = 0; H5CR_Reset(frame->data); return n; } while(0)

typedef struct {
	unsigned pc;
	void *data;
} H5CR_Frame;

void H5CR_Reset(void *data);
#endif
