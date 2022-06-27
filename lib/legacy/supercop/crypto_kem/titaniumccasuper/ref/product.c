/* ****************************** *
 * Titanium_CCA_super             *
 * Implemented by Raymond K. ZHAO *
 *                                *
 * Naive Product/MP               *
 * ****************************** */

#include "product.h"
#include "param.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void product(uint32_t *out, const uint32_t *a, uint32_t alen, const uint32_t *b, uint32_t blen)
{
	uint32_t i, j;
	
	memset(out, 0, sizeof(uint32_t) * (alen + blen - 1));
	for (i = 0; i < alen; i++)
	{
		for (j = 0; j < blen; j++)
		{
			out[i + j] = ((uint64_t)out[i + j] + (uint64_t)a[i] * (uint64_t)b[j]) % Q;
		}
	}
}

/* rev(a) mp b */
void mp(uint32_t *out, uint32_t outlen, const uint32_t *a, uint32_t alen, const uint32_t *b, uint32_t blen)
{
	uint32_t c[2 * N + 2 * K + D];
	uint32_t a1[N + K];
	
	uint32_t i;
	/* reverse a */
	for (i = 0; i < alen; i++)
	{
		a1[i] = a[alen - i - 1];
	}
	product(c, a1, alen, b, blen);
	memcpy(out, c + alen - 1, sizeof(uint32_t) * outlen);
}
