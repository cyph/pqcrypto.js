#ifndef _GENHFETAB_H
#define _GENHFETAB_H

#include "config_HFE.h"


void genHFEpolynom(sparse_monic_gf2nx F);
int genRandomInvertibleMatrixn(GLn_gf2 S);
int genRandomInvertibleMatrixnv(GLnv_gf2 S);
int genRandomInvertibleMatrixnLU(GLn_gf2 S);
int genRandomInvertibleMatrixnvLU(GLnv_gf2 S);
void genPlaintext(vecnv_gf2 plain);

#endif

