#ifndef _DIV_GF2NX_H
#define _DIV_GF2NX_H


#include "prefix_name.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "gf2nx.h"


unsigned int PREFIX_NAME(div_qr_monic_gf2nx)(gf2nx A, unsigned int da,
                                         cst_gf2nx B, unsigned int db);
unsigned int PREFIX_NAME(div_q_monic_gf2nx)(gf2nx A, unsigned int da,
                                        cst_gf2nx B, unsigned int db);
unsigned int PREFIX_NAME(div_r_gf2nx)(gf2nx A, unsigned int da,
                                  cst_gf2nx B, unsigned int db);
void PREFIX_NAME(div_r_monic_cst_gf2nx)(gf2nx A, unsigned int da,
                                    cst_gf2nx B, unsigned int db);
#define div_qr_monic_gf2nx PREFIX_NAME(div_qr_monic_gf2nx)
#define div_r_monic_gf2nx div_qr_monic_gf2nx
#define div_q_monic_gf2nx PREFIX_NAME(div_q_monic_gf2nx)
#define div_r_gf2nx PREFIX_NAME(div_r_gf2nx)
#define div_r_monic_cst_gf2nx PREFIX_NAME(div_r_monic_cst_gf2nx)


unsigned int PREFIX_NAME(div_r_HFE_gf2nx)(gf2nx poly, unsigned int dp,
                                          const complete_sparse_monic_gf2nx F,
                                          cst_gf2n cst);
void PREFIX_NAME(div_r_HFE_cstdeg_gf2nx)(gf2nx poly, unsigned int dp,
                                         const complete_sparse_monic_gf2nx F,
                                         cst_gf2n cst);
void PREFIX_NAME(div_r_HFE_cst_gf2nx)(gf2nx poly,
                                      const complete_sparse_monic_gf2nx F,
                                      cst_gf2n cst);
#define div_r_HFE_gf2nx PREFIX_NAME(div_r_HFE_gf2nx)
#define div_r_HFE_cstdeg_gf2nx PREFIX_NAME(div_r_HFE_cstdeg_gf2nx)
#define div_r_HFE_cst_gf2nx PREFIX_NAME(div_r_HFE_cst_gf2nx)


#if ENABLED_REMOVE_ODD_DEGREE
    void PREFIX_NAME(divsqr_r_HFE_cstdeg_gf2nx)(gf2nx poly, unsigned int dp,
                                           const complete_sparse_monic_gf2nx F,
                                           cst_gf2n cst);
    void PREFIX_NAME(divsqr_r_HFE_cst_gf2nx)(gf2nx poly,
                                         const complete_sparse_monic_gf2nx F,
                                         cst_gf2n cst);
    #define divsqr_r_HFE_cstdeg_gf2nx PREFIX_NAME(divsqr_r_HFE_cstdeg_gf2nx)
    #define divsqr_r_HFE_cst_gf2nx PREFIX_NAME(divsqr_r_HFE_cst_gf2nx)
#else
    #define divsqr_r_HFE_cstdeg_gf2nx PREFIX_NAME(div_r_HFE_cstdeg_gf2nx)
    #define divsqr_r_HFE_cst_gf2nx PREFIX_NAME(div_r_HFE_cst_gf2nx)
#endif


#endif

