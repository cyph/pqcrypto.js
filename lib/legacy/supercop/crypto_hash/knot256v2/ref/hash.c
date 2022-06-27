#include"api.h"
#include"crypto_hash.h"
typedef unsigned char u8;
typedef unsigned long long u64;
typedef long long i64;
typedef long long i64;
typedef unsigned int u32;

#define sbox(a, b, c, d, e, f, g, h)                                                                            \
{                                                                                                                             \
	t1 = ~a; t2 = b & t1;t3 = c ^ t2; h = d ^ t3; t5 = b | c; t6 = d ^ t1; g = t5 ^ t6; t8 = b ^ d; t9 = t3 & t6; e = t8 ^ t9; t11 = g & t8; f = t3 ^ t11; \
}

#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))
#define ROTR961(a,b,n) (((a)<<(n))|((b)>>(64-n)))
#define ROTR962(a,b,n) (((b)<<(n))|((a)>>(32-n)))

#define ROTR96MORE321(a,b,n) ((b<<(n-32))>>32)
//#define ROTR96552(a,b,n) (b<<n|a<<(n-32)|b>>(64-(n-32)))
#define ROTR96MORE322(a,b,n) (b<<n|(u64)a<<(n-32)|b>>(96-n))

u8 constant7[127] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x41, 0x03, 0x06,
		0x0c, 0x18, 0x30, 0x61, 0x42, 0x05, 0x0a, 0x14, 0x28, 0x51, 0x23, 0x47,
		0x0f, 0x1e, 0x3c, 0x79, 0x72, 0x64, 0x48, 0x11, 0x22, 0x45, 0x0b, 0x16,
		0x2c, 0x59, 0x33, 0x67, 0x4e, 0x1d, 0x3a, 0x75, 0x6a, 0x54, 0x29, 0x53,
		0x27, 0x4f, 0x1f, 0x3e, 0x7d, 0x7a, 0x74, 0x68, 0x50, 0x21, 0x43, 0x07,
		0x0e, 0x1c, 0x38, 0x71, 0x62, 0x44, 0x09, 0x12, 0x24, 0x49, 0x13, 0x26,
		0x4d, 0x1b, 0x36, 0x6d, 0x5a, 0x35, 0x6b, 0x56, 0x2d, 0x5b, 0x37, 0x6f,
		0x5e, 0x3d, 0x7b, 0x76, 0x6c, 0x58, 0x31, 0x63, 0x46, 0x0d, 0x1a, 0x34,
		0x69, 0x52, 0x25, 0x4b, 0x17, 0x2e, 0x5d, 0x3b, 0x77, 0x6e, 0x5c, 0x39,
		0x73, 0x66, 0x4c, 0x19, 0x32, 0x65, 0x4a, 0x15, 0x2a, 0x55, 0x2b, 0x57,
		0x2f, 0x5f, 0x3f, 0x7f, 0x7e, 0x7c, 0x78, 0x70, 0x60, 0x40 };
void load64(u64* x, u8* S) {
	int i;
	*x = 0;
	for (i = 0; i < 8; ++i)
		*x |= ((u64) S[i]) << (56 - i * 8);
}

void store64(u8* S, u64 x) {
	int i;
	for (i = 0; i < 8; ++i)
		S[i] = (u8) (x >> (56 - i * 8));
}

void load32(u32* x, u8* S) {
	int i;
	*x = 0;
	for (i = 0; i < 4; ++i)
		*x |= ((u32) S[i]) << (24 - i * 8);
}

void store32(u8* S, u32 x) {
	int i;
	for (i = 0; i < 4; ++i)
		S[i] = (u8) (x >> (24 - i * 8));
}
void permutation384(u8* S, int rounds, u8 *c) {
	int i;
	u32 x00, x10, x20, x30;
	u64 x01, x11, x21, x31;
	u32 x40, x50, x60, x70;
	u64 x41, x51, x61, x71;

	load32(&x00, S + 36);
	load64(&x01, S + 40);

	load32(&x10, S + 24);
	load64(&x11, S + 28);

	load32(&x20, S + 12);
	load64(&x21, S + 16);

	load32(&x30, S + 0);
	load64(&x31, S + 4);

	u64 t1, t2, t3, t5, t6, t8, t9, t11;
	for (i = 0; i < rounds; ++i) {
		// addition of round constant
		x01 ^= c[i];
		sbox(x00, x10, x20, x30, x40, x50, x60, x70);
		sbox(x01, x11, x21, x31, x41, x51, x61, x71);
		// linear diffusion layer
		x00 = x40;
		x01 = x41;
		x10 = ROTR961(x50, x51, 1);
		x11 = ROTR962(x50, x51, 1);
		x20 = ROTR961(x60, x61, 8);
		x21 = ROTR962(x60, x61, 8);
		x30 = ROTR96MORE321(x70, x71, 55);
		x31 = ROTR96MORE322(x70, x71, 55);
	}
	store32(S + 36, x00);
	store64(S + 40, x01);
	store32(S + 24, x10);
	store64(S + 28, x11);
	store32(S + 12, x20);
	store64(S + 16, x21);
	store32(S + 0, x30);
	store64(S + 4, x31);
}
int crypto_hash(unsigned char *out, const unsigned char *in,
		unsigned long long inlen) {
	int nrh = 80;
	u32 i, j;
	int b = 384, r1 = 128, r2 = 128;
	u32 size = b / 8; //48    384=4*96=4*(u64+u32)
	u32 rate1 = r1 / 8;  //16
	u32 capacity = size - rate1;  //32  256/8=32
	u64 v = inlen / rate1 + 1;
	u32 rate2 = r2 / 8;  //16
	u32 u = CRYPTO_BYTES / rate2;  //32/4=8

	u8 M[v * rate1];
	u8 S[size];
	// pad in
	for (i = 0; i < inlen; ++i)
		M[i] = in[i];
	M[inlen] = 0x80;
	for (i = inlen + 1; i < v * rate1; ++i)
		M[i] = 0;
	// initialization
	for (i = 0; i < size; ++i)
		S[i] = 0;
	S[size - 1] ^= 1;
	//absorb
	if (inlen > 0) {
		for (i = 0; i < v; ++i) {
			for (j = capacity; j < size; ++j)
				S[j] ^= M[i * rate1 + j - capacity];
			permutation384(S, nrh, constant7);
		}
	}
	//sequeez
	for (i = 0; i < u - 1; ++i) {
		for (j = 0; j < rate2; ++j) {
			out[j + i * rate2] = S[j];
		}
		permutation384(S, nrh, constant7);
	}
	for (j = 0; j < rate2; ++j) {
		out[j + (u - 1) * rate2] = S[j];
	}
	return 0;
}

