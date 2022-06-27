#include"api.h"
#include"crypto_aead.h"

typedef unsigned char u8;
typedef unsigned long long u64;
typedef long long i64;
typedef unsigned int u32;

#define RATE (96 / 8)
#define PR0_ROUNDS 76
#define PR_ROUNDS 40
#define PRF_ROUNDS 44
#define sbox(a, b, c, d, e, f, g, h)                                                                            \
{                                                                                                                             \
	t1 = ~a; t2 = b & t1;t3 = c ^ t2; h = d ^ t3; t5 = b | c; t6 = d ^ t1; g = t5 ^ t6; t8 = b ^ d; t9 = t3 & t6; e = t8 ^ t9; t11 = g & t8; f = t3 ^ t11; \
}

#define ROTR64(x,n) (((x)>>(n))|((x)<<(64-(n))))
#define ROTR32(x,n) (((x)>>(n))|((x)<<(32-(n))))
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))
#define ROTR961(a,b,n) (((a)<<(n))|((b)>>(64-n)))
#define ROTR962(a,b,n) (((b)<<(n))|((a)>>(32-n)))

#define ROTR96MORE321(a,b,n) ((b<<(n-32))>>32)
#define ROTR96MORE322(a,b,n) (b<<n|(u64)a<<(n-32)|b>>(96-n))

#define LITTLE_ENDIAN
//#define BIG_ENDIAN
#ifdef BIG_ENDIAN
#define EXT_BYTE32(x,n) ((u8)((u32)(x)>>(8*(n))))
#define INS_BYTE32(x,n) ((u32)(x)<<(8*(n)))
#define U32BIG(x) (x)
#define EXT_BYTE64(x,n) ((u8)((u64)(x)>>(8*(n))))
#define INS_BYTE64(x,n) ((u64)(x)<<(8*(n)))
#define U64BIG(x) (x)
#endif

#ifdef LITTLE_ENDIAN
#define EXT_BYTE32(x,n) ((u8)((u32)(x)>>(8*(3-(n)))))
#define INS_BYTE32(x,n) ((u32)(x)<<(8*(3-(n))))
#define U32BIG(x) \
    ((ROTR32(x,  8) & (0xFF00FF00)) | \
    ((ROTR32(x, 24) & (0x00FF00FF))))
#define EXT_BYTE64(x,n) ((u8)((u64)(x)>>(8*(7-(n)))))
#define INS_BYTE64(x,n) ((u64)(x)<<(8*(7-(n))))
#define U64BIG(x) \
    ((ROTR64(x, 8) & (0xFF000000FF000000ULL)) | \
     (ROTR64(x,24) & (0x00FF000000FF0000ULL)) | \
     (ROTR64(x,40) & (0x0000FF000000FF00ULL)) | \
     (ROTR64(x,56) & (0x000000FF000000FFULL)))
#endif

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

#define ROUND384(i) ({\
		x31^=constant7[i];\
		sbox(x30, x20, x10, x00, b30, b20, b10, b00);\
		sbox(x31, x21, x11, x01, b31, b21, b11, b01);\
		x30=b30;\
		x20=ROTR961(b20,b21,1);\
		x10=ROTR961(b10,b11,8);\
		x00=ROTR96MORE321(b00,b01,55);\
		x31=b31;\
		x21=ROTR962(b20,b21,1);\
		x11=ROTR962(b10,b11,8);\
		x01=ROTR96MORE322(b00,b01,55);\
})

