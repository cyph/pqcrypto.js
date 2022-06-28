/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the deterministic random bits (bytes) functions.
 */

#ifndef DRBG_H
#define DRBG_H

#include "r5_parameter_sets.h"
#include "misc.h"
#include "little_endian.h"
#include "shake.h"
#include "r5_hash.h"

#include <stdint.h>
#include <stddef.h>

#ifdef DOXYGEN
/* Document DRBG implementation option */
/**
 * The default implementation of the DRBG uses (c)SHAKE for the generation of
 * the deterministic random bytes. To make use of the alternative AES (in CTR
 * mode on zero input blocks) implementation, define `USE_AES_DRBG`.
 * Especially on platforms with good hardware accelerated AES instructions, this
 * can be an advantage.
 */
#define USE_AES_DRBG
#undef USE_AES_DRBG
#endif

#ifdef USE_AES_DRBG

#include "r5_memory.h"
#include <assert.h>
#include <openssl/opensslv.h>
#include <openssl/evp.h>
#include <string.h>

/**
 * The DRBG context data structure.
 */
typedef struct {
    EVP_CIPHER_CTX *aes_ctx; /**< The AES cipher context */
    uint8_t input[16]; /**< Input block (always 0). */
    uint8_t output[16]; /**< Buffer for output. */
    size_t index; /**< Current index in buffer. */
} drbg_ctx;

/**
 * Macro to initialize the AES DRBG context
 * @param ctx the DRBG context
 */
#define AES_INIT(ctx) \
    if (!(ctx.aes_ctx = EVP_CIPHER_CTX_new())) { \
        abort(); \
    } \
    do { } while (0)

#else

/**
 * The DRBG context data structure.
 */
typedef struct drbg_ctx {

    union {
        shake_ctx shake; /**< Context in case of a SHAKE generator */
        cshake_ctx cshake; /**< Context in case of a cSHAKE generator */
    } generator_ctx; /**< The generator context */
    uint8_t output[SHAKE128_RATE > SHAKE256_RATE ? SHAKE128_RATE : SHAKE256_RATE]; /**< Buffer for output. */
    size_t index; /**< Current index in buffer. */
} drbg_ctx;

#endif

/**
 * Initializes the deterministic random number generator.
 *
 * @param[in] seed      the seed to use for the deterministic number generator
 */
#ifdef USE_AES_DRBG
#define drbg_init(seed) \
    uint8_t key[32]; \
    drbg_ctx ctx = {NULL, {0}, {0}, 0}; \
    AES_INIT(ctx); \
    int res; \
    assert(PARAMS_KAPPA_BYTES == 16 || PARAMS_KAPPA_BYTES == 24 || PARAMS_KAPPA_BYTES == 32); \
    hash(key, PARAMS_KAPPA_BYTES, seed, PARAMS_KAPPA_BYTES, (uint8_t) PARAMS_KAPPA_BYTES); \
    switch (PARAMS_KAPPA_BYTES) { \
        case 16: \
            shake128(key, PARAMS_KAPPA_BYTES, seed, PARAMS_KAPPA_BYTES); \
            res = EVP_EncryptInit_ex(ctx.aes_ctx, EVP_aes_128_ctr(), NULL, key, NULL); \
            break; \
        case 24: \
            shake256(key, PARAMS_KAPPA_BYTES, seed, PARAMS_KAPPA_BYTES); \
            res = EVP_EncryptInit_ex(ctx.aes_ctx, EVP_aes_192_ctr(), NULL, key, NULL); \
            break; \
        case 32: \
            shake256(key, PARAMS_KAPPA_BYTES, seed, PARAMS_KAPPA_BYTES); \
            res = EVP_EncryptInit_ex(ctx.aes_ctx, EVP_aes_256_ctr(), NULL, key, NULL); \
            break; \
    } \
    if (res != 1) { \
        abort(); \
    } \
    ctx.index = 16

#else

#if PARAMS_KAPPA_BYTES > 16
#define drbg_init(seed) \
    drbg_ctx ctx; \
    shake256_init(&ctx.generator_ctx.shake); \
    shake256_absorb(&ctx.generator_ctx.shake, seed, PARAMS_KAPPA_BYTES); \
    ctx.index = SHAKE256_RATE

