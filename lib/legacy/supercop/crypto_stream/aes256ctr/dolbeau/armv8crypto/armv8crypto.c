/*
  armv8crypto.c
  AES-CTR
  Romain Dolbeau
  Public Domain
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arm_neon.h>
#include "crypto_stream.h"
#include "stdaes-common.h"

#define ALIGN16  __attribute__((aligned(16)))
#define ALIGN32  __attribute__((aligned(32)))
#define ALIGN64  __attribute__((aligned(64)))
#define _bswap64(a) __builtin_bswap64(a)
#define _bswap(a) __builtin_bswap(a)

/* this one uses 'aese' for the sbox (only), and is slower than the C code */
static inline void aes256_armv8_setkey_encrypt(const unsigned int key[], unsigned int *aes_edrk) {
  unsigned int i = 0;
  unsigned int rotl_aes_edrk;
  unsigned int tmp8, tmp9, tmp10, tmp11;
  unsigned int tmp12, tmp13, tmp14, tmp15;
  unsigned int temp_lds;
  unsigned int round = 0x00000001;
  uint8x16_t vzero = vreinterpretq_u8_u32(vdupq_n_u32(0));
#define armv8_aese_sbox(input) \
        vgetq_lane_u32(vreinterpretq_u32_u8(vaeseq_u8(vreinterpretq_u8_u32(vdupq_n_u32(input)), vzero)),0)

  tmp8  = (key[0]);
  aes_edrk[0] = tmp8;
  tmp9  = (key[1]);
  aes_edrk[1] = tmp9;
  tmp10 = (key[2]);
  aes_edrk[2] = tmp10;
  tmp11 = (key[3]);
  aes_edrk[3] = tmp11;
  tmp12 = (key[4]);
  aes_edrk[4] = tmp12;
  tmp13 = (key[5]);
  aes_edrk[5] = tmp13;
  tmp14 = (key[6]);
  aes_edrk[6] = tmp14;
  tmp15 = (key[7]);
  aes_edrk[7] = tmp15;

  for( i = 8; i < 56; /* i+=8 */ )
  {
    rotl_aes_edrk   = rotr(tmp15,8);

    temp_lds = armv8_aese_sbox(rotl_aes_edrk);

    tmp8 = tmp8 ^ round ^ temp_lds;
    round = round << 1;

    aes_edrk[i++]   = tmp8;
    tmp9  = tmp9  ^ tmp8;
    aes_edrk[i++]   = tmp9;
    tmp10 = tmp10 ^ tmp9;
    aes_edrk[i++]  = tmp10;
    tmp11 = tmp11 ^ tmp10;
    aes_edrk[i++]  = tmp11;

    temp_lds = armv8_aese_sbox(tmp11);

    tmp12 = tmp12 ^ temp_lds;
    aes_edrk[i++]  = tmp12;
    tmp13 = tmp13 ^ tmp12;
    aes_edrk[i++]  = tmp13;
    tmp14 = tmp14 ^ tmp13;
    aes_edrk[i++]  = tmp14;
    tmp15 = tmp15 ^ tmp14;
    aes_edrk[i++]  = tmp15;
  }

  rotl_aes_edrk   = rotr(tmp15,8);
  temp_lds = armv8_aese_sbox(rotl_aes_edrk);

  tmp8 = tmp8 ^ round ^ temp_lds;
  round = round << 1;

  aes_edrk[i++]   = tmp8;
  tmp9  = tmp9  ^ tmp8;
  aes_edrk[i++]   = tmp9;
  tmp10 = tmp10 ^ tmp9;
  aes_edrk[i++]  = tmp10;
  tmp11 = tmp11 ^ tmp10;
  aes_edrk[i++]  = tmp11;

#undef armv8_aese_sbox
}


/* useful */
#define vsetq_pair_u64(x,y) vsetq_lane_u64(x,vdupq_n_u64(y),1)

