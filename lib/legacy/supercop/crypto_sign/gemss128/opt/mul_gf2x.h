#ifndef _MUL_GF2X_H
#define _MUL_GF2X_H

#include "prefix_name.h"
#include <stdint.h>
#include "arch.h"
#include "sqr_gf2x.h"
#include "simd_intel.h"

/* Multiplication in GF(2)[X] */
/* The macros use classical multiplication */

uint64_t PREFIX_NAME(mul64low)(uint64_t A, uint64_t B);
#define mul64low PREFIX_NAME(mul64low)

#if ENABLED_GF2X
    #include <gf2x/gf2x_mul1.h>
    #include <gf2x/gf2x_mul2.h>
    #include <gf2x/gf2x_mul3.h>
    #include <gf2x/gf2x_mul4.h>
    #include <gf2x/gf2x_mul5.h>
    #include <gf2x/gf2x_mul6.h>
    #include <gf2x/gf2x_mul7.h>
    #include <gf2x/gf2x_mul8.h>
    #include <gf2x/gf2x_mul9.h>
    #define mul64(C,A,B) gf2x_mul1(C,*(A),*(B))
    #define mul128(C,A,B) gf2x_mul2(C,A,B)
    #define mul192(C,A,B) gf2x_mul3(C,A,B)
    #define mul256(C,A,B) gf2x_mul4(C,A,B)
    #define mul320(C,A,B) gf2x_mul5(C,A,B)
    #define mul384(C,A,B) gf2x_mul6(C,A,B)
    #define mul448(C,A,B) gf2x_mul7(C,A,B)
    #define mul512(C,A,B) gf2x_mul8(C,A,B)
    #define mul576(C,A,B) gf2x_mul9(C,A,B)

    #define mul96 mul128
    #define mul160 mul192
    #define mul224 mul256
    #define mul288 mul320
    #define mul352 mul384
    #define mul416 mul448
    #define mul480 mul512
    #define mul544 mul576
#else
    void PREFIX_NAME(mul64)(uint64_t C[2], const uint64_t A[1], const uint64_t B[1]);
    void PREFIX_NAME(mul128)(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]);
    void PREFIX_NAME(mul192)(uint64_t C[6], const uint64_t A[3], const uint64_t B[3]);
    void PREFIX_NAME(mul256)(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]);
    void PREFIX_NAME(mul320)(uint64_t C[10], const uint64_t A[5], const uint64_t B[5]);
    void PREFIX_NAME(mul384)(uint64_t C[12], const uint64_t A[6], const uint64_t B[6]);
    void PREFIX_NAME(mul448)(uint64_t C[14], const uint64_t A[7], const uint64_t B[7]);
    void PREFIX_NAME(mul512)(uint64_t C[16], const uint64_t A[8], const uint64_t B[8]);
    void PREFIX_NAME(mul576)(uint64_t C[18], const uint64_t A[9], const uint64_t B[9]);

    #define mul64 PREFIX_NAME(mul64)
    #define mul128 PREFIX_NAME(mul128)
    #define mul192 PREFIX_NAME(mul192)
    #define mul256 PREFIX_NAME(mul256)
    #define mul320 PREFIX_NAME(mul320)
    #define mul384 PREFIX_NAME(mul384)
    #define mul448 PREFIX_NAME(mul448)
    #define mul512 PREFIX_NAME(mul512)
    #define mul576 PREFIX_NAME(mul576)

    #ifdef ENABLED_PCLMUL
        #define mul64_cst(C,A,B) STORE128(C,_mm_clmulepi64_si128(_mm_set_epi64x(0,A),_mm_set_epi64x(0,B),0))
        #define mul64_inline(C,A,B) STORE128(C,_mm_clmulepi64_si128(LOAD64(A),LOAD64(B),0))
        void PREFIX_NAME(mul96)(uint64_t C[3], const uint64_t A[2], const uint64_t B[2]);
        void PREFIX_NAME(mul160)(uint64_t C[5], const uint64_t A[3], const uint64_t B[3]);
        void PREFIX_NAME(mul224)(uint64_t C[7], const uint64_t A[4], const uint64_t B[4]);
        void PREFIX_NAME(mul288)(uint64_t C[9], const uint64_t A[5], const uint64_t B[5]);
        void PREFIX_NAME(mul352)(uint64_t C[11], const uint64_t A[6], const uint64_t B[6]);
        void PREFIX_NAME(mul416)(uint64_t C[13], const uint64_t A[7], const uint64_t B[7]);
        void PREFIX_NAME(mul480)(uint64_t C[15], const uint64_t A[8], const uint64_t B[8]);
        void PREFIX_NAME(mul544)(uint64_t C[17], const uint64_t A[9], const uint64_t B[9]);

        #define mul96 PREFIX_NAME(mul96)
        #define mul160 PREFIX_NAME(mul160)
        #define mul224 PREFIX_NAME(mul224)
        #define mul288 PREFIX_NAME(mul288)
        #define mul352 PREFIX_NAME(mul352)
        #define mul416 PREFIX_NAME(mul416)
        #define mul480 PREFIX_NAME(mul480)
        #define mul544 PREFIX_NAME(mul544)
    #else
        #define mul96 mul128
        #define mul160 mul192
        #define mul224 mul256
    #endif
#endif



#ifdef ENABLED_PCLMUL

#define mul32 mul64low


/* C is the result, a tabular of word of 64 bits */
/* pos is the 3th argument in _mm_clmulepi64_si128 intrinsics */
/* The other variables are __m128i, with x and y initialized (to do x*y) */

/* To understand better how to use, look the file mul.c */



/***********************************************************************/
/***********************************************************************/
/********************** store at the end version ***********************/
/***********************************************************************/
/***********************************************************************/

#define MUL128_WS MUL128_WS_CLASSICAL
#define MUL128_ADD MUL128_ADD_CLASSICAL

#if PROC_HASWELL
    #define MUL192_WS MUL192_WS_KAR

    #define MUL256_WS MUL256_WS_KAR
    #define MUL256_ADD MUL256_ADD_KAR
