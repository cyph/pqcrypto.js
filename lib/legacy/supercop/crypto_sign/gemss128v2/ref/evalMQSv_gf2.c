#include "evalMQSv_gf2.h"
#include "tools_gf2m.h"
#include "tools_gf2n.h"
#include "tools_gf2v.h"
#include "sizes_HFE.h"

/* This file is a modified copy of evalMQS_gf2.c, used for the constant-time 
   evaluation of the constant of the HFEv polynomial. 
   The main modification is the parameters NB_VAR and NB_EQ */

/* Input:
    m a vector of v elements of GF(2)
    pk a MQ system with n equations in GF(2)[x1,...,xv]

  Output:
    c a vector of n elements of GF(2), c is the evaluation of pk in m
*/
/* void evalMQSv_gf2(vecn_gf2 c, cst_vecv_gf2 m, cst_mqsv_gf2n pk) */


#define HYBRID_FUNCTIONS 0


/*** These variables define the number of variables and equations ***/
#define NB_VAR (HFEv)
#define NB_EQ (HFEn)


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
    #define NB_WORD_EQ_TMP (NB_EQq+1)
#else
    #define NB_WORD_EQ_TMP NB_EQq
#endif


/* To choose macro for NB_WORD_EQ*64 bits */
#if (NB_WORD_EQ_TMP == 1)
    #define NB_WORD_EQ 1
#elif (NB_WORD_EQ_TMP == 2)
    #define NB_WORD_EQ 2
#elif (NB_WORD_EQ_TMP == 3)
    #define NB_WORD_EQ 3
#elif (NB_WORD_EQ_TMP == 4)
    #define NB_WORD_EQ 4
#elif (NB_WORD_EQ_TMP == 5)
    #define NB_WORD_EQ 5
#elif (NB_WORD_EQ_TMP == 6)
    #define NB_WORD_EQ 6
#elif (NB_WORD_EQ_TMP == 7)
    #define NB_WORD_EQ 7
#elif (NB_WORD_EQ_TMP == 8)
    #define NB_WORD_EQ 8
#elif (NB_WORD_EQ_TMP == 9)
    #define NB_WORD_EQ 9
#elif (NB_WORD_EQ_TMP == 10)
    #define NB_WORD_EQ 10
#elif (NB_WORD_EQ_TMP == 11)
    #define NB_WORD_EQ 11
#elif (NB_WORD_EQ_TMP == 12)
    #define NB_WORD_EQ 12
#endif

#define CONCAT_NB_WORD_EQ_SUP(name) CONCAT(name,NB_WORD_EQ)








/* 64-bit version */

#if (NB_WORD_EQ<7)
    #define XOR_ELEM CONCAT(CONCAT_NB_WORD_EQ_SUP(ADD),_2_GF2X)
    #define xorLoadMask1_gf2EQ CONCAT_NB_WORD_EQ_SUP(XORLOADMASK1_)
#else
    #define XOR_ELEM(a,b) ADD_2_GF2X(a,b,NB_WORD_EQ);
    #define xorLoadMask1_gf2EQ(res,a,b) XORLOADMASK1(res,a,b,NB_WORD_EQ)
#endif

#if (NB_WORD_EQ<10)
    #define COPY_64bits_variables CONCAT_NB_WORD_EQ_SUP(COPY)
#else
    #define COPY_64bits_variables(c,a) COPY(c,a,NB_WORD_EQ)
#endif




/**************************************************************************/
/**************************************************************************/
/*** Classical constant-time evaluation using a mask to multiply by xi. ***/
/**************************************************************************/
/**************************************************************************/


#if NB_VARr
    #define REM_X \
        xi=m[i];\
        for(j=0;j<NB_VARr;++j,++k)\
        {\
            x[k]=-((xi>>j)&UINT_1);\
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
void PREFIX_NAME(evalMQSv_classical_gf2)(vecm_gf2 c, cst_vecnv_gf2 m,
                                         cst_mqsnv_gf2m pk)
{
    UINT xi;
    UINT x[NB_VAR];
    static_vecm_gf2 tmp[NB_WORD_EQ];
    unsigned int i,j,k;

    /* Compute one time all -((xi>>1)&UINT_1) */
    k=0;
    for(i=0;i<NB_VARq;++i)
    {
        xi=m[i];
        for(j=0;j<NB_BITS_UINT;++j,++k)
        {
            x[k]=-((xi>>j)&UINT_1);
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
            xorLoadMask1_gf2EQ(tmp,pk,x[j]);
            pk+=NB_WORD_EQ;
        }
        /* Multiply by xi */
        xorLoadMask1_gf2EQ(c,tmp,x[i]);
    }
}







#endif

