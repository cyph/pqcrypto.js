#include "evalMQnocst_gf2.h"

#include "parameters_HFE.h"
#include "tools_gf2nv.h"
#include "dotProduct_gf2.h"
#include "MQ_gf2.h"



#if (NB_WORD_GF2nv<10)
/*  Input: mq, one equation in GF(2)[x1,...,x_(n+v)]
           m, a vector of n+v elements of GF(2)

    Output: the evaluation of mq in m, an element of GF(2)
*/
/* This function is used with the output of convMQ_uncompressed_gf2 */
UINT PREFIX_NAME(evalMQnocst_noSIMD_gf2)(const UINT* m, const UINT* mq)
{
    UINT acc=0UL;
    unsigned int i;

    #if ((NB_WORD_GF2nv==1)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[0]>>i)&1UL)
            {
                acc^=mq[i]&m[0];
            }
        }

    #if (NB_WORD_GF2nv>1)
    mq+=64;
    #if ((NB_WORD_GF2nv==2)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[1]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
            }
            mq+=2;
        }
    #endif

    #if (NB_WORD_GF2nv>2)
    #if ((NB_WORD_GF2nv==3)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[2]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
                acc^=mq[2]&m[2];
            }
            mq+=3;
        }
    #endif

    #if (NB_WORD_GF2nv>3)
    #if ((NB_WORD_GF2nv==4)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[3]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
                acc^=mq[2]&m[2];
                acc^=mq[3]&m[3];
            }
            mq+=4;
        }
    #endif

    #if (NB_WORD_GF2nv>4)
    #if ((NB_WORD_GF2nv==5)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[4]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
                acc^=mq[2]&m[2];
                acc^=mq[3]&m[3];
                acc^=mq[4]&m[4];
            }
            mq+=5;
        }
    #endif

    #if (NB_WORD_GF2nv>5)
    #if ((NB_WORD_GF2nv==6)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[5]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
                acc^=mq[2]&m[2];
                acc^=mq[3]&m[3];
                acc^=mq[4]&m[4];
                acc^=mq[5]&m[5];
            }
            mq+=6;
        }
    #endif

    #if (NB_WORD_GF2nv>6)
    #if ((NB_WORD_GF2nv==7)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[6]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
                acc^=mq[2]&m[2];
                acc^=mq[3]&m[3];
                acc^=mq[4]&m[4];
                acc^=mq[5]&m[5];
                acc^=mq[6]&m[6];
            }
            mq+=7;
        }
    #endif

    #if (NB_WORD_GF2nv>7)
    #if ((NB_WORD_GF2nv==8)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[7]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
                acc^=mq[2]&m[2];
                acc^=mq[3]&m[3];
                acc^=mq[4]&m[4];
                acc^=mq[5]&m[5];
                acc^=mq[6]&m[6];
                acc^=mq[7]&m[7];
            }
            mq+=8;
        }
    #endif

    #if (NB_WORD_GF2nv>8)
    #if ((NB_WORD_GF2nv==9)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[8]>>i)&1UL)
            {
                acc^=mq[0]&m[0];
                acc^=mq[1]&m[1];
                acc^=mq[2]&m[2];
                acc^=mq[3]&m[3];
                acc^=mq[4]&m[4];
                acc^=mq[5]&m[5];
                acc^=mq[6]&m[6];
                acc^=mq[7]&m[7];
                acc^=mq[8]&m[8];
            }
            mq+=9;
        }
    #endif

    XORBITS64(acc);
    return acc;
}
#endif




/* Input: x a vector of n+v elements of GF(2)
          mq = (Q0,Q1,...,Q_(m-1))
          the Qi are lower triangular of size (n+v)*(n+v) in GF(2)
          for each Qi, the rows are stored separately
          (we take new words for each new row)

   Output:
    res a vector of m elements of GF(2), res is the evaluation of mq in x

   This version is not in constant time.
*/
UINT PREFIX_NAME(evalMQnocst_noSIMD2_gf2)(cst_vecnv_gf2 x,const UINT* mq)
{
    UINT res_one_eq=0UL;
    unsigned int iq,ir,j;

    for(iq=0;iq<HFEnvq;++iq)
    {
        for(ir=0;ir<NB_BITS_UINT;++ir)
        {
            if((x[iq]>>ir)&1UL)
            {
                for(j=0;j<=iq;++j)
                {
                    res_one_eq^=x[j]&mq[j];
                }
            }
            mq+=iq+1;
        }
    }

    #if HFEnvr
        for(ir=0;ir<HFEnvr;++ir)
        {
            if((x[iq]>>ir)&1UL)
            {
                for(j=0;j<=iq;++j)
                {
                    res_one_eq^=x[j]&mq[j];
                }
            }
            mq+=iq+1;
        }
    #endif

    XORBITS64(res_one_eq);

    return res_one_eq;
}


/* Input: x a vector of n+v elements of GF(2)
          mq = (c,Q0,Q1,...,Q_(m-1))
          the Qi are lower triangular of size (n+v)*(n+v) in GF(2)
          for each Qi, the rows are stored separately
          (we take new words for each new row)

   Output:
    res a vector of m elements of GF(2), res is the evaluation of mq in x

   This version is not in constant time.
*/
void PREFIX_NAME(evalMQSnocst_noSIMD_gf2_m)(vecm_gf2 res,cst_vecnv_gf2 x,const UINT* mq)
{
    UINT res_one_eq;
    unsigned int k,iq,ir,j;

    /* Constant */
    copy_gf2m(res,mq);
    mq+=NB_WORD_GF2m;

    /* Eval each equation */
    for(k=0;k<HFEm;++k)
    {
        res_one_eq=0UL;
        for(iq=0;iq<HFEnvq;++iq)
        {
            for(ir=0;ir<NB_BITS_UINT;++ir)
            {
                if((x[iq]>>ir)&1UL)
                {
                    for(j=0;j<=iq;++j)
                    {
                        res_one_eq^=x[j]&mq[j];
                    }
                }
                mq+=iq+1;
            }
        }

        #if HFEnvr
            for(ir=0;ir<HFEnvr;++ir)
            {
                if((x[iq]>>ir)&1UL)
                {
                    for(j=0;j<=iq;++j)
                    {
                        res_one_eq^=x[j]&mq[j];
                    }
                }
                mq+=iq+1;
            }
        #endif

        XORBITS64(res_one_eq);
        res[k>>6]^=(res_one_eq<<(k&63));
    }
}


