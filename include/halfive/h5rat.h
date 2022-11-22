/*
Copyright Nomagno 2021, 2022

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
#ifndef H5RAT_H
#define H5RAT_H

#include <halfive/h5req.h>

typedef struct {
	_Bool sign;
	h5uint num;
	h5uint denom;
} H5Rat_uint;

typedef struct {
	_Bool sign;
	h5ulong num;
	h5ulong denom;
} H5Rat_ulong;

typedef struct {
	_Bool sign;
	h5umax num;
	h5umax denom;
} H5Rat_umax;

h5uint H5Rat_uint_GCD(h5uint a, h5uint b);
h5ulong H5Rat_ulong_GCD(h5ulong a, h5ulong b);
h5umax H5Rat_umax_GCD(h5umax a, h5umax b);

h5uint H5Rat_uint_LCM(h5uint a, h5uint b);
h5ulong H5Rat_ulong_LCM(h5ulong a, h5ulong b);
h5umax H5Rat_umax_LCM(h5umax a, h5umax b);

void H5Rat_uint_XorSwap(h5uint *a, h5uint *b);
void H5Rat_ulong_XorSwap(h5ulong *a, h5ulong *b);
void H5Rat_umax_XorSwap(h5umax *a, h5umax *b);

H5Rat_uint H5Rat_uint_Simplify(H5Rat_uint a);
H5Rat_ulong H5Rat_ulong_Simplify(H5Rat_ulong a);
H5Rat_umax H5Rat_umax_Simplify(H5Rat_umax a);

void H5Rat_uint_Equate(H5Rat_uint *a, H5Rat_uint *b);
void H5Rat_ulong_Equate(H5Rat_ulong *a, H5Rat_ulong *b);
void H5Rat_umax_Equate(H5Rat_umax *a, H5Rat_umax *b);

_Bool H5Rat_uint_Compare(H5Rat_uint a, H5Rat_uint b);
_Bool H5Rat_ulong_Compare(H5Rat_ulong a, H5Rat_ulong b);
_Bool H5Rat_umax_Compare(H5Rat_umax a, H5Rat_umax b);

H5Rat_uint H5Rat_uint_Add(H5Rat_uint a, H5Rat_uint b);
H5Rat_ulong H5Rat_ulong_Add(H5Rat_ulong a, H5Rat_ulong b);
H5Rat_umax H5Rat_umax_Add(H5Rat_umax a, H5Rat_umax b);

H5Rat_uint H5Rat_uint_Product(H5Rat_uint a, H5Rat_uint b);
H5Rat_ulong H5Rat_ulong_Product(H5Rat_ulong a, H5Rat_ulong b);
H5Rat_umax H5Rat_umax_Product(H5Rat_umax a, H5Rat_umax b);


h5uint H5Rat_uint_toInt(H5Rat_uint a);
h5ulong H5Rat_ulong_toInt(H5Rat_ulong a);
h5umax H5Rat_umax_toInt(H5Rat_umax a);

H5Rat_umax H5Rat_UtoM(H5Rat_uint a);
H5Rat_umax H5Rat_LtoM(H5Rat_ulong a);

H5Rat_ulong H5Rat_UtoL(H5Rat_uint a);
H5Rat_ulong H5Rat_MtoL(H5Rat_umax a);

H5Rat_uint H5Rat_LtoU(H5Rat_ulong a);
H5Rat_uint H5Rat_MtoU(H5Rat_umax a);

#ifdef FLOATS_SUPPORTED
	float H5Rat_uint_toFloat(H5Rat_uint a);
	float H5Rat_ulong_toFloat(H5Rat_ulong a);
	float H5Rat_umax_toFloat(H5Rat_umax a);
#endif

#ifdef H5RAT_SHORTHAND
	#define H5_GCD_U H5Rat_uint_GCD
	#define H5_GCD_L H5Rat_GCD
	#define H5_GCD_M H5Rat_GCD

	#define H5_LCM_U H5Rat_uint_LCM
	#define H5_LCM_L H5Rat_LCM
	#define H5_LCM_M H5Rat_LCM

	#define H5_RSwap_U H5Rat_uint_XorSwap
	#define H5_RSwap_L H5Rat_ulong_XorSwap
	#define H5_RSwap_M H5Rat_umax_XorSwap

	#define H5_RSimp_U H5Rat_uint_Simplify
	#define H5_RSimp_L H5Rat_ulong_Simplify
	#define H5_RSimp_M H5Rat_umax_Simplify

	#define H5_REq_U H5Rat_uint_Equate
	#define H5_REq_L H5Rat_ulong_Equate
	#define H5_REq_M H5Rat_umax_Equate

	#define H5_RCmp_U H5Rat_uint_Compare
	#define H5_RCmp_L H5Rat_ulong_Compare
	#define H5_RCmp_M H5Rat_umax_Compare

	#define H5_RAdd_U H5Rat_uint_Add
	#define H5_RAdd_L H5Rat_ulong_Add
	#define H5_RAdd_M H5Rat_umax_Add

	#define H5_RProd_U H5Rat_uint_Product
	#define H5_RProd_L H5Rat_ulong_Product
	#define H5_RProd_M H5Rat_umax_Product

	#define H5_RInt_U H5Rat_uint_toInt
	#define H5_RInt_L H5Rat_ulong_toInt
	#define H5_RInt_M H5Rat_umax_toInt

	#ifdef FLOATS_SUPPORTED
		#define H5_RFloat_U H5Rat_uint_toFloat
		#define H5_RFloat_L H5Rat_ulong_toFloat
		#define H5_RFloat_M H5Rat_umax_toFloat
	#endif
#endif
#endif
