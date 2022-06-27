#include <stdio.h>
#include <stdlib.h>
#include "gf.h"
#include "rng.h"

/* you can change the value of this parameter */
const short _m = 59;

/* irreducible polynomial over GF(2) */
const gf base_poly[65] = { 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 
0x20000005, /* x^29 + x^2 + 1 */
0x400328AF, /* x^30 + x^17 + x^16 + x^13 + x^11 + x^7 + x^5 + x^3 + x^2 + x + 1 */
0x80000009, /* x^31 + x^3 + 1 */
0, 
0, 
0, 
0x800000CA5, /* x^35 + x^11 + x^10 + x^7 + x^5 + x^2 + 1 */ 
0, 
0x3300000019, /* x^37 + x^36 + x^33 + x^32 + x^4 + x^3 + 1 */
0, 
0,
0, 
0x20000000009, /* x^41 + x^3 + 1 */ 
0,
0x80000000059, /* x^43 + x^6 + x^4 + x^3 + 1 */
0,
0,
0x400002324001, /* x^46 + x^23 + x^21 + x^20 + x^17 + x^14 + 1 */
0x800000000021, /* x^47 + x^5 + 1 */
0,
0,
0x40000380B7755, 
/* x^50 + x^29 + x^28 + x^27 + x^19 + x^17 + x^16 + x^14 + x^13
+ x^12 + x^10 + x^9 + x^8 + x^6 + x^4 + x^2 + 1 */
0x8000000019241, /* x^51 + x^16 + x^15 + x^12 + x^9 + x^6 + 1 */
0,
0x20000000000047, /* x^53 + x^6 + x^2 + x + 1 */
0,
0,
0x100000244486B1D,
/* x^56 + x^33 + x^30 + x^26 + x^22 + x^19 + x^14 + x^13 + x^11 + x^9 
+ x^8 + x^4 + x^3 + x^2 + 1  */
0,
0,
0x80000000000007B, /* x^59 + x^6 + x^5 + x^4 + x^3 + x + 1 */
0,
0x2000000000000027, /* x^61 + x^5 + x^2 + x + 1 */
0x400000017F3F7043,
/* x^62 + x^32 + x^30 + x^29 + x^28 + x^27 + x^26 + x^25 + x^24 + x^21 
+ x^20 + x^19 + x^18 + x^17 + x^16 + x^14 + x^13 + x^12 + x^6 + x + 1 */
0, 
0 
};


/* degree as a polynomial over GF(2)  */
short vec_size(gf x)
{
	short i = _m;
	gf flag = 1ULL << _m;

	for( i=_m ; i>=0 ; i--,flag>>=1 )
	  if( x & flag )
	    break;

	return i;
}


/* addition x + y */
gf gf_sum(gf x, gf y)
{
	return(x^y);
}

/* addition x += y */
void gf_add(gf *x, gf y)
{
	*x ^= y;
}


/* multiplication x * y */
gf gf_mul(gf x, gf y)
{
	gf rst = 0;
	gf max_flag = 1;
	
	max_flag <<= (_m - 1);

	while( y )
	{
		if( y & 1 ) rst ^= x;

		if( x & max_flag )
		{
			x <<= 1;
			x ^= base_poly[_m];
		}
		else
			x <<= 1;

		y >>= 1;
	}

	return rst;
}


void vec_print(gf x)
{
	short i;
	gf flag = 1;

	for( i=0 ; i<_m ; i++, flag<<=1 )
	  if( x & flag ) printf("1");
	  else           printf("0");

	printf("\n");
}

/* division function for Extended Euclidean Algorithm */
gf Div(gf x, gf y, gf *r)
{
	short i;
	short x_size,y_size;
	gf q,flag = 1;

	x_size = vec_size(x);
	y_size = vec_size(y);

	if( x_size < y_size )
	{
		*r = x;
		return 0;
	}

	i = x_size - y_size;
	flag <<= x_size;
	y <<= i;

	q = 0;

	for( ; i>=0 ; i--, flag>>=1, y>>=1 )
   	  if( x & flag )
	  {
		  x ^= y;
		  q |= flag;
	  }

	*r = x;
	q >>= y_size;

	return q;
}


gf gf_inverse(gf x)
{
	gf q,r,f,g;
	gf b,b1,b2;

	if( x==0 || x==1 ) return x;
	
	f = base_poly[_m];
	g = x;

	b2 = 0;
	b1 = 1;
	
	for( ;; )
	{
		q = Div(f,g,&r);
		b = b2 ^ gf_mul(b1,q);  /* b = b2 + b1 * q  */

		if( r==0 || r==1 ) return b;

		f = g;  g = r;
		b2 = b1;  b1 = b;
	}
}


