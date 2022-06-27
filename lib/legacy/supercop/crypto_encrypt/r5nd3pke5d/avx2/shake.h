/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the SHAKE128, SHAKE256, cSHAKE128, and cSHAKE256 hash
 * functions.
 *
 * Note: all sizes are in bytes, not bits!
 */

#ifndef _SHAKE_H_
#define _SHAKE_H_

#include "misc.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <libkeccak.a.headers/KeccakHash.h>
#include <libkeccak.a.headers/SP800-185.h>

#ifdef DOXYGEN
/* Document SHAKE implementation option macro */
/**
 * The default implementation of the SHAKE algorithm when combining the absorb
 * and squeeze steps (i.e. when using the functions `shake128()` or `shake256()`)
 * is to make use of OpenSSL (if OpenSSL >= 1.1.1 is found). To use the
 * `libkeccak` implementation in this case too, define `NO_OPENSSL_SHAKE`.
 */
#define NO_OPENSSL_SHAKE
#endif

typedef Keccak_HashInstance shake_ctx; /**< The shake context (state) */
typedef cSHAKE_Instance cshake_ctx; /**< The cshake context (state) */

/**
 * The rate of the SHAKE-128 algorithm (i.e. internal block size, in bytes).
 */
#define SHAKE128_RATE 168

/**
 * The rate of the SHAKE-256 algorithm (i.e. internal block size, in bytes).
 */
