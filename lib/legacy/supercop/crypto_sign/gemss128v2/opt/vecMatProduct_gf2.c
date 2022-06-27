#include "vecMatProduct_gf2.h"
#include "tools_gf2m.h"
#include "tools_gf2n.h"
#include "tools_gf2nv.h"
#include "tools_gf2v.h"

/* Compute the vector matrix product */


/* for a block of bits of vec */
#define LOOPIR_M(NB_IT) \
    for(ir=0;ir<NB_IT;++ir)\
    {\
        /* multiply the (iq*NB_BITS_UINT)+ir bit of vec
            by the (iq*NB_BITS_UINT)+ir row of S */\
        vec_ir=-(bit_ir&1);\
        xorLoadMask1_gf2m(res,S_cp,vec_ir);\
        /* next row of S */\
        S_cp+=NB_WORD_GFqn;\
        bit_ir>>=1;\
    }

/* for a block of bits of vec */
#define LOOPIR_N(NB_IT) \
    for(ir=0;ir<NB_IT;++ir)\
    {\
        /* multiply the (iq*NB_BITS_UINT)+ir bit of vec
            by the (iq*NB_BITS_UINT)+ir row of S */\
        vec_ir=-(bit_ir&1);\
        xorLoadMask1_gf2n(res,S_cp,vec_ir);\
        /* next row of S */\
        S_cp+=NB_WORD_GFqn;\
        bit_ir>>=1;\
    }

/* for a block of bits of vec */
#define LOOPIR_START_N(NB_IT) \
    for(;ir<NB_IT;++ir)\
    {\
        /* multiply the (iq*NB_BITS_UINT)+ir bit of vec
            by the (iq*NB_BITS_UINT)+ir row of S */\
        vec_ir=-(bit_ir&1);\
        xorLoadMask1_gf2n(res,S_cp,vec_ir);\
        /* next row of S */\
        S_cp+=NB_WORD_GFqn;\
        bit_ir>>=1;\
    }

/* for a block of bits of vec */
#define LOOPIR_NV(NB_IT) \
    for(ir=0;ir<NB_IT;++ir)\
    {\
        /* multiply the (iq*NB_BITS_UINT)+ir bit of vec
            by the (iq*NB_BITS_UINT)+ir row of S */\
        vec_ir=-(bit_ir&1);\
        xorLoadMask1_gf2nv(res,S_cp,vec_ir);\
        /* next row of S */\
        S_cp+=NB_WORD_GF2nv;\
        bit_ir>>=1;\
    }


#if HFEvr
    #define REM_V(LOOPIR) \
        bit_ir=vec[HFEvq]; \
        LOOPIR(HFEvr);
#else
    #define REM_V(LOOPIR)
#endif

#if HFEmr
    #define CLEAN_M (res)[NB_WORD_GF2m-1]&=MASK_GF2m;
#else
    #define CLEAN_M
#endif

#if HFEnr
    #define REM_N(LOOPIR) \
        bit_ir=vec[HFEnq]; \
        LOOPIR(HFEnr);

    #define REM_START_N(LOOPIR) \
        bit_ir=vec[HFEnq]>>ir; \
        LOOPIR(HFEnr);

    #define REM_M(LOOPIR) \
        bit_ir=vec[HFEnq]; \
        LOOPIR(HFEnr); \
        CLEAN_M
#else
    #define REM_N(LOOPIR)
    #define REM_START_N(LOOPIR)
    #define REM_M(LOOPIR) CLEAN_M
#endif

#if HFEnvr
    #define REM_NV(LOOPIR) \
        bit_ir=vec[HFEnvq]; \
        LOOPIR(HFEnvr);

    #define REM_START_NV(LOOPIR) \
        bit_ir=vec[HFEnvq]>>ir; \
        LOOPIR(HFEnvr);
#else
    #define REM_NV(LOOPIR)
    #define REM_START_NV(LOOPIR)
#endif


/*  Input:
        vec a vector of length n in GF(2)
        S a matrix n*n in GF(2)

    Output:
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
    REM(LOOPIR);\
}


VECMATPROD(PREFIX_NAME(vecMatProductnv_64),set0_gf2nv,LOOPIR_NV,REM_NV,HFEnvq)
VECMATPROD(PREFIX_NAME(vecMatProductnvn_64),set0_gf2n,LOOPIR_N,REM_NV,HFEnvq)
VECMATPROD(PREFIX_NAME(vecMatProductv_64),set0_gf2n,LOOPIR_N,REM_V,HFEvq)
VECMATPROD(PREFIX_NAME(vecMatProductn_64),set0_gf2n,LOOPIR_N,REM_N,HFEnq)
VECMATPROD(PREFIX_NAME(vecMatProductm_64),set0_gf2m,LOOPIR_M,REM_M,HFEnq)


#define VECMATPROD_START(NAME,SET0,LOOPIR_START,REM_START,nq) \
void NAME(vecn_gf2 res, cst_vecn_gf2 vec, cst_Mn_gf2 S, unsigned int start)\
{\
    cst_Mn_gf2 S_cp;\
    UINT bit_ir, vec_ir;\
    unsigned int iq,ir;\
\
    /* initialization of res to 0 */\
    SET0(res);\
\
    S_cp=S;\
    ir=start&63U;\
    /* for each bit of vec excepted the last block */\
    for(iq=start>>6;iq<nq;++iq)\
    {\
        bit_ir=vec[iq]>>ir;\
        LOOPIR_START(NB_BITS_UINT);\
        ir=0;\
    }\
    /* the last block */\
    REM_START(LOOPIR_START);\
}


VECMATPROD_START(PREFIX_NAME(vecMatProductnvn_start_64),set0_gf2n,
                 LOOPIR_START_N,REM_START_NV,HFEnvq)
/*
VECMATPROD_START(PREFIX_NAME(vecMatProductn_start_64),set0_gf2n,
                 LOOPIR_START_N,REM_START_N,HFEnq)
*/



