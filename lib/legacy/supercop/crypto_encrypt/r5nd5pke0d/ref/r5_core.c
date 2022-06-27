/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Implementation of the core algorithm functions.
 */

#include "r5_core.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "r5_memory.h"
#include "rng.h"
#include "little_endian.h"
#include "drbg.h"
#include "r5_hash.h"
#include "a_fixed.h"
#include "a_random.h"

/*******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * Computes a mod b using the fact that b is a power of 2.
 *
 * @param[in] a
 * @param[in] b
 * @return a mod b
 */
static uint16_t modulo(const int32_t a, const uint32_t b) {
    return (uint16_t) ((uint32_t) a & (b - 1));
}

/**
 * Create a sparse ternary vector of length len.
 * Important: the drbg must have been initialised before calling this function!
 *
 * @param[out] vector    the generated vector
 * @param[in]  len       the length of the ternary vector to create
 * @param[in]  h         the hamming weight (i.e. number of non-zero elements)
 * @return __0__ in case of success
 */
static int create_secret_vector(int16_t *vector, const uint16_t len, const uint16_t h) {
    size_t i;
    uint16_t idx;
    memset(vector, 0, sizeof (*vector) * len);

    for (i = 0; i < h; ++i) {
        do {
            idx = drbg_sampler16(len);
        } while (vector[idx] != 0);
        vector[idx] = (i & 1) ? -1 : 1;
    }

    return 0;
}

/**
 * Multiply 2 polynomials in the NTRU ring with len number of coefficients in
 * Z_mod and reduces the result modulo x^len - 1.
 *
 * @param[out] result result
 * @param[in]  pol_a  first operand
 * @param[in]  pol_b  second operand
 * @param[in]  len    number of coefficients
 * @param[in]  mod    reduction moduli for the coefficients
 * @return __0__ in case of success
 */
static int mult_poly_ntru(uint16_t *result, const int16_t *pol_a, const int16_t *pol_b, const size_t len, const uint32_t mod) {
    size_t i, j;

    for (i = 0; i < len; ++i) {
        result[i] = 0;
    }
    for (i = 0; i < len; ++i) {
        for (j = 0; j < len; j++) {
            size_t deg = (i + j) % len;
            int32_t tmp = pol_a[(i)] * pol_b[j];
            result[deg] = modulo(result[deg] + tmp, mod);
        }
    }

    return 0;
}

/**
 * Multiplies a polynomial in the cyclotomic ring times (X - 1), the result can
 * be taken to be in the NTRU ring X^(len+1) - 1.
 *
 * @param[out] ntru_pol  result
 * @param[in]  cyc_pol   polynomial in the cyclotomic ring
 * @param[in]  len       number of coefficients of the cyclotomic polynomial
 * @param[in]  mod       reduction moduli for the coefficients
 * @return __0__ in case of success
 */
static int lift_poly(uint16_t *ntru_pol, const int16_t *cyc_pol, const size_t len, const uint32_t mod) {
    size_t i;

    ntru_pol[0] = modulo(-cyc_pol[0], mod);
    for (i = 1; i < len; ++i) {
        ntru_pol[i] = modulo(cyc_pol[i - 1] - cyc_pol[i], mod);
    }
    ntru_pol[len] = modulo(cyc_pol[len - 1], mod);

    return 0;
}

/**
 * Divides a polynomial in the NTRU ring by (X - 1), the result can
 * be taken to be in the cyclotomic ring.
 *
 * @param[out] cyc_pol   result
 * @param[in]  ntru_pol  polynomial in the NTRU ring
 * @param[in]  len       number of coefficients of the cyclotomic polynomial
 * @param[in]  mod       reduction moduli for the coefficients
 * @return __0__ in case of success
 */
static int unlift_poly(uint16_t *cyc_pol, const uint16_t *ntru_pol, const size_t len, const uint32_t mod) {
    size_t i;

    cyc_pol[0] = modulo(-ntru_pol[0], mod);
    for (i = 1; i < len; ++i) {
        cyc_pol[i] = modulo(cyc_pol[i - 1] - ntru_pol[i], mod);
    }

    return 0;
}

/**
 * Multiplies two polynomials in the cyclotomic ring.
 *
 * The multiplication is done by (optionally) lifting one operand (i.e.
 * multiplying it by (X - 1)) performing the operation in the NTRU ring and then
 * unlifting the result again.
 *
 * @param[out] result result
 * @param[in]  pol_a  first operand
 * @param[in]  pol_b  second operand
 * @param[in]  len    number of coefficients of the polynomials
 * @param[in]  mod    reduction moduli for the coefficients
 * @param[in]  isXi   flag to indicate the polynomial ring (PHI or NTRU) in which operations are performed
 * @return __0__ in case of success
 */
