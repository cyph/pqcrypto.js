/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of little-endian byte array conversion functions.
 */

#include "little_endian.h"

/* Force symbols to be emitted when no compiler optimisation is applied. */
extern inline uint8_t u8_from_le(const unsigned char *x);
extern inline void u8_to_le(unsigned char *x, const uint8_t u);
extern inline uint16_t u16_from_le(const unsigned char *x);
extern inline void u16_to_le(unsigned char *x, const uint16_t u);
extern inline uint32_t u32_from_le(const unsigned char *x);
extern inline void u32_to_le(unsigned char *x, const uint32_t u);
extern inline uint64_t u64_from_le(const unsigned char *x);
extern inline void u64_to_le(unsigned char *x, const uint64_t u);
