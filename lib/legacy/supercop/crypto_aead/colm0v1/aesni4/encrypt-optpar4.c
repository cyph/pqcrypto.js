/*
 * AES-COLM_0 v1 AES-NI/AVX2 implementation
 * for pipeline depth 4 (Skylake, Kaby Lake, ...)
 *
 * colm@colm.ae
 */

#include "crypto_aead.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <wmmintrin.h>
#include <immintrin.h>


/* 
 * Debugging support 
 */
#define PRINTB(x)       printblock(#x, (unsigned char*) x, 16, 0)
#define PRINTR(x)       printblock(#x, (unsigned char*) &x, 16, 1)
static void printblock(const char* caption, unsigned char* block, unsigned char nbytes, int reg)
{
	int i;
	printf("%s: ", caption);
	if (!reg) {
		for (i = 0; i < nbytes; i++) {
			printf("%02x", block[i]);
		}
	} else {
		for (i = nbytes-1; i >= 0; i--) {
			printf("%02x", block[i]);
		}
	}
	putchar('\n');
}

#define PRINTI(i)	printf("%s: %d\n", #i, i)

/* 
 * Auxiliary routines: operations on 128-bit blocks, multiplications, AES
 */
static __inline __m128i byte_swap(__m128i x)
{
	const __m128i BSWAP_MASK = _mm_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
	return _mm_shuffle_epi8(x, BSWAP_MASK);
}


static __m128i assist128(__m128i a, __m128i b)
{
    __m128i tmp = _mm_slli_si128 (a, 0x04);
    a = _mm_xor_si128 (a, tmp);
    tmp = _mm_slli_si128 (tmp, 0x04);
    a = _mm_xor_si128 (_mm_xor_si128 (a, tmp), _mm_slli_si128 (tmp, 0x04));
    return _mm_xor_si128 (a, _mm_shuffle_epi32 (b ,0xff));
}

static void AES_set_encrypt_key(const unsigned char *key,
                                __m128i *expkey)
{
    __m128i *sched = (__m128i*) expkey;
    sched[ 0] = _mm_loadu_si128((__m128i*)key);
    sched[ 1] = assist128(sched[0], _mm_aeskeygenassist_si128(sched[0],0x1));
    sched[ 2] = assist128(sched[1], _mm_aeskeygenassist_si128(sched[1],0x2));
    sched[ 3] = assist128(sched[2], _mm_aeskeygenassist_si128(sched[2],0x4));
    sched[ 4] = assist128(sched[3], _mm_aeskeygenassist_si128(sched[3],0x8));
    sched[ 5] = assist128(sched[4], _mm_aeskeygenassist_si128(sched[4],0x10));
    sched[ 6] = assist128(sched[5], _mm_aeskeygenassist_si128(sched[5],0x20));
    sched[ 7] = assist128(sched[6], _mm_aeskeygenassist_si128(sched[6],0x40));
    sched[ 8] = assist128(sched[7], _mm_aeskeygenassist_si128(sched[7],0x80));
    sched[ 9] = assist128(sched[8], _mm_aeskeygenassist_si128(sched[8],0x1b));
    sched[10] = assist128(sched[9], _mm_aeskeygenassist_si128(sched[9],0x36));
}

static void AES_set_decrypt_key(const __m128i* enckey, __m128i* deckey)
{
	int r;

	deckey[0] = enckey[10];
	for (r = 1; r < 10; r++) {
		deckey[r] = _mm_aesimc_si128(enckey[10-r]);
	}
	deckey[10] = enckey[0];
}


static __m128i AES_encrypt(__m128i in,  const __m128i* expkey)
{
	int j;

	__m128i tmp = byte_swap(in) ^ expkey[0];
	for (j=1; j <10; j++){
		tmp = _mm_aesenc_si128 (tmp,expkey[j]);
	}
	tmp = _mm_aesenclast_si128 (tmp,expkey[10]);

	return byte_swap(tmp);
}

