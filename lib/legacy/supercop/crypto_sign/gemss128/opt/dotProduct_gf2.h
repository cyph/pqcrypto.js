#ifndef _DOTPRODUCT_GF2_H
#define _DOTPRODUCT_GF2_H

#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "tools_gf2n.h"




/* Compute the Hamming weight of n */

/* The fastest method */
#ifdef ENABLED_POPCNT
    #define COUNTBITS64_POP(n) \
        n=_mm_popcnt_u64(n);
#endif

/* The second fastest method */
/* Variable-precision SWAR algorithm */
/* 12 operations */
/* https://stackoverflow.com/questions/2709430/count-number-of-bits-in-a-64-bit-long-big-integer */
#define COUNTBITS64_SWAR(n) \
    n-=(n >> 1U) & 0x5555555555555555UL;\
    n=(n & 0x3333333333333333UL) + ((n >> 2U) & 0x3333333333333333UL);\
    n=(((n + (n >> 4U)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56U;


/* Choose the method to compute the Hamming weight */
#ifdef ENABLED_POPCNT
    #define COUNTBITS64 COUNTBITS64_POP
#else
    #define COUNTBITS64 COUNTBITS64_SWAR
#endif



/* Compute the parity of the Hamming weight of n */

/* The fastest method */
#ifdef ENABLED_POPCNT
    #define XORBITS64_POP(n) \
        COUNTBITS64_POP(n); \
        n&=1UL;
#endif

/* The second fastest method (the performance tests are assumed that 
   the constants are already in registers, it is not necessarily true) */
/* 13 logical operations */
#define XORBITS64_SWAR(n) \
    COUNTBITS64_SWAR(n); \
    n&=1UL

/* The third fastest method */
/* 13 logical operations */
#define XORBITS64_DICHO(n) \
    n^=n >> 32U;\
    n^=n >> 16U;\
    n^=n >> 8U;\
    n^=n >> 4U;\
    n^=n >> 2U;\
    n^=n >> 1U;\
    n&=1UL;



#if (NB_BITS_UINT==64U)
    /* Choose the method to compute the parity of the Hamming weight */
    #ifdef ENABLED_POPCNT
        #define XORBITS64 XORBITS64_POP
    #else
        #define XORBITS64 XORBITS64_SWAR
    #endif
#else
    /* A generic method using the principle of XORBITS64_DICHO */
    #define XORBITS(n,SIZE) \
        FOR_LOOP_COMPLETE(SIZE,RESERVED_VARIABLE>0,RESERVED_VARIABLE>>1U,n^=n>>RESERVED_VARIABLE)\
        n&=(UINT)1;

    #define XORBITS64(n) XORBITS(n,NB_BITS_UINT)
#endif







/* Dot product of vector of bits */
#define DOTPRODUCT64(res,a,b) \
    res=(a)[0]&(b)[0];\
    XORBITS64(res);

#define DOTPRODUCT128(res,a,b) \
    res=(a)[0]&(b)[0];\
    res^=(a)[1]&(b)[1];\
    XORBITS64(res);

#define DOTPRODUCT192(res,a,b) \
    res=(a)[0]&(b)[0];\
    res^=(a)[1]&(b)[1];\
    res^=(a)[2]&(b)[2];\
    XORBITS64(res);

#define DOTPRODUCT256(res,a,b) \
    res=(a)[0]&(b)[0];\
    res^=(a)[1]&(b)[1];\
    res^=(a)[2]&(b)[2];\
    res^=(a)[3]&(b)[3];\
    XORBITS64(res);

#define DOTPRODUCT320(res,a,b) \
    res=(a)[0]&(b)[0];\
    res^=(a)[1]&(b)[1];\
    res^=(a)[2]&(b)[2];\
    res^=(a)[3]&(b)[3];\
    res^=(a)[4]&(b)[4];\
    XORBITS64(res);

#define DOTPRODUCT384(res,a,b) \
    res=(a)[0]&(b)[0];\
    res^=(a)[1]&(b)[1];\
    res^=(a)[2]&(b)[2];\
    res^=(a)[3]&(b)[3];\
    res^=(a)[4]&(b)[4];\
    res^=(a)[5]&(b)[5];\
    XORBITS64(res);


#define DOTPRODUCT(res,a,b,SIZE) \
    res=(a)[0]&(b)[0];\
    FOR_LOOP_COMPLETE(1,RESERVED_VARIABLE<(SIZE),++RESERVED_VARIABLE,res^=(a)[RESERVED_VARIABLE]&(b)[RESERVED_VARIABLE])\
    XORBITS64(res);



#if (NB_WORD_GFqn<7)
    #define dotProduct_gf2_n CONCAT_NB_BITS_GFqn_SUP(DOTPRODUCT)
#else
    #define dotProduct_gf2_n(res,a,b) DOTPRODUCT(res,a,b,NB_WORD_GFqn)
#endif





#endif
