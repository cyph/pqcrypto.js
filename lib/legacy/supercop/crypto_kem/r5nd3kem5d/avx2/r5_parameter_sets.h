/*
 * Copyright (c) 2018, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

#ifndef _R5_PARAMETER_SETS_H_
#define _R5_PARAMETER_SETS_H_

#include <stdint.h>
#include <stddef.h>
#include "misc.h"

// Parameter Set definitions

/* NIST API Round5 parameter set definition */
#define AVX2
#define PARAMS_KAPPA_BYTES 24
#define PARAMS_D           756
#define PARAMS_N           756
#define PARAMS_H           242
#define PARAMS_Q_BITS      12
#define PARAMS_P_BITS      8
#define PARAMS_T_BITS      2
#define PARAMS_B_BITS      1
#define PARAMS_N_BAR       1
#define PARAMS_M_BAR       1
#define PARAMS_F           5
#define PARAMS_XE          218
#define CRYPTO_ALGNAME     "R5ND_3KEM_5d"

// appropriate types
typedef uint16_t modq_t;
#if (PARAMS_P_BITS <= 8)
typedef uint8_t modp_t;
#else
typedef uint16_t modp_t;
#endif
typedef uint8_t modt_t;

#define PARAMS_ND       PARAMS_D
#define PARAMS_K        (PARAMS_D/PARAMS_N)
#define PARAMS_Q        (1 << PARAMS_Q_BITS)
#define PARAMS_Q_MASK   (PARAMS_Q - 1)
#define PARAMS_P        (1 << PARAMS_P_BITS)
#define PARAMS_P_MASK   (PARAMS_P - 1)
#define PARAMS_KAPPA    (8 * PARAMS_KAPPA_BYTES)
#define PARAMS_MU       CEIL_DIV((PARAMS_KAPPA + PARAMS_XE), PARAMS_B_BITS)
#define PARAMS_MUT_SIZE BITS_TO_BYTES(PARAMS_MU * PARAMS_T_BITS)
#if PARAMS_K == 1
#define PARAMS_RS_DIV   (0x10000 / PARAMS_ND)
#define PARAMS_RS_LIM   (PARAMS_ND * PARAMS_RS_DIV)
#define PARAMS_NDP_SIZE BITS_TO_BYTES(PARAMS_ND * PARAMS_P_BITS)
#else
#define PARAMS_RS_DIV   (0x10000 / PARAMS_D)
#define PARAMS_RS_LIM   (PARAMS_D * PARAMS_RS_DIV)
#define PARAMS_DP_SIZE  BITS_TO_BYTES(PARAMS_N_BAR * PARAMS_D * PARAMS_P_BITS)
#define PARAMS_DPU_SIZE BITS_TO_BYTES(PARAMS_M_BAR * PARAMS_D * PARAMS_P_BITS)
#endif

// Definition of TAU parameter, defaults to 0 unless otherwise defined and non-ring
#if PARAMS_K == 1 || !defined(ROUND5_API_TAU)
#undef ROUND5_API_TAU
#define ROUND5_API_TAU 0
#endif
#define PARAMS_TAU      ROUND5_API_TAU

// Define the length of the random vector when TAU is 2 is used for generating A, defaults to parameter 2^11.
// Important: Must be a power of two and > d
#if !defined(ROUND5_API_TAU2_LEN) || ROUND5_API_TAU2_LEN == 0
#undef ROUND5_API_TAU2_LEN
#define ROUND5_API_TAU2_LEN (1<<11)
#endif
#if ROUND5_API_TAU2_LEN > (1<<31)
#error ROUND5_API_TAU2_LEN must be less than or equal to 2^31
#endif
#if (ROUND5_API_TAU2_LEN & (ROUND5_API_TAU2_LEN - 1)) != 0 || ROUND5_API_TAU2_LEN < PARAMS_D
#error ROUND5_API_TAU2_LEN must be a power of two and greater than or equal to PARAMS_D
#endif
#define PARAMS_TAU2_LEN ROUND5_API_TAU2_LEN

// Rounding constants
#if ((PARAMS_Q_BITS - PARAMS_P_BITS + PARAMS_T_BITS) < PARAMS_P_BITS)
#define PARAMS_Z_BITS   PARAMS_P_BITS
#else
#define PARAMS_Z_BITS   (PARAMS_Q_BITS - PARAMS_P_BITS + PARAMS_T_BITS)
#endif
#define PARAMS_H1       (1 << (PARAMS_Q_BITS - PARAMS_P_BITS - 1))
#define PARAMS_H2       (1 << (PARAMS_Q_BITS - PARAMS_Z_BITS - 1))
#define PARAMS_H3       ((1 << (PARAMS_P_BITS - PARAMS_T_BITS - 1)) + (1 << (PARAMS_P_BITS - PARAMS_B_BITS - 1)) - (1 << (PARAMS_Q_BITS - PARAMS_Z_BITS - 1)))

#if PARAMS_K == 1
#define PARAMS_PK_SIZE  (PARAMS_KAPPA_BYTES + PARAMS_NDP_SIZE)
#define PARAMS_CT_SIZE  (PARAMS_NDP_SIZE + PARAMS_MUT_SIZE)
#else
#define PARAMS_PK_SIZE  (PARAMS_KAPPA_BYTES + PARAMS_DP_SIZE)
#define PARAMS_CT_SIZE  (PARAMS_DPU_SIZE + PARAMS_MUT_SIZE)

// Packing shift
#if PARAMS_B_BITS == 1
#define PACK_SHIFT 3
#define PACK_AND 7
#endif
#if PARAMS_B_BITS == 2
#define PACK_SHIFT 2
#define PACK_AND 3
#endif
#if PARAMS_B_BITS == 4
#define PACK_SHIFT 1
#define PACK_AND 1
#endif

#endif

// Derive the NIST parameters
#ifdef ROUND5_CCA_PKE

// CCA_PKE Variant
#define CRYPTO_SECRETKEYBYTES  (PARAMS_KAPPA_BYTES + PARAMS_KAPPA_BYTES + PARAMS_PK_SIZE)
#define CRYPTO_PUBLICKEYBYTES  PARAMS_PK_SIZE
#define CRYPTO_BYTES           (PARAMS_CT_SIZE + PARAMS_KAPPA_BYTES + 16)
#define CRYPTO_CIPHERTEXTBYTES 0

#else

// CPA_KEM Variant
#define CRYPTO_SECRETKEYBYTES  PARAMS_KAPPA_BYTES
#define CRYPTO_PUBLICKEYBYTES  PARAMS_PK_SIZE
#define CRYPTO_BYTES           PARAMS_KAPPA_BYTES
#define CRYPTO_CIPHERTEXTBYTES PARAMS_CT_SIZE

#endif /* ROUND5_CCA_PKE */

// AVX2 implies CM_CACHE
#ifdef AVX2
#undef CM_CACHE
#define CM_CACHE
// Disable AVX2 if not supported by platform
#ifndef __AVX2__
#warning AVX2 not supported by platform
#undef AVX2
#endif
#endif

#endif /* _R5_PARAMETER_SETS_H_ */
