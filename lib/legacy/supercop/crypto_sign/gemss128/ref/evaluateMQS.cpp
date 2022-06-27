#include "evaluateMQS.hpp"
#include "convNTL.hpp"
#include "add.h"
#include "init.h"
#include "bit.h"

#include "config_gf2n.h"

#if HFEmr
    #include "evalMQnocst_gf2.h"
#endif

#include <NTL/GF2EX.h>
#include <NTL/GF2XFactoring.h>


/* Input:
    m a vector of n+v elements of GF(2)
    pk a MQ system with m equations in GF(2)[x1,...,x_(n+v)]

  Output:
    c a vector of m elements of GF(2), c is the evaluation of pk in m
*/
void evaluateMQS_pk(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk)
{
    GF2X f;
    /* Just to set the degree of extension to HFEm */
    /* The choice of f is not important here */
    BuildSparseIrred(f,HFEm);
    GF2E::init(f);
    f.kill();

    mat_GF2E MQS;
    vec_GF2E v_m,res_tmp;
    vec_GF2 c_vec;
    GF2E res;

    /* Begin after the constant */
    convUINT_pk_ToNTLmat_GF2E(MQS,pk+NB_WORD_GFqm);
    convUINTToNTLvecnv_GF2(c_vec,m);
    conv(v_m,c_vec);
    c_vec.kill();

    /* v_m*Q_pk */
    mul(res_tmp,v_m,MQS);
    MQS.kill();
    /* (v_m*Q_pk)*v_m */
    res=res_tmp*v_m;
    v_m.kill();
    res_tmp.kill();

    BytesFromGF2X((unsigned char*)c,conv<GF2X>(res),NB_BYTES_GFqm);
    clear(res);
    /* Clean the last word */
    #if HFEmr
        c[NB_WORD_GFqm-1]&=HFE_MASKm;
    #endif

    /* (v_m*Q_pk*v_m) + cst_pk */
    HFEADD2m(c,pk);
}


/* Input:
    m a vector of n+v elements of GF(2)
    pk a MQ system with m equations in GF(2)[x1,...,x_(n+v)]

  Output:
    c a vector of m elements of GF(2), c is the evaluation of pk in m
*/
void evaluateMQS_pk_hybrid(vecm_gf2 c, cst_vecnv_gf2 m, cst_mqsnv_gf2m pk)
{
    /* Evaluation of the HFEmq*64 first equations */
    #if HFEmq
        GF2X f;
        /* Just to set the degree of extension to (HFEm-HFEmr) */
        /* The choice of f is not important here */
        BuildSparseIrred(f,HFEm-HFEmr);
        GF2E::init(f);
        f.kill();

        mat_GF2E MQS;
        vec_GF2E v_m,res_tmp;
        vec_GF2 c_vec;
        GF2E res;

        /* Begin after the constant */
        convUINT_pk_hybrid_ToNTLmat_GF2E(MQS,pk+NB_WORD_GFqm);
        convUINTToNTLvecnv_GF2(c_vec,m);
        conv(v_m,c_vec);
        c_vec.kill();

        /* v_m*Q_pk */
        mul(res_tmp,v_m,MQS);
        MQS.kill();
        /* (v_m*Q_pk)*v_m */
        res=res_tmp*v_m;
        v_m.kill();
        res_tmp.kill();

        BytesFromGF2X((unsigned char*)c,conv<GF2X>(res),(HFEmq<<3));
        clear(res);
    #endif

    #if HFEmr
        c[HFEmq]=0UL;
    #endif
    HFEADD2m(c,pk);

    /* Evaluation of the HFEmr last equations */
    #if HFEmr
        pk+=ACCESS_last_equations;
        c+=HFEmq;

        unsigned int i;
        for(i=0;i<HFEmr;++i)
        {
            (*c)^=evalMQnocst_gf2(m,pk+NB_WORD_ONE_EQUATION*i)<<i;
        }
    #endif
}


/* Input:
    m a vector of v elements of GF(2)
    pk a MQ system with n equations in GF(2)[x1,...,xv]

  Output:
    c a vector of n elements of GF(2), c is the evaluation of pk in m
*/
/* This function is just a copy of the previous function, in modifying the 
 * values for the number of equations and the number of variables */
void evaluateMQS(vecn_gf2 c, cst_vecv_gf2 m, cst_mqsv_gf2n pk)
{
    /* Just to set the degree of extension to HFEn */
    /* The choice of f is not important here */
    GF2X f=GF2X(HFEn,1);
    SetCoeff(f,0);
    #ifdef __PENTANOMHFE__
        SetCoeff(f,K1);
        SetCoeff(f,K2);
    #endif
    SetCoeff(f,K3);
    GF2E::init(f);
    f.kill();

    mat_GF2E MQS;
    vec_GF2E v_m,res_tmp;
    vec_GF2 c_vec;
    GF2E res;

    /* Begin after the constant */
    convUINTToNTLmat_GF2E(MQS,pk+NB_WORD_GFqn);
    convUINTToNTLvecv_GF2(c_vec,m);
    conv(v_m,c_vec);
    c_vec.kill();

    /* v_m*Q_pk */
    mul(res_tmp,v_m,MQS);
    MQS.kill();
    /* (v_m*Q_pk)*v_m */
    res=res_tmp*v_m;
    v_m.kill();
    res_tmp.kill();

    BytesFromGF2X((unsigned char*)c,conv<GF2X>(res),NB_BYTES_GFqn);
    clear(res);
    /* Clean the last word */
    #if HFEnr
        c[NB_WORD_GFqn-1]&=HFE_MASKn;
    #endif

    /* (v_m*Q_pk*v_m) + cst_pk */
    HFEADD2(c,pk);
}
