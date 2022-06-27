/**
 *  vector_utils.c
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
#include "vector_utils.h"

#undef bit_value
#define bit_value(v,i)          (((v)[(i)>>6] & (1ULL<<((i) & 63))) >> ((i) & 63))

uint64_t vector_ff_or_64(const uint64_t *in)
{
    int32_t i;
    uint64_t out = in[0];
    for (i=1; i<PARAM_M; i++)
        out |= in[i];
    return out;
}

void vector_load_1d_64(uint64_t b[PARAM_M], const uint16_t* a, int32_t size)
{
    int32_t i, j;
    
    for (i=0; i<PARAM_M; i++) {
        b[i] = 0ULL;
        for (j=0; j<size; j++) {
            b[i] |= ((uint64_t)((a[j] >> i) & 1) << (j & 63));
        }
    }
}

void vector_load_2d_64(uint64_t (*b)[PARAM_M], const uint16_t* a, int32_t size)
{
    int32_t i, j, k;

    for (i=0; i<PARAM_M; i++) {
        for (j=0; j<size/64; j++) {
            b[j][i] = 0ULL;
            for (k=0; k<64; k++) {
                b[j][i] |= ((uint64_t)((a[64*j + k] >> i) & 1) << k);
            }
        }
        if (size & 0x3F) {
            b[j][i] = 0ULL;
            for (k=0; k<(size & 0x3F); k++) {
                b[j][i] |= ((uint64_t)((a[64*j + k] >> i) & 1) << k);
            }
        }
    }
}

void vector_store_1d_64(uint16_t* b, const uint64_t a[PARAM_M], int32_t size)
{
    int32_t i, j;
    
    memset(b, 0, size*sizeof(uint16_t));
    for (i=0; i<PARAM_M; i++) {
        for (j=0; j<size; j++) {
            b[j] |= (((a[i] >> j) & 1) << i);
        }
    }
}

void vector_store_2d_64(uint16_t* b, const uint64_t (*a)[PARAM_M], int32_t size)
{
    int32_t i, j, k;
    uint64_t v[1];
    
    memset(b, 0, size*sizeof(uint16_t));
    for (i=0; i<PARAM_M; i++) {
        for (j=0; j<size/64; j++) {
            v[0] = a[j][i];
            for (k=0; k<64; k++) {
                b[j*64 + k] |= (bit_value(v, k) << i);
            }
        }
        if (size & 0x3F) {
            v[0] = a[j][i];
            for (k=0; k<(size & 0x3F); k++) {
                b[j*64 + k] |= (bit_value(v, k) << i);
            }
        }
    }
}
