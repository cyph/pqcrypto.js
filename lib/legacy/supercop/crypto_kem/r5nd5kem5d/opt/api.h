/*
 * Copyright (c) 2019, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the NIST API functions and setting of the NIST API
 * algorithm parameters: `CRYPTO_SECRETKEYBYTES`, `CRYPTO_PUBLICKEYBYTES`,
 * `CRYPTO_BYTES`, and `CRYPTO_CIPHERBYTES`.
 */

#ifndef _API_H_
#define _API_H_

#define CRYPTO_SECRETKEYBYTES 32
#define CRYPTO_PUBLICKEYBYTES 972
#define CRYPTO_BYTES 32
#define CRYPTO_CIPHERTEXTBYTES 1063
#define CRYPTO_ALGNAME "R5ND_5KEM_5d"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Generates a CPA KEM key pair.
     *
     * @param[out] pk public key
     * @param[out] sk secret key
     * @return __0__ in case of success
     */
    int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);

    /**
     * CPA KEM encapsulate.
     *
     * @param[out] ct    key encapsulation message (ciphertext)
     * @param[out] k     shared secret
     * @param[in]  pk    public key with which the message is encapsulated
     * @return __0__ in case of success
     */
    int crypto_kem_enc(unsigned char *ct, unsigned char *k, const unsigned char *pk);

    /**
     * CPA KEM de-capsulate.
     *
     * @param[out] k     shared secret
     * @param[in]  ct    key encapsulation message (ciphertext)
     * @param[in]  sk    secret key with which the message is to be de-capsulated
     * @return __0__ in case of success
     */
    int crypto_kem_dec(unsigned char *k, const unsigned char *ct, const unsigned char *sk);

#ifdef __cplusplus
}
#endif

#endif /* _API_H_ */
