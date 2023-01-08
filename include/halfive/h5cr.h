/*Halfive Coroutine utilities*/

#ifndef H5COROUTE_H
#define H5COROUTE_H

#define H5CR_START()     \
    if (frame->pc == 0){

#define H5CR_YIELD(n)    \
	frame->pc = __LINE__;\
	return n;            \
	} else if (frame->pc >= __LINE__){

#define H5CR_RETURN(n, resetter)\
	frame->pc = 0;              \
	resetter(frame->data);      \
	return n;

#define H5CR_END() \
	}


typedef struct {
    unsigned pc;
    void *data;
} H5CR_Frame;

#endif
