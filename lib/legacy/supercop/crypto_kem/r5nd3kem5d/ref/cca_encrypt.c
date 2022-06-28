/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the encrypt and decrypt functions based on the CCA KEM
 * algorithm (NIST api).
 */

#include "cca_encrypt.h"
#include "r5_cca_pke.h"

#include <stdlib.h>

/*******************************************************************************
 * Public functions
 ******************************************************************************/

#if CRYPTO_CIPHERTEXTBYTES == 0

int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk) {
    parameters * params;
    if ((params = set_parameters_from_api()) == NULL) {
        abort();
    }
    return r5_cca_pke_keygen(pk, sk, params);
}

int crypto_encrypt(unsigned char *ct, unsigned long long *ct_len, const unsigned char *m, const unsigned long long m_len, const unsigned char *pk) {
    parameters * params;
    if ((params = set_parameters_from_api()) == NULL) {
        abort();
    }
    return r5_cca_pke_encrypt(ct, ct_len, m, m_len, pk, params);
}

int crypto_encrypt_open(unsigned char *m, unsigned long long *m_len, const unsigned char *ct, const unsigned long long ct_len, const unsigned char *sk) {
    parameters * params;
    if ((params = set_parameters_from_api()) == NULL) {
        abort();
    }
    return r5_cca_pke_decrypt(m, m_len, ct, ct_len, sk, params);
}

#endif /* CRYPTO_CIPHERTEXTBYTES */
