/* Optimized (Intel Intrinsics) implementation of SPIX-128 AEAD
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#ifndef SLISCP_LIGHT256_H
#define SLISCP_LIGHT256_H

#include<stdint.h>
#include<x86intrin.h>
#include<immintrin.h>

#define STATE_SIZE	8 // 256/32 = 8//
#define SIMECK_ROUND	8
#define STEP_ROUND	18
#define PARAL_INST_BY4	1

static const uint8_t RC1_256[18]={0x8, 0x86, 0xe2, 0x89, 0xe6, 0xca, 0x17, 0x8e, 0x64, 0x6b, 0x6f, 0x2c, 0xdd, 0x99, 0xea, 0x0f, 0x04, 0x43};// Step constants (SC_{2i})
static const uint8_t RC2_256[18]={0x64, 0x6b, 0x6f, 0x2c, 0xdd, 0x99, 0xea, 0xf, 0x4, 0x43, 0xf1, 0x44, 0x73, 0xe5, 0x0b, 0x47, 0xb2, 0xb5};// Step constants (SC_{2i+1})
static const uint8_t rc1_256[18]={0xf, 0x4, 0x43, 0xf1, 0x44, 0x73, 0xe5, 0xb, 0x47, 0xb2, 0xb5, 0x37, 0x96, 0xee, 0x4c, 0xf5, 0x7, 0x82};// Round constants (RC_{2i})
static const uint8_t rc2_256[18]={0x47, 0xb2, 0xb5, 0x37, 0x96, 0xee, 0x4c, 0xf5, 0x7, 0x82, 0xa1, 0x78, 0xa2, 0xb9, 0xf2, 0x85, 0x23, 0xd9};// Round constants (RC_{2i+1})

typedef unsigned long long int u64;
typedef unsigned int u32;
typedef unsigned int u8;

#define ROT7(x) (_mm256_slli_epi32(x, 5) | _mm256_srli_epi32(x, 27))
#define ROT2(x) (_mm256_slli_epi32(x, 1) | _mm256_srli_epi32(x, 31))
#define SWAPREG1(x) (_mm256_permutevar8x32_epi32(x, _mm256_set_epi32(7, 5, 3, 1, 6, 4, 2, 0)))
#define RC(t1, t2) (_mm256_set_epi32(0xfffffffe^t2, 0xfffffffe^t1, 0xfffffffe^t2, 0xfffffffe^t1, 0xfffffffe^t2, 0xfffffffe^t1, 0xfffffffe^t2,0xfffffffe^t1))
#define SC(t1, t2) (_mm256_set_epi32(0xffffff00^t2, 0xffffffff, 0xffffff00^t1, 0xffffffff, 0xffffff00^t2, 0xffffffff, 0xffffff00^t1, 0xffffffff))
#define SWAPBLK(x) (_mm256_permute4x64_epi64(x, _MM_SHUFFLE(2,3,0,1)))
#define SWAPREG2(x) (_mm256_permutevar8x32_epi32(x, _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0)));

#define ROAX(x, y, t1, t2)\
{\
__m256i x2tmp;\
x2tmp = x;\
x = (ROT7(x)&x)^ROT2(x)^RC(t1, t2)^y;\
y = x2tmp;\
}

#define PACK_SSb(x, y)\
{\
__m256i xtmp, ytmp;\
xtmp = SWAPREG1(x);\
ytmp = SWAPREG1(y);\
x = _mm256_permute2x128_si256(xtmp,ytmp,0x20);\
y = _mm256_permute2x128_si256(xtmp,ytmp, 0x31);\
}

#define UNPACK_SSb(x, y)\
{\
__m256i xtmp, ytmp;\
xtmp = _mm256_unpacklo_epi32(x, y);\
ytmp = _mm256_unpackhi_epi32(x, y);\
x = _mm256_permute2x128_si256(xtmp, ytmp,0x20);\
y = _mm256_permute2x128_si256(xtmp, ytmp, 0x31);\
}


#define PACK(x, y, z, w)\
{\
__m256i x2tmp, x3tmp;\
x2tmp = SWAPREG2(x);\
x3tmp = SWAPREG2(z);\
x = _mm256_permute2x128_si256(x2tmp,x3tmp,0x20);\
z = _mm256_permute2x128_si256(x2tmp,x3tmp, 0x31);\
x2tmp = SWAPREG2(y);\
x3tmp = SWAPREG2(w);\
y = _mm256_permute2x128_si256(x2tmp,x3tmp,0x20);\
w = _mm256_permute2x128_si256(x2tmp,x3tmp, 0x31);\
}

#define UNPACK(x,y,z,w)\
{\
__m256i x2tmp, x3tmp;\
x2tmp = _mm256_unpacklo_epi64(x, z);\
x3tmp = _mm256_unpackhi_epi64(x, z);\
x = _mm256_permute2x128_si256(x2tmp,x3tmp,0x20);\
z = _mm256_permute2x128_si256(x2tmp,x3tmp, 0x31);\
x2tmp = _mm256_unpacklo_epi64(y, w);\
x3tmp = _mm256_unpackhi_epi64(y, w);\
y = _mm256_permute2x128_si256(x2tmp,x3tmp,0x20);\
w = _mm256_permute2x128_si256(x2tmp,x3tmp, 0x31);\
}

#define _mm256_storeu2_m128i(/* __m128i* */ hiaddr, /* __m128i* */ loaddr, \
		                             /* __m256i */ a) \
    do { \
	            __m256i _a = (a); \
	            _mm_storeu_si128((loaddr), _mm256_castsi256_si128(_a)); \
	            _mm_storeu_si128((hiaddr), _mm256_extractf128_si256(_a, 0x1)); \
	        } while (0)

void sliscp_light256( u32 *state );
void crypto_aead_encrypt( u32 *tag, u32 tlen, u32 *ciphertext, u32 *plaintext, u32 plen, u32 *ad, u32 adlen, u8 *nonce, u8 *key );
void crypto_aead_decrypt( u32 *plaintext,  u32 *ciphertext, u32 plen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *nonce, u8 *key);
#endif
