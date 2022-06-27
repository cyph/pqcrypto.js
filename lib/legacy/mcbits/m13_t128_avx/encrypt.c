#include "encrypt.h"

#include "crypto_stream_salsa20.h"
#include "randombytes.h"
#include "params.h"
#include "util.h"

#include <stdint.h>

extern void syndrome_asm(unsigned char *s, const unsigned char *pk, unsigned char *e);

static void gen_e(unsigned char *e)
{
	int i, j, eq;

	uint16_t ind[ SYS_T ];
	uint64_t e_int[ 128 ];	
	uint64_t one = 1;	
	uint64_t mask;	
	uint64_t val[ SYS_T ];	

	unsigned char key[32];
	unsigned char nonce[8] = {3,0,0,0,0,0,0,0};

	while (1)
	{
		randombytes((unsigned char *) key, sizeof(key));
		crypto_stream_salsa20_ref((unsigned char *) ind, sizeof(ind), nonce, key);

		for (i = 0; i < SYS_T; i++)
			ind[i] &= GFMASK;

		eq = 0;

		for (i = 1; i < SYS_T; i++) for (j = 0; j < i; j++)
			if (ind[i] == ind[j]) 
				eq = 1;

		if (eq == 0)
			break;
	}

	for (j = 0; j < SYS_T; j++)
		val[j] = one << (ind[j] & 63);

	for (i = 0; i < 128; i++) 
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

	for (i = 0; i < 128; i++)
		store8(e + i*8, e_int[i]);
}

void encrypt(unsigned char *s, unsigned char *e, const unsigned char *pk)
{
	gen_e(e);

	syndrome_asm(s, pk, e);
}

