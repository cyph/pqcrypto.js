#ifndef _REM_GF2N_H
#define _REM_GF2N_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "gf2x.h"
#include "tools_gf2n.h"
#include "macro.h"
#include "rem_gf2x.h"
#include "rem5_gf2n.h"


/* Compute the modular reduction */


#define KI (HFEn&63U)
#define KI64 (64U-KI)

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
    #ifdef __TRINOMIAL_GF2N__
        /* Example: REM64_TRINOMIAL_GF2X(P,Pol,HFEn,K3mod64,Q,R,MASK_GF2n) */
        #if(K3==1)
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MMUL_SUP(REM),\
_TRINOMIAL_K31_GF2X(P,Pol,HFEn,K3mod64,Q,R,MASK_GF2n))
        #else
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MMUL_SUP(REM),\
_TRINOMIAL_GF2X(P,Pol,HFEn,K3mod64,Q,R,MASK_GF2n))
        #endif
    #endif

    #ifdef __PENTANOMIAL_GF2N__
        #if (HFEn!=64)
            /* Example: REM64_PENTANOMIAL_GF2X(P,Pol,HFEn,K1,K2,K3mod64,
                                               Q,R,MASK_GF2n) */
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MMUL_SUP(REM),\
_PENTANOMIAL_GF2X(P,Pol,HFEn,K1,K2,K3mod64,Q,R,MASK_GF2n))
        #else
            /* HFEn == 64 */
            #define REM_GF2N(P,Pol,Q,R) \
                REM64_PENTANOMIAL_K64_GF2X(P,Pol,64,K1,K2,K3mod64,R)
        #endif
    #endif

#elif(NB_WORD_GFqn==2)
    #if (HFEn<97)
        #ifdef __TRINOMIAL_GF2N__
            #define REM_GF2N(P,Pol,Q,R) REM96_TRINOMIAL_GF2X(P,Pol,\
K3mod64,KI,Q,R,MASK_GF2n)
        #endif

        #ifdef __PENTANOMIAL_GF2N__
            #define REM_GF2N(P,Pol,Q,R) REM96_PENTANOMIAL_GF2X(P,Pol,\
K1,K2,K3mod64,KI,Q,R,MASK_GF2n)
        #endif
    #else
        #ifdef __TRINOMIAL_GF2N__
            #define REM_GF2N(P,Pol,Q,R) REM128_TRINOMIAL_GF2X(P,Pol,\
K3mod64,KI,KI64,Q,R,MASK_GF2n)
        #endif

        #ifdef __PENTANOMIAL_GF2N__
            #if (HFEnr)
                #define REM_GF2N(P,Pol,Q,R) REM128_PENTANOMIAL_GF2X(P,Pol,\
K1,K2,K3mod64,KI,KI64,Q,R,MASK_GF2n)
            #else
                /* HFEn == 128 */
                #define REM_GF2N(P,Pol,Q,R) \
                    REM128_PENTANOMIAL_K128_GF2X(P,Pol,K1,K2,K3mod64,R)
            #endif
        #endif
    #endif

#else
    #ifdef __TRINOMIAL_GF2N__
        #if ((HFEn>256)&&(HFEn<289)&&(K3>32)&&(K3<64))
            #define REM_GF2N(P,Pol,Q,R) REM288_SPECIALIZED_TRINOMIAL_GF2X(P,Pol\
,K3,KI,KI64,K364,Q,R,MASK_GF2n)
        #elif (HFEn==313)
            #define REM_GF2N(P,Pol,Q,R) REM320_SPECIALIZED_TRINOMIAL_GF2X(\
P,Pol,K3mod64,KI,KI64,K364,Q,R,MASK_GF2n)
        #elif (HFEn==354)
            #define REM_GF2N(P,Pol,Q,R) REM384_SPECIALIZED_TRINOMIAL_GF2X(\
P,Pol,K3mod64,KI,KI64,K364,Q,R,MASK_GF2n)
        #elif (HFEn==358)
            #define REM_GF2N(P,Pol,Q,R) REM384_SPECIALIZED358_TRINOMIAL_GF2X(\
P,Pol,K3mod64,KI,KI64,K364,Q,R,MASK_GF2n)
        #elif (HFEn==402)
            #define REM_GF2N(P,Pol,Q,R) REM402_SPECIALIZED_TRINOMIAL_GF2X(\
P,Pol,K3mod64,KI,KI64,K364,Q,R,MASK_GF2n)
        #else
            /* Example: REM192_TRINOMIAL_GF2X(P,Pol,K3mod64,KI,KI64,K364mod64,
                                              Q,R,MASK_GF2n) */
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MMUL_SUP(REM),\
_TRINOMIAL_GF2X(P,Pol,K3mod64,KI,KI64,K364mod64,Q,R,MASK_GF2n))
        #endif
    #endif

    #ifdef __PENTANOMIAL_GF2N__
        #if ((HFEn==312)&&(K3==128))
            #define REM_GF2N(P,Pol,Q,R) REM312_PENTANOMIAL_K3_IS_128_GF2X(\
