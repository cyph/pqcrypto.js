#include "sk_gen.h"

#include "crypto_stream_salsa20.h"
#include "randombytes.h"
#include "params.h"
#include "gf.h"

#include <stdint.h>

static int irr_gen(gf *out, gf *f)
{
	int i, j, k, c;

	gf mat[ SYS_T+1 ][ SYS_T ];
	gf mask, inv, t;

	// fill matrix

	mat[0][0] = 1;

	for (i = 1; i < SYS_T; i++)
		mat[0][i] = 0;

	for (i = 0; i < SYS_T; i++)
		mat[1][i] = f[i];

	for (j = 2; j <= SYS_T; j++)
		GF_mul(mat[j], mat[j-1], f);

	// gaussian

	for (j = 0; j < SYS_T; j++)
	{
		for (k = j + 1; k < SYS_T; k++)
		{
			mask = gf_iszero(mat[ j ][ j ]);

			for (c = j; c < SYS_T + 1; c++)
				mat[ c ][ j ] ^= mat[ c ][ k ] & mask;

		}

		if ( mat[ j ][ j ] == 0 ) // return if not invertible
		{
			return -1;
		}

		// compute inverse

		inv = gf_inv(mat[j][j]);

		for (c = j; c < SYS_T + 1; c++)
			mat[ c ][ j ] = gf_mul(mat[ c ][ j ], inv) ;

		//

		for (k = 0; k < SYS_T; k++)
		{
			if (k != j)
			{
				t = mat[ j ][ k ];

				for (c = j; c < SYS_T + 1; c++)
					mat[ c ][ k ] ^= gf_mul(mat[ c ][ j ], t);
			}
		}
	}

	//

	for (i = 0; i < SYS_T; i++)
		out[i] = mat[ SYS_T ][ i ];

	return 0;
}

static void irr_store(unsigned char *dest, gf *irr)
{
	int i, j, k;
	unsigned char v;

	for (i = 0; i < GFBITS; i++)
	for (j = 0; j <     16; j++)
	{
 		v = 0;

		for (k = 7; k >= 0; k--)
		{
			v <<= 1;
			v |= (irr[j*8 + k] >> i) & 1;
		}

		dest[i*16 + j] = v;
	}
}

void sk_gen(unsigned char *sk)
{
	int i;

	gf irr[ SYS_T ]; //  irreducible polynomial
	gf f[ SYS_T ]; // random element in GF(2^mt)

	unsigned char key[32];
	unsigned char nonce_f[8] = {1,0,0,0,0,0,0,0};
	unsigned char nonce_c[8] = {2,0,0,0,0,0,0,0};

	//

	while (1)
	{
		randombytes((unsigned char *) key, sizeof(key));
		crypto_stream_salsa20_ref((unsigned char *) f, sizeof(f), nonce_f, key);

		for (i = 0; i < SYS_T; i++) f[i] &= GFMASK;

		if ( irr_gen(irr, f) == 0 ) break;
	}

	irr_store(sk, irr);

	//

	crypto_stream_salsa20_ref((unsigned char *) sk + IRR_BYTES, COND_BYTES, nonce_c, key);
}

