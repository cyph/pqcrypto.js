/**
 *
 *  bitslice_fft_256.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
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
#include "twist_factors_12.h"
#include "twiddles.h"
#include "a64_consts.h"

#include "vector_utils.h"

extern void bitslice_mul12_64 (uint64_t* d, const uint64_t* a, const uint64_t* b);
extern void bitslice_mul12_256(__m256i* d, const __m256i* a, const __m256i* b);

const uint64_t (*twist_factors)[12] = _twist_factors12_64_64;

static const uint64_t mask[5][2] =
{
    {0x8888888888888888ULL, 0x4444444444444444ULL},
    {0xC0C0C0C0C0C0C0C0ULL, 0x3030303030303030ULL},
    {0xF000F000F000F000ULL, 0x0F000F000F000F00ULL},
    {0xFF000000FF000000ULL, 0x00FF000000FF0000ULL},
    {0xFFFF000000000000ULL, 0x0000FFFF00000000ULL}
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

static void radix_conversions12(uint64_t (*in)[12])
{
    int i, j, k;
    
    for (j=0; j<=4; j++) {
        for (i=0; i<12; i++) {
            for (k=4; k>=j; k--) {
                in[0][i] ^= (in[0][i] & mask[k][0]) >> (1 << k);
                in[0][i] ^= (in[0][i] & mask[k][1]) >> (1 << k);
            }
        }
        bitslice_mul12_64(in[0], in[0], twist_factors[j]); /* Scaling for f(beta.x) */
    }
}

static void bitslice_butterflies12_256(__m256i out[][12], uint64_t (*in)[12], uint64_t mask)
{
    int32_t i, j, k, b, s;
    int32_t twiddle_idx = 0;
    __m256i tmp[12];
    __m256i va, vb;
    
    /**
     * Broadcast
     *
     * The value in `in` is transferred to `out`, but in reversed order
     * as in DIT FFT, where the input is in reversed order and the output
     * is in standard order.
     *
     * Then we broadcast the values in `out` as follows:
     *   out[  1: 63] <- out[  0]
     *   out[ 65:127] <- out[ 64]
     *              ...
     **/
    for (i=0; i<16; i++) {
        for (b=0; b<12; b++) {
            out[i][b] = _mm256_set_epi64x(-((in[0][b] >> reversal[4*i+3]) & 1),
                                          -((in[0][b] >> reversal[4*i+2]) & 1),
                                          -((in[0][b] >> reversal[4*i+1]) & 1),
                                          -((in[0][b] >> reversal[4*i+0]) & 1));
        }
    }
    
    /* i <- 0 */
    for (j=0; j<16; j++) {
        for (k=j; k<j+1; k++) {
            bitslice_mul12_256(tmp, out[k], twiddle_factors[0]);
            for (b=0; b<12; b++) {
                vb = _mm256_shuffle_epi32(tmp[b], _MM_SHUFFLE(3, 2, 3, 2));
                va = _mm256_slli_si256(out[k][b], 8);
                vb = _mm256_xor_si256(va, vb);
                out[k][b] = _mm256_xor_si256(out[k][b], vb);
            }
        }
    }
    
    /* i <- 1 */
    for (j=0; j<16; j++) {
        for (k=j; k<j+1; k++) {
            bitslice_mul12_256(tmp, out[k], twiddle_factors[1]);
            for (b=0; b<12; b++) {
                vb = _mm256_set_epi64x(tmp[b][3], tmp[b][2], tmp[b][3], tmp[b][2]);
                va = _mm256_permute2x128_si256(out[k][b], out[k][b], _MM_SHUFFLE(0, 0, 2, 0));
                vb = _mm256_xor_si256(va, vb);
                out[k][b] = _mm256_xor_si256(out[k][b], vb);
            }
        }
    }
    
    twiddle_idx = 2;
    for (i=2; i<=5; i++) {
        s = 1 << (i-2);
        for (j=0; j<16; j+=2*s) {
            for (k=j; k<j+s; k++) {
                bitslice_mul12_256(tmp, out[k+s], twiddle_factors[ twiddle_idx+(k-j) ]);
                for (b=0; b<12; b++) {
                    out[k][b]   = _mm256_xor_si256(out[k][b],   tmp[b]);
                    out[k+s][b] = _mm256_xor_si256(out[k+s][b], out[k][b]);
                }
            }
        }
        
        twiddle_idx += s;
    }
    
    /* XOR the output with (B[i]^64 & mask) */
    va = _mm256_set_epi64x(mask, mask, mask, mask);
    for (j=0; j<16; j++) {
        for (i=0; i<12; i++)
            out[j][i] ^= (a64_consts_256[j][i] & va);
    }
}

void bitslice_fft12_256(__m256i out[][12], uint64_t (*in)[12], uint64_t mask)
{
    radix_conversions12(in);
    bitslice_butterflies12_256(out, in, mask);
}

