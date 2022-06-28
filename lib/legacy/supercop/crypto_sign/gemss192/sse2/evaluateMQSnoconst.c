#include "evaluateMQSnoconst.h"
#include "init.h"
#include "bit.h"


#define LOOPJR(START,NB_IT) \
    for(jr=START;jr<NB_IT;++jr)\
    {\
        if(xj&1UL)\
        {\
            HFEADD2m(c,pk_cp);\
        }\
        pk_cp+=NB_WORD_GFqm;\
        xj>>=1;\
    }


/* This version of evaluateMQS is not in constant time */

/* Input:
    m a vector of n+v elements of GF(2)
    pk a MQ system with m equations in GF(2)[x1,...,x_(n+v)]

  Output:
    c a vector of m elements of GF(2), c is the evaluation of pk in m
*/
void evaluateMQSnoconst(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk)
{
    UINT xi,xj;
    cst_mqsnv_gf2m pk_cp;
    unsigned int iq,ir,i=HFEnv,jq,jr;

    pk_cp=pk;

    /* Constant cst_pk */
    HFECOPY_M(c,pk_cp);
    pk_cp+=NB_WORD_GFqm;

    /* for each row of the quadratic matrix of pk, excepted the last block */
    for(iq=0;iq<HFEnvq;++iq)
    {
        xi=m[iq];
        for(ir=0;ir<NB_BITS_UINT;++ir,--i)
        {
            if(xi&1UL) {

                /* for each column of the quadratic matrix of pk */

                /* xj=xi=1 */
                HFEADD2m(c,pk_cp);
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
            } else
            {
                pk_cp+=i*NB_WORD_GFqm;
            }
            xi>>=1;
        }
    }

    /* the last block */
    #if (HFEnvr)
        xi=m[HFEnvq];
        for(ir=0;ir<HFEnvr;++ir,--i)
        {
            if(xi&1UL) {

                /* for each column of the quadratic matrix of pk */

                /* xj=xi=1 */
                HFEADD2m(c,pk_cp);
                pk_cp+=NB_WORD_GFqm;

                xj=xi>>1;
                LOOPJR(ir+1,HFEnvr);
            } else
            {
                pk_cp+=i*NB_WORD_GFqm;
            }
            xi>>=1;
        }
    #endif
}


