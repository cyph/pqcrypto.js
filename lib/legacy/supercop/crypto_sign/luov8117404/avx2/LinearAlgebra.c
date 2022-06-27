#include <stdio.h>
#include <stdlib.h>

#include "LinearAlgebra.h"

/*
	Creates a new matrix

	rows : the number of rows of the new matrix
	cols : the number of columns of the new matrix

	returns : The new matrix
*/
Matrix newMatrix(unsigned int rows, unsigned int cols) {
	unsigned int i;
	Matrix new;
	new.rows = rows;
	new.cols = cols;
	new.array = malloc(rows * sizeof(FELT*));
	for (i = 0; i < rows; i++) {
		new.array[i] = aligned_alloc(32,cols * sizeof(FELT));
	}
	return new;
}

/*
	Free the memory of a matrix

	mat : the matrix to destroy
*/
void destroy_matrix(Matrix mat) {
	int i;
	for (i = 0; i < mat.rows; i++) {
		free(mat.array[i]);
	}
	free(mat.array);
}

/*
	Prints a matrix
*/
void printMatrix(Matrix Mat) {
	int i, j;
	for (i = 0; i < Mat.rows; i++) {
		for (j = 0; j < Mat.cols; j++) {
			printFELT(Mat.array[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

/*
	Swaps two rows of a matrix

	A : A matrix
	row1 , row2 : The rows of A that have to be swapped
*/
void swapRows(Matrix A, int row1, int row2) {
	FELT *temp = A.array[row1];
	A.array[row1] = A.array[row2];
	A.array[row2] = temp;
}

/*
	Multiplies all the entries of a row of a matrix by a scalar

	A : A matrix
	row : the index of the row that has to be rescaled
	a : A field element
*/
void scaleRow(Matrix A, int row, FELT a, int offset) {
	__m256i Temp;
	int i;
	for (i=0; i < OIL_BLOCKS; i++)
	{
		if(offset < (i+1)*32){
			Temp = _mm256_setzero_si256();
			scalarMul_ct(&Temp , *((__m256i *) &A.array[row][i*32]) , a );
			memcpy(&A.array[row][i*32],(FELT *)&Temp,32);
		}
	}
}

void scalarMul2(__m256i *Out, __m256i A, __m256i *table){
	static unsigned char mask[32] = {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
	__m256i *Mask = (__m256i*) mask;
	*Out = _mm256_xor_si256(*Out, _mm256_shuffle_epi8(table[0],_mm256_and_si256(*Mask,A)));
	*Out = _mm256_xor_si256(*Out, _mm256_shuffle_epi8(table[1],_mm256_and_si256(*Mask,_mm256_srli_epi16(A,4))));
}

/*
	Add a part of the scalar multiple of one row of a matrix to another row of that matrix

	A : A matrix
	destrow : The row to add to
	sourcerow  : The row that is multiplied by a scalar and added to destrow
	constant : The contant that sourcerow is multiplied with
	offset : Only the entries in columns with index larger than or equal to offset are affected
*/
void rowOp(Matrix A, int destrow, int sourcerow, __m256i *table, int offset)
{
	int i;
	for(i=0 ; i<OIL_BLOCKS ; i++){
		if(offset<(i+1)*32){
			scalarMul2((__m256i *) &A.array[destrow][i*32], *((__m256i *) &A.array[sourcerow][i*32]) , table );
		}
	}
}

/*
	Adds the sourcerow to the destrow
*/
static inline
void addRow(Matrix A, int destrow, int sourcerow, int offset, __m256i mask){
	int i; 
	for(i=0 ; i<OIL_BLOCKS ; i++){
		if(offset<(i+1)*32){
			*((__m256i *) &A.array[destrow][i*32]) ^=  mask & (*((__m256i *) &A.array[sourcerow][i*32]));
		}
	}
}

/* 
	Puts the first part of an augmented matrix in row echelon form.
	
	A : A matrix

	returns : 1 if success, 0 if the first part was singlar
*/
int rowEchelonAugmented(Matrix A)
{
	int i,j,col;
	for (col = 0; col < A.cols - 1; ++col)
	{
		for (i = col+1; i<A.rows; i++)
		{
			unsigned char pivot = A.array[col][col];
			__m256i mask = _mm256_cmpeq_epi8(_mm256_set1_epi8(pivot),_mm256_setzero_si256());
			addRow(A,col,i,col,mask);
		}

		if (A.array[col][col] == 0)
		{
			return 0;
		}

		scaleRow(A, col, inverse_ct(A.array[col][col]),col);
		__m256i Tables[2*32*OIL_BLOCKS];
		alignas(32) FELT column[OIL_BLOCKS*32];
		for (j = 0; j < OIL_VARS-col ; j++)
		{
			column[j] = A.array[j+col][col]; 
		}
		for (; j<OIL_BLOCKS*32; j++)
		{
			column[j]=0;
		}

		prepareTables(Tables,column,OIL_VARS-col);
		for (i=col+1; i < A.rows; ++i)
		{
			rowOp(A, i, col, &Tables[2*(i-col)] , col);
		}
	}
	return 1;
}

/* 
	Calculates the unique solution to a linear system described by an augmented matrix

	A : The augmented matrix of some linear system of equations
	solution : Receives the unique solution if it exists 

	returns : 1 if a unique solution exists, 0 otherwise 
*/
int getUniqueSolution(Matrix A, FELT *solution) {
	int i,j;
	int success = rowEchelonAugmented(A);

	if (success == 0) {
		return 0;
	}

	alignas (32) FELT oil_sol[OIL_BLOCKS*32];
    for( i = 0 ; i< OIL_VARS ; i++){
    	oil_sol[i] = A.array[i][A.cols-1];
    }

	for (i = A.cols - 2; i >= 0; i--) {
		__m256i col[OIL_BLOCKS] = {{0}};
		FELT * Col = (FELT *) col;
		for (j = 0; j < i; j++)
		{
			Col[j] = A.array[j][i];
		}

		for(j = 0 ; j< (i+31)/32 ; j++){
			scalarMul_ct((__m256i *) &oil_sol[j*32], col[j] , oil_sol[i]);
		}
	}

	memcpy(solution,oil_sol,OIL_VARS);

	return 1;
}
