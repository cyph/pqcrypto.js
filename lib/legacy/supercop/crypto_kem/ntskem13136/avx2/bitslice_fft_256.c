/**
 *
 *  bitslice_fft_256.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: AVX2
 *
 *  The implementation here is based on the work of McBit's, see
 *  https://tungchou.github.io/mcbits/
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdio.h>

#include "bitslice_fft_256.h"
#include "twiddles.h"
#include "twist_factors_13.h"
#include "vector_utils.h"

extern void bitslice_mul13_128(__m128i* d, const __m128i* a, const __m128i* b);
extern void bitslice_mul13_256(__m256i* d, const __m256i* a, const __m256i* b);

const __m128i (*twist_factors)[13] = _twist_factors13_136_128;

static const __m128i mask[5][2] =
{
    {{0x8888888888888888ULL, 0x8888888888888888ULL}, {0x4444444444444444ULL, 0x4444444444444444ULL}},
    {{0xC0C0C0C0C0C0C0C0ULL, 0xC0C0C0C0C0C0C0C0ULL}, {0x3030303030303030ULL, 0x3030303030303030ULL}},
    {{0xF000F000F000F000ULL, 0xF000F000F000F000ULL}, {0x0F000F000F000F00ULL, 0x0F000F000F000F00ULL}},
    {{0xFF000000FF000000ULL, 0xFF000000FF000000ULL}, {0x00FF000000FF0000ULL, 0x00FF000000FF0000ULL}},
    {{0xFFFF000000000000ULL, 0xFFFF000000000000ULL}, {0x0000FFFF00000000ULL, 0x0000FFFF00000000ULL}}
};

static const unsigned char reversal[64] =
{
    0, 32, 16, 48,  8, 40, 24, 56,
    4, 36, 20, 52, 12, 44, 28, 60,
    2, 34, 18, 50, 10, 42, 26, 58,
    6, 38, 22, 54, 14, 46, 30, 62,
    1, 33, 17, 49,  9, 41, 25, 57,
    5, 37, 21, 53, 13, 45, 29, 61,
    3, 35, 19, 51, 11, 43, 27, 59,
    7, 39, 23, 55, 15, 47, 31, 63
};

static void radix_conversions13(__m128i (*in)[13])
{
    int i, j, k;
    __m128i t0, t1;
    
    for (j=0; j<=5; j++) {
        for (i=0; i<13; i++) {
            in[1][i] = _mm_xor_si128(in[1][i], _mm_srli_si128(in[1][i], 8));
            in[0][i] = _mm_xor_si128(in[0][i], _mm_slli_si128(in[1][i], 8));
        }
        for (i=0; i<13; i++) {
            in[0][i][1] ^= ((in[0][i][1] >> 32) & 0x00000000FFFFFFFFULL);
            in[0][i][0] ^= (in[0][i][1] << 32);
            in[1][i][1] ^= ((in[1][i][1] >> 32) & 0x00000000FFFFFFFFULL);
            in[1][i][0] ^= (in[1][i][1] << 32);
        }
        for (k=4; k>=j; k--) {
            for (i=0; i<13; i++) {
                t0 = _mm_and_si128(in[0][i], mask[k][0]);
                t0 = _mm_srli_epi64(t0, (1 << k));
                in[0][i] = _mm_xor_si128(in[0][i], t0);
                
                t1 = _mm_and_si128(in[0][i], mask[k][1]);
                t1 = _mm_srli_epi64(t1, (1 << k));
                in[0][i] = _mm_xor_si128(in[0][i], t1);
                
                t0 = _mm_and_si128(in[1][i], mask[k][0]);
                t0 = _mm_srli_epi64(t0, (1 << k));
                in[1][i] = _mm_xor_si128(in[1][i], t0);
                
                t1 = _mm_and_si128(in[1][i], mask[k][1]);
                t1 = _mm_srli_epi64(t1, (1 << k));
                in[1][i] = _mm_xor_si128(in[1][i], t1);
            }
        }
        bitslice_mul13_128(in[0], in[0], twist_factors[2*j]);   /* Scaling for f(beta.x) */
        bitslice_mul13_128(in[1], in[1], twist_factors[2*j+1]); /* Scaling for f(beta.x) */
    }
    for (i=0; i<13; i++) {
        in[1][i] = _mm_xor_si128(in[1][i], _mm_srli_si128(in[1][i], 8));
        in[0][i] = _mm_xor_si128(in[0][i], _mm_slli_si128(in[1][i], 8));
    }
    bitslice_mul13_128(in[0], in[0], twist_factors[12]); /* Scaling for f(beta.x) */
    bitslice_mul13_128(in[1], in[1], twist_factors[13]); /* Scaling for f(beta.x) */
}

