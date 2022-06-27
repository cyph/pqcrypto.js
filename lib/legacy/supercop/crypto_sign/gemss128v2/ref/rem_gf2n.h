#ifndef _REM_GF2N_H
#define _REM_GF2N_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "gf2x.h"
#include "tools_gf2n.h"
#include "macro.h"
#include "rem5_gf2n.h"


#include "config_gf2n.h"


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


/***********************************************************************/
/***********************************************************************/
/******************* Choice of the modular reduction *******************/
/***********************************************************************/
/***********************************************************************/


/* To choose the rem_gf2n function like macro or like function in mul_gf2n.c and
   sqr_gf2n.c. */
/* It is set to 0 to use the reference implementation instead of macros. */
#define REM_MACRO 0

/* Choice of the best rem */
#if ((K3<33) || (defined(__TRINOMIAL_GF2N__)&&\
           (((HFEn==265)&&(K3==42)) || ((HFEn==266)&&(K3==47))\
         || ((HFEn==354)&&(K3==99)) || ((HFEn==358)&&(K3==57)))))
    /* Faster */
    #define rem_gf2n rem_gf2n_ref2
#else
    #define rem_gf2n rem_gf2n_ref
#endif

/* Choice of the best rem when the input is a square */
#define remsqr_gf2n remsqr_gf2n_ref





#endif

