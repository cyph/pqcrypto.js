/*
 * Copyright (c) 2019, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

#ifndef PROBE_CM_H
#define PROBE_CM_H

#include "r5_parameter_sets.h"

// Size of the vector to pass to probe_cm
#define PROBEVEC64  ((PARAMS_D + 63) / 64)

#ifdef __cplusplus
extern "C" {
#endif

    // Cache-resistant "occupancy probe". Tests and "occupies" a single slot at x.
    // Return value zero (false) indicates the slot was originally empty.

#if PARAMS_K != 1
    int probe_cm_odd(uint64_t *vplus, const uint16_t x);
    int probe_cm_even(uint64_t *vplus, const uint16_t x);
#else
    int probe_cm(uint64_t *v, const uint16_t x);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PROBE_CM_H */
