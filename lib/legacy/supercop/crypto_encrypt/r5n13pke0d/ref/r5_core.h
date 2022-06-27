/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 */

/**
 * @file
 * Declaration of the core algorithm functions.
 */


#ifndef PST_CORE_H
#define PST_CORE_H

#include <stddef.h>

#include "parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Creates __A__ from the given parameters and seed.
     *
     * A_master has length _d/n * d/n * n_.
     *
     * @param[out] A      created A
     * @param[in]  sigma  seed
     * @param[in]  params the algorithm parameters in use
     * @return __0__ in case of success
     */
    int create_A(uint16_t *A, const unsigned char *sigma, const parameters *params);

    /**
     * Creates random __S<sup>T</sup>__ from the given parameters.
     *
     * __S<sup>T</sup>__ has length _d/n * n * n_bar_.
     *
     * @param[out] S_T     created _S<sup>T</sup>_
     * @param[in]  sk      the secret key (used as seed)
     * @param[in]  params  the algorithm parameters in use
     * @return __0__ in case of success
     */
    int create_S_T(int16_t *S_T, const unsigned char *sk, const parameters *params);

    /**
     * Creates __R<sup>T</sup>__ from the given parameters and seed rho.
     *
     * __R<sup>T</sup>__ has length _d/n * n * m_bar_.
     *
     * @param[out] R_T      created _R<sup>T</sup>_
     * @param[in]  rho      seed
     * @param[in]  params   the algorithm parameters in use
     * @return __0__ in case of success
     */
    int create_R_T(int16_t *R_T, const unsigned char *rho, const parameters *params);

    /**
     * Computes _result = left * right_
     * Where left and right are matrices of polynomials.
     * The result is reduced modulo _x^n - 1_ and the coefficients modulo mod.
     *
     * @param[out] result  result of the operation
     * @param[in]  left    left side matrix
     * @param[in]  l_rows  number of rows of the left matrix
     * @param[in]  l_cols  number of columns of the left matrix
     * @param[in]  right   right side matrix
     * @param[in]  r_rows  number of rows of the right matrix
     * @param[in]  r_cols  number of columns of the right matrix
     * @param[in]  els     number of coefficients per polynomial
     * @param[in]  mod     modulo of the coefficients
     * @param[in]  isXi    flag to indicate the polynomial ring (PHI or NTRU) in which operations are performed
     * @return __0__ in case of success
     */
    int mult_matrix(uint16_t *result, const int16_t *left, const size_t l_rows, const size_t l_cols, const int16_t *right, const size_t r_rows, const size_t r_cols, const size_t els, const uint32_t mod, const int isXi);

    /**
     * Compress all coefficients in a matrix of polynomials from a bits to b bits
     * rounding them down (with rounding constant added).
     *
     * @param[out] matrix            matrix to compress and compressed matrix
     * @param[in]  len               size of the matrix (rows * columns)
     * @param[in]  els               number of coefficients per polynomial
     * @param[in]  a_bits            original value number of bits
     * @param[in]  b_bits            compressed value bits
     * @param[in]  rounding_constant the constant for the rounding
     * @return __0__ in case of success
     */
    int round_matrix(uint16_t *matrix, const size_t len, const size_t els, const uint16_t a_bits, const uint16_t b_bits, const uint16_t rounding_constant);

    /**
     * Decompress all coefficients in a matrix of polynomials from a bits to b bits.
     *
     * @param[out] matrix matrix to compress and compressed matrix
     * @param[in]  len    size of the matrix (rows * columns)
     * @param[in]  els    number of coefficients per polynomial
     * @param[in]  a_bits compressed value number of bits
     * @param[in]  b_bits decompressed value number of bits
     * @return __0__ in case of success
     */
    int decompress_matrix(uint16_t *matrix, const size_t len, const size_t els, const uint16_t a_bits, const uint16_t b_bits);

    /**
     * Transpose a matrix of polynomials and store it in matrix_t.
     *
     * @param[out] matrix_t transposed matrix
     * @param[in]  matrix   original matrix
     * @param[in]  rows     number of rows of the original matrix
     * @param[in]  cols     number of columns of the original matrix
     * @param[in]  els      number of coefficients per polynomial
     * @return __0__ in case of success
     */
    int transpose_matrix(uint16_t *matrix_t, const uint16_t *matrix, const size_t rows, const size_t cols, const size_t els);

#ifdef __cplusplus
}
#endif

#endif /* PST_CORE_H */
