#include "evalMQSnocst8_gf2.h"
#include "add_gf2x.h"
#include "init.h"
#include "simd.h"
#include "tools_gf2m.h"

/* This file is a modified copy of evalMQSnocst8_gf2.c, used for the hybrid 
   evaluation of the public-key. */


/***************************************************************/
/***************************************************************/
/**** Classical variable-time evaluation using accumulator. ****/
/***************************************************************/
/***************************************************************/


/* NB_EQ = 8*quo + rem */
#define NB_EQ_orig (HFEm)
#define NB_EQq_orig (NB_EQ_orig>>3)
#define NB_EQr_orig (NB_EQ_orig&7)


/*** These variables define the number of variables and equations ***/
#define NB_VAR (HFEnv)
/* MODIFICATION: This row is modified */
#if NB_EQq_orig
    #define NB_EQ (NB_EQq_orig<<3)
#else
    #define NB_EQ NB_EQ_orig
#endif



/*** Useful macro ***/

/* NB_VAR = NB_BITS_UINT*quo + rem */
#define NB_VARq (NB_VAR/NB_BITS_UINT)
#define NB_VARr (NB_VAR%NB_BITS_UINT)


/* NB_EQ = 8*quo + rem */
#define NB_EQq (NB_EQ>>3)
#define NB_EQr (NB_EQ&7)
/* The number of bytes that an element of GF(2^NB_EQ) needs */
#if (NB_EQr)
    #define NB_BYTES_EQ (NB_EQq+1)
#else
    #define NB_BYTES_EQ NB_EQq
#endif

#define MASK_EQ mask64(NB_EQ&63)
#if (NB_EQ&63)
    #define MASK_64(c) (c)&=MASK_EQ;
    #define MASK2_64(c,a) (c)=(a)&MASK_EQ;
#else
    /* The AND is useless here */
    #define MASK_64(c)
    #define MASK2_64(c,a)
#endif


/* To choose macro for NB_WORD_EQ*64 bits */
#if (NB_BYTES_EQ<=8)
    #define NB_WORD_EQ 1
#elif (NB_BYTES_EQ<=16)
    #define NB_WORD_EQ 2
#elif (NB_BYTES_EQ<=24)
    #define NB_WORD_EQ 3
#elif (NB_BYTES_EQ<=32)
    #define NB_WORD_EQ 4
#elif (NB_BYTES_EQ<=40)
    #define NB_WORD_EQ 5
#elif (NB_BYTES_EQ<=48)
    #define NB_WORD_EQ 6
#elif (NB_BYTES_EQ<=56)
    #define NB_WORD_EQ 7
#elif (NB_BYTES_EQ<=64)
    #define NB_WORD_EQ 8
#elif (NB_BYTES_EQ<=72)
    #define NB_WORD_EQ 9
#elif (NB_BYTES_EQ<=80)
    #define NB_WORD_EQ 10
#elif (NB_BYTES_EQ<=88)
    #define NB_WORD_EQ 11
#elif (NB_BYTES_EQ<=96)
    #define NB_WORD_EQ 12
#endif

#define CONCAT_NB_WORD_EQ_SUP(name) CONCAT(name,NB_WORD_EQ)



/*** Tuning ***/
#define LEN_UNROLLED_64 4







/* 64-bit version */

#if (NB_WORD_EQ<7)
    #define XOR_ELEM CONCAT(CONCAT_NB_WORD_EQ_SUP(ADD),_2_GF2X)
#else
    #define XOR_ELEM(a,b) ADD_2_GF2X(a,b,NB_WORD_EQ);
#endif

#if (NB_WORD_EQ<10)
    #define COPY_64bits_variables CONCAT_NB_WORD_EQ_SUP(COPY)
#else
    #define COPY_64bits_variables(c,a) COPY(c,a,NB_WORD_EQ)
#endif



