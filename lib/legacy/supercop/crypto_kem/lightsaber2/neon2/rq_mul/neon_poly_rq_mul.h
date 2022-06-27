/*=============================================================================
 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/
#ifndef NEON_POLY_RQ_MUL_H
#define NEON_POLY_RQ_MUL_H

#include <stdint.h>
#include "../SABER_params.h"
#include "../poly.h"

void neon_vector_vector_mul(uint16_t accumulate[SABER_N], const uint16_t modP,
                            const uint16_t polyvecA[SABER_K][SABER_N],
                            const uint16_t polyvecB[SABER_K][SABER_N]);

void neon_matrix_vector_mul(uint16_t vectorB[SABER_K][SABER_N], const uint16_t modQ,
                            const polyvec matrixA[SABER_K],
                            const uint16_t vectorS[SABER_K][SABER_N]);

void neon_matrix_vector_mul_transpose(uint16_t vectorB[SABER_K][SABER_N], const uint16_t modQ,
                                      const polyvec matrixA[SABER_K],
                                      const uint16_t vectorS[SABER_K][SABER_N]);

#endif
