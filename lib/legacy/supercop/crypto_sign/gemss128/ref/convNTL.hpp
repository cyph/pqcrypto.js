#ifndef _CONVNTL_HPP
#define _CONVNTL_HPP

#include "config_HFE.h"
#include <NTL/GF2EX.h>
#include <NTL/mat_GF2.h>
#include <NTL/mat_GF2E.h>


using namespace NTL;

void convHFEpolynomToNTLGF2EX(GF2EX& F_NTL,cst_sparse_monic_gf2nx F);
void convHFEpolynomVToNTLGF2EX(GF2EX& F_NTL,cst_sparse_monic_gf2nx F);

void convNTLvecn_GF2ToUINT(vecn_gf2 res,const vec_GF2& S);
void convNTLvecnv_GF2ToUINT(vecnv_gf2 res,const vec_GF2& S);

void convUINTToNTLvecv_GF2(vec_GF2& res,cst_vecv_gf2 S);
void convUINTToNTLvecn_GF2(vec_GF2& res,cst_vecn_gf2 S);
void convUINTToNTLvecnv_GF2(vec_GF2& res,cst_vecnv_gf2 S);

void convNTLmatT_GF2ToUINT(Mn_gf2 res,const mat_GF2& S);
void convNTLmatS_GF2ToUINT(Mnv_gf2 res,const mat_GF2& S);

void convUINTToNTLmatT_GF2(mat_GF2& res,cst_Mn_gf2 S);
void convUINTToNTLmatS_GF2(mat_GF2& res,cst_Mnv_gf2 S);
void convUINTToNTLmatV_GF2(mat_GF2& res,cst_Mv_n_gf2 S);

void convUINTlowerToNTLmatn_GF2(mat_GF2& res,cst_Tn_gf2 S);
void convUINTlowerToNTLmatnv_GF2(mat_GF2& res,cst_Tnv_gf2 S);

void convUINT_pk_ToNTLmat_GF2E(mat_GF2E& res,cst_mqsnv_gf2m MQS);
#if HFEmq
    void convUINT_pk_hybrid_ToNTLmat_GF2E(mat_GF2E& res,cst_mqsnv_gf2m MQS);
#endif
void convUINTToNTLmat_GF2E(mat_GF2E& res,cst_mqsv_gf2n MQS);

#endif

