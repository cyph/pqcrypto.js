/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */



#include "matmul.h"

#if PARAMS_K !=1 && defined(CM_CACHE)

#include "drbg.h"
#include "little_endian.h"
#include "probe_cm.h"

#include <string.h>

// create a sparse ternary vector from a seed

void create_secret_matrix_s_t(int16_t s_t[PARAMS_N_BAR][PARAMS_D], const uint8_t *seed) {
    uint64_t i;
    uint16_t h, l;
    uint16_t x;
    uint16_t base[PARAMS_H];
    uint16_t *y = &(base[PARAMS_H]);
    uint64_t v[2 * PROBEVEC64];

    memset(s_t, 0, PARAMS_N_BAR * PARAMS_D * sizeof (int16_t));

    drbg_init(seed);

    for (l = 0; l < PARAMS_N_BAR; l++) {
        memset(v, 0, sizeof (v));
        for (h = 0; h < PARAMS_H / 2; h++) {
            do {
                do {
                    if (y == &base[PARAMS_H]) {
                        y = base;
                        drbg(base, sizeof (base));
                    }
                    x = *y++;
                } while (x >= PARAMS_RS_LIM);
                x /= PARAMS_RS_DIV;
            } while (probe_cm_even(v, x));
            do {
                do {
                    if (y == &base[PARAMS_H]) {
                        y = base;
                        drbg(base, sizeof (base));
                    }
                    x = *y++;
                } while (x >= PARAMS_RS_LIM);
                x /= PARAMS_RS_DIV;
            } while (probe_cm_odd(v, x));
        }

        for (i = 0; i < PARAMS_D; i++) {
            s_t[l][i] = (int16_t) (((v[i >> 6] >> (i & 0x3F)) & 1) - ((v[(i >> 6) + PROBEVEC64] >> (i & 0x3F)) & 1)); // this is constant time since it goes through all values and always perform same operation.
        }
    }
}

// create a sparse ternary vector from a seed

void create_secret_matrix_r_t(int16_t r_t[PARAMS_M_BAR][PARAMS_D], const uint8_t * seed) {
    uint64_t i;
    uint16_t h, l;
    uint16_t x;
    uint16_t base[PARAMS_H];
    uint16_t *y = &(base[PARAMS_H]);
    uint64_t v[2 * PROBEVEC64];

    memset(r_t, 0, PARAMS_M_BAR * PARAMS_D * sizeof (int16_t));

    drbg_init(seed);

    for (l = 0; l < PARAMS_M_BAR; l++) {
        memset(v, 0, sizeof (v));
        for (h = 0; h < PARAMS_H / 2; h++) {
            do {
                do {
                    if (y == &base[PARAMS_H]) {
                        y = base;
                        drbg(base, sizeof (base));
                    }
                    x = *y++;
                } while (x >= PARAMS_RS_LIM);
                x /= PARAMS_RS_DIV;
            } while (probe_cm_even(v, x));
            do {
                do {
                    if (y == &base[PARAMS_H]) {
                        y = base;
                        drbg(base, sizeof (base));
                    }
                    x = *y++;
                } while (x >= PARAMS_RS_LIM);
                x /= PARAMS_RS_DIV;
            } while (probe_cm_odd(v, x));
        }
        for (i = 0; i < PARAMS_D; i++) {
            r_t[l][i] = (int16_t) (((v[i >> 6] >> (i & 0x3F)) & 1) - ((v[PROBEVEC64 + (i >> 6)] >> (i & 0x3F)) & 1)); // this is constant time since it goes through all values and always perform same operation.
        }
    }
}

#ifndef AVX2

// B = A * S

#if PARAMS_TAU == 0

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[PARAMS_D][PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {

#elif PARAMS_TAU == 1

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[2 * PARAMS_D * PARAMS_D], uint32_t a_permutation[PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {

#else

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[PARAMS_TAU2_LEN + PARAMS_D], uint16_t a_permutation[PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {

#endif
    size_t i, j, l;

    // Initialize result
    memset(d, 0, PARAMS_N_BAR * PARAMS_D * sizeof (modq_t));

#if PARAMS_TAU == 0
#define A_element(j, i) a[j][i]
#else
#define A_element(j, i) a[a_permutation[j] + i]
#endif
    for (j = 0; j < PARAMS_D; j++) {
        for (l = 0; l < PARAMS_N_BAR; l++) {
            for (i = 0; i < PARAMS_D; i++) {
                d[j][l] = (modq_t) (d[j][l] + s_t[l][i] * A_element(j, i));
            }
        }
    }

#undef A_element
}

// U^T = R^T * A

#if PARAMS_TAU == 0

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[PARAMS_D][PARAMS_D], int16_t r_t[PARAMS_M_BAR][PARAMS_D]) {

#elif PARAMS_TAU == 1

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[2 * PARAMS_D * PARAMS_D], uint32_t a_permutation[PARAMS_D], int16_t r_t[PARAMS_M_BAR][PARAMS_D]) {

#else

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[PARAMS_TAU2_LEN + PARAMS_D], uint16_t a_permutation[PARAMS_D], int16_t r_t[PARAMS_M_BAR][PARAMS_D]) {

#endif
    size_t i, j, l;

    // Initialize result
    memset(d, 0, PARAMS_M_BAR * PARAMS_D * sizeof (modq_t));

#if PARAMS_TAU == 0
#define A_element(i, j) a[i][j]
#else
#define A_element(i, j) a[a_permutation[i] + j]
#endif
    for (i = 0; i < PARAMS_D; i++) {
        for (j = 0; j < PARAMS_D; j++) {
            for (l = 0; l < PARAMS_M_BAR; l++) {
                d[l][j] = (modq_t) (d[l][j] + r_t[l][i] * A_element(i, j));
            }
        }
    }
#undef A_element
}

// X' = S^T * U

void matmul_stu_p(modp_t d[PARAMS_MU], modp_t u_t[PARAMS_M_BAR][PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {
    size_t i, l, j;

    // Initialize result
    memset(d, 0, PARAMS_MU * sizeof (modp_t));

    size_t index = 0;
    for (l = 0; l < PARAMS_N_BAR && index < PARAMS_MU; ++l) {
        for (j = 0; j < PARAMS_M_BAR && index < PARAMS_MU; ++j) {
            for (i = 0; i < PARAMS_D; ++i) {
                d[index] = (modp_t) (d[index] + s_t[l][i] * u_t[j][i]);
            }
            ++index;
        }
    }
}


#endif /* !AVX2 */

// X = B^T * R

void matmul_btr_p(modp_t d[PARAMS_MU], modp_t b[PARAMS_D][PARAMS_N_BAR], int16_t r_t[PARAMS_M_BAR][PARAMS_D]) {
    size_t i, j, l;

    // Initialize result
    memset(d, 0, PARAMS_MU * sizeof (modp_t));

    size_t index = 0;
    for (l = 0; l < PARAMS_N_BAR && index < PARAMS_MU; ++l) {
        for (j = 0; j < PARAMS_M_BAR && index < PARAMS_MU; ++j) {
            for (i = 0; i < PARAMS_D; ++i) {
                d[index] = (modp_t) (d[index] + b[i][l] * r_t[j][i]);
            }
            ++index;
        }
    }
}

#endif /* PARAMS_K !=1 && defined(CM_CACHE) */
