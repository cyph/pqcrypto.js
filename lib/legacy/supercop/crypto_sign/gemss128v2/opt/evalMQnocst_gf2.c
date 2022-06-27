#include "evalMQnocst_gf2.h"

#include "parameters_HFE.h"
#include "tools_gf2nv.h"
#include "dotProduct_gf2.h"
#include "bit.h"
#include "MQ_gf2.h"


#if (NB_WORD_GF2nv<10)
/**
 * @brief   Variable-time evaluation of a MQ equation in a vector. The equation
 * is uncompressed.
 * @details mq = (c,Q).
 * c is in GF(2).
 * Q is lower triangular of size (n+v)*(n+v) in GF(2).
 * The rows of Q are stored separately (we take new words for each new row).
 * Evaluation: accumulation of x_i * sum p_i,j xj.
 * If x_i==0, the sum is not computed (variable-time optimization).
 * Else, the sum is computed with a dot product of the current row of Q by m.
 * @param[in]   m   A vector of n+v elements in GF(2).
 * @param[in]   mq  A MQ equation in GF(2)[x1,...,x_(n+v)].
 * @return  The evaluation of mq in m, in GF(2).
 * @remark  Variable-time implementation.
 */
UINT PREFIX_NAME(evalMQnocst_unrolled_no_simd_gf2)(const UINT* m,
                                                   const UINT* mq)
{
    UINT acc=0;
    unsigned int i;

    #if ((NB_WORD_GF2nv==1)&&(HFEnvr))
        for(i=0;i<HFEnvr;++i)
    #else
        for(i=0;i<64;++i)
    #endif
        {
            if((m[0]>>i)&UINT_1)
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
            if((m[1]>>i)&UINT_1)
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
            if((m[2]>>i)&UINT_1)
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
            if((m[3]>>i)&UINT_1)
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
            if((m[4]>>i)&UINT_1)
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
            if((m[5]>>i)&UINT_1)
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
            if((m[6]>>i)&UINT_1)
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
            if((m[7]>>i)&UINT_1)
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
            if((m[8]>>i)&UINT_1)
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

    XORBITS_UINT(acc);
    return acc;
}
#endif


