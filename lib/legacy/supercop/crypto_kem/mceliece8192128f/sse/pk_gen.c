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

#define min(a, b) ((a < b) ? a : b)

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

static void transpose_64x64(uint64_t * out, uint64_t * in)
{
	int i, j, s, d;

	uint64_t x, y;
	uint64_t masks[6][2] = {
	                        {0x5555555555555555, 0xAAAAAAAAAAAAAAAA},
	                        {0x3333333333333333, 0xCCCCCCCCCCCCCCCC},
	                        {0x0F0F0F0F0F0F0F0F, 0xF0F0F0F0F0F0F0F0},
	                        {0x00FF00FF00FF00FF, 0xFF00FF00FF00FF00},
	                        {0x0000FFFF0000FFFF, 0xFFFF0000FFFF0000},
	                        {0x00000000FFFFFFFF, 0xFFFFFFFF00000000}
	                       };

	for (i = 0; i < 64; i++)
		out[i] = in[i];

	for (d = 5; d >= 0; d--)
	{
		s = 1 << d;

		for (i = 0; i < 64; i += s*2)
		for (j = i; j < i+s; j++)
		{
			x = (out[j] & masks[d][0]) | ((out[j+s] & masks[d][0]) << s);
			y = ((out[j] & masks[d][1]) >> s) | (out[j+s] & masks[d][1]);

			out[j+0] = x;
			out[j+s] = y;
		}
	}
}

/* return number of trailing zeros of the non-zero input in */
static inline int ctz(uint64_t in)
{
	return __builtin_ctzll(in);
}

static inline uint64_t same_mask(uint16_t x, uint16_t y)
{
        uint64_t mask;

        mask = x ^ y;
        mask -= 1;
        mask >>= 63;
        mask = -mask;

        return mask;
}

static int mov_columns(uint64_t mat[][ 128 ], uint32_t * perm)
{
	int i, j, k, s, block_idx, row;
	uint64_t buf[64], ctz_list[32], t, d, mask; 
       
	row = GFBITS * SYS_T - 32;
	block_idx = row/64;

	// extract the 32x64 matrix

	for (i = 0; i < 32; i++)
		buf[i] = (mat[ row + i ][ block_idx + 0 ] >> 32) | 
		         (mat[ row + i ][ block_idx + 1 ] << 32);
        
	// compute the column indices of pivots by Gaussian elimination.
	// the indices are stored in ctz_list

	for (i = 0; i < 32; i++)
	{
		t = buf[i];
		for (j = i+1; j < 32; j++)
			t |= buf[j];

		if (t == 0) return -1; // return if buf is not full rank

		ctz_list[i] = s = ctz(t);

		for (j = i+1; j < 32; j++) { mask = (buf[i] >> s) & 1; mask -= 1;    buf[i] ^= buf[j] & mask; }
		for (j =   0; j <  i; j++) { mask = (buf[j] >> s) & 1; mask = -mask; buf[j] ^= buf[i] & mask; }
		for (j = i+1; j < 32; j++) { mask = (buf[j] >> s) & 1; mask = -mask; buf[j] ^= buf[i] & mask; }
	}
   
	// updating permutation
  
	for (j = 0;   j < 32; j++)
	for (k = j+1; k < 64; k++)
	{
			d = perm[ row + j ] ^ perm[ row + k ];
			d &= same_mask(k, ctz_list[j]);
			perm[ row + j ] ^= d;
			perm[ row + k ] ^= d;
	}
   
	// moving columns of mat according to the column indices of pivots

	for (i = 0; i < GFBITS*SYS_T; i += 64)
	{

		for (j = 0; j < min(64, GFBITS*SYS_T - i); j++)
			buf[j] = (mat[ i+j ][ block_idx + 0 ] >> 32) | 
		             (mat[ i+j ][ block_idx + 1 ] << 32);
                
		transpose_64x64(buf, buf);

		for (j = 0; j < 32; j++)
		for (k = j+1; k < 64; k++)
		{
			d = buf[ j ] ^ buf[ k ];
			d &= same_mask(k, ctz_list[j]);
			buf[ j ] ^= d;
			buf[ k ] ^= d;
		}

		transpose_64x64(buf, buf);
                
		for (j = 0; j < min(64, GFBITS*SYS_T - i); j++)
		{
			mat[ i+j ][ block_idx + 0 ] = (mat[ i+j ][ block_idx + 0 ] << 32 >> 32) | (buf[j] << 32);
			mat[ i+j ][ block_idx + 1 ] = (mat[ i+j ][ block_idx + 1 ] >> 32 << 32) | (buf[j] >> 32);
		}
	}

	return 0;
}


int pk_gen(unsigned char * pk, const unsigned char * irr, uint32_t * perm)
{
	const int nBlocks_H = (SYS_N + 127) / 128;
	const int nblocks_I = (GFBITS * SYS_T + 63) / 64;

	int i, j, k;
	int row, c;
	
	uint64_t mat[ GFBITS * SYS_T ][ 128 ];

	uint64_t mask;	

	vec128 irr_int[ GFBITS ];

	vec128 consts[ 64 ][ GFBITS ];
	vec128 eval[ 64 ][ GFBITS ];
	vec128 prod[ 64 ][ GFBITS ];
	vec128 tmp[ GFBITS ];

	uint64_t list[1 << GFBITS];

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

	for (j = 0; j < nBlocks_H; j++)
	for (k = 0; k < GFBITS; k++)
	{
		mat[ k ][ 2*j + 0 ] = vec128_extract(prod[ j ][ k ], 0);
		mat[ k ][ 2*j + 1 ] = vec128_extract(prod[ j ][ k ], 1);
	}

	for (i = 1; i < SYS_T; i++)
	for (j = 0; j < nBlocks_H; j++)
	{
		vec128_mul(prod[j], prod[j], consts[j]);

		for (k = 0; k < GFBITS; k++)
		{
			mat[ i*GFBITS + k ][ 2*j + 0 ] = vec128_extract(prod[ j ][ k ], 0);
			mat[ i*GFBITS + k ][ 2*j + 1 ] = vec128_extract(prod[ j ][ k ], 1);
		}
	}

	// gaussian elimination

	for (i = 0; i < (GFBITS * SYS_T) / 64; i++)
	for (j = 0; j < 64; j++)
	{
		row = i*64 + j;			

		if (row == GFBITS * SYS_T - 32)
		{
			if (mov_columns(mat, perm))
				return -1;
		}

		for (k = row + 1; k < PK_NROWS; k++)
		{
			mask = mat[ row ][ i ] >> j;
			mask &= 1;
			mask -= 1;

			for (c = 0; c < 128; c++)
				mat[ row ][ c ] ^= mat[ k ][ c ] & mask;
		}

		if ( ((mat[ row ][ i ] >> j) & 1) == 0 ) // return if not systematic
		{
			return -1;
		}

		for (k = 0; k < row; k++)
		{
			mask = mat[ k ][ i ] >> j;
			mask &= 1;
			mask = -mask;

			for (c = 0; c < 128; c++)
				mat[ k ][ c ] ^= mat[ row ][ c ] & mask;
		}
		for (k = row+1; k < GFBITS * SYS_T; k++)
		{
			mask = mat[ k ][ i ] >> j;
			mask &= 1;
			mask = -mask;

			for (c = 0; c < 128; c++)
				mat[ k ][ c ] ^= mat[ row ][ c ] & mask;
		}
	}

	for (i = 0; i < GFBITS * SYS_T; i++)	
	for (j = nblocks_I; j < 128; j++)
	{
		store8(pk, mat[i][j]);
		pk += 8;
	}

	//

	return 0;
}