#else
    #define MUL192_WS MUL192_WS_CLASSICAL

    #define MUL256_WS MUL256_WS_CLASSICAL
    #define MUL256_ADD MUL256_ADD_CLASSICAL
#endif

/* 4 mul64, 3 add128 */
#define MUL128_WS_CLASSICAL(z1,z2,x,y,sum,res_low,res_high) \
    /* X^0 */\
    res_low =_mm_clmulepi64_si128(x,y,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x, y, 1);\
    z2 = _mm_clmulepi64_si128(x, y, 0x10);\
    res_high =PXOR_(z1,z2);\
\
    /* mid2_low: x^64 ... x^127 */\
    sum=LEFT_SHIFT64(res_high);\
    /* mid2_low + L */\
    z1=PXOR_(res_low,sum);\
\
    /* X^128 */\
    res_low=_mm_clmulepi64_si128(x,y,0x11);\
\
    /* mid2_high: x^128 ... x^191 */\
    sum=RIGHT_SHIFT64(res_high);\
    /* mid2_high + H */\
    z2=PXOR_(res_low,sum);


/* 4 mul64, 5 add128 */
#define MUL128_ADD_CLASSICAL(z3,z4,z1,z2,x,y,sum,res_low,res_high) \
    /* X^0 */\
    res_low =_mm_clmulepi64_si128(x,y,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x, y, 1);\
    z2 = _mm_clmulepi64_si128(x, y, 0x10);\
    res_high =PXOR_(z1,z2);\
\
    /* mid2_low: x^64 ... x^127 */\
    sum=LEFT_SHIFT64(res_high);\
    /* mid2_low + L */\
    z3^=PXOR_(res_low,sum);\
\
    /* X^128 */\
    res_low=_mm_clmulepi64_si128(x,y,0x11);\
\
    /* mid2_high: x^128 ... x^191 */\
    sum=RIGHT_SHIFT64(res_high);\
    /* mid2_high + H */\
    z4^=PXOR_(res_low,sum);


/* Karatsuba: 3 mul64, 6 add128 */
#define MUL128_WS_KAR(z1,z2,x,y,sum,res_low,res_high) \
    /* X^0 */\
    z1=_mm_clmulepi64_si128(x,y,0);\
    /* X^128 */\
    z2=_mm_clmulepi64_si128(x,y,0x11);\
\
    res_low=PXOR_(x,RIGHT_SHIFT64(x));\
    res_high=PXOR_(y,RIGHT_SHIFT64(y));\
\
    sum=_mm_clmulepi64_si128(res_low,res_high,0);\
    PXOR1_2(sum,z1);\
    PXOR1_2(sum,z2);\
\
    PXOR1_2(z1,LEFT_SHIFT64(sum));\
    PXOR1_2(z2,RIGHT_SHIFT64(sum));


#define MUL128_ADD_KAR(z3,z4,z1,z2,x,y,sum,res_low,res_high) \
    /* X^0 */\
    z1=_mm_clmulepi64_si128(x,y,0);\
    /* X^128 */\
    z2=_mm_clmulepi64_si128(x,y,0x11);\
\
    res_low=PXOR_(x,RIGHT_SHIFT64(x));\
    res_high=PXOR_(y,RIGHT_SHIFT64(y));\
\
    sum=_mm_clmulepi64_si128(res_low,res_high,0);\
    PXOR1_2(sum,z1);\
    PXOR1_2(sum,z2);\
\
    PXOR1_2(z1,LEFT_SHIFT64(sum));\
    PXOR1_2(z3,z1);\
    PXOR1_2(z2,RIGHT_SHIFT64(sum));\
    PXOR1_2(z4,z2);


/* 9 mul64, 7 add128 */
#define MUL192_WS_CLASSICAL(z3,z1,z2,x1,x2,y1,y2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z3=PXOR_(res1,sum);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 1);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
\
    /* X^256 */\
    res1=_mm_clmulepi64_si128(x2,y2,0);\
    res2=RIGHT_SHIFT64(sum);\
    z2=PXOR_(res1,res2);


/* Karatsuba with 6 multiplications */
#define MUL192_WS_KAR(z1,z2,z3,x1,x2,y1,y2,sum,res1,res2) \
    {__m128i u31;\
    /* A0*B0 */\
    z1 =_mm_clmulepi64_si128(x1,y1,0);\
    /* A1*B1 */\
    z2 =_mm_clmulepi64_si128(x1,y1,0x11);\
    /* A2*B2 */\
    z3 =_mm_clmulepi64_si128(x2,y2,0);\
\
    res1=PXOR_(z1,z2);\
    res2=PXOR_(z3,z2);\
    z2=PXOR_(res1,z3);\
    /*  C[0] = C0
        C[1] = C1^(C0^C2)
        C[2] = C2^(C1^C3)^C0^C4
        C[3] = C3^(C4^C2)^C1^C5
        C[4] = C4^(C5^C3)
        C[5] = C5 */\
    /* (A1 A1) */\
    u31=_mm_shuffle_epi32(x1,0xEE);\
    /* (B1 B1) */\
    sum=_mm_shuffle_epi32(y1,0xEE);\
    /* (A1 A1) ^ (A2 A0) */\
    PXOR1_2(u31,PXOR_(x2,LEFT_SHIFT64(x1)));\
    /* (B1 B1) ^ (B2 B0) */\
    PXOR1_2(sum,PXOR_(y2,LEFT_SHIFT64(y1)));\
    /* (A0+A1)*(B0+B1) */\
    PXOR1_2(res1,_mm_clmulepi64_si128(u31,sum,0x11));\
    /* (A1+A2)*(B1+B2) */\
    PXOR1_2(res2,_mm_clmulepi64_si128(u31,sum,0));\
    /* (A0+A2)*(B0+B2) */\
    PXOR1_2(z2,_mm_clmulepi64_si128(PXOR_(x1,x2),PXOR_(y1,y2),0));\
\
    PXOR1_2(z1,LEFT_SHIFT64(res1));\
    PXOR1_2(z3,RIGHT_SHIFT64(res2));\
    PXOR1_2(z2,MIDDLE128(res1,res2));}


