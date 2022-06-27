/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the CPA KEM functions.
 */

#include "r5_cpa_kem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r5_core.h"
#include "r5_cpa_pke.h"
#include "pack.h"
#include "r5_hash.h"
#include "misc.h"
#include "r5_memory.h"
#include "rng.h"
#include "drbg.h"

/*******************************************************************************
 * Public functions
 ******************************************************************************/

int r5_cpa_kem_keygen(unsigned char *pk, unsigned char *sk, const parameters *params) {
    return r5_cpa_pke_keygen(pk, sk, params);
}

int r5_cpa_kem_encapsulate(unsigned char *ct, unsigned char *k, const unsigned char *pk, const parameters *params) {
    unsigned char *rho;
    unsigned char *m;
    unsigned char *hash_input;

    /* Generate a random m */
    m = checked_malloc(params->kappa_bytes);
    randombytes(m, params->kappa_bytes);

    /* Randomly generate rho */
    rho = checked_malloc(params->kappa_bytes);
    randombytes(rho, params->kappa_bytes);

    /* Encrypt m */
    r5_cpa_pke_encrypt(ct, pk, m, rho, params);

    /* k = H(m, ct) */
    hash_input = checked_malloc((size_t) (params->kappa_bytes + params->ct_size));
    memcpy(hash_input, m, params->kappa_bytes);
    memcpy(hash_input + params->kappa_bytes, ct, params->ct_size);
    hash(k, params->kappa_bytes, hash_input, (size_t) (params->kappa_bytes + params->ct_size), params->kappa_bytes);

    free(hash_input);
    free(rho);
    free(m);

    return 0;
}

int r5_cpa_kem_decapsulate(unsigned char *k, const unsigned char *ct, const unsigned char *sk, const parameters *params) {
    unsigned char *hash_input;
    unsigned char *m;

    /* Allocate space */
    hash_input = checked_malloc((size_t) (params->kappa_bytes + params->ct_size));
    m = checked_malloc(params->kappa_bytes);

    /* Decrypt m */
    r5_cpa_pke_decrypt(m, sk, ct, params);

    /* k = H(m, ct) */
    memcpy(hash_input, m, params->kappa_bytes);
    memcpy(hash_input + params->kappa_bytes, ct, params->ct_size);
    hash(k, params->kappa_bytes, hash_input, (size_t) (params->kappa_bytes + params->ct_size), params->kappa_bytes);

    free(hash_input);
    free(m);

    return 0;
}
