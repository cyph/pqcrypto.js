/*
  This file is for public-key generation
*/

#include "pk_gen.h"

#include "controlbits.h"
#include "params.h"
#include "benes.h"
#include "util.h"
#include "fft.h"

#include <stdint.h>

static void de_bitslicing(uint64_t * out, const vec128 in[][GFBITS])
{
	int i, j, r;
	uint64_t u = 0;

	for (i = 0; i < (1 << GFBITS); i++)
		out[i] = 0 ;

	for (i = 0; i < 64; i++)
	{
		for (j = GFBITS-1; j >= 0; j--)
		{
			u = vec128_extract(in[i][j], 0); 
			for (r = 0; r < 64; r++) { out[i*128 + 0*64 + r] <<= 1; out[i*128 + 0*64 + r] |= (u >> r) & 1;}
			u = vec128_extract(in[i][j], 1);
			for (r = 0; r < 64; r++) { out[i*128 + 1*64 + r] <<= 1; out[i*128 + 1*64 + r] |= (u >> r) & 1;}
		}
	}
}

static void to_bitslicing_2x(vec128 out0[][GFBITS], vec128 out1[][GFBITS], const uint64_t * in)
{
	int i, j, k, r;
	uint64_t u[2];

	for (i = 0; i < 64; i++)
	{
		for (j = GFBITS-1; j >= 0; j--)
		{
			for (k = 0; k < 2; k++)
			for (r = 63; r >= 0; r--)
			{
				u[k] <<= 1;
				u[k] |= (in[i*128 + k*64 + r] >> (j + GFBITS)) & 1;
			}
        
			out1[i][j] = vec128_set2x(u[0], u[1]);
		}
        
		for (j = GFBITS-1; j >= 0; j--)
		{
			for (k = 0; k < 2; k++)
			for (r = 63; r >= 0; r--)
			{
				u[k] <<= 1;
				u[k] |= (in[i*128 + k*64 + r] >> j) & 1;
			}
        
			out0[i][GFBITS-1-j] = vec128_set2x(u[0], u[1]);
		}
	}
}

int pk_gen(unsigned char * pk, const unsigned char * irr, uint32_t * perm)
{
	const int nblocks_H = (SYS_N + 63) / 64;
	const int nBlocks_H = (SYS_N + 127) / 128;
	const int nblocks_I = (GFBITS * SYS_T + 63) / 64;
	const int nBlocks_I = (GFBITS * SYS_T + 127) / 128;
	const int block_idx = nblocks_I - 1;
	int tail = (GFBITS * SYS_T) % 64;

	int i, j, k;
	int row, c;
	
	uint64_t mat[ GFBITS * SYS_T ][ nBlocks_H * 2 ];
	uint64_t ops[ GFBITS * SYS_T ][ nblocks_I ];

	uint64_t mask;	

	vec128 irr_int[ GFBITS ];

	vec128 consts[64][ GFBITS ];
	vec128 eval[ 64 ][ GFBITS ];
	vec128 prod[ 64 ][ GFBITS ];
	vec128 tmp[ GFBITS ];

	uint64_t list[1 << GFBITS];
	uint64_t one_row[ 128 ];

	// compute the inverses 

	irr_load(irr_int, irr);

	fft(eval, irr_int);

	vec128_copy(prod[0], eval[0]);

	for (i = 1; i < 64; i++)
		vec128_mul(prod[i], prod[i-1], eval[i]);

	vec128_inv(tmp, prod[63]);

	for (i = 62; i >= 0; i--)
	{
		vec128_mul(prod[i+1], prod[i], tmp);
		vec128_mul(tmp, tmp, eval[i+1]);
	}

	vec128_copy(prod[0], tmp);

	// fill matrix 

	de_bitslicing(list, prod);

	for (i = 0; i < (1 << GFBITS); i++)
	{	
		list[i] <<= GFBITS;
		list[i] |= i;	
		list[i] |= ((uint64_t) perm[i]) << 31;
	}

	sort_63b(1 << GFBITS, list);

	to_bitslicing_2x(consts, prod, list);

	for (i = 0; i < (1 << GFBITS); i++)
		perm[i] = list[i] & GFMASK;

	for (j = 0; j < nBlocks_I; j++)
	for (k = 0; k < GFBITS; k++)
	{
		mat[ k ][ 2*j + 0 ] = vec128_extract(prod[ j ][ k ], 0);
		mat[ k ][ 2*j + 1 ] = vec128_extract(prod[ j ][ k ], 1);
	}

	for (i = 1; i < SYS_T; i++)
	for (j = 0; j < nBlocks_I; j++)
	{
		vec128_mul(prod[j], prod[j], consts[j]);

		for (k = 0; k < GFBITS; k++)
		{
			mat[ i*GFBITS + k ][ 2*j + 0 ] = vec128_extract(prod[ j ][ k ], 0);
			mat[ i*GFBITS + k ][ 2*j + 1 ] = vec128_extract(prod[ j ][ k ], 1);
		}
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

		if ( ((mat[ row ][ i ] >> j) & 1) == 0 ) // return if not systematic
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

	for (j = nBlocks_I; j < nBlocks_H; j++)
	for (k = 0; k < GFBITS; k++)
	{
		mat[ k ][ 2*j + 0 ] = vec128_extract(prod[ j ][ k ], 0);
		mat[ k ][ 2*j + 1 ] = vec128_extract(prod[ j ][ k ], 1);
	}

	for (i = 1; i < SYS_T; i++)
	for (j = nBlocks_I; j < nBlocks_H; j++)
	{
		vec128_mul(prod[j], prod[j], consts[j]);

		for (k = 0; k < GFBITS; k++)
		{
			mat[ i*GFBITS + k ][ 2*j + 0 ] = vec128_extract(prod[ j ][ k ], 0);
			mat[ i*GFBITS + k ][ 2*j + 1 ] = vec128_extract(prod[ j ][ k ], 1);	
		}
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

		pk += PK_ROW_BYTES % 8;
	}

	//

	return 0;
}

