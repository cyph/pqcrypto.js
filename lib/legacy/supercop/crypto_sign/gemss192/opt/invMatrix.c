#include "invMatrix.h"
#include "add.h"
#include "bit.h"
#include "init.h"
#include "initMatrixIdn.h"

/* Compute the inverse of a matrix */



/* Operation between two rows */
/* In Gauss elimination, we know that the i first columns are triangular */
/* So the computation is useless for iq words for each row */
/* because we know there are only zeros */
#define LOOPK(op) \
    for(k=0;k<(NB_WORD_GFqn-iq);++k)\
    {\
        op;\
    }\


#define PERMUT(LOOPK,LOOPKINV) \
        /* pivot == 1 ? */\
        if((*S_cpj)&bit_ir)\
        {\
            LOOPK;\
            LOOPKINV;\
            break;\
        }

#define ADDROW(LOOPK,LOOPKINV) \
        /* pivot */\
        pivot=-(((*S_cpj)>>ir)&1);\
        LOOPK;\
        LOOPKINV;


/* operation LOOPK and LOOPKINV if the pivot of jth row is 1 */
#define LOOPJ(OP) \
    S_cpj=S_cpi;\
    Sinv_cpj=Sinv_cpi;\
    /* for the next rows */\
    for(j=i+1;j<HFEn;++j)\
    {\
        /* next row */\
        S_cpj+=NB_WORD_GFqn;\
        Sinv_cpj+=NB_WORD_GFqn;\
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
            LOOPJ(PERMUT(LOOPK1,HFEPERMUTATION(Sinv_cpj,Sinv_cpi)));\
        }\
\
        /* add row i to row j if the pivot of jth row is 1*/\
        LOOPJ(ADDROW(LOOPK2,HFEDOTPRODUCTN(Sinv_cpj,Sinv_cpi,pivot)));\
\
        /* Next row */\
        S_cpi+=NB_WORD_GFqn;\
        Sinv_cpi+=NB_WORD_GFqn;\
        bit_ir<<=1;\
    }



/* Only modify S_inv */
#define LOOPIR_DOWN_TO_UP(NB_IT) \
    for(ir=NB_IT-1;ir!=(unsigned int)(-1);--ir,--i)\
    {\
        S_cpj=S_cpi;\
        Sinv_cpj=Sinv_cpi;\
        /* for the previous rows (i-1 to 0) */\
        for(j=i;j>0;--j)\
        {\
            /* Previous row */\
            S_cpj-=NB_WORD_GFqn;\
            Sinv_cpj-=NB_WORD_GFqn;\
            /* pivot */\
            pivot=-(((*S_cpj)>>ir)&1);\
            HFEDOTPRODUCTN(Sinv_cpj,Sinv_cpi,pivot);\
        }\
\
        /* Previous row */\
        S_cpi-=NB_WORD_GFqn;\
        Sinv_cpi-=NB_WORD_GFqn;\
    }



/* Input:
    S a matrix n*n in GF(2)

    Output:
    S_inv a matrix n*n in GF(2), inverse of S
    S is modified: transformed in upper triangular

    Requirement: S is invertible

    Method: transform S to Identity and Identity to S^(-1) (Gauss-Jordan)
*/
void invMatrix(GLn_gf2 S_inv, GLn_gf2 S)
{
    GLn_gf2 S_cpi,S_cpj,Sinv_cpi,Sinv_cpj;
    UINT bit_ir,pivot;
    unsigned int i=0,iq,ir,j,k;

    /* S_inv initialised to identity */
    initMatrixIdn(S_inv);

    /* In this code, we have : */
    /* *S_cpi = S[i*NB_WORD_GFqn+iq] */
    /* *S_cpj = S[j*NB_WORD_GFqn+iq] */
    /* *Sinv_cpi = Sinv[i*NB_WORD_GFqn] */
    /* *Sinv_cpj = Sinv[j*NB_WORD_GFqn] */

    S_cpi=S;
    Sinv_cpi=S_inv;
    /* for each row of S and of S_inv, excepted the last block */
    for(iq=0;iq<HFEnq;++iq)
    {
        LOOPIR(NB_BITS_UINT,LOOPK(PERMUTATION64(S_cpj[k],S_cpi[k])),LOOPK(S_cpj[k]^=S_cpi[k]&pivot));
        /* Next block of column */
        ++S_cpi;
    }

    #if (HFEnr)
        /* for each row of the last block excepted the last */
        /* iq = HFEnq */
        LOOPIR(HFEnr-1,PERMUTATION64(*S_cpj,*S_cpi),*S_cpj^=*S_cpi&pivot);

        /* Step 2 */
        bit_ir=1UL<<(HFEnr-1);
        LOOPIR_DOWN_TO_UP(HFEnr);
    #else
        /* To begin to last row */
        --i;
        S_cpi-=NB_WORD_GFqn;
        Sinv_cpi-=NB_WORD_GFqn;
    #endif


    /* Step 2 :
        S is upper triangular
        It uses operations which would transform S in Identity
        to transform S_inv in inverse of S
    */        


    /* A little modification (j becomes j-1) : */
    /* *S_cpj = S[(j-1)*NB_WORD_GFqn+iq] */
    /* *Sinv_cpj = Sinv[(j-1)*NB_WORD_GFqn] */

    /* for each row excepted the last block */
    for(iq=HFEnq;iq>0;--iq)
    {
        /* Previous block of column */
        --S_cpi;
        LOOPIR_DOWN_TO_UP(NB_BITS_UINT);
    }
}
