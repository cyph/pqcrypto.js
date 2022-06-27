/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the encrypt and decrypt functions based on the CCA KEM
 * algorithm (NIST api).
 */

#ifndef _CCA_ENCRYPT_H_
#define _CCA_ENCRYPT_H_

#include "r5_parameter_sets.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Conditionally provide the PKE NIST API functions.
     */

#if CRYPTO_CIPHERTEXTBYTES == 0

    /**
     * Generates an ENCRYPT key pair. Uses the fixed parameter configuration.
     *
     * @param[out] pk public key
     * @param[out] sk secret key
     * @return __0__ in case of success
     */
    int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk);

    /**
     * Encrypts a message. Uses the fixed parameter configuration.
     *
     * @param[out] ct     the encrypted message
     * @param[out] ct_len the length of the encrypted message (`CRYPTO_CIPHERTEXTBYTES` + `m_len`)
     * @param[in]  m      the message to encrypt
     * @param[in]  m_len  the length of the message to encrypt
     * @param[in]  pk     the public key to use for the encryption
     * @return __0__ in case of success
     */
    int crypto_encrypt(unsigned char *ct, unsigned long long *ct_len, const unsigned char *m, const unsigned long long m_len, const unsigned char *pk);

    /**
     * Decrypts a message. Uses the fixed parameter configuration.
     *
     * @param[out] m      the decrypted message
     * @param[out] m_len  the length of the decrypted message (`ct_len` - `CRYPTO_CIPHERTEXTBYTES`)
     * @param[in]  ct     the message to decrypt
     * @param[in]  ct_len the length of the message to decrypt
     * @param[in]  sk     the secret key to use for the decryption
     * @return __0__ in case of success
     */
    int crypto_encrypt_open(unsigned char *m, unsigned long long *m_len, const unsigned char *ct, const unsigned long long ct_len, const unsigned char *sk);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _CCA_ENCRYPT_H_ */
