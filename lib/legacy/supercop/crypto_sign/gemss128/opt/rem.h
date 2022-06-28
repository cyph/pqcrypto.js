#ifndef _REM_H
#define _REM_H

#include <stdint.h>
#include "add.h"


/* Macros to compute the modular reduction with words of 64 bits */
/* Modular reduction by trinom or pentanom */
/* Trinom: x^k + x^k3 + 1 */
/* Pentanom: x^k + x^k3 + x^k2 + x^k1 + 1 */

/* Look rem.c to know requirements of macros */

/* To compute the variable mask, use the macro: mask32(k) or mask64(k&63) */



/********** Trinom : mod x^k + x^k3 + 1 **********/
uint64_t rem32_trinom(uint64_t Pol, unsigned int k, unsigned int k3, uint32_t mask);
uint64_t rem64_trinom(const uint64_t Pol[2], unsigned int k, unsigned int k3, uint64_t mask);
void rem96_trinom(uint64_t P[2], const uint64_t Pol[3], unsigned int k, unsigned int k3, uint64_t mask);
void rem128_trinom(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k3, uint64_t mask);
void rem160_trinom(uint64_t P[3], const uint64_t Pol[5], unsigned int k, unsigned int k3, uint64_t mask);
void rem192_trinom(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k3, uint64_t mask);
void rem224_trinom(uint64_t P[4], const uint64_t Pol[7], unsigned int k, unsigned int k3, uint64_t mask);
void rem256_trinom(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k3, uint64_t mask);
void rem288_trinom(uint64_t P[5], const uint64_t Pol[9], unsigned int k, unsigned int k3, uint64_t mask);
void rem320_trinom(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k3, uint64_t mask);
void rem352_trinom(uint64_t P[6], const uint64_t Pol[11], unsigned int k, unsigned int k3, uint64_t mask);
void rem384_trinom(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k3, uint64_t mask);

