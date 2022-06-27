// ms_ntt_misc.c
// 2017-05-11  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Modified from code released by Microsoft under MIT License

/*****************************************************************************
* LatticeCrypto: an efficient post-quantum Ring-Learning With Errors
*   cryptography library
*
*    Copyright (c) Microsoft Corporation. All rights reserved.
*
*
* Abstract: NTT functions and other polynomial operations
*
*****************************************************************************/

#include "ms_priv.h"

// Component-wise multiplication with scalar

void mslc_smul(int32_t *a, int32_t scalar, unsigned int n) 
{ 
    unsigned int i;

    for (i = 0; i < n; i++) {
        a[i] = a[i] * scalar;
    }
}

// Correction modulo q

void mslc_correction(int32_t *a, int32_t p, unsigned int n) 
{ 
    unsigned int i;
    int32_t mask;

    for (i = 0; i < n; i++) {
        mask = a[i] >> (4 * sizeof(int32_t) - 1);
        a[i] += (p & mask) - p;
        mask = a[i] >> (4 * sizeof(int32_t) - 1);
        a[i] += (p & mask);
    }
}

