/*
 * Copyright (c) 2019, PQShield and Koninklijke Philips N.V.
 * Markku-Juhani O. Saarinen, Koninklijke Philips N.V.
 */

// Fast matrix arithmetic (without cache attack countermeasures)

#include "matmul.h"
#include "misc.h"

#include <string.h>

#if PARAMS_K != 1 && !defined(CM_CACHE)

#include "drbg.h"
#include "little_endian.h"

// create a sparse ternary vector from a seed

void create_secret_matrix_s_t(uint16_t s_t[PARAMS_N_BAR][PARAMS_H / 2][2], const uint8_t *seed) {
    size_t i, l;
    uint16_t x;
    uint8_t v[PARAMS_D];

    drbg_init(seed);

    for (l = 0; l < PARAMS_N_BAR; l++) {
        memset(v, 0, sizeof (v));

        for (i = 0; i < PARAMS_H; i++) {
            do {
                do {
                    drbg16(x);
                } while (x >= PARAMS_RS_LIM);
                x /= PARAMS_RS_DIV;
            } while (v[x]);
            v[x] = 1;
            s_t[l][i >> 1][i & 1] = x; // addition / subtract index
        }
    }
}

// create a sparse ternary vector from a seed

void create_secret_matrix_r_t(uint16_t r_t[PARAMS_M_BAR][PARAMS_H / 2][2], const uint8_t *seed) {
    size_t i, l;
    uint16_t x;
    uint8_t v[PARAMS_D];

    drbg_init(seed);

    for (l = 0; l < PARAMS_M_BAR; l++) {
        memset(v, 0, sizeof (v));

        for (i = 0; i < PARAMS_H; i++) {
            do {
                do {
                    drbg16(x);
                } while (x >= PARAMS_RS_LIM);
                x /= PARAMS_RS_DIV;
            } while (v[x]);
            v[x] = 1;
            r_t[l][i >> 1][i & 1] = x; // addition / subtract index
        }
    }
}

// B = A * S

#if PARAMS_TAU == 0

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[PARAMS_D][PARAMS_D], uint16_t s_t[PARAMS_N_BAR][PARAMS_H / 2][2]) {

#elif PARAMS_TAU == 1

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[2 * PARAMS_D * PARAMS_D], uint32_t a_permutation[PARAMS_D], uint16_t s_t[PARAMS_N_BAR][PARAMS_H / 2][2]) {

#else

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[PARAMS_TAU2_LEN + PARAMS_D], uint16_t a_permutation[PARAMS_D], uint16_t s_t[PARAMS_N_BAR][PARAMS_H / 2][2]) {

#endif
    size_t i, j, l;

    // Initialize result
    memset(d, 0, PARAMS_N_BAR * PARAMS_D * sizeof (modq_t));

#if PARAMS_TAU == 0
#define A_element(x) a[j][s_t[l][i][x]]
    for (j = 0; j < PARAMS_D; j++) {
        for (l = 0; l < PARAMS_N_BAR; l++) {
            for (i = 0; i < PARAMS_H / 2; i++) {
                d[j][l] = (modq_t) (d[j][l] + (A_element(0) - A_element(1)));
            }
        }
    }
#undef A_element
#else
    for (l = 0; l < PARAMS_N_BAR; l++) {
        for (i = 0; i < PARAMS_H / 2 - 3; i += 4) {
            modq_t *a_add0 = &a[s_t[l][i + 0][0]];
            modq_t *a_sub0 = &a[s_t[l][i + 0][1]];
            modq_t *a_add1 = &a[s_t[l][i + 1][0]];
            modq_t *a_sub1 = &a[s_t[l][i + 1][1]];
            modq_t *a_add2 = &a[s_t[l][i + 2][0]];
            modq_t *a_sub2 = &a[s_t[l][i + 2][1]];
            modq_t *a_add3 = &a[s_t[l][i + 3][0]];
            modq_t *a_sub3 = &a[s_t[l][i + 3][1]];
            modq_t *dst = &d[0][l];
            size_t dst_idx = 0;
            for (j = 0; j < PARAMS_D; j++) {
                size_t k = a_permutation[j];
                dst[dst_idx] = (modq_t) (dst[dst_idx] + (a_add0[k] - a_sub0[k]) + (a_add1[k] - a_sub1[k]) + (a_add2[k] - a_sub2[k]) + (a_add3[k] - a_sub3[k]));
                dst_idx += PARAMS_N_BAR;
            }
        }
        while (i < PARAMS_H / 2) {
            modq_t *a_add = &a[s_t[l][i][0]];
            modq_t *a_sub = &a[s_t[l][i][1]];
            modq_t *dst = &d[0][l];
            size_t dst_idx = 0;
            for (j = 0; j < PARAMS_D; j++) {
                size_t k = a_permutation[j];
                dst[dst_idx] = (modq_t) (dst[dst_idx] + (a_add[k] - a_sub[k]));
                dst_idx += PARAMS_N_BAR;
            }
            ++i;
        }
    }
#endif
}



