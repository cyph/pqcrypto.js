#ifndef _GENHFETAB_HPP
#define _GENHFETAB_HPP

#include "config_HFE.h"
#include <NTL/mat_GF2.h>

using namespace NTL;

void genHFEpolynom(sparse_monic_gf2nx F);
void genRandomInvertibleMatrixn(mat_GF2& S_inv, mat_GF2& S);
void genRandomInvertibleMatrixnv(mat_GF2& S_inv, mat_GF2& S);
int genRandomInvertibleMatrixLUn(mat_GF2& S);
int genRandomInvertibleMatrixLUnv(mat_GF2& S);
void genPlaintext(vecnv_gf2 plain);

#endif

