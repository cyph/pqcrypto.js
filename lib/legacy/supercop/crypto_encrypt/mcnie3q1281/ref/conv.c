#include <stdlib.h>
#include "gf.h"
#include "conv.h"

extern short _m;

static const unsigned char sbox[256] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const unsigned char sboxinv[256] =
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};


/* conversion function from binary data to error vectors with rank r */
void conv(const unsigned char *y, short n, short rank, gf *out_gf)
{
	short i,j;
	short pos,cur;
	unsigned char *y2;  /* data applying S-box */
	unsigned char flag;
	gf *D,*E;
	short nr = n - rank;
	short len;

	/* necessary bits to construct error vector */
	len = rank*(rank-1)/2 + rank*(n-rank) + rank*(_m-rank);  
	len >>= 3;  /* bytes */

	/* S-box diffusion */
	y2 = (unsigned char *)malloc(len+1);
	for( i=0 ; i<len ; i++ ) y2[i] = sbox[y[i]];

	y2[len] = y[0] ^ y[len-1];   /* last byte for remaining bits */
	

	D = (gf *)malloc(n * sizeof(*D));  
	for( i=0 ; i<rank ; i++ ) D[i] = 1ULL << i;
	for( ; i<n ; i++ ) D[i] = 0;

	E = (gf *)malloc(nr * sizeof(*E));
	for( i=0 ; i<nr ; i++ ) E[i] = 0;

	pos = cur = 0;  flag = 1;

	/* D imbedding from given data */
	for( i=0 ; i<rank ; i++ )
	  for( j=i+1 ; j<_m ; j++ )
	  {
		  if( y2[pos] & flag ) vec_put_elt(&D[i],j,1);

		  cur++;  flag<<=1;

		  if( cur==8 )
		  {
			  cur = 0;  flag = 1;
			  pos++;
		  }
	  }

	/* E2 imbedding from given data */
	for( i=0 ; i<nr ; i++ )
	  for( j=0 ; j<rank ; j++ )
	  {
		  if( y2[pos] & flag ) vec_put_elt(&E[i],j,1);

		  cur++;  flag<<=1;

		  if( cur==8 )
		  {
			  cur = 0;  flag = 1;
			  pos++;
		  }
	  }

	  /* matrix multiplication */
	  for( i=0 ; i<nr ; i++ )
		for( j=0 ; j<rank ; j++ )
		  if( vec_get_elt(E[i],j) )
			D[i+rank] ^= D[j];

	  /* copy gf elements */
	  for( i=0 ; i<n ; i++ ) out_gf[i] = D[i];

	  /* delete allocated memory */
	  free(y2); free(E); free(D);	  
}


/* inverse function of the conversion function */
void inv_conv(const gf *er, short n, short rank, unsigned char *out_data)
{
	short i,j;
	short nr = n - rank;
	short pos,cur;
	unsigned char flag8;

	gf *inv_mat,*E,*E2;
	gf flag_rank = (1ULL << rank) - 1;
	gf a = 1ULL << rank;

	/* ( D | id ) */
	inv_mat = (gf *)malloc(rank * sizeof(*inv_mat));
	for( i=0 ; i<rank ; i++ ) 
		inv_mat[i] = (er[i] & flag_rank) | (a << i);	

	/* gauss elimination (only for upper triangular) */
	for( j=1 ; j<rank ; j++ )
	  for( i=0 ; i<j ; i++ )
		if( vec_get_elt(inv_mat[i],j) ) inv_mat[i] ^= inv_mat[j];

	/* inv_mat = inverse of the matrix D */
	for( i=0 ; i<rank ; i++ ) inv_mat[i] >>= rank;


	/* load E2 * D data and recover E2 */
	E = (gf *)malloc(nr * sizeof(*E));
	for( i=0 ; i<nr ; i++ ) E[i] = er[rank+i] & flag_rank;

	E2 = (gf *)malloc(nr * sizeof(*E2));
	for( i=0 ; i<nr ; i++ )
	  for( j=0, E2[i]=0 ; j<rank ; j++ )
		if( vec_get_elt(E[i],j) )
		  E2[i] ^= inv_mat[j];


	pos = cur = 0;  flag8 = 1;
	out_data[0] = 0;

	/* imbedding from D */
	for( i=0 ; i<rank ; i++ )
	  for( j=i+1 ; j<_m ; j++ )
	  {
		  if( vec_get_elt(er[i],j) ) out_data[pos] |= flag8;

		  cur++;  flag8<<=1;

		  if( cur==8 )
		  {
			  cur = 0;  flag8 = 1;
			  pos++;  out_data[pos] = 0;
		  }
	  }


	/* imbedding from E2 */
	for( i=0 ; i<nr ; i++ )
	  for( j=0 ; j<rank ; j++ )
	  {
		  if( vec_get_elt(E2[i],j) ) out_data[pos] |= flag8;

		  cur++;  flag8<<=1; 

		  if( cur==8 )
		  {
			  cur = 0;  flag8 = 1;
			  pos++;  out_data[pos] = 0;
		  }
	  }

	/* recover S-box diffusion */
	for( i=0 ; i<=pos ; i++ )
		out_data[i] = sboxinv[out_data[i]];

	free(E); free(E2);
	free(inv_mat);	
}