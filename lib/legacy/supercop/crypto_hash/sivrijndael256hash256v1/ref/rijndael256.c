#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "rijndael256.h"

word8 rk[MAXROUNDS+1][4][MAXBC];

void rijndael256KeySched (const unsigned char *key_state)
{
	size_t i;
	size_t j;
	word8 k[4][MAXKC];
	for (j = 0; j < MAXKC; j++)
	{
		for (i = 0; i < 4; i++)
		{
			k[i][j] = key_state[j * 4 + i];
		}
	}

	rijndaelKeySched(k, rk);
}

void rijndael256Encrypt (unsigned char *state_inout, const uint8_t  domain)
{
	size_t i;
	size_t j;
	word8 a[4][MAXBC];
	for (j = 0; j < MAXBC; j++)
	{
		for (i = 0; i < 4; i++)
		{
			a[i][j] = state_inout[j * 4 + i];
		}
	}

	rijndaelEncrypt(a, rk, domain);

	for (j = 0; j < MAXBC; j++)
	{
		for (i = 0; i < 4; i++)
		{
			state_inout[j * 4 + i] = a[i][j];
		}
	}	
}

