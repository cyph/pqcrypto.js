#ifndef _TYPES_HFE_H
#define _TYPES_HFE_H


/****************** TYPES ************************************/


/* This type stores an element of GF(2). */
typedef unsigned char gf2;

/* This type stores an element of GF(2^n). */
typedef UINT* gf2n;
typedef const UINT* cst_gf2n;
/* to use data[length] */
typedef UINT static_gf2n;

/* This type stores an element of GF(2^m). */
typedef UINT* gf2m;
typedef const UINT* cst_gf2m;

/* This type stores an element of GF(2^n)[X]. Their coefficients are stored in 
lexicographic order. */
typedef UINT* gf2nx;
typedef const UINT* cst_gf2nx;

/* This type stores an element of GF(2^n)[X]. Their coefficients are stored in 
lexicographic order. Only the coefficients which can be non null are stored. 
The leading coefficient in 1 and is not stored. */
typedef UINT* sparse_monic_gf2nx;
typedef const UINT* cst_sparse_monic_gf2nx;


/* This type stores a vector of v elements of GF(2) */
typedef UINT* vecv_gf2;
typedef const UINT* cst_vecv_gf2;
/* to use data[length] */
typedef UINT static_vecv_gf2;

/* This type stores a vector of n elements of GF(2) */
typedef UINT* vecn_gf2;
typedef const UINT* cst_vecn_gf2;
/* to use data[length] */
typedef UINT static_vecn_gf2;

/* This type stores a vector of n+v elements of GF(2) */
typedef UINT* vecnv_gf2;
typedef const UINT* cst_vecnv_gf2;
/* to use data[length] */
typedef UINT static_vecnv_gf2;

/* This type stores a vector of m elements of GF(2) */
typedef UINT* vecm_gf2;
typedef const UINT* cst_vecm_gf2;
/* to use data[length] */
typedef UINT static_vecm_gf2;


/* This type stores a vector of elements of GF(2^n) */
typedef UINT* vec_gf2n;
typedef const UINT* cst_vec_gf2n;

/* This type stores a vector of n elements of GF(2^n) */
typedef UINT* vecn_gf2n;
typedef const UINT* cst_vecn_gf2n;

/* This type stores a matrix n*n in GF(2). */
typedef UINT* Mn_gf2;
typedef const UINT* cst_Mn_gf2;

/* This type stores a matrix (n+v)*(n+v) in GF(2). */
typedef UINT* Mnv_gf2;
typedef const UINT* cst_Mnv_gf2;

/* This type stores a matrix v*n in GF(2). */
typedef UINT* Mv_n_gf2;
typedef const UINT* cst_Mv_n_gf2;

/* This type stores an invertible matrix n*n in GF(2). */
typedef UINT* GLn_gf2;
typedef const UINT* cst_GLn_gf2;

/* This type stores an invertible matrix (n+v)*(n+v) in GF(2). */
typedef UINT* GLnv_gf2;
typedef const UINT* cst_GLnv_gf2;

/* This type stores a triangular matrix n*n in GF(2).*/
typedef UINT* Tn_gf2;
typedef const UINT* cst_Tn_gf2;

/* This type stores a triangular matrix (n+v)*(n+v) in GF(2).*/
typedef UINT* Tnv_gf2;
typedef const UINT* cst_Tnv_gf2;


/* This type stores a multivariate quadratic system with n equations in 
GF(2)[x1,...,xn]. It is stored like one equation in GF(2^n)[x1,...,xn]. */
typedef UINT* mqsn_gf2n;
typedef const UINT* cst_mqsn_gf2n;

/* This type stores a multivariate quadratic system with n equations in 
GF(2)[x1,...,x_(n+v)]. It is stored like one equation in 
GF(2^n)[x1,...,x_(n+v)]. */
typedef UINT* mqsnv_gf2n;
typedef const UINT* cst_mqsnv_gf2n;

/* This type stores a multivariate quadratic system with n equations in 
GF(2)[x1,...,xv]. It is stored like one equation in GF(2^n)[x1,...,xv]. */
typedef UINT* mqsv_gf2n;
typedef const UINT* cst_mqsv_gf2n;


/* This type stores a multivariate quadratic system with m equations in 
GF(2)[x1,...,xn]. It is stored like one equation in GF(2^m)[x1,...,xn]. */
typedef UINT* mqsn_gf2m;
typedef const UINT* cst_mqsn_gf2m;

/* This type stores a multivariate quadratic system with m equations in 
 * GF(2)[x1,...,x_(n+v)]. It is stored like one equation in 
 * GF(2^n)[x1,...,x_(n+v)]. */
typedef UINT* mqsnv_gf2m;
typedef const UINT* cst_mqsnv_gf2m;



#endif
