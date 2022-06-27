#ifndef _MATVECPRODUCT_GF2_H
#define _MATVECPRODUCT_GF2_H

#include "prefix_name.h"
#include "matrix_gf2.h"


void PREFIX_NAME(matVecProduct_gf2)(vecn_gf2 res, cst_Mn_gf2 T, cst_vecn_gf2 vec);
#define matVecProduct_gf2 PREFIX_NAME(matVecProduct_gf2)


#endif