/* Karatsuba: 9 mul64, 12 add128 */
#define MUL192_WS_KAR9(z1,z2,z3,x1,x2,y1,y2,sum,res1,res2) \
    {__m128i x,y,u31,u32;\
    MUL128_WS(z1,z2,x1,y1,sum,res1,res2);\
    z3=_mm_clmulepi64_si128(x2,y2,0);\
\
    x=PXOR_(x1,x2);\
    y=PXOR_(y1,y2);\
\
    PXOR1_2(z3,z2);\
    z2=PXOR_(z3,z1);\
\
    MUL128_ADD(z2,z3,u31,u32,x,y,sum,res1,res2);}


/* 16 mul64, 13 add128 */
#define MUL256_WS_CLASSICAL(z3,z4,z1,z2,x1,x2,y1,y2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z3=PXOR_(res1,sum);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z4=PXOR_(res1,z2);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x11);\
    z2 = _mm_clmulepi64_si128(y2, x2, 0);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res1 =PXOR_(res2,z1);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
\
    /* X^384 */\
    res1 =_mm_clmulepi64_si128(x2,y2,0x11);\
    sum=RIGHT_SHIFT64(res2);\
    z2=PXOR_(res1,sum);


/* 16 mul64, 13 add128 */
/* xor the res to z3,z4,z5,z6 */
#define MUL256_ADD_CLASSICAL(z3,z4,z5,z6,z1,z2,x1,x2,y1,y2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z3^=PXOR_(res1,sum);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z4^=PXOR_(res1,z2);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x11);\
    z2 = _mm_clmulepi64_si128(y2, x2, 0);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res1 =PXOR_(res2,z1);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z5^=PXOR_(res1,z2);\
\
    /* X^384 */\
    res1 =_mm_clmulepi64_si128(x2,y2,0x11);\
    sum=RIGHT_SHIFT64(res2);\
    z6^=PXOR_(res1,sum);


/* Karatsuba: 12 mul64, 16 add128 */
#define MUL256_WS_KAR(z1,z2,z3,z4,x1,x2,y1,y2,sum,res1,res2) \
    {__m128i x,y,u41,u42;\
    MUL128_WS(z1,z2,x1,y1,sum,res1,res2);\
    MUL128_WS(z3,z4,x2,y2,sum,res1,res2);\
\
    x=PXOR_(x1,x2);\
    y=PXOR_(y1,y2);\
\
    PXOR1_2(z3,z2);\
    z2=PXOR_(z3,z1);\
    PXOR1_2(z3,z4);\
\
    MUL128_ADD(z2,z3,u41,u42,x,y,sum,res1,res2);}


/* Karatsuba: 12 mul64, 16 add128 */
#define MUL256_ADD_KAR(z1,z2,z3,z4,z5,z6,x1,x2,y1,y2,sum,res1,res2) \
    {__m128i x,y,u41,u42;\
    MUL128_WS(u41,u42,x1,y1,sum,res1,res2);\
    MUL128_WS(z5,z6,x2,y2,sum,res1,res2);\
\
    x=PXOR_(x1,x2);\
    y=PXOR_(y1,y2);\
\
    PXOR1_2(z1,u41);\
    PXOR1_2(z4,z6);\
\
    PXOR1_2(z5,u42);\
    PXOR1_2(z2,PXOR_(z5,u41));\
    PXOR1_2(z3,PXOR_(z5,z6));\
\
    MUL128_ADD(z2,z3,u41,u42,x,y,sum,res1,res2);}


/* 25 mul64, 21 add128 */
#define MUL320_WS(z3,z4,z5,z1,z2,x1,x2,x3,y1,y2,y3,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z3=PXOR_(res1,sum);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z4=PXOR_(res1,z2);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =PXOR_(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =PXOR_(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z5=PXOR_(res1,z2);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0);\
    res1 =PXOR_(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 1);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
\
    /* X^512 */\
    res1 =_mm_clmulepi64_si128(x3,y3,0);\
    res2=RIGHT_SHIFT64(sum);\
    z2=PXOR_(res1,res2);


/* 25 mul64, 21 add128 */
/* xor the res to z3,z4,z5,z6,z7 */
#define MUL320_ADD(z3,z4,z5,z6,z7,z1,z2,x1,x2,x3,y1,y2,y3,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z3^=PXOR_(res1,sum);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z4^=PXOR_(res1,z2);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =PXOR_(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =PXOR_(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z5^=PXOR_(res1,z2);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0);\
    res1 =PXOR_(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 1);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z6^=PXOR_(res1,z2);\
\
    /* X^512 */\
    res1 =_mm_clmulepi64_si128(x3,y3,0);\
    res2=RIGHT_SHIFT64(sum);\
    z7^=PXOR_(res1,res2);


