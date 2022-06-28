#include "arith.h"
#include "mul.h"
#include "rem.h"


#if (NB_WORD_GFqn==1)
    #if (HFEn<33)
        uint64_t hferem(const uint64_t Pol)
        {
            uint64_t P,Q,R;
            HFEREM(P,Pol,Q,R);
            return P;
        }
    #else
        #if HFEnr
            uint64_t hferem(const uint64_t Pol[NB_WORD_MUL])
            {
                uint64_t P,Q,R;
                HFEREM(P,Pol,Q,R);
                return P;
            }
        #else
            /* HFEn == 64 */
            uint64_t hferem(const uint64_t Pol[NB_WORD_MUL])
            {
                uint64_t P,R;
                HFEREM(P,Pol,,R);
                return P;
            }
        #endif
    #endif
#else
    #if HFEnr
        void hferem(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL])
        {
            uint64_t Q[NB_WORD_GFqn],R;
            HFEREM(P,Pol,Q,R);
        }
    #else
        void hferem(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL])
        {
            uint64_t R;
            HFEREM(P,Pol,,R);
        }
    #endif
#endif


/* When rem is a macro */
#define MUL_MOD_MACRO32(name_function,mul_function,rem_function) name_function\
{ \
    uint64_t res_mul,Q,R; \
    mul_function; \
    rem_function; \
}

#define MUL_MOD_MACRO64(name_function,mul_function,rem_function,size) name_function\
{ \
    uint64_t res_mul[size],Q,R; \
    mul_function; \
    rem_function; \
}

/* HFEn == 64 */
#define MUL_MOD_MACRO64_K64(name_function,mul_function,rem_function,size) name_function\
{ \
    uint64_t res_mul[size],R; \
    mul_function; \
    rem_function; \
}


#if HFEnr
    #define INIT_Q(size2) uint64_t Q[size2]
#else
    /* Q is useless for REM */
    #define INIT_Q(size2)
#endif

#define MUL_MOD_MACRO(name_function,mul_function,rem_function,size,size2) name_function\
{ \
    uint64_t res_mul[size],R; \
    INIT_Q(size2); \
    mul_function; \
    rem_function; \
}

/* When rem is a function */
#define MUL_MOD_FUNCTION32(name_function,mul_function,rem_function) name_function\
{ \
    uint64_t res_mul; \
    mul_function; \
    rem_function; \
}

#define MUL_MOD_FUNCTION(name_function,mul_function,rem_function,size) name_function\
{ \
    uint64_t res_mul[size]; \
    mul_function; \
    rem_function; \
}

#if (REM_MACRO)
    #if (NB_WORD_GFqn==1)
        #if (HFEn<33)
            MUL_MOD_MACRO32(HFEMULMOD,res_mul=hfemul(*A,*B),HFEREM(*res,res_mul,Q,R));
            MUL_MOD_MACRO32(HFESQUAREMOD,res_mul=hfesquare(*A),HFEREM(*res,res_mul,Q,R));
        #else
            #if HFEnr
                MUL_MOD_MACRO64(HFEMULMOD,hfemul(res_mul,*A,*B),HFEREM(*res,res_mul,Q,R),NB_WORD_MUL);
                MUL_MOD_MACRO64(HFESQUAREMOD,hfesquare(res_mul,*A),HFEREM(*res,res_mul,Q,R),NB_WORD_MUL);
            #else
                /* HFEn == 64 */
                MUL_MOD_MACRO64_K64(HFEMULMOD,hfemul(res_mul,*A,*B),HFEREM(*res,res_mul,,R),NB_WORD_MUL);
                MUL_MOD_MACRO64_K64(HFESQUAREMOD,hfesquare(res_mul,*A),HFEREM(*res,res_mul,,R),NB_WORD_MUL);
            #endif
        #endif
    #else
        MUL_MOD_MACRO(HFEMULMOD,hfemul(res_mul,A,B),HFEREM(res,res_mul,Q,R),NB_WORD_MUL,NB_WORD_GFqn);
        MUL_MOD_MACRO(HFESQUAREMOD,hfesquare(res_mul,A),HFEREM(res,res_mul,Q,R),NB_WORD_MUL,NB_WORD_GFqn);
    #endif
#else
    #if (NB_WORD_GFqn==1)
        #if (HFEn<33)
            MUL_MOD_FUNCTION32(HFEMULMOD,res_mul=hfemul(*A,*B),*res=hferem(res_mul));
            MUL_MOD_FUNCTION32(HFESQUAREMOD,res_mul=hfesquare(*A),*res=hferem(res_mul));
        #else
            MUL_MOD_FUNCTION(HFEMULMOD,hfemul(res_mul,*A,*B),*res=hferem(res_mul),NB_WORD_MUL);
            MUL_MOD_FUNCTION(HFESQUAREMOD,hfesquare(res_mul,*A),*res=hferem(res_mul),NB_WORD_MUL);
        #endif
    #else
        MUL_MOD_FUNCTION(HFEMULMOD,hfemul(res_mul,A,B),hferem(res,res_mul),NB_WORD_MUL);
        MUL_MOD_FUNCTION(HFESQUAREMOD,hfesquare(res_mul,A),hferem(res,res_mul),NB_WORD_MUL);
    #endif
#endif

