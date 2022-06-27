#ifndef _INVMATRIXNV_GF2_H
#define _INVMATRIXNV_GF2_H

#include "prefix_name.h"
#include "matrix_gf2.h"
#include "arch.h"


void PREFIX_NAME(invMatrixnv_nocst_gf2)(GLnv_gf2 S_inv, GLnv_gf2 S);
#define invMatrixnv_nocst_gf2 PREFIX_NAME(invMatrixnv_nocst_gf2)

void PREFIX_NAME(invMatrixnv_cst_gf2)(GLnv_gf2 S_inv, GLnv_gf2 S);
#define invMatrixnv_cst_gf2 PREFIX_NAME(invMatrixnv_cst_gf2)

void PREFIX_NAME(invMatrixLUnv_gf2)(GLnv_gf2 S_inv, cst_Tnv_gf2 L,
                                                    cst_Tnv_gf2 U);
#define invMatrixLUnv_gf2 PREFIX_NAME(invMatrixLUnv_gf2)


#if CONSTANT_TIME
    #define invMatrixnv_gf2 invMatrixnv_cst_gf2
#else
    #define invMatrixnv_gf2 invMatrixnv_nocst_gf2
#endif


#endif

