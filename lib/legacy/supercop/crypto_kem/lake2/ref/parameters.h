/** 
 * \file parameters.h
 * \brief Parameters of the LAKE scheme
 */

#ifndef LAKE_PARAMETER_H
#define LAKE_PARAMETER_H

#define PARAM_M 89 /**< Parameter m of the scheme (finite field GF(2^m)) */
#define PARAM_N 53 /**< Parameter n of the scheme (code length) */
#define PARAM_D 7 /**< Parameter d of the scheme (weight of vectors) */
#define PARAM_R 6 /**< Parameter r of the scheme (weight of vectors) */
#define PARAM_DFR 32 /**< Decryption Failure Rate (2^-30) */
#define PARAM_SECURITY 192 /**< Expected security level */

#define NMODCOEFFS 3 //Number of "1" coefficients other than X^m + 1 in the polynomial defining the ideal
#define MODCOEFFS {1, 2, 6} //List of "1" coefficients in the polynomial defining the ideal

#define GF2MBYTES 12 //Number of bytes to store an element of GF2^m

#define SHA512_BYTES 64 /**< Size of SHA512 output */

#define PUBLIC_KEY_BYTES CRYPTO_PUBLICKEYBYTES
#define SECRET_KEY_BYTES CRYPTO_SECRETKEYBYTES
#define CIPHERTEXT_BYTES CRYPTO_CIPHERTEXTBYTES
#define SHARED_SECRET_BYTES CRYPTO_BYTES

#define SEEDEXPANDER_SEED_BYTES 40 /**< Seed size of the NIST seed expander */
#define SEEDEXPANDER_MAX_LENGTH 4294967295 /**< Max length of the NIST seed expander */

#endif

