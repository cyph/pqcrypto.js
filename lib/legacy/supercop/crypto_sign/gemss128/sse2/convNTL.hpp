#ifndef _CONVNTL_HPP
#define _CONVNTL_HPP

#include "config_HFE.h"
#include <NTL/GF2EX.h>

using namespace NTL;

void convHFEpolynomToNTLGF2EX(GF2EX& F_NTL,cst_sparse_monic_gf2nx F);
void convPolynomToNTLGF2EX(GF2EX& F_NTL,const UINT* F);

#endif

