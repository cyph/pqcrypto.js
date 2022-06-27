/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the algorithm parameters, structure, and functions.
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

#include "r5_parameter_sets.h"

/**
 * Algorithm parameters.
 */
typedef struct {
    /** @name Main Parameters
     *
     * These parameters define the working of the algorithm.
     * For a reference of their meaning, please see the algorithm
     * documentation.
     *@{*/
    uint8_t tau; /**< The variant for creating A __τ__ */
    uint32_t tau2_len; /**< The length of the vector when generating A with _τ=2_ */
    uint16_t d; /**< Dimension parameter __d__ */
    uint16_t n; /**< Dimension parameter __n__ */
    uint16_t h; /**< Hamming weight parameter __h__ */
    uint8_t q_bits; /**< The number of __q__ bits */
    uint8_t p_bits; /**< Number of __p__ bits */
    uint8_t t_bits; /**< Number of __t__ bits */
    uint16_t n_bar; /**< Dimension parameter __n̅__ */
    uint16_t m_bar; /**< Dimension parameter __m̅__ */
    uint8_t b_bits; /**< Number of extracted bits per ciphertext symbol (parameter __b__ in bits) */
    uint8_t kappa_bytes; /**< The size of shared secret, in bytes, also used for the size of seeds */
    uint8_t f; /**< Number of bit errors corrected, parameter __f__ */
    uint8_t xe; /**< Number of bits for error correction */
    /**@}*/

    /** @name Derived Parameters
     *
     * These parameters can be derived from the main parameters.
     *@{*/
    uint16_t kappa; /**< Parameter __kappa__, the size of shared secret, in bits, also used for the size of seeds */
    uint16_t k; /**< Dimension parameter __k__ = _d/n_ */
    uint32_t q; /**< Parameter __q__ */
    uint16_t p; /**< Parameter __p__ = _2<sup>p<sub>bits</sub></sup>_ */
    uint16_t mu; /**< Parameter __mu__ = _(kappa + xe) / B_ */
    /**@}*/

    /** @name Rounding Constants.
     *
     * Constants used for rounding.
     *@{*/
    uint16_t z_bits; /**< Rounding constant __z__ */
    uint16_t h1; /**< Rounding constant __h1__ */
    uint16_t h2; /**< Rounding constant __h2__ */
    uint16_t h3; /**< Rounding constant __h3__ */
    /**@}*/

    /** @name Derived NIST Parameters
     *
     * These parameters can be derived from the main parameters, or,
     * when using the NIST API macro definitions, the other way around.
     *
     * Note that for the ENCRYPT (CCA KEM) type functions, the total size of
     * the “secret key” is actually `sk_size` + `kappa_bytes` + `pk_size`, and
     * the total size of the “cipher text” is actually `ct_size` +
     * `kappa_bytes`. The sizes of the API parameters already include the
     * additional bytes so these _are_ the actual sizes.
     *@{*/
    uint32_t pk_size; /**< Size of the public key, in bytes */
    uint16_t ct_size; /**< Size of the cipher text, in bytes */
    /**@}*/
} parameters;

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Returns the size of the secret key (NIST API parameter `CRYPTO_SECRETKEYBYTES`).
     *
     * @param[in] params the algorithm parameters
     * @param[in] is_cca_or_encrypt flag indicating whether or not the size is for the CCA KEM/ENCRYPT protocol
     * @return the size of the secret key (i.e. `CRYPTO_SECRETKEYBYTES`)
     */
    uint32_t get_crypto_secret_key_bytes(const parameters *params, const int is_cca_or_encrypt);

    /**
     * Returns the size of the public key (NIST API parameter `CRYPTO_PUBLICKEYBYTES`).
     *
     * @param[in] params the algorithm parameters
     * @return the size of the public key (i.e. `CRYPTO_PUBLICKEYBYTES`)
     */
    uint32_t get_crypto_public_key_bytes(const parameters *params);

    /**
     * Returns the size of the shared secret in case of KEM or the message overhead in case of ENCRYPT (NIST API parameter `CRYPTO_BYTES`).
     *
     * @param[in] params the algorithm parameters
     * @param[in] is_encrypt flag indicating whether or not the size is for the ENCRYPT protocol
     * @return the size of the shared secret/message overhead key (i.e. `CRYPTO_BYTES`)
     */
    uint16_t get_crypto_bytes(const parameters *params, const int is_encrypt);

    /**
     * Returns the size of the cipher text in case of KEM, 0 in case of ENCRYPT (NIST API parameter `CRYPTO_CIPHERTEXTBYTES`).
     *
     * @param[in] params the algorithm parameters
     * @param[in] is_cca flag indicating whether the size is for the CCA KEM or CPA KEM protocol
     * @param[in] is_encrypt flag indicating whether or not the size is for the ENCRYPT protocol
     * @return the size of the cipher text or 0 (i.e. `CRYPTO_CIPHERTEXTBYTES`)
     */
    uint16_t get_crypto_cipher_text_bytes(const parameters *params, const int is_cca, const int is_encrypt);

    /**
     * Returns the size of the seeds used in the algorithm implementation.
     *
     * @param[in] params the algorithm parameters
     * @return the size of the seeds
     */
    uint16_t get_crypto_seed_bytes(const parameters *params);

    /**
     * Sets the algorithm parameters according to the values from the (NIST) API setting macros in `api.h`.
     *
     * @return (pointer to) the API parameters, __NULL__ in case of error
     */
    parameters *set_parameters_from_api();

    /**
     * Set the algorithm parameters as specified.
     *
     * @param[out] params the algorithm parameters set up as specified
     * @param[in] tau the variant for creating A __τ__
     * @param[in] tau2_len the length of the vector when generating A with _τ=2_ (must be 0 (for _q_) or a power of 2 and larger or equal to _d_)
     * @param[in] kappa_bytes parameter __kappa__ (in bytes)
     * @param[in] d dimension parameter __d__
     * @param[in] n dimension parameter __n__
     * @param[in] h hamming weight parameter __h__
     * @param[in] q_bits the number of bits of parameter __q__, __q__ is defined as _2<sup>q<sub>bits</sub></sup>_
     * @param[in] p_bits the number of bits of parameter __p__, __p__ is defined as _2<sup>p<sub>bits</sub></sup>_
     * @param[in] t_bits number of bits per element in the ciphertext
     * @param[in] n_bar dimension parameter __n̅__
     * @param[in] m_bar dimension parameter __m̅__
     * @param[in] b_bits the number of bits of parameter __b__, __b__ is defined as _2<sup>b<sub>bits</sub></sup>_)
     * @param[in] f number of bit-errors corrected, parameter __f__ (0 to 5)
     * @param[in] xe number of error correction bits, parameter __xe__
     *
     * @return __0__ if successful, error code otherwise
     */
    int set_parameters(parameters *params, const uint8_t tau, const uint32_t tau2_len, const uint8_t kappa_bytes, const uint16_t d, const uint16_t n, const uint16_t h, const uint8_t q_bits, const uint8_t p_bits, const uint8_t t_bits, const uint8_t b_bits, const uint16_t n_bar, const uint16_t m_bar, const uint8_t f, const uint8_t xe);

    /**
     * Updates the algorithm parameter __τ__ as specified. Note: in case of
     * ring parameters, τ is forced to 0.
     *
     * @param[in,out] params the algorithm parameters updated as specified
     * @param[in] tau the variant for creating A __τ__
     */
    void set_parameter_tau(parameters *params, const uint8_t tau);

    /**
     * Updates the length of the vector to use when generating matrix A with _τ=2_
     * (only applicable to non-ring parameters). If the value 0 is specified,
     * the value of _1<<11_ is used instead.
     *
     * @param[in,out] params the algorithm parameters updated as specified
     * @param[in] tau2_len the length of the vector when generating A with _τ=2_ (must be 0 (for _2^11_) or a power of 2 and larger or equal to _d_)
     */
    void set_parameter_tau2_len(parameters *params, const uint32_t tau2_len);

#ifdef __cplusplus
}
#endif

#endif /* PARAMETERS_H */