short vec_get_elt(gf x, short cur)
{
	gf flag = 1;

	if( cur<0 || cur>_m ) return 0;

	flag <<= cur;

	if( x & flag ) return 1;
	else           return 0;
}


void vec_put_elt(gf *x, short cur, short d)
{
	gf flag = 1;
	
	if( cur<0 || cur>_m ) return;

	flag <<= cur;

	if( d ) *x |= flag;
	else    *x &= (flag ^ 0xFFFFFFFFFFFFFFFF);
}



void Gauss_elimination(gf *M, short row, short col)
{
	short i,j,t;
	gf temp,flag;

	flag = 1;

	for( i=0,j=0 ; i<row && j<col ; j++, flag<<=1 )
	{
		/* find a nonzero entry in j-th column */
		for( t=i ; t<row ; t++ )
		  if( M[t] & flag )
			break;

		/* If all entries are zero, then we find it in next column */
		if( t==row ) continue;  

		/* if i-th row are all zero, then switch it with nonzero row */
		if( t!=i ) { temp = M[i]; M[i] = M[t]; M[t] = temp; }

		for( t=0 ; t<row ; t++ )
		{
			if( t==i ) continue;
			if( M[t] & flag )
				M[t] ^= M[i];
		}

		i++;
	}
}

/* rank metric of the vector (x1, ..., x_n) */
short bin_rank(gf *x, short n)
{
	short i;
	gf *y;

	/* copy whole vector */
	y = (gf *)malloc(n * sizeof(*y));
	for( i=0 ; i<n ; i++ ) y[i] = x[i];

	Gauss_elimination(y,n,_m);

	for( i=n-1 ; i>=0 ; i-- )
		if( y[i] ) break;

	free(y);

	return (i+1);
}


/* generate a random gf element */
gf rand_gf(void)
{
	unsigned char dat[8];
	gf rst,flag;
	gf *ptr;

	randombytes(dat,8);
	ptr = (gf *)dat;
	rst = *ptr;

	flag = (1ULL << _m) - 1;
	rst &= flag;

	return rst;
}

/* generate random n gf elements */
gf *gen_rand_vec(short n)
{
	short i;
	gf *x,flag;

	flag = (1ULL << _m) - 1;

	x = (gf *)malloc(n * sizeof(*x));

	randombytes((unsigned char *)x,(unsigned long long)n<<3);

	for( i=0 ; i<n ; i++ ) x[i] &= flag;

	return x;
}

/* generate GF(2)-independent random n gf elements */
gf *gen_indep_vec(short n)
{
	short i;
	gf *x,flag;

	flag = (1ULL << _m) - 1;

	x = (gf *)malloc(n * sizeof(*x));

	while( 1 )
	{
		randombytes((unsigned char *)x,(unsigned long long)n<<3);

		for( i=0 ; i<n ; i++ ) x[i] &= flag;

		if(bin_rank(x,n)==n) break;
	}

	return x;
}


/* convert gf elements to binary data
   xlen : the number of gf elements
   dlen : the number of uint64 */
uint64 *gf2bin(const gf *x, const long xlen, long *dlen)
{
	long i,j;
	short res;
	uint64 flag,one = 1;
	uint64 *d;
	gf y;

	*dlen = (_m * xlen - 1) / 64 + 1;
	d = (uint64 *)malloc(*dlen * sizeof(*d));
	for( i=0 ; i<*dlen ; i++ ) d[i] = 0;

	res = 64;

	for( i=0,j=0 ; i<xlen ; i++ )
	{
		y = x[i];

		if( res >= _m )
		{
			d[j] |= y << (64 - res);
			res -= _m;
			
			if(res==0) { res = 64; j++; }
		}
		else
		{
			flag = (one << res) - one;

			d[j] |= (y & flag) << (64-res);
			j++;
			d[j] = y >> res;

			res += 64 - _m;
		}
	}

	return d;
}



/* read gf elements from given data 
   xlen : the number of gf elements */
gf *bin2gf(const unsigned char *d, long xlen)
{
	long i,j;
	short res;
	gf *x;
	gf a,flag;

	x = (gf *)malloc(xlen * sizeof(*x));
	for( i=0 ; i<xlen ; i++ ) x[i] = 0;


	x[0] = (gf)d[0];
	res = _m - 8;

	for( i=0, j=1 ; i<xlen ; j++ )
	  if( res > 8 )
	  {
		  a = (gf)d[j];
		  x[i] |= a << (_m - res);
		  res -= 8;
	  }
	  else
	  {
		  a = (gf)d[j];
		  flag = (1ULL << res) - 1;
		  x[i++] |= (a & flag) << (_m - res);

		  if( i >= xlen ) break;

		  x[i] = a >> res;
		  res = _m - (8 - res);
	  }

	return x;
}



