/** 
 * \file parameters.h
 * \brief Parameters of the ROLLO scheme
 */

#ifndef ROLLO_PARAMETER_H
#define ROLLO_PARAMETER_H

#include "api.h"

#define PARAM_M 101 /**< Parameter m of the scheme (finite field GF(q^m)) */
#define PARAM_N 47 /**< Parameter n of the scheme (code length) */
#define PARAM_W 5 /**< Parameter omega of the scheme (weight of vectors) */
#define PARAM_W_R 6 /**< Parameter omega_r of the scheme (weight of vectors) */
#define PARAM_DFR 36 /**< Decryption Failure Rate (2^-36) */
#define PARAM_SECURITY 128 /**< Expected security level */

#define NMODCOEFFS 3 //Number of "1" coefficients in the polynomial defining the ideal
#define MODCOEFFS {0, 5, 47} //List of "1" coefficients in the polynomial defining the ideal

#define SECRET_KEY_BYTES CRYPTO_SECRETKEYBYTES /**< Secret key size */
#define PUBLIC_KEY_BYTES CRYPTO_PUBLICKEYBYTES /**< Public key size */
#define SHARED_SECRET_BYTES CRYPTO_BYTES /**< Shared secret size */
#define CIPHERTEXT_BYTES CRYPTO_CIPHERTEXTBYTES /**< Ciphertext size */

#define GF2MBYTES 13 //Number of bytes to store an element of GF2^m
#define FFI_VEC_R_BYTES 76 /**< Number of bytes required to store an NTL vector of size r */
#define FFI_VEC_N_BYTES 594 /**< Number of bytes required to store an NTL vector of size n */
#define SHA512_BYTES 64 /**< Size of SHA512 output */

#define SEEDEXPANDER_SEED_BYTES 40 /**< Seed size of the NIST seed expander */
#define SEEDEXPANDER_MAX_LENGTH 4294967295 /**< Max length of the NIST seed expander */

#endif

