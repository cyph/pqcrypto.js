#ifndef _REM_GF2N_H
#define _REM_GF2N_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "tools_gf2n.h"
#include "rem_gf2x.h"
#include "rem5_gf2n.h"


/* Compute the modular reduction */


#define KI (HFEn&63U)
#define KI64 (64U-KI)

#define K3mod128 (K3&127U)
#if (K3>63)
    #define K3mod64 (K3&63U)
#else
    #define K3mod64 K3
#endif

#define K364 (64U-K3mod64)
/* Only to remove the warnings */
#define K364mod64 (K364&63U)




/***********************************************************************/
/***********************************************************************/
/****************** Macro without SIMD version *************************/
/***********************************************************************/
/***********************************************************************/

/* Automatic choice of REM_GF2N */

#if(NB_WORD_GFqn==1)
    #ifdef __TRINOMHFE__
        /* Example: REM64_TRINOM(P,Pol,HFEn,K3mod64,Q,R,HFE_MASKn) */
        #if(K3==1)
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MUL_SUP(REM),_TRINOM_K31(P,Pol,HFEn,K3mod64,Q,R,HFE_MASKn))
        #else
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MUL_SUP(REM),_TRINOM(P,Pol,HFEn,K3mod64,Q,R,HFE_MASKn))
        #endif
    #endif

    #ifdef __PENTANOMHFE__
        #if (HFEn!=64)
            /* Example: REM64_PENTANOM(P,Pol,HFEn,K1,K2,K3mod64,Q,R,HFE_MASKn) */
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MUL_SUP(REM),_PENTANOM(P,Pol,HFEn,K1,K2,K3mod64,Q,R,HFE_MASKn))
        #else
            /* HFEn == 64 */
            #define REM_GF2N(P,Pol,Q,R) REM64_PENTANOM_K64(P,Pol,64,K1,K2,K3mod64,R)
        #endif
    #endif

#elif(NB_WORD_GFqn==2)
    #if (HFEn<97)
        #ifdef __TRINOMHFE__
            #define REM_GF2N(P,Pol,Q,R) REM96_TRINOM(P,Pol,K3mod64,KI,Q,R,HFE_MASKn)
        #endif

        #ifdef __PENTANOMHFE__
            #define REM_GF2N(P,Pol,Q,R) REM96_PENTANOM(P,Pol,K1,K2,K3mod64,KI,Q,R,\
                                                     HFE_MASKn)
        #endif
    #else
        #ifdef __TRINOMHFE__
            #define REM_GF2N(P,Pol,Q,R) REM128_TRINOM(P,Pol,K3mod64,KI,KI64,Q,R,\
                                                    HFE_MASKn)
        #endif

        #ifdef __PENTANOMHFE__
            #if (HFEnr)
                #define REM_GF2N(P,Pol,Q,R) REM128_PENTANOM(P,Pol,K1,K2,K3mod64,KI,\
                                                          KI64,Q,R,HFE_MASKn)
            #else
                /* HFEn == 128 */
                #define REM_GF2N(P,Pol,Q,R) REM128_PENTANOM_K128(P,Pol,K1,K2,K3mod64,R)
            #endif
        #endif
    #endif

#else

    #ifdef __TRINOMHFE__
        #if ((HFEn>256)&&(HFEn<289)&&(K3>32)&&(K3<64))
            #define REM_GF2N(P,Pol,Q,R) REM288_SPECIALIZED_TRINOM(P,Pol,K3,\
                                                    KI,KI64,K364,Q,R,HFE_MASKn)
        #elif (HFEn==313)
            #define REM_GF2N(P,Pol,Q,R) REM320_SPECIALIZED_TRINOM(P,Pol,\
                                  K3mod64,KI,KI64,K364,Q,R,HFE_MASKn)
        #elif (HFEn==354)
            #define REM_GF2N(P,Pol,Q,R) REM384_SPECIALIZED_TRINOM(P,Pol,\
                                  K3mod64,KI,KI64,K364,Q,R,HFE_MASKn)
        #elif (HFEn==358)
            #define REM_GF2N(P,Pol,Q,R) REM384_SPECIALIZED358_TRINOM(P,Pol,\
                                  K3mod64,KI,KI64,K364,Q,R,HFE_MASKn)
        #elif (HFEn==402)
            #define REM_GF2N(P,Pol,Q,R) REM402_SPECIALIZED_TRINOM(P,Pol,\
                                  K3mod64,KI,KI64,K364,Q,R,HFE_MASKn)
        #else
            /* Example: REM192_TRINOM(P,Pol,K3mod64,KI,KI64,K364mod64,Q,R,HFE_MASKn) */
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MUL_SUP(REM),_TRINOM(P,Pol,K3mod64,KI,KI64,K364mod64,Q,R,HFE_MASKn))
        #endif
    #endif


    #ifdef __PENTANOMHFE__
        #if ((HFEn==312)&&(K3==128))
            #define REM_GF2N(P,Pol,Q,R) REM312_PENTANOM_K3_IS_128(P,Pol,K1,K2,,KI,KI64,K164,K264,,Q,R,HFE_MASKn)
        #elif ((HFEn==448)&&(K3==64))
            #define REM_GF2N(P,Pol,Q,R) REM448_PENTANOM_K448_K3_IS_64(P,Pol,K1,K2,,K164,K264,,R)
        #elif ((HFEn==544)&&(K3==128))
            #define REM_GF2N(P,Pol,Q,R) REM544_PENTANOM_K3_IS_128(P,Pol,K1,K2,,KI,KI64,K164,K264,,Q,R,HFE_MASKn)
        #elif (HFEnr)
            /* Example: REM192_PENTANOM(P,Pol,K1,K2,K3mod64,KI,KI64,K164,K264,K364mod64,Q,R,HFE_MASKn) */
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MUL_SUP(REM),_PENTANOM(P,Pol,K1,K2,K3mod64,KI,KI64,K164,K264,K364mod64,Q,R,HFE_MASKn))
        #else
            /* HFEn == NB_WORD_GFqn*64 */
            /* Example: REM192_PENTANOM_K192(P,Pol,K1,K2,K3mod64,K164,K264,K364mod64,R) */
            #define REM_GF2N_TMP CONCAT_NB_BITS_MUL_SUP(CONCAT(CONCAT_NB_BITS_MUL_SUP(REM),_PENTANOM_K))
            #define REM_GF2N(P,Pol,Q,R) REM_GF2N_TMP(P,Pol,K1,K2,K3mod64,K164,K264,K364mod64,R)
        #endif
    #endif
#endif



/***********************************************************************/
/***********************************************************************/
/************************ Without SIMD version *************************/
/***********************************************************************/
/***********************************************************************/


#if (HFEn<33)
    uint64_t PREFIX_NAME(rem_noSIMD_gf2n)(uint64_t Pol);
#else
    void PREFIX_NAME(rem_noSIMD_gf2n)(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL]);
#endif

#define rem_noSIMD_gf2n PREFIX_NAME(rem_noSIMD_gf2n)




#endif
