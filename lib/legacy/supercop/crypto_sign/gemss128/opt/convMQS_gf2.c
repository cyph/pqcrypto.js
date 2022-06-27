#include "convMQS_gf2.h"
#include "tools_gf2m.h"
#include "tools_gf2nv.h"
#include "MQS_gf2.h"




#define LOOPJR(NB_IT) \
    for(jr=1;jr<NB_IT;++jr)\
    {\
        *pk2_cp^=(((*pk_cp)>>ir)&1UL)<<jr;\
        pk_cp+=NB_WORD_GF2m;\
    }\
    ++pk2_cp;

#if HFENr
    #define LOOPJQ \
        /* To have equivalence between *pk_cp and pk_cp[iq] */\
        pk_cp=pk+iq;\
        for(jq=0;jq<HFENq;++jq)\
        {\
            /* jr=0 */ \
            *pk2_cp=((*pk_cp)>>ir)&1UL;\
            pk_cp+=NB_WORD_GF2m;\
            LOOPJR(NB_BITS_UINT);\
        }\
\
        /* jr=0 */ \
        *pk2_cp=((*pk_cp)>>ir)&1UL;\
        pk_cp+=NB_WORD_GF2m;\
        LOOPJR(HFENr);
#else
    #define LOOPJQ \
        /* To have equivalence between *pk_cp and pk_cp[iq] */\
        pk_cp=pk+iq;\
        for(jq=0;jq<HFENq;++jq)\
        {\
            /* jr=0 */ \
            *pk2_cp=((*pk_cp)>>ir)&1UL;\
            pk_cp+=NB_WORD_GF2m;\
            LOOPJR(NB_BITS_UINT);\
        }
#endif



/*  Input: pk, one equation in GF(2^m)[x1,...,x_(n+v)]

    Output: pk2, m equations in GF(2)[x1,...,x_(n+v)]
            each equation of pk2 is compressed
*/
/* This function changes the representation of the public key */
void PREFIX_NAME(convMQS_one_to_m_compressed_equations_gf2)(UINT* pk2, \
                                                            const UINT* pk)
{
    const UINT* pk_cp;
    UINT* pk2_cp=pk2;
    unsigned int iq,ir,jq,jr;

    /* For each equation of result */
    for(iq=0;iq<HFEmq;++iq)
    {
        for(ir=0;ir<NB_BITS_UINT;++ir)
        {
            /* Loop on every monomials */
            LOOPJQ;
        }
    }

    #if HFEmr
        for(ir=0;ir<HFEmr;++ir)
        {
            /* Loop on every monomials */
            LOOPJQ;
        }
    #endif
}



#if HFEmr
    #define REM_K \
        for(kr=0;kr<HFEmr;++kr,++k)\
        {\
            if(ir)\
            {\
                pk2_cp2[k*NB_WORD_ONE_EQUATION]^=(((*pk)>>kr)&1UL)<<ir;\
            } else\
            {\
                pk2_cp2[k*NB_WORD_ONE_EQUATION]=((*pk)>>kr)&1UL;\
            }\
        }\
        ++pk;
#else
    #define REM_K
#endif


#define LOOPK2 \
    k=0;\
    for(kq=0;kq<HFEmq;++kq)\
    {\
        for(kr=0;kr<NB_BITS_UINT;++kr,++k)\
        {\
            if(ir)\
            {\
                pk2_cp2[k*NB_WORD_ONE_EQUATION]^=(((*pk)>>kr)&1UL)<<ir;\
            } else\
            {\
                pk2_cp2[k*NB_WORD_ONE_EQUATION]=((*pk)>>kr)&1UL;\
            }\
        }\
        ++pk;\
    }\
    REM_K;



/* Input: pk = (c,Q), with c the constant part in GF(2^m)
          Q is upper triangular matrix of size (n+v)*(n+v) in GF(2^m)

   Output: pk2 = (c,Q0,Q1,...,Q_(m-1))
           the Qi are lower triangular of size (n+v)*(n+v) in GF(2)
           for each Qi, the rows are stored separately
           (we take new words for each new row)
*/
/* This function changes the representation of the public key */
/* The output format is used to improve the evaluation */
void PREFIX_NAME(convMQS_one_to_m_equations_gf2)(UINT* pk2, const UINT* pk)
{
    UINT *pk2_cp,*pk2_cp2;
    unsigned int iq,ir,jq,jr,kq,kr,k;

    /* Copy the constant */
    copy_gf2m(pk2,pk);
    pk+=NB_WORD_GF2m;
    pk2+=NB_WORD_GF2m;
    pk2_cp=pk2-1;

    /* Loop on the columns of Q of pk2 */
    for(iq=0;iq<HFEnvq;++iq)
    {
        for(ir=0;ir<NB_BITS_UINT;++ir)
        {
            pk2_cp+=iq+1;
            pk2_cp2=pk2_cp;
            for(jr=ir;jr<NB_BITS_UINT;++jr)
            {
                LOOPK2;
                pk2_cp2+=iq+1;
            }

            for(jq=iq+1;jq<HFEnvq;++jq)
            {
                for(jr=0;jr<NB_BITS_UINT;++jr)
                {
                    LOOPK2;
                    pk2_cp2+=jq+1;
                }
            }

            #if HFEnvr
                for(jr=0;jr<HFEnvr;++jr)
                {
                    LOOPK2;
                    pk2_cp2+=jq+1;
                }
            #endif
        }
    }

    #if HFEnvr
        for(ir=0;ir<HFEnvr;++ir)
        {
            pk2_cp+=iq+1;
            pk2_cp2=pk2_cp;
            for(jr=ir;jr<HFEnvr;++jr)
            {
                LOOPK2;
                pk2_cp2+=HFEnvq+1;
            }
        }
    #endif
}