static int mult_poly(uint16_t *result, const int16_t *pol_a, const int16_t *pol_b, const size_t len, const uint32_t mod, const int isXi) {
    uint16_t *ntru_a;
    int16_t *ntru_b;
    uint16_t *ntru_res;

    ntru_a = checked_malloc((len + 1) * sizeof (*ntru_a));
    ntru_b = checked_malloc((len + 1) * sizeof (*ntru_b));
    ntru_res = checked_malloc((len + 1) * sizeof (*ntru_res));

    if (isXi) {
        memcpy(ntru_a, pol_a, len * sizeof (*ntru_a));
        ntru_a[len] = 0;
    } else {
        lift_poly(ntru_a, pol_a, len, mod);
    }

    memcpy(ntru_b, pol_b, len * sizeof (*ntru_b));
    ntru_b[len] = 0;

    mult_poly_ntru(ntru_res, (int16_t *) ntru_a, ntru_b, len + 1, mod);

    if (isXi) {
        memcpy(result, ntru_res + 1, len * sizeof (*result));
    } else {
        unlift_poly(result, ntru_res, len, mod);
    }

    free(ntru_a);
    free(ntru_b);
    free(ntru_res);

    return 0;
}

/**
 * Add 2 polynomials with len number of coefficients in Z_mod and reduce the
 * result modulo x^len - 1.
 *
 * @param[out] result result
 * @param[in]  pol_a  first operand
 * @param[in]  pol_b  second operand
 * @param[in]  len    number of coefficients
 * @param[in]  mod    reduction modulus
 * @return __0__ in case of success
 */
static int add_poly(uint16_t *result, const uint16_t *pol_a, const uint16_t *pol_b, const size_t len, const uint32_t mod) {
    size_t i;

    for (i = 0; i < len; ++i) {
        result[i] = (uint16_t) (pol_a[i] + pol_b[i]);
        result[i] = modulo(result[i], mod);
    }

    return 0;
}

/**
 * Compress and round an element from a to b bits where a and b are power of 2 by
 * means of flooring and rounding constant.
 *
 * @param[out] x                 the value to round and compress
 * @param[in]  a_bits            original bit size
 * @param[in]  b_bits            compressed bit size
 * @param[in]  b_mask            mask to perform module computation with b
 * @param[in]  rounding_constant constant to add for rounding
 * @return __0__ in case of success
 */
static int round_element(uint16_t *x, const uint16_t a_bits, const uint16_t b_bits, const uint16_t b_mask, const uint16_t rounding_constant) {
    const uint16_t shift = (uint16_t) (a_bits - b_bits);
    *x = (uint16_t) (*x + rounding_constant);
    *x = (uint16_t) (*x >> shift);
    *x &= b_mask;
    return 0;
}

/**
 * Decompress a number from a to b bits.
 *
 * @param[out] x       the value to decompress
 * @param[in]  a_bits  compressed bit size
 * @param[in]  b_bits  decompressed bit size
 * @param[in]  b_mask  mask to perform module computation with b
 * @return __0__ in case of success
 */
static int decompress_element(uint16_t *x, const uint16_t a_bits, const uint16_t b_bits, const uint16_t b_mask) {
    const uint16_t shift = (uint16_t) (b_bits - a_bits);
    *x = (uint16_t) (*x << shift);
    *x &= b_mask;
    return 0;
}

/**
 * The DRBG customization when creating the tau=1 or tau=2 permutations.
 */
static const uint8_t permutation_customization[2] = {0, 1};

/**
 * Generates the permutation for the A matrix creation variant tau=1.
 *
 * @param[out] row_disp  the row displacements
 * @param[in]  seed      the seed
 * @param[in]  params    the algorithm parameters in use
 * @return __0__ on success
 */
static int permutation_tau_1(uint32_t *row_disp, const unsigned char *seed, const parameters *params) {
    uint32_t i;
    uint16_t rnd;

    drbg_init_customization(seed, params->kappa_bytes, permutation_customization, sizeof (permutation_customization));

    for (i = 0; i < params->d; ++i) {
        rnd = drbg_sampler16(params->d);
        row_disp[i] = i * params->d + rnd;
    }

    return 0;
}

/**
 * Generates the permutation for the A matrix creation variant tau=2.
 *
 * @param[out] row_disp  the row displacements
 * @param[in]  seed      the seed
 * @param[in]  params    the algorithm parameters in use
 * @return __0__ on success
 */
static int permutation_tau_2(uint32_t *row_disp, const unsigned char *seed, const parameters *params) {
    uint32_t i;
    uint16_t rnd;
    uint8_t *v = checked_calloc(params->tau2_len, 1);

    drbg_init_customization(seed, params->kappa_bytes, permutation_customization, sizeof (permutation_customization));

    for (i = 0; i < params->k; ++i) {
        do {
            rnd = drbg_sampler16_2(params->tau2_len);
        } while (v[rnd]);
        v[rnd] = 1;
        row_disp[i] = rnd;
    }

    free(v);

    return 0;
}

/*******************************************************************************
 * Public functions
 ******************************************************************************/

