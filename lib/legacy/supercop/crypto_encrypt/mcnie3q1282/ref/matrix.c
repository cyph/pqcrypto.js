#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

/* generate the matrix with gf entry */
Gmat gen_matrix(short row, short col)
{
	short i;
	Gmat M;

	M.row = row;
	M.col = col;

	M.dat = (gf **)malloc(row * sizeof(*(M.dat)));

	for( i=0 ; i<row ; i++ )
		M.dat[i] = (gf *)malloc(col * sizeof(**(M.dat)));

	return M;	
}

/* generate the matrix whose left part is identity matrix (systematic form).
   all other entries are zero */
Gmat gen_matrix_id(short row, short col)
{
	short i,j,n;
	Gmat M;

	M.row = row;
	M.col = col;

	M.dat = (gf **)malloc(row * sizeof(*(M.dat)));

	for( i=0 ; i<row ; i++ )
		M.dat[i] = (gf *)malloc(col * sizeof(**(M.dat)));

	for( i=0 ; i<row ; i++ )
	  for( j=0 ; j<col ; j++ )
		M.dat[i][j] = 0;

	n = ( row < col ) ? row : col;

	for( i=0 ; i<n ; i++ )
	  M.dat[i][i] = 1;

	return M;	

}

/* delete allocated memory of matrix data */
void free_matrix(Gmat *M)
{
	short i;

	for( i=0 ; i<M->row ; i++ ) free(M->dat[i]);

	free(M->dat);
}


void print_matrix(Gmat *M)
{
	short i,j;

	for( i=0 ; i<M->row ; i++ )
	{
		for( j=0 ; j<M->col ; j++ )
		  printf("%016llX ",M->dat[i][j]);
		printf("\n");
	}

	printf("\n");
}


/* multiplication A * B */
Gmat mat_mul(Gmat A, Gmat B)
{
	short i,j,t;
	Gmat X;

	if(A.col != B.row) { printf("matrix size error\n"); exit(-1); }

	X = gen_matrix(A.row,B.col);

	for( i=0 ; i<X.row ; i++ )
	  for( j=0 ; j<X.col ; j++ )
		for( t=0, X.dat[i][j]=0 ; t<A.col ; t++ )
		  X.dat[i][j] ^= gf_mul(A.dat[i][t],B.dat[t][j]);

	return X;
}

/* multiplication A * B^T */
Gmat mat_mul_t(Gmat A, Gmat B)
{
	short i,j,t;
	Gmat X;

	if(A.col != B.col) { printf("matrix size error\n"); exit(-1); }

	X = gen_matrix(A.row,B.row);

	for( i=0 ; i<X.row ; i++ )
	  for( j=0 ; j<X.col ; j++ )
		for( t=0, X.dat[i][j]=0 ; t<A.col ; t++ )
		  X.dat[i][j] ^= gf_mul(A.dat[i][t],B.dat[j][t]);


	return X;
}


/* Gauss elimination */
void gf_gauss_elm(Gmat *M)
{
	short i,j,t,tt;
	gf inv,y;
	gf *temp;

	for( i=0, j=0 ; i<M->row && j<M->col ; j++ )
	{
		/* find a nonzero entry in j-th row */
		for( t=i ; t<M->row ; t++ )
		  if( M->dat[t][j] )
			break;

		/* if all entries are zero, then we find it in next column */
		if( t==M->row ) continue;

		/* if i-th row are all zero, then switch it with nonzero row */
		if( t!=i ) { temp = M->dat[i];  M->dat[i] = M->dat[t];  M->dat[t] = temp; }

		/* row operation in order that (i,j) entry is 1 */
		inv = gf_inverse(M->dat[i][j]);
		for( t=j ; t<M->col ; t++ ) M->dat[i][t] = gf_mul(M->dat[i][t],inv);

		for( t=0 ; t<M->row ; t++ )
		{
			if( t==i ) continue;
			y = M->dat[t][j];
			if( y )
			{
				for( tt=0 ; tt<M->col ; tt++ )
					gf_add(&(M->dat[t][tt]),gf_mul(M->dat[i][tt],y));
			}
		}

		i++;		
	}

}

/* Gauss elimination ( M | X )
   based on matrix M */
