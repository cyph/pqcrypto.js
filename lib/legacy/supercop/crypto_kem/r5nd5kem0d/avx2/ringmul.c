/*
 * Copyright (c) 2018, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

// Fast ring arithmetic (without cache attack countermeasures)

#include "ringmul.h"
#include "r5_parameter_sets.h"

#if PARAMS_K == 1 && !defined(CM_CACHE)

#include "drbg.h"
#include "little_endian.h"

#include <string.h>

// create a sparse ternary vector from a seed

void create_secret_vector(uint16_t idx[PARAMS_H / 2][2], const uint8_t *seed) {
    size_t i;
    uint16_t x;
    uint8_t v[PARAMS_ND];

    memset(v, 0, sizeof (v));
    drbg_init(seed);

    for (i = 0; i < PARAMS_H; i++) {
        do {
            do {
                drbg16(x);
            } while (x >= PARAMS_RS_LIM);
            x /= PARAMS_RS_DIV;
        } while (v[x]);
        v[x] = 1;
        idx[i >> 1][i & 1] = x; // addition / subtract index
    }
}

// multiplication mod q, result length n

void ringmul_q(modq_t d[PARAMS_ND], modq_t a[PARAMS_ND], uint16_t idx[PARAMS_H / 2][2]) {
    size_t i, j;
    modq_t *p_add, *p_sub;
    modq_t p[2 * (PARAMS_ND + 1)];

    // Note: order of coefficients a[1..n] is *NOT* reversed!
    // "lift" -- multiply by (x - 1)
    p[0] = (modq_t) (-a[0]);
    for (i = 1; i < PARAMS_ND; i++) {
        p[i] = (modq_t) (a[i - 1] - a[i]);
    }
    p[PARAMS_ND] = a[PARAMS_ND - 1];

    // Duplicate at the end
    memcpy(p + (PARAMS_ND + 1), p, (PARAMS_ND + 1) * sizeof (modq_t));

    // Initialize result
    memset(d, 0, PARAMS_ND * sizeof (modq_t));

    for (i = 0; i < PARAMS_H / 2; i++) {
        p_add = &p[PARAMS_ND + 1 - idx[i][0]];
        p_sub = &p[PARAMS_ND + 1 - idx[i][1]];

        for (j = 0; j < PARAMS_ND; j++) {
            d[j] = (modq_t) (d[j] + p_add[j] - p_sub[j]);
        }
    }

    // "unlift"
    d[0] = (uint16_t) (-d[0]);
    for (i = 1; i < PARAMS_ND; ++i) {
        d[i] = (uint16_t) (d[i - 1] - d[i]);
    }
}


// multiplication mod p, result length mu

void ringmul_p(modp_t d[PARAMS_MU], modp_t a[PARAMS_ND], uint16_t idx[PARAMS_H / 2][2]) {
    size_t i, j;
    modp_t *p_add, *p_sub;
    modp_t p[(PARAMS_MU + 2) + (PARAMS_ND + 1)];

    // Note: order of coefficients p[1..N] is *NOT* reversed!
#if (PARAMS_XE == 0) && (PARAMS_F == 0)
    // Without error correction we "lift" -- i.e. multiply by (x - 1)
    p[0] = (modp_t) (-a[0]);
    for (i = 1; i < PARAMS_ND; i++) {
        p[i] = (modp_t) (a[i - 1] - a[i]);
    }
    p[PARAMS_ND] = a[PARAMS_ND - 1];
#else
    // With error correction we do not "lift"
    memcpy(p, a, PARAMS_ND * sizeof (modp_t));
    p[PARAMS_ND] = 0;
#endif

    // Duplicate elements so we don't need to perform index modulo
    memcpy(p + (PARAMS_ND + 1), p, (PARAMS_MU + 2) * sizeof (modp_t));

    // Initialize result
    memset(d, 0, (PARAMS_MU) * sizeof (modp_t));

    for (i = 0; i < PARAMS_H / 2; i++) {
#if (PARAMS_XE == 0) && (PARAMS_F == 0)
        p_add = &p[PARAMS_ND + 1 - idx[i][0]];
        p_sub = &p[PARAMS_ND + 1 - idx[i][1]];
#else
        p_add = &p[PARAMS_ND + 2 - idx[i][0]];
        p_sub = &p[PARAMS_ND + 2 - idx[i][1]];
#endif
        for (j = 0; j < PARAMS_MU; j++) {
            d[j] = (modp_t) (d[j] + p_add[j] - p_sub[j]);
        }
    }

#if (PARAMS_XE == 0) && (PARAMS_F == 0)
    // Without error correction we "lifted" so we now need to "unlift"
    d[0] = (modp_t) (-d[0]);
    for (i = 1; i < PARAMS_MU; ++i) {
        d[i] = (modp_t) (d[i - 1] - d[i]);
    }
#endif
}

#endif /* PARAMS_K == 1 && !defined(CM_CACHE) */
