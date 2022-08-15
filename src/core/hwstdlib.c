/*
Copyright 2021 Plan 9 Foundation

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include <halfworld/hwreq.h>
#include <halfworld/hwstdlib.h>

sulong _hwlibcstrtoul(char *nptr, char **endptr, int base)
{
	char *p;
	sulong n, nn, m;
	int c, ovfl, neg, v, ndig;

	p = nptr;
	neg = 0;
	n = 0;
	ndig = 0;
	ovfl = 0;

	/*
	 * White space
	 */
	for (;; p++) {
		switch (*p) {
		case ' ':
		case '\t':
		case '\n':
		case '\f':
		case '\r':
		case '\v':
			continue;
		}
		break;
	}

	/*
	 * Sign
	 */
	if (*p == '-' || *p == '+')
		if (*p++ == '-')
			neg = 1;

	/*
	 * Base
	 */
	if (base == 0) {
		if (*p != '0')
			base = 10;
		else {
			base = 8;
			if (p[1] == 'x' || p[1] == 'X')
				base = 16;
		}
	}
	if (base < 2 || 36 < base)
		goto Return;
	if (base == 16 && *p == '0') {
		if (p[1] == 'x' || p[1] == 'X')
			if (('0' <= p[2] && p[2] <= '9') ||
			    ('a' <= p[2] && p[2] <= 'f') ||
			    ('A' <= p[2] && p[2] <= 'F'))
				p += 2;
	}
	/*
	 * Non-empty sequence of digits
	 */
	n = 0;
	m = ULONG_MAX / base;
	for (;; p++, ndig++) {
		c = *p;
		v = base;
		if ('0' <= c && c <= '9')
			v = c - '0';
		else if ('a' <= c && c <= 'z')
			v = c - 'a' + 10;
		else if ('A' <= c && c <= 'Z')
			v = c - 'A' + 10;
		if (v >= base)
			break;
		if (n > m)
			ovfl = 1;
		nn = n * base + v;
		if (nn < n)
			ovfl = 1;
		n = nn;
	}

Return:
	if (ndig == 0)
		p = nptr;
	if (endptr)
		*endptr = p;
	if (ovfl)
		return ULONG_MAX;
	if (neg)
		return -n;
	return n;
}

int _hwlibcstrcmp(char *s1, char *s2)
{
	unsigned c1, c2;

	for (;;) {
		c1 = *s1++;
		c2 = *s2++;
		if (c1 != c2) {
			if (c1 > c2)
				return 1;
			return -1;
		}
		if (c1 == 0)
			return 0;
	}
}

char *_hwlibcstrtok(char *s, char *b)
{
	static char *under_rock;
	char map[N] = {0}, *os;

	while (*b)
		map[*(suchar *)b++] = 1;
	if (s == 0)
		s = under_rock;
	while (map[*(suchar *)s++])
		;
	if (*--s == 0)
		return 0;
	os = s;
	while (map[*(suchar *)s] == 0)
		if (*s++ == 0) {
			under_rock = s - 1;
			return os;
		}
	*s++ = 0;
	under_rock = s;
	return os;
}



/*
Copyright Nomagno 2022

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

void *_hwlibcmemcpy(void *dest, const void *src, size_t n) {
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;
	for (size_t i = 0; i < n; i++) 
		d[i] = s[i];
	return dest;
}

void *_hwlibcmemset(void *str, int val, size_t n) {
	unsigned char *s = (unsigned char *)str;
	for (size_t i = 0; i < n; i++) 
		s[i] = val;
	return str;
}

