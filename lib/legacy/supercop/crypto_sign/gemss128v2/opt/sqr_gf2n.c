#include "sqr_gf2n.h"
#include "rem_gf2n.h"
#include "arch.h"
#include "simd.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"


/***********************************************************************/
/***********************************************************************/
/*************************** sqr without rem ***************************/
/***********************************************************************/
/***********************************************************************/


/**
 * @brief   Squaring in GF(2)[x].
 * @details For each 32-bit block on the input, we use the following strategy:
   Assume we want to insert a null bit between each bit of 0x00000000FFFFFFFF.
   We do as following:
    0x00000000FFFFFFFF (it is already an insertion of a zero 32-bit packed)
    0x0000FFFF0000FFFF (insertion by pack of 16 bits)
    0x00FF00FF00FF00FF (insertion by pack of 8 bits)
    0x0F0F0F0F0F0F0F0F (insertion by pack of 4 bits)
    0x3333333333333333 (insertion by pack of 2 bits)
    0x5555555555555555 (insertion by pack of 1 bit).
 * @param[in]   A   An element of GF(2^n).
 * @param[out]  C   C=A*A in GF(2)[x] (the result is not reduced).
 * @remark  Constant-time implementation.
 */
void PREFIX_NAME(sqr_no_simd_gf2x_ref2)(static_gf2x C[NB_WORD_MUL],
                                        cst_static_gf2n A[NB_WORD_GFqn])
{
    uint64_t Ci;
    unsigned int i;

    C+=NB_WORD_MUL-1;
    #if (NB_WORD_MUL&1)
        i=NB_WORD_GFqn-1;
        /* Lower 32 bits of A[i] */
        Ci=(A[i]^(A[i]<<16))&((uint64_t)0x0000FFFF0000FFFF);
        Ci=(Ci^(Ci<<8))&((uint64_t)0x00FF00FF00FF00FF);
        Ci=(Ci^(Ci<<4))&((uint64_t)0x0F0F0F0F0F0F0F0F);
        Ci=(Ci^(Ci<<2))&((uint64_t)0x3333333333333333);
        Ci=(Ci^(Ci<<1))&((uint64_t)0x5555555555555555);
        *C=Ci;
        --C;
    for(i=NB_WORD_GFqn-2;i!=(unsigned int)(-1);--i)
    #else
    for(i=NB_WORD_GFqn-1;i!=(unsigned int)(-1);--i)
    #endif
    {
        /* Higher 32 bits of A[i] */
        Ci=((A[i]>>32)^((A[i]>>32)<<16))&((uint64_t)0x0000FFFF0000FFFF);
        Ci=(Ci^(Ci<<8))&((uint64_t)0x00FF00FF00FF00FF);
        Ci=(Ci^(Ci<<4))&((uint64_t)0x0F0F0F0F0F0F0F0F);
        Ci=(Ci^(Ci<<2))&((uint64_t)0x3333333333333333);
        Ci=(Ci^(Ci<<1))&((uint64_t)0x5555555555555555);
        *C=Ci;
        --C;

        /* Lower 32 bits of A[i] */
        Ci=((A[i]&((uint64_t)0xFFFFFFFF))^(A[i]<<16))
           &((uint64_t)0x0000FFFF0000FFFF);
        Ci=(Ci^(Ci<<8))&((uint64_t)0x00FF00FF00FF00FF);
        Ci=(Ci^(Ci<<4))&((uint64_t)0x0F0F0F0F0F0F0F0F);
        Ci=(Ci^(Ci<<2))&((uint64_t)0x3333333333333333);
        Ci=(Ci^(Ci<<1))&((uint64_t)0x5555555555555555);
        *C=Ci;
        --C;
    }
}


/***********************************************************************/
/***********************************************************************/
/***************************** sqr with rem ****************************/
/***********************************************************************/
/***********************************************************************/


/***********************************************************************/
/***********************************************************************/
/************************* sqr then rem version ************************/
/***********************************************************************/
/***********************************************************************/


/* When rem is a macro */
#if (K3!=1)
    #define MUL_MOD_MACRO32(name_function,mul_function,rem_function) \
        name_function\
        {\
            uint64_t res_mul,Q,R;\
            mul_function;\
            rem_function;\
        }
#else
    #define MUL_MOD_MACRO32(name_function,mul_function,rem_function) \
        name_function\
        {\
            uint64_t res_mul,Q;\
            mul_function;\
            rem_function;\
        }
#endif

#define MUL_MOD_MACRO64(name_function,mul_function,rem_function,size) \
    name_function\
    {\
        uint64_t res_mul[size],Q,R;\
        mul_function;\
        rem_function;\
    }

