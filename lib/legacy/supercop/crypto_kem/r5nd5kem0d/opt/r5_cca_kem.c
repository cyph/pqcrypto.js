/*
 * Copyright (c) 2018, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

#include "r5_cpa_pke.h"
#include "r5_parameter_sets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r5_hash.h"
#include "rng.h"
#include "misc.h"
#include "r5_memory.h"

// CCA-KEM KeyGen()

int r5_cca_kem_keygen(uint8_t *pk, uint8_t *sk) {
    uint8_t y[PARAMS_KAPPA_BYTES];

    /* Generate the base key pair */
    r5_cpa_pke_keygen(pk, sk);

    /* Append y and pk to sk */
    randombytes(y, PARAMS_KAPPA_BYTES);
    memcpy(sk + PARAMS_KAPPA_BYTES, y, PARAMS_KAPPA_BYTES);
    memcpy(sk + PARAMS_KAPPA_BYTES + PARAMS_KAPPA_BYTES, pk, PARAMS_PK_SIZE);

    return 0;
}

// CCA-KEM Encaps()

int r5_cca_kem_encapsulate(uint8_t *ct, uint8_t *k, const uint8_t *pk) {
    uint8_t hash_in[PARAMS_KAPPA_BYTES + (PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES > PARAMS_PK_SIZE ? PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES : PARAMS_PK_SIZE)];
    uint8_t m[PARAMS_KAPPA_BYTES];
    uint8_t L_g_rho[3][PARAMS_KAPPA_BYTES];

    randombytes(m, PARAMS_KAPPA_BYTES); // generate random m

    memcpy(hash_in, m, PARAMS_KAPPA_BYTES); // G: (l | g | rho) = h(m | pk);
    memcpy(hash_in + PARAMS_KAPPA_BYTES, pk, PARAMS_PK_SIZE);
    hash((uint8_t *) L_g_rho, 3 * PARAMS_KAPPA_BYTES, hash_in, PARAMS_KAPPA_BYTES + PARAMS_PK_SIZE, PARAMS_KAPPA_BYTES);

#ifdef NIST_KAT_GENERATION
    print_hex("r5_cca_kem_encapsulate: m", m, PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cca_kem_encapsulate: L", L_g_rho[0], PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cca_kem_encapsulate: g", L_g_rho[1], PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cca_kem_encapsulate: rho", L_g_rho[2], PARAMS_KAPPA_BYTES, 1);
#endif

    /* Encrypt  */
    r5_cpa_pke_encrypt(ct, pk, m, L_g_rho[2]); // m: ct = (U,v)

    /* Append g: ct = (U,v,g) */
    memcpy(ct + PARAMS_CT_SIZE, L_g_rho[1], PARAMS_KAPPA_BYTES);

    /* k = H(L, ct) */
    memcpy(hash_in, L_g_rho[0], PARAMS_KAPPA_BYTES);
    memcpy(hash_in + PARAMS_KAPPA_BYTES,
            ct, PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES);
    hash(k, PARAMS_KAPPA_BYTES, hash_in, PARAMS_KAPPA_BYTES + PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES, PARAMS_KAPPA_BYTES);

    return 0;
}

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

// CCA-KEM Decaps()

int r5_cca_kem_decapsulate(uint8_t *k, const uint8_t *ct, const uint8_t *sk) {
    uint8_t hash_in[PARAMS_KAPPA_BYTES + (PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES > PARAMS_PK_SIZE ? PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES : PARAMS_PK_SIZE)];
    uint8_t m_prime[PARAMS_KAPPA_BYTES];
    uint8_t L_g_rho_prime[3][PARAMS_KAPPA_BYTES];
    uint8_t ct_prime[PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES];
    uint8_t fail;

    r5_cpa_pke_decrypt(m_prime, sk, ct); // r5_cpa_pke_decrypt m'

    memcpy(hash_in, m_prime, PARAMS_KAPPA_BYTES);
    memcpy(hash_in + PARAMS_KAPPA_BYTES, // (L | g | rho) = h(m | pk)
            sk + PARAMS_KAPPA_BYTES + PARAMS_KAPPA_BYTES, PARAMS_PK_SIZE);
    hash((uint8_t *) L_g_rho_prime, 3 * PARAMS_KAPPA_BYTES, hash_in, PARAMS_KAPPA_BYTES + PARAMS_PK_SIZE, PARAMS_KAPPA_BYTES);

#ifdef NIST_KAT_GENERATION
    print_hex("r5_cca_kem_decapsulate: m_prime", m_prime, PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cca_kem_decapsulate: L_prime", L_g_rho_prime[0], PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cca_kem_decapsulate: g_prime", L_g_rho_prime[1], PARAMS_KAPPA_BYTES, 1);
    print_hex("r5_cca_kem_decapsulate: rho_prime", L_g_rho_prime[2], PARAMS_KAPPA_BYTES, 1);
#endif

    // Encrypt m: ct' = (U',v')
    r5_cpa_pke_encrypt(ct_prime, sk + PARAMS_KAPPA_BYTES + PARAMS_KAPPA_BYTES, m_prime, L_g_rho_prime[2]);

    // ct' = (U',v',g')
    memcpy(ct_prime + PARAMS_CT_SIZE, L_g_rho_prime[1], PARAMS_KAPPA_BYTES);

    // k = H(L', ct')
    memcpy(hash_in, L_g_rho_prime[0], PARAMS_KAPPA_BYTES);
    // verification ok ?
    fail = (uint8_t) verify(ct, ct_prime, PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES);
    // k = H(y, ct') depending on fail state
    conditional_constant_time_memcpy(hash_in, sk + PARAMS_KAPPA_BYTES, PARAMS_KAPPA_BYTES, fail);

    memcpy(hash_in + PARAMS_KAPPA_BYTES, ct_prime, PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES);
    hash(k, PARAMS_KAPPA_BYTES, hash_in, PARAMS_KAPPA_BYTES + PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES, PARAMS_KAPPA_BYTES);

    return 0;
}