static __m128i AES_decrypt(__m128i in,  const __m128i* expkey)
{
	int j;

	__m128i tmp = byte_swap(in) ^ expkey[0];
	for (j=1; j <10; j++){
		tmp = _mm_aesdec_si128 (tmp,expkey[j]);
	}
	tmp = _mm_aesdeclast_si128 (tmp,expkey[10]);

	return byte_swap(tmp);
}

/* constant-time doubling in GF(2^128) */
static __m128i gf128_mul2(const __m128i x)
{
    const __m128i REDPOLY = _mm_set_epi64x(0, 0x87);
    const __m128i ZERO = _mm_setzero_si128();
	__m128i x2;

    __m128i mask = _mm_cmpgt_epi32(ZERO, x);
    mask = _mm_shuffle_epi32(mask, 0xff);

    x2 = _mm_slli_epi64(x, 1) | _mm_srli_epi64(_mm_slli_si128(x, 8), 63);

    return x2 ^ (REDPOLY & mask);
}

static __m128i gf128_mul3(const __m128i x)
{
	return gf128_mul2(x) ^ x;
}

static __m128i gf128_mul7(const __m128i x)
{
	__m128i x2 = gf128_mul2(x); 
	__m128i x4 = gf128_mul2(x2);

	return x4 ^ x2 ^ x;
}

/*  endian bswap */
#define BSWAP_BLOCKS4(b1,b2,b3,b4) do { \
	b1 = byte_swap(b1); \
	b2 = byte_swap(b2); \
	b3 = byte_swap(b3); \
	b4 = byte_swap(b4); \
} while (0)

#define BSWAP_BLOCKS8(b1,b2,b3,b4,b5,b6,b7,b8) do { \
	b1 = byte_swap(b1); \
	b2 = byte_swap(b2); \
	b3 = byte_swap(b3); \
	b4 = byte_swap(b4); \
	b5 = byte_swap(b5); \
	b6 = byte_swap(b6); \
	b7 = byte_swap(b7); \
	b8 = byte_swap(b8); \
} while (0)

#define AES_ENCRYPT4(block1,block2,block3,block4,expkey) do { \
	int r; \
	BSWAP_BLOCKS4(block1,block2,block3,block4); \
	block1 ^=  expkey[0]; \
	block2 ^=  expkey[0]; \
	block3 ^=  expkey[0]; \
	block4 ^=  expkey[0]; \
	for(r = 1; r < 10; r++){ \
		block1 = _mm_aesenc_si128 (block1,expkey[r]); \
		block2 = _mm_aesenc_si128 (block2,expkey[r]); \
		block3 = _mm_aesenc_si128 (block3,expkey[r]); \
		block4 = _mm_aesenc_si128 (block4,expkey[r]); \
	} \
	block1 = _mm_aesenclast_si128 (block1,expkey[10]); \
	block2 = _mm_aesenclast_si128 (block2,expkey[10]); \
	block3 = _mm_aesenclast_si128 (block3,expkey[10]); \
	block4 = _mm_aesenclast_si128 (block4,expkey[10]); \
	BSWAP_BLOCKS4(block1,block2,block3,block4); \
} while (0)

#define AES_ENCRYPT8(block1,block2,block3,block4,block5,block6,block7,block8,expkey) do { \
	int r; \
	BSWAP_BLOCKS8(block1,block2,block3,block4,block5,block6,block7,block8); \
	block1 ^=  expkey[0]; \
	block2 ^=  expkey[0]; \
	block3 ^=  expkey[0]; \
	block4 ^=  expkey[0]; \
	block5 ^=  expkey[0]; \
	block6 ^=  expkey[0]; \
	block7 ^=  expkey[0]; \
	block8 ^=  expkey[0]; \
	for(r = 1; r < 10; r++){ \
		block1 = _mm_aesenc_si128 (block1,expkey[r]); \
		block2 = _mm_aesenc_si128 (block2,expkey[r]); \
		block3 = _mm_aesenc_si128 (block3,expkey[r]); \
		block4 = _mm_aesenc_si128 (block4,expkey[r]); \
		block5 = _mm_aesenc_si128 (block5, expkey[r]); \
		block6 = _mm_aesenc_si128 (block6, expkey[r]); \
		block7 = _mm_aesenc_si128 (block7, expkey[r]); \
		block8 = _mm_aesenc_si128 (block8, expkey[r]); \
	} \
	block1 = _mm_aesenclast_si128 (block1,expkey[10]); \
	block2 = _mm_aesenclast_si128 (block2,expkey[10]); \
	block3 = _mm_aesenclast_si128 (block3,expkey[10]); \
	block4 = _mm_aesenclast_si128 (block4,expkey[10]); \
	block5 = _mm_aesenclast_si128 (block5 ,expkey[10]); \
	block6 = _mm_aesenclast_si128 (block6 ,expkey[10]); \
	block7 = _mm_aesenclast_si128 (block7 ,expkey[10]); \
	block8 = _mm_aesenclast_si128 (block8 ,expkey[10]); \
	BSWAP_BLOCKS8(block1,block2,block3,block4,block5,block6,block7,block8); \
} while (0)

