/**
 *
 *  bitslice_fft_128.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: SSE2/SSE4.1
 *
 *  The implementation here is based on the work of McBit's, see
 *  https://tungchou.github.io/mcbits/
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include "bitslice_fft_128.h"
#include "twiddles.h"
#include "twist_factors_13.h"
#include "vector_utils.h"

extern void bitslice_mul13_128(__m128i* d, const __m128i* a, const __m128i* b);

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

static void bitslice_butterflies13_128(__m128i out[][13], __m128i (*in)[13])
{
    int32_t i, j, k, s, b;
    uint64_t index = 0;
    uint32_t u0, u1, u2, u3;
    __m128i t[13], u[13], v[13], w[13];

    /**
     * Broadcast
     *
     * The value in `in` is transferred to `out`, but in reversed order
     * as in DIT FFT, where the input is in reversed order and the output
     * is in standard order.
     *
     * Then we broadcast the values in `out` as follows:
     *   out[  1: 31] <- out[  0]
     *   out[ 33: 63] <- out[ 64]
     *              ...
     **/
    for (j=0; j<64; j++) {
        for (i=0; i<13; i++) {
            u0 = (in[0][i][0] >> reversal[j]) & 1;
            u1 = (in[1][i][0] >> reversal[j]) & 1;
            u2 = (in[0][i][1] >> reversal[j]) & 1;
            u3 = (in[1][i][1] >> reversal[j]) & 1;
            out[j][i] = _mm_set_epi32(-u3, -u2, -u1, -u0);
        }
    }

    /**
     * Butterflies
     *
     * 2-point DFT: multiply with a twiddle-factor and
     * combine the two points.
     **/
    for (j=0; j<64; j+=2) {
        for (b=0; b<13; b++) {
            u[b] = _mm_set_epi32(((out[j+1][b][1] >> 32) & 0xFFFFFFFF),
                                 ((out[j+1][b][0] >> 32) & 0xFFFFFFFF),
                                 ((out[ j ][b][1] >> 32) & 0xFFFFFFFF),
                                 ((out[ j ][b][0] >> 32) & 0xFFFFFFFF));
            v[b] = _mm_set_epi32((out[j+1][b][1] & 0xFFFFFFFF),
                                 (out[j+1][b][0] & 0xFFFFFFFF),
                                 (out[ j ][b][1] & 0xFFFFFFFF),
                                 (out[ j ][b][0] & 0xFFFFFFFF));
        }
        bitslice_mul13_128(w, u, twiddle_factors[0]);
        for (b=0; b<13; b++) {
            v[b] = _mm_xor_si128(v[b], w[b]);
            u[b] = _mm_xor_si128(u[b], v[b]);
            out[j][b]   = _mm_unpacklo_epi32(v[b], u[b]);
            out[j+1][b] = _mm_unpackhi_epi32(v[b], u[b]);
        }
    }

    for (j=0; j<64; j+=2) {
        for (b=0; b<13; b++) {
            u[b] = _mm_set_epi64x(out[j+1][b][1], out[j][b][1]);
            v[b] = _mm_set_epi64x(out[j+1][b][0], out[j][b][0]);
        }
        bitslice_mul13_128(w, u, twiddle_factors[1]);
        for (b=0; b<13; b++) {
            v[b] = _mm_xor_si128(v[b], w[b]);
            u[b] = _mm_xor_si128(u[b], v[b]);
            out[j][b]   = _mm_set_epi64x(u[b][0], v[b][0]);
            out[j+1][b] = _mm_set_epi64x(u[b][1], v[b][1]);
        }
    }
    index = 2;

    for (i=0; i<6; i++) {
        s = 1 << i;
        for (j=0; j<64; j+=2*s) {
            for (k=j; k<j+s; k++) {
                bitslice_mul13_128(t, out[k+s], twiddle_factors[index + (k-j)]);
                
                for (b=0; b<13; b++) {
                    out[k][b]   = _mm_xor_si128(out[k][b],   t[b]);
                    out[k+s][b] = _mm_xor_si128(out[k+s][b], out[k][b]);
                }
            }
        }
        index += s;
    }
}

void bitslice_fft13_128(__m128i out[][13], __m128i (*in)[13])
{
    radix_conversions13(in);
    bitslice_butterflies13_128(out, in);
}