/********** Pentanom : mod x^k + x^k3 + x^k2 + x^k1 + 1 **********/
uint64_t rem32_pentanom(uint64_t Pol, unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint32_t mask);
uint64_t rem64_pentanom(const uint64_t Pol[2], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
uint64_t rem64_pentanom_k64(const uint64_t Pol[2], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void rem96_pentanom(uint64_t P[2], const uint64_t Pol[3], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void rem128_pentanom(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void rem128_pentanom_k128(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask);
void rem160_pentanom(uint64_t P[3], const uint64_t Pol[5], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem192_pentanom(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem192_pentanom_k192(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem224_pentanom(uint64_t P[4], const uint64_t Pol[7], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem256_pentanom(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem256_pentanom_k256(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem288_pentanom(uint64_t P[5], const uint64_t Pol[9], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem320_pentanom(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem320_pentanom_k320(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem352_pentanom(uint64_t P[6], const uint64_t Pol[11], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem384_pentanom(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);
void rem384_pentanom_k320(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask);

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
#define REM32_TRINOM_K3_1(P,Pol,k,Q,mask) \
    Q=(Pol)>>k; \
    P=(Pol)&mask; \
    P^=Q; \
    P^=Q<<1;


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
    Q=(Pol[0]>>k)^(Pol[1]<<(64-k)); \
    P=Pol[0]^Q; \
    P^=Q<<k3; \
    R=Q>>(k-k3); \
    P^=R; \
    P^=R<<k3; \
    P&=mask;


#define REM64_PENTANOM(P,Pol,k,k1,k2,k3,Q,R,mask) \
    Q=(Pol[0]>>k)^(Pol[1]<<(64-k)); \
    P=Pol[0]^Q; \
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
    P=Pol[0]^Pol[1];\
    P^=Pol[1]<<k1; \
    P^=Pol[1]<<k2; \
    P^=Pol[1]<<k3; \
    R=Pol[1]>>(k-k1); \
    R^=Pol[1]>>(k-k2); \
    R^=Pol[1]>>(k-k3); \
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
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>(64-k3))^(Q[1]<<k3);\
    /* x^k = x^k3 + 1 */\
    R=(ki>=k3)?Q[1]>>(ki-k3):(Q[0]>>(64-(k3-ki)))^(Q[1]<<(k3-ki));\
    /* x^k = 1 */\
    P[0]^=R;\
    /* x^k = x^k3 */\
    P[0]^=R<<k3;\
    P[1]&=mask;

#define REM96_TRINOM(P,Pol,k3,ki,Q,R,mask) \
    MACRO_128_TRINOM(Q[0]=(Pol[1]>>ki)^(Pol[2]<<(64-ki)),Q[1]=Pol[2]>>ki,P,Pol,k3,ki,Q,R,mask)

#define REM128_TRINOM(P,Pol,k3,ki,ki64,Q,R,mask) \
    MACRO_128_TRINOM(Q[0]=(Pol[1]>>ki)^(Pol[2]<<(ki64)),Q[1]=(Pol[2]>>ki)^(Pol[3]<<ki64),P,Pol,k3,ki,Q,R,mask)


#define MACRO_128_PENTANOM(ROW1,ROW2,P,Pol,k1,k2,k3,ki,ki64,Q,R,mask) \
    /* x^k = x^k3 + x^k2 + x^k1 + 1 */\
    ROW1;\
    ROW2;\
    /* x^k = 1 */\
    ADD128(P,Pol,Q);\
    /* x^k = x^k1 */\
    P[0]^=Q[0]<<k1;\
    P[1]^=(Q[0]>>(64-k1))^(Q[1]<<k1);\
    /* x^k = x^k2 */\
    P[0]^=Q[0]<<k2;\
    P[1]^=(Q[0]>>(64-k2))^(Q[1]<<k2);\
    /* x^k = x^k3 */\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>(64-k3))^(Q[1]<<k3);\
    /* x^k = x^k3 + x^k2 + x^k1 + 1 */\
    R=(ki>=k3)?Q[1]>>(ki-k3):(Q[0]>>(64-(k3-ki)))^(Q[1]<<(k3-ki));\
    R^=(ki>=k2)?Q[1]>>(ki-k2):(Q[0]>>(64-(k2-ki)))^(Q[1]<<(k2-ki));\
    R^=(ki>=k1)?Q[1]>>(ki-k1):(Q[0]>>(64-(k1-ki)))^(Q[1]<<(k1-ki));\
    /* x^k = 1 */\
    P[0]^=R;\
    /* x^k = x^k1 */\
    P[0]^=R<<k1;\
    /* x^k = x^k2 */\
    P[0]^=R<<k2;\
    /* x^k = x^k3 */\
    P[0]^=R<<k3;\
    P[1]&=mask;

#define REM96_PENTANOM(P,Pol,k1,k2,k3,ki,Q,R,mask) \
    MACRO_128_PENTANOM(Q[0]=(Pol[1]>>ki)^(Pol[2]<<(64-ki)),Q[1]=(Pol[2]>>ki),P,Pol,k1,k2,k3,ki,,Q,R,mask)

#define REM128_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,Q,R,mask) \
    MACRO_128_PENTANOM(Q[0]=(Pol[1]>>ki)^(Pol[2]<<ki64),Q[1]=(Pol[2]>>ki)^(Pol[3]<<ki64),P,Pol,k1,k2,k3,ki,ki64,Q,R,mask)


/* Special case k==128 */
#define REM128_PENTANOM_K128(P,Pol,k1,k2,k3,R) \
    ADD128(P,Pol,Pol+2);\
    P[0]^=Pol[2]<<k1;\
    P[1]^=(Pol[2]>>(64-k1))^(Pol[3]<<k1);\
    P[0]^=Pol[2]<<k2;\
    P[1]^=(Pol[2]>>(64-k2))^(Pol[3]<<k2);\
    P[0]^=Pol[2]<<k3;\
    P[1]^=(Pol[2]>>(64-k3))^(Pol[3]<<k3);\
    R=Pol[3]>>(64-k3);\
    R^=Pol[3]>>(64-k2);\
    /* Useless if k1==1 */\
    R^=Pol[3]>>(64-k1);\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;


#define MACRO_192_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=(Pol[2]>>ki)^(Pol[3]<<ki64);\
    Q[1]=(Pol[3]>>ki)^(Pol[4]<<ki64);\
    ROW;\
    ADD192(P,Pol,Q);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[2]>>(ki-k3):(Q[1]>>(k364+ki))^(Q[2]<<(k3-ki));\
    P[0]^=R;\
    P[0]^=R<<k3;\
    P[2]&=mask;

#define REM160_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_192_TRINOM(Q[2]=Pol[4]>>ki,P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM192_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_192_TRINOM(Q[2]=(Pol[4]>>ki)^(Pol[5]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


#define MACRO_192_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=(Pol[2]>>ki)^(Pol[3]<<ki64);\
    Q[1]=(Pol[3]>>ki)^(Pol[4]<<ki64);\
    ROW;\
    ADD192(P,Pol,Q);\
    P[0]^=Q[0]<<k1;\
    P[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    P[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    P[0]^=Q[0]<<k2;\
    P[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    P[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[2]>>(ki-k3):(Q[1]>>(k364+ki))^(Q[2]<<(k3-ki));\
    R^=(ki>=k2)?Q[2]>>(ki-k2):(Q[1]>>(k264+ki))^(Q[2]<<(k2-ki));\
    R^=(ki>=k1)?Q[2]>>(ki-k1):(Q[1]>>(k164+ki))^(Q[2]<<(k1-ki));\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;\
    P[2]&=mask;

#define REM160_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_192_PENTANOM(Q[2]=(Pol[4]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM192_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_192_PENTANOM(Q[2]=(Pol[4]>>ki)^(Pol[5]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==192 */
#define REM192_PENTANOM_K192(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD192(P,Pol,Pol+3);\
    P[0]^=Pol[3]<<k1;\
    P[1]^=(Pol[3]>>k164)^(Pol[4]<<k1);\
    P[2]^=(Pol[4]>>k164)^(Pol[5]<<k1);\
    P[0]^=Pol[3]<<k2;\
    P[1]^=(Pol[3]>>k264)^(Pol[4]<<k2);\
    P[2]^=(Pol[4]>>k264)^(Pol[5]<<k2);\
    P[0]^=Pol[3]<<k3;\
    P[1]^=(Pol[3]>>k364)^(Pol[4]<<k3);\
    P[2]^=(Pol[4]>>k364)^(Pol[5]<<k3);\
    R=Pol[5]>>k364;\
    R^=Pol[5]>>k264;\
    /* Useless if k1==1 */\
    R^=Pol[5]>>k164;\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;


#define MACRO_256_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=(Pol[3]>>ki)^(Pol[4]<<ki64);\
    Q[1]=(Pol[4]>>ki)^(Pol[5]<<ki64);\
    Q[2]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    ROW;\
    ADD256(P,Pol,Q);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[3]>>(ki-k3):(Q[2]>>(k364+ki))^(Q[3]<<(k3-ki));\
    P[0]^=R;\
    P[0]^=R<<k3;\
    P[3]&=mask;

#define REM224_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_256_TRINOM(Q[3]=Pol[6]>>ki,P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM256_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_256_TRINOM(Q[3]=(Pol[6]>>ki)^(Pol[7]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


#define MACRO_256_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=(Pol[3]>>ki)^(Pol[4]<<ki64);\
    Q[1]=(Pol[4]>>ki)^(Pol[5]<<ki64);\
    Q[2]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    ROW;\
    ADD256(P,Pol,Q);\
    P[0]^=Q[0]<<k1;\
    P[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    P[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    P[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    P[0]^=Q[0]<<k2;\
    P[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    P[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    P[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[3]>>(ki-k3):(Q[2]>>(k364+ki))^(Q[3]<<(k3-ki));\
    R^=(ki>=k2)?Q[3]>>(ki-k2):(Q[2]>>(k264+ki))^(Q[3]<<(k2-ki));\
    R^=(ki>=k1)?Q[3]>>(ki-k1):(Q[2]>>(k164+ki))^(Q[3]<<(k1-ki));\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;\
    P[3]&=mask;

#define REM224_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_256_PENTANOM(Q[3]=(Pol[6]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM256_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_256_PENTANOM(Q[3]=(Pol[6]>>ki)^(Pol[7]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==256 */
#define REM256_PENTANOM_K256(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD256(P,Pol,Pol+4);\
    P[0]^=Pol[4]<<k1;\
    P[1]^=(Pol[4]>>k164)^(Pol[5]<<k1);\
    P[2]^=(Pol[5]>>k164)^(Pol[6]<<k1);\
    P[3]^=(Pol[6]>>k164)^(Pol[7]<<k1);\
    P[0]^=Pol[4]<<k2;\
    P[1]^=(Pol[4]>>k264)^(Pol[5]<<k2);\
    P[2]^=(Pol[5]>>k264)^(Pol[6]<<k2);\
    P[3]^=(Pol[6]>>k264)^(Pol[7]<<k2);\
    P[0]^=Pol[4]<<k3;\
    P[1]^=(Pol[4]>>k364)^(Pol[5]<<k3);\
    P[2]^=(Pol[5]>>k364)^(Pol[6]<<k3);\
    P[3]^=(Pol[6]>>k364)^(Pol[7]<<k3);\
    R=Pol[7]>>k364;\
    R^=Pol[7]>>k264;\
    /* Useless if k1==1 */\
    R^=Pol[7]>>k164;\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;


#define MACRO_320_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=(Pol[4]>>ki)^(Pol[5]<<ki64);\
    Q[1]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    Q[2]=(Pol[6]>>ki)^(Pol[7]<<ki64);\
    Q[3]=(Pol[7]>>ki)^(Pol[8]<<ki64);\
    ROW;\
    ADD320(P,Pol,Q);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    P[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[4]>>(ki-k3):(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    P[0]^=R;\
    P[0]^=R<<k3;\
    P[4]&=mask;

#define REM288_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_320_TRINOM(Q[4]=(Pol[8]>>ki),P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM320_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_320_TRINOM(Q[4]=(Pol[8]>>ki)^(Pol[9]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


/* Specialized rem for K3<64 */
#define REM288_SPECIALIZED_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=(Pol[4]>>ki)^(Pol[5]<<ki64);\
    Q[1]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    Q[2]=(Pol[6]>>ki)^(Pol[7]<<ki64);\
    Q[3]=(Pol[7]>>ki)^(Pol[8]<<ki64);\
    Q[4]=(Pol[8]>>ki);\
    ADD320(P,Pol,Q);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    P[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[4]>>(ki-k3):(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    P[0]^=R;\
    P[0]^=R<<k3;\
    /* This row is the uniq difference with REM288_TRINOM */\
    P[1]^=R>>k364;\
    P[4]&=mask;


#define MACRO_320_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=(Pol[4]>>ki)^(Pol[5]<<ki64);\
    Q[1]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    Q[2]=(Pol[6]>>ki)^(Pol[7]<<ki64);\
    Q[3]=(Pol[7]>>ki)^(Pol[8]<<ki64);\
    ROW;\
    ADD320(P,Pol,Q);\
    P[0]^=Q[0]<<k1;\
    P[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    P[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    P[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    P[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    P[0]^=Q[0]<<k2;\
    P[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    P[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    P[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    P[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    P[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[4]>>(ki-k3):(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    R^=(ki>=k2)?Q[4]>>(ki-k2):(Q[3]>>(k264+ki))^(Q[4]<<(k2-ki));\
    R^=(ki>=k1)?Q[4]>>(ki-k1):(Q[3]>>(k164+ki))^(Q[4]<<(k1-ki));\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;\
    P[4]&=mask;

#define REM288_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_320_PENTANOM(Q[4]=(Pol[8]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM320_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_320_PENTANOM(Q[4]=(Pol[8]>>ki)^(Pol[9]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==320 */
#define REM320_PENTANOM_K320(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD320(P,Pol,Pol+5);\
    P[0]^=Pol[5]<<k1;\
    P[1]^=(Pol[5]>>k164)^(Pol[6]<<k1);\
    P[2]^=(Pol[6]>>k164)^(Pol[7]<<k1);\
    P[3]^=(Pol[7]>>k164)^(Pol[8]<<k1);\
    P[4]^=(Pol[8]>>k164)^(Pol[9]<<k1);\
    P[0]^=Pol[5]<<k2;\
    P[1]^=(Pol[5]>>k264)^(Pol[6]<<k2);\
    P[2]^=(Pol[6]>>k264)^(Pol[7]<<k2);\
    P[3]^=(Pol[7]>>k264)^(Pol[8]<<k2);\
    P[4]^=(Pol[8]>>k264)^(Pol[9]<<k2);\
    P[0]^=Pol[5]<<k3;\
    P[1]^=(Pol[5]>>k364)^(Pol[6]<<k3);\
    P[2]^=(Pol[6]>>k364)^(Pol[7]<<k3);\
    P[3]^=(Pol[7]>>k364)^(Pol[8]<<k3);\
    P[4]^=(Pol[8]>>k364)^(Pol[9]<<k3);\
    R=Pol[9]>>k364;\
    R^=Pol[9]>>k264;\
    /* Useless if k1==1 */\
    R^=Pol[9]>>k164;\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;



#define MACRO_384_TRINOM(ROW,P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    Q[1]=(Pol[6]>>ki)^(Pol[7]<<ki64);\
    Q[2]=(Pol[7]>>ki)^(Pol[8]<<ki64);\
    Q[3]=(Pol[8]>>ki)^(Pol[9]<<ki64);\
    Q[4]=(Pol[9]>>ki)^(Pol[10]<<ki64);\
    ROW;\
    ADD384(P,Pol,Q);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    P[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    P[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[5]>>(ki-k3):(Q[4]>>(k364+ki))^(Q[5]<<(k3-ki));\
    P[0]^=R;\
    P[0]^=R<<k3;\
    P[5]&=mask;

#define REM352_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_384_TRINOM(Q[5]=(Pol[10]>>ki),P,Pol,k3,ki,ki64,k364,Q,R,mask)

#define REM384_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    MACRO_384_TRINOM(Q[5]=(Pol[10]>>ki)^(Pol[11]<<ki64),P,Pol,k3,ki,ki64,k364,Q,R,mask)


/* Requirement: k=354, and give k3=k3&63 */
#define REM384_SPECIALIZED_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask) \
    Q[0]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    Q[1]=(Pol[6]>>ki)^(Pol[7]<<ki64);\
    Q[2]=(Pol[7]>>ki)^(Pol[8]<<ki64);\
    Q[3]=(Pol[8]>>ki)^(Pol[9]<<ki64);\
    Q[4]=(Pol[9]>>ki)^(Pol[10]<<ki64);\
    Q[5]=(Pol[10]>>ki)^(Pol[11]<<ki64);\
    ADD384(P,Pol,Q);\
    P[1]^=Q[0]<<k3;\
    P[2]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[3]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[4]^=(Q[2]>>k364)^(Q[3]<<k3);\
    P[5]^=Q[3]>>k364;\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(Q[3]>>(k364+ki))^(Q[4]<<(k3-ki));\
    P[0]^=R;\
    P[1]^=R<<k3;\
    P[2]^=R>>k364;\
    R=(Q[4]>>(k364+ki))^(Q[5]<<(k3-ki));\
    P[1]^=R;\
    P[2]^=R<<k3;\
    P[3]^=R>>k364;\
    P[5]&=mask;


#define MACRO_384_PENTANOM(ROW,P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    Q[0]=(Pol[5]>>ki)^(Pol[6]<<ki64);\
    Q[1]=(Pol[6]>>ki)^(Pol[7]<<ki64);\
    Q[2]=(Pol[7]>>ki)^(Pol[8]<<ki64);\
    Q[3]=(Pol[8]>>ki)^(Pol[9]<<ki64);\
    Q[4]=(Pol[9]>>ki)^(Pol[10]<<ki64);\
    ROW;\
    ADD384(P,Pol,Q);\
    P[0]^=Q[0]<<k1;\
    P[1]^=(Q[0]>>k164)^(Q[1]<<k1);\
    P[2]^=(Q[1]>>k164)^(Q[2]<<k1);\
    P[3]^=(Q[2]>>k164)^(Q[3]<<k1);\
    P[4]^=(Q[3]>>k164)^(Q[4]<<k1);\
    P[5]^=(Q[4]>>k164)^(Q[5]<<k1);\
    P[0]^=Q[0]<<k2;\
    P[1]^=(Q[0]>>k264)^(Q[1]<<k2);\
    P[2]^=(Q[1]>>k264)^(Q[2]<<k2);\
    P[3]^=(Q[2]>>k264)^(Q[3]<<k2);\
    P[4]^=(Q[3]>>k264)^(Q[4]<<k2);\
    P[5]^=(Q[4]>>k264)^(Q[5]<<k2);\
    P[0]^=Q[0]<<k3;\
    P[1]^=(Q[0]>>k364)^(Q[1]<<k3);\
    P[2]^=(Q[1]>>k364)^(Q[2]<<k3);\
    P[3]^=(Q[2]>>k364)^(Q[3]<<k3);\
    P[4]^=(Q[3]>>k364)^(Q[4]<<k3);\
    P[5]^=(Q[4]>>k364)^(Q[5]<<k3);\
    /* 64-(k364+ki) == (k3-ki) */\
    R=(ki>=k3)?Q[5]>>(ki-k3):(Q[4]>>(k364+ki))^(Q[5]<<(k3-ki));\
    R^=(ki>=k2)?Q[5]>>(ki-k2):(Q[4]>>(k264+ki))^(Q[5]<<(k2-ki));\
    R^=(ki>=k1)?Q[5]>>(ki-k1):(Q[4]>>(k164+ki))^(Q[5]<<(k1-ki));\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;\
    P[5]&=mask;

#define REM352_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_384_PENTANOM(Q[5]=(Pol[10]>>ki),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)

#define REM384_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask) \
    MACRO_384_PENTANOM(Q[5]=(Pol[10]>>ki)^(Pol[11]<<ki64),P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask)


/* Special case k==384 */
#define REM384_PENTANOM_K384(P,Pol,k1,k2,k3,k164,k264,k364,R) \
    ADD384(P,Pol,Pol+6);\
    P[0]^=Pol[6]<<k1;\
    P[1]^=(Pol[6]>>k164)^(Pol[7]<<k1);\
    P[2]^=(Pol[7]>>k164)^(Pol[8]<<k1);\
    P[3]^=(Pol[8]>>k164)^(Pol[9]<<k1);\
    P[4]^=(Pol[9]>>k164)^(Pol[10]<<k1);\
    P[5]^=(Pol[10]>>k164)^(Pol[11]<<k1);\
    P[0]^=Pol[6]<<k2;\
    P[1]^=(Pol[6]>>k264)^(Pol[7]<<k2);\
    P[2]^=(Pol[7]>>k264)^(Pol[8]<<k2);\
    P[3]^=(Pol[8]>>k264)^(Pol[9]<<k2);\
    P[4]^=(Pol[9]>>k264)^(Pol[10]<<k2);\
    P[5]^=(Pol[10]>>k264)^(Pol[11]<<k2);\
    P[0]^=Pol[6]<<k3;\
    P[1]^=(Pol[6]>>k364)^(Pol[7]<<k3);\
    P[2]^=(Pol[7]>>k364)^(Pol[8]<<k3);\
    P[3]^=(Pol[8]>>k364)^(Pol[9]<<k3);\
    P[4]^=(Pol[9]>>k364)^(Pol[10]<<k3);\
    P[5]^=(Pol[10]>>k364)^(Pol[11]<<k3);\
    R=Pol[11]>>k364;\
    R^=Pol[11]>>k264;\
    /* Useless if k1==1 */\
    R^=Pol[11]>>k164;\
    P[0]^=R;\
    P[0]^=R<<k1;\
    P[0]^=R<<k2;\
    P[0]^=R<<k3;


#endif
