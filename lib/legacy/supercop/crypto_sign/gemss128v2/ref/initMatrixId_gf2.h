#ifndef _INITMATRIXID_GF2_H
#define _INITMATRIXID_GF2_H

#include "prefix_name.h"
#include "matrix_gf2.h"


void PREFIX_NAME(initMatrixIdn_gf2)(Mn_gf2 S);
void PREFIX_NAME(initMatrixIdnv_gf2)(Mnv_gf2 S);

#define initMatrixIdn_gf2 PREFIX_NAME(initMatrixIdn_gf2)
#define initMatrixIdnv_gf2 PREFIX_NAME(initMatrixIdnv_gf2)


#endif

