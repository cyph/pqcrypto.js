/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the CPA KEM functions (NIST api).
 */

#ifndef _CPA_KEM_H_
#define _CPA_KEM_H_

#include "r5_parameter_sets.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Conditionally provide the KEM NIST API functions.
     */

#if CRYPTO_CIPHERTEXTBYTES != 0

    /**
     * Generates a CPA KEM key pair. Uses the fixed parameter configuration.
     *
     * @param[out] pk public key
     * @param[out] sk secret key
     * @return __0__ in case of success
     */
    int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);

    /**
     * CPA KEM encapsulate. Uses the fixed parameter configuration.
     *
     * @param[out] ct    key encapsulation message (ciphertext)
     * @param[out] k     shared secret
     * @param[in]  pk    public key with which the message is encapsulated
     * @return __0__ in case of success
     */
    int crypto_kem_enc(unsigned char *ct, unsigned char *k, const unsigned char *pk);

    /**
     * CPA KEM de-capsulate. Uses the fixed parameter configuration.
     *
     * @param[out] k     shared secret
     * @param[in]  ct    key encapsulation message (ciphertext)
     * @param[in]  sk    secret key with which the message is to be de-capsulated
     * @return __0__ in case of success
     */
    int crypto_kem_dec(unsigned char *k, const unsigned char *ct, const unsigned char *sk);

#endif

#ifdef __cplusplus
}
#endif

#endif /* CPA_KEM_H */
