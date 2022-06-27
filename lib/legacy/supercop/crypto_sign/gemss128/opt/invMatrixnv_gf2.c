#include "invMatrixnv_gf2.h"
#include "tools_gf2nv.h"
#include "initMatrixId_gf2.h"

/* Compute the inverse of a matrix */



/* Operation between two rows */
/* In Gauss elimination, we know that the i first columns are triangular */
/* So the computation is useless for iq words for each row */
/* because we know there are only zeros */
#define LOOPK(op) \
    for(k=0;k<(NB_WORD_GF2nv-iq);++k)\
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
        mask=-(((*S_cpj)>>ir)&1);\
        LOOPK;\
        LOOPKINV;


/* operation LOOPK and LOOPKINV if the pivot of jth row is 1 */
#define LOOPJ(OP) \
    S_cpj=S_cpi;\
    Sinv_cpj=Sinv_cpi;\
    /* for the next rows */\
    for(j=i+1;j<HFEnv;++j)\
    {\
        /* next row */\
        S_cpj+=NB_WORD_GF2nv;\
        Sinv_cpj+=NB_WORD_GF2nv;\
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
            LOOPJ(PERMUT(LOOPK1,swap_gf2nv(Sinv_cpj,Sinv_cpi)));\
        }\
\
        /* add row i to row j if the pivot of jth row is 1*/\
        LOOPJ(ADDROW(LOOPK2,addLoadMask1_gf2nv(Sinv_cpj,Sinv_cpi,mask)));\
\
        /* Next row */\
        S_cpi+=NB_WORD_GF2nv;\
        Sinv_cpi+=NB_WORD_GF2nv;\
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
            S_cpj-=NB_WORD_GF2nv;\
            Sinv_cpj-=NB_WORD_GF2nv;\
            /* pivot */\
            mask=-(((*S_cpj)>>ir)&1);\
            addLoadMask1_gf2nv(Sinv_cpj,Sinv_cpi,mask);\
        }\
\
        /* Previous row */\
        S_cpi-=NB_WORD_GF2nv;\
        Sinv_cpi-=NB_WORD_GF2nv;\
    }



/* Input:
    S a matrix (n+v)*(n+v) in GF(2)

    Output:
    S_inv a matrix (n+v)*(n+v) in GF(2), inverse of S
    S is modified: transformed in upper triangular

    Requirement: S is invertible

    Method: transform S to Identity and Identity to S^(-1) (Gauss-Jordan)
*/
void PREFIX_NAME(invMatrixnv_nocst_gf2)(GLnv_gf2 S_inv, GLnv_gf2 S)
{
    GLnv_gf2 S_cpi,S_cpj,Sinv_cpi,Sinv_cpj;
    UINT bit_ir,mask;
    unsigned int i=0,iq,ir,j,k;

    /* S_inv initialised to identity */
    initMatrixIdnv_gf2(S_inv);

    /* In this code, we have: */
    /* *S_cpi = S[i*NB_WORD_GF2nv+iq] */
    /* *S_cpj = S[j*NB_WORD_GF2nv+iq] */
    /* *Sinv_cpi = Sinv[i*NB_WORD_GF2nv] */
    /* *Sinv_cpj = Sinv[j*NB_WORD_GF2nv] */

    S_cpi=S;
    Sinv_cpi=S_inv;
    /* for each row of S and of S_inv, excepted the last block */
    for(iq=0;iq<HFEnvq;++iq)
    {
        LOOPIR(NB_BITS_UINT,LOOPK(PERMUTATION64(S_cpj[k],S_cpi[k])),LOOPK(S_cpj[k]^=S_cpi[k]&mask));
        /* Next block of column */
        ++S_cpi;
    }

    #if (HFEnvr)
        /* for each row of the last block excepted the last */
        /* iq = HFEnvq */
        LOOPIR(HFEnvr-1,PERMUTATION64(*S_cpj,*S_cpi),*S_cpj^=*S_cpi&mask);

        /* Step 2 */
        bit_ir=1UL<<(HFEnvr-1);
        LOOPIR_DOWN_TO_UP(HFEnvr);
    #else
        /* To begin to last row */
        --i;
        S_cpi-=NB_WORD_GF2nv;
        Sinv_cpi-=NB_WORD_GF2nv;
    #endif


    /* Step 2:
        S is upper triangular
        It uses operations which would transform S in Identity
        to transform S_inv in inverse of S
    */        


    /* A little modification (j becomes j-1): */
    /* *S_cpj = S[(j-1)*NB_WORD_GF2nv+iq] */
    /* *Sinv_cpj = Sinv[(j-1)*NB_WORD_GF2nv] */

    /* for each row excepted the last block */
    for(iq=HFEnvq;iq>0;--iq)
    {
        /* Previous block of column */
        --S_cpi;
        LOOPIR_DOWN_TO_UP(NB_BITS_UINT);
    }
}