int crypto_aead_encrypt(unsigned char *c, unsigned long long *clen,
		const unsigned char *m, unsigned long long mlen,
		const unsigned char *ad, unsigned long long adlen,
		const unsigned char *nsec, const unsigned char *npub,
		const unsigned char *k) {

	u64 rlen, i; //RATE=96/8=12
	u64 b01, b11, b21, b31;
	u32 b00, b10, b20, b30;
	u64 t1, t2, t3, t5, t6, t8, t9, t11;
	u32 x30 = 0, x20 = 0, x10 = 0, x00 = 0;
	u64 x31 = 0, x21 = 0, x11 = 0, x01 = 0;

	// initialization
	x00 = U32BIG(*(u32* )(npub));
	x01 = U64BIG(*(u64* )(npub + 4));
	x10 = U32BIG(*(u32* )(npub + 12));
	x11 = U64BIG(*(u64* )(npub + 16));
	x20 = U32BIG(*(u32* )(k));
	x21 = U64BIG(*(u64* )(k + 4));
	x30 = U32BIG(*(u32* )(k + 12));
	x31 = U64BIG(*(u64* )(k + 16));

	for (i = 0; i < PR0_ROUNDS; i++) {
		ROUND384(i);
	}
	// process associated data
	if (adlen) {
		rlen = adlen;
		while (rlen >= RATE) {
			x00 ^= U32BIG(*(u32* )(ad));
			x01 ^= U64BIG(*(u64* )(ad + 4));
			for (i = 0; i < PR_ROUNDS; i++) {
				ROUND384(i);
			}
			rlen -= RATE;
			ad += RATE;
		}
		for (i = 0; i < rlen; ++i, ++ad) {
			if (i >= 4) {
				x01 ^= INS_BYTE64(*ad, i - 4);
			} else {
				x00 ^= INS_BYTE32(*ad, i);
			}
		}
		if (rlen >= 4) {
			x01 ^= INS_BYTE64(0x80, rlen - 4);
		} else {
			x00 ^= INS_BYTE32(0x80, rlen);
		}

		for (i = 0; i < PR_ROUNDS; i++) {
			ROUND384(i);
		}
	}
	x31 ^= 1;
	// process plaintext
	if (mlen) {
		rlen = mlen;
		while (rlen >= RATE) {
			x00 ^= U32BIG(*(u32* )(m));
			x01 ^= U64BIG(*(u64* )(m + 4));
			*(u32*) c = U32BIG(x00);
			*(u64*) (c + 4) = U64BIG(x01);

			for (i = 0; i < PR_ROUNDS; i++) {
				ROUND384(i);
			}
			rlen -= RATE;
			m += RATE;
			c += RATE;
		}
		for (i = 0; i < rlen; ++i, ++m, ++c) {
			if (i >= 4) {
				x01 ^= INS_BYTE64(*m, i - 4);
				*c = EXT_BYTE64(x01, i - 4);
			} else {
				x00 ^= INS_BYTE32(*m, i);
				*c = EXT_BYTE32(x00, i);
			}
		}

		if (rlen >= 4) {
			x01 ^= INS_BYTE64(0x80, rlen - 4);
		} else {
			x00 ^= INS_BYTE32(0x80, rlen);
		}
	}
	// finalization
	for (i = 0; i < PRF_ROUNDS; i++) {
		ROUND384(i);
	}
	// return tag
	*(u32*) c = U32BIG(x00);
	*(u64*) (c + 4) = U64BIG(x01);
	*(u32*) (c + 12) = U32BIG(x10);
	*(u64*) (c + 16) = U64BIG(x11);
	*clen = mlen + CRYPTO_KEYBYTES;
	return 0;
}
int crypto_aead_decrypt(unsigned char *m, unsigned long long *mlen,
		unsigned char *nsec, const unsigned char *c, unsigned long long clen,
		const unsigned char *ad, unsigned long long adlen,
		const unsigned char *npub, const unsigned char *k) {

	u64 rlen, i; //RATE=96/8=12
	u64 b01, b11, b21, b31;
	u32 b00, b10, b20, b30;
	u64 t1, t2, t3, t5, t6, t8, t9, t11;
	u32 x30 = 0, x20 = 0, x10 = 0, x00 = 0;
	u64 x31 = 0, x21 = 0, x11 = 0, x01 = 0;

	// initialization
	x00 = U32BIG(*(u32* )(npub));
	x01 = U64BIG(*(u64* )(npub + 4));
	x10 = U32BIG(*(u32* )(npub + 12));
	x11 = U64BIG(*(u64* )(npub + 16));
	x20 = U32BIG(*(u32* )(k));
	x21 = U64BIG(*(u64* )(k + 4));
	x30 = U32BIG(*(u32* )(k + 12));
	x31 = U64BIG(*(u64* )(k + 16));
	for (i = 0; i < PR0_ROUNDS; i++) {
		ROUND384(i);
	}
	// process associated data
	if (adlen) {
		rlen = adlen;
		while (rlen >= RATE) {
			x00 ^= U32BIG(*(u32* )(ad));
			x01 ^= U64BIG(*(u64* )(ad + 4));
			for (i = 0; i < PR_ROUNDS; i++) {
				ROUND384(i);
			}
			rlen -= RATE;
			ad += RATE;
		}
		for (i = 0; i < rlen; ++i, ++ad) {
			if (i >= 4) {
				x01 ^= INS_BYTE64(*ad, i - 4);
			} else {
				x00 ^= INS_BYTE32(*ad, i);
			}
		}
		if (rlen >= 4) {
			x01 ^= INS_BYTE64(0x80, rlen - 4);
		} else {
			x00 ^= INS_BYTE32(0x80, rlen);
		}

		for (i = 0; i < PR_ROUNDS; i++) {
			ROUND384(i);
		}
	}
	x31 ^= 1;
	// process c
	rlen = clen - CRYPTO_KEYBYTES;
	if (rlen>0) {
		while (rlen >= RATE) {
			*(u32*) (m) = U32BIG(x00) ^ (*(u32*) (c));
			*(u64*) (m + 4) = U64BIG(x01) ^ (*(u64*) (c + 4));

			x00 = U32BIG(*(u32* )(c));
			x01 = U64BIG(*(u64* )(c + 4));
			for (i = 0; i < PR_ROUNDS; i++) {
				ROUND384(i);
			}
			rlen -= RATE;
			m += RATE;
			c += RATE;
		}

		for (i = 0; i < rlen; ++i, ++m, ++c) {
			if (i >= 4) {
				*m = EXT_BYTE64(x01, i-4) ^ *c;
				x01 &= ~INS_BYTE64(0xff, i - 4);
				x01 |= INS_BYTE64(*c, i - 4);
			} else {
				*m = EXT_BYTE32(x00, i) ^ *c;
				x00 &= ~INS_BYTE32(0xff, i);
				x00 |= INS_BYTE32(*c, i);
			}
		}
		if (rlen >= 4) {
			x01 ^= INS_BYTE64(0x80, rlen - 4);
		} else {
			x00 ^= INS_BYTE32(0x80, rlen);
		}
	}
	// finalization
	for (i = 0; i < PRF_ROUNDS; i++) {
		ROUND384(i);
	}
	// return -1 if verification fails
	if (*(u32*) (c) != U32BIG(x00) || *(u64*) (c + 4) != U64BIG(x01)
			|| *(u32*) (c + 12) != U32BIG(x10)
			|| *(u64*) (c + 16) != U64BIG(x11))
		return -1;
	// return plaintext
	*mlen = clen - CRYPTO_KEYBYTES;

	return 0;
}


