#include <string.h>
#include <stdio.h>
#include "crypto_aead.h"

//Constant
const unsigned int CONSTANT[20] =
{ 0x14f1c272,0x3279c419,0x4b8ea41d,0x0cc80863,
	0xd28062e1,0xe71d3dda,0xe3c4d158,0xa7f067ac,
	0x94935056,0x8ee5c63d,0xf5a0cec3,0xd33da5a7,
	0x7de892ac,0xe8fd9b12,0xfb625a84,0xf15a5323,
	0xd93d3995,0x9a485a71,0xdab8ecd1,0x9d9b3e2e };

//256-bit State
unsigned char S[256];
unsigned char a, b;

//Two Sboxes
#define SB (S[30]&S[29])^(S[96]&S[88])^(S[159]&S[132])^(S[207]&S[206])^1
#define SB1 (S[30]&S[132])^(S[29]&S[206])^(S[96]&S[207])^(S[159]&S[88])

#define ADDa { S[63]^=a; S[191]^=a; }
#define ADDb { S[127]^=b; S[255]^=b; }

//Core step
void CoreStep()
{
	a = SB;
	b = (SB1 ^ S[32]) & 1;

	unsigned char f[4];

	f[0] = S[0] ^ S[31] ^ S[32] ^ S[77];
	f[1] = S[64] ^ S[92] ^ S[94] ^ S[129];
	f[2] = S[128] ^ S[150] ^ S[155] ^ S[218];
	f[3] = S[192] ^ S[200] ^ S[211] ^ S[31];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 63; j++)
		{
			S[64 * i + j] = S[64 * i + j + 1];
		}
	}

	S[63] = f[0];
	S[127] = f[1];
	S[191] = f[2];
	S[255] = f[3];
}

//Initinalization and finilation process by bit
void InitBit(unsigned char ivbit)
{
	CoreStep();

	a ^= ivbit;

	ADDa;
	ADDb;
}

//Initinalization and finilation process by byte
void InitByte(unsigned char ivbyte)
{
	unsigned char ivbit;
	for (int i = 0; i < 8; i++)
	{
		ivbit = (ivbyte >> i) & 1;
		InitBit(ivbit);
	}
}

//Encrypt by bit
void EncryptBit(unsigned char plaintextbit, unsigned char *ciphertextbit)
{
	CoreStep();

	*ciphertextbit = b ^ plaintextbit;
	a ^= plaintextbit;

	ADDa;
}

//Encrypt by byte
void EncryptByte(unsigned char plaintextbyte, unsigned char *ciphertextbyte)
{
	*ciphertextbyte = 0;
	unsigned char plaintextbit, ciphertextbit;
	for (int i = 0; i < 8; i++)
	{
		plaintextbit = (plaintextbyte >> i) & 1;
		EncryptBit(plaintextbit, &ciphertextbit);
		*ciphertextbyte |= (ciphertextbit << i);
	}
}

//Decrypt by bit
void DecryptBit(unsigned char *plaintextbit, unsigned char ciphertextbit)
{
	CoreStep();

	*plaintextbit = b ^ ciphertextbit;
	a ^= *plaintextbit;

	ADDa;
}

//Decrypt by byte
void DecryptByte(unsigned char *plaintextbyte, unsigned char ciphertextbyte)
{
	*plaintextbyte = 0;
	unsigned char plaintextbit, ciphertextbit;
	for (int i = 0; i < 8; i++)
	{
		ciphertextbit = (ciphertextbyte >> i) & 1;
		DecryptBit(&plaintextbit, ciphertextbit);
		*plaintextbyte |= (plaintextbit << i);
	}
}

//Load constants and key to the state
void Load(const unsigned char *key)
{
	for (int rc = 0; rc < 4; rc++)
	{
		for (int i = 0; i < 8; i++)
		{
			unsigned char cbit = (CONSTANT[rc] >> (31 - i)) & 1;
			S[i + rc * 64] = cbit;
		}
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				unsigned char cbit = (CONSTANT[rc] >> (23 - 8 * i - j)) & 1;
				S[i * 8 + j + 40 + rc * 64] = cbit;
			}
		}
	}

	for (int i = 0; i < 16; i++)
	{
		for (unsigned char j = 0; j < 8; j++)
		{
			unsigned char keybit = (key[i] >> (7 - j)) & 1;
			S[8 * (i % 4) + j + (i / 4) * 64 + 8] = keybit;
		}
	}
}

//Encrypt a message
int crypto_aead_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
)
{
	//Load
	Load(k);

	//Process IV
	for (int i = 0; i < 16; i++)
	{
		InitByte(npub[i]);
	}

	//Process associated data
	for (unsigned long long i = 0; i < adlen; i++)
	{
		InitByte(ad[i]);
	}

	//512 steps 0-stream
	for (int i = 0; i < 512 / 8; i++)
	{
		InitByte(0);
	}

	//Process message
	for (unsigned long long i = 0; i < mlen; i++)
	{
		EncryptByte(m[i], &c[i]);
	}

	//512 steps 0-stream
	for (int i = 0; i < 512 / 8; i++)
	{
		InitByte(0);
	}

	unsigned char tag[16];
	//Tag generation
	for (int i = 0; i < 128 / 8; i++)
	{
		EncryptByte(0, &tag[i]);
	}

	//Tag is appended to ciphertext
	*clen = mlen + 16;
	memcpy(c + mlen, tag, 16);

	return 0;
}

//Decrypt a ciphertext
int crypto_aead_decrypt(
	unsigned char *m, unsigned long long *mlen,
	unsigned char *nsec,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *ad, unsigned long long adlen,
	const unsigned char *npub,
	const unsigned char *k
)
{
	//Ciphertext length is less than 128-bit
	if (clen < 16)
	{
		return -1;
	}

	//Load
	Load(k);

	//Process IV
	for (int i = 0; i < 16; i++)
	{
		InitByte(npub[i]);
	}

	//Processage associated data
	for (unsigned long long i = 0; i < adlen; i++)
	{
		InitByte(ad[i]);
	}

	//512 steps 0-stream
	for (int i = 0; i < 512 / 8; i++)
	{
		InitByte(0);
	}

	//Process ciphertext
	*mlen = clen - 16;
	for (unsigned long long i = 0; i < *mlen; i++)
	{
		DecryptByte(&m[i], c[i]);
	}

	//512 steps 0-stream
	for (int i = 0; i < 512 / 8; i++)
	{
		InitByte(0);
	}

	unsigned char tag[16];
	//Tag generation
	for (int i = 0; i < 128 / 8; i++)
	{
		EncryptByte(0, &tag[i]);
	}

	unsigned char check = 0;
	//Check authentication
	for (int i = 0; i < 16; i++)
	{
		check |= tag[i] ^ c[clen - 16 + i];
	}

	if (check == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
