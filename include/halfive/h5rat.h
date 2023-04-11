/*
Copyright Nomagno 2021, 2022, 2023

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

/*Halfive Rational number handling utilities*/

#ifndef H5RAT_H
#define H5RAT_H

#include <halfive/h5req.h>

#define POS_SIGN (0)
#define NEG_SIGN (1)

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

h5uint H5Rat_uint_gcd(h5uint a, h5uint b);
h5ulong H5Rat_ulong_gcd(h5ulong a, h5ulong b);
h5umax H5Rat_umax_gcd(h5umax a, h5umax b);

h5uint H5Rat_uint_lcm(h5uint a, h5uint b);
h5ulong H5Rat_ulong_lcm(h5ulong a, h5ulong b);
h5umax H5Rat_umax_lcm(h5umax a, h5umax b);

void H5Rat_uint_xorSwap(h5uint *a, h5uint *b);
void H5Rat_ulong_xorSwap(h5ulong *a, h5ulong *b);
void H5Rat_umax_xorSwap(h5umax *a, h5umax *b);

H5Rat_uint H5Rat_uint_simplify(H5Rat_uint a);
H5Rat_ulong H5Rat_ulong_simplify(H5Rat_ulong a);
H5Rat_umax H5Rat_umax_simplify(H5Rat_umax a);

void H5Rat_uint_equate(H5Rat_uint *a, H5Rat_uint *b);
void H5Rat_ulong_equate(H5Rat_ulong *a, H5Rat_ulong *b);
void H5Rat_umax_equate(H5Rat_umax *a, H5Rat_umax *b);

_Bool H5Rat_uint_compare(H5Rat_uint a, H5Rat_uint b);
_Bool H5Rat_ulong_compare(H5Rat_ulong a, H5Rat_ulong b);
_Bool H5Rat_umax_compare(H5Rat_umax a, H5Rat_umax b);

H5Rat_uint H5Rat_uint_add(H5Rat_uint a, H5Rat_uint b);
H5Rat_ulong H5Rat_ulong_add(H5Rat_ulong a, H5Rat_ulong b);
H5Rat_umax H5Rat_umax_add(H5Rat_umax a, H5Rat_umax b);

H5Rat_uint H5Rat_uint_multiply(H5Rat_uint a, H5Rat_uint b);
H5Rat_ulong H5Rat_ulong_multiply(H5Rat_ulong a, H5Rat_ulong b);
H5Rat_umax H5Rat_umax_multiply(H5Rat_umax a, H5Rat_umax b);

h5uint H5Rat_uint_convertToInt(H5Rat_uint a);
h5ulong H5Rat_ulong_convertToInt(H5Rat_ulong a);
h5umax H5Rat_umax_toInt(H5Rat_umax a);

h5sint H5Rat_sint_convertToInt(H5Rat_uint a);
h5slong H5Rat_slong_convertToInt(H5Rat_ulong a);
h5smax H5Rat_smax_convertToInt(H5Rat_umax a);

H5Rat_umax H5Rat_UtoM(H5Rat_uint a);
H5Rat_umax H5Rat_LtoM(H5Rat_ulong a);

H5Rat_ulong H5Rat_UtoL(H5Rat_uint a);
H5Rat_ulong H5Rat_MtoL(H5Rat_umax a);

H5Rat_uint H5Rat_LtoU(H5Rat_ulong a);
H5Rat_uint H5Rat_MtoU(H5Rat_umax a);

#ifdef FLOATS_SUPPORTED
float H5Rat_uint_convertToFloat(H5Rat_uint a);
float H5Rat_ulong_convertToFloat(H5Rat_ulong a);
float H5Rat_umax_convertToFloat(H5Rat_umax a);
#endif

#ifdef H5RAT_SHORTHAND
#define H5_gcd_U H5Rat_uint_gcd
#define H5_gcd_L H5Rat_gcd
#define H5_gcd_M H5Rat_gcd

#define H5_lcm_U H5Rat_uint_lcm
#define H5_lcm_L H5Rat_lcm
#define H5_lcm_M H5Rat_lcm

#define H5_RSwap_U H5Rat_uint_xorSwap
#define H5_RSwap_L H5Rat_ulong_xorSwap
#define H5_RSwap_M H5Rat_umax_xorSwap

#define H5_RSimp_U H5Rat_uint_simplify
#define H5_RSimp_L H5Rat_ulong_simplify
#define H5_RSimp_M H5Rat_umax_simplify

#define H5_REq_U H5Rat_uint_equate
#define H5_REq_L H5Rat_ulong_equate
#define H5_REq_M H5Rat_umax_equate

#define H5_RCmp_U H5Rat_uint_compare
#define H5_RCmp_L H5Rat_ulong_compare
#define H5_RCmp_M H5Rat_umax_compare

#define H5_RAdd_U H5Rat_uint_add
#define H5_RAdd_L H5Rat_ulong_add
#define H5_RAdd_M H5Rat_umax_add

#define H5_RMult_U H5Rat_uint_multiply
#define H5_RMult_L H5Rat_ulong_multiply
#define H5_RMult_M H5Rat_umax_multiply

#define H5_RSInt_U H5Rat_sint_convertToInt
#define H5_RSInt_L H5Rat_slong_convertToInt
#define H5_RSInt_M H5Rat_smax_convertToInt

#define H5_RInt_U H5Rat_uint_convertToInt
#define H5_RInt_L H5Rat_ulong_convertToInt
#define H5_RInt_M H5Rat_umax_convertToInt

#ifdef FLOATS_SUPPORTED
#define H5_RFloat_U H5Rat_uint_convertToFloat
#define H5_RFloat_L H5Rat_ulong_converToFloat
#define H5_RFloat_M H5Rat_umax_converToFloat
#endif
#endif
#endif
