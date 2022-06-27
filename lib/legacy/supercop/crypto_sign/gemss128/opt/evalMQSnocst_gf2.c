#include "evalMQSnocst_gf2.h"
#include "add_gf2x.h"
#include "init.h"
#include "tools_gf2m.h"


/***************************************************************/
/***************************************************************/
/**** Classical variable time evaluation using accumulator. ****/
/***************************************************************/
/***************************************************************/


/*** These variables define the number of variables and equations ***/
#define NB_VAR (HFEnv)
#define NB_EQ (HFEm)



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



/*** Tuning ***/
#define LEN_UNROLLED_64 4





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



#define LOOPJR_NOCST_64(START,NB_IT) \
    for(jr=START;jr<NB_IT;++jr)\
    {\
        if(xj&1UL)\
        {\
            XOR_WORD(c,pk);\
        }\
        pk+=NB_WORD_EQ;\
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
            if(xj&1UL)\
            {\
                XOR_WORD(c,pk);\
            }\
            pk+=NB_WORD_EQ;\
            xj>>=1;\
        }\
    }\
    for(;jr<NB_IT;++jr)\
    {\
        if(xj&1UL)\
        {\
            XOR_WORD(c,pk);\
        }\
        pk+=NB_WORD_EQ;\
        xj>>=1;\
    }

#endif



void PREFIX_NAME(evalMQSnocst_unrolled_gf2)(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk)
{
    UINT xi,xj;
    unsigned int iq,ir,i=NB_VAR,jq,jr;
    #if (LEN_UNROLLED_64!=1)
        unsigned int h;
    #endif

    /* Constant cst_pk */
    COPY_64bits_variables(c,pk);
    pk+=NB_WORD_EQ;

    /* for each row of the quadratic matrix of pk, excepted the last block */
    for(iq=0;iq<NB_VARq;++iq)
    {
        xi=m[iq];
        for(ir=0;ir<NB_BITS_UINT;++ir,--i)
        {
            if(xi&1UL) {

                /* for each column of the quadratic matrix of pk */

                /* xj=xi=1 */
                XOR_WORD(c,pk);
                pk+=NB_WORD_EQ;

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
                pk+=i*NB_WORD_EQ;
            }
            xi>>=1;
        }
    }

    /* the last block */
    #if (NB_VARr)
        xi=m[NB_VARq];
        for(ir=0;ir<NB_VARr;++ir,--i)
        {
            if(xi&1UL) {

                /* for each column of the quadratic matrix of pk */

                /* xj=xi=1 */
                XOR_WORD(c,pk);
                pk+=NB_WORD_EQ;

                xj=xi>>1;
                #if(NB_VARr<(LEN_UNROLLED_64<<1))
                    LOOPJR_NOCST_64(ir+1,NB_VARr);
                #else
                    LOOPJR_UNROLLED_64(ir+1,NB_VARr);
                #endif
            } else
            {
                pk+=i*NB_WORD_EQ;
            }
            xi>>=1;
        }
    #endif
}



