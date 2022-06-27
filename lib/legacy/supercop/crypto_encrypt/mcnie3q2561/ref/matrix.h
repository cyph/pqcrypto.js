#ifndef matrix_h
#define matrix_h

#include "gf.h"

struct gf_matrix
{
	short row,col;
	gf **dat;
};

typedef struct gf_matrix Gmat;

Gmat gen_matrix(short row, short col);     /* generate the matrix with gf entry */
Gmat gen_matrix_id(short row, short col);  /* generate the matrix whose left part is identity matrix */


void free_matrix(Gmat *M);  /* delete allocated memory of matrix data */

void print_matrix(Gmat *M);

Gmat mat_mul(Gmat A, Gmat B);    /* multiplication A * B */
Gmat mat_mul_t(Gmat A, Gmat B);  /* multiplication A * B^T */

/* Gauss elimination  */
void gf_gauss_elm(Gmat *M);
void gf_gauss_elm2(Gmat *M, Gmat *X);

short mat_rank(const Gmat A);
short mat_inv(Gmat A, Gmat *X);


#endif