#define LOOPK2_REM \
    pk+=NB_WORD_GF2m;\
    for(kr=0;kr<HFEmr;++kr)\
    {\
        if(ir)\
        {\
            pk2_cp2[kr*NB_WORD_ONE_EQUATION]^=(((*pk)>>kr)&1UL)<<ir;\
        } else\
        {\
            pk2_cp2[kr*NB_WORD_ONE_EQUATION]=((*pk)>>kr)&1UL;\
        }\
    }\



#if HFEmr
/* Input: pk = (c,Q), with c the constant part in GF(2^m)
          Q is upper triangular of size (n+v)*(n+v) in GF(2^m)

   Output: pk2 = (Q_(HFEm-HFEmr),...,Q_(m-1)), there are HFEmr equations
           the Qi are lower triangular matrix of size (n+v)*(n+v) in GF(2)
           for each Qi, the rows are stored separately
           (we take new words for each new row)
*/
static void convMQS_one_to_last_mr_equations_gf2(UINT* pk2, const UINT* pk)
{
    UINT *pk2_cp,*pk2_cp2;
    unsigned int iq,ir,jq,jr,kr;

    pk+=NB_WORD_GF2m-1;
    pk2_cp=pk2-1;

    /* Loop on the columns of Q of pk2 */
    for(iq=0;iq<HFEnvq;++iq)
    {
        for(ir=0;ir<NB_BITS_UINT;++ir)
        {
            pk2_cp+=iq+1;
            pk2_cp2=pk2_cp;
            for(jr=ir;jr<NB_BITS_UINT;++jr)
            {
                LOOPK2_REM;
                pk2_cp2+=iq+1;
            }

            for(jq=iq+1;jq<HFEnvq;++jq)
            {
                for(jr=0;jr<NB_BITS_UINT;++jr)
                {
                    LOOPK2_REM;
                    pk2_cp2+=jq+1;
                }
            }

            #if HFEnvr
                for(jr=0;jr<HFEnvr;++jr)
                {
                    LOOPK2_REM;
                    pk2_cp2+=jq+1;
                }
            #endif
        }
    }

    #if HFEnvr
        for(ir=0;ir<HFEnvr;++ir)
        {
            pk2_cp+=iq+1;
            pk2_cp2=pk2_cp;
            for(jr=ir;jr<HFEnvr;++jr)
            {
                LOOPK2_REM;
                pk2_cp2+=HFEnvq+1;
            }
        }
    #endif
}
#endif




/* Input: pk = (c,Q), with c the constant part in GF(2^m)
          Q is upper triangular matrix of size (n+v)*(n+v) in GF(2^m)

   Output: pk2 = (c,Q',Q_(HFEm-HFEmr),...,Q_(m-1))
           Q' is upper triangular of size (n+v)*(n+v) in GF(2^(m-mr))
           the HFEmr Qi are lower triangular of size (n+v)*(n+v) in GF(2)
           for each Qi, the rows are stored separately
           (we take new words for each new row)
*/
void PREFIX_NAME(convMQS_one_eq_to_hybrid_representation_gf2)(UINT* pk2, const UINT* pk)
{
    #if HFEmr
        convMQS_one_to_last_mr_equations_gf2(pk2+ACCESS_last_equations,pk);
    #endif

    /* Constant */
    copy_gf2m(pk2,pk);

    #if HFEmq
        unsigned int i,j;
        pk+=NB_WORD_GF2m;
        pk2+=NB_WORD_GF2m;
        for(i=1;i<NB_MONOM_PK;++i)
        {
            for(j=0;j<HFEmq;++j)
            {
                *pk2=*pk;
                ++pk2;
                ++pk;
            }
            /* Jump the coefficients of the HFEmr last equations */
            #if HFEmr
                ++pk;
            #endif
        }
    #endif
}






