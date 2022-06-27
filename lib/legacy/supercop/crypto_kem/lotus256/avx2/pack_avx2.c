/**
 * @file pack.c
 * @author Takuya HAYASHI (t-hayashi@eedept.kobe-u.ac.jp)
 * @brief Parameter specific pack/unpack functions, AVX2 optimzied implementation
 */

#include "type.h"
#include "param.h"
#include <immintrin.h>

#define MSB    0x8000
#define MSB_   0x7fff
#define MS3B   0xe000
#define MS3B_  0x1fff
#define LOWER6 0x003f
#define LOWER8 0x00ff


/**
 * @brief pack 64 elems to 104 bytes, SIMD friendly implementation.
 */
void pack_64elems(U8 *rop, const U16 *op){
  __m128i r[6], s[2], t, u, m = _mm_set1_epi16(MS3B);
  r[0] = _mm_loadu_si128((__m128i*)(op));
  r[1] = _mm_loadu_si128((__m128i*)(op + 8));
  r[2] = _mm_loadu_si128((__m128i*)(op + 16));
  r[3] = _mm_loadu_si128((__m128i*)(op + 24));
  r[4] = _mm_loadu_si128((__m128i*)(op + 32));
  r[5] = _mm_loadu_si128((__m128i*)(op + 40));
  s[0] = _mm_loadu_si128((__m128i*)(op + 48));
  s[1] = _mm_loadu_si128((__m128i*)(op + 56));
  t = _mm_slli_epi16(s[0], 3);
  t = _mm_and_si128(t, m);
  r[0] = _mm_or_si128(r[0], t);
  t = _mm_slli_epi16(s[0], 6);
  t = _mm_and_si128(t, m);
  r[1] = _mm_or_si128(r[1], t);
  t = _mm_slli_epi16(s[0], 9);
  t = _mm_and_si128(t, m);
  r[2] = _mm_or_si128(r[2], t);
  t = _mm_slli_epi16(s[0], 12);
  t = _mm_and_si128(t, m);
  r[3] = _mm_or_si128(r[3], t);
  t = _mm_slli_epi16(s[0], 15);
  u = _mm_slli_epi16(s[1], 2);
  t = _mm_or_si128(t, u);
  t = _mm_and_si128(t, m);
  r[4] = _mm_or_si128(r[4], t);
  t = _mm_slli_epi16(s[1], 5);
  t = _mm_and_si128(t, m);
  r[5] = _mm_or_si128(r[5], t);

  _mm_storeu_si128((__m128i*)(rop), r[0]);
  _mm_storeu_si128((__m128i*)(rop + 16), r[1]);
  _mm_storeu_si128((__m128i*)(rop + 32), r[2]);
  _mm_storeu_si128((__m128i*)(rop + 48), r[3]);
  _mm_storeu_si128((__m128i*)(rop + 64), r[4]);
  _mm_storeu_si128((__m128i*)(rop + 80), r[5]);
  
  U16 *ptr = (U16*)rop;
  U32 i;
  for(i = 0; i < 4; ++i) ptr[48 + i] = (op[56 + i] & LOWER8) | ((op[60 + i] & LOWER8) << 8);
}

/**
 * @brief unpack 64 elems from 104 bytes, SIMD friendly implementation.
 */