#define AES_DECRYPT4(block1,block2,block3,block4,expkey) do { \
	int r; \
	BSWAP_BLOCKS4(block1,block2,block3,block4); \
	block1 ^=  expkey[0]; \
	block2 ^=  expkey[0]; \
	block3 ^=  expkey[0]; \
	block4 ^=  expkey[0]; \
	for(r = 1; r < 10; r++){ \
		block1 = _mm_aesdec_si128 (block1,expkey[r]); \
		block2 = _mm_aesdec_si128 (block2,expkey[r]); \
		block3 = _mm_aesdec_si128 (block3,expkey[r]); \
		block4 = _mm_aesdec_si128 (block4,expkey[r]); \
	} \
	block1 = _mm_aesdeclast_si128 (block1,expkey[10]); \
	block2 = _mm_aesdeclast_si128 (block2,expkey[10]); \
	block3 = _mm_aesdeclast_si128 (block3,expkey[10]); \
	block4 = _mm_aesdeclast_si128 (block4,expkey[10]); \
	BSWAP_BLOCKS4(block1,block2,block3,block4); \
} while (0)

/* in-place rho */
#define rho(block, W, newW) do { \
	/* (Y,W') = rho(block, W) */ \
	/* W' = block+2*W */ \
	/* Y =  block+3*W = W'+W */ \
	newW = gf128_mul2(W) ^ block; \
	block = newW ^ W; \
	W = newW; \
} while(0) 

/* in-place rho inverse */
#define rho_inv(block, W, newW) do { \
	/* (X,W') = rho^-1(block, W) */ \
	/* W' = block + W */ \
	newW = gf128_mul2(W); \
	W ^= block; \
	/* X =  block + 3*W = block + W + 2*W */ \
	block = newW ^ W; \
} while(0) 


/*
 * COLM's AD processing
 */

__m128i mac(const unsigned char* in, unsigned long long len, 
		const unsigned char* nonce,
		const __m128i LL,
		const __m128i* expkey)
{
	__m128i v, block, delta;
	__m128i delta1, delta2, delta3, delta4; 
	__m128i block1, block2, block3, block4;
	uint8_t buf[16] = { 0 };

	delta = gf128_mul3(LL);
	v = _mm_set_epi64x(0, *(uint64_t*)nonce);
	v = byte_swap(v);
	v ^= delta;
	v = AES_encrypt(v, expkey);

	/* mac blocks in parallel */
	delta4 = delta;
	while (len >= 4*16) {
		delta1 = gf128_mul2(delta4);
		delta2 = gf128_mul2(delta1);
		delta3 = gf128_mul2(delta2);
		delta4 = gf128_mul2(delta3);

		block1 = byte_swap(_mm_loadu_si128((__m128i*)in));
		block2 = byte_swap(_mm_loadu_si128((__m128i*)in+1));
		block3 = byte_swap(_mm_loadu_si128((__m128i*)in+2));
		block4 = byte_swap(_mm_loadu_si128((__m128i*)in+3));

		block1 ^= delta1;
		block2 ^= delta2;
		block3 ^= delta3;
		block4 ^= delta4;

		AES_ENCRYPT4(block1,block2,block3,block4, expkey);

		v ^= block1 ^ block2 ^ block3 ^ block4;

		in += 4*16;
		len -= 4*16;
	}

	/* mac full blocks */
	delta = delta4;
	while (len >= 16) {
		delta = gf128_mul2(delta);
		block = _mm_loadu_si128((__m128i*)in);
		block = byte_swap(block);
		block ^= delta;
		block = AES_encrypt(block, expkey);
		v ^= block;

		in += 16;
		len -= 16;
	}

	if (len > 0) { /* last block partial */
		delta = gf128_mul7(delta);
		memcpy(buf, in, len);
		buf[len] ^= 0x80; /* padding */
		block = _mm_loadu_si128((__m128i*) buf);
		block = byte_swap(block);
		block ^= delta;

		block = AES_encrypt(block, expkey);
		v ^= block;
	}

	return v;
}


