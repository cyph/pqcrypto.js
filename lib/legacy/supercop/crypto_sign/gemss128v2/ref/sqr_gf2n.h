#ifndef _SQR_GF2N_H
#define _SQR_GF2N_H

/* Really important include for the definition of 
   __TRINOMIAL_GF2N__ and __PENTANOMIAL_GF2N__ */
#include "rem_gf2n.h"
#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "macro.h"


/***********************************************************************/
/***********************************************************************/
/********************** Choice of sqr in GF(2)[x] **********************/
/***********************************************************************/
/***********************************************************************/


void PREFIX_NAME(sqr_no_simd_gf2x_ref2)(static_gf2x C[NB_WORD_MUL],
                                        cst_static_gf2n A[NB_WORD_GFqn]);
#define sqr_no_simd_gf2x_ref2 PREFIX_NAME(sqr_no_simd_gf2x_ref2)


/* Choice of the squaring in GF(2)[x] for the arithmetic in GF(2^n). */
#define sqr_gf2x sqr_no_simd_gf2x_ref2
#define sqr_nocst_gf2x sqr_gf2x


/***********************************************************************/
/***********************************************************************/
/************************* sqr then rem version ************************/
/***********************************************************************/
/***********************************************************************/


/* Function sqr in GF(2^x), then modular reduction */
#define SQR_THEN_REM_GF2N void \
            PREFIX_NAME(sqr_then_rem_gf2n)(uint64_t res[NB_WORD_GFqn], \
                                     const uint64_t A[NB_WORD_GFqn])
#define SQR_NOCST_THEN_REM_GF2N void \
            PREFIX_NAME(sqr_nocst_then_rem_gf2n)(uint64_t res[NB_WORD_GFqn], \
                                           const uint64_t A[NB_WORD_GFqn])
SQR_THEN_REM_GF2N;
SQR_NOCST_THEN_REM_GF2N;
#define sqr_then_rem_gf2n PREFIX_NAME(sqr_then_rem_gf2n)
#define sqr_nocst_then_rem_gf2n PREFIX_NAME(sqr_nocst_then_rem_gf2n)


/***********************************************************************/
/***********************************************************************/
/********************** Choice of sqr in GF(2^n) ***********************/
/***********************************************************************/
/***********************************************************************/


/* Choice the best sqr */
#define sqr_nocst_gf2n sqr_nocst_then_rem_gf2n
#if CONSTANT_TIME
    #define sqr_gf2n sqr_then_rem_gf2n
#else
    #define sqr_gf2n sqr_nocst_gf2n
#endif



#endif