int create_A(uint16_t *A, const unsigned char *sigma, const parameters *params) {
    uint32_t i;
    uint16_t *A_master = NULL;
    uint32_t *A_permutation = NULL;
    const uint16_t els_row = (uint16_t) (params->k * params->n);

    /* Create A/A_Master*/
    assert(params->tau <= 2);
    switch (params->tau) {
        case 0:
            create_A_random(A, sigma, params);
            break;
        case 1:
            assert(A_fixed != NULL && A_fixed_len == (size_t) (params->d * params->k));
            A_master = A_fixed;
            break;
        case 2:
            A_master = checked_malloc((size_t) (params->tau2_len + params->d) * sizeof (*A_master));
            create_A_random(A_master, sigma, params);
            memcpy(A_master + params->tau2_len, A_master, params->d * sizeof (*A_master));
            break;
    }

    /* Compute and apply the permutation to get A */
    if (params->tau == 1 || params->tau == 2) {
        A_permutation = checked_malloc(params->k * sizeof (*A_permutation));

        /* Compute and apply permutation */
        if (params->tau == 1) {
            permutation_tau_1(A_permutation, sigma, params);
            for (i = 0; i < params->k; ++i) {
                uint32_t mod_d = A_permutation[i] % params->d;
                if (mod_d == 0) {
                    memcpy(A + (i * els_row), A_master + A_permutation[i], els_row * sizeof (*A));
                } else {
                    memcpy(A + (i * els_row), A_master + A_permutation[i], (els_row - mod_d) * sizeof (*A));
                    memcpy(A + (i * els_row) + (els_row - mod_d), A_master + A_permutation[i] - mod_d, mod_d * sizeof (*A));
                }
            }
        } else if (params->tau == 2) {
            permutation_tau_2(A_permutation, sigma, params);
            for (i = 0; i < params->k; ++i) {
                for (i = 0; i < params->k; ++i) {
                    memcpy(A + (i * els_row), A_master + A_permutation[i], els_row * sizeof (*A));
                }
            }
        }

        /* Free allocated memory */
        if (params->tau == 2) {
            free(A_master);
        }
        free(A_permutation);
    }

    return 0;
}

int create_S_T(int16_t *S_T, const unsigned char *sk, const parameters *params) {
    size_t i;
    const uint16_t len = (uint16_t) (params->k * params->n);

    /* Initialize drbg */
    drbg_init(sk, params->kappa_bytes);

    /* Create rows of sparse vectors */
    for (i = 0; i < params->n_bar; ++i) {
        create_secret_vector(&S_T[i * len], len, params->h);
    }

    return 0;
}

int create_R_T(int16_t *R_T, const unsigned char *rho, const parameters *params) {
    size_t i;
    const uint16_t len = (uint16_t) (params->k * params->n);

    /* Initialize drbg */
    drbg_init(rho, params->kappa_bytes);

    for (i = 0; i < params->m_bar; ++i) {
        create_secret_vector(&R_T[i * len], len, params->h);
    }

    return 0;
}

int mult_matrix(uint16_t *result, const int16_t *left, const size_t l_rows, const size_t l_cols, const int16_t *right, const size_t r_rows, const size_t r_cols, const size_t els, const uint32_t mod, const int isXi) {
    assert(l_cols == r_rows);

    size_t i, j, k;
    uint16_t *temp_poly = checked_malloc(els * sizeof (*temp_poly));

    /* Initialize result to zero */
    /* Note: this might not be constant-time */
    memset(result, 0, (size_t) (l_rows * r_cols * els * sizeof (*result)));

    for (i = 0; i < l_rows; ++i) {
        for (j = 0; j < r_cols; j++) {
            for (k = 0; k < l_cols; k++) {
                mult_poly(temp_poly, &left[i * (l_cols * els) + k * els], &right[k * (r_cols * els) + j * els], els, mod, isXi);
                add_poly(&result[i * (r_cols * els) + j * els], &result[i * (r_cols * els) + j * els], temp_poly, els, mod);
            }
        }
    }

    free(temp_poly);

    return 0;
}

int round_matrix(uint16_t *matrix, const size_t len, const size_t els, const uint16_t a, const uint16_t b, const uint16_t rounding_constant) {
    size_t i;

    uint16_t b_mask = (uint16_t) (((uint16_t) 1 << b) - 1);
    for (i = 0; i < len * els; ++i) {
        round_element(matrix + i, a, b, b_mask, rounding_constant);
    }

    return 0;
}

int decompress_matrix(uint16_t *matrix, const size_t len, const size_t els, const uint16_t a_bits, const uint16_t b_bits) {
    size_t i;

    const uint16_t b_mask = (uint16_t) (((uint16_t) 1 << b_bits) - 1);
    for (i = 0; i < len * els; ++i) {
        decompress_element(matrix + i, a_bits, b_bits, b_mask);
    }

    return 0;
}

int transpose_matrix(uint16_t *matrix_t, const uint16_t *matrix, const size_t rows, const size_t cols, const size_t els) {
    size_t i, j, k;

    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            for (k = 0; k < els; ++k) {
                matrix_t[j * (rows * els) + (i * els) + k] = matrix[i * (cols * els) + (j * els) + k];
            }
        }
    }

    return 0;
}