/* 36 mul64, 31 add128 */
#define MUL384_WS(z3,z4,z5,z6,z1,z2,x1,x2,x3,y1,y2,y3,sum,res1,res2)\
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z3=PXOR_(res1,sum);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z4=PXOR_(res1,z2);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =PXOR_(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =PXOR_(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x1, y3, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 1);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z5=PXOR_(res1,z2);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y3, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    res1 =PXOR_(z1,sum);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0);\
    sum =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x11);\
    res1 =PXOR_(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x2, y3, 1);\
    sum =PXOR_(z1,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    z1 =PXOR_(sum,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 1);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z6=PXOR_(res1,z2);\
\
    /* X^512 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0x11);\
    z2 = _mm_clmulepi64_si128(x3, y3, 0);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0x11);\
    res1 =PXOR_(res2,z1);\
\
    /* X^576 */\
    z1 = _mm_clmulepi64_si128(x3, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x3, y3, 1);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
\
    /* X^640 */\
    res1 =_mm_clmulepi64_si128(x3,y3,0x11);\
    sum=RIGHT_SHIFT64(res2);\
    z2=PXOR_(res1,sum);


/* 49 mul64, 43 add128 */
#define MUL448_WS_CLASSICAL(z3,z4,z5,z6,z7,z1,z2,x1,x2,x3,x4,y1,y2,y3,y4,sum,res1,res2)\
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z3=PXOR_(res1,sum);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z4=PXOR_(res1,z2);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =PXOR_(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =PXOR_(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x1, y3, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 1);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z5=PXOR_(res1,z2);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x1, y4, 0);\
    z2 = _mm_clmulepi64_si128(x1, y3, 0x11);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y3, 0);\
    res1 =PXOR_(z1,sum);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    sum =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0);\
    res1 =PXOR_(z1,sum);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0x11);\
    sum =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x4, y1, 0);\
    res1 =PXOR_(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x1, y4, 1);\
    z2 = _mm_clmulepi64_si128(x2, y3, 0x10);\
    sum =PXOR_(z1,z2);\
    z2 = _mm_clmulepi64_si128(x2, y3, 1);\
    z1 =PXOR_(sum,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 1);\
    z1 =PXOR_(sum,z2);\
    z2 = _mm_clmulepi64_si128(x4, y1, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z6=PXOR_(res1,z2);\
\
    /* X^512 */\
    z1 = _mm_clmulepi64_si128(x2, y4, 0);\
    z2 = _mm_clmulepi64_si128(x2, y3, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y3, 0);\
    res1 =PXOR_(res2,z1);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x11);\
    res2 =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x4, y2, 0);\
    res1 =PXOR_(res2,z1);\
\
    /* X^576 */\
    z1 = _mm_clmulepi64_si128(x2, y4, 1);\
    z2 = _mm_clmulepi64_si128(x3, y3, 0x10);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y3, 1);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x4, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z7=PXOR_(res1,z2);\
\
    /* X^640 */\
    z1 = _mm_clmulepi64_si128(x3, y4, 0);\
    z2 = _mm_clmulepi64_si128(x3, y3, 0x11);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x4, y3, 0);\
    res1 =PXOR_(z1,sum);\
\
    /* X^704 */\
    z1 = _mm_clmulepi64_si128(x3, y4, 1);\
    z2 = _mm_clmulepi64_si128(x4, y3, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
\
    /* X^768 */\
    res1 =_mm_clmulepi64_si128(x4,y4,0);\
    res2=RIGHT_SHIFT64(sum);\
    z2=PXOR_(res1,res2);


/* Karatsuba: 41 mul64, 46 add128 */
#define MUL448_WS(z1,z2,z3,z4,z5,z6,z7,x1,x2,x3,x4,y1,y2,y3,y4,sum,res1,res2)\
    {__m128i x1m,x2m,y1m,y2m,t1,t2;\
    MUL256_WS(z1,z2,z3,z4,x1,x2,y1,y2,sum,res1,res2);\
    MUL192_WS(z5,z6,z7,x3,x4,y3,y4,sum,res1,res2);\
\
    x1m=PXOR_(x1,x3);\
    x2m=PXOR_(x2,x4);\
    y1m=PXOR_(y1,y3);\
    y2m=PXOR_(y2,y4);\
\
    z5=PXOR_(z3,z5);\
    z6=PXOR_(z4,z6);\
    z3=PXOR_(z5,z1);\
    z4=PXOR_(z6,z2);\
    z5=PXOR_(z5,z7);\
\
    MUL256_ADD(z3,z4,z5,z6,t1,t2,x1m,x2m,y1m,y2m,sum,res1,res2)};


/* Karatsuba: 48 mul64, 53 add128 */
#define MUL512_WS(z1,z2,z3,z4,z5,z6,z7,z8,x1,x2,x3,x4,y1,y2,y3,y4,sum,res1,res2)\
    {__m128i x1m,x2m,y1m,y2m,t1,t2;\
    MUL256_WS(z1,z2,z3,z4,x1,x2,y1,y2,sum,res1,res2);\
    MUL256_WS(z5,z6,z7,z8,x3,x4,y3,y4,sum,res1,res2);\
\
    x1m=PXOR_(x1,x3);\
    x2m=PXOR_(x2,x4);\
    y1m=PXOR_(y1,y3);\
    y2m=PXOR_(y2,y4);\
\
    z5=PXOR_(z3,z5);\
    z6=PXOR_(z4,z6);\
    z3=PXOR_(z5,z1);\
    z4=PXOR_(z6,z2);\
    z5=PXOR_(z5,z7);\
    z6=PXOR_(z6,z8);\
\
    MUL256_ADD(z3,z4,z5,z6,t1,t2,x1m,x2m,y1m,y2m,sum,res1,res2)};


/* Karatsuba: 66 mul64 */
#define MUL576_WS(z1,z2,z3,z4,z5,z6,z7,z8,z9,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,sum,res1,res2)\
    {__m128i x1m,x2m,y1m,y2m,t1,t2;\
    MUL256_WS(z1,z2,z3,z4,x1,x2,y1,y2,sum,res1,res2);\
    MUL320_WS(z5,z6,z7,z8,z9,x3,x4,x5,y3,y4,y5,sum,res1,res2);\
\
    x1m=PXOR_(x1,x3);\
    x2m=PXOR_(x2,x4);\
    y1m=PXOR_(y1,y3);\
    y2m=PXOR_(y2,y4);\
\
    z5=PXOR_(z3,z5);\
    z6=PXOR_(z4,z6);\
    z3=PXOR_(z5,z1);\
    z4=PXOR_(z6,z2);\
    z5=PXOR_(z5,z7);\
    z6=PXOR_(z6,z8);\
    z7=PXOR_(z7,z9);\
\
    MUL320_ADD(z3,z4,z5,z6,z7,t1,t2,x1m,x2m,x5,y1m,y2m,y5,sum,res1,res2)};



/***********************************************************************/
/***********************************************************************/
/********************** mul and store version **************************/
/***********************************************************************/
/***********************************************************************/


/* 1 mul64 */
#define MUL64LOW(x,y,pos) _mm_cvtsi128_si64x(_mm_clmulepi64_si128(x, y, pos))

#define MUL32(x,y) MUL64LOW(x,y,0)

/* 1 mul64 */
#define MUL64(C,x,y,z,pos) \
    z = _mm_clmulepi64_si128(x, y, pos);\
    STORE128(C,z);



/* 4 mul64, 3 add128 */
#define MUL128_FINALMUL(FINAL_EXTRACT,C,x,y,z1,z2,sum,res_low,res_high) \
    /* X^0 */\
    res_low =_mm_clmulepi64_si128(x,y,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x, y, 1);\
    z2 = _mm_clmulepi64_si128(x, y, 0x10);\
    res_high =PXOR_(z1,z2);\
\
    /* mid2_low: x^64 ... x^127 */\
    sum=LEFT_SHIFT64(res_high);\
    /* mid2_low + L */\
    z1=PXOR_(res_low,sum);\
    STORE128(C,z1);\
\
    /* X^128 */\
    res_low=_mm_clmulepi64_si128(x,y,0x11);\
\
    /* mid2_high: x^128 ... x^191 */\
    sum=RIGHT_SHIFT64(res_high);\
    /* mid2_high + H */\
    z2=PXOR_(res_low,sum);\
    FINAL_EXTRACT;

#define MUL96(C,x,y,z1,z2,sum,res_low,res_high) MUL128_FINALMUL(STORE64(C+2,z2),C,x,y,z1,z2,sum,res_low,res_high)
#define MUL128(C,x,y,z1,z2,sum,res_low,res_high) MUL128_FINALMUL(STORE128(C+2,z2),C,x,y,z1,z2,sum,res_low,res_high)



/* 9 mul64, 7 add128 */
#define MUL192_FINALMUL(FINAL_EXTRACT,C,x1,x2,y1,y2,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=PXOR_(res1,sum);\
    STORE128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 1);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+2,z1);\
