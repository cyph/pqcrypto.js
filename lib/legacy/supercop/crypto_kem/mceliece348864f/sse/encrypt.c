/*
  This file is for Niederreiter encryption
*/

#include "encrypt.h"

#include "gf.h"
#include "util.h"
#include "params.h"
#include "randombytes.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* input: public key pk, error vector e */
/* output: syndrome s */
extern void syndrome_asm(unsigned char *s, const unsigned char *pk, unsigned char *e);

/* output: e, an error vector of weight t */
static void gen_e(unsigned char *e)
{
	int i, j, eq, count;

	uint16_t ind_[ SYS_T*2 ];
	uint16_t ind[ SYS_T*2 ];
	uint64_t e_int[ (SYS_N+63)/64 ];	
	uint64_t one = 1;	
	uint64_t mask;	
	uint64_t val[ SYS_T ];	

	while (1)
	{
		randombytes((unsigned char *) ind_, sizeof(ind_));

		for (i = 0; i < SYS_T*2; i++)
			ind_[i] &= GFMASK;

		count = 0;
		for (i = 0; i < SYS_T*2; i++)
			if (ind_[i] < SYS_N)
				ind[ count++ ] = ind_[i];
		
		if (count < SYS_T) continue;

		// check for repetition

		eq = 0;

		for (i = 1; i < SYS_T; i++) for (j = 0; j < i; j++)
			if (ind[i] == ind[j]) 
				eq = 1;

		if (eq == 0)
			break;
	}

	for (j = 0; j < SYS_T; j++)
		val[j] = one << (ind[j] & 63);

	for (i = 0; i < (SYS_N+63)/64; i++) 
	{
		e_int[i] = 0;

		for (j = 0; j < SYS_T; j++)
		{
			mask = i ^ (ind[j] >> 6);
			mask -= 1;
			mask >>= 63;
			mask = -mask;

			e_int[i] |= val[j] & mask;
		}
	}

	for (i = 0; i < (SYS_N+63)/64 - 1; i++) 
		{ store8(e, e_int[i]); e += 8; }

	for (j = 0; j < (SYS_N % 64); j+=8) 
		e[ j/8 ] = (e_int[i] >> j) & 0xFF;
}

/* input: public key pk, error vector e */
/* output: syndrome s */
void syndrome(unsigned char *s, const unsigned char *pk, unsigned char *e)
{
	unsigned char b, row[SYS_N/8];
	const unsigned char *pk_ptr = pk;

	int i, j;

	for (i = 0; i < SYND_BYTES; i++)
		s[i] = 0;

	for (i = 0; i < PK_NROWS; i++)	
	{
		for (j = 0; j < SYS_N/8; j++) 
			row[j] = 0;

		for (j = 0; j < PK_ROW_BYTES; j++) 
			row[ SYS_N/8 - PK_ROW_BYTES + j ] = pk_ptr[j];

		row[i/8] |= 1 << (i%8);
		
		b = 0;
		for (j = 0; j < SYS_N/8; j++)
			b ^= row[j] & e[j];

		b ^= b >> 4;
		b ^= b >> 2;
		b ^= b >> 1;
		b &= 1;

		s[ i/8 ] |= (b << (i%8));

		pk_ptr += PK_ROW_BYTES;
	}
}

void encrypt(unsigned char *s, const unsigned char *pk, unsigned char *e)
{
	gen_e(e);

#ifdef KAT
  {
    int k;
    printf("encrypt e: positions");
    for (k = 0;k < SYS_N;++k)
      if (e[k/8] & (1 << (k&7)))
        printf(" %d",k);
    printf("\n");
  }
#endif

	syndrome_asm(s, pk, e);
}