void unpack_64elems(U16 *rop, const U8 *op){
  __m128i r[8], s[6], m = _mm_set1_epi16(MS3B_);
  
  s[0] = _mm_loadu_si128((__m128i*)(op));
  s[1] = _mm_loadu_si128((__m128i*)(op + 16));
  s[2] = _mm_loadu_si128((__m128i*)(op + 32));
  s[3] = _mm_loadu_si128((__m128i*)(op + 48));
  s[4] = _mm_loadu_si128((__m128i*)(op + 64));
  s[5] = _mm_loadu_si128((__m128i*)(op + 80));

  r[0] = _mm_and_si128(s[0], m);
  r[1] = _mm_and_si128(s[1], m);
  r[2] = _mm_and_si128(s[2], m);
  r[3] = _mm_and_si128(s[3], m);
  r[4] = _mm_and_si128(s[4], m);
  r[5] = _mm_and_si128(s[5], m);
  m = _mm_set1_epi16(MS3B);
  s[0] = _mm_and_si128(s[0], m);
  s[1] = _mm_and_si128(s[1], m);
  s[2] = _mm_and_si128(s[2], m);
  s[3] = _mm_and_si128(s[3], m);
  s[4] = _mm_and_si128(s[4], m);
  s[0] = _mm_srli_epi16(s[0], 3);
  s[1] = _mm_srli_epi16(s[1], 6);
  s[2] = _mm_srli_epi16(s[2], 9);
  s[3] = _mm_srli_epi16(s[3], 12);
  r[6] = _mm_or_si128(s[0], s[1]);
  r[6] = _mm_or_si128(r[6], s[2]);
  s[0] = _mm_srli_epi16(s[4], 15);
  r[6] = _mm_or_si128(r[6], s[3]);
  r[6] = _mm_or_si128(r[6], s[0]);

  _mm_storeu_si128((__m128i*)(rop), r[0]);
  _mm_storeu_si128((__m128i*)(rop + 8), r[1]);
  _mm_storeu_si128((__m128i*)(rop + 16), r[2]);
  _mm_storeu_si128((__m128i*)(rop + 24), r[3]);
  _mm_storeu_si128((__m128i*)(rop + 32), r[4]);
  _mm_storeu_si128((__m128i*)(rop + 40), r[5]);
  _mm_storeu_si128((__m128i*)(rop + 48), r[6]);

  const U16 *ptr = (const U16*)op;
  U32 i;
  for(i = 0; i < 4; ++i) rop[56 + i] = ((ptr[32 + i] & (MSB ^ MS3B)) >> 2)
                           | ((ptr[40 + i] & MS3B) >> 5)
                           | (ptr[48 + i] & LOWER8);
  for(i = 0; i < 4; ++i) rop[60 + i] = ((ptr[36 + i] & (MSB ^ MS3B)) >> 2)
                           | ((ptr[44 + i] & MS3B) >> 5)
                           | ((ptr[48 + i] >> 8) & LOWER8);
}

/**
 * @brief pack 128 elems to 208 bytes, SIMD friendly implementation.
 */
void pack_128elems(U8 *rop, const U16 *op){
  __m256i r[6], s[2], t, u, m = _mm256_set1_epi16(MS3B);
  __m128i r6, s7h, m_ = _mm_set1_epi16(LOWER8);
  r[0] = _mm256_loadu_si256((__m256i*)(op));
  r[1] = _mm256_loadu_si256((__m256i*)(op + 16));
  r[2] = _mm256_loadu_si256((__m256i*)(op + 32));
  r[3] = _mm256_loadu_si256((__m256i*)(op + 48));
  r[4] = _mm256_loadu_si256((__m256i*)(op + 64));
  r[5] = _mm256_loadu_si256((__m256i*)(op + 80));
  s[0] = _mm256_loadu_si256((__m256i*)(op + 96));
  s[1] = _mm256_loadu_si256((__m256i*)(op + 112));
  t = _mm256_slli_epi16(s[0], 3);
  t = _mm256_and_si256(t, m);
  r[0] = _mm256_or_si256(r[0], t);
  t = _mm256_slli_epi16(s[0], 6);
  t = _mm256_and_si256(t, m);
  r[1] = _mm256_or_si256(r[1], t);
  t = _mm256_slli_epi16(s[0], 9);
  t = _mm256_and_si256(t, m);
  r[2] = _mm256_or_si256(r[2], t);
  t = _mm256_slli_epi16(s[0], 12);
  t = _mm256_and_si256(t, m);
  r[3] = _mm256_or_si256(r[3], t);
  t = _mm256_slli_epi16(s[0], 15);
  u = _mm256_slli_epi16(s[1], 2);
  t = _mm256_or_si256(t, u);
  t = _mm256_and_si256(t, m);
  r[4] = _mm256_or_si256(r[4], t);
  t = _mm256_slli_epi16(s[1], 5);
  t = _mm256_and_si256(t, m);
  r[5] = _mm256_or_si256(r[5], t);

  r6 = _mm256_extractf128_si256(s[1], 0);
  r6 = _mm_and_si128(r6, m_);
  s7h = _mm256_extractf128_si256(s[1], 1);
  s7h = _mm_and_si128(s7h, m_);
  s7h = _mm_slli_epi16(s7h, 8);
  r6 = _mm_or_si128(r6, s7h);

  _mm256_storeu_si256((__m256i*)(rop), r[0]);
  _mm256_storeu_si256((__m256i*)(rop + 32), r[1]);
  _mm256_storeu_si256((__m256i*)(rop + 64), r[2]);
  _mm256_storeu_si256((__m256i*)(rop + 96), r[3]);
  _mm256_storeu_si256((__m256i*)(rop + 128), r[4]);
  _mm256_storeu_si256((__m256i*)(rop + 160), r[5]);
  _mm_storeu_si128((__m128i*)(rop + 192), r6);
}