\
    /* X^256 */\
    res1=_mm_clmulepi64_si128(x2,y2,0);\
    z1=RIGHT_SHIFT64(sum);\
    z2=PXOR_(res1,z1);\
    FINAL_EXTRACT;


/* Karatsuba with 6 multiplications */
#define MUL192_KAR_FINALMUL(FINAL_EXTRACT,C,x1,x2,y1,y2,z1,z2,sum,res1,res2) \
    {__m128i u31,u333;\
    /* A0*B0 */\
    z1 =_mm_clmulepi64_si128(x1,y1,0);\
    /* A1*B1 */\
    z2 =_mm_clmulepi64_si128(x1,y1,0x11);\
    /* A2*B2 */\
    u333 =_mm_clmulepi64_si128(x2,y2,0);\
\
    res1=PXOR_(z1,z2);\
    res2=PXOR_(u333,z2);\
    z2=PXOR_(res1,u333);\
    /*  C[0] = C0
        C[1] = C1^(C0^C2)
        C[2] = C2^(C1^C3)^C0^C4
        C[3] = C3^(C4^C2)^C1^C5
        C[4] = C4^(C5^C3)
        C[5] = C5 */\
    /* (A1 A1) */\
    u31=_mm_shuffle_epi32(x1,0xEE);\
    /* (B1 B1) */\
    sum=_mm_shuffle_epi32(y1,0xEE);\
    /* (A1 A1) ^ (A2 A0) */\
    PXOR1_2(u31,PXOR_(x2,LEFT_SHIFT64(x1)));\
    /* (B1 B1) ^ (B2 B0) */\
    PXOR1_2(sum,PXOR_(y2,LEFT_SHIFT64(y1)));\
    /* (A0+A1)*(B0+B1) */\
    PXOR1_2(res1,_mm_clmulepi64_si128(u31,sum,0x11));\
    /* (A1+A2)*(B1+B2) */\
    PXOR1_2(res2,_mm_clmulepi64_si128(u31,sum,0));\
    /* (A0+A2)*(B0+B2) */\
    PXOR1_2(z2,_mm_clmulepi64_si128(PXOR_(x1,x2),PXOR_(y1,y2),0));\
\
    PXOR1_2(z1,LEFT_SHIFT64(res1));\
    PXOR1_2(u333,RIGHT_SHIFT64(res2));\
    PXOR1_2(z2,MIDDLE128(res1,res2));\
\
    STORE128(C,z1);\
    FINAL_EXTRACT;\
    STORE128(C+2,z2);}

#if PROC_HASWELL
    #define MUL160(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL192_KAR_FINALMUL(STORE64(C+4,u333),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)
    #define MUL192(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL192_KAR_FINALMUL(STORE128(C+4,u333),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)
#else
    #define MUL160(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL192_FINALMUL(STORE64(C+4,z2),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)
    #define MUL192(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL192_FINALMUL(STORE128(C+4,z2),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)
#endif


/* 16 mul64, 13 add128 */
#define MUL256_FINALMUL(FINAL_EXTRACT,C,x1,x2,y1,y2,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=PXOR_(res1,sum);\
    STORE128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+2,z1);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x11);\
    z2 = _mm_clmulepi64_si128(y2, x2, 0);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res1 =PXOR_(res2,z1);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
    STORE128(C+4,z1);\
\
    /* X^384 */\
    res1 =_mm_clmulepi64_si128(x2,y2,0x11);\
    z1=RIGHT_SHIFT64(res2);\
    z2=PXOR_(res1,z1);\
    FINAL_EXTRACT;


#define MUL224(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL256_FINALMUL(STORE64(C+6,z2),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)
#define MUL256(C,x1,x2,y1,y2,z1,z2,sum,res1,res2) MUL256_FINALMUL(STORE128(C+6,z2),C,x1,x2,y1,y2,z1,z2,sum,res1,res2)


/* 25 mul64, 21 add128 */
#define MUL320_FINALMUL(FINAL_EXTRACT,C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=PXOR_(res1,sum);\
    STORE128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+2,z1);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =PXOR_(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =PXOR_(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 1);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
    STORE128(C+4,z1);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0);\
    res1 =PXOR_(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 1);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+6,z1);\
\
    /* X^512 */\
    res1 =_mm_clmulepi64_si128(x3,y3,0);\
    z1=RIGHT_SHIFT64(sum);\
    z2=PXOR_(res1,z1);\
    FINAL_EXTRACT;


