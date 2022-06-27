/**
 *
 *  bitslice_fft_64.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 80)
 *  Platform: Intel 64-bit
 *
 *  The implementation here is based on the work of McBit's, see
 *  https://tungchou.github.io/mcbits/
 *
 *  This file is part of the optimized implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include "bitslice_fft_64.h"
#include "twiddles.h"
#include "twist_factors_13.h"
#include "vector_utils.h"

extern void bitslice_mul13_64(uint64_t* d, const uint64_t* a, const uint64_t* b);

const uint64_t (*twist_factors)[13] = _twist_factors13_80_64;

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

static void radix_conversions13(uint64_t (*in)[13])
{
    int i, j, k;
    uint64_t t0, t1;
    
    for (j=0; j<=5; j++) {
        for (i=0; i<13; i++) {
            in[1][i] ^= (in[1][i] >> 32);
            in[0][i] ^= (in[1][i] << 32);
        }
        for (i=0; i<13; i++) {
            for (k=4; k>=j; k--) {
                t0 = in[0][i] & mask[k][0];
                t0 = t0 >> (1 << k);
                in[0][i] ^= t0;
                t0 = in[1][i] & mask[k][0];
                t0 = t0 >> (1 << k);
                in[1][i] ^= t0;
                
                t1 = in[0][i] & mask[k][1];
                t1 = t1 >> (1 << k);
                in[0][i] ^= t1;
                t1 = in[1][i] & mask[k][1];
                t1 = t1 >> (1 << k);
                in[1][i] ^= t1;
            }
        }
        bitslice_mul13_64(in[0], in[0], twist_factors[2*j]); /* Scaling for f(beta.x) */
        bitslice_mul13_64(in[1], in[1], twist_factors[2*j+1]); /* Scaling for f(beta.x) */
    }
}

static void bitslice_butterflies13_64(uint64_t out[][13], uint64_t (*in)[13])
{
    int32_t i, j, k, s, b;
    uint64_t v0, v1, index = 0;
    uint64_t t[2][13], u[2][13], v[2][13], w[2][13];
    
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
    for (j=0; j<64; j++) {
        for (i=0; i<13; i++) {
            v0 = (in[0][i] >> reversal[j]) & 1;
            v1 = (in[1][i] >> reversal[j]) & 1;
            out[2*j][i] = -v0;
            out[2*j+1][i] = -v1;
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
            u[0][b] = out[2*j+1][b];
            u[1][b] = out[2*j+3][b];
            v[0][b] = out[2*j][b];
            v[1][b] = out[2*j+2][b];
        }
        bitslice_mul13_64(w[0], u[0], twiddle_factors[0]);
        bitslice_mul13_64(w[1], u[1], twiddle_factors[1]);
        for (b=0; b<13; b++) {
            v[0][b] ^= w[0][b];
            v[1][b] ^= w[1][b];
            u[0][b] ^= v[0][b];
            u[1][b] ^= v[1][b];
            out[2*j][b] = v[0][b];
            out[2*j+1][b] = u[0][b];
            out[2*j+2][b] = v[1][b];
            out[2*j+3][b] = u[1][b];
        }
    }
    index += 2;
    for (i=0; i<6; i++) {
        s = 1 << i;
        for (j=0; j<64; j+=2*s) {
            for (k=j; k<j+s; k++) {
                bitslice_mul13_64(t[0], out[2*(k+s)], twiddle_factors[index + 2*(k-j)]);
                bitslice_mul13_64(t[1], out[2*(k+s)+1], twiddle_factors[index + 2*(k-j)+1]);

                for (b=0; b<13; b++) {
                    out[2*k][b] ^= t[0][b];
                    out[2*k+1][b] ^= t[1][b];
                    out[2*(k+s)][b] ^= out[2*k][b];
                    out[2*(k+s)+1][b] ^= out[2*k+1][b];
                }
            }
        }
        index += (2*s);
    }
}

void bitslice_fft13_64(uint64_t out[][13], uint64_t (*in)[13])
{
    radix_conversions13(in);
    bitslice_butterflies13_64(out, in);
}
