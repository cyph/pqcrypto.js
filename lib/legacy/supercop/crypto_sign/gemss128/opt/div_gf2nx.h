#ifndef _DIV_GF2NX_H
#define _DIV_GF2NX_H

#include "prefix_name.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "gf2nx.h"


unsigned int PREFIX_NAME(div_qr_gf2nx)(gf2nx Q, gf2nx A, unsigned int deg, cst_gf2nx B, \
                           unsigned int d2);
unsigned int PREFIX_NAME(div_qr_not_monic_gf2nx)(gf2nx Q, gf2nx A, unsigned int deg, 
                                    cst_gf2nx B, unsigned int d2);
unsigned int PREFIX_NAME(div_r_gf2nx)(gf2nx A, unsigned int deg, cst_gf2nx B, \
                         unsigned int d2);
unsigned int PREFIX_NAME(div_r_not_monic_gf2nx)(gf2nx A, unsigned int deg, cst_gf2nx B, \
                         unsigned int d2);
void PREFIX_NAME(div_r_HFE_gf2nx_d_cst)(gf2nx poly, unsigned int d, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst);
unsigned int PREFIX_NAME(div_r_HFE_gf2nx)(gf2nx poly, unsigned int deg, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst);
void PREFIX_NAME(div_r_HFE_gf2nx_cst)(gf2nx poly, \
                             const complete_sparse_monic_gf2nx F, cst_gf2n cst);

#define div_qr_gf2nx PREFIX_NAME(div_qr_gf2nx)
#define div_qr_not_monic_gf2nx PREFIX_NAME(div_qr_not_monic_gf2nx)
#define div_r_gf2nx PREFIX_NAME(div_r_gf2nx)
#define div_r_not_monic_gf2nx PREFIX_NAME(div_r_not_monic_gf2nx)
#define div_r_HFE_gf2nx_d_cst PREFIX_NAME(div_r_HFE_gf2nx_d_cst)
#define div_r_HFE_gf2nx PREFIX_NAME(div_r_HFE_gf2nx)
#define div_r_HFE_gf2nx_cst PREFIX_NAME(div_r_HFE_gf2nx_cst)



#endif

