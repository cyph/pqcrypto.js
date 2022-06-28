/*
 * matrix_operations.h
 *
 *  Created on: May 4, 2018
 *      Author: vader
 */

#ifndef INCLUDE_STRUCTURES_MATRIX_OPERATIONS_H_
#define INCLUDE_STRUCTURES_MATRIX_OPERATIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "definitions.h"
#include "gf.h"

extern matrix* make_matrix(const int n_rows, const int n_cols);

extern matrix* diagonal_matrix(gf* data, const int n_rows, const int n_cols);

extern void free_matrix(matrix* m);

extern void print_matrix(matrix* m);

void matrix_multiply(matrix* restrict H, const matrix* restrict H_cauchy, const gf z[n0]);

extern matrix* submatrix(const matrix* m, const int star_row, const int end_row, const int start_column, const int end_column);

extern matrix* augment(const matrix *a, const matrix *b);

extern void echelon_form(matrix *a);

extern matrix* transpose_matrix(const matrix* restrict a);

extern void multiply_vector_matrix(unsigned char* restrict u, matrix *G, gf  *c);

extern void quasi_dyadic_bloc_matrix( matrix *M, gf  *sig, const int ind_col, const int ind_rown);


#endif /* INCLUDE_STRUCTURES_MATRIX_OPERATIONS_H_ */
