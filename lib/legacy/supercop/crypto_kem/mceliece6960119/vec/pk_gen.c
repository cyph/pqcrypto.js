/*
  This file is for public-key generation
*/

#include "pk_gen.h"

#include "controlbits.h"
#include "uint64_sort.h"
#include "params.h"
#include "benes.h"
#include "util.h"
#include "fft.h"
#include "crypto_declassify.h"
#include "crypto_uint64.h"

static crypto_uint64 uint64_is_equal_declassify(uint64_t t,uint64_t u)
{
  crypto_uint64 mask = crypto_uint64_equal_mask(t,u);
  crypto_declassify(&mask,sizeof mask);
  return mask;
}

static crypto_uint64 uint64_is_zero_declassify(uint64_t t)
{
  crypto_uint64 mask = crypto_uint64_zero_mask(t);
  crypto_declassify(&mask,sizeof mask);
  return mask;
}

#include <stdint.h>

static void de_bitslicing(uint64_t * out, const vec in[][GFBITS])
{
	int i, j, r;

	for (i = 0; i < (1 << GFBITS); i++)
		out[i] = 0 ;

	for (i = 0; i < 128; i++)
	for (j = GFBITS-1; j >= 0; j--)
	for (r = 0; r < 64; r++) 
	{ 
		out[i*64 + r] <<= 1; 
		out[i*64 + r] |= (in[i][j] >> r) & 1; 
	}
}

static void to_bitslicing_2x(vec out0[][GFBITS], vec out1[][GFBITS], const uint64_t * in)
{
	int i, j, r;

	for (i = 0; i < 128; i++)
	{
		for (j = GFBITS-1; j >= 0; j--)
		for (r = 63; r >= 0; r--)
		{
			out1[i][j] <<= 1;
			out1[i][j] |= (in[i*64 + r] >> (j + GFBITS)) & 1;
		}
        
		for (j = GFBITS-1; j >= 0; j--)
		for (r = 63; r >= 0; r--)
		{
			out0[i][GFBITS-1-j] <<= 1;
			out0[i][GFBITS-1-j] |= (in[i*64 + r] >> j) & 1;
		}
	}
}