/*
 * main encryption and decryption routines
 */

int crypto_aead_encrypt(
       unsigned char *c,unsigned long long *clen,
       const unsigned char *m,unsigned long long mlen,
       const unsigned char *ad,unsigned long long adlen,
       const unsigned char *nsec,
       const unsigned char *npub,
       const unsigned char *k
     )
{
	const unsigned char* in = m;
	unsigned char* out = c;
	unsigned long long remaining = mlen;
	uint8_t buf[16] = { 0 };

	__m128i W, Wtmp;
	__m128i block, Lup, Ldown;
	__m128i Lup1, Lup2, Lup3, Lup4;
	__m128i Ldown1, Ldown2, Ldown3, Ldown4;
	__m128i block1, block2, block3, block4;
	__m128i checksum = { 0 };
	__m128i LL = { 0 };
	__m128i expkey[11];

	/* key schedule */
	AES_set_encrypt_key(k, expkey);

	/* set ouput length */
	*clen = mlen + 16;

	LL = AES_encrypt(LL, expkey); /* LL = AES(0) */
	Lup = LL;
	Ldown = gf128_mul3(gf128_mul3(LL)); /* Ldown = 3^2*LL */

	/* mac AD + nonce */
	W = mac(ad, adlen, npub, LL, expkey);

	Lup4 = Lup;
	Ldown4 = Ldown;
	while (remaining > 4*16) {
		Lup1 = gf128_mul2(Lup4);
		Lup2 = gf128_mul2(Lup1);
		Lup3 = gf128_mul2(Lup2);
		Lup4 = gf128_mul2(Lup3);

		block1 = byte_swap(_mm_loadu_si128((__m128i*)in));
		block2 = byte_swap(_mm_loadu_si128((__m128i*)in+1));
		block3 = byte_swap(_mm_loadu_si128((__m128i*)in+2));
		block4 = byte_swap(_mm_loadu_si128((__m128i*)in+3));

		checksum ^= block1 ^ block2 ^ block3 ^ block4;

		block1 ^= Lup1;
		block2 ^= Lup2;
		block3 ^= Lup3;
		block4 ^= Lup4;

		AES_ENCRYPT4(block1,block2,block3,block4, expkey);

		Ldown1 = gf128_mul2(Ldown4);
		Ldown2 = gf128_mul2(Ldown1);
		Ldown3 = gf128_mul2(Ldown2);
		Ldown4 = gf128_mul2(Ldown3);

		rho(block1, W, Wtmp);
		rho(block2, W, Wtmp);
		rho(block3, W, Wtmp);
		rho(block4, W, Wtmp);

		AES_ENCRYPT4(block1,block2,block3,block4, expkey);

		block1 ^= Ldown1;
		block2 ^= Ldown2;
		block3 ^= Ldown3;
		block4 ^= Ldown4;

		_mm_storeu_si128((__m128i*)out, byte_swap(block1));
		_mm_storeu_si128((__m128i*)out+1, byte_swap(block2));
		_mm_storeu_si128((__m128i*)out+2, byte_swap(block3));
		_mm_storeu_si128((__m128i*)out+3, byte_swap(block4));

		in += 4*16;
		out += 4*16;
		remaining -= 4*16;
	}

	Lup = Lup4;
	Ldown = Ldown4;

	while (remaining > 16) {
		Lup = gf128_mul2(Lup);
		Ldown = gf128_mul2(Ldown);

		block = _mm_loadu_si128((__m128i*)in);
		block = byte_swap(block);
		checksum ^= block;
		block ^= Lup;
		block = AES_encrypt(block, expkey);

		/* (Y,W') = rho(block, W) */
		rho(block, W, Wtmp);

		block = AES_encrypt(block, expkey);
		block ^= Ldown;
		_mm_storeu_si128((__m128i*)out, byte_swap(block));

		in += 16;
		out += 16;
		remaining -= 16;
	}

	/* XOR checksum into last block (padded if necessary) */
	memcpy(buf, in, remaining);

	Lup = gf128_mul7(Lup);
	Ldown = gf128_mul7(Ldown);
	if (remaining < 16) {
		buf[remaining] = 0x80;
		Lup = gf128_mul7(Lup);
		Ldown = gf128_mul7(Ldown);
	}
	block = _mm_loadu_si128((__m128i*)buf);
	block = byte_swap(block);
	checksum ^= block;

	block = checksum ^ Lup;
	block = AES_encrypt(block, expkey);

	/* (Y,W') = rho(block, W) */
	rho(block, W, Wtmp);

	block = AES_encrypt(block, expkey);
	block ^= Ldown;
	_mm_storeu_si128((__m128i*)out, byte_swap(block));
	out += 16;

	/* stop here if remaining == 0 */
	if (remaining == 0) return 0;

	/* Duplicate last block */
	Lup = gf128_mul2(Lup);
	Ldown = gf128_mul2(Ldown);

	block = checksum ^ Lup;
	block = AES_encrypt(block, expkey);

	/* (Y,W') = rho(block, W) */
	rho(block, W, Wtmp);

	block = AES_encrypt(block, expkey);
	block ^= Ldown;

	_mm_storeu_si128((__m128i*)buf, byte_swap(block));
	memcpy(out, buf, remaining);

	return 0;
}


