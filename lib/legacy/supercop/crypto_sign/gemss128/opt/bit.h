#ifndef _BIT_H
#define _BIT_H

#include "arch.h"

/* Macros about computations on the bits of a word */


/* (2^k) - 1, k<32, and -1 for k=0 */
#define mask32(k) ((k)?(1U<<(k))-1U:-1U)


#if (NB_BITS_UINT!=64U)
    #define mask64(k) ((k)?(((UINT)1)<<(k))-((UINT)1):-((UINT)1))

    /** The i-th bit of the integer val. */
    #define ITHBIT(val,i) ((val>>i)&((UINT)1))
#else
/* (2^k) - 1, k<64, and -1 for k=0  */
#define mask64(k) ((k)?(1UL<<(k))-1UL:-1UL)

/** The i-th bit of the integer val. */
#define ITHBIT(val,i) ((val>>i)&1UL)


/** Compute the MSB position of one word. */
/*  Input: U an unsigned integer
    Output: res the MSB position of U. If U is zero, res=0
*/
#define MSB_SP(res,U,j) \
    res=0;\
    /* Search the MSB position of one word */\
    for(j=32;j!=0;j>>=1) \
    {\
        if((U)>>(res^j))\
        {\
            /* To remember the choice of the high part */\
            res^=j;\
        }\
    }

/** Compute the MSB position of a multi-precision integer. */
/*  Input: U an multiple precision unsigned integer, stored on nb_word words
    Output: res the MSB position of U. If U is zero, res=0
*/
#define MSB_MP(res,U,i,j,nb_word) \
    i=nb_word-1;\
    /* Search the first word different from zero */\
    while(i&&(!U[i])) \
    {\
        --i;\
    }\
    /* Search the MSB of one word */\
    MSB_SP(res,U[i],j);\
    res^=i<<6;

#endif


#endif
