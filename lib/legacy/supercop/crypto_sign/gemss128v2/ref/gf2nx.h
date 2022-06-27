#ifndef _GF2NX_H
#define _GF2NX_H


#include "arch.h"
#include "parameters_HFE.h"
#include "MQS_gf2.h"
#include "matrix_gf2n.h"


/* This type stores an element of GF(2^n)[X]. Their coefficients are stored in 
   the lexicographic order. */
typedef UINT* gf2nx;
typedef const UINT* cst_gf2nx;
/* to use data[length]. */
typedef UINT static_gf2nx;


/* Version with a structure */
typedef struct _poly_gf2nx
{
    /* Vector of coefficients stored in the lexicographic order */
    vec_gf2n coefs;
    /* deg+1 coefficients */
    unsigned int deg;
} poly_gf2nx;


/* When a polynomial is stored with a sparse representation */
typedef UINT* sparse_gf2nx;
typedef const UINT* cst_sparse_gf2nx;


/* This type stores an element of GF(2^n)[X]. Their coefficients are stored in 
the lexicographic order. Only the coefficients which can be non null are stored. 
The leading coefficient in 1 and is not stored. */
typedef UINT* sparse_monic_gf2nx;
typedef const UINT* cst_sparse_monic_gf2nx;


/* This type stores a vector in GF(2^n)[X]. */
typedef UINT* vec_gf2nx;
typedef const UINT* cst_vec_gf2nx;


/****************** OPTION TO MODIFY THE HFE POLYNOMIAL ******************/


/* Remark: if HFEDeg is even, we can also optimize for s=0 */
#if (HFEs || (!(HFEDeg&1U)))
    /* Set to 1 to remove terms which have an odd degree strictly greater than
       HFE_odd_degree */
    #define ENABLED_REMOVE_ODD_DEGREE 1
#else
    #define ENABLED_REMOVE_ODD_DEGREE 0
#endif


#if ENABLED_REMOVE_ODD_DEGREE
    /* HFE_odd_degree = 1 + 2^LOG_odd_degree */
    #define LOG_odd_degree (HFEDegI-HFEs)
    #define HFE_odd_degree ((1U<<(LOG_odd_degree))+1)

    #if (HFEDeg&1)
        #error "HFEDeg is odd, so to remove the leading term would decrease \
                the degree."
    #endif

    #if (HFE_odd_degree>HFEDeg)
        #error "It is useless to remove 0 term."
    #endif

    #if (HFE_odd_degree<=1)
        #error "The case where the term X^3 is removing is not implemented."
    #endif
#endif


/****************** HFE POLYNOMIAL AND SPARSE REPRESENTATION ******************/


/* Number of stored coefficients of a HFEv polynomial */
/* Remark: when the vinegar variables of a HFEv polynomial are evaluated,
           we obtain a HFE polynomial. */
#if (HFEDeg==1)
    /* Do not store the leading term <=> only constant coefficient is stored */
    #define NB_COEFS_HFEPOLY 1U
    #define NB_COEFS_HFEVPOLY NB_MONOMIAL_VINEGAR
#else
    #if (GFq==2)
        /* Set i=HFEDegI and j=HFEDegJ, the number of non zero monomials is:
           1 (X^0) + 1 (X^1) + i(i+1)/2 (X^2 to X^(2*2^(i-1))) + 
                                  (j+1) (X^(2^i + 2^0) to X^(2^i + 2^j))
                                     -1 (leading term == 1 is not stored) */
        #if ENABLED_REMOVE_ODD_DEGREE
            #define NB_COEFS_HFEPOLY \
                        (2+HFEDegJ+((HFEDegI*(HFEDegI-1))>>1)+LOG_odd_degree)
        #else
            #define NB_COEFS_HFEPOLY \
                        (2+HFEDegJ+((HFEDegI*(HFEDegI+1))>>1))
        #endif

        #define NB_COEFS_HFEVPOLY \
                    (NB_COEFS_HFEPOLY+(NB_MONOMIAL_VINEGAR-1)+(HFEDegI+1)*HFEv)
    #else
        /* Not implemented */
    #endif
#endif


#define NB_UINT_HFEVPOLY (NB_COEFS_HFEVPOLY*NB_WORD_GFqn)
#define NB_UINT_HFEPOLY (NB_COEFS_HFEPOLY*NB_WORD_GFqn)


/* Allocation */
#define MALLOC_HFEPOLY (UINT*)malloc(NB_UINT_HFEPOLY*sizeof(UINT));
#define MALLOC_HFEVPOLY (UINT*)malloc(NB_UINT_HFEVPOLY*sizeof(UINT));


/* A structure with a special list to find the exponents of the monomials */
typedef struct {
    cst_sparse_monic_gf2nx poly;
    /* List of the successive differences of the exponents of the monomials of
       poly multiplied by NB_WORD_GFqn */
    unsigned int L[NB_COEFS_HFEPOLY];
} complete_sparse_monic_gf2nx;


#endif