/**
 * @brief unpack 128 elems from 208 bytes, SIMD friendly implementation.
 */
void unpack_128elems(U16 *rop, const U8 *op){
  __m256i r[8], s[6], m = _mm256_set1_epi16(MS3B_);
  __m128i s6, r7[2], m_ = _mm_set1_epi16(LOWER8);
  
  s[0] = _mm256_loadu_si256((__m256i*)(op));
  s[1] = _mm256_loadu_si256((__m256i*)(op + 32));
  s[2] = _mm256_loadu_si256((__m256i*)(op + 64));
  s[3] = _mm256_loadu_si256((__m256i*)(op + 96));
  s[4] = _mm256_loadu_si256((__m256i*)(op + 128));
  s[5] = _mm256_loadu_si256((__m256i*)(op + 160));
  s6 = _mm_loadu_si128((__m128i*)(op + 192));

  r[0] = _mm256_and_si256(s[0], m);
  r[1] = _mm256_and_si256(s[1], m);
  r[2] = _mm256_and_si256(s[2], m);
  r[3] = _mm256_and_si256(s[3], m);
  r[4] = _mm256_and_si256(s[4], m);
  r[5] = _mm256_and_si256(s[5], m);
  m = _mm256_set1_epi16(MS3B);
  s[0] = _mm256_and_si256(s[0], m);
  s[1] = _mm256_and_si256(s[1], m);
  s[2] = _mm256_and_si256(s[2], m);
  s[3] = _mm256_and_si256(s[3], m);
  s[4] = _mm256_and_si256(s[4], m);
  s[0] = _mm256_srli_epi16(s[0], 3);
  s[1] = _mm256_srli_epi16(s[1], 6);
  s[2] = _mm256_srli_epi16(s[2], 9);
  s[3] = _mm256_srli_epi16(s[3], 12);
  r[6] = _mm256_or_si256(s[0], s[1]);
  r[6] = _mm256_or_si256(r[6], s[2]);
  s[0] = _mm256_srli_epi16(s[4], 15);
  r[6] = _mm256_or_si256(r[6], s[3]);
  r[6] = _mm256_or_si256(r[6], s[0]);
  r[7] = _mm256_and_si256(s[5], m);
  r[7] = _mm256_srli_epi16(r[7], 5);
  m = _mm256_set1_epi16(MSB_);
  s[4] = _mm256_and_si256(s[4], m);
  s[4] = _mm256_srli_epi16(s[4], 2);
  r[7] = _mm256_or_si256(r[7], s[4]);
  r7[0] = _mm256_extractf128_si256(r[7], 0);
  r7[1] = _mm256_extractf128_si256(r[7], 1);
  r7[0] = _mm_or_si128(r7[0], _mm_and_si128(s6, m_));
  r7[1] = _mm_or_si128(r7[1], _mm_and_si128(_mm_srli_epi16(s6, 8), m_));
  r[7] = _mm256_insertf128_si256(_mm256_castsi128_si256(r7[0]), r7[1], 1);
  _mm256_storeu_si256((__m256i*)(rop), r[0]);
  _mm256_storeu_si256((__m256i*)(rop + 16), r[1]);
  _mm256_storeu_si256((__m256i*)(rop + 32), r[2]);
  _mm256_storeu_si256((__m256i*)(rop + 48), r[3]);
  _mm256_storeu_si256((__m256i*)(rop + 64), r[4]);
  _mm256_storeu_si256((__m256i*)(rop + 80), r[5]);
  _mm256_storeu_si256((__m256i*)(rop + 96), r[6]);
  _mm256_storeu_si256((__m256i*)(rop + 112), r[7]);
}