int crypto_aead_decrypt(
   unsigned char *m,unsigned long long *mlen,
   unsigned char *nsec,
   const unsigned char *c,unsigned long long clen,
   const unsigned char *ad,unsigned long long adlen,
   const unsigned char *npub,
   const unsigned char *k
   )
{
	const unsigned char* in = c;
	unsigned char* out = m;
	unsigned long long remaining;
	unsigned int i;
	uint8_t buf[16] = { 0 };

	__m128i W, Wtmp;
	__m128i block, Lup, Ldown;
	__m128i Lup1, Lup2, Lup3, Lup4;
	__m128i Ldown1, Ldown2, Ldown3, Ldown4;
	__m128i block1, block2, block3, block4;
	__m128i checksum = { 0 };
	__m128i LL = { 0 };
	__m128i expkey[11];
	__m128i decexpkey[11];

	/* key schedule */
	AES_set_encrypt_key(k, expkey);
	AES_set_decrypt_key(expkey, decexpkey);

	if (clen < 16) {
		return -1;
	}
	/* Set output length */
	*mlen = remaining = clen - 16;

	LL = AES_encrypt(LL, expkey); /* LL = AES(0) */
	Lup = LL;
	Ldown = gf128_mul3(gf128_mul3(LL)); /* Ldown = 3^2*LL */

	/* mac AD + nonce */
	W = mac(ad, adlen, npub, LL, expkey);

	/* Decrypt 4 blocks parallel */
	Lup4 = Lup;
	Ldown4 = Ldown;
	while (remaining > 4*16) {
		Ldown1 = gf128_mul2(Ldown4);
		Ldown2 = gf128_mul2(Ldown1);
		Ldown3 = gf128_mul2(Ldown2);
		Ldown4 = gf128_mul2(Ldown3);

		block1 = byte_swap(_mm_loadu_si128((__m128i*)in));
		block2 = byte_swap(_mm_loadu_si128((__m128i*)in+1));
		block3 = byte_swap(_mm_loadu_si128((__m128i*)in+2));
		block4 = byte_swap(_mm_loadu_si128((__m128i*)in+3));

		block1 ^= Ldown1;
		block2 ^= Ldown2;
		block3 ^= Ldown3;
		block4 ^= Ldown4;

		AES_DECRYPT4(block1,block2,block3,block4, decexpkey);

		Lup1 = gf128_mul2(Lup4);
		Lup2 = gf128_mul2(Lup1);
		Lup3 = gf128_mul2(Lup2);
		Lup4 = gf128_mul2(Lup3);

		rho_inv(block1, W, Wtmp);
		rho_inv(block2, W, Wtmp);
		rho_inv(block3, W, Wtmp);
		rho_inv(block4, W, Wtmp);

		AES_DECRYPT4(block1,block2,block3,block4, decexpkey);

		block1 ^= Lup1;
		block2 ^= Lup2;
		block3 ^= Lup3;
		block4 ^= Lup4;

		checksum ^= block1 ^ block2 ^ block3 ^ block4;

		_mm_storeu_si128((__m128i*)out, byte_swap(block1));
		_mm_storeu_si128((__m128i*)out+1, byte_swap(block2));
		_mm_storeu_si128((__m128i*)out+2, byte_swap(block3));
		_mm_storeu_si128((__m128i*)out+3, byte_swap(block4));

		in += 4*16;
		out += 4*16;
		remaining -= 4*16;
	}

	Lup = Lup4;
	Ldown = Ldown4;
	
	/* Decrypt */
	while (remaining > 16) {
		Lup = gf128_mul2(Lup);
		Ldown = gf128_mul2(Ldown);

		block = byte_swap(_mm_loadu_si128((__m128i*)in));
		block ^= Ldown;
		block = AES_decrypt(block, decexpkey);

		/* (X,W') = rho^-1(block, W) */
		rho_inv(block, W, Wtmp);

		block = AES_decrypt(block, decexpkey);
		block ^= Lup;
		checksum ^= block;

		_mm_storeu_si128((__m128i*)out, byte_swap(block));

		in += 16;
		out += 16;
		remaining -= 16;
	}

	Lup = gf128_mul7(Lup);
	Ldown = gf128_mul7(Ldown);
	if (remaining < 16) {
		Lup = gf128_mul7(Lup);
		Ldown = gf128_mul7(Ldown);
	}

	block = _mm_loadu_si128((__m128i*)in);
	block = byte_swap(block);
	block ^= Ldown;
	block = AES_decrypt(block, decexpkey);

	/* (X,W') = rho^-1(block, W) */
	rho_inv(block, W, Wtmp);

	block = AES_decrypt(block, decexpkey);
	block ^= Lup;
	/* block now contains M[l] = M[l+1] */

	checksum ^= block;
	/* checksum now contains M*[l] */
	in += 16;

	/* output last (maybe partial) plaintext block */
	_mm_storeu_si128((__m128i*)buf, byte_swap(checksum));
	memcpy(out, buf, remaining);

	/* work on M[l+1] */
	Lup = gf128_mul2(Lup);
	Ldown = gf128_mul2(Ldown);

	block ^= Lup;
	block = AES_encrypt(block, expkey);
	/* (Y,W') = rho(block, W) */
	rho(block, W, Wtmp);

	block = AES_encrypt(block, expkey);
	block ^= Ldown;
	/* block now contains C'[l+1] */

	_mm_storeu_si128((__m128i*)buf, byte_swap(block));
	if (memcmp(in, buf, remaining) != 0) {
		return -1;
	} 

	if (remaining < 16) {
		_mm_storeu_si128((__m128i*)buf, byte_swap(checksum));
		if (buf[remaining] != 0x80) {
			return -1;
		}
		for (i = remaining+1; i < 16; i++) {
			if (buf[i] != 0) {
				return -1;
			}
		}
	}

	return 0;
}

