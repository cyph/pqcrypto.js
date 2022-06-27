#include "changeVariablesMQS_gf2.h"
#include <stdlib.h>
#include "tools_gf2n.h"
#include "arith_gf2n.h"

/* To do a changement of variables in a MQ System */




/****************************************/
/*** Special fast version for 64 bits ***/
/****************************************/


/* Compute a dot product with one word of S */
#define LOOPKR(START,NB_IT) \
    for(kr=START;kr<NB_IT;++kr)\
    {\
        /* multiply one bit of S by one element of MQS_cpj */\
        mask=-(bit_kr&1UL); \
        addLoadMask1_gf2n(MQS2_cp,MQS_cpj,mask);\
        MQS_cpj+=NB_WORD_GFqn;\
        bit_kr>>=1;\
    }

/* Compute a dot product for the last word of S */
#if (HFEnvr)
    #define LOOPKR_REMAINDER \
        bit_kr=S_cpj[kq];\
        LOOPKR(0,HFEnvr);
#else
    #define LOOPKR_REMAINDER
#endif

/* Compute a dot product for the last word of S */
#define LOOPK_REM \
    bit_kr=(*S_cpj)>>ir;\
    LOOPKR(ir,HFEnvr);


/* Compute a dot product with iq words equal to zero at the begin of the rows */
#define LOOPK \
    bit_kr=(*S_cpj)>>ir;\
    LOOPKR(ir,NB_BITS_UINT);\
\
    for(kq=1;kq<(HFEnvq-iq);++kq)\
    {\
        bit_kr=S_cpj[kq];\
        LOOPKR(0,NB_BITS_UINT);\
    }\
\
    LOOPKR_REMAINDER;


/* Compute a dot product with complete rows */
#define LOOPK_COMPLETE \
    for(kq=0;kq<HFEnvq;++kq)\
    {\
        bit_kr=S_cpj[kq];\
        LOOPKR(0,NB_BITS_UINT);\
    }\
    LOOPKR_REMAINDER;


/* Loop for a block of rows */
#define LOOPIR(STARTIR,NB_ITIR,LOOPK) \
    for(ir=STARTIR;ir<NB_ITIR;++ir)\
    {\
        /* Compute a dot product */\
        LOOPK;\
        /* update the next element to compute */\
        MQS2_cp+=NB_WORD_GFqn;\
    }


/* Loop for a block of rows */
/* Init to 0 the res */
#define LOOPIR_INIT(STARTIR,NB_ITIR) \
    for(ir=STARTIR;ir<NB_ITIR;++ir)\
    {\
        set0_gf2n(MQS2_cp);\
        MQS_cpj=MQS_cpi;\
        /* Compute a dot product */\
        LOOPK_COMPLETE;\
        /* update the next element to compute */\
        MQS2_cp+=NB_WORD_GFqn;\
        /* update the next row of S to use */\
        S_cpj+=NB_WORD_GF2nv;\
    }


