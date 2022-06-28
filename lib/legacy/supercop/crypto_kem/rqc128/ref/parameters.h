/** 
 * \file parameters.h
 * \brief Parameters of the RQC_KEM IND-CCA2 scheme
 */

#include "api.h"

#ifndef RQC_PARAMETER_H
#define RQC_PARAMETER_H


#define PARAM_Q 2 /**< Parameter q of the scheme (finite field GF(q^m)) */
#define PARAM_M 97 /**< Parameter m of the scheme (finite field GF(q^m)) */
#define PARAM_K 4 /**< Parameter k of the scheme (code dimension) */
#define PARAM_N 67 /**< Parameter n of the scheme (code length) */
#define PARAM_W 5 /**< Parameter omega of the scheme (weight of vectors) */
#define PARAM_W_R 6 /**< Parameter omega_r of the scheme (weight of vectors) */
#define PARAM_SECURITY 128 /**< Expected security level */

#define SECRET_KEY_BYTES CRYPTO_SECRETKEYBYTES /**< Secret key size */
#define PUBLIC_KEY_BYTES CRYPTO_PUBLICKEYBYTES /**< Public key size */
#define SHARED_SECRET_BYTES CRYPTO_BYTES /**< Shared secret size */
#define CIPHERTEXT_BYTES CRYPTO_CIPHERTEXTBYTES /**< Ciphertext size */

#define VEC_K_BYTES 49 /**< Number of bytes required to store a vector of size k */
#define VEC_N_BYTES 813 /**< Number of bytes required to store a vector of size n */
#define FFI_ELT_BYTES 13 /**< Number of bytes required to store an element of GF(q^m) */
#define FFI_VEC_K_BYTES PARAM_K * FFI_ELT_BYTES /**< Number of bytes required to store a vector of size k using the NTL-based ffi_vec implementation */
#define FFI_VEC_N_BYTES PARAM_N * FFI_ELT_BYTES /**< Number of bytes required to store a vector of size n using the NTL-based ffi_vec implementation */

#define SHA512_BYTES 64 /**< Size of SHA512 output */

#define SEEDEXPANDER_SEED_BYTES 40 /**< Seed size of the NIST seed expander */
#define SEEDEXPANDER_MAX_LENGTH 4294967295 /**< Max length of the NIST seed expander */

#endif

