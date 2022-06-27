/* Optimized (Intel Intrinsics) implementation of SPIX-128 AEAD
   Written by:
   Kalikinkar Mandal <kmandal@uwaterloo.ca>
*/

#ifndef SLISCP_LIGHT256_H
#define SLISCP_LIGHT256_H

#include<stdint.h>
#include<x86intrin.h>
#include<smmintrin.h>

#define STATE_SIZE	8 // 256/32 = 8//
#define SIMECK_ROUND	8
#define STEP_ROUND	18
#define PARAL_INST_BY2	1


static const uint8_t RC1_256[18]={0x8, 0x86, 0xe2, 0x89, 0xe6, 0xca, 0x17, 0x8e, 0x64, 0x6b, 0x6f, 0x2c, 0xdd, 0x99, 0xea, 0x0f, 0x04, 0x43};// Step constants (SC_{2i})
static const uint8_t RC2_256[18]={0x64, 0x6b, 0x6f, 0x2c, 0xdd, 0x99, 0xea, 0xf, 0x4, 0x43, 0xf1, 0x44, 0x73, 0xe5, 0x0b, 0x47, 0xb2, 0xb5};// Step constants (SC_{2i+1})
static const uint8_t rc1_256[18]={0xf, 0x4, 0x43, 0xf1, 0x44, 0x73, 0xe5, 0xb, 0x47, 0xb2, 0xb5, 0x37, 0x96, 0xee, 0x4c, 0xf5, 0x7, 0x82};// Round constants (RC_{2i})
static const uint8_t rc2_256[18]={0x47, 0xb2, 0xb5, 0x37, 0x96, 0xee, 0x4c, 0xf5, 0x7, 0x82, 0xa1, 0x78, 0xa2, 0xb9, 0xf2, 0x85, 0x23, 0xd9};// Round constants (RC_{2i+1})


typedef unsigned long long int u64;
typedef unsigned int u32;
typedef unsigned int u8;

#define ROT7(x)		(_mm_slli_epi32(x, 5) | _mm_srli_epi32(x, 27))
#define ROT2(x) 	(_mm_slli_epi32(x, 1) | _mm_srli_epi32(x, 31))
#define RC(t1, t2)	(_mm_set_epi32(0xfffffffe^t2, 0xfffffffe^t1, 0xfffffffe^t2, 0xfffffffe^t1))
#define SC(t1, t2)	(_mm_set_epi32(0xffffff00^t2, 0xffffffff, 0xffffff00^t1, 0xffffffff ))
#define SWAPREG1(x)	(_mm_shuffle_epi32(x, _MM_SHUFFLE(3, 1, 2, 0)))
#define SWAPBLK(x)	(_mm_slli_si128(x, 8)|_mm_srli_si128(x, 8))

#define ROAX(x, y, t1, t2)\
{\
__m128i xtmp;\
xtmp = x;\
x = (ROT7(x)&x)^ROT2(x)^RC(t1, t2)^y;\
y = xtmp;\
}

#define PACK_SSb(x, y)\
{\
__m128i xtmp, ytmp;\
xtmp = SWAPREG1(x);\
ytmp = SWAPREG1(y);\
x = _mm_unpacklo_epi64(xtmp, ytmp);\
y = _mm_unpackhi_epi64(xtmp, ytmp);\
}


#define UNPACK_SSb(x, y)\
{\
__m128i xtmp, ytmp;\
xtmp = _mm_unpacklo_epi32(x, y);\
ytmp = _mm_unpackhi_epi32(x, y);\
x = xtmp;\
y = ytmp;\
}

#define PACK(x, y, z, w, state, i1, i2, i3, i4)\
{\
__m128i xtmp, ytmp;\
xtmp = _mm_loadu_si128((void *) (state + i1));\
ytmp = _mm_loadu_si128((void *) (state + i2));\
x = _mm_unpacklo_epi64(xtmp, ytmp);\
z = _mm_unpackhi_epi64(xtmp, ytmp);\
xtmp = _mm_loadu_si128((void *) (state + i3));\
ytmp = _mm_loadu_si128((void *) (state + i4));\
y = _mm_unpacklo_epi64(xtmp, ytmp);\
w = _mm_unpackhi_epi64(xtmp, ytmp);\
}

#define UNPACK(x, y, z, w)\
{\
__m128i xtmp, ytmp;\
xtmp = _mm_unpacklo_epi64(x, z);\
ytmp = _mm_unpackhi_epi64(x, z);\
x = xtmp;\
z = ytmp;\
xtmp = _mm_unpacklo_epi64(y, w);\
ytmp = _mm_unpackhi_epi64(y, w);\
y = xtmp;\
w = ytmp;\
}

#define INIT_AND_PACK(x, y, z, w, key, nonce, index)\
{\
__m128i xtmp, ytmp;\
xtmp = _mm_set_epi32 (nonce[index+0], nonce[index+1], key[index], key[index+1] );\
ytmp = _mm_set_epi32 ( nonce[index+2], nonce[index+3], key[index+2], key[index+3] );\
x = _mm_unpacklo_epi64(xtmp, ytmp);\
z = _mm_unpackhi_epi64(xtmp, ytmp);\
xtmp = _mm_set_epi32 (nonce[index+4], nonce[index+5], key[index+4], key[index+5]);\
ytmp = _mm_set_epi32 (nonce[index+6], nonce[index+7], key[index+6], key[index+7]);\
y = _mm_unpacklo_epi64(xtmp, ytmp);\
w = _mm_unpackhi_epi64(xtmp, ytmp);\
}

void sliscp_light256( u32 *state );

void crypto_aead_encrypt( u32 *tag, u32 tlen, u32 *ciphertext, u32 *plaintext, u32 plen, u32 *ad, u32 adlen, u8 *nonce, u8 *key);
void crypto_aead_decrypt( u32 *plaintext, u32 *ciphertext, u32 plen, u32 *tag, u32 tlen, u32 *ad, u32 adlen, u8 *nonce, u8 *key);
#endif
