
#include "crypto_aead.h"
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
static void disp(__m128i in) {
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
/*
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
*/
static void dispReg(__m256i in) {
    MIE_ALIGN(256)
    unsigned char tmp[32] = {0};
    _mm256_store_si256((__m256i *)(tmp), in);

    //Output
    printf("O \t0x");
    for (int i = 7; i >= 0; i--)
    {
        for (int j = 3; j >= 0; j--)
        {
            printf("%02X", tmp[4 * i + j]);
        }
        if (i == 4)
            printf(" ");
    }
    printf("\n");
}

/*==== 56x Triad Update for XMM Register ====*/
const __m128i BlendMask = _mm_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
void triad_update_56x_encryption(const __m128i in[3], __m128i out[3], __m128i &ks) {
    // REG B
    __m128i xmmB1 = _mm_slli_si128(in[1], 1);
    __m128i xmmB3 = _mm_slli_si128(in[1], 3);

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
    __m128i xmmC1 = _mm_slli_si128(in[2], 1);
    __m128i xmmC2 = _mm_slli_si128(in[2], 2);
    __m128i xmmC3 = _mm_slli_si128(in[2], 3);

    __m128i newc = _mm_slli_epi64(xmmC3, 4);
    newc = _mm_xor_si128(newc, outB);
    out[2] = _mm_srli_si128(in[2], 7);
    out[2] = _mm_blendv_epi8(out[2], newc, BlendMask);

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
    __m128i xmmA1 = _mm_slli_si128(in[0], 1);
    __m128i xmmA2 = _mm_slli_si128(in[0], 2);

    __m128i newa = _mm_slli_epi64(xmmA2, 2);
    newa = _mm_xor_si128(newa, outC);
    out[0] = _mm_srli_si128(in[0], 7);
    out[0] = _mm_blendv_epi8(out[0], newa, BlendMask);

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
    out[1] = _mm_srli_si128(in[1], 7);
    out[1] = _mm_blendv_epi8(out[1], newb, BlendMask);

    ks = _mm_xor_si128(ksa, ksb);
    ks = _mm_xor_si128(ks, ksc);
    ks = _mm_srli_si128(ks, 9);

    return;
}

const __m256i BlendMask256List[8] = {
    _mm256_set_epi8(0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
    _mm256_set_epi8(0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
    _mm256_set_epi8(0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
    _mm256_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
    _mm256_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
    _mm256_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
    _mm256_set_epi8(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)};

/*==== Template<i>x Triad Update for YMM Register ====*/
template <int i>
static inline void triad_update_encryption(const __m256i in[3], __m256i out[3]) {
    // REG B
    __m256i xmmB1 = _mm256_slli_si256(in[1], 8 - i);
    __m256i xmmB3 = _mm256_slli_si256(in[1], 10 - i);

    __m256i newb = _mm256_slli_epi64(xmmB1, 2);
    __m256i ksb = _mm256_slli_epi64(xmmB3, 8);
    __m256i ffb = xmmB1;
    ksb = _mm256_xor_si256(ksb, ffb);

    __m256i nlb1 = _mm256_slli_epi64(xmmB3, 7);
    __m256i nlb2 = _mm256_slli_epi64(xmmB1, 1);
    __m256i nlb3 = _mm256_slli_epi64(xmmB3, 5);
    __m256i nlb = _mm256_and_si256(nlb1, nlb2);

    __m256i outB = _mm256_xor_si256(ksb, nlb);

    // REG C
    __m256i xmmC1 = _mm256_slli_si256(in[2], 8 - i);
    __m256i xmmC2 = _mm256_slli_si256(in[2], 9 - i);
    __m256i xmmC3 = _mm256_slli_si256(in[2], 10 - i);

    __m256i newc = _mm256_slli_epi64(xmmC3, 4);
    newc = _mm256_xor_si256(newc, outB);
    out[2] = _mm256_srli_si256(in[2], i);
    out[2] = _mm256_blendv_epi8(out[2], newc, BlendMask256List[i - 1]);

    __m256i ksc = _mm256_slli_epi64(xmmC3, 8);
    __m256i ffc = _mm256_slli_epi64(xmmC1, 4);
    ksc = _mm256_xor_si256(ksc, ffc);

    __m256i nlc1 = _mm256_slli_epi64(xmmC3, 7);
    __m256i nlc2 = _mm256_slli_epi64(xmmC2, 5);
    __m256i nlc3 = _mm256_slli_epi64(xmmC3, 5);
    __m256i nlc = _mm256_and_si256(nlc1, nlc2);
    __m256i nld = _mm256_and_si256(nlb3, nlc3);

    __m256i outC = _mm256_xor_si256(ksc, nlc);

    // REG A
    __m256i xmmA1 = _mm256_slli_si256(in[0], 8 - i);
    __m256i xmmA2 = _mm256_slli_si256(in[0], 9 - i);

    __m256i newa = _mm256_slli_epi64(xmmA2, 2);
    newa = _mm256_xor_si256(newa, outC);
    out[0] = _mm256_srli_si256(in[0], i);
    out[0] = _mm256_blendv_epi8(out[0], newa, BlendMask256List[i - 1]);

    __m256i ksa = _mm256_slli_epi64(xmmA2, 8);
    __m256i ffa = _mm256_slli_epi64(xmmA1, 4);
    ksa = _mm256_xor_si256(ksa, ffa);
    ksa = _mm256_xor_si256(ksa, nld);

    __m256i nla1 = _mm256_slli_epi64(xmmA2, 7);
    __m256i nla2 = _mm256_slli_epi64(xmmA2, 1);
    __m256i nla = _mm256_and_si256(nla1, nla2);

    __m256i outA = _mm256_xor_si256(ksa, nla);

    //
    newb = _mm256_xor_si256(newb, outA);
    out[1] = _mm256_srli_si256(in[1], i);
    out[1] = _mm256_blendv_epi8(out[1], newb, BlendMask256List[i - 1]);

    return;
}

/*==== Template<i>x Triad Update and Key Stream Generation for YMM Register ====*/
template <int i>
static inline void triad_update_encryption(const __m256i in[3], __m256i out[3], __m128i &ks) {
    // REG B
    __m256i xmmB3 = _mm256_slli_si256(in[1], 10 - i);

    __m256i xmmB1 = _mm256_slli_si256(in[1], 8 - i);
    __m256i ksb = _mm256_slli_epi64(xmmB3, 8);
    __m256i nlb1 = _mm256_slli_epi64(xmmB3, 7);

    __m256i nlb2 = _mm256_slli_epi64(xmmB1, 1);
    __m256i newb = _mm256_slli_epi64(xmmB1, 2);
    __m256i nlb3 = _mm256_slli_epi64(xmmB3, 5);

    __m256i ffb = xmmB1;
    ksb = _mm256_xor_si256(ksb, ffb);

    __m256i nlb = _mm256_and_si256(nlb1, nlb2);
    __m256i outB = _mm256_xor_si256(ksb, nlb);


    // REG C
    __m256i xmmC1 = _mm256_slli_si256(in[2], 8 - i);
    __m256i xmmC2 = _mm256_slli_si256(in[2], 9 - i);
    __m256i xmmC3 = _mm256_slli_si256(in[2], 10 - i);

    __m256i newc = _mm256_slli_epi64(xmmC3, 4);
    newc = _mm256_xor_si256(newc, outB);
    out[2] = _mm256_srli_si256(in[2], i);

    __m256i ksc = _mm256_slli_epi64(xmmC3, 8);
    __m256i ffc = _mm256_slli_epi64(xmmC1, 4);
    ksc = _mm256_xor_si256(ksc, ffc);

    __m256i nlc1 = _mm256_slli_epi64(xmmC3, 7);
    __m256i nlc2 = _mm256_slli_epi64(xmmC2, 5);
    __m256i nlc3 = _mm256_slli_epi64(xmmC3, 5);
    __m256i nlc = _mm256_and_si256(nlc1, nlc2);
    __m256i nld = _mm256_and_si256(nlb3, nlc3);

    __m256i outC = _mm256_xor_si256(ksc, nlc);
    out[2] = _mm256_blendv_epi8(out[2], newc, BlendMask256List[i - 1]);



    // REG A
    __m256i xmmA1 = _mm256_slli_si256(in[0], 8 - i);
    __m256i xmmA2 = _mm256_slli_si256(in[0], 9 - i);

    __m256i newa = _mm256_slli_epi64(xmmA2, 2);
    newa = _mm256_xor_si256(newa, outC);
    out[0] = _mm256_srli_si256(in[0], i);
    out[0] = _mm256_blendv_epi8(out[0], newa, BlendMask256List[i - 1]);

    __m256i ksa = _mm256_slli_epi64(xmmA2, 8);
    __m256i ffa = _mm256_slli_epi64(xmmA1, 4);
    ksa = _mm256_xor_si256(ksa, ffa);
    ksa = _mm256_xor_si256(ksa, nld);

    __m256i nla1 = _mm256_slli_epi64(xmmA2, 7);
    __m256i nla2 = _mm256_slli_epi64(xmmA2, 1);
    __m256i nla = _mm256_and_si256(nla1, nla2);

    __m256i outA = _mm256_xor_si256(ksa, nla);

    //
    newb = _mm256_xor_si256(newb, outA);
    out[1] = _mm256_srli_si256(in[1], i);
    out[1] = _mm256_blendv_epi8(out[1], newb, BlendMask256List[i - 1]);

    ksa = _mm256_xor_si256(ksa, ksb);
    ksa = _mm256_xor_si256(ksa, ksc);
    ksa = _mm256_srli_si256(ksa, 16 - i);
    ks = _mm256_extractf128_si256(ksa, 1);

    return;
}

/*==== Template<i>x Triad Update and Tag Generation for YMM Register ====*/
template <int i>
static inline void triad_update_encryptionTag(const __m256i in[3], __m256i out[3], __m128i &ks) {
    // REG B
    __m256i xmmB1 = _mm256_slli_si256(in[1], 8 - i);
    __m256i xmmB3 = _mm256_slli_si256(in[1], 10 - i);

    __m256i newb = _mm256_slli_epi64(xmmB1, 2);
    __m256i ksb = _mm256_slli_epi64(xmmB3, 8);
    __m256i ffb = xmmB1;
    ksb = _mm256_xor_si256(ksb, ffb);

    __m256i nlb1 = _mm256_slli_epi64(xmmB3, 7);
    __m256i nlb2 = _mm256_slli_epi64(xmmB1, 1);
    __m256i nlb3 = _mm256_slli_epi64(xmmB3, 5);
    __m256i nlb = _mm256_and_si256(nlb1, nlb2);

    __m256i outB = _mm256_xor_si256(ksb, nlb);

    // REG C
    __m256i xmmC1 = _mm256_slli_si256(in[2], 8 - i);
    __m256i xmmC2 = _mm256_slli_si256(in[2], 9 - i);
    __m256i xmmC3 = _mm256_slli_si256(in[2], 10 - i);

    __m256i newc = _mm256_slli_epi64(xmmC3, 4);
    newc = _mm256_xor_si256(newc, outB);
    out[2] = _mm256_srli_si256(in[2], i);
    out[2] = _mm256_blendv_epi8(out[2], newc, BlendMask256List[i - 1]);

    __m256i ksc = _mm256_slli_epi64(xmmC3, 8);
    __m256i ffc = _mm256_slli_epi64(xmmC1, 4);
    ksc = _mm256_xor_si256(ksc, ffc);

    __m256i nlc1 = _mm256_slli_epi64(xmmC3, 7);
    __m256i nlc2 = _mm256_slli_epi64(xmmC2, 5);
    __m256i nlc3 = _mm256_slli_epi64(xmmC3, 5);
    __m256i nlc = _mm256_and_si256(nlc1, nlc2);
    __m256i nld = _mm256_and_si256(nlb3, nlc3);

    __m256i outC = _mm256_xor_si256(ksc, nlc);

    // REG A
    __m256i xmmA1 = _mm256_slli_si256(in[0], 8 - i);
    __m256i xmmA2 = _mm256_slli_si256(in[0], 9 - i);

    __m256i newa = _mm256_slli_epi64(xmmA2, 2);
    newa = _mm256_xor_si256(newa, outC);
    out[0] = _mm256_srli_si256(in[0], i);
    out[0] = _mm256_blendv_epi8(out[0], newa, BlendMask256List[i - 1]);

    __m256i ksa = _mm256_slli_epi64(xmmA2, 8);
    __m256i ffa = _mm256_slli_epi64(xmmA1, 4);
    ksa = _mm256_xor_si256(ksa, ffa);
    ksa = _mm256_xor_si256(ksa, nld);

    __m256i nla1 = _mm256_slli_epi64(xmmA2, 7);
    __m256i nla2 = _mm256_slli_epi64(xmmA2, 1);
    __m256i nla = _mm256_and_si256(nla1, nla2);

    __m256i outA = _mm256_xor_si256(ksa, nla);

    //
    newb = _mm256_xor_si256(newb, outA);
    out[1] = _mm256_srli_si256(in[1], i);
    out[1] = _mm256_blendv_epi8(out[1], newb, BlendMask256List[i - 1]);

    ksa = _mm256_xor_si256(ksa, ksb);
    ksa = _mm256_xor_si256(ksa, ksc);
    ksa = _mm256_srli_si256(ksa, 16 - i);
    ks = _mm256_castsi256_si128(ksa);

    return;
}



/*==== AEAD Encryption ====*/
void triad_high_speed_encryption(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *ad, unsigned long long adlen, const unsigned char *npub, const unsigned char *k) {
    unsigned char c_enc[4] = {0xFEU, 0xFFU, 0xFFU, 0xFFU};
    int i;

    MIE_ALIGN(256)
    unsigned char tmp0[32] = {0};
    memcpy(tmp0, k, 16);
    memcpy(tmp0 + 16, k, 16);
    MIE_ALIGN(256)
    unsigned char tmp1[32] = {0};
    memcpy(tmp1 + 4, npub, 12);
    memcpy(tmp1, c_enc, 4);
    memcpy(tmp1 + 20, npub, 12);
    memcpy(tmp1 + 16, c_enc, 4);

    __m256i in[3];
    in[2] = _mm256_load_si256((const __m256i *)(tmp0));
    in[1] = _mm256_load_si256((const __m256i *)(tmp1));
    in[0] = _mm256_unpacklo_epi8(in[2], in[1]);
    in[0] = _mm256_slli_si256(in[0], 6);


    
    // initialization (56 * 9 * 2 = 1008 rounds)
    __m128i p_bar_input = _mm_set_epi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    triad_update_encryption<7>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(p_bar_input));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(p_bar_input));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(p_bar_input));
    for (i = 0; i < 17; i++)
      triad_update_encryption<7>(in, in);
    triad_update_encryption<2>(in, in);
    
    __m256i s1024a = in[0];
    __m256i s1024b = in[1];
    __m256i s1024c = in[2];

    // absorb associated data
    for (i = 0; i < (int)(adlen)-6; i += 7)
    {
        __m128i xmm_a = _mm_loadu_si128((const __m128i *)(ad + i));
        xmm_a = _mm_slli_si128(xmm_a, 9);
        triad_update_encryption<7>(in, in);
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_a));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_a));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_a));
    }

    // absorb last block of associated data
    unsigned char last_ad_block[16];
    memcpy(last_ad_block, ad + i, adlen - i);
    __m128i xmm_a = _mm_loadu_si128((const __m128i *)(last_ad_block));
    int last_block_lenght = adlen - i;
    if (last_block_lenght == 1)
    {
      __m128i xmm_b = _mm_slli_si128(xmm_a, 15);
      triad_update_encryption<1>(in, in);
      in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
      in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
      in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 2)
    {
      __m128i xmm_b = _mm_slli_si128(xmm_a, 14);
      triad_update_encryption<2>(in, in);
      in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
      in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
      in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 3)
    {
      __m128i xmm_b = _mm_slli_si128(xmm_a, 13);
      triad_update_encryption<3>(in, in);
      in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
      in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
      in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 4)
    {
      __m128i xmm_b = _mm_slli_si128(xmm_a, 12);
      triad_update_encryption<4>(in, in);
      in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
      in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
      in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 5)
    {
      __m128i xmm_b = _mm_slli_si128(xmm_a, 11);
      triad_update_encryption<5>(in, in);
      in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
      in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
      in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 6)
    {
      __m128i xmm_b = _mm_slli_si128(xmm_a, 10);
      triad_update_encryption<6>(in, in);
      in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
      in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
      in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }

    // absorb adlength
    xmm_a = _mm_loadu_si128((const __m128i *)(&adlen));
    __m128i xmm_b = _mm_slli_si128(xmm_a, 9);
    triad_update_encryption<7>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));


    // blend
    in[0] = _mm256_blend_epi32(in[0], s1024a, 0xF0);
    in[1] = _mm256_blend_epi32(in[1], s1024b, 0xF0);
    in[2] = _mm256_blend_epi32(in[2], s1024c, 0xF0);

    
    // absorb and encrypt message
    __m128i ks;
    for (i = 0; i < (int)(mlen)-7; i += 7)
    {
        triad_update_encryption<7>(in, in, ks);

        // encrypt
        __m128i xmm_m = _mm_loadu_si128((const __m128i *)(m + i));
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        _mm_storeu_si128((__m128i *)(c + i), xmm_c);

        // absorb
        xmm_m = _mm_slli_si128(xmm_m, 9);
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));

    }

    
    unsigned char last_m_block[16];
    memcpy(last_m_block, m + i, mlen - i);
    __m128i xmm_m = _mm_loadu_si128((const __m128i *)(last_m_block));

    last_block_lenght = mlen - i;
    if (last_block_lenght == 1)
    {
        triad_update_encryption<1>(in, in, ks);
        xmm_b = _mm_slli_si128(xmm_m, 15);

        // encryption
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        unsigned char last_ct[16];
        _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
        memcpy(c + i, last_ct, last_block_lenght);

        // mac (absorb)
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 2)
    {
        triad_update_encryption<2>(in, in, ks);
        xmm_b = _mm_slli_si128(xmm_m, 14);

        // encryption
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        unsigned char last_ct[16];
        _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
        memcpy(c + i, last_ct, last_block_lenght);

        // mac (absorb)
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 3)
    {
        triad_update_encryption<3>(in, in, ks);
        xmm_b = _mm_slli_si128(xmm_m, 13);

        // encryption
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        unsigned char last_ct[16];
        _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
        memcpy(c + i, last_ct, last_block_lenght);

        // mac (absorb)
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 4)
    {
        triad_update_encryption<4>(in, in, ks);
        xmm_b = _mm_slli_si128(xmm_m, 12);

        // encryption
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        unsigned char last_ct[16];
        _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
        memcpy(c + i, last_ct, last_block_lenght);

        // mac (absorb)
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 5)
    {
        triad_update_encryption<5>(in, in, ks);
        xmm_b = _mm_slli_si128(xmm_m, 11);

        // encryption
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        unsigned char last_ct[16];
        _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
        memcpy(c + i, last_ct, last_block_lenght);

        // mac (absorb)
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 6)
    {
        triad_update_encryption<6>(in, in, ks);
        xmm_b = _mm_slli_si128(xmm_m, 10);

        // encryption
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        unsigned char last_ct[16];
        _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
        memcpy(c + i, last_ct, last_block_lenght);

        // mac (absorb)
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }
    else if (last_block_lenght == 7)
    {
        triad_update_encryption<7>(in, in, ks);
        xmm_b = _mm_slli_si128(xmm_m, 9);

        // encryption
        __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
        unsigned char last_ct[16];
        _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
        memcpy(c + i, last_ct, last_block_lenght);

        // mac (absorb)
        in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
        in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
        in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
    }


    
    // finalization
    triad_update_encryption<7>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(p_bar_input));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(p_bar_input));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(p_bar_input));
    for (i = 0; i < 17; i++)
        triad_update_encryption<7>(in, in);
    triad_update_encryption<2>(in, in);

    
    // tag generation
    unsigned char tag[32];
    triad_update_encryptionTag<1>(in, in, ks);
    _mm_storeu_si128((__m128i *)(tag), ks);

    triad_update_encryptionTag<7>(in, in, ks);
    _mm_storeu_si128((__m128i *)(tag + 1), ks);

    memcpy(c + mlen, tag, 8);
    
}