/* for each row j > i, operation on the columns */
#define LOOPJ_CST(OP) \
    S_cpj=S_cpi;\
    Sinv_cpj=Sinv_cpi;\
    /* for the next rows */\
    for(j=i+1;j<HFEnv;++j)\
    {\
        /* next row */\
        S_cpj+=NB_WORD_GF2nv;\
        Sinv_cpj+=NB_WORD_GF2nv;\
        OP;\
    }

#define LOOPIR_CST(NB_IT) \
    for(ir=0;ir<NB_IT;++ir,++i)\
    {\
        /* row i += (1-pivot_i)* row j */\
        LOOPJ_CST({mask=(-(1UL-(((*S_cpi)>>ir)&1UL)));\
                   LOOPK(ADDLOADMASK1_64(S_cpi+k,S_cpj+k,mask);)\
                   addLoadMask1_gf2nv(Sinv_cpi,Sinv_cpj,mask);\
                  });\
\
        /* row j += (pivot_j) * row_i */\
        LOOPJ_CST({mask=(-(((*S_cpj)>>ir)&1UL));\
                   LOOPK(ADDLOADMASK1_64(S_cpj+k,S_cpi+k,mask);)\
                   addLoadMask1_gf2nv(Sinv_cpj,Sinv_cpi,mask);\
                  });\
\
        /* Next row */\
        S_cpi+=NB_WORD_GF2nv;\
        Sinv_cpi+=NB_WORD_GF2nv;\
    }



/* Input:
    S a matrix (n+v)*(n+v) in GF(2)

    Output:
    S_inv a matrix (n+v)*(n+v) in GF(2), inverse of S
    S is modified: transformed in upper triangular

    Requirement: S is invertible

    Method: transform S to Identity and Identity to S^(-1) (Gauss-Jordan)
            A constant-time Gauss elimination is used.
*/
void PREFIX_NAME(invMatrixnv_gf2)(GLnv_gf2 S_inv, GLnv_gf2 S)
{
    GLnv_gf2 S_cpi,S_cpj,Sinv_cpi,Sinv_cpj;
    UINT mask;
    unsigned int i=0,iq,ir,j,k;

    /* S_inv initialised to identity */
    initMatrixIdnv_gf2(S_inv);

    /* In this code, we have: */
    /* *S_cpi = S[i*NB_WORD_GF2nv+iq] */
    /* *S_cpj = S[j*NB_WORD_GF2nv+iq] */
    /* *Sinv_cpi = Sinv[i*NB_WORD_GF2nv] */
    /* *Sinv_cpj = Sinv[j*NB_WORD_GF2nv] */

    S_cpi=S;
    Sinv_cpi=S_inv;
    /* for each row of S and of S_inv, excepted the last block */
    for(iq=0;iq<HFEnvq;++iq)
    {
        LOOPIR_CST(NB_BITS_UINT);
        /* Next block of column */
        ++S_cpi;
    }

    #if (HFEnvr)
        LOOPIR_CST(HFEnvr-1);

        /* Step 2 */
        LOOPIR_DOWN_TO_UP(HFEnvr);
    #else
        /* To begin to last row */
        --i;
        S_cpi-=NB_WORD_GF2nv;
        Sinv_cpi-=NB_WORD_GF2nv;
    #endif


    /* Step 2:
        S is upper triangular
        It uses operations which would transform S in Identity
        to transform S_inv in inverse of S
    */        


    /* A little modification (j becomes j-1): */
    /* *S_cpj = S[(j-1)*NB_WORD_GF2nv+iq] */
    /* *Sinv_cpj = Sinv[(j-1)*NB_WORD_GF2nv] */

    /* for each row excepted the last block */
    for(iq=HFEnvq;iq>0;--iq)
    {
        /* Previous block of column */
        --S_cpi;
        LOOPIR_DOWN_TO_UP(NB_BITS_UINT);
    }
}



