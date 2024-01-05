#ifndef H5REQ_H
#define H5REQ_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#define _Bool bool
#endif

/*printf is available on this system*/
#define PRINTF_AVAILABLE


/*Floats are supported on this system*/
#define FLOATS_SUPPORTED

/*MAXIMUM NUMBER OF PLAYERS*/
#define RACENUM 4

/*VM Simulation is supported on this system*/
#define H5PHY_VM_SIMULATE

#if (!defined(UINT8_MAX)) || (!defined(UINT16_MAX)) ||                        \
	(!defined(UINT32_MAX)) || (!defined(UINTMAX_MAX)) ||                      \
	(!defined(INT8_MAX)) || (!defined(INT16_MAX)) || (!defined(INT32_MAX)) || \
	(!defined(INTMAX_MAX)) || (!defined(SIZE_MAX))
#error error : \
One of the following types is missing; \
uint8_t, uint16_t, uint32_t, uintmax_t, \
int8_t, int16_t, int32_t, intmax_t, \
size_t, ssize_t
#endif

#define H5_ABS_INT(n)	((((h5slong)(n)) < 0) ? (-((h5slong)(n))) : ((h5slong)(n)))

#ifdef FLOATS_SUPPORTED
#include <float.h>
#define H5FLT_MAX FLT_MAX
#define H5FLT_MIN FLT_MIN
typedef float h5float;
#define H5_ABS_FLOAT(n)	((((h5float)(n)) < 0) ? (-((h5float)(n))) : ((h5float)(n)))
#define TLR						  0.01f /*Default tolerance for comparisons*/
#define _FLT_CMP(a, b, tolerance) (H5_ABS_FLOAT(a - b) <= tolerance)
#define H5FLOAT_CMP(a, b)			  _FLT_CMP(a, b, TLR)
#endif

#define H5UCHAR_MAX UINT8_MAX
typedef uint8_t h5uchar;

#define H5UINT_MAX UINT16_MAX
typedef uint16_t h5uint;

#define H5ULONG_MAX UINT32_MAX
typedef uint32_t h5ulong;

#define H5UMAX_MAX UINTMAX_MAX
typedef uintmax_t h5umax;

#define H5SCHAR_MAX INT8_MAX
#define H5SCHAR_MIN INT8_MIN
typedef int8_t h5schar;

#define H5SINT_MAX INT16_MAX
#define H5SINT_MIN INT16_MIN
typedef int16_t h5sint;

#define H5SLONG_MAX INT32_MAX
#define H5SLONG_MIN INT32_MIN
typedef int32_t h5slong;

#define H5SMAX_MAX INTMAX_MAX
#define H5SMAX_MIN INTMAX_MIN
typedef intmax_t h5smax;

#ifdef __cplusplus
static int H5Req_const_one = 1;
#define IS_LITTLE_ENDIAN (*(h5uchar *)&H5Req_const_one)
#else
#define IS_LITTLE_ENDIAN (*(h5uchar *)&(h5uint){1})
#endif

#define ELEMNUM(a) (sizeof(a)/sizeof(a[0]))
#define $(a) &((char[]){ #a }[0])

#endif
