/*
 * Support functions for signatures (hash-to-point, norm).
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2017-2019  Falcon Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@nccgroup.com>
 */

#include "inner.h"

/* see inner.h */
void
falcon512tree_ref_hash_to_point(
	shake256_context *sc,
	uint16_t *x, unsigned logn, uint8_t *tmp)
{

	/*
	 * This is the straightforward per-the-spec implementation. It
	 * is not constant-time, thus it might reveal information on the
	 * plaintext (at least, enough to check the plaintext against a
	 * list of potential plaintexts) in a scenario where the
	 * attacker does not have access to the signature value or to
	 * the public key, but knows the nonce (without knowledge of the
	 * nonce, the hashed output cannot be matched against potential
	 * plaintexts).
	 */
	size_t n;

	(void)tmp;
	n = (size_t)1 << logn;
	while (n > 0) {
		uint8_t buf[2];
		uint32_t w;

		shake256_extract(sc, (void *)buf, sizeof buf);
		w = ((unsigned)buf[0] << 8) | (unsigned)buf[1];
		if (w < 61445) {
			while (w >= 12289) {
				w -= 12289;
			}
			*x ++ = (uint16_t)w;
			n --;
		}
	}

}

/* see inner.h */
int
falcon512tree_ref_is_short(
	const int16_t *s1, const int16_t *s2, unsigned logn)
{
	/*
	 * We use the l2-norm. Code below uses only 32-bit operations to
	 * compute the square of the norm with saturation to 2^32-1 if
	 * the value exceeds 2^31-1.
	 */
	size_t n, u;
	uint32_t s, ng;

	n = (size_t)1 << logn;
	s = 0;
	ng = 0;
	for (u = 0; u < n; u ++) {
		int32_t z;

		z = s1[u];
		s += (uint32_t)(z * z);
		ng |= s;
		z = s2[u];
		s += (uint32_t)(z * z);
		ng |= s;
	}
	s |= -(ng >> 31);

	/*
	 * Acceptance bound on the l2-norm is:
	 *   1.2*1.55*sqrt(q)*sqrt(2*N)
	 * Value 7085 is floor((1.2^2)*(1.55^2)*2*1024).
	 */
	return s < (((uint32_t)7085 * (uint32_t)12289) >> (10 - logn));
}

/* see inner.h */
int
falcon512tree_ref_is_short_half(
	uint32_t sqn, const int16_t *s2, unsigned logn)
{
	size_t n, u;
	uint32_t ng;

	n = (size_t)1 << logn;
	ng = -(sqn >> 31);
	for (u = 0; u < n; u ++) {
		int32_t z;

		z = s2[u];
		sqn += (uint32_t)(z * z);
		ng |= sqn;
	}
	sqn |= -(ng >> 31);

	/*
	 * Acceptance bound on the l2-norm is:
	 *   1.2*1.55*sqrt(q)*sqrt(2*N)
	 * Value 7085 is floor((1.2^2)*(1.55^2)*2*1024).
	 */
	return sqn < (((uint32_t)7085 * (uint32_t)12289) >> (10 - logn));
}
