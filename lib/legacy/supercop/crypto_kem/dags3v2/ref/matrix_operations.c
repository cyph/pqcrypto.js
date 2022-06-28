/*
 * matrix_operations.c
 *
 *  Created on: May 4, 2018
 *      Author: vader
 */

#include "matrix_operations.h"

#define min(a,b) (((a)<(b))?(a):(b))

/*
 * make_matrix
 * 	Creates a matrix initially with all zeros
 * param:
 *   n_rows				Provide the number of rows
 *   n_cols       Provide the number of columns
 * return:
 * 	Returns a pointer to the matrix
 */
matrix* make_matrix(const int n_rows, const int n_cols) {

	matrix * m = (matrix *) malloc(sizeof(matrix));
	// set dimensions
	m->rows = n_rows;
	m->cols = n_cols;

	// allocate a double array of length rows * cols
	m->data = (gf *) calloc(n_rows * n_cols, sizeof(gf));

	return m;
}



void gf_q_m_mult_block( gf* restrict C,  const  gf* restrict B, const gf A) {
	gf reduction[signature_block_size];
	uint32_t tmp[signature_block_size] = {0};

	int i,j;

	//Multiplication
	for (i = 0; i < 18; i++)	tmp[0] ^= (A * (B[0] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[1] ^= (A * (B[1] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[2] ^= (A * (B[2] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[3] ^= (A * (B[3] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[4] ^= (A * (B[4] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[5] ^= (A * (B[5] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[6] ^= (A * (B[6] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[7] ^= (A * (B[7] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[8] ^= (A * (B[8] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[9] ^= (A * (B[9] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[10] ^= (A * (B[10] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[11] ^= (A * (B[11] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[12] ^= (A * (B[12] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[13] ^= (A * (B[13] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[14] ^= (A * (B[14] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[15] ^= (A * (B[15] & (1 << i)));

	for (i = 0; i < 18; i++)	tmp[16] ^= (A * (B[16] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[17] ^= (A * (B[17] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[18] ^= (A * (B[18] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[19] ^= (A * (B[19] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[20] ^= (A * (B[20] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[21] ^= (A * (B[21] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[22] ^= (A * (B[22] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[23] ^= (A * (B[23] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[24] ^= (A * (B[24] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[25] ^= (A * (B[25] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[26] ^= (A * (B[26] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[27] ^= (A * (B[27] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[28] ^= (A * (B[28] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[29] ^= (A * (B[29] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[30] ^= (A * (B[30] & (1 << i)));
	for (i = 0; i < 18; i++)	tmp[31] ^= (A * (B[31] & (1 << i)));


	for (j = 0; j < signature_block_size; j++){
		for (i = 0; i < 2; i++){
            reduction[j] = (tmp[j] >> 16) &0x7fff;
            tmp[j] = tmp[j] & 0xFFFF;
            tmp[j] ^= reduction[j];
            tmp[j] ^= reduction[j] << 2;
            tmp[j] ^= reduction[j] << 3;
            tmp[j] ^= reduction[j] << 5;
        }
	}

	for (j = 0; j < signature_block_size; j++){
		C[j] =  tmp[j];
	}
}


void matrix_multiply(matrix* restrict H, const matrix* restrict H_cauchy, const gf z[n0]) {
	int row, col;
	int tmp_index;
	for (row = 0; row < H_cauchy->rows; row++){
		for (col = 0; col < n0; col++){
			tmp_index = row*code_length + col*signature_block_size;
			gf_q_m_mult_block(&H->data[tmp_index], &H_cauchy->data[tmp_index], z[col]);
		}
	}

}
/*
 * free_matrix
 * 	Frees the memory that was allocated by make_matrix
 */
void free_matrix(matrix* mtx) {
	if (NULL != mtx){
		free(mtx->data);
		free(mtx);
	}
}

matrix* submatrix(const matrix* m, const int i, const int j, const int nr_row, const int nr_col) {

	matrix* m_new = make_matrix(nr_row, nr_col);
	int j_temp = j;
	int i_temp = i;
	int new_row = 0;
	for (int row = 0; row < nr_row; row++) {
		int new_col = 0;
		j_temp = j;
		for (int col = 0; col < nr_col; col++) {
			m_new->data[new_row * nr_col + new_col]
										= m->data[i_temp * m->cols + j_temp];
			new_col++;
			j_temp++;
		}
		i_temp++;
		new_row++;
	}
	return m_new;

}

matrix* augment(const matrix* restrict a, const matrix* restrict b) {
	const int n_rows = a->rows;
	const int n_cols = a->cols + b->cols;
	matrix *result = make_matrix(n_rows, n_cols);

	for (int i = 0; i < n_rows; i++) {
		memcpy(&result->data[result->cols * i], &a->data[a->cols * i], a->cols*sizeof(gf));
		memcpy(&result->data[result->cols * i + a->cols], &b->data[b->cols * i], (n_cols-a->cols)*sizeof(gf) );
	}
	return result;
}

void echelon_form(matrix *a) {
	int nrows = a->rows;
	int ncols = a->cols;
	int c;
	int lead = 0;

	int d, m;
	while (lead < nrows) {

		for (int r = 0; r < nrows; r++) { // for each row ...
			/* calculate divisor and multiplier */
			d = a->data[lead * ncols + lead];
			m = gf_div(a->data[r * ncols + lead], a->data[lead * ncols + lead]);

			// for (int c = 0; c < ncols; c++) { // for each column ...
			// 	if (r == lead)
			// 		a->data[r * ncols + c] = gf_div(a->data[r * ncols + c], d); // make pivot = 1
			// 	else
			// 		a->data[r * ncols + c] ^= gf_mult(a->data[lead * ncols + c], m); // make other = 0
			// }

			if (r == lead){
				for (c = 0; c < ncols; c++){
					a->data[r * ncols + c] = gf_div(a->data[r * ncols + c], d); // make pivot = 1
				}
			}
			else{
				for (c = 0; c < ncols; c++){
					a->data[r * ncols + c] ^= gf_mult(a->data[lead * ncols + c], m); // make other = 0
				}
			}
		}
		lead++;
	}
}

matrix * transpose_matrix(const matrix* restrict a) {

	int n_a_cols = a->cols;
	int n_a_rows = a->rows;
	matrix* transposed = make_matrix(n_a_cols, n_a_rows);

	for (int c = 0; c < n_a_rows; c++)
		for (int d = 0; d < n_a_cols; d++)
			transposed->data[d * n_a_rows + c] = a->data[c * n_a_cols + d];

	return transposed;
}

void print_matrix(matrix* m) {
	int i, j;

	for (i = 0; i < m->rows; i++) {
		printf("| ");
		for (j = 0; j < m->cols; j++)
			printf(" %" PRIu16 " ", m->data[i * m->cols + j]);
		printf(" |\n");
	}
	printf("\n");
}

void multiply_vector_matrix( unsigned char* restrict u, matrix *G, gf* c) {

	int i, k;
	for (i = 0; i < G->cols; i++) {
		for (k = 0; k < G->rows; k++) {
			c[i] ^= gf_mult(G->data[k * G->cols + i], u[k]);
		}
	}
}

void quasi_dyadic_bloc_matrix(matrix *M, gf *sig, const int ind_col, const int ind_rown) {
	int i, j;
	for (i = ind_rown; i < signature_block_size + ind_rown; i++) {
		for (j = ind_col; j < signature_block_size + ind_col; j++) {
			M->data[i * M->cols + j] = sig[(i ^ j) & (signature_block_size-1)];
		}
	}
}
