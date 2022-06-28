#include "rem.h"


/* Requirement for every functions :
    Degree(Pol) <= 2*(k-1)
    k3 < sup(k/2)+1
    k3 <= 32
*/

/* We use 2 modular reductions, so the leading term x^(2*(k-1)) becomes x^(2*(k3-1)) at the end of computations.
   So it needs 2*(k3-1) <= k-1 to have a reduced result
   2*(k3-1) <= k-1  <=>  k3 <= (k+1)/2  <=>  k3 < sup(k/2)+1 because k3 is integer

    For performance, the last remainder used only one word of 64 bits,
    so it also needs 2*(k3-1) <= 63  <=> k3<=32 (because k3 is integer) */


uint64_t rem32_trinom(uint64_t Pol, unsigned int k, unsigned int k3, uint32_t mask)
{
    uint64_t P,Q,R;
    REM32_TRINOM(P,Pol,k,k3,Q,R,mask);
    return P;
}

uint64_t rem64_trinom(const uint64_t Pol[2], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t P,Q,R;
    REM64_TRINOM(P,Pol,k,k3,Q,R,mask);
    return P;
}


void rem96_trinom(uint64_t P[2], const uint64_t Pol[3], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[2],R;
    unsigned int ki=k&63;
    REM96_TRINOM(P,Pol,k3,ki,Q,R,mask);
}


void rem128_trinom(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[2],R;
    unsigned int ki=k&63,ki64=64-ki;
    REM128_TRINOM(P,Pol,k3,ki,ki64,Q,R,mask);
}


void rem160_trinom(uint64_t P[3], const uint64_t Pol[5], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[3],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM160_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask);
}


void rem192_trinom(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[3],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM192_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask);
}


void rem224_trinom(uint64_t P[4], const uint64_t Pol[7], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[4],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM224_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask)
}


void rem256_trinom(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[4],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM256_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask);
}


void rem288_trinom(uint64_t P[5], const uint64_t Pol[9], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[5],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM288_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask);
}


void rem320_trinom(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[5],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM320_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask);
}


void rem352_trinom(uint64_t P[6], const uint64_t Pol[11], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[6],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM352_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask);
}


void rem384_trinom(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k3, uint64_t mask)
{
    uint64_t Q[6],R;
    unsigned int ki=k&63,ki64=64-ki,k364=64-k3;
    REM384_TRINOM(P,Pol,k3,ki,ki64,k364,Q,R,mask);
}



uint64_t rem32_pentanom(uint64_t Pol, unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint32_t mask)
{
    uint64_t P,Q,R;
    REM32_PENTANOM(P,Pol,k,k1,k2,k3,Q,R,mask);
    return P;
}


uint64_t rem64_pentanom(const uint64_t Pol[2], unsigned int k, unsigned int k3, unsigned int k2, unsigned int k1, uint64_t mask)
{
    uint64_t P,Q,R;
    REM64_PENTANOM(P,Pol,k,k1,k2,k3,Q,R,mask);
    return P;
}


void rem96_pentanom(uint64_t P[2], const uint64_t Pol[3], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[2],R;
    unsigned int ki=k&63;
    REM96_PENTANOM(P,Pol,k1,k2,k3,ki,Q,R,mask);
}


void rem128_pentanom(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[2],R;
    unsigned int ki=k&63,ki64=64-ki;
    REM128_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,Q,R,mask);
}


void rem128_pentanom_k128(uint64_t P[2], const uint64_t Pol[4], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t R;
    REM128_PENTANOM_K128(P,Pol,k1,k2,k3,R);
}


void rem160_pentanom(uint64_t P[3], const uint64_t Pol[5], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[3],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM160_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem192_pentanom(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[3],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM192_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem192_pentanom_k192(uint64_t P[3], const uint64_t Pol[6], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t R;
    unsigned int k164=64-k1,k264=64-k2,k364=64-k3;
    REM192_PENTANOM_K192(P,Pol,k1,k2,k3,k164,k264,k364,R);
}


void rem224_pentanom(uint64_t P[4], const uint64_t Pol[7], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[4],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM224_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem256_pentanom(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[4],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM256_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem256_pentanom_k256(uint64_t P[4], const uint64_t Pol[8], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t R;
    unsigned int k164=64-k1,k264=64-k2,k364=64-k3;
    REM256_PENTANOM_K256(P,Pol,k1,k2,k3,k164,k264,k364,R);
}


void rem288_pentanom(uint64_t P[5], const uint64_t Pol[9], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[5],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM288_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem320_pentanom(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[5],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM320_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem320_pentanom_k320(uint64_t P[5], const uint64_t Pol[10], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t R;
    unsigned int k164=64-k1,k264=64-k2,k364=64-k3;
    REM320_PENTANOM_K320(P,Pol,k1,k2,k3,k164,k264,k364,R);
}


void rem352_pentanom(uint64_t P[6], const uint64_t Pol[11], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[6],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM352_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem384_pentanom(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t Q[6],R;
    unsigned int ki=k&63,ki64=64-ki,k164=64-k1,k264=64-k2,k364=64-k3;
    REM384_PENTANOM(P,Pol,k1,k2,k3,ki,ki64,k164,k264,k364,Q,R,mask);
}


void rem384_pentanom_k384(uint64_t P[6], const uint64_t Pol[12], unsigned int k, unsigned int k1, unsigned int k2, unsigned int k3, uint64_t mask)
{
    uint64_t R;
    unsigned int k164=64-k1,k264=64-k2,k364=64-k3;
    REM384_PENTANOM_K384(P,Pol,k1,k2,k3,k164,k264,k364,R);
}