/* HFEn == 64 */
#define MUL_MOD_MACRO64_K64(name_function,mul_function,rem_function,size) \
    name_function\
    {\
        uint64_t res_mul[size],R;\
        mul_function;\
        rem_function;\
    }


#if HFEnr
    #define INIT_Q(size2) uint64_t Q[size2];
#else
    /* Q is useless for REM */
    #define INIT_Q(size2)
#endif

#if ((HFEn==312)||(HFEn==402)||(HFEn==544))
    #define MUL_MOD_MACRO(name_function,mul_function,rem_function,size,size2) \
        name_function\
        {\
            uint64_t res_mul[size];\
            INIT_Q(size2);\
            mul_function;\
            rem_function;\
        }
#else
    #define MUL_MOD_MACRO(name_function,mul_function,rem_function,size,size2) \
        name_function\
        {\
            uint64_t res_mul[size],R;\
            INIT_Q(size2);\
            mul_function;\
            rem_function;\
        }
#endif

/* When rem is a function */
#define MUL_MOD_FUNCTION32(name_function,mul_function,rem_function) \
    name_function\
    {\
        uint64_t res_mul;\
        mul_function;\
        rem_function;\
    }

#define MUL_MOD_FUNCTION(name_function,mul_function,rem_function,size) \
    name_function\
    {\
        uint64_t res_mul[size];\
        mul_function;\
        rem_function;\
    }

#if (REM_MACRO)
    #if (NB_WORD_GFqn!=1)
        MUL_MOD_MACRO(SQR_THEN_REM_GF2N,sqr_gf2x(res_mul,A),
                      REM_GF2N(res,res_mul,Q,R),NB_WORD_MUL,NB_WORD_GFqn);
        MUL_MOD_MACRO(SQR_NOCST_THEN_REM_GF2N,sqr_nocst_gf2x(res_mul,A),
                      REM_GF2N(res,res_mul,Q,R),NB_WORD_MUL,NB_WORD_GFqn);
    #elif (HFEn<33)
        MUL_MOD_MACRO32(SQR_THEN_REM_GF2N,sqr_gf2x(&res_mul,A),
                        REM_GF2N(*res,res_mul,Q,R));
        MUL_MOD_MACRO32(SQR_NOCST_THEN_REM_GF2N,sqr_nocst_gf2x(&res_mul,A),
                        REM_GF2N(*res,res_mul,Q,R));
    #elif HFEnr
        MUL_MOD_MACRO64(SQR_THEN_REM_GF2N,sqr_gf2x(res_mul,A),
                        REM_GF2N(*res,res_mul,Q,R),NB_WORD_MUL);
        MUL_MOD_MACRO64(SQR_NOCST_THEN_REM_GF2N,sqr_nocst_gf2x(res_mul,A),
                        REM_GF2N(*res,res_mul,Q,R),NB_WORD_MUL);
    #else
        /* HFEn == 64 */
        MUL_MOD_MACRO64_K64(SQR_THEN_REM_GF2N,sqr_gf2x(res_mul,A),
                            REM_GF2N(*res,res_mul,,R),NB_WORD_MUL);
        MUL_MOD_MACRO64_K64(SQR_NOCST_THEN_REM_GF2N,sqr_nocst_gf2x(res_mul,A),
                            REM_GF2N(*res,res_mul,,R),NB_WORD_MUL);
    #endif
#elif (NB_WORD_GFqn!=1)
    MUL_MOD_FUNCTION(SQR_THEN_REM_GF2N,sqr_gf2x(res_mul,A),
                     remsqr_gf2n(res,res_mul),NB_WORD_MUL);
    MUL_MOD_FUNCTION(SQR_NOCST_THEN_REM_GF2N,sqr_nocst_gf2x(res_mul,A),
                     remsqr_gf2n(res,res_mul),NB_WORD_MUL);
#elif (HFEn<33)
    MUL_MOD_FUNCTION32(SQR_THEN_REM_GF2N,sqr_gf2x(&res_mul,A),
                       remsqr_gf2n(res,&res_mul));
    MUL_MOD_FUNCTION32(SQR_NOCST_THEN_REM_GF2N,sqr_nocst_gf2x(&res_mul,A),
                       remsqr_gf2n(res,&res_mul));
#else
    MUL_MOD_FUNCTION(SQR_THEN_REM_GF2N,sqr_gf2x(res_mul,A),
                     remsqr_gf2n(res,res_mul),NB_WORD_MUL);
    MUL_MOD_FUNCTION(SQR_NOCST_THEN_REM_GF2N,sqr_nocst_gf2x(res_mul,A),
                     remsqr_gf2n(res,res_mul),NB_WORD_MUL);
#endif


