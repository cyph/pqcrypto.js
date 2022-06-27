/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Definition of the NIST API Round5 parameter sets and declaration of the
 * (internal) Round5 parameter set variables.
 */

#ifndef R5_PARAMETER_SETS_H
#define R5_PARAMETER_SETS_H

#include <stdint.h>

/* Positions of the NIST API parameters */

/** The location of API parameter `CRYPTO_SECRETKEYBYTES` in the parameter set.  */
#define API_SECRET   0
/** The location of API parameter `CRYPTO_PUBLICKEYBYTES` in the parameter set.  */
#define API_PUBLIC   1
/** The location of API parameter `CRYPTO_BYTES` in the parameter set. */
#define API_BYTES    2
/** The location of API parameter `CRYPTO_CIPHERETEXTBYTES` in the parameter set.  */
#define API_CIPHER   3

/* Positions of our internal parameters */

/** The location of algorithm parameter `kappa_bytes` in the parameter set */
#define POS_KAPPA_BYTES 4
/** The location of algorithm parameter `d` in the parameter set */
#define POS_D           5
/** The location of algorithm parameter `n` in the parameter set */
#define POS_N           6
/** The location of algorithm parameter `h` in the parameter set */
#define POS_H           7
/** The location of algorithm parameter `q_bits` in the parameter set */
#define POS_Q_BITS      8
/** The location of algorithm parameter `p_bits` in the parameter set */
#define POS_P_BITS      9
/** The location of algorithm parameter `t_bits` in the parameter set */
#define POS_T_BITS     10
/** The location of algorithm parameter `b_bits` in the parameter set */
#define POS_B_BITS     11
/** The location of algorithm parameter `n_bar` in the parameter set */
#define POS_N_BAR      12
/** The location of algorithm parameter `m_bar` in the parameter set */
#define POS_M_BAR      13
/** The location of algorithm parameter `f` in the parameter set */
#define POS_F          14
/** The location of algorithm parameter `xe` in the parameter set */
#define POS_XE         15

/* NIST API Round5 parameter set definition */
#ifndef ROUND5_API_TAU
#define ROUND5_API_TAU 2
#endif
#define ROUND5_API_SET 12
#define CRYPTO_SECRETKEYBYTES 16
#define CRYPTO_PUBLICKEYBYTES 5214
#define CRYPTO_BYTES 16
#define CRYPTO_CIPHERTEXTBYTES 5236
#define CRYPTO_ALGNAME "R5N1_1KEM_0d"

/** The Round5 parameter set parameter values. */
const uint32_t r5_parameter_sets[81][16];

/** The names of the Round5 parameter sets. */
const char *r5_parameter_set_names[81];

/* Default ROUND5_API_TAU to 0 if not yet defined */
#if !defined(ROUND5_API_TAU)
/** Defines the variant tau to use for the creation of A. */
#define ROUND5_API_TAU 0
#endif

/* Default ROUND5_API_TAU2_LEN to 2<<11 if not yet defined (or 0) */
#if !defined(ROUND5_API_TAU2_LEN) || ROUND5_API_TAU2_LEN == 0
#undef ROUND5_API_TAU2_LEN
/**
 * Defines the length of the random vector for the A matrix when A is created using TAU=2.
 * The value 0 (the default) means that the value of parameter _2^11_ will be used.
 *
 * <strong>Important: this must be a power of 2 and larger than parameter _d_!</strong>
 */
#define ROUND5_API_TAU2_LEN (1<<11)
#endif
#if ROUND5_API_TAU2_LEN > (1<<31)
#error ROUND5_API_TAU2_LEN must be less than or equal to 2^31
#endif
#if (ROUND5_API_TAU2_LEN & (ROUND5_API_TAU2_LEN - 1)) != 0
#error ROUND5_API_TAU2_LEN must be a power of two
#endif

#endif /* R5_PARAMETER_SETS_H */
