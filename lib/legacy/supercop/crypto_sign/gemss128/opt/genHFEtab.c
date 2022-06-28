#include "genHFEtab.h"
#include <stdlib.h>
#include "randombytes.h"
#include "determinant.h"
#include "determinantnv.h"
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


#if HFEnr
    #define TMP_MACRO(MASKn) \
        *(S_cp-1)&=MASKn;
#else
    #define TMP_MACRO(MASKn)
#endif


/* Input: matrix of size (HFEn*NB_WORD_GFqn) UINT */
/* Output: random matrix n*n in GF(2) */
#define GENRANDMAT(NAME,n,NB_BYTES_n,NB_WORD_n,MASKn) \
static void NAME(Mn_gf2 S)\
{\
    Mn_gf2 S_cp=S;\
    unsigned int i;\
    for(i=0;i<n;++i)\
    {\
        randombytes((unsigned char*)S_cp,NB_BYTES_n);\
        S_cp+=NB_WORD_n;\
        /* Clean the last word */\
        TMP_MACRO(MASKn);\
    }\
}


GENRANDMAT(genRandomMatrixn,HFEn,NB_BYTES_GFqn,NB_WORD_GFqn,HFE_MASKn);
GENRANDMAT(genRandomMatrixnv,HFEnv,NB_BYTES_GFqnv,NB_WORD_GFqnv,HFE_MASKnv);


/* Input: matrix of size (HFEn*NB_WORD_GFqn) UINT */
/* Output: invertible random matrix n*n in GF(2) \
           return ERROR_ALLOC if an allocation fails, the determinant of S else */
int genRandomInvertibleMatrixn(GLn_gf2 S)
{
    int ret;
    do
    {
        genRandomMatrixn(S);
        ret=determinantn(S);
    } while(!ret);
    return ret;
}

int genRandomInvertibleMatrixnv(GLnv_gf2 S)
{
    int ret;
    do
    {
        genRandomMatrixnv(S);
        ret=determinantnv(S);
    } while(!ret);
    return ret;
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


#define LOOPJR(NB_IT) \
    mini=(iq<jq)?iq:jq;\
    *S_cp=0UL;\
    for(jr=0;jr<NB_IT;++jr)\
    {\
        /* Dot product */\
        tmp=L_cp[0]&U_cp[0];\
        for(k=1;k<mini;++k)\
        {\
            tmp^=L_cp[k]&U_cp[k];\
        }\
        XORBITS64(tmp);\
        *S_cp^=tmp<<jr;\
        U_cp+=jq;\
    }\
    ++S_cp;


    #define LOOPIR(NB_IT,nq,nr,REM) \
        for(ir=0;ir<NB_IT;++ir)\
        {\
            U_cp=U;\
            /* for each row of U (multiply by the transpose) */\
            for(jq=1;jq<=nq;++jq)\
            {\
                LOOPJR(NB_BITS_UINT);\
            }\
            REM;\
            L_cp+=iq;\
        }


/* Input: matrix of size (HFEn*NB_WORD_GFqn) UINT */
/* Output: invertible random matrix n*n in GF(2) 
           ERROR_ALLOC if the malloc functions fail 
           else 0 */
#define GENRANDINVMATRIXLU(NAME,LTRIANGULAR_SIZE,genLowerMatrix,nq,nr,REM) \
int NAME(GLn_gf2 S) \
{\
    Tn_gf2 L,L_cp,U,U_cp;\
    GLn_gf2 S_cp=S;\
    UINT tmp;\
    unsigned int iq,ir,jq,jr,k,mini;\
\
    /* Generation of L and U */\
    L=(UINT*)malloc(LTRIANGULAR_SIZE*sizeof(UINT));\
    VERIFY_ALLOC_RET(L);\
    U=(UINT*)malloc(LTRIANGULAR_SIZE*sizeof(UINT));\
    if(!U)\
    {\
        free(L);\
        return ERROR_ALLOC;\
    }\
\
    genLowerMatrix(L);\
    /* Generate the transpose of U */\
    genLowerMatrix(U);\
\
    /* Computation of S = L*U */\
    L_cp=L;\
    /* for each row of L (and S) */\
    for(iq=1;iq<=nq;++iq)\
    {\
        LOOPIR(NB_BITS_UINT,nq,nr,REM);\
    }\
    LOOPIR(nr,nq,nr,REM);\
\
    free(L);\
    free(U);\
    return 0;\
}

#if HFEnr
GENRANDINVMATRIXLU(genRandomInvertibleMatrixnLU,LTRIANGULAR_N_SIZE,genLowerMatrixn,HFEnq,HFEnr,LOOPJR(HFEnr));
#else
GENRANDINVMATRIXLU(genRandomInvertibleMatrixnLU,LTRIANGULAR_N_SIZE,genLowerMatrixn,HFEnq,HFEnr,);
#endif

#if HFEnvr
GENRANDINVMATRIXLU(genRandomInvertibleMatrixnvLU,LTRIANGULAR_NV_SIZE,genLowerMatrixnv,HFEnvq,HFEnvr,LOOPJR(HFEnvr));
#else
GENRANDINVMATRIXLU(genRandomInvertibleMatrixnvLU,LTRIANGULAR_NV_SIZE,genLowerMatrixnv,HFEnvq,HFEnvr,);
#endif


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


