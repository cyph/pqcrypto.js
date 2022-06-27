#ifndef _REM_GF2X_H
#define _REM_GF2X_H

#include "prefix_name.h"
#include <stdint.h>
#include "add_gf2x.h"


/* Macros to compute the modular reduction with words of 64 bits */
/* Modular reduction by trinomial or pentanomial */
/* Trinomial: x^k + x^k3 + 1 */
/* Pentanomial: x^k + x^k3 + x^k2 + x^k1 + 1 */

/* Look rem.c to know requirements of macros */

/* To compute the variable mask, use the macro: mask32(k) or mask64(k&63) */



/********** Trinomial : mod x^k + x^k3 + 1 **********/
uint64_t PREFIX_NAME(rem32_trinom)(uint64_t Pol, unsigned int k, unsigned int k3, uint32_t mask);
uint64_t PREFIX_NAME(rem64_trinom)(const uint64_t Pol[2], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem96_trinom)(uint64_t P[2], const uint64_t Pol[3], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem128_trinom)(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem160_trinom)(uint64_t P[3], const uint64_t Pol[5], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem192_trinom)(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem224_trinom)(uint64_t P[4], const uint64_t Pol[7], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem256_trinom)(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem288_trinom)(uint64_t P[5], const uint64_t Pol[9], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem320_trinom)(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem352_trinom)(uint64_t P[6], const uint64_t Pol[11], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem384_trinom)(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem416_trinom)(uint64_t P[7], const uint64_t Pol[13], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem448_trinom)(uint64_t P[7], const uint64_t Pol[14], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem480_trinom)(uint64_t P[8], const uint64_t Pol[15], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem512_trinom)(uint64_t P[8], const uint64_t Pol[16], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem544_trinom)(uint64_t P[9], const uint64_t Pol[17], unsigned int k, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem576_trinom)(uint64_t P[9], const uint64_t Pol[18], unsigned int k, unsigned int k3, uint64_t mask);


#define rem32_trinom PREFIX_NAME(rem32_trinom)
#define rem64_trinom PREFIX_NAME(rem64_trinom)
#define rem96_trinom PREFIX_NAME(rem96_trinom)
#define rem128_trinom PREFIX_NAME(rem128_trinom)
#define rem160_trinom PREFIX_NAME(rem160_trinom)
#define rem192_trinom PREFIX_NAME(rem192_trinom)
#define rem224_trinom PREFIX_NAME(rem224_trinom)
#define rem256_trinom PREFIX_NAME(rem256_trinom)
#define rem288_trinom PREFIX_NAME(rem288_trinom)
#define rem320_trinom PREFIX_NAME(rem320_trinom)
#define rem352_trinom PREFIX_NAME(rem352_trinom)
#define rem384_trinom PREFIX_NAME(rem384_trinom)
#define rem416_trinom PREFIX_NAME(rem416_trinom)
#define rem448_trinom PREFIX_NAME(rem448_trinom)
#define rem480_trinom PREFIX_NAME(rem480_trinom)
#define rem512_trinom PREFIX_NAME(rem512_trinom)
#define rem544_trinom PREFIX_NAME(rem544_trinom)
#define rem576_trinom PREFIX_NAME(rem576_trinom)

