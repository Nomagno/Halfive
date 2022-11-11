#ifndef H5REQ_H
#define H5REQ_H

#ifdef __cplusplus
	#define _Bool bool
#endif



#include <limits.h>
#include <stddef.h>
#include <stdint.h>

/*CHANGE RACENUM*/
#define RACENUM 4

#ifndef UINT8_MAX
#error uint8_t is required, sorry!
#endif

#ifndef UINT16_MAX
#error uint16_t is required, sorry!
#endif

#ifndef UINT32_MAX
#error uint32_t is required, sorry!
#endif

#define H5UCHAR_MAX UINT8_MAX
typedef uint8_t h5uchar;

#define H5UINT_MAX UINT16_MAX
typedef uint16_t h5uint;

#define H5ULONG_MAX UINT32_MAX
typedef uint32_t h5ulong;

#define H5UMAX_MAX UINTMAX_MAX
typedef uintmax_t h5umax;

#ifdef __cplusplus
	static int H5Req_const_one = 1;
	#define IS_LITTLE_ENDIAN (*(h5uchar *)&H5Req_const_one)
#else
	#define IS_LITTLE_ENDIAN (*(h5uchar *)&(h5uint){1})
#endif

#endif
