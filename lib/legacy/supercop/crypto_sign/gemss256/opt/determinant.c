#include "determinant.h"
#include <stdlib.h>
#include "init.h"

/* To compute the determinant of a square matrix n*n */



/* Operation between two rows */
/* In Gauss elimination, we know that the i first columns are triangular */
/* So the computation is useless for iq words for each row */
/* because we know there are only zeros */
#define LOOPK(op) \
    for(k=0;k<(NB_WORD_GFqn-iq);++k)\
    {\
        op;\
    }\


#define PERMUT(LOOPK) \
        /* pivot == 1 ? */\
        if((*S_cpj)&bit_ir)\
        {\
            LOOPK;\
            break;\
        }

#define ADDROW(LOOPK) \
        /* pivot */\
        pivot=-(((*S_cpj)>>ir)&1);\
        LOOPK;


/* operation LOOPK if the pivot of jth row is 1 */
#define LOOPJ(OP) \
    S_cpj=S_cpi;\
    /* for the next rows */\
    for(j=i+1;j<HFEn;++j)\
    {\
        /* next row */\
        S_cpj+=NB_WORD_GFqn;\
        OP;\
    }


#define LOOPIR(NB_IT,LOOPK1,LOOPK2) \
    bit_ir=1UL;\
    for(ir=0;ir<NB_IT;++ir,++i)\
    {\
        /* If the pivot is 0, search the pivot */\
        if(!((*S_cpi)&bit_ir))\
        {\
            /* permutation row i and j if the pivot of jth row is 1 */\
            LOOPJ(PERMUT(LOOPK1));\
\
            /* If there is not no null pivot */\
            if(j==HFEn)\
            {\
                free(S_cp);\
                /* the determinant is 0 */\
                return 0U;\
            }\
        }\
\
        /* add row i to row j if the pivot of jth row is 1*/\
        LOOPJ(ADDROW(LOOPK2));\
\
        /* Next row */\
        S_cpi+=NB_WORD_GFqn;\
        bit_ir<<=1;\
    }


/*  Input:
        S a matrix n*n in GF(2)

    Output:
        return ERROR_ALLOC if the allocation fails, the determinant of S else
        the determinant of S, computed with Gauss elimination
*/
int determinantn(cst_Mn_gf2 S)
{
    Mn_gf2 S_cp,S_cpi,S_cpj;
    UINT bit_ir,pivot;
    unsigned int i=0,iq,ir,j,k;

    /* In this code, we have : */
    /* *S_cpi = S_cp[i*NB_WORD_GFqn+iq] */
    /* *S_cpj = S_cp[j*NB_WORD_GFqn+iq] */

    /* Initialization of a copy of S */
    S_cp=MALLOC_MATRIXn;
    VERIFY_ALLOC_RET(S_cp);
    for(iq=0;iq<MATRIXn_SIZE;++iq)
    {
        S_cp[iq]=S[iq];
    }

    /* Remark : if HFEnr=0, we could avoid the last line in Gauss elimination */

    S_cpi=S_cp;
    /* for each row of S excepted the last block */
    for(iq=0;iq<HFEnq;++iq)
    {
        LOOPIR(NB_BITS_UINT,LOOPK(PERMUTATION64(S_cpj[k],S_cpi[k])),LOOPK(S_cpj[k]^=S_cpi[k]&pivot));
        /* Next block of column */
        ++S_cpi;
    }

    #if (!HFEnr)
        free(S_cp);
        /* Every coefficients of diagonal of S_cp are 1 */
        return 1U;
    #endif

    /* for each row of the last block excepted the last */
    /* iq = HFEnq */
    LOOPIR(HFEnr-1,PERMUTATION64(*S_cpj,*S_cpi),*S_cpj^=*S_cpi&pivot);

    /* We know there are 1 on diagonal excepted for the last line */
    bit_ir&=*S_cpi;
    free(S_cp);
    return bit_ir?1U:0U;
}



