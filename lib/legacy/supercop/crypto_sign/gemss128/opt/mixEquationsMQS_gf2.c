#include "mixEquationsMQS_gf2.h"
#include "vecMatProduct_gf2.h"
#include "dotProduct_gf2.h"


/* To mix equations in a MQ System */



/* for a block of NB_BITS_UINT rows */
#define LOOPJR(NB_IT) \
    /* jr = 0 : initialisation of jqth word of ith monom of output */\
    dotProduct_gf2_n(tmp,T_cp,MQS);\
    T_cp+=NB_WORD_GFqn;\
    *pk=tmp;\
    for(jr=1;jr<NB_IT;++jr)\
    {\
        /* dot product (jq*NB_BITS_UINT)+jr row of T by the ith monom */\
        dotProduct_gf2_n(tmp,T_cp,MQS);\
        /* next row of T */\
        T_cp+=NB_WORD_GFqn;\
        /* xor the computed bit */\
        *pk^=tmp<<jr;\
    }\
    /* next word of monom */\
    ++pk;


/* Input:
    MQS a MQ system with n equations in GF(2)[x1,...,x_(n+v)]
    T a matrix n*n in GF(2), but used as matrix m*n
    so the (n-m) last rows are not used

  Output:
    pk a MQ system with m equations in GF(2)[x1,...,x_(n+v)]
    pk = T*MQS
*/
void PREFIX_NAME(mixEquationsMQS_gf2_left)(mqsnv_gf2m pk, cst_mqsnv_gf2n MQS, cst_Mn_gf2 T)
{
    UINT tmp;
    cst_Mn_gf2 T_cp;
    unsigned int i,jq,jr;

    /* for each monom of MQS and of pk */
    for(i=0;i<NB_MONOM_PK;++i)
    {
        T_cp=T;
        /* for each row of T excepted the last block */
        for(jq=0;jq<HFEmq;++jq)
        {
            LOOPJR(NB_BITS_UINT);
        }
        /* the last block */
        #if (HFEmr)
            LOOPJR(HFEmr);
        #endif

        /* next monom */
        MQS+=NB_WORD_GFqn;
    }
}



void PREFIX_NAME(mixEquationsMQS_gf2_right)(mqsnv_gf2m pk, cst_mqsnv_gf2n MQS, cst_Mn_gf2 T)
{
    unsigned int i;

    /* for each monomial of MQS and pk */
    for(i=0;i<NB_MONOM_PK;++i)
    {
        vecMatProductm_gf2(pk,MQS,T);

        /* next monomial */
        MQS+=NB_WORD_GFqn;
        pk+=NB_WORD_GF2m;
    }
}



