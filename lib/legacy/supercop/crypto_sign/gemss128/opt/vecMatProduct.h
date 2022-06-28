#ifndef _VECMATPRODUCT_H
#define _VECMATPRODUCT_H

#include "config_HFE.h"

void vecMatProductv(vecn_gf2 res, cst_vecv_gf2 vec, cst_Mv_n_gf2 S);
void vecMatProductnv(vecnv_gf2 res, cst_vecnv_gf2 vec, cst_Mnv_gf2 S);

#endif

