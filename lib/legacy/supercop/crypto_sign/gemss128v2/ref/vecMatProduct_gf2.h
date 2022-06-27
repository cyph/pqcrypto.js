#ifndef _VECMATPRODUCT_GF2_H
#define _VECMATPRODUCT_GF2_H

#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "tools_gf2nv.h"
#include "tools_gf2m.h"
#include "tools_gf2v.h"
#include "matrix_gf2.h"


/* To choose the best vector matrix product */

/* (1,n+v)*(n+v,n+v) */
#define vecMatProductnv_gf2 vecMatProductnv_64

/* (1,n+v)*(n+v,n) */
#define vecMatProductnvn_gf2 vecMatProductnvn_64

/* (1,v)*(v,n) */
#define vecMatProductv_gf2 vecMatProductv_64

/* (1,n)*(n,n) */
#define vecMatProductn_gf2 vecMatProductn_64

/* (1,n)*(n,n) but the result is (1,m) with m <= n */
#define vecMatProductm_gf2 vecMatProductm_64


/* (1,n+v)*(n+v,n) with "start" */
#define vecMatProductnvn_start_gf2 vecMatProductnvn_start_64


/* (1,n)*(n,n) with "start" */
#define vecMatProductn_start_gf2 vecMatProductn_start_64




/* Without SIMD */

/* (1,n+v)*(n+v,n+v) */
void PREFIX_NAME(vecMatProductnv_64)(vecnv_gf2 res, cst_vecnv_gf2 vec,
                                                    cst_Mnv_gf2 S);
/* (1,n+v)*(n+v,n) */
void PREFIX_NAME(vecMatProductnvn_64)(vecn_gf2 res, cst_vecnv_gf2 vec,
                                                    cst_Mnv_n_gf2 S);
/* (1,v)*(v,n) */
void PREFIX_NAME(vecMatProductv_64)(vecn_gf2 res, cst_vecv_gf2 vec,
                                                  cst_Mv_n_gf2 S);
/* (1,n)*(n,n) */
void PREFIX_NAME(vecMatProductn_64)(vecn_gf2 res, cst_vecn_gf2 vec,
                                                  cst_Mn_gf2 S);
/* (1,n)*(n,n) but the result is (1,m) with m <= n */
void PREFIX_NAME(vecMatProductm_64)(vecn_gf2 res, cst_vecn_gf2 vec,
                                                  cst_Mn_gf2 S);

#define vecMatProductnv_64 PREFIX_NAME(vecMatProductnv_64)
#define vecMatProductnvn_64 PREFIX_NAME(vecMatProductnvn_64)
#define vecMatProductv_64 PREFIX_NAME(vecMatProductv_64)
#define vecMatProductn_64 PREFIX_NAME(vecMatProductn_64)
#define vecMatProductm_64 PREFIX_NAME(vecMatProductm_64)


/* (1,n+v)*(n+v,n) with "start" */
void PREFIX_NAME(vecMatProductnvn_start_64)(vecn_gf2 res, cst_vecnv_gf2 vec,
                                           cst_Mnv_n_gf2 S, unsigned int start);
/* (1,n)*(n,n) with "start" */
/*
void PREFIX_NAME(vecMatProductn_start_64)(vecn_gf2 res, cst_vecn_gf2 vec,
                                          cst_Mn_gf2 S, unsigned int start);
*/
#define vecMatProductnvn_start_64 PREFIX_NAME(vecMatProductnvn_start_64)
/*
#define vecMatProductn_start_64 PREFIX_NAME(vecMatProductn_start_64)
*/


#endif

