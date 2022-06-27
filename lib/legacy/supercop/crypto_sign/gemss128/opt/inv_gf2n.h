#ifndef _INV_GF2N_H
#define _INV_GF2N_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "mul_gf2x.h"

#include "config_gf2n.h"



#define CONCAT4(a,b) a ## b
#define CONCAT3(a,b) CONCAT4(a,b)

#if ((HFEn>2)&&(HFEn<577))
    #define ITOH CONCAT3(ITOH,HFEn)
#endif




/* Extended Euclidean Algorithm */
/* To define this variable is slower. */
/*#define EEA*/

#if ((!defined(EEA))&&(defined(ENABLED_PCLMUL))&&(!ENABLED_GF2X)&&(defined(ITOH)))
    #define CHOOSE_ITMIA
#else
    #define EEA
#endif


#ifdef EEA
    #define inv_gf2n inv_EEA_gf2n
#elif (defined(ENABLED_PCLMUL)&&(NB_WORD_GFqn<10))
    #define inv_gf2n inv_ITMIA_gf2n
#endif


#if (HFEn&63)
    void PREFIX_NAME(inv_EEA_gf2n)(uint64_t v2[NB_WORD_GFqn], const uint64_t A[NB_WORD_GFqn]);
    #define inv_EEA_gf2n PREFIX_NAME(inv_EEA_gf2n)
#endif


#ifdef ITOH
    #if (defined(ENABLED_PCLMUL)&&(NB_WORD_GFqn<10))
        void PREFIX_NAME(inv_ITMIA_gf2n)(uint64_t res[NB_WORD_GFqn], const uint64_t A[NB_WORD_GFqn]);
        #define inv_ITMIA_gf2n PREFIX_NAME(inv_ITMIA_gf2n)
    #endif
#endif



#endif

