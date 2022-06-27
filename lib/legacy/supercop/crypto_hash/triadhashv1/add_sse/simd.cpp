
#include "crypto_hash.h"
#include "api.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>

#ifdef _MSC_VER
#include <Windows.h>
#include <intrin.h>
#include <immintrin.h>
#define MIE_ALIGN(x) __declspec(align(x))
#else
#include <x86intrin.h>
#define MIE_ALIGN(x) __attribute__((aligned(x)))
#endif


/*==== display function ====*/
static void disp(__m128i in)
{
    MIE_ALIGN(128)
    unsigned char tmp[16] = {0};
    _mm_store_si128((__m128i *)(tmp), in);

    //Output
    printf("O \t");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%2x ", tmp[4 * i + j]);
        }
        printf(" | ");
    }
    printf("\n");
}

static void dispReg(__m128i in)
{
    MIE_ALIGN(128)
    unsigned char tmp[16] = {0};
    _mm_store_si128((__m128i *)(tmp), in);

    //Output
    printf("O \t0x");
    for (int i = 3; i >= 0; i--)
    {
        for (int j = 3; j >= 0; j--)
        {
            printf("%02X", tmp[4 * i + j]);
        }
    }
    printf("\n");
}

/*==== 56x Triad Update for XMM Register ====*/
const __m128i BlendMaskList[7] = {
  _mm_set_epi8(0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  _mm_set_epi8(0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  _mm_set_epi8(0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
};

/*==== Template<i>x Triad Update for XMM Register ====*/
template <int i>
static inline void triad_update_encryption(const __m128i in[3], __m128i out[3])
{
  // REG B
  __m128i xmmB1 = _mm_slli_si128(in[1], 8 - i);
  __m128i xmmB3 = _mm_slli_si128(in[1], 10 - i);

  __m128i newb = _mm_slli_epi64(xmmB1, 2);
  __m128i ksb = _mm_slli_epi64(xmmB3, 8);
  __m128i ffb = xmmB1;
  ksb = _mm_xor_si128(ksb, ffb);

  __m128i nlb1 = _mm_slli_epi64(xmmB3, 7);
  __m128i nlb2 = _mm_slli_epi64(xmmB1, 1);
  __m128i nlb3 = _mm_slli_epi64(xmmB3, 5);
  __m128i nlb = _mm_and_si128(nlb1, nlb2);

  __m128i outB = _mm_xor_si128(ksb, nlb);

  // REG C
  __m128i xmmC1 = _mm_slli_si128(in[2], 8 - i);
  __m128i xmmC2 = _mm_slli_si128(in[2], 9 - i);
  __m128i xmmC3 = _mm_slli_si128(in[2], 10 - i);

  __m128i newc = _mm_slli_epi64(xmmC3, 4);
  newc = _mm_xor_si128(newc, outB);
  out[2] = _mm_srli_si128(in[2], i);
  out[2] = _mm_blendv_epi8(out[2], newc, BlendMaskList[i - 1]);

  __m128i ksc = _mm_slli_epi64(xmmC3, 8);
  __m128i ffc = _mm_slli_epi64(xmmC1, 4);
  ksc = _mm_xor_si128(ksc, ffc);

  __m128i nlc1 = _mm_slli_epi64(xmmC3, 7);
  __m128i nlc2 = _mm_slli_epi64(xmmC2, 5);
  __m128i nlc3 = _mm_slli_epi64(xmmC3, 5);
  __m128i nlc = _mm_and_si128(nlc1, nlc2);
  __m128i nld = _mm_and_si128(nlb3, nlc3);

  __m128i outC = _mm_xor_si128(ksc, nlc);

  // REG A
  __m128i xmmA1 = _mm_slli_si128(in[0], 8 - i);
  __m128i xmmA2 = _mm_slli_si128(in[0], 9 - i);

  __m128i newa = _mm_slli_epi64(xmmA2, 2);
  newa = _mm_xor_si128(newa, outC);
  out[0] = _mm_srli_si128(in[0], i);
  out[0] = _mm_blendv_epi8(out[0], newa, BlendMaskList[i - 1]);

  __m128i ksa = _mm_slli_epi64(xmmA2, 8);
  __m128i ffa = _mm_slli_epi64(xmmA1, 4);
  ksa = _mm_xor_si128(ksa, ffa);
  ksa = _mm_xor_si128(ksa, nld);

  __m128i nla1 = _mm_slli_epi64(xmmA2, 7);
  __m128i nla2 = _mm_slli_epi64(xmmA2, 1);
  __m128i nla = _mm_and_si128(nla1, nla2);

  __m128i outA = _mm_xor_si128(ksa, nla);

  //
  newb = _mm_xor_si128(newb, outA);
  out[1] = _mm_srli_si128(in[1], i);
  out[1] = _mm_blendv_epi8(out[1], newb, BlendMaskList[i - 1]);

  return;
}




/*==== HASH ====*/
void triad_high_speed_hash(const unsigned char *in, unsigned long long inlen, unsigned char *out)
{
    MIE_ALIGN(128)
    unsigned char c_hash[16] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x5aU, 0x88U, 0xa8U, 0xf6U, 0x43U, 0xe2U, 0x8aU, 0x62U, 0x51U, 0xe1U, 0xb7U };

    MIE_ALIGN(128)
    unsigned char c_padding[16] = { 0x80U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };

    const __m128i outMask = _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    int i;

    __m128i x[3];
    x[0] = _mm_xor_si128(x[0], x[0]);
    x[1] = _mm_load_si128((const __m128i *)(c_hash));
    x[2] = _mm_xor_si128(x[2], x[2]);
    

    // absorb in
    for (i = 0; i < (int)(inlen) - 3; i += 4)
    {
      __m128i xmm_a = _mm_loadu_si128((const __m128i *)(in + i));
      xmm_a = _mm_slli_si128(xmm_a, 12);
      x[0] = _mm_xor_si128(x[0], xmm_a);

      //dispReg(x[0]);
      //dispReg(x[1]);
      //dispReg(x[2]);

      for (int j = 0; j < 18; j++)
        triad_update_encryption<7>(x, x);
      triad_update_encryption<2>(x, x);
    }

    // absorb last block
    int last_block_len = (int)(inlen)-i;

    MIE_ALIGN(128)
    unsigned char last_block[16] = { 0x00 };
    memcpy(last_block, in + i, last_block_len);
    last_block[last_block_len] = 0x80;
    __m128i xmm_a = _mm_loadu_si128((const __m128i *)(last_block));
    xmm_a = _mm_slli_si128(xmm_a, 12);
    x[0] = _mm_xor_si128(x[0], xmm_a);

    for (int j = 0; j < 18; j++)
      triad_update_encryption<7>(x, x);
    triad_update_encryption<2>(x, x);

    __m128i hash1 = _mm_and_si128(x[0], outMask);
    hash1 = _mm_xor_si128(hash1, _mm_srli_si128(x[1], 10));
    _mm_storeu_si128((__m128i *)(out), hash1);

    for (int j = 0; j < 18; j++)
      triad_update_encryption<7>(x, x);
    triad_update_encryption<2>(x, x);

    __m128i hash2 = _mm_and_si128(x[0], outMask);
    hash2 = _mm_xor_si128(hash2, _mm_srli_si128(x[1], 10));
    _mm_storeu_si128((__m128i *)(out + 16), hash2);

}



int crypto_hash(
  unsigned char *out,
  const unsigned char *in,
  unsigned long long inlen
) {

  triad_high_speed_hash(in, inlen, out);

  return 0;
}

