/**
 *
 *  bitslice_bma_64.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: Intel 64-bit
 *
 *  This file is part of the optimized implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bits.h"
#include "bitslice_bma_64.h"
#include "vector_utils.h"

#define PARAM_M         13
#define PARAM_T         136

extern void bitslice_mul13_64(uint64_t* d, const uint64_t* a, const uint64_t* b);

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

static inline uint64_t bit_reverse(uint64_t x)
{
    unsigned long long t;
    
    x = (x << 32) | (x >> 32);
    x = (x & 0x0001FFFF0001FFFFLL) << 15 | (x & 0xFFFE0000FFFE0000LL) >> 17;
    t = (x ^ (x >> 10)) & 0x003F801F003F801FLL;
    x = (t | (t << 10)) ^ x;
    t = (x ^ (x >> 4)) & 0x0E0384210E038421LL;
    x = (t | (t << 4)) ^ x;
    t = (x ^ (x >> 2)) & 0x2248884222488842LL;
    x = (t | (t << 2)) ^ x;
    
    return x;
}

static inline uint64_t bsr(uint64_t x)
{
    uint64_t r = 0;
    
    if (x & 0xffffffff00000000UL )  { x >>= 32;  r += 32; }
    if (x & 0xffff0000UL) { x >>= 16; r += 16; }
    if (x & 0x0000ff00UL) { x >>=  8; r +=  8; }
    if (x & 0x000000f0UL) { x >>=  4; r +=  4; }
    if (x & 0x0000000cUL) { x >>=  2; r +=  2; }
    if (x & 0x00000002UL) {           r +=  1; }
    
    return MUX(EQ0(x), 0, r);
}

void bitslice_bma(uint64_t (*out)[13], uint64_t s[][PARAM_M], int *xi)
{
    int32_t i, j, c;
    int64_t R = 0LL, L = 0LL;
    uint64_t _d = 0, d_eq_0, control;
    uint64_t sigma[4][PARAM_M] = {{0}}, beta[4][PARAM_M] = {{0}};
    uint64_t d[4][PARAM_M], delta[4][PARAM_M] = {{0}};
    uint64_t psi[4][PARAM_M] = {{0}}, tmp[4][PARAM_M];
    uint64_t ss[6][PARAM_M] = {{0}};
    
    /* Initialise sigma and beta */
    sigma[1][0] = 0ULL; sigma[0][0] = 1ULL;
    beta [1][0] = 0ULL; beta [0][0] = 2ULL;
    delta[1][0] = -1;   delta[0][0] = -1;
    psi[1][0] = sigma[1][0]; psi[0][0] = sigma[0][0];

    *xi = 0;
    for (j=0; j<PARAM_M; j++)
        ss[0][j] = s[0][j] & 1ULL;
    for (i=0; i<63; i++) {
        bitslice_mul13_64(d[0], sigma[0], ss[0]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = (vector_popcount(d[0][j]) & 1);
            d[1][j] = (long long)-c;
            d[0][j] = (long long)-c;
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_64(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_64(psi[0], d[0], beta[0]);

        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = MUX(control, beta[0][j], sigma[0][j]) << 1;
            psi[0][j] ^= tmp[0][j];
            delta[0][j] = MUX(control, delta[0][j], d[0][j]);
            sigma[0][j] = psi[0][j];
            ss[0][j] <<= 1;
            ss[0][j] |= ((s[0][j] & (1ULL << (i+1))) >> (i+1));
        }
    }
    for (;i<127; i++) {
        bitslice_mul13_64(d[0], sigma[0], ss[0]);
        bitslice_mul13_64(d[1], sigma[1], ss[1]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = ((vector_popcount(d[0][j]) + vector_popcount(d[1][j])) & 1);
            d[1][j] = (long long)-c;
            d[0][j] = (long long)-c;
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_64(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_64(tmp[1], delta[1], sigma[1]);
        bitslice_mul13_64(psi[0], d[0], beta[0]);
        bitslice_mul13_64(psi[1], d[0], beta[1]);

        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = MUX(control, beta[0][j], sigma[0][j]);
            beta[1][j] = MUX(control, beta[1][j], sigma[1][j]);
            beta[1][j] = (beta[1][j] << 1) | ((beta[0][j] >> 63) & 1);
            beta[0][j] <<= 1;
            psi[0][j] ^= tmp[0][j];
            psi[1][j] ^= tmp[1][j];
            delta[0][j] = MUX(control, delta[0][j], d[0][j]);
            delta[1][j] = MUX(control, delta[1][j], d[0][j]);

            sigma[0][j] = psi[0][j]; sigma[1][j] = psi[1][j];
            ss[1][j] = (ss[1][j] << 1) | ((ss[0][j] >> 63) & 1);
            ss[0][j] <<= 1;
            ss[0][j] |= ((s[1][j] & (1ULL << (i-63))) >> (i-63));
        }
    }
    for (;i<191; i++) {
        bitslice_mul13_64(d[0], sigma[0], ss[0]);
        bitslice_mul13_64(d[1], sigma[1], ss[1]);
        bitslice_mul13_64(d[2], sigma[2], ss[2]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = ((vector_popcount(d[0][j]) + vector_popcount(d[1][j]) + vector_popcount(d[2][j])) & 1);
            d[1][j] = (long long)-c;
            d[0][j] = (long long)-c;
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_64(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_64(tmp[1], delta[1], sigma[1]);
        bitslice_mul13_64(tmp[2], delta[2], sigma[2]);
        bitslice_mul13_64(psi[0], d[0], beta[0]);
        bitslice_mul13_64(psi[1], d[0], beta[1]);
        bitslice_mul13_64(psi[2], d[0], beta[2]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = MUX(control, beta[0][j], sigma[0][j]);
            beta[1][j] = MUX(control, beta[1][j], sigma[1][j]);
            beta[2][j] = MUX(control, beta[2][j], sigma[2][j]);
            beta[2][j] = (beta[2][j] << 1) | ((beta[1][j] >> 63) & 1);
            beta[1][j] = (beta[1][j] << 1) | ((beta[0][j] >> 63) & 1);
            beta[0][j] <<= 1;
            psi[0][j] ^= tmp[0][j];
            psi[1][j] ^= tmp[1][j];
            psi[2][j] ^= tmp[2][j];
            delta[0][j] = MUX(control, delta[0][j], d[0][j]);
            delta[1][j] = MUX(control, delta[1][j], d[0][j]);
            delta[2][j] = MUX(control, delta[2][j], d[0][j]);

            sigma[0][j] = psi[0][j]; sigma[1][j] = psi[1][j];
            sigma[2][j] = psi[2][j];
            ss[2][j] = (ss[2][j] << 1) | ((ss[1][j] >> 63) & 1);
            ss[1][j] = (ss[1][j] << 1) | ((ss[0][j] >> 63) & 1);
            ss[0][j] <<= 1;
            ss[0][j] |= ((s[2][j] & (1ULL << (i-127))) >> (i-127));
        }
    }
    for (;i<255; i++) {
        bitslice_mul13_64(d[0], sigma[0], ss[0]);
        bitslice_mul13_64(d[1], sigma[1], ss[1]);
        bitslice_mul13_64(d[2], sigma[2], ss[2]);
        bitslice_mul13_64(d[3], sigma[3], ss[3]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = ((vector_popcount(d[0][j]) + vector_popcount(d[1][j]) +
                  vector_popcount(d[2][j]) + vector_popcount(d[3][j])) & 1);
            d[1][j] = (long long)-c;
            d[0][j] = (long long)-c;
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_64(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_64(tmp[1], delta[1], sigma[1]);
        bitslice_mul13_64(tmp[2], delta[2], sigma[2]);
        bitslice_mul13_64(tmp[3], delta[3], sigma[3]);
        bitslice_mul13_64(psi[0], d[0], beta[0]);
        bitslice_mul13_64(psi[1], d[0], beta[1]);
        bitslice_mul13_64(psi[2], d[0], beta[2]);
        bitslice_mul13_64(psi[3], d[0], beta[3]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = MUX(control, beta[0][j], sigma[0][j]);
            beta[1][j] = MUX(control, beta[1][j], sigma[1][j]);
            beta[2][j] = MUX(control, beta[2][j], sigma[2][j]);
            beta[3][j] = MUX(control, beta[3][j], sigma[3][j]);
            beta[3][j] = (beta[3][j] << 1) | ((beta[2][j] >> 63) & 1);
            beta[2][j] = (beta[2][j] << 1) | ((beta[1][j] >> 63) & 1);
            beta[1][j] = (beta[1][j] << 1) | ((beta[0][j] >> 63) & 1);
            beta[0][j] <<= 1;
            psi[0][j] ^= tmp[0][j];
            psi[1][j] ^= tmp[1][j];
            psi[2][j] ^= tmp[2][j];
            psi[3][j] ^= tmp[3][j];
            delta[0][j] = MUX(control, delta[0][j], d[0][j]);
            delta[1][j] = MUX(control, delta[1][j], d[0][j]);
            delta[2][j] = MUX(control, delta[2][j], d[0][j]);
            delta[3][j] = MUX(control, delta[3][j], d[0][j]);
            
            sigma[0][j] = psi[0][j]; sigma[1][j] = psi[1][j];
            sigma[2][j] = psi[2][j]; sigma[3][j] = psi[3][j];
            ss[3][j] = (ss[3][j] << 1) | ((ss[2][j] >> 63) & 1);
            ss[2][j] = (ss[2][j] << 1) | ((ss[1][j] >> 63) & 1);
            ss[1][j] = (ss[1][j] << 1) | ((ss[0][j] >> 63) & 1);
            ss[0][j] <<= 1;
            ss[0][j] |= ((s[3][j] & (1ULL << (i-191))) >> (i-191));
        }
    }
    for (;i<2*PARAM_T-1; i++) {
        bitslice_mul13_64(d[0], sigma[0], ss[0]);
        bitslice_mul13_64(d[1], sigma[1], ss[1]);
        bitslice_mul13_64(d[2], sigma[2], ss[2]);
        bitslice_mul13_64(d[3], sigma[3], ss[3]);
        for (_d=0,j=0; j<PARAM_M; j++) {
            c = ((vector_popcount(d[0][j]) + vector_popcount(d[1][j]) +
                  vector_popcount(d[2][j]) + vector_popcount(d[3][j])) & 1);
            d[1][j] = (long long)-c;
            d[0][j] = (long long)-c;
            _d |= c;
        }
        
        d_eq_0 = EQ0(_d);
        control = d_eq_0 || LT(i, L<<1);
        L = MUX(control, L, i-L+1);
        R = MUX(control, R + d_eq_0, 0);
        
        bitslice_mul13_64(tmp[0], delta[0], sigma[0]);
        bitslice_mul13_64(tmp[1], delta[1], sigma[1]);
        bitslice_mul13_64(tmp[2], delta[2], sigma[2]);
        bitslice_mul13_64(tmp[3], delta[3], sigma[3]);
        bitslice_mul13_64(psi[0], d[0], beta[0]);
        bitslice_mul13_64(psi[1], d[0], beta[1]);
        bitslice_mul13_64(psi[2], d[0], beta[2]);
        bitslice_mul13_64(psi[3], d[0], beta[3]);
        
        for (j=0; j<PARAM_M; j++) {
            beta[0][j] = MUX(control, beta[0][j], sigma[0][j]);
            beta[1][j] = MUX(control, beta[1][j], sigma[1][j]);
            beta[2][j] = MUX(control, beta[2][j], sigma[2][j]);
            beta[3][j] = MUX(control, beta[3][j], sigma[3][j]);
            beta[3][j] = (beta[3][j] << 1) | ((beta[2][j] >> 63) & 1);
            beta[2][j] = (beta[2][j] << 1) | ((beta[1][j] >> 63) & 1);
            beta[1][j] = (beta[1][j] << 1) | ((beta[0][j] >> 63) & 1);
            beta[0][j] <<= 1;
            psi[0][j] ^= tmp[0][j];
            psi[1][j] ^= tmp[1][j];
            psi[2][j] ^= tmp[2][j];
            psi[3][j] ^= tmp[3][j];
            delta[0][j] = MUX(control, delta[0][j], d[0][j]);
            delta[1][j] = MUX(control, delta[1][j], d[0][j]);
            delta[2][j] = MUX(control, delta[2][j], d[0][j]);
            delta[3][j] = MUX(control, delta[3][j], d[0][j]);
            
            sigma[0][j] = psi[0][j]; sigma[1][j] = psi[1][j];
            sigma[2][j] = psi[2][j]; sigma[3][j] = psi[3][j];
            ss[3][j] = (ss[3][j] << 1) | ((ss[2][j] >> 63) & 1);
            ss[2][j] = (ss[2][j] << 1) | ((ss[1][j] >> 63) & 1);
            ss[1][j] = (ss[1][j] << 1) | ((ss[0][j] >> 63) & 1);
            ss[0][j] <<= 1;
            ss[0][j] |= ((s[4][j] & (1ULL << (i-255))) >> (i-255));
        }
    }
    bitslice_mul13_64(d[0], sigma[0], ss[0]);
    bitslice_mul13_64(d[1], sigma[1], ss[1]);
    bitslice_mul13_64(d[2], sigma[2], ss[2]);
    bitslice_mul13_64(d[3], sigma[3], ss[3]);
    for (_d=0,j=0; j<PARAM_M; j++) {
        c = ((vector_popcount(d[0][j]) + vector_popcount(d[1][j]) +
              vector_popcount(d[2][j]) + vector_popcount(d[3][j])) & 1);
        d[1][j] = (long long)-c;
        d[0][j] = (long long)-c;
        _d |= c;
    }

    bitslice_mul13_64(sigma[0], d[0], beta[0]);
    bitslice_mul13_64(sigma[1], d[0], beta[1]);
    bitslice_mul13_64(sigma[2], d[0], beta[2]);
    bitslice_mul13_64(sigma[3], d[0], beta[3]);
    bitslice_mul13_64(tmp[0], delta[0], psi[0]);
    bitslice_mul13_64(tmp[1], delta[1], psi[1]);
    bitslice_mul13_64(tmp[2], delta[2], psi[2]);
    bitslice_mul13_64(tmp[3], delta[3], psi[3]);

    for (_d=0,j=0; j<PARAM_M; j++) {
        psi[0][j] = tmp[0][j] ^ sigma[0][j];
        psi[1][j] = tmp[1][j] ^ sigma[1][j];
        psi[2][j] = tmp[2][j] ^ sigma[2][j];
        psi[3][j] = tmp[3][j] ^ sigma[3][j];
        _d |= psi[2][j];
    }

    L = bsr(_d);
    *xi = CT_is_less_than(128+((int32_t)L), (int32_t)(PARAM_T - (R>>1)));
    for (j=0; j<PARAM_M; j++) {
        d[0][0] = (1ULL << (55+(*xi)))-1;
        tmp[0][0] = bit_reverse(psi[2][j]);
        tmp[1][0] = bit_reverse(psi[1][j]);
        tmp[2][0] = bit_reverse(psi[0][j]);
        out[0][j] = (tmp[1][0] << (9-(*xi))) | ((tmp[0][0] >> (55+(*xi))) & d[0][0]);
        out[1][j] = (tmp[2][0] << (9-(*xi))) | ((tmp[1][0] >> (55+(*xi))) & d[0][0]);
        out[2][j] = ((tmp[2][0] >> (55+(*xi))) & d[0][0]);
    }
}
