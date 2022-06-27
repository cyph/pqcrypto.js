#include <string.h>
#include <stdio.h>
#include "api.h"
#include "crypto_hash.h"

#define constB1  0xb7e151628aeull
#define constB2  0x243f6a8885aull

#define LengthA  80
#define LengthB  88
#define LengthC  88
#define NLa  73
#define NLb  65
#define NLc  77
#define FBa  74
#define FBb  66
#define FBc  84
#define FFa  68
#define FFb  64
#define FFc  68
#define INb  85
#define INc  85

#define P_ROUNDS  1024


void Stateupdate(unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3, unsigned char exbit)
{
	unsigned char p = 0, q = 0, r = 0;
	int i;

	p = (FSR1[NLa - 1] & FSR1[LengthA - 1 - 1]) ^ FSR1[LengthA - 1] ^ FSR1[FFa - 1] ^ FSR2[FBb - 1] ^ (FSR2[INb - 1] & FSR3[INc - 1]);
	q = (FSR2[NLb - 1] & FSR2[LengthB - 1 - 1]) ^ FSR2[LengthB - 1] ^ FSR2[FFb - 1] ^ FSR3[FBc - 1];
	r = (FSR3[NLc - 1] & FSR3[LengthC - 1 - 1]) ^ FSR3[LengthC - 1] ^ FSR3[FFc - 1] ^ FSR1[FBa - 1];

	for (i = LengthA - 1; i >= 1; i--)
    FSR1[i] = FSR1[i - 1];
	FSR1[0] = (r ^ exbit);

	for (i = LengthB - 1; i >= 1; i--)
    FSR2[i] = FSR2[i - 1];
	FSR2[0] = (p ^ exbit);

	for (i = LengthC - 1; i >= 1; i--)
    FSR3[i] = FSR3[i - 1];
	FSR3[0] = (q ^ exbit);


}

void Squeeze_digest(unsigned char *out, unsigned char *FSR)
{
	unsigned char i;
	unsigned char statebit = 0;

	for (i = 0; i < 8; i++)
	{
		statebit = FSR[i];
		*out |= (statebit << (7 - i % 8));
	}
}



int crypto_hash(
	unsigned char *out, 
	const unsigned char *in, 
	unsigned long long inlen
	)
{
	unsigned char FSR1[LengthA], FSR2[LengthB], FSR3[LengthC];
	unsigned char c1[44], c2[44];
	unsigned char pad[4] = { 0, 0, 0, 0 };
	unsigned long long i,j;

	memset(FSR1, 0x00, LengthA);
	memset(FSR2, 0x00, LengthB);
	memset(FSR3, 0x00, LengthC);

	memset(c1, 0x00, 44);
	memset(c2, 0x00, 44);


	for (i = 0; i < 44; i++)
    c1[i] = (constB1 >> (43 - i)) & 0x1;
	for (i = 0; i < 44; i++)
    c2[i] = (constB2 >> (43 - i)) & 0x1;

	memcpy(FSR2, c1, 44);
	memcpy(FSR2 + 44, c2, 44);

	
	for (i = 0; i < (inlen) / 4; i++){
		for (j = 0; j < 32; j++){
			FSR1[j] = FSR1[j] ^ ((in[3 - (j / 8) + (i * 4)] >> (7 - (j % 8))) & 0x1U);
		}
		for (j = 0; j < P_ROUNDS; j++)
      Stateupdate(FSR1, FSR2, FSR3, 0);
	}



	for (i = 0; i < (inlen % 4); i++)
		pad[i] = in[inlen - (inlen % 4) + i];
	

	pad[i] = 0x80U;
	for (j = 0; j < 32; j++){
		FSR1[j] = FSR1[j] ^ ((pad[3 - (j / 8)] >> (7 - (j % 8))) & 0x1U);
	}
	for (j = 0; j < P_ROUNDS; j++)
    Stateupdate(FSR1, FSR2, FSR3, 0);

	
	for (i = 0; i < 6; i++)
    Squeeze_digest(out + i, FSR2 + 40 - 8 * i);
	for (i = 0; i < 10; i++)
    Squeeze_digest(out + 6 + i, FSR1 + 72 - 8 * i);
		

	for (j = 0; j < P_ROUNDS; j++)
    Stateupdate(FSR1, FSR2, FSR3, 0);

	for (i = 0; i < 6; i++)
    Squeeze_digest(out + 16 + i, FSR2 + 40 - 8 * i);
	for (i = 0; i < 10; i++)
    Squeeze_digest(out + 16 + 6 + i, FSR1 + 72 - 8 * i);


	return 0;
}