#else
#define drbg_init(seed) \
    drbg_ctx ctx; \
    shake128_init(&ctx.generator_ctx.shake); \
    shake128_absorb(&ctx.generator_ctx.shake, seed, PARAMS_KAPPA_BYTES); \
    ctx.index = SHAKE128_RATE

#endif

#endif

/**
 * Initializes the deterministic random number generator with the specified
 * customization string.
 *
 * @param[in] seed              the seed to use for the deterministic number generator
 * @param[in] customization     the customization string to use
 * @param[in] customization_len the length of the customization string
 */
#ifdef USE_AES_DRBG
#define drbg_init_customization(seed, customization, customization_len) \
    uint8_t key[32]; \
    drbg_ctx ctx = {NULL, {0}, {0}, 0}; \
    AES_INIT(ctx); \
    int res; \
    assert(PARAMS_KAPPA_BYTES == 16 || PARAMS_KAPPA_BYTES == 24 || PARAMS_KAPPA_BYTES == 32); \
    hash_customization(key, PARAMS_KAPPA_BYTES, seed, PARAMS_KAPPA_BYTES, customization, customization_len, (uint8_t) PARAMS_KAPPA_BYTES); \
    switch (PARAMS_KAPPA_BYTES) { \
        case 16: \
            res = EVP_EncryptInit_ex(ctx.aes_ctx, EVP_aes_128_ctr(), NULL, key, NULL); \
            break; \
        case 24: \
            res = EVP_EncryptInit_ex(ctx.aes_ctx, EVP_aes_192_ctr(), NULL, key, NULL); \
            break; \
        case 32: \
            res = EVP_EncryptInit_ex(ctx.aes_ctx, EVP_aes_256_ctr(), NULL, key, NULL); \
            break; \
    } \
    if (res != 1) { \
        abort(); \
    } \
    ctx.index = 16

#else

#if PARAMS_KAPPA_BYTES > 16
#define drbg_init_customization(seed, customization, customization_len) \
    drbg_ctx ctx; \
    cshake256_init(&ctx.generator_ctx.cshake, customization, customization_len); \
    cshake256_absorb(&ctx.generator_ctx.cshake, seed, PARAMS_KAPPA_BYTES); \
    ctx.index = SHAKE256_RATE

#else
#define drbg_init_customization(seed, customization, customization_len) \
    drbg_ctx ctx; \
    cshake128_init(&ctx.generator_ctx.cshake, customization, customization_len); \
    cshake128_absorb(&ctx.generator_ctx.cshake, seed, PARAMS_KAPPA_BYTES); \
    ctx.index = SHAKE128_RATE

#endif

#endif

/**
 * Generates the next sequence of deterministic random bytes using the
 * (initial) seed as set with `drbg_init()`.
 *
 * @param[out] x    destination buffer for the random bytes
 * @param[in]  xlen the number of deterministic random bytes to generate
 */
#ifdef USE_AES_DRBG
#define drbg(x, xlen) do { \
    size_t i, j; \
    i = ctx.index; \
    for (j = 0; j < xlen; j++) { \
        if (i >= 16) { \
            int len; \
            if (EVP_EncryptUpdate(ctx.aes_ctx, ctx.output, &len, ctx.input, 16) != 1) { \
                abort(); \
            } \
            i = 0; \
        } \
        ((uint8_t *) x)[j] = ctx.output[i++]; \
    } \
    ctx.index = i; \
} while (0)
#else
#if PARAMS_KAPPA_BYTES > 16
#define drbg(x, xlen) do { \
    size_t i, j; \
    i = ctx.index; \
    for (j = 0; j < xlen; j++) { \
        if (i >= SHAKE256_RATE) { \
            shake256_squeezeblocks(&ctx.generator_ctx.shake, ctx.output, 1); \
            i = 0; \
        } \
        ((uint8_t *) x)[j] = ctx.output[i++]; \
    } \
    ctx.index = i; \
} while (0)
#else
#define drbg(x, xlen) do { \
    size_t i, j; \
    i = ctx.index; \
    for (j = 0; j < xlen; j++) { \
        if (i >= SHAKE128_RATE) { \
            shake128_squeezeblocks(&ctx.generator_ctx.shake, ctx.output, 1); \
            i = 0; \
        } \
        ((uint8_t *) x)[j] = ctx.output[i++]; \
    } \
    ctx.index = i; \
} while (0)
#endif
#endif