#define LOOPJR_NOCST_64(START,NB_IT) \
    for(jr=START;jr<NB_IT;++jr)\
    {\
        if(xj&UINT_1)\
        {\
            XOR_ELEM(c,(const UINT*)pk);\
        }\
        pk+=NB_BYTES_EQ;\
        xj>>=1;\
    }



#if (LEN_UNROLLED_64==1)
    #define LOOPJR_UNROLLED_64 LOOPJR_NOCST_64
#else

#define LOOPJR_UNROLLED_64(START,NB_IT) \
    for(jr=START;jr<(NB_IT-LEN_UNROLLED_64+1);jr+=LEN_UNROLLED_64)\
    {\
        for(h=0;h<LEN_UNROLLED_64;++h)\
        {\
            if(xj&UINT_1)\
            {\
                XOR_ELEM(c,(const UINT*)pk);\
            }\
            pk+=NB_BYTES_EQ;\
            xj>>=1;\
        }\
    }\
    for(;jr<NB_IT;++jr)\
    {\
        if(xj&UINT_1)\
        {\
            XOR_ELEM(c,(const UINT*)pk);\
        }\
        pk+=NB_BYTES_EQ;\
        xj>>=1;\
    }

#endif



/* Requirement: 
            to increase the size of pk by (8-(NB_BYTES_EQ&7))&7 bytes */
void PREFIX_NAME(evalMQSnocst8_unrolled_quo_gf2)(vecm_gf2 c, cst_vecnv_gf2 m,
                                            cst_mqsnv8_gf2m pk)
{
    UINT xi,xj;
    unsigned int iq,ir,i=NB_VAR,jq,jr;
    #if (LEN_UNROLLED_64!=1)
        unsigned int h;
    #endif

    /* Constant cst_pk */
    COPY_64bits_variables(c,(const UINT*)pk);
    pk+=NB_BYTES_EQ;

    /* for each row of the quadratic matrix of pk, excepted the last block */
    for(iq=0;iq<NB_VARq;++iq)
    {
        xi=m[iq];
        for(ir=0;ir<NB_BITS_UINT;++ir,--i)
        {
            if(xi&UINT_1) {

                /* for each column of the quadratic matrix of pk */

                /* xj=xi=1 */
                XOR_ELEM(c,(const UINT*)pk);
                pk+=NB_BYTES_EQ;

                xj=xi>>1;
                LOOPJR_UNROLLED_64(ir+1,NB_BITS_UINT);
                for(jq=iq+1;jq<NB_VARq;++jq)
                {
                    xj=m[jq];
                    LOOPJR_UNROLLED_64(0,NB_BITS_UINT);
                }
                #if (NB_VARr)
                    xj=m[NB_VARq];
                    #if(NB_VARr<(LEN_UNROLLED_64<<1))
                        LOOPJR_NOCST_64(0,NB_VARr);
                    #else
                        LOOPJR_UNROLLED_64(0,NB_VARr);
                    #endif
                #endif
            } else
            {
                pk+=i*NB_BYTES_EQ;
            }
            xi>>=1;
        }
    }

    /* the last block */
    #if (NB_VARr)
        xi=m[NB_VARq];
        for(ir=0;ir<NB_VARr;++ir,--i)
        {
            if(xi&UINT_1) {

                /* for each column of the quadratic matrix of pk */

                /* xj=xi=1 */
                XOR_ELEM(c,(const UINT*)pk);
                pk+=NB_BYTES_EQ;

                xj=xi>>1;
                #if(NB_VARr<(LEN_UNROLLED_64<<1))
                    LOOPJR_NOCST_64(ir+1,NB_VARr);
                #else
                    LOOPJR_UNROLLED_64(ir+1,NB_VARr);
                #endif
            } else
            {
                pk+=i*NB_BYTES_EQ;
            }
            xi>>=1;
        }
    #endif

    MASK_64(c[NB_WORD_EQ-1]);
}