#define MUL288(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL320_FINALMUL(STORE64(C+8,z2),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)
#define MUL320(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL320_FINALMUL(STORE128(C+8,z2),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)


/* 36 mul64, 31 add128 */
#define MUL384_FINALMUL(FINAL_EXTRACT,C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=PXOR_(res1,sum);\
    STORE128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+2,z1);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =PXOR_(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =PXOR_(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x1, y3, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 1);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
    STORE128(C+4,z1);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y3, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    res1 =PXOR_(z1,sum);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0);\
    sum =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x11);\
    res1 =PXOR_(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x2, y3, 1);\
    sum =PXOR_(z1,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    z1 =PXOR_(sum,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 1);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+6,z1);\
\
    /* X^512 */\
    z1 = _mm_clmulepi64_si128(x2, y3, 0x11);\
    z2 = _mm_clmulepi64_si128(x3, y3, 0);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0x11);\
    res1 =PXOR_(res2,z1);\
\
    /* X^576 */\
    z1 = _mm_clmulepi64_si128(x3, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x3, y3, 1);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
    STORE128(C+8,z1);\
\
    /* X^640 */\
    res1 =_mm_clmulepi64_si128(x3,y3,0x11);\
    z1=RIGHT_SHIFT64(res2);\
    z2=PXOR_(res1,z1);\
    FINAL_EXTRACT;


#define MUL352(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL384_FINALMUL(STORE64(C+10,z2),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)
#define MUL384(C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2) MUL384_FINALMUL(STORE128(C+10,z2),C,x1,x2,x3,y1,y2,y3,z1,z2,sum,res1,res2)


/* 49 mul64, 43 add128 */
#define MUL448_FINALMUL(FINAL_EXTRACT,C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2) \
    /* X^0 */\
    res1 =_mm_clmulepi64_si128(x1,y1,0);\
\
    /* X^64 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 1);\
    z2 = _mm_clmulepi64_si128(x1, y1, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    sum=LEFT_SHIFT64(res2);\
    z1=PXOR_(res1,sum);\
    STORE128(C,z1);\
\
    /* X^128 */\
    z1 = _mm_clmulepi64_si128(x1, y1, 0x11);\
    z2 = _mm_clmulepi64_si128(x2, y1, 0);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x1, y2, 0);\
    res1 =PXOR_(sum,z1);\
\
    /* X^192 */\
    z1 = _mm_clmulepi64_si128(x1, y2, 1);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(y1, x2, 1);\
    z2 =PXOR_(sum,z1);\
    z1 = _mm_clmulepi64_si128(y1, x2, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+2,z1);\
\
    /* X^256 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0);\
    z2 = _mm_clmulepi64_si128(x1, y2, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0);\
    res1 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y1, 0x11);\
    res2 =PXOR_(z1,res1);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0);\
    res1 =PXOR_(res2,z2);\
\
    /* X^320 */\
    z1 = _mm_clmulepi64_si128(x1, y3, 0x10);\
    z2 = _mm_clmulepi64_si128(x1, y3, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y2, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x2, y2, 1);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y1, 0x10);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x3, y1, 1);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
    STORE128(C+4,z1);\
\
    /* X^384 */\
    z1 = _mm_clmulepi64_si128(x1, y4, 0);\
    z2 = _mm_clmulepi64_si128(x1, y3, 0x11);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x2, y3, 0);\
    res1 =PXOR_(z1,sum);\
    z2 = _mm_clmulepi64_si128(x2, y2, 0x11);\
    sum =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y2, 0);\
    res1 =PXOR_(z1,sum);\
    z2 = _mm_clmulepi64_si128(x3, y1, 0x11);\
    sum =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x4, y1, 0);\
    res1 =PXOR_(z1,sum);\
\
    /* X^448 */\
    z1 = _mm_clmulepi64_si128(x1, y4, 1);\
    z2 = _mm_clmulepi64_si128(x2, y3, 0x10);\
    sum =PXOR_(z1,z2);\
    z2 = _mm_clmulepi64_si128(x2, y3, 1);\
    z1 =PXOR_(sum,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x10);\
    sum =PXOR_(z1,z2);\
    z2 = _mm_clmulepi64_si128(x3, y2, 1);\
    z1 =PXOR_(sum,z2);\
    z2 = _mm_clmulepi64_si128(x4, y1, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+6,z1);\
\
    /* X^512 */\
    z1 = _mm_clmulepi64_si128(x2, y4, 0);\
    z2 = _mm_clmulepi64_si128(x2, y3, 0x11);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y3, 0);\
    res1 =PXOR_(res2,z1);\
    z2 = _mm_clmulepi64_si128(x3, y2, 0x11);\
    res2 =PXOR_(res1,z2);\
    z1 = _mm_clmulepi64_si128(x4, y2, 0);\
    res1 =PXOR_(res2,z1);\
\
    /* X^576 */\
    z1 = _mm_clmulepi64_si128(x2, y4, 1);\
    z2 = _mm_clmulepi64_si128(x3, y3, 0x10);\
    res2 =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x3, y3, 1);\
    z2 =PXOR_(res2,z1);\
    z1 = _mm_clmulepi64_si128(x4, y2, 0x10);\
    res2 =PXOR_(z1,z2);\
\
    z2=MIDDLE128(sum,res2);\
    z1=PXOR_(res1,z2);\
    STORE128(C+8,z1);\
\
    /* X^640 */\
    z1 = _mm_clmulepi64_si128(x3, y4, 0);\
    z2 = _mm_clmulepi64_si128(x3, y3, 0x11);\
    sum =PXOR_(z1,z2);\
    z1 = _mm_clmulepi64_si128(x4, y3, 0);\
    res1 =PXOR_(z1,sum);\
\
    /* X^704 */\
    z1 = _mm_clmulepi64_si128(x3, y4, 1);\
    z2 = _mm_clmulepi64_si128(x4, y3, 0x10);\
    sum =PXOR_(z1,z2);\
\
    z2=MIDDLE128(res2,sum);\
    z1=PXOR_(res1,z2);\
    STORE128(C+10,z1);\
