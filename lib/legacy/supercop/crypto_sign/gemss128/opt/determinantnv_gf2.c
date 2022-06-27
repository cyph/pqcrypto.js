#include "determinantnv_gf2.h"
#include <stdlib.h>
#include "init.h"
#include "boolean.h"

/* To compute the determinant of a square matrix (n+v)*(n+v) */



/* Operation between two rows */
/* In Gauss elimination, we know that the i first columns are triangular */
/* So the computation is useless for ivq words for each row */
/* because we know there are only zeros */
#define LOOPK(op) \
    for(k=0;k<(NB_WORD_GF2nv-iq);++k)\
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
    for(j=i+1;j<HFEnv;++j)\
    {\
        /* next row */\
        S_cpj+=NB_WORD_GF2nv;\
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
            if(j==HFEnv)\
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
        S_cpi+=NB_WORD_GF2nv;\
        bit_ir<<=1;\
    }


/*  Input:
        S a matrix (n+v)*(n+v) in GF(2)

    Output:
        return ERROR_ALLOC if the allocation fails, the determinant of S else
        the determinant of S, computed with a Gauss elimination
*/
int PREFIX_NAME(determinantnv_nocst_gf2)(cst_Mnv_gf2 S)
{
    Mnv_gf2 S_cp,S_cpi,S_cpj;
    UINT bit_ir,pivot;
    unsigned int i=0,iq,ir,j,k;

    /* In this code, we have: */
    /* *S_cpi = S_cp[i*NB_WORD_GF2nv+iq] */
    /* *S_cpj = S_cp[j*NB_WORD_GF2nv+iq] */

    /* Initialization of a copy of S */
    S_cp=MALLOC_MATRIXnv;
    VERIFY_ALLOC_RET(S_cp);
    for(iq=0;iq<MATRIXnv_SIZE;++iq)
    {
        S_cp[iq]=S[iq];
    }

    /* Remark: if HFEnvr=0, we could avoid the last line in Gauss elimination */

    S_cpi=S_cp;
    /* for each row of S excepted the last block */
    for(iq=0;iq<HFEnvq;++iq)
    {
        LOOPIR(NB_BITS_UINT,LOOPK(PERMUTATION64(S_cpj[k],S_cpi[k])),LOOPK(S_cpj[k]^=S_cpi[k]&pivot));
        /* Next block of column */
        ++S_cpi;
    }

    #if (!HFEnvr)
        free(S_cp);
        /* Every coefficients of diagonal of S_cp are 1 */
        return 1U;
    #endif

    /* for each row of the last block excepted the last */
    /* iq = HFEnvq */
    LOOPIR(HFEnvr-1,PERMUTATION64(*S_cpj,*S_cpi),*S_cpj^=*S_cpi&pivot);

    /* We know there are 1 on diagonal excepted for the last line */
    bit_ir&=*S_cpi;
    free(S_cp);
    return bit_ir?1U:0U;
}




/* for each row j > i, operation on the columns */
#define LOOPJ_CST(OP) \
    S_cpj=S_cpi;\
    /* for the next rows */\
    for(j=i+1;j<HFEnv;++j)\
    {\
        /* next row */\
        S_cpj+=NB_WORD_GF2nv;\
        OP;\
    }

#define LOOPIR_CST(NB_IT) \
    for(ir=0;ir<NB_IT;++ir,++i)\
    {\
        /* row i += (1-pivot_i)* row j */\
        LOOPJ_CST({mask=(-(1UL-(((*S_cpi)>>ir)&1UL)));LOOPK(ADDLOADMASK1_64(S_cpi+k,S_cpj+k,mask);)});\
\
        /* Here, the pivot is 1 if S is invertible */\
        det_i&=(*S_cpi)>>ir;\
        /* If the pivot is 0, we could return 0 directly. */\
        /* In this case, the algorithm would be constant-time only for the 
           invertible matrices. */\
        /* However, for a random matrix in GF(2), the probability to do not 
           find a pivot 1 is:
           - 0.29 at the (n+v)-th column
           - 0.19 at the (n+v-1)-th column
           - 0.11 at the (n+v-2)-th column
           - 0.059 at the (n+v-3)-th column

           The probability that a random matrix is not invertible is 0.711.
           On the 4 last columns, we have a probability 0.650 to find a pivot 0.
           So, we have a big probability to find the pivot 0 only with the last 
           columns, and so it is useless to return 0 before the end of the 
           algorithm. */\
\
        /* row j += (pivot_j) * row_i */\
        LOOPJ_CST({mask=(-(((*S_cpj)>>ir)&1UL));LOOPK(ADDLOADMASK1_64(S_cpj+k,S_cpi+k,mask);)});\
\
        /* Next row */\
        S_cpi+=NB_WORD_GF2nv;\
    }




/*  Input:
        S a matrix (n+v)*(n+v) in GF(2)

    Output:
        return ERROR_ALLOC if the allocation fails, the determinant of S else
        the determinant of S, computed with a constant-time Gauss elimination
*/
int PREFIX_NAME(determinantnv_gf2)(cst_Mnv_gf2 S)
{
    Mnv_gf2 S_cp,S_cpi,S_cpj;
    UINT det_i,mask;
    unsigned int i=0,iq,ir,j,k;

    det_i=1UL;

    /* In this code, we have: */
    /* *S_cpi = S_cp[i*NB_WORD_GF2nv+iq] */
    /* *S_cpj = S_cp[j*NB_WORD_GF2nv+iq] */

    /* Initialization of a copy of S */
    S_cp=MALLOC_MATRIXnv;
    VERIFY_ALLOC_RET(S_cp);
    for(iq=0;iq<MATRIXnv_SIZE;++iq)
    {
        S_cp[iq]=S[iq];
    }

    /* Remark: if HFEnvr=0, we could avoid the last line in the Gauss 
       elimination */

    S_cpi=S_cp;
    /* for each row of S excepted the last block */
    for(iq=0;iq<HFEnvq;++iq)
    {
        LOOPIR_CST(NB_BITS_UINT);
        /* Next block of column */
        ++S_cpi;
    }

    #if (HFEnvr)
        /* for each row of the last block excepted the last */
        /* iq = HFEnvq */
        LOOPIR_CST(HFEnvr-1);

        /* For the last row */
        det_i&=(*S_cpi)>>ir;
    #endif

    free(S_cp);
    return det_i;
}


