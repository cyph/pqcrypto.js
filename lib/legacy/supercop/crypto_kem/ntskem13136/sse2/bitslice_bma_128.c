/**
 *
 *  bitslice_bma_128.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: SSE2/SSE4.1
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bits.h"
#include "bitslice_bma_128.h"
#include "vector_utils.h"

#define PARAM_M         13
#define PARAM_T         136

extern void bitslice_mul13_128(__m128i* d, const __m128i* a, const __m128i* b);

static inline uint64_t MUX(uint64_t ctl, uint64_t a, uint64_t b)
{
    return b ^ (-ctl & (a ^ b));
}

static inline uint64_t EQ0(int64_t a)
{
    uint64_t b = (uint64_t)a;
    return ~(b | -b) >> 63;
}

static inline uint64_t LT(uint64_t a, uint64_t b)
{
    uint64_t c = a - b;
    return (c ^ ((a ^ b) & (b ^ c))) >> 63;
}

static inline __m128i vMUX(uint64_t ctl, __m128i a, __m128i b)
{
    return _mm_xor_si128(b,
                         _mm_and_si128(_mm_set1_epi64x(-ctl),
                                       _mm_xor_si128(a, b)));
}

static inline uint32_t vector_popcount_128(__m128i a)
{
    const __m128i a_hi = _mm_unpackhi_epi64(a, a);
    return popcount(_mm_cvtsi128_si64(a_hi)) + popcount(_mm_cvtsi128_si64(a));
}

static inline __m128i shift_left(__m128i a, int cnt)
{
    __m128i a1, a2;
    
    a1 = _mm_slli_epi64(a, cnt);
    a2 = _mm_slli_si128(a, 8);
    a2 = _mm_srli_epi64(a2, 64 - cnt);
    a1 = _mm_or_si128(a1, a2);
    
    return a1;
}

static inline __m128i shift_right(__m128i a, int cnt)
{
    __m128i a1, a2;
    
    a1 = _mm_srli_epi64(a, cnt);
    a2 = _mm_srli_si128(a, 8);
    a2 = _mm_slli_epi64(a2, 64 - cnt);
    a1 = _mm_or_si128(a1, a2);
    
    return a1;
}

static inline __m128i bit_reverse(__m128i x)
{
    __m128i a0, a1;
    a0 = _mm_and_si128(x, _mm_set_epi64x(0x5555555555555555ULL, 0x5555555555555555ULL));
    a1 = _mm_and_si128(x, _mm_set_epi64x(0xAAAAAAAAAAAAAAAAULL, 0xAAAAAAAAAAAAAAAAULL));
    x = _mm_or_si128(shift_left(a0, 1), shift_right(a1, 1));
    a0 = _mm_and_si128(x, _mm_set_epi64x(0x3333333333333333ULL, 0x3333333333333333ULL));
    a1 = _mm_and_si128(x, _mm_set_epi64x(0xCCCCCCCCCCCCCCCCULL, 0xCCCCCCCCCCCCCCCCULL));
    x = _mm_or_si128(shift_left(a0, 2), shift_right(a1, 2));
    a0 = _mm_and_si128(x, _mm_set_epi64x(0x0F0F0F0F0F0F0F0FULL, 0x0F0F0F0F0F0F0F0FULL));
    a1 = _mm_and_si128(x, _mm_set_epi64x(0xF0F0F0F0F0F0F0F0ULL, 0xF0F0F0F0F0F0F0F0ULL));
    x = _mm_or_si128(shift_left(a0, 4), shift_right(a1, 4));
    return _mm_shuffle_epi8(x, _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
}

void bitslice_bma(__m128i (*out)[13], __m128i s[][PARAM_M], int *xi)
{
    uint8_t b8;
    int32_t i, j, c;
    int64_t R = 0LL, L = 0LL;
    uint64_t _d = 0, d_eq_0, control;
    __m128i sigma[2][PARAM_M] = {{{0}}}, beta[2][PARAM_M] = {{{0}}};
    __m128i d[2][PARAM_M], delta[2][PARAM_M] = {{{0}}};
    __m128i psi[2][PARAM_M] = {{{0}}}, tmp[2][PARAM_M];
    __m128i ss[3][PARAM_M] = {{{0}}};
    __m128i mask;
    const __m128i out_mask = _mm_set_epi32(0x00, 0x00, 0x00, 0x000001FF);
    
    /* Initialise sigma and beta */
    sigma[0][0] = _mm_set_epi64x(0ULL, 1ULL);
    beta [0][0] = _mm_set_epi64x(0ULL, 2ULL);
    delta[0][0] = _mm_set1_epi64x(-1);
    psi  [0][0] = sigma[0][0];
    
    *xi = 0;
    for (j=0; j<PARAM_M; j++)
        ss[0][j][0] = s[0][j][0] & 1ULL;
    for (i=0; i<63; i++) {
        bitslice_mul13_128(d[0], sigma[0], ss[0]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = (vector_popcount_128(d[0][j]) & 1);
            d[0][j] = _mm_set1_epi64x((long long)-c);
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_128(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_128(psi[0], d[0], beta[0]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j][0] = MUX(control, beta[0][j][0], sigma[0][j][0]) << 1;
            psi[0][j] = _mm_xor_si128(tmp[0][j], psi[0][j]);
            delta[0][j][0] = MUX(control, delta[0][j][0], d[0][j][0]);
            sigma[0][j][0] = psi[0][j][0];
            ss[0][j] = shift_left(ss[0][j], 1);
            ss[0][j][0] |= ((s[0][j][0] & (1ULL << (i+1))) >> (i+1));
        }
    }
    for (;i<127; i++) {
        bitslice_mul13_128(d[0], sigma[0], ss[0]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = (vector_popcount_128(d[0][j]) & 1);
            d[0][j] = _mm_set1_epi64x((long long)-c);
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_128(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_128(psi[0], d[0], beta[0]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = vMUX(control, beta[0][j], sigma[0][j]);
            beta[0][j] = shift_left(beta[0][j], 1);
            psi[0][j] = _mm_xor_si128(tmp[0][j], psi[0][j]);
            delta[0][j] = vMUX(control, delta[0][j], d[0][j]);
            
            sigma[0][j][0] = psi[0][j][0]; sigma[0][j][1] = psi[0][j][1];
            ss[0][j] = shift_left(ss[0][j], 1);
            ss[0][j][0] |= ((s[0][j][1] & (1ULL << (i-63))) >> (i-63));
        }
    }
    for (;i<191; i++) {
        bitslice_mul13_128(d[0], sigma[0], ss[0]);
        bitslice_mul13_128(d[1], sigma[1], ss[1]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = ((vector_popcount_128(d[0][j]) + vector_popcount_128(d[1][j])) & 1);
            d[0][j] = _mm_set1_epi64x((long long)-c);
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_128(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_128(tmp[1], delta[1], sigma[1]);
        bitslice_mul13_128(psi[0], d[0], beta[0]);
        bitslice_mul13_128(psi[1], d[0], beta[1]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = vMUX(control, beta[0][j], sigma[0][j]);
            beta[1][j] = vMUX(control, beta[1][j], sigma[1][j]);
            beta[1][j] = shift_left(beta[1][j], 1);
            beta[1][j][0] |= ((beta[0][j][1] >> 63) & 1);
            beta[0][j] = shift_left(beta[0][j], 1);
            psi[0][j] = _mm_xor_si128(tmp[0][j], psi[0][j]);
            psi[1][j] = _mm_xor_si128(tmp[1][j], psi[1][j]);
            delta[0][j] = vMUX(control, delta[0][j], d[0][j]);
            delta[1][j] = vMUX(control, delta[1][j], d[0][j]);
            
            sigma[0][j][0] = psi[0][j][0]; sigma[0][j][1] = psi[0][j][1];
            sigma[1][j][0] = psi[1][j][0]; sigma[1][j][1] = psi[1][j][1];
            ss[1][j] = shift_left(ss[1][j], 1);
            ss[1][j][0] |= ((ss[0][j][1] >> 63) & 1);
            ss[0][j] = shift_left(ss[0][j], 1);
            ss[0][j][0] |= ((s[1][j][0] & (1ULL << (i-127))) >> (i-127));
        }
    }
    for (;i<255; i++) {
        bitslice_mul13_128(d[0], sigma[0], ss[0]);
        bitslice_mul13_128(d[1], sigma[1], ss[1]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = ((vector_popcount_128(d[0][j]) + vector_popcount_128(d[1][j])) & 1);
            d[0][j] = _mm_set1_epi64x((long long)-c);
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_128(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_128(tmp[1], delta[1], sigma[1]);
        bitslice_mul13_128(psi[0], d[0], beta[0]);
        bitslice_mul13_128(psi[1], d[0], beta[1]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = vMUX(control, beta[0][j], sigma[0][j]);
            beta[1][j] = vMUX(control, beta[1][j], sigma[1][j]);
            beta[1][j] = shift_left(beta[1][j], 1);
            beta[1][j][0] |= ((beta[0][j][1] >> 63) & 1);
            beta[0][j] = shift_left(beta[0][j], 1);
            psi[0][j] = _mm_xor_si128(tmp[0][j], psi[0][j]);
            psi[1][j] = _mm_xor_si128(tmp[1][j], psi[1][j]);
            delta[0][j] = vMUX(control, delta[0][j], d[0][j]);
            delta[1][j] = vMUX(control, delta[1][j], d[0][j]);
            
            sigma[0][j][0] = psi[0][j][0]; sigma[0][j][1] = psi[0][j][1];
            sigma[1][j][0] = psi[1][j][0]; sigma[1][j][1] = psi[1][j][1];
            ss[1][j] = shift_left(ss[1][j], 1);
            ss[1][j][0] |= ((ss[0][j][1] >> 63) & 1);
            ss[0][j] = shift_left(ss[0][j], 1);
            ss[0][j][0] |= ((s[1][j][1] & (1ULL << (i-191))) >> (i-191));
        }
    }
    for (;i<2*PARAM_T-1; i++) {
        bitslice_mul13_128(d[0], sigma[0], ss[0]);
        bitslice_mul13_128(d[1], sigma[1], ss[1]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = ((vector_popcount_128(d[0][j]) + vector_popcount_128(d[1][j])) & 1);
            d[0][j] = _mm_set1_epi64x((long long)-c);
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_128(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_128(tmp[1], delta[1], sigma[1]);
        bitslice_mul13_128(psi[0], d[0], beta[0]);
        bitslice_mul13_128(psi[1], d[0], beta[1]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = vMUX(control, beta[0][j], sigma[0][j]);
            beta[1][j] = vMUX(control, beta[1][j], sigma[1][j]);
            beta[1][j] = shift_left(beta[1][j], 1);
            beta[1][j][0] |= ((beta[0][j][1] >> 63) & 1);
            beta[0][j] = shift_left(beta[0][j], 1);
            psi[0][j] = _mm_xor_si128(tmp[0][j], psi[0][j]);
            psi[1][j] = _mm_xor_si128(tmp[1][j], psi[1][j]);
            delta[0][j] = vMUX(control, delta[0][j], d[0][j]);
            delta[1][j] = vMUX(control, delta[1][j], d[0][j]);
            
            sigma[0][j][0] = psi[0][j][0]; sigma[0][j][1] = psi[0][j][1];
            sigma[1][j][0] = psi[1][j][0]; sigma[1][j][1] = psi[1][j][1];
            ss[1][j] = shift_left(ss[1][j], 1);
            ss[1][j][0] |= ((ss[0][j][1] >> 63) & 1);
            ss[0][j] = shift_left(ss[0][j], 1);
            ss[0][j][0] |= ((s[2][j][0] & (1ULL << (i-255))) >> (i-255));
        }
    }
    bitslice_mul13_128(d[0], sigma[0], ss[0]);
    bitslice_mul13_128(d[1], sigma[1], ss[1]);
    for (_d=0,j=0; j<PARAM_M; j++) {
        c = ((vector_popcount_128(d[0][j]) + vector_popcount_128(d[1][j])) & 1);
        d[0][j] = _mm_set1_epi64x((long long)-c);
        _d |= c;
    }

    bitslice_mul13_128(sigma[0], d[0], beta[0]);
    bitslice_mul13_128(sigma[1], d[0], beta[1]);
    bitslice_mul13_128(tmp[0], delta[0], psi[0]);
    bitslice_mul13_128(tmp[1], delta[1], psi[1]);
    for (_d=0,j=0; j<PARAM_M; j++) {
        psi[0][j] = _mm_xor_si128(tmp[0][j], sigma[0][j]);
        psi[1][j] = _mm_xor_si128(tmp[1][j], sigma[1][j]);
        _d |= psi[1][j][0];
    }

    __asm__ __volatile__ ("bsrq %1, %q0" : "=r" (L) : "rm" (_d));
    *xi = CT_is_less_than(128+((int32_t)L), (int32_t)(PARAM_T - (R>>1)));
    
    b8 = (0x0E ^ (((int8_t)-(*xi)) & 0x8E));
	mask = _mm_set_epi64x(0x8080808080808080ULL, 0x808080808080000FULL | (b8 << 8));

    for (j=0; j<PARAM_M; j++) {
        __m128i carry_mask = _mm_set_epi64x(0ULL, (uint64_t) ((0x01ULL ^ *xi) << 8));
        __m128i lo_carry = _mm_and_si128(psi[0][j], carry_mask);
        __m128i hi_carry = _mm_and_si128(psi[1][j], carry_mask);
        lo_carry = _mm_srli_si128(lo_carry, 1);
        hi_carry = _mm_srli_si128(hi_carry, 1);
        out[0][j] = bit_reverse(psi[0][j]);
        psi[0][j] = bit_reverse(psi[1][j]);
        out[1][j] = _mm_shuffle_epi8(out[0][j], mask);
        out[0][j] = _mm_alignr_epi8(out[0][j], psi[0][j], 15);
        out[0][j] = shift_left(out[0][j], 0x01 ^ *xi);
        out[1][j] = shift_left(out[1][j], 0x01 ^ *xi);
        out[0][j] = _mm_xor_si128(out[0][j], hi_carry);
        out[1][j] = _mm_xor_si128(out[1][j], lo_carry);
        out[1][j] = _mm_and_si128(out[1][j], out_mask);
    }
}
