#include "vecMatProduct.h"
#include "add.h"
#include "bit.h"
#include "init.h"

/* Compute the vector matrix product */



/* for a block of bits of vec */
#define LOOPIR_N(NB_IT) \
    for(ir=0;ir<NB_IT;++ir)\
    {\
        /* multiply the (iq*NB_BITS_UINT)+ir bit of vec \
            by the (iq*NB_BITS_UINT)+ir row of S */\
        vec_ir=-(bit_ir&1);\
        HFEDOTPRODUCTN(res,S_cp,vec_ir);\
        /* next row of S */\
        S_cp+=NB_WORD_GFqn;\
        bit_ir>>=1;\
    }

/* for a block of bits of vec */
#define LOOPIR_NV(NB_IT) \
    for(ir=0;ir<NB_IT;++ir)\
    {\
        /* multiply the (iq*NB_BITS_UINT)+ir bit of vec \
            by the (iq*NB_BITS_UINT)+ir row of S */\
        vec_ir=-(bit_ir&1);\
        HFEDOTPRODUCTNV(res,S_cp,vec_ir);\
        /* next row of S */\
        S_cp+=NB_WORD_GFqnv;\
        bit_ir>>=1;\
    }


#if HFEvr
    #define REM_N \
        bit_ir=vec[HFEvq]; \
        LOOPIR_N(HFEvr);
#else
    #define REM_N
#endif


#if HFEnvr
    #define REM_NV \
        bit_ir=vec[HFEnvq]; \
        LOOPIR_NV(HFEnvr);
#else
    #define REM_NV
#endif


/*  Input:
        vec a vector of length n in GF(2)
        S a matrix n*n in GF(2)

    Output :
        res a vector of length n in GF(2)
        res = dotproduct(v,S) = v.S
*/
#define VECMATPROD(NAME,SET0,LOOPIR,REM,nq) \
void NAME(vecn_gf2 res, cst_vecn_gf2 vec, cst_Mn_gf2 S)\
{\
    cst_Mn_gf2 S_cp;\
    UINT bit_ir, vec_ir;\
    unsigned int iq,ir;\
\
    /* initialization of res to 0 */\
    SET0(res);\
\
    S_cp=S;\
    /* for each bit of vec excepted the last block */\
    for(iq=0;iq<nq;++iq)\
    {\
        bit_ir=vec[iq];\
        LOOPIR(NB_BITS_UINT);\
    }\
    /* the last block */\
    REM;\
}


VECMATPROD(vecMatProductv,HFESET0,LOOPIR_N,REM_N,HFEvq)
VECMATPROD(vecMatProductnv,HFESET0_NV,LOOPIR_NV,REM_NV,HFEnvq)



