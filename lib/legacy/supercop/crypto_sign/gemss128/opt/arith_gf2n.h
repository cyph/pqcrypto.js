#ifndef _ARITH_GF2N_H
#define _ARITH_GF2N_H


#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "tools_gf2n.h"
#include "add_gf2x.h"
#include "rem_gf2n.h"



/* Addition in GF(2^n) */
#if (NB_WORD_GFqn==1)
    #define ADD_GF2N ADD64_TAB
    #define ADD2_GF2N ADD64_TAB2
#elif (NB_WORD_GFqn<7)
    #define ADD_GF2N CONCAT_NB_BITS_GFqn_SUP(ADD)
    #define ADD2_GF2N CONCAT(CONCAT_NB_BITS_GFqn_SUP(ADD),_2)
#else
    #define ADD_GF2N(a,b,c) ADD(a,b,c,NB_WORD_GFqn)
    #define ADD2_GF2N(a,b) ADD2(a,b,NB_WORD_GFqn)
#endif


/* Addition of the res of a multiplication in GF(2^n) */
#if (NB_WORD_MUL == 1)
    #define NB_BITS_MULADD_SUP 64
#elif (NB_WORD_MUL == 2)
    #define NB_BITS_MULADD_SUP 128
#elif (NB_WORD_MUL == 3)
    #define NB_BITS_MULADD_SUP 192
#elif (NB_WORD_MUL == 4)
    #define NB_BITS_MULADD_SUP 256
#elif (NB_WORD_MUL == 5)
    #define NB_BITS_MULADD_SUP 320
#elif (NB_WORD_MUL == 6)
    #define NB_BITS_MULADD_SUP 384
#elif (NB_WORD_MUL == 7)
    #define NB_BITS_MULADD_SUP 448
#elif (NB_WORD_MUL == 8)
    #define NB_BITS_MULADD_SUP 512
#elif (NB_WORD_MUL == 9)
    #define NB_BITS_MULADD_SUP 576
#elif (NB_WORD_MUL == 10)
    #define NB_BITS_MULADD_SUP 640
#elif (NB_WORD_MUL == 11)
    #define NB_BITS_MULADD_SUP 704
#elif (NB_WORD_MUL == 12)
    #define NB_BITS_MULADD_SUP 768
#elif (NB_WORD_MUL == 13)
    #define NB_BITS_MULADD_SUP 832
#elif (NB_WORD_MUL == 14)
    #define NB_BITS_MULADD_SUP 896
#elif (NB_WORD_MUL == 15)
    #define NB_BITS_MULADD_SUP 960
#elif (NB_WORD_MUL == 16)
    #define NB_BITS_MULADD_SUP 1024
#elif (NB_WORD_MUL == 17)
    #define NB_BITS_MULADD_SUP 1088
#elif (NB_WORD_MUL == 18)
    #define NB_BITS_MULADD_SUP 1152
#endif

#define CONCAT_NB_BITS_MULADD_SUP(name) CONCAT(name,NB_BITS_MULADD_SUP)


#if (HFEn<33)
    #define ADD2MUL_GF2N ADD32_TAB2
#else
    #define ADD2MUL_GF2N CONCAT(CONCAT_NB_BITS_MULADD_SUP(ADD),_2)
#endif



/* The addition function is inlined */
#define add_gf2n ADD_GF2N
#define add2_gf2n ADD2_GF2N
#define add2mul_gf2n ADD2MUL_GF2N



/* Mul and sqr in GF(2)[x] */
/* It is used in inv_gf2n.c */
#define MUL_GF2X CONCAT_NB_BITS_MUL_SUP(MUL)
#define SQR_GF2X CONCAT_NB_BITS_MUL_SUP(SQUARE)

#if (HFEn<33)
    #define mul_gf2x(C,A,B) (*(C))=mul32(*(A),*(B));
    #define sqr_gf2x(C,A) (*(C))=square32(*(A));
#else
    #ifdef ENABLED_PCLMUL
        #define mul_gf2x CONCAT_NB_BITS_MUL_SUP(PREFIX_NAME(mul))
        #define sqr_gf2x CONCAT_NB_BITS_MUL_SUP(PREFIX_NAME(sqr))
    #else
        #define mul_gf2x CONCAT_NB_BITS_MUL_SUP(mul)
        #define sqr_gf2x CONCAT_NB_BITS_MUL_SUP(square)
    #endif
#endif



/* Mul and sqr in GF(2^n) */
#define MULMOD_GF2N void PREFIX_NAME(mulmod_gf2n)(uint64_t res[NB_WORD_GFqn], \
                const uint64_t A[NB_WORD_GFqn], const uint64_t B[NB_WORD_GFqn])
#define SQRMOD_GF2N void PREFIX_NAME(squaremod_gf2n)(uint64_t res[NB_WORD_GFqn], \
                                       const uint64_t A[NB_WORD_GFqn])

MULMOD_GF2N;
SQRMOD_GF2N;

#define mulmod_gf2n PREFIX_NAME(mulmod_gf2n)
#define squaremod_gf2n PREFIX_NAME(squaremod_gf2n)



/* To choose rem_gf2n function like macro or like function */
#define REM_MACRO 1


#if (HFEn<33)
    #define rem_gf2n(P,Q) *(P)=rem_noSIMD_gf2n(*(Q));
#else
    #define rem_gf2n rem_noSIMD_gf2n
#endif



#if ((NB_WORD_GFqn==1)&&defined(ENABLED_PCLMUL))
    #define mul_pclmul mul64_inline
#else
    #define mul_pclmul CONCAT_NB_BITS_GFqn_SUP(PREFIX_NAME(mul))
#endif


#define mul_gf2n mulmod_gf2n



#if (NB_WORD_GFqn==1)
    #define sqr_pclmul sqr64_inline
#else
    #define sqr_pclmul CONCAT_NB_BITS_GFqn_SUP(PREFIX_NAME(sqr))
#endif

#define sqr_gf2n squaremod_gf2n



#endif
