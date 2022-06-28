/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the deterministic random bits (bytes) functions.
 */

#ifndef DRBG_H
#define DRBG_H

#include "little_endian.h"

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

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Initializes the deterministic random number generator.
     *
     * @param[in] seed      the seed to use for the deterministic number generator
     * @param[in] seed_size the size of the seed (must be 16, 24, or 32 bytes when USE_AES_DRBG is defined)
     */
    void drbg_init(const void *seed, const size_t seed_size);

    /**
     * Initializes the deterministic random number generator with the specified
     * customization string.
     *
     * @param[in] seed              the seed to use for the deterministic number generator
     * @param[in] seed_size         the size of the seed (must be 16, 24, or 32 bytes when USE_AES_DRBG is defined)
     * @param[in] customization     the customization string to use
     * @param[in] customization_len the length of the customization string
     */
    void drbg_init_customization(const void *seed, const size_t seed_size, const uint8_t *customization, const size_t customization_len);

    /**
     * Generates the next sequence of deterministic random bytes using the
     * (initial) seed as set with `drbg_init()`.
     *
     * @param[out] x    destination of the random bytes
     * @param[in]  xlen the number of deterministic random bytes to generate
     * @return __0__ in case of success
     */
    int drbg(void *x, const size_t xlen);

    /**
     * Generates the next uniformly distributed random number in the given
     * range using the (initial) seed as set with `drbg_init()`.
     *
     * Note: since this function re-calculates internal range factors, it is
     * better to use the `DRBG_SAMPLER16_INIT()` and `DRBG_SAMPLER16()` macros when
     * generating multiple random numbers.
     *
     * We use the "scaled" random number trick to quickly generate uniformly
     * distributed random numbers in a range.
     *
     * We scale the range so it is very close to a power of two (2^16 in this
     * case) and then scale back to get random number in the correct range.
     * This is much better than the normal approach since the number of rejects
     * is much smaller. For instance say we want to generate numbers in the
     * range 0-700, the normal approach would generate numbers in the range
     * 0-1023 and then retry if it was > 700. This is more than 30% rejects!
     * The scale trick on the other hand generates number in the range 0-65535
     * of which it only rejects if it was > 65.100, which happens in less than
     * 1% of the cases!
     *
     * @param range the maximum value (exclusive)
     * @return the next random number in the range _[0..range)_
     */
    uint16_t drbg_sampler16(const uint32_t range);

    /**
     * Generates the next sequence of deterministic random numbers using the
     * (initial) seed as set with `drbg_init()`.
     *
     * @param[in]  range the maximum value (exclusive, must be a power of 2!)
     * @return the next random number in the range _[0..range)_
     */
    uint16_t drbg_sampler16_2(const uint32_t range);

    /**
     * Generates a sequence of deterministic random numbers using the given seed.
     * Can only be used to generate a single sequence of random numbers from the
     * given seed.
     *
     * Use this function to generate a fixed number of deterministic numbers
     * from a seed. It is faster than calling `drbg_init()` and
     * `drbg_sampler16_2()` separately.
     *
     * @param[out] x         destination of the random numbers
     * @param[in]  xlen      the number of deterministic random numbers to generate
     * @param[in]  seed      the seed to use for the deterministic number generator
     * @param[in]  seed_size the size of the seed (must be 16, 24, or 32 bytes when USE_AES_DRBG is defined)
     * @param[in]  range     the maximum value (exclusive, must be a power of 2!)
     * @return __0__ in case of success
     */
    int drbg_sampler16_2_once(uint16_t *x, const size_t xlen, const void *seed, const size_t seed_size, const uint32_t range);

    /**
     * Generates a sequence of deterministic random numbers using the given seed
     * and customization string.
     * Can only be used to generate a single sequence of random numbers from the
     * given seed.
     *
     * Use this function to generate a fixed number of deterministic numbers
     * from a seed. It is faster than calling `drbg_init()` and
     * `drbg_sampler16_2()` separately.
     *
     * @param[out] x                 destination of the random numbers
     * @param[in]  xlen              the number of deterministic random numbers to generate
     * @param[in]  seed              the seed to use for the deterministic number generator
     * @param[in]  seed_size         the size of the seed (must be 16, 24, or 32 bytes when USE_AES_DRBG is defined)
     * @param[in]  customization     the customization string to use
     * @param[in]  customization_len the length of the customization string
     * @param[in]  range             the maximum value (exclusive)
     * @return __0__ in case of success
     */
    int drbg_sampler16_2_once_customization(uint16_t *x, const size_t xlen, const void *seed, const size_t seed_size, const void *customization, const size_t customization_len, const uint32_t range);

#ifdef __cplusplus
}
#endif

#endif /* DRBG_H */