static void bitslice_butterflies13_256(__m256i out[][13], __m128i (*in)[13])
{
    int32_t i, j, k, s, b;
    uint64_t index = 0;
    uint32_t u32[16] = {0};
    __m128i x, t1, t0;
    __m256i t[13], u[13], v[13], w[13];
    const __m256i mask256 = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
    
    /**
     * Broadcast
     *
     * The value in `in` is transferred to `out`, but in reversed order
     * as in DIT FFT, where the input is in reversed order and the output
     * is in standard order.
     *
     * Then we broadcast the values in `out` as follows:
     *   out[  1: 31] <- out[  0]
     *   out[ 33: 63] <- out[ 32]
     *              ...
     **/
    for (j=0; j<32; j++) {
        for (i=0; i<13; i++) {
            u32[0] = (in[0][i][0] >> reversal[ 2*j ]) & 1;
            u32[1] = (in[1][i][0] >> reversal[ 2*j ]) & 1;
            u32[2] = (in[0][i][1] >> reversal[ 2*j ]) & 1;
            u32[3] = (in[1][i][1] >> reversal[ 2*j ]) & 1;
            u32[4] = (in[0][i][0] >> reversal[2*j+1]) & 1;
            u32[5] = (in[1][i][0] >> reversal[2*j+1]) & 1;
            u32[6] = (in[0][i][1] >> reversal[2*j+1]) & 1;
            u32[7] = (in[1][i][1] >> reversal[2*j+1]) & 1;
            out[j][i] = _mm256_set_epi32(-u32[7], -u32[6], -u32[5], -u32[4],
                                         -u32[3], -u32[2], -u32[1], -u32[0]);
        }
    }
    
    /**
     * Butterflies
     *
     * 2-point DFT: multiply with a twiddle-factor and
     * combine the two points.
     **/
    for (j=0; j<32; j+=2) {
        for (b=0; b<13; b++) {
            t[0] = _mm256_permutevar8x32_epi32(out[ j ][b], mask256);
            t[1] = _mm256_permutevar8x32_epi32(out[j+1][b], mask256);
            v[b] = _mm256_permute2x128_si256(t[0], t[1], 0x20);
            v[b] = _mm256_permute4x64_epi64(v[b], 0xD8);
            u[b] = _mm256_permute2x128_si256(t[0], t[1], 0x31);
            u[b] = _mm256_permute4x64_epi64(u[b], 0xD8);
        }
        bitslice_mul13_256(w, u, twiddle_factors[0]);
        for (b=0; b<13; b++) {
            v[b] = _mm256_xor_si256(v[b], w[b]);
            u[b] = _mm256_xor_si256(u[b], v[b]);
            out[ j ][b] = _mm256_unpacklo_epi32(v[b], u[b]);
            out[j+1][b] = _mm256_unpackhi_epi32(v[b], u[b]);
        }
    }
    for (j=0; j<32; j+=2) {
        for (b=0; b<13; b++) {
            t[0] = _mm256_permute4x64_epi64(out[ j ][b], 0xD8);
            t[1] = _mm256_permute4x64_epi64(out[j+1][b], 0xD8);
            v[b] = _mm256_permute2x128_si256(t[0], t[1], 0x20);
            u[b] = _mm256_permute2x128_si256(t[0], t[1], 0x31);
        }
        bitslice_mul13_256(t, u, twiddle_factors[1]);
        for (b=0; b<13; b++) {
            v[b] = _mm256_xor_si256(v[b], t[b]);
            u[b] = _mm256_xor_si256(u[b], v[b]);
            out[j][b]   = _mm256_set_epi64x(u[b][1], v[b][1], u[b][0], v[b][0]);
            out[j+1][b] = _mm256_set_epi64x(u[b][3], v[b][3], u[b][2], v[b][2]);
        }
    }
    for (j=0; j<32; j+=2) {
        for (b=0; b<13; b++) {
            t1   = _mm256_extracti128_si256(out[j+1][b], 1);
		    t0   = _mm256_extracti128_si256(out[j][b],   1);
            t[b] = _mm256_set_epi64x(t1[1], t1[0], t0[1], t0[0]);
        }
        bitslice_mul13_256(t, t, twiddle_factors[2]);
        for (b=0; b<13; b++) {
            t0 = _mm256_extracti128_si256(t[b], 0);
            t1 = _mm256_extracti128_si256(t[b], 1);
            t0 = _mm_xor_si128(_mm256_extracti128_si256(out[j][b],   0), t0);
            t1 = _mm_xor_si128(_mm256_extracti128_si256(out[j+1][b], 0), t1);
            
            x  = _mm_xor_si128(_mm256_extracti128_si256(out[j][b], 1), t0);
            out[j][b]   = _mm256_set_epi64x(x[1], x[0], t0[1], t0[0]);
            
            x  = _mm_xor_si128(_mm256_extracti128_si256(out[j+1][b], 1), t1);
            out[j+1][b] = _mm256_set_epi64x(x[1], x[0], t1[1], t1[0]);
        }
    }
    index = 3;
    for (i=1; i<6; i++) {
        s = 1 << (i-1);
        for (j=0; j<32; j+=2*s) {
            for (k=j; k<j+s; k++) {
                bitslice_mul13_256(t, out[k+s], twiddle_factors[index + (k-j)]);
                
                for (b=0; b<13; b++) {
                    out[k][b]   = _mm256_xor_si256(out[k][b],   t[b]);
                    out[k+s][b] = _mm256_xor_si256(out[k+s][b], out[k][b]);
                }
            }
        }
        index += s;
    }
}

void bitslice_fft13_256(__m256i out[][13], __m128i (*in)[13])
{
    radix_conversions13(in);
    bitslice_butterflies13_256(out, in);
}
