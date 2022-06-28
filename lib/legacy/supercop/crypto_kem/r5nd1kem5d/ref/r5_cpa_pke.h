/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the encryption functions used within the implementation.
 */

#ifndef PST_ENCRYPT_H
#define PST_ENCRYPT_H

#include "parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Generates a key pair. Uses the parameters as specified.
     *
     * @param[out] pk     public key
     * @param[out] sk     secret key
     * @param[in]  params the algorithm parameters to use
     * @return __0__ in case of success
     */
    int r5_cpa_pke_keygen(unsigned char *pk, unsigned char *sk, const parameters *params);

    /**
     * Encrypts a plaintext using the provided seed for R.
     *
     * @param[out] ct     ciphertext
     * @param[in]  pk     public key with which the message is encrypted
     * @param[in]  m      plaintext
     * @param[in]  rho    seed of R
     * @param[in]  params the algorithm parameters to use
     * @return __0__ in case of success
     */
    int r5_cpa_pke_encrypt(unsigned char *ct, const unsigned char *pk, const unsigned char *m, const unsigned char *rho, const parameters *params);

    /**
     * Decrypts a ciphertext.
     *
     * @param[out] m     plaintext
     * @param[in]  ct     ciphertext
     * @param[in]  sk    secret key with which the message is decrypted
     * @param[in]  params the algorithm parameters to use
     * @return __0__ in case of success
     */
    int r5_cpa_pke_decrypt(unsigned char *m, const unsigned char *sk, const unsigned char *ct, const parameters *params);

#ifdef __cplusplus
}
#endif

#endif /* PST_ENCRYPT_H */
