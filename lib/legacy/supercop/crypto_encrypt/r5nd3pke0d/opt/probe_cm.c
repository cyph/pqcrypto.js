/*
 * Copyright (c) 2018, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

#include "r5_parameter_sets.h"

#ifdef CM_CACHE

#include "probe_cm.h"

#ifndef SHIFT_LEFT64_CONSTANT_TIME

/**
 * Constant-time 64-bit left shift of 1. Use when 64-bit shifts may not
 * be constant-time on platform.
 *
 * @param out output variable (64-bit)
 * @param shift_amount the number of bits to shift the value 1 to the left
 * @param flag flag to indicate the shift amount can be >= 32
 * @return 1 << shift_amount
 */
#define constant_time_shift_1_left64(out, shift_amount, flag) do { \
    uint64_t tmp; \
    out = 1llu; \
    tmp = (uint64_t) (-((shift_amount) & 1)); \
    out = ((out << 1) & tmp) ^ (out & ~tmp); \
    tmp = (uint64_t) (-(((shift_amount) >> 1) & 1)); \
    out = ((out << 2) & tmp) ^ (out & ~tmp); \
    tmp = (uint64_t) (-(((shift_amount) >> 2) & 1)); \
    out = ((out << 4) & tmp) ^ (out & ~tmp); \
    tmp = (uint64_t) (-(((shift_amount) >> 3) & 1)); \
    out = ((out << 8) & tmp) ^ (out & ~tmp); \
    tmp = (uint64_t) (-(((shift_amount) >> 4) & 1)); \
    out = ((out << 16) & tmp) ^ (out & ~tmp); \
    if (flag) { \
        tmp = (uint64_t) (-(((shift_amount) >> 5) & 1)); \
        out = ((out << 32) & tmp) ^ (out & ~tmp); \
    } \
} while (0)

#else

/**
 * Constant-time 64-bit left shift of 1. Use if platform's left shift with
 * variable amount is constant-time.
 *
 * @param shift_amount the number of bits to shift the value 1 to the left
 * @param flag flag to indicate the shift amount can be >= 32 (ignored)
 * @return 1 << shift_amount
 */
#define constant_time_shift_1_left64(out, shift_amount, flag) out = (1llu << (shift_amount))

#endif

#if PARAMS_K !=1

// Cache-resistant "occupancy probe". Tests and "occupies" a single slot at x.
// Return value zero (false) indicates the slot was originally empty.

int probe_cm_odd(uint64_t *vplus, const uint16_t x) {
    int i;
    uint64_t a, b, c, y, z;
    uint64_t *vminus = vplus + PROBEVEC64;

    // construct the selector
    constant_time_shift_1_left64(y, x & 0x3F, 1); // low bits of index
    constant_time_shift_1_left64(z, x >> 6, 0); // high bits of index

    c = 0;
    for (i = 0; i < PROBEVEC64; i++) { // always scan through all
        a = vplus[i] | vminus[i];
        b = a | (y & (-(z & 1))); // set bit if not occupied.
        c |= a ^ b; // If change, mask. Change is in the corresponding bit to be set.
        vminus[i] ^= a ^ b; // update value of vminus[i] by xoring with the bit to change. This should only change the bit that changes.
        z >>= 1;
    }

    // final comparison doesn't need to be constant time
    return c == 0; // return true if was occupied before
}

int probe_cm_even(uint64_t *vplus, const uint16_t x) {
    int i;
    uint64_t a, b, c, y, z;
    uint64_t *vminus = vplus + PROBEVEC64;

    // construct the selector
    constant_time_shift_1_left64(y, x & 0x3F, 1); // low bits of index
    constant_time_shift_1_left64(z, x >> 6, 0); // high bits of index

    c = 0;
    for (i = 0; i < PROBEVEC64; i++) { // always scan through all
        a = vplus[i] | vminus[i];
        b = a | (y & (-(z & 1))); // set bit if not occupied.
        c |= a ^ b; // If change, mask. Change is in the corresponding bit to be set.
        vplus[i] ^= a ^ b; // update value of vplus[i]
        z >>= 1;
    }

    // final comparison doesn't need to be constant time
    return c == 0; // return true if was occupied before
}

#else

// Cache-resistant "occupancy probe". Tests and "occupies" a single slot at x.
// Return value zero (false) indicates the slot was originally empty.

int probe_cm(uint64_t *v, const uint16_t x) {
    int i;
    uint64_t a, b, c, y, z;

    // construct the selector
    constant_time_shift_1_left64(y, x & 0x3F, 1); // low bits of index
    constant_time_shift_1_left64(z, x >> 6, 0); // high bits of index

    c = 0;
    for (i = 0; i < PROBEVEC64; i++) { // always scan through all
        a = v[i];
        b = a | (y & (-(z & 1))); // set bit if not occupied.
        c |= a ^ b; // If change, mask.
        v[i] = b; // update value of v[i]
        z >>= 1;
    }

    // final comparison doesn't need to be constant time
    return c == 0; // return true if was occupied before
}

#endif

#endif
