/**
 *  vector_utils.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: SSE2/SSE4.1
 *
 *  This file is part of the additional implemention of NTS-KEM
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

__m128i vector_ff_or_128(const __m128i *in)
{
    int32_t i;
    __m128i out = in[0];
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

void vector_load_1d_128(__m128i b[PARAM_M], const uint16_t* a, int32_t size)
{
    int32_t i, k;
    uint64_t v[2];
    
    for (i=0; i<PARAM_M; i++) {
        memset(v, 0, sizeof(v));
        for (k=0; k<size; k++) {
            v[k >> 6] |= ((uint64_t)((a[k] >> i) & 1) << (k & 63));
        }
        b[i] = _mm_set_epi64x(v[1], v[0]);
    }
}

void vector_load_2d_128(__m128i (*b)[PARAM_M], const uint16_t* a, int32_t size)
{
    int32_t i, j, k;
    uint64_t v[2];
    
    for (i=0; i<PARAM_M; i++) {
        for (j=0; j<size/128; j++) {
            memset(v, 0, sizeof(v));
            for (k=0; k<128; k++) {
                v[k >> 6] |= ((uint64_t)((a[128*j + k] >> i) & 1) << (k & 63));
            }
            b[j][i] = _mm_set_epi64x(v[1], v[0]);
        }
        if (size & 0x7F) {
            memset(v, 0, sizeof(v));
            for (k=0; k<(size & 0x7F); k++) {
                v[k >> 6] |= ((uint64_t)((a[128*j + k] >> i) & 1) << (k & 63));
            }
            b[j][i] = _mm_set_epi64x(v[1], v[0]);
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

void vector_store_1d_128(uint16_t* b, const __m128i a[PARAM_M], int32_t size)
{
    int32_t i, j;
    uint64_t v[2];
    
    memset(b, 0, size*sizeof(uint16_t));
    for (i=0; i<PARAM_M; i++) {
        _mm_store_si128((__m128i*)v, a[i]);
        for (j=0; j<size; j++) {
            b[j] |= (bit_value(v, j) << i);
        }
    }
}

void vector_store_2d_128(uint16_t* b, const __m128i (*a)[PARAM_M], int32_t size)
{
    int32_t i, j, k;
    uint64_t v[2];
    
    memset(b, 0, size*sizeof(uint16_t));
    for (i=0; i<PARAM_M; i++) {
        for (j=0; j<size/128; j++) {
            _mm_store_si128((__m128i*)v, a[j][i]);
            for (k=0; k<128; k++) {
                b[j*128 + k] |= (bit_value(v, k) << i);
            }
        }
        if (size & 0x7F) {
            _mm_store_si128((__m128i*)v, a[j][i]);
            for (k=0; k<(size & 0x7F); k++) {
                b[j*128 + k] |= (bit_value(v, k) << i);
            }
        }
    }
}
