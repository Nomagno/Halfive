#ifndef HWREQ_H
#define HWREQ_H

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

typedef uint8_t hwuchar;
typedef uint16_t hwuint;
typedef uint32_t hwulong;
typedef uintmax_t hwumax;

#ifdef __cplusplus
	static const int HWReq_const_one = 1;
	#define IS_LITTLE_ENDIAN (*(hwuchar *)&one)
#else
	#define IS_LITTLE_ENDIAN (*(hwuchar *)&(hwuint){1})
#endif

#endif