/**
 * @brief Pack 128 discrete Gaussian samples to 96 bytes, SIMD friendly.
 */
void pack_128dg(U8 *rop, const U16 *op){
  __m256i r[3], s[8], m = _mm256_set1_epi16(LOWER6), t;
  s[0] = _mm256_loadu_si256((__m256i*)(op));
  s[1] = _mm256_loadu_si256((__m256i*)(op + 16));
  s[2] = _mm256_loadu_si256((__m256i*)(op + 32));
  s[3] = _mm256_loadu_si256((__m256i*)(op + 48));
  s[4] = _mm256_loadu_si256((__m256i*)(op + 64));
  s[5] = _mm256_loadu_si256((__m256i*)(op + 80));
  s[6] = _mm256_loadu_si256((__m256i*)(op + 96));
  s[7] = _mm256_loadu_si256((__m256i*)(op + 112));
  s[0] = _mm256_and_si256(s[0], m);
  s[1] = _mm256_and_si256(s[1], m);
  s[2] = _mm256_and_si256(s[2], m);
  s[3] = _mm256_and_si256(s[3], m);
  s[4] = _mm256_and_si256(s[4], m);
  s[5] = _mm256_and_si256(s[5], m);
  s[6] = _mm256_and_si256(s[6], m);
  s[7] = _mm256_and_si256(s[7], m);
  s[1] = _mm256_slli_epi16(s[1], 6);
  r[0] = _mm256_or_si256(s[0], s[1]);
  t = _mm256_slli_epi16(s[2], 12);
  r[0] = _mm256_or_si256(r[0], t);
  s[2] = _mm256_srli_epi16(s[2], 4);
  s[3] = _mm256_slli_epi16(s[3], 2);
  s[4] = _mm256_slli_epi16(s[4], 8);
  t = _mm256_slli_epi16(s[5], 14);
  r[1] = _mm256_or_si256(s[2], s[3]);
  r[1] = _mm256_or_si256(r[1], s[4]);
  r[1] = _mm256_or_si256(r[1], t);
  s[5] = _mm256_srli_epi16(s[5], 2);
  s[6] = _mm256_slli_epi16(s[6], 4);
  s[7] = _mm256_slli_epi16(s[7], 10);
  r[2] = _mm256_or_si256(s[5], s[6]);
  r[2] = _mm256_or_si256(r[2], s[7]);
  _mm256_storeu_si256((__m256i*)(rop), r[0]);
  _mm256_storeu_si256((__m256i*)(rop + 32), r[1]);
  _mm256_storeu_si256((__m256i*)(rop + 64), r[2]);
}

/**
 * @brief Unpack 128 discrete Gaussian samples from 96 bytes, SIMD friendly.
 */
