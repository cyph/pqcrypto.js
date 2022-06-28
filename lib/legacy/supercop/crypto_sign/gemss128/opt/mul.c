#include "mul.h"


uint64_t square64low(uint64_t A) 
{
    uint64_t C;
    SQUARE64LOW(C,A);
    return C;
}


uint64_t mul32(uint64_t A, uint64_t B) 
{
    uint64_t C;
    unsigned int i;
    MUL32(C,A,B,i);
    return C;
}

uint64_t mul64low(uint64_t A, uint64_t B) 
{
    uint64_t C;
    unsigned int i;
    MUL64LOW(C,A,B,i);
    return C;
}

#if !ENABLE_GF2X

void mul64(uint64_t C[2], uint64_t A, uint64_t B) 
{
    uint64_t tmp;
    unsigned int i;
    MUL64(C,A,B,i,tmp);
}

void mul128(uint64_t C[4], const uint64_t A[2], const uint64_t B[2]) 
{
    uint64_t tmp,AA,BB;
    unsigned int i;
    MUL128(C,A,B,i,tmp,AA,BB);
}

void mul192(uint64_t C[6], const uint64_t A[3], const uint64_t B[3]) 
{
    uint64_t tmp,AA,BB;
    unsigned int i;
    MUL192(C,A,B,i,tmp,AA,BB);
}

void mul256(uint64_t C[8], const uint64_t A[4], const uint64_t B[4]) 
{
    uint64_t tmp,AA[2],BB[2],tmp1[2],tmp2[2];
    unsigned int i;
    MUL256(C,A,B,i,tmp,AA,BB,tmp1,tmp2);
}

#endif

