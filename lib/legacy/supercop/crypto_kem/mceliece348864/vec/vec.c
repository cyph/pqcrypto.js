#include "vec.h"

#include "params.h"

void vec_mul(vec * h, const vec * f, const vec * g)
{
	int i, j;
	vec buf[ 2*GFBITS-1 ];

	for (i = 0; i < 2*GFBITS-1; i++)
		buf[i] = 0;

	for (i = 0; i < GFBITS; i++)
	for (j = 0; j < GFBITS; j++)
		buf[i+j] ^= f[i] & g[j];
		
	for (i = 2*GFBITS-2; i >= GFBITS; i--)
	{
		buf[i-GFBITS+3] ^= buf[i]; 
		buf[i-GFBITS+0] ^= buf[i]; 
	}

	for (i = 0; i < GFBITS; i++)
		h[i] = buf[i];
}

/* bitsliced field squarings */
void vec_sq(vec *out, vec *in)
{
        int i;
        uint64_t result[GFBITS];

        //

        result[0] = in[0]^in[6];
        result[1] = in[11];
        result[2] = in[1]^in[7];
        result[3] = in[6];
        result[4] = in[2] ^ in[11] ^ in[8];
        result[5] = in[7];
        result[6] = in[3]^in[9];
        result[7] = in[8];
        result[8] = in[4]^in[10];
        result[9] = in[9];
        result[10] = in[5] ^ in[11];
        result[11] = in[10];

        //

        for (i = 0; i < GFBITS; i++)
                out[i] = result[i];
}

/* bitsliced field inverses */
void vec_inv(vec *out, vec *in)
{
        uint64_t tmp_11[GFBITS];
        uint64_t tmp_1111[GFBITS];

        vec_copy(out, in);

        vec_sq(out, out);
        vec_mul(tmp_11, out, in); // 11

        vec_sq(out, tmp_11);
        vec_sq(out, out);
        vec_mul(tmp_1111, out, tmp_11); // 1111

        vec_sq(out, tmp_1111);
        vec_sq(out, out);
        vec_sq(out, out);
        vec_sq(out, out);
        vec_mul(out, out, tmp_1111); // 11111111

        vec_sq(out, out);
        vec_sq(out, out);
        vec_mul(out, out, tmp_11); // 1111111111

        vec_sq(out, out);
        vec_mul(out, out, in); // 11111111111

        vec_sq(out, out); // 111111111110
}