/* Input:
    L and U two matrices (n+v)*(n+v) in GF(2), such that S = LU
    The transpose of U is stored.

    Output:
    S_inv a matrix (n+v)*(n+v) in GF(2), inverse of S

    Method: transform S to Identity and Identity to S^(-1) (Gauss-Jordan)
            Here, we do not need to transform S to Identity.
            We use L to transform Identity to a lower triangular S', then
            we use U to transform S' to S^(-1).
*/
void PREFIX_NAME(invMatrixLUnv_gf2)(GLnv_gf2 S_inv, cst_Tnv_gf2 L, cst_Tnv_gf2 U)
{
    GLnv_gf2 Sinv_cpi,Sinv_cpj;
    cst_Tnv_gf2 L_cpj;
    UINT mask;
    unsigned int i,iq,ir,j,k;

    /* Initialize to 0 */
    for(i=0;i<MATRIXnv_SIZE;++i)
    {
        S_inv[i]=0UL;
    }


    Sinv_cpi=S_inv;
    /* for each row of S and of S_inv, excepted the last block */
    for(i=0,iq=0;iq<HFEnvq;++iq)
    {
        for(ir=0;ir<NB_BITS_UINT;++ir,++i)
        {
            /* The element of the diagonal is 1 */
            *(Sinv_cpi+iq)^=(1UL<<ir);

            Sinv_cpj=Sinv_cpi;
            L_cpj=L;
            /* for the next rows */
            for(j=i;j<(HFEnv-1);++j)
            {
                /* next row */
                Sinv_cpj+=NB_WORD_GF2nv;
                L_cpj+=(j>>6)+1;

                mask=(-(((*L_cpj)>>ir)&1UL));
                for(k=0;k<=iq;++k)
                {
                    ADDLOADMASK1_64(Sinv_cpj+k,Sinv_cpi+k,mask);
                }
            }

            /* Next row */
            Sinv_cpi+=NB_WORD_GF2nv;
            L+=iq+1;
        }
        /* Next column */
        ++L;
    }
    #if (HFEnvr>1)
        for(ir=0;ir<(HFEnvr-1);++ir,++i)
        {
            /* The element of the diagonal is 1 */
            *(Sinv_cpi+iq)^=(1UL<<ir);

            Sinv_cpj=Sinv_cpi;
            L_cpj=L;
            /* for the next rows */
            for(j=i;j<(HFEnv-1);++j)
            {
                /* next row */
                Sinv_cpj+=NB_WORD_GF2nv;
                L_cpj+=(j>>6)+1;

                mask=(-(((*L_cpj)>>ir)&1UL));
                for(k=0;k<=iq;++k)
                {
                    ADDLOADMASK1_64(Sinv_cpj+k,Sinv_cpi+k,mask);
                }
            }

            /* Next row */
            Sinv_cpi+=NB_WORD_GF2nv;
            L+=iq+1;
        }
        /* ir = HFEnvr-1 */
        *(Sinv_cpi+iq)^=(1UL<<ir);
        Sinv_cpi+=NB_WORD_GF2nv;
    #endif


    /* Here, Sinv_cpi is at the end of Sinv */

    /* End of U */
    U+=LTRIANGULAR_NV_SIZE;

    /* for each row excepted the first */
    for(i=HFEnv-1;i>0;--i)
    {
        /* Previous row */
        U-=1+(i>>6);

        /* Row i of Sinv */
        Sinv_cpi-=NB_WORD_GF2nv;

        /* Row j of Sinv */
        Sinv_cpj=S_inv;

        /* for the previous rows */
        for(j=0;j<i;++j)
        {
            /* pivot */
            mask=-(((U[j>>6])>>(j&63U))&1U);
            addLoadMask1_gf2nv(Sinv_cpj,Sinv_cpi,mask);

            /* next row */
            Sinv_cpj+=NB_WORD_GF2nv;
        }
    }
}




