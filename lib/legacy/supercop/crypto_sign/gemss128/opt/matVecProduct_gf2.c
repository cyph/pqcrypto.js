#include "matVecProduct_gf2.h"
#include "dotProduct_gf2.h"

/* Compute the matrix vector product */



/* for a block of NB_BITS_UINT rows */
#define LOOPIR(NB_IT) \
    /* ir = 0 : initialisation of res[iq] */\
    dotProduct_gf2_n(tmp,T_cp,vec);\
    T_cp+=NB_WORD_GFqn;\
    res[iq]=tmp;\
    for(ir=1;ir<NB_IT;++ir)\
    {\
        /* dot product (iq*NB_BITS_UINT)+ir row of T by vec */\
        dotProduct_gf2_n(tmp,T_cp,vec);\
        /* next row of T */\
        T_cp+=NB_WORD_GFqn;\
        /* xor the computed bit */\
        res[iq]^=tmp<<ir;\
    }



/*  Input:
        T a matrix n*n in GF(2)
        vec a vector of length n in GF(2)

    Output:
        res a vector of length n in GF(2)
        res = dotproduct(T,v) = T.v
*/
void PREFIX_NAME(matVecProduct_gf2)(vecn_gf2 res, cst_Mn_gf2 T, cst_vecn_gf2 vec)
{
    cst_Mn_gf2 T_cp;
    UINT tmp;
    unsigned int iq,ir;

    T_cp=T;
    /* for each row of T excepted the last block */
    for(iq=0;iq<HFEnq;++iq)
    {
        LOOPIR(NB_BITS_UINT);
    }
    /* the last block */
    #if (HFEnr)
        LOOPIR(HFEnr);
    #endif
}