/*==== AEAD Decryption ====*/
void triad_high_speed_decryption(unsigned char *tag, unsigned char *rm, const unsigned char *c, unsigned long long clen, const unsigned char *ad, unsigned long long adlen, const unsigned char *npub, const unsigned char *k) {
  unsigned char c_enc[4] = { 0xFEU, 0xFFU, 0xFFU, 0xFFU };
  int i;

  MIE_ALIGN(256)
    unsigned char tmp0[32] = { 0 };
  memcpy(tmp0, k, 16);
  memcpy(tmp0 + 16, k, 16);
  MIE_ALIGN(256)
    unsigned char tmp1[32] = { 0 };
  memcpy(tmp1 + 4, npub, 12);
  memcpy(tmp1, c_enc, 4);
  memcpy(tmp1 + 20, npub, 12);
  memcpy(tmp1 + 16, c_enc, 4);

  __m256i in[3];
  in[2] = _mm256_load_si256((const __m256i *)(tmp0));
  in[1] = _mm256_load_si256((const __m256i *)(tmp1));
  in[0] = _mm256_unpacklo_epi8(in[2], in[1]);
  in[0] = _mm256_slli_si256(in[0], 6);



  // initialization (56 * 9 * 2 = 1008 rounds)
  __m128i p_bar_input = _mm_set_epi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
  triad_update_encryption<7>(in, in);
  in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(p_bar_input));
  in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(p_bar_input));
  in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(p_bar_input));
  for (i = 0; i < 17; i++)
    triad_update_encryption<7>(in, in);
  triad_update_encryption<2>(in, in);

  __m256i s1024a = in[0];
  __m256i s1024b = in[1];
  __m256i s1024c = in[2];

  // absorb associated data
  for (i = 0; i < (int)(adlen)-6; i += 7)
  {
    __m128i xmm_a = _mm_loadu_si128((const __m128i *)(ad + i));
    xmm_a = _mm_slli_si128(xmm_a, 9);
    triad_update_encryption<7>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_a));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_a));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_a));
  }

  // absorb last block of associated data
  unsigned char last_ad_block[16];
  memcpy(last_ad_block, ad + i, adlen - i);
  __m128i xmm_a = _mm_loadu_si128((const __m128i *)(last_ad_block));
  int last_block_lenght = adlen - i;
  if (last_block_lenght == 1)
  {
    __m128i xmm_b = _mm_slli_si128(xmm_a, 15);
    triad_update_encryption<1>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
  }
  else if (last_block_lenght == 2)
  {
    __m128i xmm_b = _mm_slli_si128(xmm_a, 14);
    triad_update_encryption<2>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
  }
  else if (last_block_lenght == 3)
  {
    __m128i xmm_b = _mm_slli_si128(xmm_a, 13);
    triad_update_encryption<3>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
  }
  else if (last_block_lenght == 4)
  {
    __m128i xmm_b = _mm_slli_si128(xmm_a, 12);
    triad_update_encryption<4>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
  }
  else if (last_block_lenght == 5)
  {
    __m128i xmm_b = _mm_slli_si128(xmm_a, 11);
    triad_update_encryption<5>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
  }
  else if (last_block_lenght == 6)
  {
    __m128i xmm_b = _mm_slli_si128(xmm_a, 10);
    triad_update_encryption<6>(in, in);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_b));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_b));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_b));
  }

  /*
  // blend
  in[0] = _mm256_blend_epi32(in[0], s1024a, 0xF0);
  in[1] = _mm256_blend_epi32(in[1], s1024b, 0xF0);
  in[2] = _mm256_blend_epi32(in[2], s1024c, 0xF0);

  // absorb adlen and generate key stream
  __m128i ks;
  __m128i xmm_m = _mm_loadu_si128((const __m128i *)(&adlen));
  xmm_m = _mm_slli_si128(xmm_m, 9);
  triad_update_encryption<7>(in, in, ks);
  in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
  in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
  in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));


  // absorb and encrypt message
  for (i = 0; i < (int)(clen) - 7 - 8; i += 7)
  {
    // decrypt
    __m128i xmm_c = _mm_loadu_si128((const __m128i *)(c + i));
    xmm_m = _mm_xor_si128(xmm_c, ks);
    _mm_storeu_si128((__m128i *)(rm + i), xmm_m);

    // update
    triad_update_encryption<7>(in, in, ks);

    // absorb
    xmm_m = _mm_slli_si128(xmm_m, 9);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }
  */
  // absorb adlen
  __m128i xmm_m = _mm_loadu_si128((const __m128i *)(&adlen));
  xmm_m = _mm_slli_si128(xmm_m, 9);
  triad_update_encryption<7>(in, in);
  in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
  in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
  in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));

  // blend
  in[0] = _mm256_blend_epi32(in[0], s1024a, 0xF0);
  in[1] = _mm256_blend_epi32(in[1], s1024b, 0xF0);
  in[2] = _mm256_blend_epi32(in[2], s1024c, 0xF0);

  // absorb and decrypt message
  __m128i ks;
  for (i = 0; i < (int)(clen) - 7 - 8; i += 7) {

    // update
    triad_update_encryption<7>(in, in, ks);

    // decrypt
    __m128i xmm_c = _mm_loadu_si128((const __m128i *)(c + i));
    xmm_m = _mm_xor_si128(xmm_c, ks);
    _mm_storeu_si128((__m128i *)(rm + i), xmm_m);

    // absorb
    xmm_m = _mm_slli_si128(xmm_m, 9);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }

  unsigned char last_m_block[16];
  memcpy(last_m_block, c + i, clen - 8 - i);
  __m128i xmm_c = _mm_loadu_si128((const __m128i *)(last_m_block));

  last_block_lenght = clen - 8 - i;
  if (last_block_lenght == 1)
  {
    // update
    triad_update_encryption<1>(in, in, ks);

    // decryption
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_lenght);

    // mac (absorb)
    xmm_m = _mm_slli_si128(xmm_m, 15);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }
  else if (last_block_lenght == 2)
  {
    // update
    triad_update_encryption<2>(in, in, ks);

    // decryption
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_lenght);

    // mac (absorb)
    xmm_m = _mm_slli_si128(xmm_m, 14);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }
  else if (last_block_lenght == 3)
  {
    // update
    triad_update_encryption<3>(in, in, ks);

    // decryption
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_lenght);

    // mac (absorb)
    xmm_m = _mm_slli_si128(xmm_m, 13);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }
  else if (last_block_lenght == 4)
  {
    // update
    triad_update_encryption<4>(in, in, ks);

    // decryption
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_lenght);

    // mac (absorb)
    xmm_m = _mm_slli_si128(xmm_m, 12);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }
  else if (last_block_lenght == 5)
  { 
    // update
    triad_update_encryption<5>(in, in, ks);
  
    // decryption
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_lenght);

    // mac (absorb)
    xmm_m = _mm_slli_si128(xmm_m, 11);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }
  else if (last_block_lenght == 6)
  {
    // update
    triad_update_encryption<6>(in, in, ks);

    // decryption
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_lenght);

    // mac (absorb)
    xmm_m = _mm_slli_si128(xmm_m, 10);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }
  else if (last_block_lenght == 7)
  {
    // update
    triad_update_encryption<7>(in, in, ks);

    // decryption
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_lenght);

    // mac (absorb)
    xmm_m = _mm_slli_si128(xmm_m, 9);
    in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(xmm_m));
    in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(xmm_m));
    in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(xmm_m));
  }

  
  // finalization
  triad_update_encryption<7>(in, in);
  in[0] = _mm256_xor_si256(in[0], _mm256_castsi128_si256(p_bar_input));
  in[1] = _mm256_xor_si256(in[1], _mm256_castsi128_si256(p_bar_input));
  in[2] = _mm256_xor_si256(in[2], _mm256_castsi128_si256(p_bar_input));
  for (i = 0; i < 17; i++)
    triad_update_encryption<7>(in, in);
  triad_update_encryption<2>(in, in);


  // tag generation
  unsigned char tmp[32];
  triad_update_encryptionTag<1>(in, in, ks);
  _mm_storeu_si128((__m128i *)(tmp), ks);

  triad_update_encryptionTag<7>(in, in, ks);
  _mm_storeu_si128((__m128i *)(tmp + 1), ks);

  memcpy(tag, tmp, 8);

}



int crypto_aead_encrypt(
  unsigned char *c,
  unsigned long long *clen,
  const unsigned char *m,
  unsigned long long mlen,
  const unsigned char *ad,
  unsigned long long adlen,
  const unsigned char *nsec,
  const unsigned char *npub,
  const unsigned char *k) {

  triad_high_speed_encryption(c, m, mlen, ad, adlen, npub, k);
  *clen = mlen + CRYPTO_ABYTES;

  return 0;
}

int crypto_aead_decrypt(
  unsigned char *m,
  unsigned long long *mlen,
  unsigned char *nsec,
  const unsigned char *c,
  unsigned long long clen,
  const unsigned char *ad,
  unsigned long long adlen,
  const unsigned char *npub,
  const unsigned char *k) {

  unsigned char tag[CRYPTO_ABYTES];
  int i;
  int flag = 0;
  *mlen = clen - CRYPTO_ABYTES;


  triad_high_speed_decryption(tag, m, c, clen, ad, adlen, npub, k);


  for (i = 0; i < 8; i++) {
    if (tag[i] != c[*mlen + i]) {
      flag = -1;
    }
  }
  return(flag);
}
