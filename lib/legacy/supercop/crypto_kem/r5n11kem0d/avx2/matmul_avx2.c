/*
 * Copyright (c) 2018, Koninklijke Philips N.V.
 *
 */

// Fast matrix arithmetic with AVX2 instructions

#include "matmul.h"

#if PARAMS_K !=1 && defined(AVX2)

#include "misc.h"

#include "drbg.h"
#include "little_endian.h"

#include <immintrin.h>
#include <string.h>

//  This allows working on blocks of data < PARAMS_D.
#define BLOCK_SIZE_COL PARAMS_D
//  If > 1, this allows using multiple secrets in parallel.
#define BLOCK_SIZE_ROW 1
// Number of elements for which the operations are performed in parallel.
#define BLOCK_AVX 16

// B = A * S

modq_t sum_array_elements(__m256i input) {
    __m256i tmp = _mm256_hadd_epi16(input, input); // 8 = 4 + 4
    tmp = _mm256_hadd_epi16(tmp, tmp); // 4 = 2 + 2
    tmp = _mm256_hadd_epi16(tmp, tmp); // 2= 1 + 1
    return (modq_t) (_mm256_extract_epi16(tmp, 0) + _mm256_extract_epi16(tmp, 8));
}

#if PARAMS_TAU == 2

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[PARAMS_TAU2_LEN + PARAMS_D], uint16_t a_permutation[PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {
#elif PARAMS_TAU == 1

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[2 * PARAMS_D * PARAMS_D], uint32_t a_permutation[PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {
#else //TAU = 0

void matmul_as_q(modq_t d[PARAMS_D][PARAMS_N_BAR], modq_t a[PARAMS_D][PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {
#endif

    size_t block_r, block_c, r, c, l;
    modq_t *local_block;

    memset(d, 0, PARAMS_N_BAR * PARAMS_D * sizeof (modq_t));

    for (block_r = 0; block_r < PARAMS_D / BLOCK_SIZE_ROW; block_r++) {
        for (r = 0; r < BLOCK_SIZE_ROW; r++) {
            for (block_c = 0; block_c < PARAMS_D / BLOCK_SIZE_COL; block_c++) {
#if PARAMS_TAU == 2 || PARAMS_TAU == 1
                local_block = &a[a_permutation[block_r * BLOCK_SIZE_ROW + r]]; // TAU=1,2
#else // TAU = 0
                local_block = &a[block_r * BLOCK_SIZE_ROW + r][0]; //TAU=0
#endif
                __m256i accum256[PARAMS_N_BAR] __attribute__ ((aligned(32)));
                for (l = 0; l < PARAMS_N_BAR; l++) {
                    accum256[l] = _mm256_setzero_si256();
                }
                for (c = 0; c < BLOCK_AVX * (BLOCK_SIZE_COL / BLOCK_AVX); c += BLOCK_AVX) {
                    __m256i x256 = _mm256_loadu_si256((__m256i*) & local_block[r * BLOCK_SIZE_COL + c]);
                    for (l = 0; l < PARAMS_N_BAR; l++) {
                        __m256i y256 = _mm256_loadu_si256((__m256i*) & s_t[l][block_c * BLOCK_SIZE_COL + c]);
                        __m256i z256 = _mm256_mullo_epi16(x256, y256);
                        accum256[l] = _mm256_add_epi16(accum256[l], z256);
                    }
                }
                for (l = 0; l < PARAMS_N_BAR; l++) {
                    d[block_r * BLOCK_SIZE_ROW + r ][l] = sum_array_elements(accum256[l]);
                }
#if PARAMS_D/BLOCK_AVX != 0
                for (c = BLOCK_AVX * (BLOCK_SIZE_COL / BLOCK_AVX); c < BLOCK_SIZE_COL; c++) {
                    for (l = 0; l < PARAMS_N_BAR; l++) {
                        d[block_r * BLOCK_SIZE_ROW + r ][l] += s_t[l][block_c * BLOCK_SIZE_COL + c] * local_block[r * BLOCK_SIZE_COL + c];
                    }
                }
#endif
            }
        }
    }
}

// U^T = R^T * A
#if PARAMS_TAU == 2

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[PARAMS_TAU2_LEN + PARAMS_D], uint16_t a_permutation[PARAMS_D], int16_t r_t[PARAMS_M_BAR][PARAMS_D]) {
#elif PARAMS_TAU == 1

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[2 * PARAMS_D * PARAMS_D], uint32_t a_permutation[PARAMS_D], int16_t r_t[PARAMS_M_BAR][PARAMS_D]) {
#else //TAU = 0

void matmul_rta_q(modq_t d[PARAMS_M_BAR][PARAMS_D], modq_t a[PARAMS_D][PARAMS_D], int16_t r_t[PARAMS_M_BAR][PARAMS_D]) {
#endif

    size_t block_r, block_c, r, c, l;
    __m256i accum[PARAMS_M_BAR * BLOCK_SIZE_COL / BLOCK_AVX] __attribute__ ((aligned(32)));
    modq_t * local_block __attribute__ ((aligned(32)));

    memset(d, 0, PARAMS_M_BAR * PARAMS_D * sizeof (modq_t));

    for (l = 0; l < PARAMS_M_BAR; l++) {
        for (c = 0; c < BLOCK_AVX * (BLOCK_SIZE_COL / BLOCK_AVX); c += BLOCK_AVX) {
            accum[(l * BLOCK_SIZE_COL + c) >> 4] = _mm256_setzero_si256();
        }
    }

    for (block_r = 0; block_r < PARAMS_D / BLOCK_SIZE_ROW; block_r++) {
        for (r = 0; r < BLOCK_SIZE_ROW; r++) {
            for (block_c = 0; block_c < PARAMS_D / BLOCK_SIZE_COL; block_c++) {
#if PARAMS_TAU == 2 || PARAMS_TAU == 1
                local_block = &a[a_permutation[block_r * BLOCK_SIZE_ROW + r] + block_c * BLOCK_SIZE_COL]; // TAU=1,2
#else
                local_block = &a[block_r * BLOCK_SIZE_ROW + r][block_c * BLOCK_SIZE_COL]; // TAU=0
#endif
                for (c = 0; c < BLOCK_SIZE_COL / BLOCK_AVX; c++) {
                    __m256i x256 = _mm256_loadu_si256((__m256i*) & local_block[c * BLOCK_AVX]);
                    for (l = 0; l < PARAMS_M_BAR; l++) {
                        __m256i s_c = _mm256_set1_epi16(r_t[l][block_r * BLOCK_SIZE_ROW + r]);
                        __m256i y256 = _mm256_mullo_epi16(s_c, x256);
                        accum[l * BLOCK_SIZE_COL / BLOCK_AVX + c] = _mm256_add_epi16(accum[l * BLOCK_SIZE_COL / BLOCK_AVX + c], y256);
                    }
                }
#if PARAMS_D/BLOCK_AVX != 0
                for (c = BLOCK_AVX * (BLOCK_SIZE_COL / BLOCK_AVX); c < BLOCK_SIZE_COL; c++) {
                    for (l = 0; l < PARAMS_M_BAR; l++) {
                        d[l][ block_c * BLOCK_SIZE_COL + c ] += r_t[l][block_r * BLOCK_SIZE_ROW + r] * local_block[r * BLOCK_SIZE_COL + c];
                    }
                }
#endif
            }
        }
    }
    for (l = 0; l < PARAMS_M_BAR; l++) {
        for (c = 0; c < BLOCK_AVX * (BLOCK_SIZE_COL / BLOCK_AVX); c += BLOCK_AVX) {
            _mm256_storeu_si256((__m256i *) &(d[l][c]), accum[(l * BLOCK_SIZE_COL + c) >> 4]);
        }
    }
}


// X' = S^T * U

void matmul_stu_p(modp_t d[PARAMS_MU], modp_t u_t[PARAMS_M_BAR][PARAMS_D], int16_t s_t[PARAMS_N_BAR][PARAMS_D]) {
    size_t i, l, j;
    // Initialize result
    memset(d, 0, PARAMS_MU * sizeof (modp_t));
    __m256i accum256[PARAMS_M_BAR] __attribute__ ((aligned(32)));
    size_t index = 0;
    for (l = 0; l < PARAMS_N_BAR && index < PARAMS_MU; ++l) {
        for (j = 0; j < PARAMS_M_BAR; ++j) {
            accum256[j] = _mm256_setzero_si256();
        }
        for (i = 0; i < BLOCK_AVX * (BLOCK_SIZE_COL / BLOCK_AVX); i += BLOCK_AVX) {
            __m256i y256 = _mm256_loadu_si256((__m256i*) & s_t[l][i]);
            for (j = 0; j < PARAMS_M_BAR; ++j) {
                __m256i x256 = _mm256_loadu_si256((__m256i*) & u_t[j][i]);
                __m256i z256 = _mm256_mullo_epi16(x256, y256);
                accum256[j] = _mm256_add_epi16(accum256[j], z256);
            }
        }
        for (j = 0; j < PARAMS_M_BAR && index < PARAMS_MU; ++j) {
            d[index] = sum_array_elements(accum256[j]);
            for (i = BLOCK_AVX * (BLOCK_SIZE_COL / BLOCK_AVX); i < PARAMS_D; ++i) {
                d[index] = (modp_t) (d[index] + s_t[l][i] * u_t[j][i]);
            }
            index++;
        }
    }
}

#endif /* PARAMS_K !=1 && defined(AVX2) */
