/*
  This file is for Niederreiter encryption
*/

#include "encrypt.h"

#include "util.h"
#include "params.h"
#include "randombytes.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "gf.h"

/* output: e, an error vector of weight t */
static void gen_e(unsigned char *e)
{
	int i, j, eq, count;

	uint16_t ind[ SYS_T*2 ];
	uint32_t ind32[ SYS_T*2 ];
	uint64_t e_int[ (SYS_N+63)/64 ];	
	uint64_t one = 1;	
	uint64_t mask;	
	uint64_t val[ SYS_T ];	

	while (1)
	{
		randombytes((unsigned char *) ind, sizeof(ind));

		for (i = 0; i < SYS_T*2; i++)
			ind[i] &= GFMASK;

		// moving and counting indices in the correct range

		count = 0;
		for (i = 0; i < SYS_T*2; i++)
			if (ind[i] < SYS_N)
				ind32[ count++ ] = ind[i];
		
		if (count < SYS_T) continue;
	
		// check for repetition

		eq = 0;

		for (i = 1; i < SYS_T; i++) for (j = 0; j < i; j++)
			if (ind32[i] == ind32[j]) 
				eq = 1;

		if (eq == 0)
			break;
	}

	for (j = 0; j < SYS_T; j++)
		val[j] = one << (ind32[j] & 63);

	for (i = 0; i < (SYS_N+63)/64; i++) 
	{
		e_int[i] = 0;

		for (j = 0; j < SYS_T; j++)
		{
			mask = i ^ (ind32[j] >> 6);
			mask -= 1;
			mask >>= 63;
			mask = -mask;

			e_int[i] |= val[j] & mask;
		}
	}

	for (i = 0; i < (SYS_N+63)/64; i++) 
		{ store8(e, e_int[i]); e += 8; }
}

/* input: public key pk, error vector e */
/* output: syndrome s */
void syndrome(unsigned char *s, const unsigned char *pk, unsigned char *e)
{
	uint64_t b;

	const uint64_t *pk_ptr; 
	const uint64_t *e_ptr = ((uint64_t *) (e + SYND_BYTES));

	int i, j;

	//

	for (i = 0; i < SYND_BYTES; i++)
		s[i] = e[i];

	for (i = 0; i < PK_NROWS; i++)	
	{
		pk_ptr = ((uint64_t *) (pk + PK_ROW_BYTES * i));
	
		b = 0;
		for (j = 0; j < PK_NCOLS/64; j++)
			b ^= pk_ptr[j] & e_ptr[j];

		b ^= ((uint32_t *) &pk_ptr[j])[0] & ((uint32_t *) &e_ptr[j])[0];

		b ^= b >> 32;
		b ^= b >> 16;
		b ^= b >> 8;
		b ^= b >> 4;
		b ^= b >> 2;
		b ^= b >> 1;
		b &= 1;

		s[ i/8 ] ^= (b << (i%8));
	}
}

/* input: public key pk */
/* output: error vector e, syndrome s */
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

	syndrome(s, pk, e);
}

