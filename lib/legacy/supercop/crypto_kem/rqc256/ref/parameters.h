/** 
 * \file parameters.h
 * \brief Parameters of the RQC_KEM IND-CCA2 scheme
 */

#ifndef RQC_PARAMETER_H
#define RQC_PARAMETER_H

#define RQC_SECRET_KEY_BYTES 4130 /**< Secret key size */
#define RQC_PUBLIC_KEY_BYTES 4090 /**< Public key size */
#define RQC_SHARED_SECRET_BYTES 64 /**< Shared secret size */
#define RQC_CIPHERTEXT_BYTES 8164 /**< Ciphertext size */

#define RQC_PARAM_Q 2 /**< Parameter q of the scheme (finite field GF(q^m)) */
#define RQC_PARAM_M 181 /**< Parameter m of the scheme (finite field GF(q^m)) */
#define RQC_PARAM_K 3 /**< Parameter k of the scheme (code dimension) */
#define RQC_PARAM_N 179 /**< Parameter n of the scheme (code length) */
#define RQC_PARAM_W 9 /**< Parameter omega of the scheme (weight of vectors) */
#define RQC_PARAM_W_R 9 /**< Parameter omega_r of the scheme (weight of vectors) */
#define RQC_PARAM_W_E 16 /**< Parameter omega_r + delta of the scheme (weight of vectors) */
#define RQC_SECURITY 256 /**< Expected security level */

#define RQC_VEC_K_BYTES 68 /**< Number of bytes required to store a vector of size k */
#define RQC_VEC_N_BYTES 4050 /**< Number of bytes required to store a vector of size n */
#define SHA512_BYTES 64 /**< Size of SHA2_512 and SHA3_512 outputs */

#define SEEDEXPANDER_SEED_BYTES 40 /**< Seed size of the NIST seed expander */
#define SEEDEXPANDER_MAX_LENGTH 4294967295 /**< Max length of the NIST seed expander */

#endif