/** multiple-blocks-at-once AES encryption with crypto extension */
/* Step 1 : loading the nonce */
/* load & increment the n vector (non-vectorized, unused for now) */
#define NVx(a)                                                  \
  uint32x4_t nv##a = vreinterpretq_u32_u8(vrev64q_u8(vld1q_u8((const uint8_t*)n)); incle(n)
/* load the incremented n vector (vectorized, probably buggy) */
#define NVxV_DEC(a)                                                     \
  uint32x4_t nv##a;
#define NVxV_NOWRAP(a)                                                  \
  nv##a = vreinterpretq_u32_u8(vrev64q_u8(vreinterpretq_u8_u64(vaddq_u64(vreinterpretq_u64_u32(nv0i), vsetq_pair_u64(a,0)))))
#define NVxV_WRAP(a)                                                    \
  uint32x4_t ad##a = vreinterpretq_u32_u64(vaddq_u64(vreinterpretq_u64_u32(nv0i), vsetq_pair_u64(a,a>=wrapnumber?1:0))); \
  nv##a = vreinterpretq_u32_u8(vrev64q_u8(vreinterpretq_u8_u32(ad##a)))

/* Step 2 : define value before round one */
#define TEMPx(a)                                        \
  uint32x4_t temp##a = nv##a

/* Step 3: one round of AES */
//#define AES_USE_ASM
#ifdef AES_USE_ASM
#define AESENCx(a) 					\
  asm("aese %0.16b, %1.16b\naesmc %0.16b, %0.16b\n" : "+w"(temp##a) : "w"(rkeys[i]));
#else
#define AESENCx(a)                                      \
  temp##a =  vreinterpretq_u32_u8(vaeseq_u8(vreinterpretq_u8_u32(temp##a), vreinterpretq_u8_u32(rkeys[i])));temp##a = vreinterpretq_u32_u8(vaesmcq_u8(vreinterpretq_u8_u32(temp##a)))
#endif

/* Step 4: last rounds of AES */
#define AESENCLASTx(a)                                  \
  temp##a =  vreinterpretq_u32_u8(vaeseq_u8(vreinterpretq_u8_u32(temp##a), vreinterpretq_u8_u32(rkeys[13])));temp##a = veorq_u32(temp##a, rkeys[14])

/* Step 5: store result */
#define STOREx(a)                                       \
  vst1q_u32((uint32_t*)(out+(a*16)), temp##a)

/* all the MAKE* macros are for automatic explicit unrolling */
#define MAKE4(X)                                \
  X(0);X(1);X(2);X(3)

#define MAKE6(X)                                \
  X(0);X(1);X(2);X(3);                          \
  X(4);X(5)

#define MAKE7(X)                                \
  X(0);X(1);X(2);X(3);                          \
  X(4);X(5);X(6)

#define MAKE8(X)                                \
  X(0);X(1);X(2);X(3);                          \
  X(4);X(5);X(6);X(7)

#define MAKE10(X)                               \
  X(0);X(1);X(2);X(3);                          \
  X(4);X(5);X(6);X(7);                          \
  X(8);X(9)

#define MAKE12(X)                               \
  X(0);X(1);X(2);X(3);                          \
  X(4);X(5);X(6);X(7);                          \
  X(8);X(9);X(10);X(11)

/* create a function of unrolling N ; the MAKEN is the unrolling
   macro, defined above. The N in MAKEN must match N, obviously. */
#define FUNC(N, MAKEN)                          \
  static inline void aesni_encrypt##N(unsigned char *out, unsigned char *n, uint32x4_t rkeys[16]) { \
    uint32x4_t nv0i = vld1q_u32((const uint32_t*)n);                  \
    long long nl = *(long long*)&n[8];                                  \
    MAKEN(NVxV_DEC);                                                    \
    /* check for nonce wraparound */                                    \
    if ((nl < 0) && (nl + N) >= 0) {                                \
      int wrapnumber = (int)(N - (nl+N));                               \
      MAKEN(NVxV_WRAP);                                                 \
      vst1q_u64((uint64_t*)n, vaddq_u64(vreinterpretq_u64_u32(nv0i), vsetq_pair_u64(N,1))); \
    } else {                                                            \
      MAKEN(NVxV_NOWRAP);                                               \
      vst1q_u64((uint64_t*)n, vaddq_u64(vreinterpretq_u64_u32(nv0i), vsetq_pair_u64(N,0))); \
    }                                                                   \
    int i;                                                              \
    MAKEN(TEMPx);                                                       \
    for (i = 0 ; i < 13 ; i++) {                                        \
      MAKEN(AESENCx);                                                   \
    }                                                                   \
    MAKEN(AESENCLASTx);                                                 \
    MAKEN(STOREx);                                                      \
  }

/* and now building our unrolled function is trivial */
FUNC(4, MAKE4)
FUNC(6, MAKE6)
FUNC(7, MAKE7)
FUNC(8, MAKE8)
FUNC(10, MAKE10)
FUNC(12, MAKE12)

int crypto_stream(
unsigned char *out,
unsigned long long outlen,
const unsigned char *n,
const unsigned char *k
)
{
  uint32x4_t rkeys[16];
  ALIGN16 unsigned char n2[16];
  unsigned long long i, j;
  aes256_setkey_encrypt((unsigned int*)k, (unsigned int*)rkeys);
  //aes256_armv8_setkey_encrypt((unsigned int*)k, (unsigned int*)rkeys);
  /* n2 is in byte-reversed (i.e., native little endian)
     order to make increment/testing easier */
  (*(unsigned long long*)&n2[8]) = _bswap64((*(unsigned long long*)&n[8]));
  (*(unsigned long long*)&n2[0]) = _bswap64((*(unsigned long long*)&n[0]));
  
#define LOOP(iter)                                       \
  int lb = iter * 16;                                    \
  for (i = 0 ; i < outlen ; i+= lb) {                    \
    ALIGN16 unsigned char outni[lb];                     \
    aesni_encrypt##iter(outni, n2, rkeys);               \
    unsigned long long mj = lb;                          \
    if ((i+mj)>=outlen)                                  \
      mj = outlen-i;                                     \
    for (j = 0 ; j < mj ; j++)                           \
      out[i+j] = outni[j];                               \
  }
  
  LOOP(8);

  return 0;
}

int crypto_stream_xor(
unsigned char *out,
const unsigned char *in,
unsigned long long inlen,
const unsigned char *n,
const unsigned char *k
)
{
  uint32x4_t rkeys[16];
  ALIGN16 unsigned char n2[16];
  unsigned long long i, j;
  aes256_setkey_encrypt((unsigned int*)k, (unsigned int*)rkeys);
  //aes256_armv8_setkey_encrypt((unsigned int*)k, (unsigned int*)rkeys);
  /* n2 is in byte-reversed (i.e., native little endian)
     order to make increment/testing easier */
  (*(unsigned long long*)&n2[8]) = _bswap64((*(unsigned long long*)&n[8]));
  (*(unsigned long long*)&n2[0]) = _bswap64((*(unsigned long long*)&n[0]));
  
#define LOOPXOR(iter)                                    \
  int lb = iter * 16;                                    \
  for (i = 0 ; i < inlen ; i+= lb) {                     \
    ALIGN16 unsigned char outni[lb];                     \
    aesni_encrypt##iter(outni, n2, rkeys);               \
    unsigned long long mj = lb;                          \
    if ((i+mj)>=inlen)                                   \
      mj = inlen-i;                                      \
    for (j = 0 ; j < mj ; j++)                           \
      out[i+j] = in[i+j] ^ outni[j];                     \
  }
  
  LOOPXOR(8);

  return 0;
}
