/**
 *  api.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: AVX2
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTS_KEM_API_H
#define __NTS_KEM_API_H

#define CRYPTO_BYTES            32
#define CRYPTO_ALGNAME          "NTS-KEM(12, 64)"

#define CRYPTO_SECRETKEYBYTES   9216
#define CRYPTO_PUBLICKEYBYTES   319488
#define CRYPTO_CIPHERTEXTBYTES  128

/**
 *  Generate a key-pair
 *
 *  @note
 *  The size of the public and private key buffers are 
 *  given by CRYPTO_PUBLICKEYBYTES and CRYPTO_SECRETKEYBYTES
 *  respectively. It is assumed that the caller has allocated
 *  enough storage for both `pk` and `sk`.
 *
 *  @param[out] pk  The pointer to the public key
 *  @param[out] sk  The pointer to the private key
 *  @return NTS_KEM_SUCCESS on success, otherwise a negative error code
 *          {@see nts_kem_errors.h}
 **/
int crypto_kem_keypair(unsigned char* pk,
                       unsigned char* sk);

/**
 *  Encapsulate a shared-secret (`ss`) that is randomly generated
 *  to a ciphertext (`ct`)
 *
 *  @note
 *  The size of the shared-secret and ciphertext buffers are
 *  given by CRYPTO_CIPHERTEXTBYTES and CRYPTO_BYTES respectively.
 *  It is assumed that the caller has allocated enough storage 
 *  for both `ss` and `ct`.
 *
 *  @param[out] ct  The pointer to the ciphertext
 *  @param[out] ss  The pointer to the shared secret
 *  @param[in]  pk  The pointer to the public key
 *  @return NTS_KEM_SUCCESS on success, otherwise a negative error code
 *          {@see nts_kem_errors.h}
 **/
int crypto_kem_enc(unsigned char *ct,
                   unsigned char *ss,
                   const unsigned char *pk);

/**
 *  Decapsulate a piece of ciphertext (`ct`) to produce a
 *  piece of shared secret (`ss`)
 *
 *  @note
 *  The size of the shared-secret and ciphertext buffers are
 *  given by CRYPTO_CIPHERTEXTBYTES and CRYPTO_BYTES respectively.
 *  It is assumed that the caller has allocated enough storage
 *  for `ss`.
 *
 *  @param[out] ss  The pointer to the shared secret
 *  @param[in]  ct  The pointer to the ciphertext
 *  @param[in]  sk  The pointer to the public key
 *  @return NTS_KEM_SUCCESS on success, NTS_KEM_INVALID_CIPHERTEXT 
 *          if the ciphertext (`ct`) is invalid, or a negative code
 *          for other errors {@see nts_kem_errors.h}
 **/
int crypto_kem_dec(unsigned char *ss,
                   const unsigned char *ct,
                   const unsigned char *sk);

#endif /* __NTS_KEM_API_H */
