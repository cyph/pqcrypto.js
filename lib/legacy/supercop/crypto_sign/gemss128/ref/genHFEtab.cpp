#include "genHFEtab.hpp"
#include <stdlib.h>
#include "randombytes.h"
#include "convNTL.hpp"
#include "init.h"

/* Generate random HFE polynom and random invertible matrix */



/* Input: F of size NB_UINT_HFEPOLY words */
/* Output: random monic HFE polynom, coefficients in GF(2^n) */
void genHFEpolynom(sparse_monic_gf2nx F)
{
    /* Generation of random excepted for the leading term */
    randombytes((unsigned char*)F,NB_UINT_HFEPOLY<<3);

    /* Clean the last word of each element of GF(2^n) */
    #if HFEnr
        sparse_monic_gf2nx F_cp=F-1;
        unsigned int i;
        for(i=0;i<NB_COEFS_HFEPOLY;++i)
        {
            F_cp+=NB_WORD_GFqn;
            *F_cp&=HFE_MASKn;
        }
    #endif

    /* The leading term is 1 and is not stored */
}


/* Input: matrix of size HFEn*HFEn */
/* Output: random invertible matrix n*n in GF(2), and its inverse */
void genRandomInvertibleMatrixn(mat_GF2& S_inv, mat_GF2& S)
{
    unsigned char* S_conv;
    unsigned int i;
    GF2 det;

    S.SetDims(HFEn,HFEn);
    S_inv.SetDims(HFEn,HFEn);

    /* Generation of random invertible matrix + inverse */
    do
    {
        /* Generation of random matrix */
        /* for each row */
        for(i=0;i<HFEn;++i)
        {
            S_conv = (unsigned char*)(S[i].rep.elts());
            /* random row */
            randombytes(S_conv,NB_BYTES_GFqn);
            /* clean the last byte */
            #if HFEnr8
                S_conv[NB_BYTES_GFqn-1]&=HFE_MASKn8;
            #endif
        }
        inv(det,S_inv,S);
    } while(IsZero(det));
}


/* Input: matrix of size HFEnv*HFEnv */
/* Output: random invertible matrix (n+v)*(n+v) in GF(2), and its inverse */
void genRandomInvertibleMatrixnv(mat_GF2& S_inv, mat_GF2& S)
{
    unsigned char* S_conv;
    unsigned int i;
    GF2 det;

    S.SetDims(HFEnv,HFEnv);
    S_inv.SetDims(HFEnv,HFEnv);

    /* Generation of random invertible matrix + inverse */
    do
    {
        /* Generation of random matrix */
        /* for each row */
        for(i=0;i<HFEnv;++i)
        {
            S_conv = (unsigned char*)(S[i].rep.elts());
            /* random row */
            randombytes(S_conv,NB_BYTES_GFqnv);
            /* clean the last byte */
            #if HFEnvr8
                S_conv[NB_BYTES_GFqnv-1]&=HFE_MASKnv8;
            #endif
        }
        inv(det,S_inv,S);
    } while(IsZero(det));
}


/* Output: lower triangular random matrix with 1 on diagonal */
#define GENLOWMATRIX(NAME,LTRIANGULAR_SIZE,nq,nr) \
static void NAME(Tn_gf2 L) \
{\
    Tn_gf2 L_cp=L;\
    UINT mask;\
    unsigned int iq,ir;\
\
    /* >>1 <<3 == <<2 */\
    randombytes((unsigned char*)L,LTRIANGULAR_SIZE<<3);\
\
    /* for each row */\
    for(iq=1;iq<=nq;++iq)\
    {\
        mask=0;\
        for(ir=0;ir<NB_BITS_UINT;++ir)\
        {\
            /* Put the bit of diagonal to 1 + zeros before the diagonal */\
            *L_cp&=mask;\
            *L_cp^=1UL<<ir;\
            mask<<=1;\
            ++mask;\
\
            L_cp+=iq;\
        }\
        /* Next column */\
        ++L_cp;\
    }\
\
    /* iq = HFEnq */\
    mask=0;\
    for(ir=0;ir<nr;++ir)\
    {\
        /* Put the bit of diagonal to 1 + zeros before the diagonal */\
        *L_cp&=mask;\
        *L_cp^=1UL<<ir;\
        mask<<=1;\
        ++mask;\
\
        L_cp+=iq;\
    }\
}

GENLOWMATRIX(genLowerMatrixn,LTRIANGULAR_N_SIZE,HFEnq,HFEnr);
GENLOWMATRIX(genLowerMatrixnv,LTRIANGULAR_NV_SIZE,HFEnvq,HFEnvr);


/* Input: matrix of size HFEn*HFEn in GF(2) */
/* Output: invertible random matrix n*n in GF(2) 
           ERROR_ALLOC if the malloc functions fail 
           else 0 */
#define GENRANDINVMATRIXLU(NAME,LTRIANGULAR_SIZE,genLowerMatrix,convUINTlowerToNTLmat_GF2,n) \
int NAME(mat_GF2& S) \
{\
    Tn_gf2 L_UINT,U_UINT;\
    mat_GF2 L,U;\
\
    S.SetDims(n,n);\
    L.SetDims(n,n);\
    U.SetDims(n,n);\
\
    /* Generation of L and transpose(U) */\
    L_UINT=(UINT*)malloc(LTRIANGULAR_SIZE*sizeof(UINT));\
    VERIFY_ALLOC_RET(L_UINT);\
    U_UINT=(UINT*)malloc(LTRIANGULAR_SIZE*sizeof(UINT));\
    if(!U_UINT)\
    {\
        free(L_UINT);\
        return ERROR_ALLOC;\
    }\
\
    genLowerMatrix(L_UINT);\
    convUINTlowerToNTLmat_GF2(L,L_UINT);\
    free(L_UINT);\
\
    /* Generate the transpose of U */\
    genLowerMatrix(U_UINT);\
    convUINTlowerToNTLmat_GF2(U,U_UINT);\
    free(U_UINT);\
\
    /* L*U */\
    mul(S,L,transpose(U));\
    L.kill();\
    U.kill();\
    return 0;\
}

GENRANDINVMATRIXLU(genRandomInvertibleMatrixLUn,LTRIANGULAR_N_SIZE,genLowerMatrixn,convUINTlowerToNTLmatn_GF2,HFEn);
GENRANDINVMATRIXLU(genRandomInvertibleMatrixLUnv,LTRIANGULAR_NV_SIZE,genLowerMatrixnv,convUINTlowerToNTLmatnv_GF2,HFEnv);



/* Input: element of size NB_WORD_GFqnv UINT */
/* Output: random vector of n+v bits */
void genPlaintext(vecnv_gf2 plain)
{
    randombytes((unsigned char*)plain,NB_BYTES_GFqnv);
    /* Clean the last word */
    #if HFEnvr
        plain[NB_WORD_GFqnv-1]&=HFE_MASKnv;
    #endif
}


