#ifndef _FROBENIUSMAP_GF2NX_H
#define _FROBENIUSMAP_GF2NX_H

#include "prefix_name.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "gf2nx.h"


unsigned int PREFIX_NAME(frobeniusMap_gf2nx)(gf2nx Xqn, cst_gf2nx F, unsigned int df);
#define frobeniusMap_gf2nx PREFIX_NAME(frobeniusMap_gf2nx)


unsigned int PREFIX_NAME(frobeniusMap_HFE_gf2nx)(gf2nx Xqn, const \
                                    complete_sparse_monic_gf2nx F, cst_gf2n U);
#define frobeniusMap_HFE_gf2nx PREFIX_NAME(frobeniusMap_HFE_gf2nx)



/* Multi-squaring at the power 2^II */
/* A script has been used to find the value of II which minimizes the number 
   of multiplication in GF(2^n). */
#if ((HFEDeg==17)||(HFEDeg==18))
    #if(HFEn<40)
        #define II 2
    #elif(HFEn<112)
        #define II 3 
    #else
        #define II 4
    #endif
#elif ((HFEDeg==33)||(HFEDeg==34))
    #if(HFEn<32)
        #define II 2
    #elif(HFEn<77)
        #define II 3 
    #elif(HFEn<210)
        #define II 4
    #else
        #define II 5
    #endif
#elif ((HFEDeg==129)||(HFEDeg==130))
    #if(HFEn<19)
        #define II 2
    #elif(HFEn<40)
        #define II 3 
    #elif(HFEn<102)
        #define II 4
    #elif(HFEn<259)
        #define II 5
    #else
        #define II 6
    #endif
#elif ((HFEDeg==513)||(HFEDeg==514))
    #if(HFEn<11)
        #define II 2
    #elif(HFEn<27)
        #define II 3 
    #elif(HFEn<41)
        #define II 4
    #elif(HFEn<109)
        #define II 5
    #elif(HFEn<279)
        #define II 6
    #else
        #define II 7
    #endif
/* Old Gui */
#elif ((HFEn==240)&&(HFEDeg==9))
    #define II 3
#elif ((HFEn==241)&&(HFEDeg==9))
    #define II 3
#elif ((HFEn==249)&&(HFEDeg==9))
    #define II 3
#else
    /* Heuristic for the multi-squaring at the power 2^II */
    #if((!HFEDegI)||(HFEDegI==1))
        #define II 1
    #elif (HFEDegI<9)
        #define II (HFEDegI-1)
    #else
        #define II 7
    #endif
#endif

#define POW_II (1UL<<II)

/* First k s.t. k*(2^II) >= D, i.e. k = Ceil(D/2^II) */
#define KP ((HFEDeg>>II)+((HFEDeg%POW_II)?1:0))
/* Number of coefficients k s.t. k*(2^II) >= D and k < D */
#define KX (HFEDeg-KP)


unsigned int PREFIX_NAME(frobeniusMap_multisqr_HFE_gf2nx)(gf2nx Xqn, const \
                                    complete_sparse_monic_gf2nx F, cst_gf2n U);
#define frobeniusMap_multisqr_HFE_gf2nx PREFIX_NAME(frobeniusMap_multisqr_HFE_gf2nx)



/* Heuristic to choose the fastest function */
/* In particular, it is optimal for GeMSS, Gui and DualModeMS parameters */
#if (HFEDeg>=256)
    /* It is the best choice for all n<=576 (or more) */
    #define best_frobeniusMap_HFE_gf2nx frobeniusMap_HFE_gf2nx

#elif (HFEDeg<=34)
    #if(HFEn>=17)
        /* It is the best choice for all n>=17 */
        #define best_frobeniusMap_HFE_gf2nx frobeniusMap_multisqr_HFE_gf2nx
    #else
        /* Not necessarily the best choice for D<33 */
        #define best_frobeniusMap_HFE_gf2nx frobeniusMap_HFE_gf2nx
    #endif

#elif ((HFEDeg==129)||(HFEDeg==130))
    /* Here, we have counted the number of multiplications to choose */
    #if(HFEn<=196)
        #define best_frobeniusMap_HFE_gf2nx frobeniusMap_HFE_gf2nx
    #elif ENABLED_REMOVE_ODD_DEGREE
        #define best_frobeniusMap_HFE_gf2nx frobeniusMap_HFE_gf2nx
    #else
        #define best_frobeniusMap_HFE_gf2nx frobeniusMap_multisqr_HFE_gf2nx
    #endif

#elif (HFEDeg<129)
    /* We need a default choice ... */
    #define best_frobeniusMap_HFE_gf2nx frobeniusMap_multisqr_HFE_gf2nx

#else
    /* We need a default choice ... */
    #define best_frobeniusMap_HFE_gf2nx frobeniusMap_HFE_gf2nx
#endif



#endif
