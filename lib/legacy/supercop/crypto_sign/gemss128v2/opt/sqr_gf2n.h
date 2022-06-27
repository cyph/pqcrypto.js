#ifndef _SQR_GF2N_H
#define _SQR_GF2N_H

#include "sqr_gf2x.h"
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


/* Here, we define the squaring functions in GF2[x] */
#if (HFEn<33)
    #define sqr_no_simd_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(sqr),_no_simd_gf2x)
#elif (NB_WORD_GFqn==1)
    #define sqr_no_simd_gf2x(C,A) SQR64_NO_SIMD_GF2X((C),(*(A)))
#else
    #define sqr_no_simd_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(SQR),_NO_SIMD_GF2X)
#endif
#define psqr_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(psqr),_gf2x)
#define psqr_shuffle_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(psqr),_shuffle_gf2x)
#define vpsqr_shuffle_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(vpsqr),_shuffle_gf2x)

#if (NB_WORD_GFqn==1)
    #define sqr_pclmul_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(sqr),\
                                   _inlined_pclmul_gf2x)
#else
    #define sqr_pclmul_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(sqr),_pclmul_gf2x)
#endif



/* This macro is used only in inv_gf2n.c */
#define SQR_GF2X CONCAT(CONCAT_NB_BITS_MUL_SUP(SQR),_PCLMUL_GF2X)



/* Choice of the squaring in GF(2)[x] for the arithmetic in GF(2^n). */
#ifdef MQSOFT_REF
    #define sqr_gf2x sqr_no_simd_gf2x_ref2
    #define sqr_nocst_gf2x sqr_gf2x
#elif (HFEn<33)
    #ifdef ENABLED_PCLMUL
        #define sqr_nocst_gf2x(C,A) best_sqr_nocst_32_gf2x(*(C),*(A));
    #else
        #define sqr_nocst_gf2x(C,A) (*(C))=best_sqr_nocst_32_gf2x(*(A));
    #endif

    #if CONSTANT_TIME
        #ifdef ENABLED_PCLMUL
            #define sqr_gf2x(C,A) best_sqr32_gf2x(*(C),*(A));
        #else
            #define sqr_gf2x(C,A) (*(C))=best_sqr32_gf2x(*(A));
        #endif
    #else
        /* Faster */
        #define sqr_gf2x sqr_nocst_gf2x
    #endif
#else
    #define sqr_nocst_gf2x \
                CONCAT(CONCAT_NB_BITS_MMUL_SUP(best_sqr_nocst_),_gf2x)

    #if CONSTANT_TIME
        #define sqr_gf2x CONCAT(CONCAT_NB_BITS_MMUL_SUP(best_sqr),_gf2x)
    #else
        /* Faster */
        #define sqr_gf2x sqr_nocst_gf2x
    #endif
#endif


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
#if ((HFEn<33)||defined(MQSOFT_REF))
    #define sqr_nocst_gf2n sqr_nocst_then_rem_gf2n
    #if CONSTANT_TIME
        #define sqr_gf2n sqr_then_rem_gf2n
    #else
        #define sqr_gf2n sqr_nocst_gf2n
    #endif
#else
    #define sqr_nocst_gf2n sqr_nocst_then_rem_gf2n
    #if CONSTANT_TIME
        #define sqr_gf2n sqr_then_rem_gf2n
    #else
        #define sqr_gf2n sqr_nocst_gf2n
    #endif
#endif



#endif

