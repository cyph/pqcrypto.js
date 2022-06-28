/** 
 * \file parameters.h
 * \brief Parameters of the ROLLO scheme
 */

#ifndef ROLLO_PARAMETER_H
#define ROLLO_PARAMETER_H

#include "api.h"

#define PARAM_M 83 /**< Parameter m of the scheme (finite field GF(2^m)) */
#define PARAM_N 149 /**< Parameter n of the scheme (code length) */
#define PARAM_D 8 /**< Parameter d of the scheme (weight of vectors) */
#define PARAM_R 5 /**< Parameter r of the scheme (weight of vectors) */
#define PARAM_DFR 128 /**< Decryption Failure Rate (2^-30) */
#define PARAM_SECURITY 128 /**< Expected security level */

#define NMODCOEFFS 5 //Number of "1" coefficients in the polynomial defining the ideal
#define MODCOEFFS {0, 7, 9, 10, 149} //List of "1" coefficients in the polynomial defining the ideal

#define PUBLIC_KEY_BYTES CRYPTO_PUBLICKEYBYTES
#define SECRET_KEY_BYTES CRYPTO_SECRETKEYBYTES
#define CIPHERTEXT_BYTES CRYPTO_CIPHERTEXTBYTES
#define SHARED_SECRET_BYTES CRYPTO_BYTES

#define GF2MBYTES 11 //Number of bytes to store an element of GF2^m
#define FFI_VEC_R_BYTES 52 //Number of bytes to store R elements of GF2^m
#define FFI_VEC_N_BYTES 1546 //Number of bytes to store N elements of GF2^m
#define SHA512_BYTES 64 /**< Size of SHA512 output */

#define SEEDEXPANDER_SEED_BYTES 40 /**< Seed size of the NIST seed expander */
#define SEEDEXPANDER_MAX_LENGTH 4294967295 /**< Max length of the NIST seed expander */

#endif

