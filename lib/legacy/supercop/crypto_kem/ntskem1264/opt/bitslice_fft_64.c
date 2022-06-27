/**
 *
 *  bitslice_fft_64.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
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
#include "a64_consts.h"
#include "twist_factors_12.h"
#include "vector_utils.h"

extern void bitslice_mul12_64 (uint64_t* d, const uint64_t* a, const uint64_t* b);
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

static void bitslice_butterflies12_64(uint64_t (*out)[12], uint64_t (*in)[12], uint64_t mask)
{
    int32_t i, j, k, b, s;
    int32_t twiddle_idx = 0;
    uint64_t v[2][12], u;
    
    /**
     * Broadcast
     *
     * The value in `in` is transferred to `out`, but in reversed order
     * as in DIT FFT, where the input is in reversed order and the output
     * is in standard order.
     *
     * Then we broadcast the values in `out` as follows:
     *   out[  1: 31] <- out[  0]
     *   out[ 32: 63] <- out[ 32]
     *              ...
     **/
    for (i=0; i<32; i++) {
        for (b=0; b<12; b++) {
            j = (in[0][b] >> reversal[2*i])   & 1;
            k = (in[0][b] >> reversal[2*i+1]) & 1;
            out[2*i][b] = -j;
            out[2*i+1][b] = -k;
        }
    }
    
    /* i <- 0 */
    for (j=0; j<32; j++) {
        bitslice_mul12_64(v[0], out[2*j],   twiddle_factors[ twiddle_idx   ]);
        bitslice_mul12_64(v[1], out[2*j+1], twiddle_factors[ twiddle_idx+1 ]);
        for (b=0; b<12; b++) {
            v[0][b] = v[1][b];
            u = (out[2*j][b] ^ v[0][b]);
            out[2*j+1][b] ^= (v[1][b] ^ out[2*j][b]);
            out[2*j][b] = u;
        }
    }
    
    twiddle_idx = 2;
    for (i=1; i<=5; i++) {
        s = 1 << (i-1);
        for (j=0; j<32; j+=2*s) {
            for (k=j; k<j+s; k++) {
                bitslice_mul12_64(v[0], out[2*(k+s)], twiddle_factors[ twiddle_idx+2*(k-j) ]);
                bitslice_mul12_64(v[1], out[2*(k+s)+1], twiddle_factors[ twiddle_idx+2*(k-j)+1 ]);
                
                for (b=0; b<12; b++) {
                    out[2*k][b] = out[2*k][b] ^ v[0][b];
                    out[2*k+1][b] = out[2*k+1][b] ^ v[1][b];
                    out[2*(k+s)][b] = out[2*(k+s)][b] ^ out[2*k][b];
                    out[2*(k+s)+1][b] = out[2*(k+s)+1][b] ^ out[2*k+1][b];
                }
            }
        }
        
        twiddle_idx += (2*s);
    }
    
    /* XOR the output with (B[i]^64 & mask) */
    v[0][0] = mask; v[1][0] = mask;
    for (j=0; j<32; j++) {
        for (i=0; i<12; i++) {
            out[2*j][i] ^= (a64_consts_64[2*j][i] & v[0][0]);
            out[2*j+1][i] ^= (a64_consts_64[2*j+1][i] & v[1][0]);
        }
    }
}

void bitslice_fft12_64(uint64_t (*out)[12], uint64_t (*in)[12], uint64_t mask)
{
    radix_conversions12(in);
    bitslice_butterflies12_64(out, in, mask);
}