/* Input:
    S a matrix (n+v)*(n+v) in GF(2)
    MQS a MQ system with n equations in GF(2)[x1,...,x_(n+v)]

  Output:
    0 if the result is correct, ERROR_ALLOC for error from calloc function
    MQS a MQ system with n equations in GF(2)[x1,...,x_(n+v)]
    Transform the matrix of quadratic terms Q in S*Q*transpose(S)
*/
int PREFIX_NAME(changeVariablesMQS64_gf2)(mqsnv_gf2n MQS, cst_GLnv_gf2 S)
{
    mqsnv_gf2n MQS2, MQS2_cp;
    UINT bit_kr, mask;
    cst_mqsnv_gf2n MQS_cpi,MQS_cpj;
    cst_GLnv_gf2 S_cpi,S_cpj;
    unsigned int iq,ir,j,jq,jr,kq,kr;

    /* Tmp matrix (n+v)*(n+v) of quadratic terms to compute S*Q */
    MQS2=(UINT*)calloc(HFEnv*HFEnv*NB_WORD_GFqn,sizeof(UINT));
    VERIFY_ALLOC_RET(MQS2);

    /* To avoid the constant of MQS */
    MQS_cpi=MQS+NB_WORD_GFqn;

    MQS2_cp=MQS2;
    S_cpj=S;

    /* Step 1 : compute MQS2 = S*Q */
    /* Use multiplication by transpose (so by rows of Q) */
    /* It is possible because X*Q*tX = X*tQ*tX (with X = (x1 ... xn)) */
    /* Warning : Q is a upper triangular matrix in GF(q^n) */

    /* In this code, we have : */
    /* i = iq*NB_BITS_UINT + ir */
    /* k = kq*NB_BITS_UINT + kr */

    /* *MQS_cpi = MQS[NB_WORD_GFqn] */
    /* *MQS_cpj = MQS_cpi[(((i*(2n-i+1))/2) + k)*NB_WORD_GFqn] */
    /* The previous formula is a bit complicated, so the idea is :
       *MQS_cpj would equal MQS_cpi[i][i+k] if MQS used n*n in memory */
    /* *MQS2_cp = MQS2[i*NB_WORD_GFqn] */
    /* *S_cpj = S[j*NB_WORD_GFqn+iq] */

    /* for each row j of S */
    for(j=0;j<HFEnv;++j)
    {
        /* initialisation at the first row of Q */
        MQS_cpj=MQS_cpi;
        /* for each row of Q excepted the last block */
        for(iq=0;iq<HFEnvq;++iq)
        {
            LOOPIR(0,NB_BITS_UINT,LOOPK);
            /* 64 bits of zero in Q */
            ++S_cpj;
        }
        /* the last block */
        #if (HFEnvr)
            LOOPIR(0,HFEnvr,LOOPK_REM);
            /* Next row of S */
            ++S_cpj;
        #endif
    }

    /* Step 2 : compute MQS = MQS2*tS = (S*Q)*tS */
    /* Use multiplication by transpose (so by rows of S) */

    /* Permute MQS and MQS2 */
    MQS_cpi=MQS2;
    MQS2_cp=MQS+NB_WORD_GFqn;
    S_cpi=S;
 
    /* First : compute upper triangular result */


    /* In this code, we have : */
    /* *MQS_cpi = MQS2[j*n*NB_WORD_GFqn] */
    /* *MQS_cpj = MQS2[(j*n+k)*NB_WORD_GFqn] */
    /* *MQS2_cp = MQS[(((j*(2n-j+1))/2) + i-j)*NB_WORD_GFqn] */
    /* The previous formula is a bit complicated, so the idea is :
       *MQS2_cp would equal MQS[j][i] if MQS used n*n in memory */
    /* *S_cpi = S[j*NB_WORD_GFqn] */
    /* *S_cpj = S[i*NB_WORD_GFqn] */


    /* for each row j of MQS2 excepted the last block */
    for(jq=0;jq<HFEnvq;++jq)
    {
        for(jr=0;jr<NB_BITS_UINT;++jr)
        {
            S_cpj=S_cpi;
            /* for each row >=j of S */
            LOOPIR_INIT(jr,NB_BITS_UINT);
            for(iq=jq+1;iq<HFEnvq;++iq)
            {
                LOOPIR_INIT(0,NB_BITS_UINT);
            }
            /* the last block */
            #if (HFEnvr)
                LOOPIR_INIT(0,HFEnvr);
            #endif
            /* Next row of MQS2 */
            MQS_cpi=MQS_cpj;
            /* Next row of S because of upper triangular */
            S_cpi+=NB_WORD_GF2nv;
        }
    }
    /* the last block */
    #if (HFEnvr)
        for(jr=0;jr<HFEnvr;++jr)
        {
            S_cpj=S_cpi;
            MQS_cpj=MQS_cpi;
            /* for each row >=j of S, the last block */
            LOOPIR_INIT(jr,HFEnvr);
            MQS_cpi=MQS_cpj;
            S_cpi+=NB_WORD_GF2nv;
        }
    #endif

    /* Second : compute lower triangular result */

    MQS_cpi=MQS2;
    MQS2_cp=MQS+NB_WORD_GFqn;
    S_cpj=S;


    /* In this code, we have : */
    /* *MQS_cpi = MQS2[(j+1)*n*NB_WORD_GFqn] */
    /* *MQS_cpj = MQS2[(j+1)*n+k)*NB_WORD_GFqn] */
    /* *MQS2_cp = MQS[(((j*(2n-j+1))/2) + i-j)*NB_WORD_GFqn] */
    /* The previous formula is a bit complicated, so the idea is :
       *MQS2_cp would equal MQS[j][i] if MQS used n*n in memory */
    /* *S_cpj = S[j*NB_WORD_GFqn] */


    /* for each row j of S excepted the last block */
    for(jq=0;jq<HFEnvq;++jq)
    {
        for(jr=0;jr<NB_BITS_UINT;++jr)
        {
            /* i=j : the diagonal is already computing */
            MQS2_cp+=NB_WORD_GFqn;
            /* The line j of MQS2 is useless */
            MQS_cpi+=HFEnv*NB_WORD_GFqn;
            MQS_cpj=MQS_cpi;
            /* for each row >j of MQS2 */
            LOOPIR(jr+1,NB_BITS_UINT,LOOPK_COMPLETE);
            for(iq=jq+1;iq<HFEnvq;++iq)
            {
                LOOPIR(0,NB_BITS_UINT,LOOPK_COMPLETE);
            }
            /* the last block */
            #if (HFEnvr)
                LOOPIR(0,HFEnvr,LOOPK_COMPLETE);
            #endif
            /* Next row of S */
            S_cpj+=NB_WORD_GF2nv;
        }
    }
    /* the last block excepted the last row */
    #if (HFEnvr)
       for(jr=0;jr<HFEnvr-1;++jr)
       {
            /* i=j : the diagonal is already computing */
            MQS2_cp+=NB_WORD_GFqn;
            /* The line j of MQS2 is useless */
            MQS_cpi+=HFEnv*NB_WORD_GFqn;
            MQS_cpj=MQS_cpi;
            /* for each row >=j of S */
            /* the last block */
            LOOPIR(jr+1,HFEnvr,LOOPK_COMPLETE);
            /* Next row of S */
            S_cpj+=NB_WORD_GF2nv;
       }
    #endif


    free(MQS2);
    return 0;
}


