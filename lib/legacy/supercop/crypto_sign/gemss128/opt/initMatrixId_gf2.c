#include "initMatrixId_gf2.h"

/* Initialize a matrix n*n to identity matrix */



/*  Input:
        S a matrix n*n in GF(2)

    Output:
        S is the identity
*/
#define INITMATID(NAME,MATRIX_SIZE,nq,nr,NB_WORD_n) \
void NAME(Mn_gf2 S)\
{\
    Mn_gf2 S_cp;\
    UINT bit_ir;\
    unsigned int iq,ir;\
    S_cp=S;\
\
    /* Initialize to 0 */\
    for(iq=0;iq<MATRIX_SIZE;++iq)\
    {\
        S[iq]=0UL;\
    }\
\
    /* for each row excepted the last block */\
    for(iq=0;iq<nq;++iq)\
    {\
        bit_ir=1UL;\
        for(ir=0;ir<NB_BITS_UINT;++ir)\
        {\
            *S_cp=bit_ir;\
            /* Next row */\
            S_cp+=NB_WORD_n;\
            bit_ir<<=1;\
        }\
        /* Next block of column */\
        ++S_cp;\
    }\
\
    /* for each row of the last block */\
    bit_ir=1UL;\
    for(ir=0;ir<nr;++ir)\
    {\
        *S_cp=bit_ir;\
        /* Next row */\
        S_cp+=NB_WORD_n;\
        bit_ir<<=1;\
    }\
}

INITMATID(PREFIX_NAME(initMatrixIdn_gf2),MATRIXn_SIZE,HFEnq,HFEnr,NB_WORD_GFqn);
INITMATID(PREFIX_NAME(initMatrixIdnv_gf2),MATRIXnv_SIZE,HFEnvq,HFEnvr,NB_WORD_GF2nv);