int pk_gen(unsigned char * pk, const unsigned char * irr, uint32_t * perm, int16_t * pi)
{
	const int nblocks_H = (SYS_N + 63) / 64;
	const int nblocks_I = (PK_NROWS + 63) / 64;
	const int block_idx = nblocks_I - 1;
	int tail = PK_NROWS % 64;

	int i, j, k;
	int row, c;
	
	uint64_t mat[ PK_NROWS ][ nblocks_H ];
	uint64_t ops[ PK_NROWS ][ nblocks_I ];

	uint64_t mask;	

	vec irr_int[2][ GFBITS ];

	vec consts[ 128 ][ GFBITS ];
	vec eval[ 128 ][ GFBITS ];
	vec prod[ 128 ][ GFBITS ];
	vec tmp[ GFBITS ];

	uint64_t list[1 << GFBITS];
	uint64_t one_row[ 128 ];

	// compute the inverses 

	irr_load(irr_int, irr);

	fft(eval, irr_int);

	vec_copy(prod[0], eval[0]);

	for (i = 1; i < 128; i++)
		vec_mul(prod[i], prod[i-1], eval[i]);

	vec_inv(tmp, prod[127]);

	for (i = 126; i >= 0; i--)
	{
		vec_mul(prod[i+1], prod[i], tmp);
		vec_mul(tmp, tmp, eval[i+1]);
	}

	vec_copy(prod[0], tmp);

	// fill matrix 

	de_bitslicing(list, prod);

	for (i = 0; i < (1 << GFBITS); i++)
	{	
		list[i] <<= GFBITS;
		list[i] |= i;	
		list[i] |= ((uint64_t) perm[i]) << 31;
	}

	uint64_sort(list, 1 << GFBITS);

	for (i = 1; i < (1 << GFBITS); i++)
		if (uint64_is_equal_declassify(list[i-1] >> 31,list[i] >> 31))
			return -1;

	to_bitslicing_2x(consts, prod, list);

	for (i = 0; i < (1 << GFBITS); i++)
		pi[i] = list[i] & GFMASK;

	for (j = 0; j < nblocks_I; j++)
	for (k = 0; k < GFBITS; k++)
		mat[ k ][ j ] = prod[ j ][ k ];

	for (i = 1; i < SYS_T; i++)
	for (j = 0; j < nblocks_I; j++)
	{
		vec_mul(prod[j], prod[j], consts[j]);

		for (k = 0; k < GFBITS; k++)
			mat[ i*GFBITS + k ][ j ] = prod[ j ][ k ];
	}

	// gaussian elimination to obtain an upper triangular matrix 
	// and keep track of the operations in ops

	for (i = 0; i < PK_NROWS; i++)
	for (j = 0; j < nblocks_I; j++)
		ops[ i ][ j ] = 0;

	for (i = 0; i < PK_NROWS; i++)
	{
		ops[ i ][ i / 64 ] = 1;
		ops[ i ][ i / 64 ] <<= (i % 64);
	}

	uint64_t column[ PK_NROWS ];

	for (i = 0; i < PK_NROWS; i++) 
		column[i] = mat[ i ][ block_idx ];

	for (row = 0; row < PK_NROWS; row++)
	{
		i = row >> 6;
		j = row & 63;

		for (k = row + 1; k < PK_NROWS; k++)
		{
			mask = mat[ row ][ i ] >> j;
			mask &= 1;
			mask -= 1;

			for (c = 0; c < nblocks_I; c++)
			{
				mat[ row ][ c ] ^= mat[ k ][ c ] & mask;
				ops[ row ][ c ] ^= ops[ k ][ c ] & mask;
			}
		}

                if ( uint64_is_zero_declassify((mat[ row ][ i ] >> j) & 1) ) // return if not systematic
		{
			return -1;
		}

		for (k = row+1; k < PK_NROWS; k++)
		{
			mask = mat[ k ][ i ] >> j;
			mask &= 1;
			mask = -mask;

			for (c = 0; c < nblocks_I; c++)
			{
				mat[ k ][ c ] ^= mat[ row ][ c ] & mask;
				ops[ k ][ c ] ^= ops[ row ][ c ] & mask;
			}
		}
	}

	// computing the lineaer map required to obatin the systematic form

	for (row = PK_NROWS-1; row >= 0; row--)
	for (k = 0; k < row; k++)
	{
		mask = mat[ k ][ row/64 ] >> (row&63);
		mask &= 1;
		mask = -mask;

		for (c = 0; c < nblocks_I; c++)
			ops[ k ][ c ] ^= ops[ row ][ c ] & mask;
	}

	// apply the linear map to the non-systematic part

	for (j = nblocks_I; j < nblocks_H; j++)
	for (k = 0; k < GFBITS; k++)
		mat[ k ][ j ] = prod[ j ][ k ];

	for (i = 1; i < SYS_T; i++)
	for (j = nblocks_I; j < nblocks_H; j++)
	{
		vec_mul(prod[j], prod[j], consts[j]);

		for (k = 0; k < GFBITS; k++)
			mat[ i*GFBITS + k ][ j ] = prod[ j ][ k ];
	}

	for (i = 0; i < PK_NROWS; i++) 
		mat[ i ][ block_idx ] = column[i];

	for (row = 0; row < PK_NROWS; row++)
	{
		i = row >> 6;
		j = row & 63;

		for (k = 0; k < nblocks_H; k++)
			one_row[ k ] = 0;

		for (c = 0; c < PK_NROWS; c++)
		{
			mask = ops[ row ][ c >> 6 ] >> (c & 63);
			mask &= 1;
			mask = -mask;

			for (k = block_idx; k < nblocks_H; k++)
				one_row[ k ] ^= mat[ c ][ k ] & mask;
		}

		for (k = block_idx; k < nblocks_H - 1; k++)
		{
                        one_row[k] = (one_row[k] >> tail) | (one_row[k+1] << (64-tail));
			store8(pk, one_row[k]);
			pk += 8;
		}

		one_row[k] >>= tail;
                store_i(pk, one_row[k], PK_ROW_BYTES % 8);

		pk[ (PK_ROW_BYTES % 8)-1 ] &= (1 << (PK_NCOLS % 8)) - 1; // removing redundant bits

		pk += PK_ROW_BYTES % 8;
	}

	//

	return 0;
}