void gf_gauss_elm2(Gmat *M, Gmat *X)
{
	short i,j,t,tt;
	gf inv,y;
	gf *temp;

	if( M->row != X->row ) { printf("Matrix size error.\n"); exit(-1); }

	for( i=j=0 ; i<M->row && j<M->col ; j++ )
	{
		for( t=i ; t<M->row ; t++ )
		  if( M->dat[t][j] )
			break;

		if( t==M->row ) continue;

		if( t!=i )
		{
			temp = M->dat[i]; M->dat[i] = M->dat[t]; M->dat[t] = temp;
			temp = X->dat[i]; X->dat[i] = X->dat[t]; X->dat[t] = temp; 
		}

		inv = gf_inverse(M->dat[i][j]);
		for( t=0 ; t<M->col ; t++ ) M->dat[i][t] = gf_mul(M->dat[i][t],inv);
		for( t=0 ; t<X->col ; t++ ) X->dat[i][t] = gf_mul(X->dat[i][t],inv);

		for( t=0 ; t<M->row ; t++ )
		{
			if( t==i ) continue;
			y = M->dat[t][j];
			if( y )
			{
				for( tt=0 ; tt<M->col ; tt++ ) M->dat[t][tt] ^= gf_mul(M->dat[i][tt],y);
				for( tt=0 ; tt<X->col ; tt++ ) X->dat[t][tt] ^= gf_mul(X->dat[i][tt],y);
			}
		}

		i++;
	}
}


short mat_rank(const Gmat A)
{
	short i,j,t,tt;
	short nz;
	gf inv,y;
	gf *temp;
	Gmat B;

	/* copy whole matrix */
	B = gen_matrix(A.row,A.col);
	for( i=0 ; i<A.row ; i++ )
	  for( j=0 ; j<A.col ; j++ )
		B.dat[i][j] = A.dat[i][j];
	

	/* transform the matrix to upper triangular */
	for( i=j=0 ; i<B.row && j<B.col ; j++ )
	{
		for( t=i ; t<B.row ; t++ )
		  if( B.dat[t][j] )
			break;

		if( t==B.row ) continue;

		if( t!=i ) { temp = B.dat[i]; B.dat[i] = B.dat[t]; B.dat[t] = temp; }

		inv = gf_inverse(B.dat[i][j]);
		for( t=j ; t<B.col ; t++ ) B.dat[i][t] = gf_mul(B.dat[i][t],inv);

		for( t=i+1 ; t<B.row ; t++ )
		{
			y = B.dat[t][j];
			if( y )
			  for( tt=0 ; tt<B.col ; tt++ )
				B.dat[t][tt] ^= gf_mul(B.dat[i][tt],y);
		}

		i++;
	}


	/* count the number of nonzero rows */
	for( i=B.row-1 ; i>=0 ; i-- )
	{
		/* check that there exists a nonzero entry in i-th row */
		for( j=B.col-1, nz=0 ; j>=0 ; j-- )
			if( B.dat[i][j] ) { nz=1 ; break; }

		if( nz ) break;
	}

	free_matrix(&B);

	return (i+1);
}



/* find the inverse of matrix A
   return 0 if it is not square or inverse does not exist
   return 1 if inverse exists */
short mat_inv(Gmat A, Gmat *X)
{
	short i,j,t,tt;
	short N = A.row;
	gf *temp,inv,y;
	Gmat B;

	/* if it is not square */
	if( A.row != A.col ) return 0;

	/* copy the whole matrix */
	B = gen_matrix(N,N);
	for( i=0 ; i<N ; i++ )
	  for( j=0 ; j<N ; j++ )
		  B.dat[i][j] = A.dat[i][j];


	/* At first we set X = id.
	   it is transformed to inverse of A after Gauss elimination */
	*X = gen_matrix(N,N);
	for( i=0 ; i<N ; i++ )
	  for( j=0 ; j<N ; j++ )
		X->dat[i][j] = 0;

	for( i=0 ; i<N ; i++ )
	  X->dat[i][i] = 1;

	
	/* Gauss elimination */
	for( i=0,j=0 ; i<N && j<N ; j++ )
	{
		for( t=i ; t<N ; t++ )
		  if( B.dat[t][j] )
			break;

		if( t==N ) continue;

		if( t!=i )
		{
			temp = B.dat[i]; B.dat[i] = B.dat[t]; B.dat[t] = temp;
			temp = X->dat[i]; X->dat[i] = X->dat[t]; X->dat[t] = temp;
		}

		inv = gf_inverse(B.dat[i][j]);
		for( t=0 ; t<N ; t++ )
		{
			B.dat[i][t] = gf_mul(B.dat[i][t],inv);
			X->dat[i][t] = gf_mul(X->dat[i][t],inv);
		}

		for( t=0 ; t<N ; t++ )
		{
			if( t==i ) continue;
			y = B.dat[t][j];
			if( y )
			  for( tt=0 ; tt<N ; tt++ )
			  {
				  B.dat[t][tt] ^= gf_mul(B.dat[i][tt],y);
				  X->dat[t][tt] ^= gf_mul(X->dat[i][tt],y);
			  }
		}

		i++;
	}

	/* X is inverse of A only if the result of Gauss elimination is identity */
	if( B.dat[N-1][N-1] != 1 ) 
	{ 
		free_matrix(&B); 
		free_matrix(X);
		return 0; 
	}
	else                       
	{
		free_matrix(&B); 
		return 1; 
	}

}