/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the CCA KEM functions.
 */

#include "r5_cca_kem.h"

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
 * Private functions
 ******************************************************************************/

/**
 * Verifies whether or not two byte strings are equal (in constant time).
 *
 * @param s1 the byte string to compare to
 * @param s2 the byte string to compare
 * @param n the number of bytes to compare
 * @return 0 if all size bytes are equal, non-zero otherwise
 */
static int verify(const void *s1, const void *s2, size_t n) {
    return constant_time_memcmp(s1, s2, n);
}

/*******************************************************************************
 * Public functions
 ******************************************************************************/

int r5_cca_kem_keygen(unsigned char *pk, unsigned char *sk, const parameters *params) {
    unsigned char *y = checked_malloc(params->kappa_bytes);

    /* Generate the base key pair */
    r5_cpa_pke_keygen(pk, sk, params);

    /* Append y and pk to sk */
    randombytes(y, params->kappa_bytes);
    memcpy(sk + params->kappa_bytes, y, params->kappa_bytes);
    memcpy(sk + params->kappa_bytes + params->kappa_bytes, pk, params->pk_size);

    free(y);

    return 0;
}

int r5_cca_kem_encapsulate(unsigned char *ct, unsigned char *k, const unsigned char *pk, const parameters *params) {
    unsigned char *hash_input;
    unsigned char *m;
    unsigned char *L_g_rho;

    /* Allocate space */
    hash_input = checked_malloc((size_t) (params->kappa_bytes + params->pk_size));
    m = checked_malloc(params->kappa_bytes);
    L_g_rho = checked_malloc(3U * params->kappa_bytes);

    /* Generate random m */
    randombytes(m, params->kappa_bytes);

    /* Determine l, g, and rho */
    memcpy(hash_input, m, params->kappa_bytes);
    memcpy(hash_input + params->kappa_bytes, pk, params->pk_size);
    hash(L_g_rho, 3U * params->kappa_bytes, hash_input, (size_t) (params->kappa_bytes + params->pk_size), params->kappa_bytes);

#ifdef NIST_KAT_GENERATION
    print_hex("r5_cca_kem_encapsulate: m", m, params->kappa_bytes, 1);
    print_hex("r5_cca_kem_encapsulate: L", L_g_rho, params->kappa_bytes, 1);
    print_hex("r5_cca_kem_encapsulate: g", L_g_rho + params->kappa_bytes, params->kappa_bytes, 1);
    print_hex("r5_cca_kem_encapsulate: rho", L_g_rho + 2 * params->kappa_bytes, params->kappa_bytes, 1);
#endif

    /* Encrypt m: ct = (U^T,v) */
    r5_cpa_pke_encrypt(ct, pk, m, L_g_rho + 2 * params->kappa_bytes, params);

    /* Append g: ct = (U^T,v,g) */
    memcpy(ct + params->ct_size, L_g_rho + params->kappa_bytes, params->kappa_bytes);

    /* k = H(L, ct) */
    hash_input = checked_realloc(hash_input, (size_t) (params->kappa_bytes + params->ct_size + params->kappa_bytes));
    memcpy(hash_input, L_g_rho, params->kappa_bytes);
    memcpy(hash_input + params->kappa_bytes, ct, (size_t) (params->ct_size + params->kappa_bytes));
    hash(k, params->kappa_bytes, hash_input, (size_t) (params->kappa_bytes + params->ct_size + params->kappa_bytes), params->kappa_bytes);

    free(hash_input);
    free(L_g_rho);
    free(m);

    return 0;
}

int r5_cca_kem_decapsulate(unsigned char *k, const unsigned char *ct, const unsigned char *sk, const parameters *params) {
    unsigned char *hash_input;
    unsigned char *m_prime;
    unsigned char *L_g_rho_prime;
    unsigned char *ct_prime;
    const unsigned char *y = sk + params->kappa_bytes; /* y is located after the sk */
    const unsigned char *pk = y + params->kappa_bytes; /* pk is located after y  */

    /* Allocate space */
    hash_input = checked_malloc((size_t) (params->kappa_bytes + params->pk_size));
    m_prime = checked_malloc(params->kappa_bytes);
    L_g_rho_prime = checked_malloc(3U * params->kappa_bytes);
    ct_prime = checked_malloc((size_t) (params->ct_size + params->kappa_bytes));

    /* Decrypt m' */
    r5_cpa_pke_decrypt(m_prime, sk, ct, params);

    /* Determine l', g', and rho' from m' */
    memcpy(hash_input, m_prime, params->kappa_bytes);
    memcpy(hash_input + params->kappa_bytes, pk, params->pk_size);
    hash(L_g_rho_prime, 3U * params->kappa_bytes, hash_input, (size_t) (params->kappa_bytes + params->pk_size), params->kappa_bytes);

#ifdef NIST_KAT_GENERATION
    print_hex("r5_cca_kem_decapsulate: m_prime", m_prime, params->kappa_bytes, 1);
    print_hex("r5_cca_kem_decapsulate: L_prime", L_g_rho_prime, params->kappa_bytes, 1);
    print_hex("r5_cca_kem_decapsulate: g_prime", L_g_rho_prime + params->kappa_bytes, params->kappa_bytes, 1);
    print_hex("r5_cca_kem_decapsulate: rho_prime", L_g_rho_prime + 2 * params->kappa_bytes, params->kappa_bytes, 1);
#endif

    /* Encrypt m: ct' = (U'^T,v') */
    r5_cpa_pke_encrypt(ct_prime, pk, m_prime, L_g_rho_prime + 2 * params->kappa_bytes, params);
    /* Append g': ct' = (U'^T,v',g') */
    memcpy(ct_prime + params->ct_size, L_g_rho_prime + params->kappa_bytes, params->kappa_bytes);

    /* k = H(L', ct') or k = H(y, ct') depending on fail status */
    hash_input = checked_realloc(hash_input, (size_t) (params->kappa_bytes + params->ct_size + params->kappa_bytes));
    uint8_t fail = (uint8_t) verify(ct, ct_prime, (size_t) (params->ct_size + params->kappa_bytes));
    memcpy(hash_input, L_g_rho_prime, params->kappa_bytes);
    memcpy(hash_input + params->kappa_bytes, ct_prime, (size_t) (params->ct_size + params->kappa_bytes));
    conditional_constant_time_memcpy(hash_input, y, params->kappa_bytes, fail); /* Overwrite L' with y in case of failure */
    hash(k, params->kappa_bytes, hash_input, (size_t) (params->kappa_bytes + params->ct_size + params->kappa_bytes), params->kappa_bytes);

    free(hash_input);
    free(m_prime);
    free(L_g_rho_prime);
    free(ct_prime);

    return 0;
}
