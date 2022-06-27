#include <string.h>
#include <stdio.h>
#include "crypto_hash.h"

//Constant
const unsigned int CONSTANT[20] =
{ 0x14f1c272,0x3279c419,0x4b8ea41d,0x0cc80863,
	0xd28062e1,0xe71d3dda,0xe3c4d158,0xa7f067ac,
	0x94935056,0x8ee5c63d,0xf5a0cec3,0xd33da5a7,
	0x7de892ac,0xe8fd9b12,0xfb625a84,0xf15a5323,
	0xd93d3995,0x9a485a71,0xdab8ecd1,0x9d9b3e2e };

//256-bit State
unsigned char S[256], S4[512];
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

//Buffer state update function
unsigned char BfStep(unsigned char b)
{
	unsigned char d = S4[0];
	for (int i = 0; i < 511; i++)
	{
		S4[i] = S4[i + 1];
	}

	unsigned char h = d ^ b;
	S4[511] = h;

	return d;
}

//Hash round function
void HHashStep(unsigned char mbit)
{
	CoreStep();

	a = a ^ mbit;
	b = BfStep(b);

	ADDa;
	ADDb;
}

//Process messaage by byte
void HashByte(unsigned char mbyte)
{
	unsigned char mbit;
	for (int i = 0; i < 8; i++)
	{
		mbit = (mbyte >> i) & 1;
		HHashStep(mbit);
	}
}

//Load constants to the state and buffer
void Load()
{
	a = 0;
	b = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			unsigned char cbit = (CONSTANT[i] >> (31 - j)) & 1;
			S[32 * i + j] = S[32 * i + 128 + j] = cbit;
		}

	}

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			unsigned char cbit = (CONSTANT[i + 4] >> (31 - j)) & 1;
			S4[32 * i + j] = cbit;
		}

	}
}

int crypto_hash(
	unsigned char *out,
	const unsigned char *in,
	unsigned long long inlen)
{
	//Load
	Load();

	//Process 32-bit zero when message length is zero
	if (inlen == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			HashByte(0);
		}
	}

	//Process message, 4 bytes message and 4 bytes zero per step
	for (unsigned long long i = 0; i < inlen / 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			HashByte(in[4 * i + j]);
		}

		for (int j = 0; j < 4; j++)
		{
			HashByte(0);
		}
	}

	//Process the rest message
	if (inlen % 4 != 0)
	{
		for (unsigned long long i = (inlen / 4) * 4; i < inlen; i++)
		{
			HashByte(in[i]);
		}

		//Message is padded to 32-bit block using zero
		for (int i = 0; i < 4 - inlen % 4; i++)
		{
			HashByte(0);
		}

		for (int i = 0; i < 4; i++)
		{
			HashByte(0);
		}
	}

	union { unsigned long long u64; unsigned char u8[8]; } bmlen;
	bmlen.u64 = inlen * 8;

	//Set string U
	unsigned char U[128];
	for (int i = 0; i < 64; i++)
	{
		unsigned char ct4_byte = 0;
		ct4_byte = (CONSTANT[4 + i / 4] >> (8 * (3 - i % 4))) & 0xff;
		U[i] = bmlen.u8[7 - i % 8] ^ ct4_byte;
	}
	for (int i = 64; i < 128; i++)
	{
		U[i] = bmlen.u8[7 - i % 8];
	}

	//Use string U and buffer to update the state and buffer
	for (int i = 0; i < 1024 / 8; i++)
	{
		unsigned char tmp = 0;
		for (int j = 0; j < 8; j++)
		{
			tmp |= (S4[256 + j] << j);
		}
		HashByte(U[i] ^ tmp);
	}

	//The output digest is the newest 256 bits of buffer
	for (int i = 0; i < 32; i++)
	{
		out[i] = 0;
		for (int j = 0; j < 8; j++)
		{
			out[i] |= (S4[256 + 8 * i + j] << j);
		}
	}

	return 0;
}
