#include "crypto_kem.h"
/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the CPA KEM functions.
 */

#include "cpa_kem.h"
#include "r5_cpa_kem.h"

#include <stdlib.h>

/*******************************************************************************
 * Public functions
 ******************************************************************************/

#if CRYPTO_CIPHERTEXTBYTES != 0

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk) {
    parameters * params;
    if ((params = set_parameters_from_api()) == NULL) {
        abort();
    }
    return r5_cpa_kem_keygen(pk, sk, params);
}

int crypto_kem_enc(unsigned char *ct, unsigned char *k, const unsigned char *pk) {
    parameters * params;
    if ((params = set_parameters_from_api()) == NULL) {
        abort();
    }
    return r5_cpa_kem_encapsulate(ct, k, pk, params);
}

int crypto_kem_dec(unsigned char *k, const unsigned char *ct, const unsigned char *sk) {
    parameters * params;
    if ((params = set_parameters_from_api()) == NULL) {
        abort();
    }
    return r5_cpa_kem_decapsulate(k, ct, sk, params);
}

#endif /* CRYPTO_CIPHERTEXTBYTES */
