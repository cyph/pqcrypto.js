#ifndef LINEARALGEBRA_H
#define LINEARALGEBRA_H

#include <stdlib.h>
#include <string.h>
#include <stdalign.h>

#include "F256Field.h"
#include "AVX_Operations.h"

#define PRINTMATRIX(M) printf(#M " = \n"); printMatrix(M);

/*Matrix over F_Q*/
typedef struct {
	int rows;
	int cols;
	FELT** array;
} Matrix;

Matrix newMatrix(unsigned int rows, unsigned int cols);
void destroy_matrix(Matrix mat);
void printMatrix(Matrix Mat);
int getUniqueSolution(Matrix A, FELT *solution);

#endif
