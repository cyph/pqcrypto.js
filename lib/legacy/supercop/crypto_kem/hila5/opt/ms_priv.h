// ms_priv.h   Modified from code released by Microsoft under MIT License
// 2017-05-11  Markku-Juhani O. Saarinen <mjos@iki.fi>

/*****************************************************************************
* LatticeCrypto: an efficient post-quantum Ring-Learning With Errors
* cryptography library
*
*    Copyright (c) Microsoft Corporation. All rights reserved.
*
*
* Abstract: internal header file
*
*****************************************************************************/

#ifndef MS_PRIV_H
#define MS_PRIV_H

#include <stdint.h>

// NTT tables

extern const int32_t mslc_psi_rev_ntt1024[1024];
extern const int32_t mslc_inv_rev_ntt1024[1024];

/******************** Function prototypes *******************/

// Forward NTT
void mslc_ntt(int32_t *a, const int32_t *psi_rev, unsigned int n);

// Inverse NTT
void mslc_intt(int32_t *a, const int32_t *omegainv_rev, 
    const int32_t omegainv1N_rev, const int32_t Ninv, unsigned int n);

// Two consecutive reductions modulo q
void mslc_two_reduce12289(int32_t *a, unsigned int n);

// Correction modulo q
void mslc_correction(int32_t *a, int32_t p, unsigned int n);

// Component-wise multiplication
void mslc_pmul(const int32_t *a, const int32_t *b, int32_t *c, 
    unsigned int n);

// Component-wise multiplication and addition
void mslc_pmuladd(const int32_t *a, const int32_t *b, const int32_t *c, 
    int32_t *d, unsigned int n);

// Component-wise multiplication with scalar
void mslc_smul(int32_t *a, int32_t scalar, unsigned int n);

#endif

