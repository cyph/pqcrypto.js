#include "arith_gf2n.h"
#include "sqr_gf2x.h"
#include "mul_gf2x.h"
#include "rem_gf2x.h"
#include "rem_gf2n.h"

#include "init.h"


/* When rem is a macro */
#if (K3!=1)
#define MUL_MOD_MACRO32(name_function,mul_function,rem_function) name_function\
{ \
    uint64_t res_mul,Q,R; \
    mul_function; \
    rem_function; \
}
#else
#define MUL_MOD_MACRO32(name_function,mul_function,rem_function) name_function\
{ \
    uint64_t res_mul,Q; \
    mul_function; \
    rem_function; \
}
#endif

#define MUL_MOD_MACRO64(name_function,mul_function,rem_function,size) name_function\
{ \
    uint64_t res_mul[size] ALIGNED,Q,R; \
    mul_function; \
    rem_function; \
}

/* HFEn == 64 */
#define MUL_MOD_MACRO64_K64(name_function,mul_function,rem_function,size) name_function\
{ \
    uint64_t res_mul[size] ALIGNED,R; \
    mul_function; \
    rem_function; \
}


#if HFEnr
    #define INIT_Q(size2) uint64_t Q[size2] ALIGNED;
#else
    /* Q is useless for REM */
    #define INIT_Q(size2)
#endif

#if ((HFEn==312)||(HFEn==402)||(HFEn==544))
    #define MUL_MOD_MACRO(name_function,mul_function,rem_function,size,size2) name_function\
    { \
        uint64_t res_mul[size] ALIGNED; \
        INIT_Q(size2); \
        mul_function; \
        rem_function; \
    }
#else
    #define MUL_MOD_MACRO(name_function,mul_function,rem_function,size,size2) name_function\
    { \
        uint64_t res_mul[size] ALIGNED,R; \
        INIT_Q(size2); \
        mul_function; \
        rem_function; \
    }
#endif

/* When rem is a function */
#define MUL_MOD_FUNCTION32(name_function,mul_function,rem_function) name_function\
{ \
    uint64_t res_mul; \
    mul_function; \
    rem_function; \
}

#define MUL_MOD_FUNCTION(name_function,mul_function,rem_function,size) name_function\
{ \
    uint64_t res_mul[size] ALIGNED; \
    mul_function; \
    rem_function; \
}

#if (REM_MACRO)
    #if (NB_WORD_GFqn==1)
        #if (HFEn<33)
            MUL_MOD_MACRO32(MULMOD_GF2N,mul_gf2x(&res_mul,A,B),REM_GF2N(*res,res_mul,Q,R));
            MUL_MOD_MACRO32(SQRMOD_GF2N,sqr_gf2x(&res_mul,A),REM_GF2N(*res,res_mul,Q,R));
        #else
            #if HFEnr
                MUL_MOD_MACRO64(MULMOD_GF2N,mul_gf2x(res_mul,A,B),REM_GF2N(*res,res_mul,Q,R),NB_WORD_MUL);
                MUL_MOD_MACRO64(SQRMOD_GF2N,sqr_gf2x(res_mul,A),REM_GF2N(*res,res_mul,Q,R),NB_WORD_MUL);
            #else
                /* HFEn == 64 */
                MUL_MOD_MACRO64_K64(MULMOD_GF2N,mul_gf2x(res_mul,A,B),REM_GF2N(*res,res_mul,,R),NB_WORD_MUL);
                MUL_MOD_MACRO64_K64(SQRMOD_GF2N,sqr_gf2x(res_mul,A),REM_GF2N(*res,res_mul,,R),NB_WORD_MUL);
            #endif
        #endif
    #else
        MUL_MOD_MACRO(MULMOD_GF2N,mul_gf2x(res_mul,A,B),REM_GF2N(res,res_mul,Q,R),NB_WORD_MUL,NB_WORD_GFqn);
        MUL_MOD_MACRO(SQRMOD_GF2N,sqr_gf2x(res_mul,A),REM_GF2N(res,res_mul,Q,R),NB_WORD_MUL,NB_WORD_GFqn);
    #endif
#else
    #if (NB_WORD_GFqn==1)
        #if (HFEn<33)
            MUL_MOD_FUNCTION32(MULMOD_GF2N,mul_gf2x(&res_mul,A,B),rem_gf2n(res,&res_mul));
            MUL_MOD_FUNCTION32(SQRMOD_GF2N,sqr_gf2x(&res_mul,A),rem_gf2n(res,&res_mul));
        #else
            MUL_MOD_FUNCTION(MULMOD_GF2N,mul_gf2x(res_mul,A,B),rem_gf2n(res,res_mul),NB_WORD_MUL);
            MUL_MOD_FUNCTION(SQRMOD_GF2N,sqr_gf2x(res_mul,A),rem_gf2n(res,res_mul),NB_WORD_MUL);
        #endif
    #else
        MUL_MOD_FUNCTION(MULMOD_GF2N,mul_gf2x(res_mul,A,B),rem_gf2n(res,res_mul),NB_WORD_MUL);
        MUL_MOD_FUNCTION(SQRMOD_GF2N,sqr_gf2x(res_mul,A),rem_gf2n(res,res_mul),NB_WORD_MUL);
    #endif
#endif