/********** Pentanomial : mod x^k + x^k3 + x^k2 + x^k1 + 1 **********/
uint64_t PREFIX_NAME(rem32_pentanom)(uint64_t Pol, unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint32_t mask);
uint64_t PREFIX_NAME(rem64_pentanom)(const uint64_t Pol[2], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
uint64_t PREFIX_NAME(rem64_pentanom_k64)(const uint64_t Pol[2], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void PREFIX_NAME(rem96_pentanom)(uint64_t P[2], const uint64_t Pol[3], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void PREFIX_NAME(rem128_pentanom)(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void PREFIX_NAME(rem128_pentanom_k128)(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void PREFIX_NAME(rem160_pentanom)(uint64_t P[3], const uint64_t Pol[5], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem192_pentanom)(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem192_pentanom_k192)(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem224_pentanom)(uint64_t P[4], const uint64_t Pol[7], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem256_pentanom)(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem256_pentanom_k256)(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem288_pentanom)(uint64_t P[5], const uint64_t Pol[9], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem320_pentanom)(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem320_pentanom_k320)(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem352_pentanom)(uint64_t P[6], const uint64_t Pol[11], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem384_pentanom)(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem384_pentanom_k384)(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem416_pentanom)(uint64_t P[7], const uint64_t Pol[13], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem448_pentanom)(uint64_t P[7], const uint64_t Pol[14], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem448_pentanom_k448)(uint64_t P[7], const uint64_t Pol[14], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem480_pentanom)(uint64_t P[8], const uint64_t Pol[15], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem512_pentanom)(uint64_t P[8], const uint64_t Pol[16], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem544_pentanom)(uint64_t P[9], const uint64_t Pol[17], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void PREFIX_NAME(rem576_pentanom)(uint64_t P[9], const uint64_t Pol[18], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);

#define rem32_pentanom PREFIX_NAME(rem32_pentanom)
#define rem64_pentanom PREFIX_NAME(rem64_pentanom)
#define rem64_pentanom_k64 PREFIX_NAME(rem64_pentanom_k64)
#define rem96_pentanom PREFIX_NAME(rem96_pentanom)
#define rem128_pentanom PREFIX_NAME(rem128_pentanom)
#define rem128_pentanom_k128 PREFIX_NAME(rem128_pentanom_k128)
#define rem160_pentanom PREFIX_NAME(rem160_pentanom)
#define rem192_pentanom PREFIX_NAME(rem192_pentanom)
#define rem192_pentanom_k192 PREFIX_NAME(rem192_pentanom_k192)
#define rem224_pentanom PREFIX_NAME(rem224_pentanom)
#define rem256_pentanom PREFIX_NAME(rem256_pentanom)
#define rem256_pentanom_k256 PREFIX_NAME(rem256_pentanom_k256)
#define rem288_pentanom PREFIX_NAME(rem288_pentanom)
#define rem320_pentanom PREFIX_NAME(rem320_pentanom)
#define rem320_pentanom_k320 PREFIX_NAME(rem320_pentanom_k320)
#define rem352_pentanom PREFIX_NAME(rem352_pentanom)
#define rem384_pentanom PREFIX_NAME(rem384_pentanom)
#define rem384_pentanom_k384 PREFIX_NAME(rem384_pentanom_k384)
#define rem416_pentanom PREFIX_NAME(rem416_pentanom)
#define rem448_pentanom PREFIX_NAME(rem448_pentanom)
#define rem448_pentanom_k448 PREFIX_NAME(rem448_pentanom_k448)
#define rem480_pentanom PREFIX_NAME(rem480_pentanom)
#define rem512_pentanom PREFIX_NAME(rem512_pentanom)
#define rem544_pentanom PREFIX_NAME(rem544_pentanom)
#define rem576_pentanom PREFIX_NAME(rem576_pentanom)

/* P is the result, Pol is the input */
/* Q and R are not initialised */
/* Look rem.c to look an example of how use these macros */


#define REM32_TRINOM(P,Pol,k,k3,Q,R,mask) \
    Q=(Pol)>>k; \
    P=(Pol)^Q; \
    R=Q<<k3; \
    P^=R; \
    R>>=k; \
    P^=R; \
    P^=R<<k3; \
    P&=mask;


/* For K3==1 */
#define REM32_TRINOM_K31(P,Pol,k,k3,Q,R,mask) \
    Q=(Pol)>>k; \
    P=((Pol)&mask)^Q^(Q<<1);

#define REM32_TRINOM_K312(P,Pol,k,k3,Q,R,mask) \
    Q=(Pol)>>k; \
    P=(Pol)&mask; \
    P^=Q; \
    P^=Q<<1;

#define REM32_TRINOM_K3_1(P,Pol,k,Q,mask) REM32_TRINOM_K31(P,Pol,k,k3,Q,R,mask)


#define REM32_PENTANOM(P,Pol,k,k1,k2,k3,Q,R,mask) \
    Q=(Pol)>>k; \
    P=(Pol)&mask; \
    P^=Q; \
    P^=Q<<k1; \
    P^=Q<<k2; \
    P^=Q<<k3; \
    R=P>>k; \
    P^=R; \
    P^=R<<k1; \
    P^=R<<k2; \
    P^=R<<k3; \
    P&=mask;


#define REM64_TRINOM(P,Pol,k,k3,Q,R,mask) \
    Q=((Pol)[0]>>k)^((Pol)[1]<<(64-k)); \
    R=Q>>(k-k3); \
    Q^=R; \
    P=(Pol)[0]^Q; \
    P^=Q<<k3; \
    P&=mask;


/* K3==1 */
#define REM64_TRINOM_K31(P,Pol,k,k3,Q,R,mask) \
    Q=((Pol)[0]>>k)^((Pol)[1]<<(64-k)); \
    P=(Pol)[0]^Q^(Q<<k3);\
    P&=mask;


/*
#define REM64_TRINOM(P,Pol,k,k3,Q,R,mask) \
    Q=((Pol)[0]>>k)^((Pol)[1]<<(64-k)); \
    P=(Pol)[0]^Q; \
    P^=Q<<k3; \
    R=Q>>(k-k3); \
    P^=R; \
    P^=R<<k3; \
    P&=mask;
*/

#define REM64_PENTANOM(P,Pol,k,k1,k2,k3,Q,R,mask) \
    Q=((Pol)[0]>>k)^((Pol)[1]<<(64-k)); \
    P=(Pol)[0]^Q; \
    P^=Q<<k1; \
    P^=Q<<k2; \
    P^=Q<<k3; \
    R=Q>>(k-k1); \
    R^=Q>>(k-k2); \
    R^=Q>>(k-k3); \
    P^=R; \
    P^=R<<k1; \
    P^=R<<k2; \
    P^=R<<k3; \
    P&=mask;


/* Special case k==64 */
#define REM64_PENTANOM_K64(P,Pol,k,k1,k2,k3,R) \
    P=(Pol)[0]^(Pol)[1];\
    P^=(Pol)[1]<<k1; \
    P^=(Pol)[1]<<k2; \
    P^=(Pol)[1]<<k3; \
    R=(Pol)[1]>>(k-k1); \
    R^=(Pol)[1]>>(k-k2); \
    R^=(Pol)[1]>>(k-k3); \
    P^=R; \
    P^=R<<k1; \
    P^=R<<k2; \
    P^=R<<k3;


#define MACRO_128_TRINOM(ROW1,ROW2,P,Pol,k3,ki,Q,R,mask) \
    /* x^k = x^k3 + 1 */\
    ROW1;\
    ROW2;\
    /* x^k = 1 */\
    ADD128(P,Pol,Q);\
    /* x^k = x^k3 */\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>(64-k3))^(Q[1]<<k3);\
    /* x^k = x^k3 + 1 */\
    R=(ki>=k3)?Q[1]>>(ki-k3):(Q[0]>>(64-(k3-ki)))^(Q[1]<<(k3-ki));\
    /* x^k = 1 */\
    (P)[0]^=R;\
    /* x^k = x^k3 */\
    (P)[0]^=R<<k3;\
    (P)[1]&=mask;

#define REM96_TRINOM(P,Pol,k3,ki,Q,R,mask) \
    MACRO_128_TRINOM(Q[0]=((Pol)[1]>>ki)^((Pol)[2]<<(64-ki)),Q[1]=(Pol)[2]>>ki,P,Pol,k3,ki,Q,R,mask)

#define REM128_TRINOM(P,Pol,k3,ki,ki64,Q,R,mask) \
    MACRO_128_TRINOM(Q[0]=((Pol)[1]>>ki)^((Pol)[2]<<(ki64)),Q[1]=((Pol)[2]>>ki)^((Pol)[3]<<ki64),P,Pol,k3,ki,Q,R,mask)


#define MACRO_128_PENTANOM(ROW1,ROW2,P,Pol,k1,k2,k3,ki,ki64,Q,R,mask) \
    /* x^k = x^k3 + x^k2 + x^k1 + 1 */\
    ROW1;\
    ROW2;\
    /* x^k = 1 */\
    ADD128(P,Pol,Q);\
    /* x^k = x^k1 */\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>(64-k1))^(Q[1]<<k1);\
    /* x^k = x^k2 */\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>(64-k2))^(Q[1]<<k2);\
    /* x^k = x^k3 */\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>(64-k3))^(Q[1]<<k3);\
    /* x^k = x^k3 + x^k2 + x^k1 + 1 */\
    R=(ki>=k3)?Q[1]>>(ki-k3):(Q[0]>>(64-(k3-ki)))^(Q[1]<<(k3-ki));\
    R^=(ki>=k2)?Q[1]>>(ki-k2):(Q[0]>>(64-(k2-ki)))^(Q[1]<<(k2-ki));\
    R^=(ki>=k1)?Q[1]>>(ki-k1):(Q[0]>>(64-(k1-ki)))^(Q[1]<<(k1-ki));\
    /* x^k = 1 */\
    (P)[0]^=R;\
    /* x^k = x^k1 */\
    (P)[0]^=R<<k1;\
    /* x^k = x^k2 */\
    (P)[0]^=R<<k2;\
    /* x^k = x^k3 */\
    (P)[0]^=R<<k3;\
    (P)[1]&=mask;

#define REM96_PENTANOM(P,Pol,k1,k2,k3,ki,Q,R,mask) \
    MACRO_128_PENTANOM(Q[0]=((Pol)[1]>>ki)^((Pol)[2]<<(64-ki)),Q[1]=((Pol)[2]>>ki),P,Pol,k1,k2,k3,ki,,Q,R,mask)

#define REM128_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,Q,R,mask) \
    MACRO_128_PENTANOM(Q[0]=((Pol)[1]>>ki)^((Pol)[2]<<ki64),Q[1]=((Pol)[2]>>ki)^((Pol)[3]<<ki64),P,Pol,k1,k2,k3,ki,ki64,Q,R,mask)


/* Special case k==128 */
#define REM128_PENTANOM_K128(P,Pol,k1,k2,k3,R) \
    ADD128(P,Pol,Pol+2);\
    (P)[0]^=(Pol)[2]<<k1;\
    (P)[1]^=((Pol)[2]>>(64-k1))^((Pol)[3]<<k1);\
    (P)[0]^=(Pol)[2]<<k2;\
    (P)[1]^=((Pol)[2]>>(64-k2))^((Pol)[3]<<k2);\
    (P)[0]^=(Pol)[2]<<k3;\
    (P)[1]^=((Pol)[2]>>(64-k3))^((Pol)[3]<<k3);\
    R=(Pol)[3]>>(64-k3);\
    R^=(Pol)[3]>>(64-k2);\
    /* Useless if k1==1 */\
    R^=(Pol)[3]>>(64-k1);\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;


#define MACRO_192_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[2]>>ki)^((Pol)[3]<<ki64);\
    Q[1]=((Pol)[3]>>ki)^((Pol)[4]<<ki64);\
    ROW;\
    ADD192(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[2]>>(ki-k3):(Q[1]>>(k364+ki))^(Q[2]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    (P)[2]&=mask;

#define REM160_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_192_TRINOM(Q[2]=(Pol)[4]>>ki,P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM192_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_192_TRINOM(Q[2]=((Pol)[4]>>ki)^((Pol)[5]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


#define MACRO_192_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[2]>>ki)^((Pol)[3]<<ki64);\
    Q[1]=((Pol)[3]>>ki)^((Pol)[4]<<ki64);\
    ROW;\
    ADD192(P,Pol,Q);\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[2]>>(ki-k3):(Q[1]>>(k364+ki))^(Q[2]<<(k3-ki));\
    R^=(ki>=k2)?Q[2]>>(ki-k2):(Q[1]>>(k264+ki))^(Q[2]<<(k2-ki));\
    R^=(ki>=k1)?Q[2]>>(ki-k1):(Q[1]>>(k164+ki))^(Q[2]<<(k1-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[2]&=mask;

#define REM160_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_192_PENTANOM(Q[2]=((Pol)[4]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM192_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_192_PENTANOM(Q[2]=((Pol)[4]>>ki)^((Pol)[5]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==192 */
#define REM192_PENTANOM_K192(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD192(P,Pol,Pol+3);\
    (P)[0]^=(Pol)[3]<<k1;\
    (P)[1]^=((Pol)[3]>>k164)^((Pol)[4]<<k1);\
    (P)[2]^=((Pol)[4]>>k164)^((Pol)[5]<<k1);\
    (P)[0]^=(Pol)[3]<<k2;\
    (P)[1]^=((Pol)[3]>>k264)^((Pol)[4]<<k2);\
    (P)[2]^=((Pol)[4]>>k264)^((Pol)[5]<<k2);\
    (P)[0]^=(Pol)[3]<<k3;\
    (P)[1]^=((Pol)[3]>>k364)^((Pol)[4]<<k3);\
    (P)[2]^=((Pol)[4]>>k364)^((Pol)[5]<<k3);\
    R=(Pol)[5]>>k364;\
    R^=(Pol)[5]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[5]>>k164;\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;


#define MACRO_256_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[3]>>ki)^((Pol)[4]<<ki64);\
    Q[1]=((Pol)[4]>>ki)^((Pol)[5]<<ki64);\
    Q[2]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    ROW;\
    ADD256(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[3]>>(ki-k3):(Q[2]>>(k364+ki))^(Q[3]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    (P)[3]&=mask;

#define REM224_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_256_TRINOM(Q[3]=(Pol)[6]>>ki,P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM256_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_256_TRINOM(Q[3]=((Pol)[6]>>ki)^((Pol)[7]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


#define MACRO_256_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[3]>>ki)^((Pol)[4]<<ki64);\
    Q[1]=((Pol)[4]>>ki)^((Pol)[5]<<ki64);\
    Q[2]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    ROW;\
    ADD256(P,Pol,Q);\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[3]>>(ki-k3):(Q[2]>>(k364+ki))^(Q[3]<<(k3-ki));\
    R^=(ki>=k2)?Q[3]>>(ki-k2):(Q[2]>>(k264+ki))^(Q[3]<<(k2-ki));\
    R^=(ki>=k1)?Q[3]>>(ki-k1):(Q[2]>>(k164+ki))^(Q[3]<<(k1-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[3]&=mask;

#define REM224_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_256_PENTANOM(Q[3]=((Pol)[6]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM256_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_256_PENTANOM(Q[3]=((Pol)[6]>>ki)^((Pol)[7]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==256 */
#define REM256_PENTANOM_K256(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD256(P,Pol,Pol+4);\
    (P)[0]^=(Pol)[4]<<k1;\
    (P)[1]^=((Pol)[4]>>k164)^((Pol)[5]<<k1);\
    (P)[2]^=((Pol)[5]>>k164)^((Pol)[6]<<k1);\
    (P)[3]^=((Pol)[6]>>k164)^((Pol)[7]<<k1);\
    (P)[0]^=(Pol)[4]<<k2;\
    (P)[1]^=((Pol)[4]>>k264)^((Pol)[5]<<k2);\
    (P)[2]^=((Pol)[5]>>k264)^((Pol)[6]<<k2);\
    (P)[3]^=((Pol)[6]>>k264)^((Pol)[7]<<k2);\
    (P)[0]^=(Pol)[4]<<k3;\
    (P)[1]^=((Pol)[4]>>k364)^((Pol)[5]<<k3);\
    (P)[2]^=((Pol)[5]>>k364)^((Pol)[6]<<k3);\
    (P)[3]^=((Pol)[6]>>k364)^((Pol)[7]<<k3);\
    R=(Pol)[7]>>k364;\
    R^=(Pol)[7]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[7]>>k164;\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;


#define MACRO_320_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[4]>>ki)^((Pol)[5]<<ki64);\
    Q[1]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[2]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[3]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    ROW;\
    ADD320(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[4]>>(ki-k3):(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    (P)[4]&=mask;

#define REM288_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_320_TRINOM(Q[4]=((Pol)[8]>>ki),P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM320_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_320_TRINOM(Q[4]=((Pol)[8]>>ki)^((Pol)[9]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


/* Specialized rem for K3<64 */
#define REM288_SPECIALIZED_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[4]>>ki)^((Pol)[5]<<ki64);\
    Q[1]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[2]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[3]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[4]=((Pol)[8]>>ki);\
    ADD320(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[4]>>(ki-k3):(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    /* This row is the uniq difference with REM288_TRINOM */\
    (P)[1]^=R>>k364;\
    (P)[4]&=mask;


/* Requirement: k=313, and give k3=k3&63 */
#define REM320_SPECIALIZED_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    {uint64_t R2;\
    Q[0]=((Pol)[4]>>ki)^((Pol)[5]<<ki64);\
    Q[1]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[2]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[3]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[4]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    ADD320(P,Pol,Q);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(Q[3]>>(ki-k3))^(Q[4]<<(ki64+k3));\
    (P)[0]^=R;\
    R2=Q[4]>>(ki-k3);\
    (P)[1]^=R2;\
    (P)[1]^=(R^Q[0])<<k3;\
    (P)[2]^=((R^Q[0])>>k364)^((R2^Q[1])<<k3);\
    (P)[3]^=((R2^Q[1])>>k364)^(Q[2]<<k3);\
    (P)[4]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]&=mask;}


#define MACRO_320_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[4]>>ki)^((Pol)[5]<<ki64);\
    Q[1]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[2]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[3]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    ROW;\
    ADD320(P,Pol,Q);\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[4]>>(ki-k3):(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    R^=(ki>=k2)?Q[4]>>(ki-k2):(Q[3]>>(k264+ki))^(Q[4]<<(k2-ki));\
    R^=(ki>=k1)?Q[4]>>(ki-k1):(Q[3]>>(k164+ki))^(Q[4]<<(k1-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[4]&=mask;

#define REM288_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_320_PENTANOM(Q[4]=((Pol)[8]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM320_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_320_PENTANOM(Q[4]=((Pol)[8]>>ki)^((Pol)[9]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==320 */
#define REM320_PENTANOM_K320(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD320(P,Pol,Pol+5);\
    (P)[0]^=(Pol)[5]<<k1;\
    (P)[1]^=((Pol)[5]>>k164)^((Pol)[6]<<k1);\
    (P)[2]^=((Pol)[6]>>k164)^((Pol)[7]<<k1);\
    (P)[3]^=((Pol)[7]>>k164)^((Pol)[8]<<k1);\
    (P)[4]^=((Pol)[8]>>k164)^((Pol)[9]<<k1);\
    (P)[0]^=(Pol)[5]<<k2;\
    (P)[1]^=((Pol)[5]>>k264)^((Pol)[6]<<k2);\
    (P)[2]^=((Pol)[6]>>k264)^((Pol)[7]<<k2);\
    (P)[3]^=((Pol)[7]>>k264)^((Pol)[8]<<k2);\
    (P)[4]^=((Pol)[8]>>k264)^((Pol)[9]<<k2);\
    (P)[0]^=(Pol)[5]<<k3;\
    (P)[1]^=((Pol)[5]>>k364)^((Pol)[6]<<k3);\
    (P)[2]^=((Pol)[6]>>k364)^((Pol)[7]<<k3);\
    (P)[3]^=((Pol)[7]>>k364)^((Pol)[8]<<k3);\
    (P)[4]^=((Pol)[8]>>k364)^((Pol)[9]<<k3);\
    R=(Pol)[9]>>k364;\
    R^=(Pol)[9]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[9]>>k164;\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;


/* Special case k==312 and k3=128 */
#define REM312_PENTANOM_K3_IS_128(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[4]>>ki)^((Pol)[5]<<ki64);\
    Q[1]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[2]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[3]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[4]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    /* R for K1=5 */\
    Q[0]^=(Q)[4]>>51;\
    /* R for K2=15 */\
    Q[0]^=(Q)[4]>>41;\
    /* R for K3=128 */\
    Q[0]^=((Pol)[7]>>48)^(Pol[8]<<16);\
    Q[1]^=((Pol)[8]>>48)^(Pol[9]<<16);\
    ADD320(P,Pol,Q);\
    /* K3=128 */\
    (P)[2]^=Q[0];\
    (P)[3]^=Q[1];\
    (P)[4]^=Q[2];\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    (P)[4]&=mask;



#define MACRO_384_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[1]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[2]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[3]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[4]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    ROW;\
    ADD384(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[5]>>(ki-k3):(Q[4]>>(k364+ki))^(Q[5]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    (P)[5]&=mask;

#define REM352_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_384_TRINOM(Q[5]=((Pol)[10]>>ki),P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM384_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_384_TRINOM(Q[5]=((Pol)[10]>>ki)^((Pol)[11]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


#define REM384_SPECIALIZED358_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[1]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[2]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[3]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[4]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[5]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(Q[4]>>(k364+ki))^(Q[5]<<(k3-ki));\
    Q[0]^=R;\
    ADD384(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364);\
    (P)[5]&=mask;


/* Requirement: k=354, and give k3=k3&63 */
#define REM384_SPECIALIZED_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    {uint64_t R2;\
    Q[0]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[1]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[2]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[3]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[4]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[5]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    ADD384(P,Pol,Q);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    (P)[0]^=R;\
    R2=(Q[4]>>(k364+ki))^(Q[5]<<(k3-ki));\
    (P)[1]^=R2;\
    (P)[1]^=(R^Q[0])<<k3;\
    (P)[2]^=((R^Q[0])>>k364)^((R2^Q[1])<<k3);\
    (P)[3]^=((R2^Q[1])>>k364)^(Q[2]<<k3);\
    (P)[4]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[5]^=Q[3]>>k364;\
    (P)[5]&=mask;}



#define MACRO_384_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[5]>>ki)^((Pol)[6]<<ki64);\
    Q[1]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[2]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[3]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[4]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    ROW;\
    ADD384(P,Pol,Q);\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    (P)[5]^=(Q[4]>>k164)^(Q[5]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    (P)[5]^=(Q[4]>>k264)^(Q[5]<<k2);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[5]>>(ki-k3):(Q[4]>>(k364+ki))^(Q[5]<<(k3-ki));\
    R^=(ki>=k2)?Q[5]>>(ki-k2):(Q[4]>>(k264+ki))^(Q[5]<<(k2-ki));\
    R^=(ki>=k1)?Q[5]>>(ki-k1):(Q[4]>>(k164+ki))^(Q[5]<<(k1-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[5]&=mask;

#define REM352_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_384_PENTANOM(Q[5]=((Pol)[10]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM384_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_384_PENTANOM(Q[5]=((Pol)[10]>>ki)^((Pol)[11]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==384 */
#define REM384_PENTANOM_K384(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD384(P,Pol,Pol+6);\
    (P)[0]^=(Pol)[6]<<k1;\
    (P)[1]^=((Pol)[6]>>k164)^((Pol)[7]<<k1);\
    (P)[2]^=((Pol)[7]>>k164)^((Pol)[8]<<k1);\
    (P)[3]^=((Pol)[8]>>k164)^((Pol)[9]<<k1);\
    (P)[4]^=((Pol)[9]>>k164)^((Pol)[10]<<k1);\
    (P)[5]^=((Pol)[10]>>k164)^((Pol)[11]<<k1);\
    (P)[0]^=(Pol)[6]<<k2;\
    (P)[1]^=((Pol)[6]>>k264)^((Pol)[7]<<k2);\
    (P)[2]^=((Pol)[7]>>k264)^((Pol)[8]<<k2);\
    (P)[3]^=((Pol)[8]>>k264)^((Pol)[9]<<k2);\
    (P)[4]^=((Pol)[9]>>k264)^((Pol)[10]<<k2);\
    (P)[5]^=((Pol)[10]>>k264)^((Pol)[11]<<k2);\
    (P)[0]^=(Pol)[6]<<k3;\
    (P)[1]^=((Pol)[6]>>k364)^((Pol)[7]<<k3);\
    (P)[2]^=((Pol)[7]>>k364)^((Pol)[8]<<k3);\
    (P)[3]^=((Pol)[8]>>k364)^((Pol)[9]<<k3);\
    (P)[4]^=((Pol)[9]>>k364)^((Pol)[10]<<k3);\
    (P)[5]^=((Pol)[10]>>k364)^((Pol)[11]<<k3);\
    R=(Pol)[11]>>k364;\
    R^=(Pol)[11]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[11]>>k164;\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;



#define MACRO_448_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[1]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[2]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[3]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[4]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[5]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    ROW;\
    ADD448(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    (P)[6]^=(Q[5]>>k364)^(Q[6]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[6]>>(ki-k3):(Q[5]>>(k364+ki))^(Q[6]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    (P)[6]&=mask;


#define REM416_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_448_TRINOM(Q[6]=((Pol)[12]>>ki),P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM448_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_448_TRINOM(Q[6]=((Pol)[12]>>ki)^((Pol)[13]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)



/* Requirement: k=402, and give k3=k3&63 */
#define REM402_SPECIALIZED_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[1]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[2]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[3]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[4]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[5]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    Q[6]=((Pol)[12]>>ki);\
\
    Q[0]^=(Q[3]>>39)^(Q[4]<<25);\
    Q[1]^=(Q[4]>>39)^(Q[5]<<25);\
    Q[2]^=(Q[5]>>39)^(Q[6]<<25);\
\
    ADD448(P,Pol,Q);\
\
    (P)[2]^=(Q[0]<<k3);\
    (P)[3]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[4]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[5]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[6]^=Q[3]>>k364;\
    (P)[6]&=mask;



#define MACRO_448_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[6]>>ki)^((Pol)[7]<<ki64);\
    Q[1]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[2]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[3]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[4]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[5]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    ROW;\
    ADD448(P,Pol,Q);\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    (P)[5]^=(Q[4]>>k164)^(Q[5]<<k1);\
    (P)[6]^=(Q[5]>>k164)^(Q[6]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    (P)[5]^=(Q[4]>>k264)^(Q[5]<<k2);\
    (P)[6]^=(Q[5]>>k264)^(Q[6]<<k2);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    (P)[6]^=(Q[5]>>k364)^(Q[6]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[6]>>(ki-k3):(Q[5]>>(k364+ki))^(Q[6]<<(k3-ki));\
    R^=(ki>=k2)?Q[6]>>(ki-k2):(Q[5]>>(k264+ki))^(Q[6]<<(k2-ki));\
    R^=(ki>=k1)?Q[6]>>(ki-k1):(Q[5]>>(k164+ki))^(Q[6]<<(k1-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[6]&=mask;

#define REM416_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_448_PENTANOM(Q[6]=((Pol)[12]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM448_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_448_PENTANOM(Q[6]=((Pol)[12]>>ki)^((Pol)[13]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)



/* Special case k==448 */
#define REM448_PENTANOM_K448(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    R=(Pol)[13]>>k364;\
    R^=(Pol)[13]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[13]>>k164;\
    R^=(Pol)[7];\
    (P)[0]=(Pol)[0]^R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[1]=(Pol)[1]^(Pol)[8];\
    (P)[2]=(Pol)[2]^(Pol)[9];\
    (P)[3]=(Pol)[3]^(Pol)[10];\
    (P)[4]=(Pol)[4]^(Pol)[11];\
    (P)[5]=(Pol)[5]^(Pol)[12];\
    (P)[6]=(Pol)[6]^(Pol)[13];\
    (P)[1]^=((Pol)[7]>>k164)^((Pol)[8]<<k1);\
    (P)[2]^=((Pol)[8]>>k164)^((Pol)[9]<<k1);\
    (P)[3]^=((Pol)[9]>>k164)^((Pol)[10]<<k1);\
    (P)[4]^=((Pol)[10]>>k164)^((Pol)[11]<<k1);\
    (P)[5]^=((Pol)[11]>>k164)^((Pol)[12]<<k1);\
    (P)[6]^=((Pol)[12]>>k164)^((Pol)[13]<<k1);\
    (P)[1]^=((Pol)[7]>>k264)^((Pol)[8]<<k2);\
    (P)[2]^=((Pol)[8]>>k264)^((Pol)[9]<<k2);\
    (P)[3]^=((Pol)[9]>>k264)^((Pol)[10]<<k2);\
    (P)[4]^=((Pol)[10]>>k264)^((Pol)[11]<<k2);\
    (P)[5]^=((Pol)[11]>>k264)^((Pol)[12]<<k2);\
    (P)[6]^=((Pol)[12]>>k264)^((Pol)[13]<<k2);\
    (P)[1]^=((Pol)[7]>>k364)^((Pol)[8]<<k3);\
    (P)[2]^=((Pol)[8]>>k364)^((Pol)[9]<<k3);\
    (P)[3]^=((Pol)[9]>>k364)^((Pol)[10]<<k3);\
    (P)[4]^=((Pol)[10]>>k364)^((Pol)[11]<<k3);\
    (P)[5]^=((Pol)[11]>>k364)^((Pol)[12]<<k3);\
    (P)[6]^=((Pol)[12]>>k364)^((Pol)[13]<<k3);


/* Special case k==448 and k3=64 */
#define REM448_PENTANOM_K448_K3_IS_64(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    R=(Pol)[13];\
    R^=(Pol)[13]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[13]>>k164;\
    R^=(Pol)[7];\
    (P)[0]=(Pol)[0]^R;\
    (P)[1]=(Pol)[1]^R^(Pol)[8];\
    (P)[2]=(Pol)[2]^(Pol)[8]^(Pol)[9];\
    (P)[3]=(Pol)[3]^(Pol)[9]^(Pol)[10];\
    (P)[4]=(Pol)[4]^(Pol)[10]^(Pol)[11];\
    (P)[5]=(Pol)[5]^(Pol)[11]^(Pol)[12];\
    (P)[6]=(Pol)[6]^(Pol)[12]^(Pol)[13];\
    (P)[0]^=R<<k1;\
    (P)[1]^=(R>>k164)^((Pol)[8]<<k1);\
    (P)[2]^=((Pol)[8]>>k164)^((Pol)[9]<<k1);\
    (P)[3]^=((Pol)[9]>>k164)^((Pol)[10]<<k1);\
    (P)[4]^=((Pol)[10]>>k164)^((Pol)[11]<<k1);\
    (P)[5]^=((Pol)[11]>>k164)^((Pol)[12]<<k1);\
    (P)[6]^=((Pol)[12]>>k164)^((Pol)[13]<<k1);\
    (P)[0]^=R<<k2;\
    (P)[1]^=(R>>k264)^((Pol)[8]<<k2);\
    (P)[2]^=((Pol)[8]>>k264)^((Pol)[9]<<k2);\
    (P)[3]^=((Pol)[9]>>k264)^((Pol)[10]<<k2);\
    (P)[4]^=((Pol)[10]>>k264)^((Pol)[11]<<k2);\
    (P)[5]^=((Pol)[11]>>k264)^((Pol)[12]<<k2);\
    (P)[6]^=((Pol)[12]>>k264)^((Pol)[13]<<k2);



#define MACRO_512_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[1]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[2]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[3]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[4]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    Q[5]=((Pol)[12]>>ki)^((Pol)[13]<<ki64);\
    Q[6]=((Pol)[13]>>ki)^((Pol)[14]<<ki64);\
    ROW;\
    ADD512(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    (P)[6]^=(Q[5]>>k364)^(Q[6]<<k3);\
    (P)[7]^=(Q[6]>>k364)^(Q[7]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[7]>>(ki-k3):(Q[6]>>(k364+ki))^(Q[7]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    (P)[7]&=mask;


#define REM480_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_512_TRINOM(Q[7]=((Pol)[14]>>ki),P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM512_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_512_TRINOM(Q[7]=((Pol)[14]>>ki)^((Pol)[15]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)



#define MACRO_512_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[7]>>ki)^((Pol)[8]<<ki64);\
    Q[1]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[2]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[3]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[4]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    Q[5]=((Pol)[12]>>ki)^((Pol)[13]<<ki64);\
    Q[6]=((Pol)[13]>>ki)^((Pol)[14]<<ki64);\
    ROW;\
    ADD512(P,Pol,Q);\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    (P)[5]^=(Q[4]>>k164)^(Q[5]<<k1);\
    (P)[6]^=(Q[5]>>k164)^(Q[6]<<k1);\
    (P)[7]^=(Q[6]>>k164)^(Q[7]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    (P)[5]^=(Q[4]>>k264)^(Q[5]<<k2);\
    (P)[6]^=(Q[5]>>k264)^(Q[6]<<k2);\
    (P)[7]^=(Q[6]>>k264)^(Q[7]<<k2);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    (P)[6]^=(Q[5]>>k364)^(Q[6]<<k3);\
    (P)[7]^=(Q[6]>>k364)^(Q[7]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[7]>>(ki-k3):(Q[6]>>(k364+ki))^(Q[7]<<(k3-ki));\
    R^=(ki>=k2)?Q[7]>>(ki-k2):(Q[6]>>(k264+ki))^(Q[7]<<(k2-ki));\
    R^=(ki>=k1)?Q[7]>>(ki-k1):(Q[6]>>(k164+ki))^(Q[7]<<(k1-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[7]&=mask;

#define REM480_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_512_PENTANOM(Q[7]=((Pol)[14]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM512_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_512_PENTANOM(Q[7]=((Pol)[14]>>ki)^((Pol)[15]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)



/* Special case k==512 */
#define REM512_PENTANOM_K512(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    R=(Pol)[15]>>k364;\
    R^=(Pol)[15]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[15]>>k164;\
    R^=(Pol)[8];\
    (P)[0]=(Pol)[0]^R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[1]=(Pol)[1]^(Pol)[9];\
    (P)[2]=(Pol)[2]^(Pol)[10];\
    (P)[3]=(Pol)[3]^(Pol)[11];\
    (P)[4]=(Pol)[4]^(Pol)[12];\
    (P)[5]=(Pol)[5]^(Pol)[13];\
    (P)[6]=(Pol)[6]^(Pol)[14];\
    (P)[7]=(Pol)[7]^(Pol)[15];\
    (P)[1]^=((Pol)[8]>>k164)^((Pol)[9]<<k1);\
    (P)[2]^=((Pol)[9]>>k164)^((Pol)[10]<<k1);\
    (P)[3]^=((Pol)[10]>>k164)^((Pol)[11]<<k1);\
    (P)[4]^=((Pol)[11]>>k164)^((Pol)[12]<<k1);\
    (P)[5]^=((Pol)[12]>>k164)^((Pol)[13]<<k1);\
    (P)[6]^=((Pol)[13]>>k164)^((Pol)[14]<<k1);\
    (P)[7]^=((Pol)[14]>>k164)^((Pol)[15]<<k1);\
    (P)[1]^=((Pol)[8]>>k264)^((Pol)[9]<<k2);\
    (P)[2]^=((Pol)[9]>>k264)^((Pol)[10]<<k2);\
    (P)[3]^=((Pol)[10]>>k264)^((Pol)[11]<<k2);\
    (P)[4]^=((Pol)[11]>>k264)^((Pol)[12]<<k2);\
    (P)[5]^=((Pol)[12]>>k264)^((Pol)[13]<<k2);\
    (P)[6]^=((Pol)[13]>>k264)^((Pol)[14]<<k2);\
    (P)[7]^=((Pol)[14]>>k264)^((Pol)[15]<<k2);\
    (P)[1]^=((Pol)[8]>>k364)^((Pol)[9]<<k3);\
    (P)[2]^=((Pol)[9]>>k364)^((Pol)[10]<<k3);\
    (P)[3]^=((Pol)[10]>>k364)^((Pol)[11]<<k3);\
    (P)[4]^=((Pol)[11]>>k364)^((Pol)[12]<<k3);\
    (P)[5]^=((Pol)[12]>>k364)^((Pol)[13]<<k3);\
    (P)[6]^=((Pol)[13]>>k364)^((Pol)[14]<<k3);\
    (P)[7]^=((Pol)[14]>>k364)^((Pol)[15]<<k3);


#define MACRO_576_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[1]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[2]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[3]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    Q[4]=((Pol)[12]>>ki)^((Pol)[13]<<ki64);\
    Q[5]=((Pol)[13]>>ki)^((Pol)[14]<<ki64);\
    Q[6]=((Pol)[14]>>ki)^((Pol)[15]<<ki64);\
    Q[7]=((Pol)[15]>>ki)^((Pol)[16]<<ki64);\
    ROW;\
    ADD576(P,Pol,Q);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    (P)[6]^=(Q[5]>>k364)^(Q[6]<<k3);\
    (P)[7]^=(Q[6]>>k364)^(Q[7]<<k3);\
    (P)[8]^=(Q[7]>>k364)^(Q[8]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[8]>>(ki-k3):(Q[7]>>(k364+ki))^(Q[8]<<(k3-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k3;\
    (P)[8]&=mask;



#define REM544_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_576_TRINOM(Q[8]=((Pol)[16]>>ki),P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM576_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_576_TRINOM(Q[8]=((Pol)[16]>>ki)^((Pol)[17]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)



#define MACRO_576_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[1]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[2]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[3]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    Q[4]=((Pol)[12]>>ki)^((Pol)[13]<<ki64);\
    Q[5]=((Pol)[13]>>ki)^((Pol)[14]<<ki64);\
    Q[6]=((Pol)[14]>>ki)^((Pol)[15]<<ki64);\
    Q[7]=((Pol)[15]>>ki)^((Pol)[16]<<ki64);\
    ROW;\
    ADD576(P,Pol,Q);\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    (P)[5]^=(Q[4]>>k164)^(Q[5]<<k1);\
    (P)[6]^=(Q[5]>>k164)^(Q[6]<<k1);\
    (P)[7]^=(Q[6]>>k164)^(Q[7]<<k1);\
    (P)[8]^=(Q[7]>>k164)^(Q[8]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    (P)[5]^=(Q[4]>>k264)^(Q[5]<<k2);\
    (P)[6]^=(Q[5]>>k264)^(Q[6]<<k2);\
    (P)[7]^=(Q[6]>>k264)^(Q[7]<<k2);\
    (P)[8]^=(Q[7]>>k264)^(Q[8]<<k2);\
    (P)[0]^=Q[0]<<k3;\
    (P)[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    (P)[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    (P)[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    (P)[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    (P)[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    (P)[6]^=(Q[5]>>k364)^(Q[6]<<k3);\
    (P)[7]^=(Q[6]>>k364)^(Q[7]<<k3);\
    (P)[8]^=(Q[7]>>k364)^(Q[8]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[8]>>(ki-k3):(Q[7]>>(k364+ki))^(Q[8]<<(k3-ki));\
    R^=(ki>=k2)?Q[8]>>(ki-k2):(Q[7]>>(k264+ki))^(Q[8]<<(k2-ki));\
    R^=(ki>=k1)?Q[8]>>(ki-k1):(Q[7]>>(k164+ki))^(Q[8]<<(k1-ki));\
    (P)[0]^=R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[8]&=mask;

#define REM544_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_576_PENTANOM(Q[8]=((Pol)[16]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM576_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_576_PENTANOM(Q[8]=((Pol)[16]>>ki)^((Pol)[17]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)



/* Special case k==576 */
#define REM576_PENTANOM_K576(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    R=(Pol)[17]>>k364;\
    R^=(Pol)[17]>>k264;\
    /* Useless if k1==1 */\
    R^=(Pol)[17]>>k164;\
    R^=(Pol)[9];\
    (P)[0]=(Pol)[0]^R;\
    (P)[0]^=R<<k1;\
    (P)[0]^=R<<k2;\
    (P)[0]^=R<<k3;\
    (P)[1]=(Pol)[1]^(Pol)[10];\
    (P)[2]=(Pol)[2]^(Pol)[11];\
    (P)[3]=(Pol)[3]^(Pol)[12];\
    (P)[4]=(Pol)[4]^(Pol)[13];\
    (P)[5]=(Pol)[5]^(Pol)[14];\
    (P)[6]=(Pol)[6]^(Pol)[15];\
    (P)[7]=(Pol)[7]^(Pol)[16];\
    (P)[8]=(Pol)[8]^(Pol)[17];\
    (P)[1]^=((Pol)[9]>>k164)^((Pol)[10]<<k1);\
    (P)[2]^=((Pol)[10]>>k164)^((Pol)[11]<<k1);\
    (P)[3]^=((Pol)[11]>>k164)^((Pol)[12]<<k1);\
    (P)[4]^=((Pol)[12]>>k164)^((Pol)[13]<<k1);\
    (P)[5]^=((Pol)[13]>>k164)^((Pol)[14]<<k1);\
    (P)[6]^=((Pol)[14]>>k164)^((Pol)[15]<<k1);\
    (P)[7]^=((Pol)[15]>>k164)^((Pol)[16]<<k1);\
    (P)[8]^=((Pol)[16]>>k164)^((Pol)[17]<<k1);\
    (P)[1]^=((Pol)[9]>>k264)^((Pol)[10]<<k2);\
    (P)[2]^=((Pol)[10]>>k264)^((Pol)[11]<<k2);\
    (P)[3]^=((Pol)[11]>>k264)^((Pol)[12]<<k2);\
    (P)[4]^=((Pol)[12]>>k264)^((Pol)[13]<<k2);\
    (P)[5]^=((Pol)[13]>>k264)^((Pol)[14]<<k2);\
    (P)[6]^=((Pol)[14]>>k264)^((Pol)[15]<<k2);\
    (P)[7]^=((Pol)[15]>>k264)^((Pol)[16]<<k2);\
    (P)[8]^=((Pol)[16]>>k264)^((Pol)[17]<<k2);\
    (P)[1]^=((Pol)[9]>>k364)^((Pol)[10]<<k3);\
    (P)[2]^=((Pol)[10]>>k364)^((Pol)[11]<<k3);\
    (P)[3]^=((Pol)[11]>>k364)^((Pol)[12]<<k3);\
    (P)[4]^=((Pol)[12]>>k364)^((Pol)[13]<<k3);\
    (P)[5]^=((Pol)[13]>>k364)^((Pol)[14]<<k3);\
    (P)[6]^=((Pol)[14]>>k364)^((Pol)[15]<<k3);\
    (P)[7]^=((Pol)[15]>>k364)^((Pol)[16]<<k3);\
    (P)[8]^=((Pol)[16]>>k364)^((Pol)[17]<<k3);


/* Special case k==544 and k3=128 */
#define REM544_PENTANOM_K3_IS_128(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=((Pol)[8]>>ki)^((Pol)[9]<<ki64);\
    Q[1]=((Pol)[9]>>ki)^((Pol)[10]<<ki64);\
    Q[2]=((Pol)[10]>>ki)^((Pol)[11]<<ki64);\
    Q[3]=((Pol)[11]>>ki)^((Pol)[12]<<ki64);\
    Q[4]=((Pol)[12]>>ki)^((Pol)[13]<<ki64);\
    Q[5]=((Pol)[13]>>ki)^((Pol)[14]<<ki64);\
    Q[6]=((Pol)[14]>>ki)^((Pol)[15]<<ki64);\
    Q[7]=((Pol)[15]>>ki)^((Pol)[16]<<ki64);\
    Q[8]=((Pol)[16]>>ki);\
    /* R for K2=3 */\
    Q[0]^=(Pol)[16]>>(k264);\
    /* R for K3=128 */\
    Q[0]^=(Q[6]>>ki)^(Q[7]<<ki64);\
    Q[1]^=(Q[7]>>ki)^(Q[8]<<ki64);\
    ADD576(P,Pol,Q);\
    /* K3=128 */\
    (P)[2]^=Q[0];\
    (P)[3]^=Q[1];\
    (P)[4]^=Q[2];\
    (P)[5]^=Q[3];\
    (P)[6]^=Q[4];\
    (P)[7]^=Q[5];\
    (P)[8]^=Q[6];\
    (P)[0]^=Q[0]<<k1;\
    (P)[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    (P)[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    (P)[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    (P)[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    (P)[5]^=(Q[4]>>k164)^(Q[5]<<k1);\
    (P)[6]^=(Q[5]>>k164)^(Q[6]<<k1);\
    (P)[7]^=(Q[6]>>k164)^(Q[7]<<k1);\
    (P)[8]^=(Q[7]>>k164)^(Q[8]<<k1);\
    (P)[0]^=Q[0]<<k2;\
    (P)[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    (P)[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    (P)[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    (P)[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    (P)[5]^=(Q[4]>>k264)^(Q[5]<<k2);\
    (P)[6]^=(Q[5]>>k264)^(Q[6]<<k2);\
    (P)[7]^=(Q[6]>>k264)^(Q[7]<<k2);\
    (P)[8]^=(Q[7]>>k264)^(Q[8]<<k2);\
    (P)[8]&=mask;


#endif
