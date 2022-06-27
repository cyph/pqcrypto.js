#ifndef _INVMATRIXN_GF2_H
#define _INVMATRIXN_GF2_H

#include "prefix_name.h"
#include "matrix_gf2.h"
#include "arch.h"


void PREFIX_NAME(invMatrixn_nocst_gf2)(GLn_gf2 S_inv, GLn_gf2 S);
#define invMatrixn_nocst_gf2 PREFIX_NAME(invMatrixn_nocst_gf2)

void PREFIX_NAME(invMatrixn_cst_gf2)(GLn_gf2 S_inv, GLn_gf2 S);
#define invMatrixn_cst_gf2 PREFIX_NAME(invMatrixn_cst_gf2)

void PREFIX_NAME(invMatrixLUn_gf2)(GLn_gf2 S_inv, cst_Tn_gf2 L, cst_Tn_gf2 U);
#define invMatrixLUn_gf2 PREFIX_NAME(invMatrixLUn_gf2)


#if CONSTANT_TIME
    #define invMatrixn_gf2 invMatrixn_cst_gf2
#else
    #define invMatrixn_gf2 invMatrixn_nocst_gf2
#endif


#endif