// U^T = R^T * A

#if PARAMS_TAU == 0

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[PARAMS_D][PARAMS_D], uint16_t r_t[PARAMS_M_BAR][PARAMS_H / 2][2]) {

#elif PARAMS_TAU == 1

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[2 * PARAMS_D * PARAMS_D], uint32_t a_permutation[PARAMS_D], uint16_t r_t[PARAMS_M_BAR][PARAMS_H / 2][2]) {

#else

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[PARAMS_TAU2_LEN + PARAMS_D], uint16_t a_permutation[PARAMS_D], uint16_t r_t[PARAMS_M_BAR][PARAMS_H / 2][2]) {

#endif
    size_t i, j, l;

    // Initialize result
    memset(d, 0, PARAMS_M_BAR * PARAMS_D * sizeof (modq_t));

#if PARAMS_TAU == 0
#define A_element(x) a[r_t[l][i][x]][j]
#else
#define A_element(x) a[a_permutation[r_t[l][i][x]] + j]
#endif

    for (l = 0; l < PARAMS_M_BAR; l++) {
        for (i = 0; i < PARAMS_H / 2; i++) {
            j = 0;
            modq_t *a_add = &A_element(0);
            modq_t *a_sub = &A_element(1);
            for (j = 0; j < PARAMS_D; j++) {
                d[l][j] = (modq_t) (d[l][j] + a_add[j] - a_sub[j]);
            }
        }
    }
#undef A_element
}


// X' = S^T * U

void matmul_stu_p(modp_t d[PARAMS_MU], modp_t u_t[PARAMS_M_BAR][PARAMS_D], uint16_t s_t[PARAMS_N_BAR][PARAMS_H / 2][2]) {
    size_t k, i, j;

    // Initialize result
    memset(d, 0, PARAMS_MU * sizeof (modp_t));

    size_t index = 0;
    for (i = 0; i < PARAMS_N_BAR && index < PARAMS_MU; ++i) {
        for (j = 0; j < PARAMS_M_BAR && index < PARAMS_MU; ++j) {
            for (k = 0; k < PARAMS_H / 2; ++k) {
                d[index] = (modp_t) (d[index] + (u_t[j][s_t[i][k][0]] - u_t[j][s_t[i][k][1]]));
            }
            ++index;
        }
    }
}

// X = B^T * R

void matmul_btr_p(modp_t d[PARAMS_MU], modp_t b[PARAMS_D][PARAMS_N_BAR], uint16_t r_t[PARAMS_M_BAR][PARAMS_H / 2][2]) {
    size_t i, j, l;

    // Initialize result
    memset(d, 0, PARAMS_MU * sizeof (modp_t));

    size_t index = 0;
    for (l = 0; l < PARAMS_N_BAR && index < PARAMS_MU; ++l) {
        for (j = 0; j < PARAMS_M_BAR && index < PARAMS_MU; ++j) {
            for (i = 0; i < PARAMS_H / 2; ++i) {
                d[index] = (modp_t) (d[index] + (b[r_t[j][i][0]][l] - b[r_t[j][i][1]][l]));
            }
            ++index;
        }
    }
}

#endif /* PARAMS_K != 1 && !defined(CM_CACHE) */
