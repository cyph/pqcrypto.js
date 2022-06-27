/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the encrypt and decrypt functions based on the CCA KEM
 * algorithm.
 */

#ifndef R5_CCA_PKE_H
#define R5_CCA_PKE_H

#include "parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Generates an ENCRYPT key pair. Uses the parameters as specified.
     *
     * @param[out] pk     public key
     * @param[out] sk     secret key (<b>important:</b> the size of `sk` is `sk_size` + `kappa_bytes` + `pk_size`!)
     * @param[in]  params the algorithm parameters to use
     * @return __0__ in case of success
     */
    int r5_cca_pke_keygen(unsigned char *pk, unsigned char *sk, const parameters *params);

    /**
     * Encrypts a message. Uses the parameters as specified.
     *
     * @param[out] ct     the encrypted message
     * @param[out] ct_len the length of the encrypted message (`m_len` + `ct_size` + `kappa_bytes` + 16)
     * @param[in]  m      the message to encrypt
     * @param[in]  m_len  the length of the message to encrypt
     * @param[in]  pk     the public key to use for the encryption
     * @param[in]  params the algorithm parameters to use
     * @return __0__ in case of success
     */
    int r5_cca_pke_encrypt(unsigned char *ct, unsigned long long *ct_len, const unsigned char *m, const unsigned long long m_len, const unsigned char *pk, const parameters *params);

    /**
     * Decrypts a message. Uses the parameters as specified.
     *
     * @param[out] m       the decrypted message
     * @param[out] m_len   the length of the decrypted message (`ct_len` - `ct_size` - `kappa_bytes` - 16)
     * @param[in]  ct      the message to decrypt
     * @param[in]  ct_len  the length of the message to decrypt
     * @param[in]  sk      the secret key to use for the decryption
     * @param[in]  params  the algorithm parameters to use
     * @return __0__ in case of success
     */
    int r5_cca_pke_decrypt(unsigned char *m, unsigned long long *m_len, const unsigned char *ct, const unsigned long long ct_len, const unsigned char *sk, const parameters *params);

#ifdef __cplusplus
}
#endif

#endif /* R5_CCA_PKE_H */