void unpack_128dg(U16 *rop, const U8 *op){
  __m256i r[8], s[3], m = _mm256_set1_epi16(LOWER6), z = _mm256_setzero_si256(), t;
  s[0] = _mm256_loadu_si256((__m256i*)(op));
  s[1] = _mm256_loadu_si256((__m256i*)(op + 32));
  s[2] = _mm256_loadu_si256((__m256i*)(op + 64));

  r[0] = _mm256_and_si256(s[0], m);
  t = _mm256_srli_epi16(r[0], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[0] = _mm256_or_si256(r[0], t);

  r[1] = _mm256_and_si256(_mm256_srli_epi16(s[0], 6), m);
  t = _mm256_srli_epi16(r[1], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[1] = _mm256_or_si256(r[1], t);

  r[2] = _mm256_and_si256(_mm256_or_si256(_mm256_srli_epi16(s[0], 12), _mm256_slli_epi16(s[1], 4)), m);
  t = _mm256_srli_epi16(r[2], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[2] = _mm256_or_si256(r[2], t);

  r[3] = _mm256_and_si256(_mm256_srli_epi16(s[1], 2), m);
  t = _mm256_srli_epi16(r[3], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[3] = _mm256_or_si256(r[3], t);

  r[4] = _mm256_and_si256(_mm256_srli_epi16(s[1], 8), m);
  t = _mm256_srli_epi16(r[4], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[4] = _mm256_or_si256(r[4], t);

  r[5] = _mm256_and_si256(_mm256_or_si256(_mm256_srli_epi16(s[1], 14), _mm256_slli_epi16(s[2], 2)), m);
  t = _mm256_srli_epi16(r[5], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[5] = _mm256_or_si256(r[5], t);

  r[6] = _mm256_and_si256(_mm256_srli_epi16(s[2], 4), m);
  t = _mm256_srli_epi16(r[6], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[6] = _mm256_or_si256(r[6], t);

  r[7] = _mm256_srli_epi16(s[2], 10);
  t = _mm256_srli_epi16(r[7], 5);
  t = _mm256_sub_epi16(z, t);
  t = _mm256_slli_epi16(t, 6);
  r[7] = _mm256_or_si256(r[7], t);

  m = _mm256_set1_epi16(MS3B_);
  _mm256_storeu_si256((__m256i*)(rop), _mm256_and_si256(r[0], m));
  _mm256_storeu_si256((__m256i*)(rop + 16), _mm256_and_si256(r[1], m));
  _mm256_storeu_si256((__m256i*)(rop + 32), _mm256_and_si256(r[2], m));
  _mm256_storeu_si256((__m256i*)(rop + 48), _mm256_and_si256(r[3], m));
  _mm256_storeu_si256((__m256i*)(rop + 64), _mm256_and_si256(r[4], m));
  _mm256_storeu_si256((__m256i*)(rop + 80), _mm256_and_si256(r[5], m));
  _mm256_storeu_si256((__m256i*)(rop + 96), _mm256_and_si256(r[6], m));
  _mm256_storeu_si256((__m256i*)(rop + 112), _mm256_and_si256(r[7], m));
}

/**
 * @brief Pack public key
 */
void pack_pk(U8 *bin, const U16 *pk){
  const U32 len = (_LOTUS_LWE_DIM * _LOTUS_LWE_DIM + _LOTUS_LWE_DIM * _LOTUS_LWE_PT) / 128;
  U32 i;
  for(i = 0; i < len; ++i){
    pack_128elems(bin, pk);
    bin += 208, pk += 128;
  }
  /* len % 128 = 0 */
}

/**
 * @brief Unpack public key
 */
void unpack_pk(U16 *pk, const U8 *bin){
  const U32 len = (_LOTUS_LWE_DIM * _LOTUS_LWE_DIM + _LOTUS_LWE_DIM * _LOTUS_LWE_PT) / 128;
  U32 i;
  for(i = 0; i < len; ++i){
    unpack_128elems(pk, bin);
    pk += 128, bin += 208;
  }
  /* len % 128 = 0 */
}

/**
 * @brief Pack secret key
 */
void pack_sk(U8 *bin, const U16 *sk){
  const U32 len = (_LOTUS_LWE_DIM * _LOTUS_LWE_PT) / 128;
  U32 i;
  for(i = 0; i < len; ++i){
    pack_128dg(bin, sk);
    bin += 96, sk += 128;
  }
}

/**
 * @brief Unpack secret key
 */
void unpack_sk(U16 *sk, const U8 *bin){
  const U32 len = (_LOTUS_LWE_DIM * _LOTUS_LWE_PT) / 128;
  U32 i;
  for(i = 0; i < len; ++i){
    unpack_128dg(sk, bin);
    sk += 128, bin += 96;
  }
}

/**
 * @brief Pack ciphertext
 */
void pack_ct(U8 *bin, const U16 *ct){
  const U32 len = (_LOTUS_LWE_DIM + _LOTUS_LWE_PT) / 128;
  U32 i;
  for(i = 0; i < len; ++i){
    pack_128elems(bin, ct);
    bin += 208, ct += 128;
  }
  pack_64elems(bin, ct); /* len % 128 = 64 */
}

/**
 * @brief Unpack ciphertext
 */
void unpack_ct(U16 *ct, const U8 *bin){
  const U32 len = (_LOTUS_LWE_DIM + _LOTUS_LWE_PT) / 128;
  U32 i;
  for(i = 0; i < len; ++i){
    unpack_128elems(ct, bin);
    ct += 128, bin += 208;
  }
  unpack_64elems(ct, bin); /* len % 128 = 64 */
}
