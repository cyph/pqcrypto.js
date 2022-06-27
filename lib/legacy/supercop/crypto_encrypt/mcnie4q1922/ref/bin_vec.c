#include <stdlib.h>
#include <stdio.h>
#include "bin_vec.h"
#include "rng.h"


/* generate the binary vector of given data  */
Bvec gen_vec(short len, uint32_t *dd)
{
	short i;
	short n = (len + 31) >> 5;      /* the number of uint32 data (len+31)/32 */
	short last = len - ((n-1)<<5);  /* the number of real bits of the last uint32 data */
	uint32_t flag;
	Bvec rst;

	rst.size = len;
	rst.data = (uint32_t *)malloc(n << 2);

	for( i=0 ; i<n ; i++ ) rst.data[i] = dd[i];

	if( last!=32 )
	{
		flag = (1 << last) - 1;
		rst.data[n-1] &= flag;
	}

	return rst;
}

/* generate the binary vector whose components are all zero */
Bvec gen_vec_zero(short len)
{
	short i;
	short n = (len + 31) >> 5;
	Bvec rst;

	rst.size = len;
	rst.data = (uint32_t *)malloc(n << 2);

	for( i=0 ; i<n ; i++ ) rst.data[i] = 0;

	return rst;	
}

/* generate a random binary vector */
Bvec gen_vec_rand(short len)
{
	short n = (len + 31) >> 5;      /* the number of uint32 data (len+31)/32 */
	short last = len - ((n-1)<<5);  /* the number of real bits of the last uint32 data */
	uint32_t flag;
	Bvec rst;

	rst.size = len;
	rst.data = (uint32_t *)malloc(n << 2);

	randombytes((unsigned char *)rst.data,n<<2);

	if( last!=32 )
	{
		flag = (1 << last) - 1;
		rst.data[n-1] &= flag;
	}

	return rst;
}


Bvec gen_vec_copy(Bvec smp)
{
	short i;
	short n = (smp.size + 31) >> 5;
	Bvec rst;

	rst.size = smp.size;
	rst.data = (uint32_t *)malloc(n << 2);

	for( i=0 ; i<n ; i++ )
		rst.data[i] = smp.data[i];

	return rst;
}


/* return 1 if it is zero vector */
short chk_zero(Bvec x)
{
	short i,n,res;
	uint32_t flag;

	n = (x.size + 31) >> 5;       /* the number of blocks */
	res = x.size - ((n-1) << 5);  /* the number of bits of the last block */
	
	if( res==32 ) flag = 0xFFFFFFFF;
	else          flag = (1 << res) - 1;

	for( i=0 ; i<n-1 ; i++ )
	  if( x.data[i] )
		return 0;

	if( x.data[n-1] & flag ) return 0;

	return 1;
}


/* addition x + y  */
Bvec vec_sum(Bvec x, Bvec y)
{
	short i,s1,s2;
	short max = (x.size > y.size) ? x.size : y.size; 
	Bvec rst = gen_vec_zero(max);

	s1 = (x.size + 31) >> 5;
	s2 = (y.size + 31) >> 5;

	if(x.size > y.size)
	{
		for( i=0 ; i<s2 ; i++ ) rst.data[i] = x.data[i] ^ y.data[i];
		for( ; i<s1 ; i++ )     rst.data[i] = x.data[i];
	}
	else
	{
		for( i=0 ; i<s1 ; i++ ) rst.data[i] = x.data[i] ^ y.data[i];
		for( ; i<s2 ; i++ )     rst.data[i] = y.data[i];
	}
	
	return rst;
}

/* addition x += y */
void vec_add(Bvec *x, Bvec y)
{
	short i,s;

	if(x->size < y.size) s = (x->size + 31) >> 5;
	else                 s = (y.size + 31) >> 5;

	for( i=0 ; i<s ; i++ ) x->data[i] ^= y.data[i];
}


uint32_t get_elt(Bvec x, short cur)
{
	uint32_t flag;
	short m,n;

	if( cur<0 || cur>=x.size ) return 0;

	/* quotient and remainder when divided by 32 */
	m = cur >> 5;
	n = cur & 31;

	flag = 1 << n;

	return( x.data[m] & flag );
}


void put_elt(Bvec *x, short cur, uint32_t d)
{
	uint32_t flag;
	short m,n;

	if( cur<0 || cur>=x->size ) return;

	/* quotient and remainder when divided by 32 */
	m = cur >> 5;
	n = cur & 31;

	flag = 1 << n;

	if( d ) x->data[m] |= flag;
	else    x->data[m] &= (flag ^ 0xFFFFFFFF);
}

void print_bin(Bvec x)
{
	uint32_t flag = 1;
	short i,cur = 0;

	if( x.data == NULL ) { printf("0\n"); return; }

	for( i=0 ; i<x.size ; i++ )
	{
		if(x.data[cur] & flag) printf("1");
		else                 printf("0");

		if(flag == 0x80000000) { cur++;  flag=1; }
		else                  flag <<= 1;
	}

	printf("\n");
}


/* x << 1 */
void Shift(Bvec *x)
{
	short i;
	short n = (x->size + 31) >> 5;
	short last = x->size - ((n-1)<<5);
	uint32_t flag;

	flag = 1 << 31;

	for( i=n-1 ; i>=1 ; i-- )
	{
		x->data[i] <<= 1;

		if( x->data[i-1] & flag ) x->data[i] |= 1;
	}

	x->data[0] <<= 1;
}



void vec_swap(Bvec *x, Bvec *y)
{
	short s;
	uint32_t *p;

	s = x->size;
	x->size = y->size;
	y->size = s;

	p = x->data;
	x->data = y->data;
	y->data = p;
}

void gauss_elm(Bvec *M, short row, short col)
{
	short i,j,t;

	for( i=0, j=0 ; i<row && j<col ; j++ )
	{
		/* find a nonzero entry in j-th row */
		for( t=i ; t<row ; t++ )
		  if( get_elt(M[t],j) )
			  break;

		/* If all entries are zero, then we find it in next column */
		if( t==row ) continue; 

		/* if i-th row are all zero, then switch it with nonzero row */
		if( t!=i ) vec_swap(&M[i],&M[t]);

		for( t=0 ; t<row ; t++ )
		{
			if( t==i ) continue;
			if( get_elt(M[t],j) ) vec_add(&M[t],M[i]);
		}

		i++;
	}
}