/**
 * Generates the next deterministic random 16-bit integer using the
 * (initial) seed as set with `drbg_init()`.
 *
 * @param[out] x    destination variable for the 16-bit integer
 */
#define drbg16(x) do { \
    drbg(&x, 2); \
    x = (uint16_t) LITTLE_ENDIAN16(x); \
} while (0)

/**
 * Generates the next sequence of deterministic random bytes using the
 * (initial) seed as set with `drbg_init_customization()`.
 *
 * @param[out] x    destination of the random bytes
 * @param[in]  xlen the number of deterministic random bytes to generate
 * @return __0__ in case of success
 */
#ifdef USE_AES_DRBG
#define drbg_customization(x, xlen) do { \
    size_t i, j; \
    i = ctx.index; \
    for (j = 0; j < xlen; j++) { \
        if (i >= 16) { \
            int len; \
            if (EVP_EncryptUpdate(ctx.aes_ctx, ctx.output, &len, ctx.input, 16) != 1) { \
                abort(); \
            } \
            i = 0; \
        } \
        ((uint8_t *) x)[j] = ctx.output[i++]; \
    } \
    ctx.index = i; \
} while (0)
#else
#if PARAMS_KAPPA_BYTES > 16
#define drbg_customization(x, xlen) do { \
    size_t i, j; \
    i = ctx.index; \
    for (j = 0; j < xlen; j++) { \
        if (i >= SHAKE256_RATE) { \
            cshake256_squeezeblocks(&ctx.generator_ctx.cshake, ctx.output, 1); \
            i = 0; \
        } \
        ((uint8_t *) x)[j] = ctx.output[i++]; \
    } \
    ctx.index = i; \
} while (0)
#else
#define drbg_customization(x, xlen) do { \
    size_t i, j; \
    i = ctx.index; \
    for (j = 0; j < xlen; j++) { \
        if (i >= SHAKE128_RATE) { \
            cshake128_squeezeblocks(&ctx.generator_ctx.cshake, ctx.output, 1); \
            i = 0; \
        } \
        ((uint8_t *) x)[j] = ctx.output[i++]; \
    } \
    ctx.index = i; \
} while (0)
#endif
#endif

/**
 * Generates the next deterministic random 16-bit integer using the
 * (initial) seed as set with `drbg_init_customization()`.
 *
 * @param[out] x    destination variable for the 16-bit integer
 */
#define drbg16_customization(x) do { \
    drbg_customization(&x, 2); \
    x = (uint16_t) LITTLE_ENDIAN16(x); \
} while (0)

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Generates a sequence of deterministic random numbers using the given seed.
     * Can only be used to generate a single sequence of random numbers from the
     * given seed.
     *
     * Use this function to generate a fixed number of deterministic numbers
     * from a seed. It is faster than calling `drbg_init()` and
     * `drbg16()` separately.
     *
     * @param[out] x         destination of the random numbers
     * @param[in]  xlen      the number of deterministic random numbers to generate
     * @param[in]  seed      the seed to use for the deterministic number generator
     * @return __0__ in case of success
     */
    int drbg_sampler16_2_once(uint16_t *x, const size_t xlen, const void *seed);

    /**
     * Generates a sequence of deterministic random numbers using the given seed
     * and customization string.
     * Can only be used to generate a single sequence of random numbers from the
     * given seed.
     *
     * Use this function to generate a fixed number of deterministic numbers
     * from a seed. It is faster than calling `drbg_init()` and
     * `drbg16_customization()` separately.
     *
     * @param[out] x                 destination of the random numbers
     * @param[in]  xlen              the number of deterministic random numbers to generate
     * @param[in]  seed              the seed to use for the deterministic number generator
     * @param[in]  customization     the customization string to use
     * @param[in]  customization_len the length of the customization string
     * @return __0__ in case of success
     */
    int drbg_sampler16_2_once_customization(uint16_t *x, const size_t xlen, const void *seed, const void *customization, const size_t customization_len);

#ifdef __cplusplus
}
#endif

#endif /* DRBG_H */
