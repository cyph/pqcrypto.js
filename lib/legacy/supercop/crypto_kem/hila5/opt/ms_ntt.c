// ms_ntt.c    Modified from code released by Microsoft under MIT License
// 2017-05-11  Markku-Juhani O. Saarinen <mjos@iki.fi>

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

// Reduction modulo q

static int32_t mslc_reduce12289(int64_t a) 
{ 
    int32_t c0, c1;

    c0 = (int32_t)(a & 0xFFF);
    c1 = (int32_t)(a >> 12);

    return (3 * c0 - c1);
}

// Two merged reductions modulo q

static int32_t mslc_reduce12289_2x(int64_t a) 
{
    int32_t c0, c1, c2;

    c0 = (int32_t)(a & 0xFFF);
    c1 = (int32_t)((a >> 12) & 0xFFF);
    c2 = (int32_t)(a >> 24);

    return (9 * c0 - 3 * c1 + c2);
}

// Forward NTT

void mslc_ntt(int32_t *a, const int32_t *psi_rev, unsigned int n) 
{ 
    unsigned int m, i, j, j1, j2, k = n;
    int32_t S, U, V;

    for (m = 1; m < 128; m = 2 * m) {
        k = k >> 1;
        for (i = 0; i < m; i++) {
            j1 = 2 * i * k;
            j2 = j1 + k - 1;
            S = psi_rev[m + i];
            for (j = j1; j <= j2; j++) {
                U = a[j];
                V = mslc_reduce12289((int64_t) a[j + k] * S);
                a[j] = U + V;
                a[j + k] = U - V;
            }
        }
    }

    k = 4;
    for (i = 0; i < 128; i++) {
        j1 = 8 * i;
        j2 = j1 + 3;
        S = psi_rev[i + 128];
        for (j = j1; j <= j2; j++) {
            U = mslc_reduce12289((int64_t) a[j]);
            V = mslc_reduce12289_2x((int64_t) a[j + 4] * S);
            a[j] = U + V;
            a[j + 4] = U - V;
        }
    }

    for (m = 256; m < n; m = 2 * m) {
        k = k >> 1;
        for (i = 0; i < m; i++) {
            j1 = 2 * i * k;
            j2 = j1 + k - 1;
            S = psi_rev[m + i];
            for (j = j1; j <= j2; j++) {
                U = a[j];
                V = mslc_reduce12289((int64_t) a[j + k] * S);
                a[j] = U + V;
                a[j + k] = U - V;
            }
        }
    }
    return;
}


// Inverse NTT

void mslc_intt(int32_t *a, const int32_t *omegainv_rev, 
    const int32_t omegainv1n_rev, const int32_t ninv, unsigned int n) 
{ 
    unsigned int m, h, i, j, j1, j2, k = 1;
    int32_t S, U, V;
    int64_t temp;

    for (m = n; m > 2; m >>= 1) {
        j1 = 0;
        h = m >> 1;
        for (i = 0; i < h; i++) {
            j2 = j1 + k - 1;
            S = omegainv_rev[h + i];
            for (j = j1; j <= j2; j++) {
                U = a[j];
                V = a[j + k];
                a[j] = U + V;
                temp = (int64_t)(U - V) * S;
                if (m == 32) {
                    a[j] = mslc_reduce12289((int64_t) a[j]);
                    a[j + k] = mslc_reduce12289_2x(temp);
                } else {
                    a[j + k] = mslc_reduce12289(temp);
                }
            }
            j1 = j1 + 2 * k;
        }
        k = 2 * k;
    }
    for (j = 0; j < k; j++) {
        U = a[j];
        V = a[j + k];
        a[j] = mslc_reduce12289((int64_t)(U + V) * ninv);
        a[j + k] = mslc_reduce12289((int64_t)(U - V) * omegainv1n_rev);
    }
    return;
}


// Two consecutive reductions modulo q

void mslc_two_reduce12289(int32_t *a, unsigned int n) 
{ 
    unsigned int i;

    for (i = 0; i < n; i++) {
        a[i] = mslc_reduce12289((int64_t) a[i]);
        a[i] = mslc_reduce12289((int64_t) a[i]);
    }
}


// Component-wise multiplication

void mslc_pmul(const int32_t *a, const int32_t *b, int32_t *c, 
    unsigned int n) 
{ 
    unsigned int i;

    for (i = 0; i < n; i++) {
        c[i] = mslc_reduce12289((int64_t) a[i] * b[i]);
        c[i] = mslc_reduce12289((int64_t) c[i]);
    }
}


// Component-wise multiplication and addition

void mslc_pmuladd(const int32_t *a, const int32_t *b, const int32_t *c, 
    int32_t *d, unsigned int n)
{ 
    unsigned int i;

    for (i = 0; i < n; i++) {
        d[i] = mslc_reduce12289((int64_t) a[i] * b[i] + c[i]);
        d[i] = mslc_reduce12289((int64_t) d[i]);
    }
}


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

