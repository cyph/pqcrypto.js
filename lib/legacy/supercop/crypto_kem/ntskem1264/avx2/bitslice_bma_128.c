/**
 *
 *  bitslice_bma_128.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: AVX2
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bitslice_bma_128.h"
#include "bits.h"
#include "vector_utils.h"

#define PARAM_M         12
#define PARAM_T         64

extern void bitslice_mul12_64 (uint64_t* c, const uint64_t* a, const uint64_t* b);
extern void bitslice_mul12_128(__m128i* c,  const __m128i* a,  const __m128i* b);
extern uint16_t ff_inv_12(const void* unused, uint16_t a);

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

static inline uint64_t bit_reverse(uint64_t x)
{
    x = ((x & 0x5555555555555555ULL) << 1) | ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
    x = ((x & 0x3333333333333333ULL) << 2) | ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
    x = ((x & 0x0F0F0F0F0F0F0F0FULL) << 4) | ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
    __asm__ ("bswap %0" : "=r" (x) : "0" (x));
    return x;
}

void bitslice_bma(uint64_t (*out)[12], __m128i (*s)[PARAM_M], int *xi)
{
    int32_t i, j, c;
    int64_t R = 0LL, L = 0LL;
    uint64_t _d = 0, d_eq_0, control;
    __m128i sigma[PARAM_M] = {{0}}, beta[PARAM_M] = {{0}};
    __m128i d[PARAM_M], delta[PARAM_M] = {{0}};
    __m128i psi[PARAM_M] = {{0}}, tmp[PARAM_M];
    __m128i ss[PARAM_M] = {{0}};
    uint16_t a = 0, a_inv = 0;
    uint64_t inv[PARAM_M] = {0};

    /* Initialise sigma and beta */
    sigma[ 0] = _mm_set_epi64x(0ULL, 1ULL);
    beta [ 0] = _mm_set_epi64x(0ULL, 2ULL);
    delta[ 0] = _mm_set1_epi64x(-1);
    psi  [ 0] = sigma[0];
    
    *xi = 0;
    for (j=0; j<PARAM_M; j++)
        ss[j][0] = s[0][j][0] & 1ULL;
    for (i=0; i<63; i++) {
        bitslice_mul12_128(d, sigma, ss);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = (vector_popcount_128(d[j]) & 1);
            d[j] = _mm_set1_epi64x((long long)-c);
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul12_128(sigma, d, beta);
        bitslice_mul12_128(tmp, delta, psi);
        
        for (j=0; j<PARAM_M; j++) {
            beta[j] = vMUX(control, beta[j], psi[j]);
            beta[j] = shift_left(beta[j], 1);
            psi[j] = _mm_xor_si128(tmp[j], sigma[j]);
            delta[j] = vMUX(control, delta[j], d[j]);
            sigma[j] = psi[j];
            ss[j] = shift_left(ss[j], 1);
            ss[j][0] |= ((s[0][j][0] & (1ULL << (i+1))) >> (i+1));
        }
    }
    for (;i<2*PARAM_T-1; i++) {
        bitslice_mul12_128(d, sigma, ss);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = (vector_popcount_128(d[j]) & 1);
            d[j] = _mm_set1_epi64x((long long)-c);
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul12_128(sigma, d, beta);
        bitslice_mul12_128(tmp, delta, psi);
        
        for (j=0; j<PARAM_M; j++) {
            beta[j] = vMUX(control, beta[j], psi[j]);
            beta[j] = shift_left(beta[j], 1);
            psi[j] = _mm_xor_si128(tmp[j], sigma[j]);
            delta[j] = vMUX(control, delta[j], d[j]);
            sigma[j] = (__m128i)_mm_shuffle_ps((__m128)psi[j], (__m128)psi[j], _MM_SHUFFLE(3, 2, 1, 0));
            ss[j] = shift_left(ss[j], 1);
            ss[j][0] |= ((s[0][j][1] & (1ULL << (i-63))) >> (i-63));
        }
    }
    bitslice_mul12_128(d, sigma, ss);
    for (_d=0,j=0; j<PARAM_M; j++) {
        c = (vector_popcount_128(d[j]) & 1);
        d[j] = _mm_set1_epi64x((long long)-c);
    }
    
    bitslice_mul12_128(sigma, d, beta);
    bitslice_mul12_128(tmp, delta, psi);
    for (_d=0,j=0; j<PARAM_M; j++) {
        psi[j] = _mm_xor_si128(tmp[j], sigma[j]);
        _d |= ((psi[j][1] << 63) | ((psi[j][0]>>1) & 0x7FFFFFFFFFFFFFFFULL));
    }

    __asm__ __volatile__ ("bsrq %1, %q0" : "=r" (L) : "rm" (_d));
    L = L + 1;
    *xi = CT_is_less_than(((int32_t)L), (int32_t)(PARAM_T - (R>>1)));
    for (j=0; j<PARAM_M; j++) {
        a |= (psi[j][0] & 1) << j;
        out[0][j] = (bit_reverse(psi[j][0])); // >> (63 - PARAM_T + *xi)) & _d;
        out[0][j] <<= (1 - *xi);
        out[0][j] |= (psi[j][1] & 1);
    }

    a_inv = ff_inv_12(NULL, a);
    for (j=0; j<PARAM_M; j++) {
        inv[j] = -((a_inv >> j) & 1);
    }
    bitslice_mul12_64(out[0], out[0], inv);
}