P,Pol,K1,K2,,KI,KI64,K164,K264,,Q,R,MASK_GF2n)
        #elif ((HFEn==448)&&(K3==64))
            #define REM_GF2N(P,Pol,Q,R) REM448_PENTANOMIAL_K448_K3_IS_64_GF2X(\
P,Pol,K1,K2,,K164,K264,,R)
        #elif ((HFEn==544)&&(K3==128))
            #define REM_GF2N(P,Pol,Q,R) REM544_PENTANOMIAL_K3_IS_128_GF2X(\
P,Pol,K1,K2,,KI,KI64,K164,K264,,Q,R,MASK_GF2n)
        #elif (HFEnr)
            /* Example: REM192_PENTANOMIAL_GF2X(P,Pol,K1,K2,K3mod64,KI,KI64,
                        K164,K264,K364mod64,Q,R,MASK_GF2n) */
            #define REM_GF2N(P,Pol,Q,R) CONCAT(CONCAT_NB_BITS_MMUL_SUP(REM),\
_PENTANOMIAL_GF2X(P,Pol,K1,K2,K3mod64,KI,KI64,K164,K264,K364mod64,Q,R,\
MASK_GF2n))
        #else
            /* HFEn == NB_WORD_GFqn*64 */
            /* Example: REM192_PENTANOMIAL_K192_GF2X(P,Pol,K1,K2,K3mod64,\
                                                     K164,K264,K364mod64,R) */
            #define REM_GF2N_TMP CONCAT(CONCAT_NB_BITS_MMUL_SUP(CONCAT(\
CONCAT_NB_BITS_MMUL_SUP(REM),_PENTANOMIAL_K)),_GF2X)
            #define REM_GF2N(P,Pol,Q,R) \
                REM_GF2N_TMP(P,Pol,K1,K2,K3mod64,K164,K264,K364mod64,R)
        #endif
    #endif
#endif


/***********************************************************************/
/***********************************************************************/
/************************ Without SIMD version *************************/
/***********************************************************************/
/***********************************************************************/


void PREFIX_NAME(rem_gf2n_ref)(static_gf2n P[NB_WORD_GFqn],
                               cst_static_gf2x Pol[NB_WORD_MUL]);
void PREFIX_NAME(remsqr_gf2n_ref)(static_gf2n P[NB_WORD_GFqn],
                                  cst_static_gf2x Pol[NB_WORD_MUL]);
#define rem_gf2n_ref PREFIX_NAME(rem_gf2n_ref)
#define remsqr_gf2n_ref PREFIX_NAME(remsqr_gf2n_ref)

#if ((K3<33) || (defined(__TRINOMIAL_GF2N__)&&\
           (((HFEn==265)&&(K3==42)) || ((HFEn==266)&&(K3==47))\
         || ((HFEn==354)&&(K3==99)) || ((HFEn==358)&&(K3==57)))))
    void PREFIX_NAME(rem_gf2n_ref2)(static_gf2n P[NB_WORD_GFqn],
                                    cst_static_gf2x Pol[NB_WORD_MUL]);
    #define rem_gf2n_ref2 PREFIX_NAME(rem_gf2n_ref2)
#endif


#if (HFEn<33)
    UINT PREFIX_NAME(rem_no_simd_gf2n)(UINT Pol);
#else
    void PREFIX_NAME(rem_no_simd_gf2n)(static_gf2n P[NB_WORD_GFqn],
                                       cst_static_gf2x Pol[NB_WORD_MUL]);
#endif

#define rem_no_simd_gf2n PREFIX_NAME(rem_no_simd_gf2n)


/***********************************************************************/
/***********************************************************************/
/******************* Choice of the modular reduction *******************/
/***********************************************************************/
/***********************************************************************/


/* To choose the rem_gf2n function like macro or like function in mul_gf2n.c and
   sqr_gf2n.c. */
#ifdef MQSOFT_REF
    /* It is set to 0 to use the reference implementation instead of macros. */
    #define REM_MACRO 0
#else
    /* The user can set to 0 ou 1 this macro. */
    #define REM_MACRO 1
#endif

/* Choice of the best rem */
#ifdef MQSOFT_REF
    #if ((K3<33) || (defined(__TRINOMIAL_GF2N__)&&\
               (((HFEn==265)&&(K3==42)) || ((HFEn==266)&&(K3==47))\
             || ((HFEn==354)&&(K3==99)) || ((HFEn==358)&&(K3==57)))))
        /* Faster */
        #define rem_gf2n rem_gf2n_ref2
    #else
        #define rem_gf2n rem_gf2n_ref
    #endif
#elif (HFEn<33)
    #define rem_gf2n(P,Q) *(P)=rem_no_simd_gf2n(*(Q));
#else
    #define rem_gf2n rem_no_simd_gf2n
#endif

/* Choice of the best rem when the input is a square */
#if (defined(MQSOFT_REF)&&\
    (!((K3<33) || (defined(__TRINOMIAL_GF2N__)&&\
           (((HFEn==265)&&(K3==42)) || ((HFEn==266)&&(K3==47))\
         || ((HFEn==354)&&(K3==99)) || ((HFEn==358)&&(K3==57)))))))
    #define remsqr_gf2n remsqr_gf2n_ref
#else
    #define remsqr_gf2n rem_gf2n
#endif





#endif

