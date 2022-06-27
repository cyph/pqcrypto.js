#include "evalMQS_gf2.h"
#include "tools_gf2m.h"
#include "tools_gf2n.h"
#include "tools_gf2v.h"
#include "sizes_HFE.h"


#define HYBRID_FUNCTIONS 0


/*** These variables define the number of variables and equations ***/
#define NB_VAR (HFEnv)
#define NB_EQ (HFEm)


#if NB_VAR


/*** Useful macro ***/

/* NB_VAR = NB_BITS_UINT*quo + rem */
#define NB_VARq (NB_VAR/NB_BITS_UINT)
#define NB_VARr (NB_VAR%NB_BITS_UINT)


/* NB_EQ = NB_BITS_UINT*quo + rem */
#define NB_EQq (NB_EQ/NB_BITS_UINT)
#define NB_EQr (NB_EQ%NB_BITS_UINT)
/* The number of word that an element of GF(2^NB_EQ) needs */
#if (NB_EQr)
    #define NB_WORD_EQ (NB_EQq+1)
#else
    #define NB_WORD_EQ NB_EQq
#endif


/* To choose macro for NB_WORD_EQ*64 bits */
#if (NB_WORD_EQ == 1)
    #define NB_BITS_EQ_SUP 64
#elif (NB_WORD_EQ == 2)
    #define NB_BITS_EQ_SUP 128
#elif (NB_WORD_EQ == 3)
    #define NB_BITS_EQ_SUP 192
#elif (NB_WORD_EQ == 4)
    #define NB_BITS_EQ_SUP 256
#elif (NB_WORD_EQ == 5)
    #define NB_BITS_EQ_SUP 320
#elif (NB_WORD_EQ == 6)
    #define NB_BITS_EQ_SUP 384
#elif (NB_WORD_EQ == 7)
    #define NB_BITS_EQ_SUP 448
#elif (NB_WORD_EQ == 8)
    #define NB_BITS_EQ_SUP 512
#elif (NB_WORD_EQ == 9)
    #define NB_BITS_EQ_SUP 576
#elif (NB_WORD_EQ == 10)
    #define NB_BITS_EQ_SUP 640
#elif (NB_WORD_EQ == 11)
    #define NB_BITS_EQ_SUP 704
#elif (NB_WORD_EQ == 12)
    #define NB_BITS_EQ_SUP 768
#endif

#define CONCAT_NB_BITS_EQ_SUP(name) CONCAT(name,NB_BITS_EQ_SUP)










/* 64 bits version */

#if (NB_WORD_EQ==1)
    #define XOR_WORD ADD64_TAB2
#elif (NB_WORD_EQ<7)
    #define XOR_WORD CONCAT(CONCAT_NB_BITS_EQ_SUP(ADD),_2)
#else
    #define XOR_WORD(a,b) ADD2(a,b,NB_WORD_EQ);
#endif

#if (NB_WORD_EQ<10)
    #define COPY_64bits_variables CONCAT_NB_BITS_EQ_SUP(COPY)
#else
    #define COPY_64bits_variables(c,a) COPY(c,a,NB_WORD_EQ)
#endif


#if (NB_WORD_EQ<7)
    #define addLoadMask1_gf2EQ CONCAT_NB_BITS_EQ_SUP(ADDLOADMASK1_)
#else
    #define addLoadMask1_gf2EQ(res,a,b) ADDLOADMASK1(res,a,b,NB_WORD_EQ)
#endif





/**************************************************************************/
/**************************************************************************/
/*** Classical constant time evaluation using a mask to multiply by xi. ***/
/**************************************************************************/
/**************************************************************************/


#if NB_VARr
    #define REM_X \
        xi=m[i];\
        for(j=0;j<NB_VARr;++j,++k)\
        {\
            x[k]=-((xi>>j)&1UL);\
        }
#else
    #define REM_X
#endif




/* Input:
    m a vector of n+v elements of GF(2)
    pk a MQ system with m equations in GF(2)[x1,...,x_(n+v)]

  Output:
    c a vector of m elements of GF(2), c is the evaluation of pk in m
*/
void PREFIX_NAME(evalMQS_classical_gf2)(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk)
{
    UINT xi;
    UINT x[NB_VAR];
    static_vecm_gf2 tmp[NB_WORD_EQ];
    unsigned int i,j,k;

    /* Compute one time all -((xi>>1)&1UL) */
    k=0;
    for(i=0;i<NB_VARq;++i)
    {
        xi=m[i];
        for(j=0;j<NB_BITS_UINT;++j,++k)
        {
            x[k]=-((xi>>j)&1UL);
        }
    }
    REM_X;

    /* Constant cst_pk */
    COPY_64bits_variables(c,pk);
    #if HYBRID_FUNCTIONS
        c[NB_EQq]=pk[NB_EQq];
    #endif
    pk+=NB_WORD_EQ;

    /* for each row of the quadratic matrix of pk, excepted the last block */
    for(i=0;i<NB_VAR;++i)
    {
        /* for each column of the quadratic matrix of pk */

        /* xj=xi */
        COPY_64bits_variables(tmp,pk)
        pk+=NB_WORD_EQ;

        for(j=i+1;j<NB_VAR;++j)
        {
            addLoadMask1_gf2EQ(tmp,pk,x[j]);
            pk+=NB_WORD_EQ;
        }
        /* Multiply by xi */
        addLoadMask1_gf2EQ(c,tmp,x[i]);
    }
}



#if (NB_VAR>2)
void PREFIX_NAME(evalMQS_unrolled_gf2)(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk)
{
    UINT xi;
    UINT x[NB_VAR];
    static_vecm_gf2 tmp[NB_WORD_EQ];
    unsigned int i,j,k;

    /* Compute one time all -((xi>>1)&1UL) */
    k=0;
    for(i=0;i<NB_VARq;++i)
    {
        xi=m[i];
        for(j=0;j<NB_BITS_UINT;++j,++k)
        {
            x[k]=-((xi>>j)&1UL);
        }
    }
    REM_X;

    /* Constant cst_pk */
    COPY_64bits_variables(c,pk);
    #if HYBRID_FUNCTIONS
        c[NB_EQq]=pk[NB_EQq];
    #endif
    pk+=NB_WORD_EQ;

    /* for each row of the quadratic matrix of pk, excepted the last block */
    for(i=0;i<NB_VAR;++i)
    {
        /* for each column of the quadratic matrix of pk */

        /* xj=xi */
        COPY_64bits_variables(tmp,pk)
        pk+=NB_WORD_EQ;

        for(j=i+1;j<(NB_VAR-3);j+=4)
        {
            addLoadMask1_gf2EQ(tmp,pk,x[j]);
            pk+=NB_WORD_EQ;
            addLoadMask1_gf2EQ(tmp,pk,x[j+1]);
            pk+=NB_WORD_EQ;
            addLoadMask1_gf2EQ(tmp,pk,x[j+2]);
            pk+=NB_WORD_EQ;
            addLoadMask1_gf2EQ(tmp,pk,x[j+3]);
            pk+=NB_WORD_EQ;
        }
        for(;j<NB_VAR;++j)
        {
            addLoadMask1_gf2EQ(tmp,pk,x[j]);
            pk+=NB_WORD_EQ;
        }
        /* Multiply by xi */
        addLoadMask1_gf2EQ(c,tmp,x[i]);
    }
}
#endif







#endif

