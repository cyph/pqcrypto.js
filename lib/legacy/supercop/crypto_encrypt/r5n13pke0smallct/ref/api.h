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

#define CRYPTO_SECRETKEYBYTES 163584
#define CRYPTO_PUBLICKEYBYTES 163536
#define CRYPTO_BYTES 988
#define CRYPTO_CIPHERTEXTBYTES 0
#define CRYPTO_ALGNAME "R5N1_3PKE_0smallCT"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Generates an ENCRYPT key pair. Uses the fixed parameter configuration.
     *
     * @param[out] pk public key
     * @param[out] sk secret key
     * @return __0__ in case of success
     */
    int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk);

    /**
     * Encrypts a message.
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
     * Decrypts a message.
     *
     * @param[out] m      the decrypted message
     * @param[out] m_len  the length of the decrypted message (`ct_len` - `CRYPTO_CIPHERTEXTBYTES`)
     * @param[in]  ct     the message to decrypt
     * @param[in]  ct_len the length of the message to decrypt
     * @param[in]  sk     the secret key to use for the decryption
     * @return __0__ in case of success
     */
    int crypto_encrypt_open(unsigned char *m, unsigned long long *m_len, const unsigned char *ct, const unsigned long long ct_len, const unsigned char *sk);

#ifdef __cplusplus
}
#endif

#endif /* _API_H_ */