\
    /* X^768 */\
    res1 =_mm_clmulepi64_si128(x4,y4,0);\
    res2=RIGHT_SHIFT64(sum);\
    z2=PXOR_(res1,res2);\
    FINAL_EXTRACT;


#define MUL416_CLASSICAL(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2) \
        MUL448_FINALMUL(STORE64(C+12,z2),C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)

#define MUL448_CLASSICAL(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2) \
        MUL448_FINALMUL(STORE128(C+12,z2),C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)


/* Karatsuba: 41 mul64, 46 add128 */
#define MUL448_KARATSUBA_FINALMUL(FINAL_STORE,C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)\
    {__m128i x1m,x2m,y1m,y2m,t1,t2,z3,z4,z5,z6,z7;\
    MUL256_WS(z1,z2,z3,z4,x1,x2,y1,y2,sum,res1,res2);\
    MUL192_WS(z5,z6,z7,x3,x4,y3,y4,sum,res1,res2);\
\
    x1m=PXOR_(x1,x3);\
    x2m=PXOR_(x2,x4);\
    y1m=PXOR_(y1,y3);\
    y2m=PXOR_(y2,y4);\
\
    z5=PXOR_(z3,z5);\
    z6=PXOR_(z4,z6);\
    z3=PXOR_(z5,z1);\
    z4=PXOR_(z6,z2);\
    z5=PXOR_(z5,z7);\
\
    MUL256_ADD(z3,z4,z5,z6,t1,t2,x1m,x2m,y1m,y2m,sum,res1,res2);\
\
    STORE128(C,z1);\
    STORE128(C+2,z2);\
    STORE128(C+4,z3);\
    STORE128(C+6,z4);\
    STORE128(C+8,z5);\
    STORE128(C+10,z6);\
    FINAL_STORE;}


#define MUL416(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2) \
        MUL448_KARATSUBA_FINALMUL(STORE64(C+12,z7),C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)

#define MUL448(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2) \
        MUL448_KARATSUBA_FINALMUL(STORE128(C+12,z7),C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)


/* Karatsuba: 48 mul64, 53 add128 */
#define MUL512_FINALMUL(FINAL_STORE,C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)\
    {__m128i x1m,x2m,y1m,y2m,t1,t2,z3,z4,z5,z6,z7,z8;\
    MUL256_WS(z1,z2,z3,z4,x1,x2,y1,y2,sum,res1,res2);\
    MUL256_WS(z5,z6,z7,z8,x3,x4,y3,y4,sum,res1,res2);\
\
    x1m=PXOR_(x1,x3);\
    x2m=PXOR_(x2,x4);\
    y1m=PXOR_(y1,y3);\
    y2m=PXOR_(y2,y4);\
\
    z5=PXOR_(z3,z5);\
    z6=PXOR_(z4,z6);\
    z3=PXOR_(z5,z1);\
    z4=PXOR_(z6,z2);\
    z5=PXOR_(z5,z7);\
    z6=PXOR_(z6,z8);\
\
    MUL256_ADD(z3,z4,z5,z6,t1,t2,x1m,x2m,y1m,y2m,sum,res1,res2);\
\
    STORE128(C,z1);\
    STORE128(C+2,z2);\
    STORE128(C+4,z3);\
    STORE128(C+6,z4);\
    STORE128(C+8,z5);\
    STORE128(C+10,z6);\
    STORE128(C+12,z7);\
    FINAL_STORE;}


#define MUL480(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2) \
        MUL512_FINALMUL(STORE64(C+14,z8),C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)

#define MUL512(C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2) \
        MUL512_FINALMUL(STORE128(C+14,z8),C,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,sum,res1,res2)


/* Karatsuba: 66 mul64 */
#define MUL576_FINALMUL(FINAL_STORE,C,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2)\
    {__m128i x1m,x2m,y1m,y2m,t1,t2,z3,z4,z5,z6,z7,z8,z9;\
    MUL256_WS(z1,z2,z3,z4,x1,x2,y1,y2,sum,res1,res2);\
    MUL320_WS(z5,z6,z7,z8,z9,x3,x4,x5,y3,y4,y5,sum,res1,res2);\
\
    x1m=PXOR_(x1,x3);\
    x2m=PXOR_(x2,x4);\
    y1m=PXOR_(y1,y3);\
    y2m=PXOR_(y2,y4);\
\
    z5=PXOR_(z3,z5);\
    z6=PXOR_(z4,z6);\
    z3=PXOR_(z5,z1);\
    z4=PXOR_(z6,z2);\
    z5=PXOR_(z5,z7);\
    z6=PXOR_(z6,z8);\
    z7=PXOR_(z7,z9);\
\
    MUL320_ADD(z3,z4,z5,z6,z7,t1,t2,x1m,x2m,x5,y1m,y2m,y5,sum,res1,res2);\
\
    STORE128(C,z1);\
    STORE128(C+2,z2);\
    STORE128(C+4,z3);\
    STORE128(C+6,z4);\
    STORE128(C+8,z5);\
    STORE128(C+10,z6);\
    STORE128(C+12,z7);\
    STORE128(C+14,z8);\
    FINAL_STORE;}


#define MUL544(C,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2) \
        MUL576_FINALMUL(STORE64(C+16,z9),C,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2)

#define MUL576(C,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2) \
        MUL576_FINALMUL(STORE128(C+16,z9),C,x1,x2,x3,x4,x5,y1,y2,y3,y4,y5,z1,z2,sum,res1,res2)


#else

/***********************************************************************/
/***********************************************************************/
/********************** version without vectorization ******************/
/***********************************************************************/
/***********************************************************************/

uint64_t PREFIX_NAME(mul32)(uint64_t A, uint64_t B);
#define mul32 PREFIX_NAME(mul32)

/* initialize C */
#define MUL32(C,A,B,i) \
    (C)=(-((B)&1))&(A); \
    for(i=1;i<32;++i)\
    {\
        (C)^=((-(((B)>>i)&1UL))&(A))<<i; \
    }

