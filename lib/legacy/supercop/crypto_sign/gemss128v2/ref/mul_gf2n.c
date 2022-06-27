#include "mul_gf2n.h"
#include "rem_gf2n.h"
#include "arch.h"
#include "simd.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"


/**
 * @brief   Multiplication in GF(2)[x].
 * @param[in]   A   An element of GF(2^n).
 * @param[in]   B   An element of GF(2^n).
 * @param[out]  C   C=A*B in GF(2)[x] (the result is not reduced).
 * @remark  Constant-time implementation.
 */
void PREFIX_NAME(mul_no_simd_gf2x_ref)(uint64_t C[NB_WORD_MUL],
                                       const uint64_t A[NB_WORD_GFqn],
                                       const uint64_t B[NB_WORD_GFqn])
{
    uint64_t mask_B,tmp_A;
    unsigned int i,j,k,jc;

    for(i=0;i<NB_WORD_MUL;++i)
    {
        C[i]=0;
    }

    for(i=0;i<HFEnq;++i)
    {
        /* j=0 */
        mask_B=-((*B)&ONE64);
        for(k=0;k<NB_WORD_GFqn;++k)
        {
            C[k]^=A[k]&mask_B;
        }

        #if HFEnr
            /* The last 64-bit block BL of A contains HFEnr bits.
               So, there is not overflow for BL<<j while j<=(64-HFEnr). */
            for(j=1;j<=(64-HFEnr);++j)
            {
                jc=64-j;
                mask_B=-(((*B)>>j)&ONE64);
                /* k=0 */
                tmp_A=(*A)&mask_B;
                C[0]^=tmp_A<<j;
                for(k=1;k<NB_WORD_GFqn;++k)
                {
                    C[k]^=tmp_A>>jc;
                    tmp_A=A[k]&mask_B;
                    C[k]^=tmp_A<<j;
                }
            }
        for(;j<64;++j)
        #else
        for(j=1;j<64;++j)
        #endif
        {
            jc=64-j;
            mask_B=-(((*B)>>j)&ONE64);
            /* k=0 */
            tmp_A=(*A)&mask_B;
            C[0]^=tmp_A<<j;
            for(k=1;k<NB_WORD_GFqn;++k)
            {
                C[k]^=tmp_A>>jc;
                tmp_A=A[k]&mask_B;
                C[k]^=tmp_A<<j;
            }
            /* "overflow" from the last 64-bit block of A */
            C[k]^=tmp_A>>jc;
        }

        ++B;
        ++C;
    }

    #if HFEnr
        /* j=0 */
        mask_B=-((*B)&ONE64);
        for(k=0;k<NB_WORD_GFqn;++k)
        {
            C[k]^=A[k]&mask_B;
        }

        /* The last 64-bit block BL of A contains HFEnr bits.
           So, there is not overflow for BL<<j while j<=(64-HFEnr). */
        #if (HFEnr>32)
        for(j=1;j<=(64-HFEnr);++j)
        #else
        for(j=1;j<HFEnr;++j)
        #endif
        {
            jc=64-j;
            mask_B=-(((*B)>>j)&ONE64);
            /* k=0 */
            tmp_A=(*A)&mask_B;
            C[0]^=tmp_A<<j;
            for(k=1;k<NB_WORD_GFqn;++k)
            {
                C[k]^=tmp_A>>jc;
                tmp_A=A[k]&mask_B;
                C[k]^=tmp_A<<j;
            }
        }

        #if (HFEnr>32)
            for(;j<HFEnr;++j)
            {
                jc=64-j;
                mask_B=-(((*B)>>j)&ONE64);
                /* k=0 */
                tmp_A=(*A)&mask_B;
                C[0]^=tmp_A<<j;
                for(k=1;k<NB_WORD_GFqn;++k)
                {
                    C[k]^=tmp_A>>jc;
                    tmp_A=A[k]&mask_B;
                    C[k]^=tmp_A<<j;
                }
                /* "overflow" from the last 64-bit block of A */
                C[k]^=tmp_A>>jc;
            }
        #endif
    #endif
}


/***********************************************************************/
/***********************************************************************/
/************************* mul then rem version ************************/
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
        MUL_MOD_MACRO(MUL_THEN_REM_GF2N,mul_gf2x(res_mul,A,B),
                      REM_GF2N(res,res_mul,Q,R),NB_WORD_MUL,NB_WORD_GFqn);
    #elif (HFEn<33)
        MUL_MOD_MACRO32(MUL_THEN_REM_GF2N,mul_gf2x(&res_mul,A,B),
                        REM_GF2N(*res,res_mul,Q,R));
    #elif HFEnr
        MUL_MOD_MACRO64(MUL_THEN_REM_GF2N,mul_gf2x(res_mul,A,B),
                        REM_GF2N(*res,res_mul,Q,R),NB_WORD_MUL);
    #else
        /* HFEn == 64 */
        MUL_MOD_MACRO64_K64(MUL_THEN_REM_GF2N,mul_gf2x(res_mul,A,B),
                            REM_GF2N(*res,res_mul,,R),NB_WORD_MUL);
    #endif
#elif (NB_WORD_GFqn!=1)
    MUL_MOD_FUNCTION(MUL_THEN_REM_GF2N,mul_gf2x(res_mul,A,B),
                     rem_gf2n(res,res_mul),NB_WORD_MUL);
#elif (HFEn<33)
    MUL_MOD_FUNCTION32(MUL_THEN_REM_GF2N,mul_gf2x(&res_mul,A,B),
                       rem_gf2n(res,&res_mul));
#else
    MUL_MOD_FUNCTION(MUL_THEN_REM_GF2N,mul_gf2x(res_mul,A,B),
                     rem_gf2n(res,res_mul),NB_WORD_MUL);
#endif


