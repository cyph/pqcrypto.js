/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the little-endian byte array conversion functions.
 */

#ifndef LITTLE_ENDIAN_H
#define LITTLE_ENDIAN_H

#include <stdint.h>

/**
 * Macro to switch the byte order of a 64-bit integer to/from little-endian.
 *
 * @param x the 64-bit integer
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LITTLE_ENDIAN64(x) (x)
#else
#define LITTLE_ENDIAN64(x) (             \
    (((x) & 0xFF00000000000000U) >> 56) | \
    (((x) & 0x00FF000000000000U) >> 40) | \
    (((x) & 0x0000FF0000000000U) >> 24) | \
    (((x) & 0x000000FF00000000U) >> 8)  | \
    (((x) & 0x00000000FF000000U) << 8)  | \
    (((x) & 0x0000000000FF0000U) << 24) | \
    (((x) & 0x000000000000FF00U) << 40) | \
    (((x) & 0x00000000000000FFU) << 56)   \
)
#endif

/**
 * Macro to switch the byte order of a 32-bit integer to/from little-endian.
 *
 * @param x the 32-bit integer
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LITTLE_ENDIAN32(x) (x)
#else
#define LITTLE_ENDIAN32(x) (     \
    (((x) & 0xFF000000U) >> 24) | \
    (((x) & 0x00FF0000U) >> 8)  | \
    (((x) & 0x0000FF00U) << 8)  | \
    (((x) & 0x000000FFU) << 24)   \
)
#endif

/**
 * Macro to switch the byte order of a 16-bit integer to/from little-endian.
 *
 * @param x the 16-bit integer
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LITTLE_ENDIAN16(x) (x)
#else
#define LITTLE_ENDIAN16(x) ( \
    (((x) & 0xFF00U) >> 8) |  \
    (((x) & 0x00FFU) << 8)    \
)
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Note that on a little-endian machine, the compiler actually optimises
     * below functions to to a memory copy already so we don't have to
     * optimise the code ourselves.
     */

    /**
     * Creates an 8 bit value from an array of 1 byte.
     *
     * @param[in] x array of 1 byte (little-endian) that make up the 8 bit
     *              unsigned integer
     * @return the 8 bit value represented by the byte
     */
    inline uint8_t u8_from_le(const unsigned char *x) {
        return (uint8_t) x[0];
    }

    /**
     * Creates an array of 1 byte from an 8 bit unsigned integer.
     *
     * @param[out] x array of 1 bytes
     * @param[in]  u the 8 bit unsigned integer
     */
    inline void u8_to_le(unsigned char *x, const uint8_t u) {
        x[0] = u;
    }

    /**
     * Creates a 16 bit value from an array of 2 bytes (little-endian).
     *
     * @param[in] x array of 2 bytes (little-endian) that make up the 16 bit
     *              unsigned integer
     * @return the 16 bit value represented by the 2 bytes
     */
    inline uint16_t u16_from_le(const unsigned char *x) {
        return (uint16_t) (x[0]
                | (uint16_t) (x[1]) << 8);
    }

    /**
     * Creates an array of 2 bytes (little-endian) from a 16 bit unsigned
     * integer.
     *
     * @param[out] x array of 2 bytes
     * @param[in]  u the 16 bit unsigned integer
     */
    inline void u16_to_le(unsigned char *x, const uint16_t u) {
        x[0] = (unsigned char) u;
        x[1] = (unsigned char) (u >> 8);
    }

    /**
     * Creates a 32 bit value from an array of 4 bytes (little-endian).
     *
     * @param[in] x array of 4 bytes (little-endian) that make up the 32 bit
     *              unsigned integer
     * @return the 32 bit value represented by the 4 bytes
     */
    inline uint32_t u32_from_le(const unsigned char *x) {
        return (uint32_t) (x[0])
                | (((uint32_t) (x[1])) << 8)
                | (((uint32_t) (x[2])) << 16)
                | (((uint32_t) (x[3])) << 24);
    }

    /**
     * Creates an array of 4 bytes (little-endian) from a 32 bit unsigned
     * integer.
     *
     * @param[out] x array of 4 bytes
     * @param[in]  u the 32 bit unsigned integer
     */
    inline void u32_to_le(unsigned char *x, const uint32_t u) {
        x[0] = (unsigned char) u;
        x[1] = (unsigned char) (u >> 8);
        x[2] = (unsigned char) (u >> 16);
        x[3] = (unsigned char) (u >> 24);
    }

    /**
     * Creates a 64 bit value from an array of 8 bytes (little-endian).
     *
     * @param[in] x array of 8 bytes (little-endian) that make up the 64 bit
     *              unsigned integer
     * @return the 64 bit value represented by the 8 bytes
     */
    inline uint64_t u64_from_le(const unsigned char *x) {
        return (uint64_t) (x[0])
                | (((uint64_t) (x[1])) << 8)
                | (((uint64_t) (x[2])) << 16)
                | (((uint64_t) (x[3])) << 24)
                | (((uint64_t) (x[4])) << 32)
                | (((uint64_t) (x[5])) << 40)
                | (((uint64_t) (x[6])) << 48)
                | (((uint64_t) (x[7])) << 56);
    }

    /**
     * Creates an array of 8 bytes (little-endian) from a 64 bit unsigned
     * integer.
     *
     * @param[out] x array of 8 bytes
     * @param[in]  u the 64 bit unsigned integer
     */
    inline void u64_to_le(unsigned char *x, const uint64_t u) {
        x[0] = (unsigned char) u;
        x[1] = (unsigned char) (u >> 8);
        x[2] = (unsigned char) (u >> 16);
        x[3] = (unsigned char) (u >> 24);
        x[4] = (unsigned char) (u >> 32);
        x[5] = (unsigned char) (u >> 40);
        x[6] = (unsigned char) (u >> 48);
        x[7] = (unsigned char) (u >> 56);
    }

#ifdef __cplusplus
}
#endif

#endif /* LITTLE_ENDIAN_H */