/* initialize C */
#define MUL64LOW(C,A,B,i) \
    (C)=(-((B)&1))&A; \
    for(i=1;i<64;++i)\
    {\
        (C)^=((-(((B)>>i)&1UL))&A)<<i; \
    }

/* initialize C */
#define MUL64(C,A,B,i,tmp) \
    (C)[0]=(-((B)&1UL))&A; \
    tmp=((-(((B)>>63)&1UL))&A);\
    (C)[0]^=tmp<<63; \
    (C)[1]=tmp>>1; \
    for(i=1;i<63;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C)[0]^=tmp<<i; \
        (C)[1]^=tmp>>(64-i); \
    }

/* initialize C[0] only */
#define MUL64_INIT1(C,A,B,i,tmp) \
    (C)[0]=(-((B)&1UL))&A; \
    for(i=1;i<64;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C)[0]^=tmp<<i; \
        (C)[1]^=tmp>>(64-i); \
    }

/* No initialize C */
#define MUL64_NOINIT(C,A,B,i,tmp) \
    (C)[0]^=(-((B)&1UL))&A; \
    for(i=1;i<64;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C)[0]^=tmp<<i; \
        (C)[1]^=tmp>>(64-i); \
    }

/* C0 is the low part of result and C1 is the high part */
/* No initialize C0 and C1 */
#define MUL64_NOINIT_SPLIT(C0,C1,A,B,i,tmp) \
    (C0)^=(-((B)&1UL))&A; \
    for(i=1;i<64;++i)\
    {\
        tmp=((-(((B)>>i)&1UL))&A);\
        (C0)^=tmp<<i; \
        (C1)^=tmp>>(64-i); \
    }


/* Karatsuba */
/* initialize C */
#define MUL128(C,A,B,i,tmp,AA,BB) \
    MUL64(C+2,(A)[1],(B)[1],i,tmp);\
    MUL64_INIT1(C+1,(A)[0],(B)[0],i,tmp);\
    /*  C[0] = unitialised
        C[1] = C0
        C[2] = C1^C2
        C[3] = C3 */\
    (C)[0]=(C)[1];\
    (C)[1]^=(C)[2];\
    (C)[2]^=(C)[3];\
    /*  C[0] = C0
        C[1] = C0^C1^C2
        C[2] = C1^C2^C3
        C[3] = C3 */\
    AA=(A)[0]^(A)[1];\
    BB=(B)[0]^(B)[1];\
    MUL64_NOINIT(C+1,AA,BB,i,tmp);

/* No initialize C */
#define MUL128_NOINIT(C,A,B,i,tmp,AA,BB) \
    MUL64(AA,(A)[0],(B)[0],i,tmp);\
    MUL64(BB,(A)[1],(B)[1],i,tmp);\
    /* C1^C2 */\
    (AA)[1]^=(BB)[0];\
    /* ^=C0 */\
    (C)[0]^=(AA)[0];\
    /* ^=C0^(C1^C2) */\
    (C)[1]^=(AA)[0]^(AA)[1];\
    /* ^=C3^(C1^C2) */\
    (C)[2]^=(BB)[1]^(AA)[1];\
    /* ^=C3 */\
    (C)[3]^=(BB)[1];\
    (AA)[0]^=(A)[1];\
    (BB)[0]^=(B)[1];\
    MUL64_NOINIT(C+1,(AA)[0],(BB)[0],i,tmp);

/* Karatsuba with 6 multiplications */
/* initialize C */
#define MUL192(C,A,B,i,tmp,AA,BB) \
    /* A0*B0 */\
    MUL64(C,(A)[0],(B)[0],i,tmp);\
    /* A2*B2 */\
    MUL64(C+4,(A)[2],(B)[2],i,tmp);\
    /* A1*B1 */\
    MUL64_NOINIT_SPLIT(C[1],C[4],(A)[1],(B)[1],i,tmp)\
    (C)[2]=(C)[4];\
    (C)[3]=(C)[1];\
    /*  C[0] = C0
        C[1] = C1^C2
        C[2] = C4^C3
        C[3] = C1^C2
        C[4] = C4^C3
        C[5] = C5 */\
    (C)[1]^=(C)[0];\
    (C)[4]^=(C)[5];\
    (C)[2]^=(C)[1];\
    (C)[3]^=(C)[4];\
    /*  C[0] = C0
        C[1] = C0^(C1^C2)
        C[2] = (C0^C1^C2)^(C3^C4)
        C[3] = (C1^C2)^(C3^C4^C5)
        C[4] = (C3^C4)^C5
        C[5] = C5 */\
    AA=(A)[0]^(A)[1];\
    BB=(B)[0]^(B)[1];\
    /* (A0+A1)*(B0+B1) */\
    MUL64_NOINIT(C+1,AA,BB,i,tmp);\
    AA=(A)[1]^(A)[2];\
    BB=(B)[1]^(B)[2];\
    /* (A1+A2)*(B1+B2) */\
    MUL64_NOINIT(C+3,AA,BB,i,tmp);\
    AA=(A)[0]^(A)[2];\
    BB=(B)[0]^(B)[2];\
    /* (A0+A2)*(B0+B2) */\
    MUL64_NOINIT(C+2,AA,BB,i,tmp);

/* Karatsuba */
/* initialize C */
#define MUL256(C,A,B,i,tmp,AA,BB,tmp1,tmp2) \
    MUL128(C,A,B,i,tmp,BB[0],BB[1]);\
    MUL128(C+4,A+2,B+2,i,tmp,AA[0],AA[1]);\
    C[2]^=C[4];\
    C[3]^=C[5];\
    C[4]=C[2];\
    C[5]=C[3];\
    C[2]^=C[0];\
    C[3]^=C[1];\
    C[4]^=C[6];\
    C[5]^=C[7];\
    AA[0]=A[0]^A[2];\
    AA[1]=A[1]^A[3];\
    BB[0]=B[0]^B[2];\
    BB[1]=B[1]^B[3];\
    MUL128_NOINIT(C+2,AA,BB,i,tmp,tmp1,tmp2);

/* Here it misses a code for others mul, so use gf2x for these sizes */


#endif


#endif
