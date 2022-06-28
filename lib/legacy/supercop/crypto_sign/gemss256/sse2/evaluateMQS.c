#include "evaluateMQS.h"
#include "init.h"
#include "bit.h"

#define LOOPJR(START,NB_IT) \
    for(jr=START;jr<NB_IT;++jr)\
    {\
        xjn=-(xj&1UL);\
        HFEDOTPRODUCTM(tmp,pk_cp,xjn);\
        pk_cp+=NB_WORD_GFqm;\
        xj>>=1;\
    }


/* Input:
    m a vector of n+v elements of GF(2)
    pk a MQ system with m equations in GF(2)[x1,...,x_(n+v)]

  Output:
    c a vector of m elements of GF(2), c is the evaluation of pk in m
*/
void evaluateMQS(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk)
{
    UINT xi, xin, xj, xjn;
    static_vecm_gf2 tmp[NB_WORD_GFqm];
    cst_mqsnv_gf2m pk_cp;
    unsigned int iq,ir,jq,jr;

    pk_cp=pk;

    /* Constant cst_pk */
    HFECOPY_M(c,pk_cp);
    pk_cp+=NB_WORD_GFqm;

    /* for each row of the quadratic matrix of pk, excepted the last block */
    for(iq=0;iq<HFEnvq;++iq)
    {
        xi=m[iq];
        for(ir=0;ir<NB_BITS_UINT;++ir)
        {
            /* for each column of the quadratic matrix of pk */

            /* xj=xi */
            HFECOPY_M(tmp,pk_cp);
            pk_cp+=NB_WORD_GFqm;

            xj=xi>>1;
            LOOPJR(ir+1,NB_BITS_UINT);
            for(jq=iq+1;jq<HFEnvq;++jq)
            {
                xj=m[jq];
                LOOPJR(0,NB_BITS_UINT);
            }
            #if (HFEnvr)
                xj=m[HFEnvq];
                LOOPJR(0,HFEnvr);
            #endif
            /* Multiply by xi */
            xin=-(xi&1UL);
            HFEDOTPRODUCTM(c,tmp,xin);

            xi>>=1;
        }
    }

    /* the last block */
    #if (HFEnvr)
        xi=m[HFEnvq];
        for(ir=0;ir<HFEnvr;++ir)
        {
            /* for each column of the quadratic matrix of pk */

            /* xj=xi */
            HFECOPY_M(tmp,pk_cp);
            pk_cp+=NB_WORD_GFqm;

            xj=xi>>1;
            LOOPJR(ir+1,HFEnvr);

            /* Multiply by xi */
            xin=-(xi&1UL);
            HFEDOTPRODUCTM(c,tmp,xin);

            xi>>=1;
        }
    #endif
}




#define LOOPJR_V(START,NB_IT) \
    for(jr=START;jr<NB_IT;++jr)\
    {\
        xjn=-(xj&1UL);\
        HFEDOTPRODUCTN(tmp,pk_cp,xjn);\
        pk_cp+=NB_WORD_GFqn;\
        xj>>=1;\
    }


/* Input:
    m a vector of v elements of GF(2)
    pk a MQ system with n equations in GF(2)[x1,...,xv]

  Output:
    c a vector of n elements of GF(2), c is the evaluation of pk in m
*/
void evaluateMQSv(vecn_gf2 c, cst_vecv_gf2 m, cst_mqsv_gf2n pk)
{
    UINT xi, xin, xj, xjn;
    static_vecn_gf2 tmp[NB_WORD_GFqn];
    cst_mqsv_gf2n pk_cp;
    unsigned int iq,ir,jq,jr;

    pk_cp=pk;

    /* Constant cst_pk */
    HFECOPY(c,pk_cp);
    pk_cp+=NB_WORD_GFqn;

    /* for each row of the quadratic matrix of pk, excepted the last block */
    for(iq=0;iq<HFEvq;++iq)
    {
        xi=m[iq];
        for(ir=0;ir<NB_BITS_UINT;++ir)
        {
            /* for each column of the quadratic matrix of pk */

            /* xj=xi */
            HFECOPY(tmp,pk_cp);
            pk_cp+=NB_WORD_GFqn;

            xj=xi>>1;
            LOOPJR_V(ir+1,NB_BITS_UINT);
            for(jq=iq+1;jq<HFEvq;++jq)
            {
                xj=m[jq];
                LOOPJR_V(0,NB_BITS_UINT);
            }
            #if (HFEvr)
                xj=m[HFEvq];
                LOOPJR_V(0,HFEvr);
            #endif
            /* Multiply by xi */
            xin=-(xi&1UL);
            HFEDOTPRODUCTN(c,tmp,xin);

            xi>>=1;
        }
    }

    /* the last block */
    #if (HFEvr)
        xi=m[HFEvq];
        for(ir=0;ir<HFEvr;++ir)
        {
            /* for each column of the quadratic matrix of pk */

            /* xj=xi */
            HFECOPY(tmp,pk_cp);
            pk_cp+=NB_WORD_GFqn;

            xj=xi>>1;
            LOOPJR_V(ir+1,HFEvr);

            /* Multiply by xi */
            xin=-(xi&1UL);
            HFEDOTPRODUCTN(c,tmp,xin);

            xi>>=1;
        }
    #endif
}


