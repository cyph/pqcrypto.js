#ifndef _INV_GF2N_H
#define _INV_GF2N_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"

#include "config_gf2n.h"


#define CONCAT4(a,b) a ## b
#define CONCAT3(a,b) CONCAT4(a,b)

#if ((HFEn>2)&&(HFEn<577))
    #define ITOH CONCAT3(ITOH,HFEn)
#endif


#define CHOOSE_ITMIA


#ifdef MQSOFT_REF
    #define inv_gf2n inv_ITMIA_gf2n_ref
    /* Slower */
/*    #define inv_gf2n inv_Fermat_gf2n_ref*/
#elif (HFEn==2)
    /* {0,1,2,3} --> {0,1,3,2} */
    #define inv_gf2n(res,A) (*(res))=(*(A))^((*(A))>>1);
#elif (defined(ENABLED_PCLMUL)&&(NB_WORD_GFqn<10))
    #define inv_gf2n inv_ITMIA_gf2n
#else
    #define inv_gf2n inv_ITMIA_gf2n_ref
#endif


void PREFIX_NAME(inv_Fermat_gf2n_ref)(uint64_t res[NB_WORD_GFqn],
                                      const uint64_t A[NB_WORD_GFqn]);
#define inv_Fermat_gf2n_ref PREFIX_NAME(inv_Fermat_gf2n_ref)


void PREFIX_NAME(inv_ITMIA_gf2n_ref)(static_gf2n res[NB_WORD_GFqn],
                                     cst_static_gf2n A[NB_WORD_GFqn]);
#define inv_ITMIA_gf2n_ref PREFIX_NAME(inv_ITMIA_gf2n_ref)


#ifdef ITOH
    #if (defined(ENABLED_PCLMUL)&&(NB_WORD_GFqn<10))
        void PREFIX_NAME(inv_ITMIA_gf2n)(static_gf2n res[NB_WORD_GFqn],
                                     cst_static_gf2n A[NB_WORD_GFqn]);
        #define inv_ITMIA_gf2n PREFIX_NAME(inv_ITMIA_gf2n)
    #endif
#endif


#endif

