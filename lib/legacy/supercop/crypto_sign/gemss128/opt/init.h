#ifndef _INIT_H
#define _INIT_H

/* init.h: macros to initialize */


#include "add_gf2x.h"


/** A negative integer for the failure of a memory allocation. */
#define ERROR_ALLOC (-2)

/** Verify if the allocation by malloc or calloc succeeds. 
 *  Exit in the failure case. */
#define VERIFY_ALLOC(p) \
    if(!p) \
    {\
        exit(ERROR_ALLOC);\
    }

/** Verify if the allocation by malloc or calloc succeeds. 
 *  Return ERROR_ALLOC in the failure case. */
#define VERIFY_ALLOC_RET(p) \
    if(!p) \
    {\
        return(ERROR_ALLOC);\
    }



#define PERMUTATION(ADD2,a,b,SIZE) \
    ADD2(a,b,SIZE); \
    ADD2(b,a,SIZE); \
    ADD2(a,b,SIZE);

#define COPY(c,a,SIZE) \
    FOR_LOOP((c)[RESERVED_VARIABLE]=(a)[RESERVED_VARIABLE],SIZE)

#define SET0(c,SIZE) \
    FOR_LOOP((c)[RESERVED_VARIABLE]=0UL,SIZE)

#define SET1(c,SIZE) \
    (c)[0]=1UL;\
    FOR_LOOP((c)[RESERVED_VARIABLE+1]=0UL,(SIZE)-1)


#if (NB_BITS_UINT==64U)

/* Copy */
#define COPY64(c,a) \
    (c)[0]=(a)[0];

#define COPY128(c,a) \
    COPY64(c,a);\
    (c)[1]=(a)[1];

#define COPY192(c,a) \
    COPY128(c,a); \
    (c)[2]=(a)[2];

#define COPY256(c,a) \
    COPY192(c,a); \
    (c)[3]=(a)[3];

#define COPY320(c,a) \
    COPY256(c,a); \
    (c)[4]=(a)[4];

#define COPY384(c,a) \
    COPY320(c,a); \
    (c)[5]=(a)[5];

#define COPY448(c,a) \
    COPY384(c,a); \
    (c)[6]=(a)[6];

#define COPY512(c,a) \
    COPY448(c,a); \
    (c)[7]=(a)[7];

#define COPY576(c,a) \
    COPY512(c,a); \
    (c)[8]=(a)[8];


/* Init to 0 */
#define SET0_64(c) \
    (c)[0]=0UL;

#define SET0_128(c) \
    SET0_64(c);\
    (c)[1]=0UL;

#define SET0_192(c) \
    SET0_128(c);\
    (c)[2]=0UL;

#define SET0_256(c) \
    SET0_192(c);\
    (c)[3]=0UL;

#define SET0_320(c) \
    SET0_256(c);\
    (c)[4]=0UL;

#define SET0_384(c) \
    SET0_320(c);\
    (c)[5]=0UL;

#define SET0_448(c) \
    SET0_384(c);\
    (c)[6]=0UL;

/* Init to 1 */
#define SET1_64(c) \
    (c)[0]=1UL; \

#define SET1_128(c) \
    SET1_64(c);\
    SET0_64(c+1);

#define SET1_192(c) \
    SET1_64(c);\
    SET0_128(c+1);

#define SET1_256(c) \
    SET1_64(c);\
    SET0_192(c+1);

#define SET1_320(c) \
    SET1_64(c);\
    SET0_256(c+1);

#define SET1_384(c) \
    SET1_64(c);\
    SET0_320(c+1);


/* Permutation */
/* a,b = b,a */
#define PERMUTATION_(ADD2,a,b) \
    ADD2(a,b); \
    ADD2(b,a); \
    ADD2(a,b);

#define PERMUTATION64(a,b) PERMUTATION_(ADD64_2,a,b);
#define PERMUTATION64_TAB(a,b) PERMUTATION_(ADD64_TAB2,a,b);
#define PERMUTATION128(a,b) PERMUTATION_(ADD128_2,a,b);
#define PERMUTATION192(a,b) PERMUTATION_(ADD192_2,a,b);
#define PERMUTATION256(a,b) PERMUTATION_(ADD256_2,a,b);
#define PERMUTATION320(a,b) PERMUTATION_(ADD320_2,a,b);
#define PERMUTATION384(a,b) PERMUTATION_(ADD384_2,a,b);
#define PERMUTATION448(a,b) PERMUTATION_(ADD448_2,a,b);

#endif



#endif
