#include "randMatrix_gf2.h"
#include "randombytes.h"
#include "determinantn_gf2.h"
#include "determinantnv_gf2.h"

#include <stdlib.h>
#include "init.h"
#include "dotProduct_gf2.h"



/* Input: matrix of size (HFEn*NB_WORD_GFqn) UINT */
/* Output: random matrix n*n in GF(2) */
void PREFIX_NAME(randMatrix_gf2_n)(Mn_gf2 S)
{
    unsigned int i;
    for(i=0;i<HFEn;++i)
    {
        randombytes((unsigned char*)S,NB_BYTES_GFqn);
        S+=NB_WORD_GFqn;
        /* Clean the last word */
        #if HFEnr
            *(S-1)&=HFE_MASKn;
        #endif
    }
}


#if HFEv
/* Input: matrix of size (HFEnv*NB_WORD_GF2nv) UINT */
/* Output: random matrix (n+v)*(n+v) in GF(2) */
void PREFIX_NAME(randMatrix_gf2_nv)(Mnv_gf2 S)
{
    unsigned int i;
    for(i=0;i<HFEnv;++i)
    {
        randombytes((unsigned char*)S,NB_BYTES_GFqnv);
        S+=NB_WORD_GF2nv;
        /* Clean the last word */
        #if HFEnvr
            *(S-1)&=HFE_MASKnv;
        #endif
    }
}
#endif



/* Input: matrix of size (HFEn*NB_WORD_GFqn) UINT */
/* Output: invertible random matrix n*n in GF(2) \
   return ERROR_ALLOC if an allocation fails, the determinant of S else */
int PREFIX_NAME(randInvMatrix_gf2_n)(GLn_gf2 S)
{
    int ret;
    do
    {
        randMatrix_gf2_n(S);
        ret=determinantn_gf2(S);
    } while(!ret);
    return ret;
}


#if HFEv
/* Input: matrix of size (HFEnv*NB_WORD_GF2nv) UINT */
/* Output: invertible random matrix (n+v)*(n+v) in GF(2) \
   return ERROR_ALLOC if an allocation fails, the determinant of S else */
int PREFIX_NAME(randInvMatrix_gf2_nv)(GLnv_gf2 S)
{
    int ret;
    do
    {
        randMatrix_gf2_nv(S);
        ret=determinantnv_gf2(S);
    } while(!ret);
    return ret;
}
#endif







/* Output: lower triangular random matrix with 1 on diagonal */
#define GENLOWMATRIX(NAME,LTRIANGULAR_SIZE,nq,nr) \
void NAME(Tn_gf2 L) \
{\
    UINT mask;\
    unsigned int iq,ir;\
\
    randombytes((unsigned char*)L,LTRIANGULAR_SIZE<<3);\
\
    /* for each row */\
    for(iq=1;iq<=nq;++iq)\
    {\
        mask=0;\
        for(ir=0;ir<NB_BITS_UINT;++ir)\
        {\
            /* Put the bit of diagonal to 1 + zeros after the diagonal */\
            *L&=mask;\
            *L^=1UL<<ir;\
            mask<<=1;\
            ++mask;\
\
            L+=iq;\
        }\
        /* Next column */\
        ++L;\
    }\
\
    /* iq = HFEnq */\
    mask=0;\
    for(ir=0;ir<nr;++ir)\
    {\
        /* Put the bit of diagonal to 1 + zeros after the diagonal */\
        *L&=mask;\
        *L^=1UL<<ir;\
        mask<<=1;\
        ++mask;\
\
        L+=iq;\
    }\
}

GENLOWMATRIX(PREFIX_NAME(genLowerMatrixn),LTRIANGULAR_N_SIZE,HFEnq,HFEnr);
#if HFEv
    GENLOWMATRIX(PREFIX_NAME(genLowerMatrixnv),LTRIANGULAR_NV_SIZE,HFEnvq,
                 HFEnvr);
#endif


#define LOOPJR(NB_IT) \
    mini=(iq<jq)?iq:jq;\
    *S=0UL;\
    for(jr=0;jr<NB_IT;++jr)\
    {\
        /* Dot product */\
        tmp=L_cp[0]&U_cp[0];\
        for(k=1;k<mini;++k)\
        {\
            tmp^=L_cp[k]&U_cp[k];\
        }\
        XORBITS64(tmp);\
        *S^=tmp<<jr;\
        U_cp+=jq;\
    }\
    ++S;


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
/* Output: invertible random matrix n*n in GF(2) */
#define GENRANDINVMATRIXLU(NAME,nq,nr,REM) \
void NAME(GLn_gf2 S, cst_Tn_gf2 L, cst_Tn_gf2 U) \
{\
    cst_Tn_gf2 L_cp,U_cp;\
    UINT tmp;\
    unsigned int iq,ir,jq,jr,k,mini;\
\
    /* Computation of S = L*U */\
    L_cp=L;\
    /* for each row of L (and S) */\
    for(iq=1;iq<=nq;++iq)\
    {\
        LOOPIR(NB_BITS_UINT,nq,nr,REM);\
    }\
    LOOPIR(nr,nq,nr,REM);\
}

#if HFEnr
GENRANDINVMATRIXLU(PREFIX_NAME(randInvMatrixLU_gf2_n),HFEnq,HFEnr,LOOPJR(HFEnr));
#else
GENRANDINVMATRIXLU(PREFIX_NAME(randInvMatrixLU_gf2_n),HFEnq,HFEnr,);
#endif

#if HFEv
    #if HFEnvr
        GENRANDINVMATRIXLU(PREFIX_NAME(randInvMatrixLU_gf2_nv),HFEnvq,HFEnvr,LOOPJR(HFEnvr));
    #else
        GENRANDINVMATRIXLU(PREFIX_NAME(randInvMatrixLU_gf2_nv),HFEnvq,HFEnvr,);
    #endif
#endif