#define SHAKE256_RATE 136

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Performs the initialisation step of the SHAKE-128 XOF.
     *
     * @param ctx the shake context
     */
    inline void shake128_init(shake_ctx *ctx) {
        if (Keccak_HashInitialize_SHAKE128(ctx) != 0) {
            abort();
        }
    }

    /**
     * Performs the absorb step of the SHAKE-128 XOF.
     *
     * @param ctx the shake context
     * @param input the input absorbed into the state
     * @param input_len the length of the input
     */
    inline void shake128_absorb(shake_ctx *ctx, const unsigned char *input, const size_t input_len) {
        if (Keccak_HashUpdate(ctx, input, input_len * 8) != 0) {
            abort();
        }
        if (Keccak_HashFinal(ctx, NULL) != 0) {
            abort();
        }
    }

    /**
     * Performs the squeeze step of the SHAKE-128 XOF. Squeezes full blocks of
     * SHAKE128_RATE bytes each. Can be called multiple times to keep squeezing
     * (i.e. this function is incremental).
     *
     * @param ctx the shake context
     * @param output the output
     * @param nr_blocks the number of blocks to squeeze
     */
    inline void shake128_squeezeblocks(shake_ctx *ctx, unsigned char *output, const size_t nr_blocks) {
        if (Keccak_HashSqueeze(ctx, output, nr_blocks * SHAKE128_RATE * 8) != 0) {
            abort();
        }
    }

    /**
     * Performs the full SHAKE-128 XOF to the given input.
     * @param output the final output
     * @param output_len the length of the output
     * @param input the input
     * @param input_len the length of the input
     */
    void shake128(unsigned char *output, size_t output_len, const unsigned char *input, const size_t input_len);

    /**
     * Performs the initialisation step of the SHAKE-256 XOF.
     *
     * @param ctx the shake context
     */
    inline void shake256_init(shake_ctx *ctx) {
        if (Keccak_HashInitialize_SHAKE256(ctx) != 0) {
            abort();
        }
    }

    /**
     * Performs the absorb step of the SHAKE-256 XOF.
     *
     * @param ctx the shake context
     * @param input the input absorbed into the state
     * @param input_len the length of the input
     */
    inline void shake256_absorb(shake_ctx *ctx, const unsigned char *input, const size_t input_len) {
        if (Keccak_HashUpdate(ctx, input, input_len * 8) != 0) {
            abort();
        }
        if (Keccak_HashFinal(ctx, NULL) != 0) {
            abort();
        }
    }

    /**
     * Performs the squeeze step of the SHAKE-256 XOF. Squeezes full blocks of
     * SHAKE256_RATE bytes each. Can be called multiple times to keep squeezing
     * (i.e. this function is incremental).
     *
     * @param ctx the shake context
     * @param output the output
     * @param nr_blocks the number of blocks to squeeze
     */
    inline void shake256_squeezeblocks(shake_ctx *ctx, unsigned char *output, const size_t nr_blocks) {
        if (Keccak_HashSqueeze(ctx, output, nr_blocks * SHAKE256_RATE * 8) != 0) {
            abort();
        }
    }

    /**
     * Performs the full SHAKE-256 XOF to the given input.
     * @param output the final output
     * @param output_len the length of the output
     * @param input the input
     * @param input_len the length of the input
     */
    void shake256(unsigned char *output, const size_t output_len, const unsigned char *input, const size_t input_len);

    /**
     * Performs the initialisation step of the cSHAKE-128 XOF.
     *
     * @param ctx the cshake context
     * @param customization the customization string
     * @param customization_len the length of the customization string
     */
    inline void cshake128_init(cshake_ctx *ctx, const unsigned char *customization, const size_t customization_len) {
        if (cSHAKE128_Initialize(ctx, 0, NULL, 0, customization, customization_len * 8) != 0) {
            abort();
        }
    }

    /**
     * Performs the absorb step of the cSHAKE-128 XOF.
     *
     * @param ctx the cshake context
     * @param input the input absorbed into the state
     * @param input_len the length of the input
     */
    inline void cshake128_absorb(cshake_ctx *ctx, const unsigned char *input, const size_t input_len) {
        if (cSHAKE128_Update(ctx, input, input_len * 8) != 0) {
            abort();
        }
        if (cSHAKE128_Final(ctx, NULL) != 0) {
            abort();
        }
    }

    /**
     * Performs the squeeze step of the cSHAKE-128 XOF. Squeezes full blocks of
     * SHAKE128_RATE bytes each. Can be called multiple times to keep squeezing
     * (i.e. this function is incremental).
     *
     * @param ctx the cshake context
     * @param output the output
     * @param nr_blocks the number of blocks to squeeze
     */
    inline void cshake128_squeezeblocks(cshake_ctx *ctx, unsigned char *output, const size_t nr_blocks) {
        if (cSHAKE128_Squeeze(ctx, output, nr_blocks * SHAKE128_RATE * 8) != 0) {
            abort();
        }
    }

    /**
     * Performs the full cSHAKE-128 XOF to the given input.
     * @param output the final output
     * @param output_len the length of the output
     * @param input the input
     * @param input_len the length of the input
     * @param customization the customization string
     * @param customization_len the length of the customization string
     */
    inline void cshake128(unsigned char *output, size_t output_len, const unsigned char *input, const size_t input_len, const unsigned char *customization, const size_t customization_len) {
        if (cSHAKE128(input, input_len * 8, output, output_len * 8, NULL, 0, customization, customization_len) != 0) {
            abort();
        }
    }

    /**
     * Performs the initialisation step of the cSHAKE-256 XOF.
     *
     * @param ctx the cshake context
     * @param customization the customization string
     * @param customization_len the length of the customization string
     */
    inline void cshake256_init(cshake_ctx *ctx, const unsigned char *customization, const size_t customization_len) {
        if (cSHAKE256_Initialize(ctx, 0, NULL, 0, customization, customization_len * 8) != 0) {
            abort();
        }
    }

    /**
     * Performs the absorb step of the cSHAKE-256 XOF.
     *
     * @param ctx the cshake context
     * @param input the input absorbed into the state
     * @param input_len the length of the input
     */
    inline void cshake256_absorb(cshake_ctx *ctx, const unsigned char *input, const size_t input_len) {
        if (cSHAKE256_Update(ctx, input, input_len * 8) != 0) {
            abort();
        }
        if (cSHAKE256_Final(ctx, NULL) != 0) {
            abort();
        }
    }

    /**
     * Performs the squeeze step of the cSHAKE-256 XOF. Squeezes full blocks of
     * SHAKE256_RATE bytes each. Can be called multiple times to keep squeezing
     * (i.e. this function is incremental).
     *
     * @param ctx the cshake context
     * @param output the output
     * @param nr_blocks the number of blocks to squeeze
     */
    inline void cshake256_squeezeblocks(cshake_ctx *ctx, unsigned char *output, const size_t nr_blocks) {
        if (cSHAKE256_Squeeze(ctx, output, nr_blocks * SHAKE256_RATE * 8) != 0) {
            abort();
        }
    }

    /**
     * Performs the full cSHAKE-256 XOF to the given input.
     * @param output the final output
     * @param output_len the length of the output
     * @param input the input
     * @param input_len the length of the input
     * @param customization the customization string
     * @param customization_len the length of the customization string
     */
    inline void cshake256(unsigned char *output, size_t output_len, const unsigned char *input, const size_t input_len, const unsigned char *customization, const size_t customization_len) {
        if (cSHAKE256(input, input_len * 8, output, output_len * 8, NULL, 0, customization, customization_len) != 0) {
            abort();
        }
    }

#ifdef __cplusplus
}
#endif

#endif /* SHAKE_H */
