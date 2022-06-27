#ifndef _CONV_GF2NX_H
#define _CONV_GF2NX_H

#include "prefix_name.h"
#include "gf2nx.h"


void PREFIX_NAME(convMonic_gf2nx)(gf2nx F, unsigned int d);
#define convMonic_gf2nx PREFIX_NAME(convMonic_gf2nx)


void PREFIX_NAME(convHFEpolynomialSparseToDense_gf2nx)(gf2nx F_dense, \
                                          const complete_sparse_monic_gf2nx F);
#define convHFEpolynomialSparseToDense_gf2nx PREFIX_NAME(convHFEpolynomialSparseToDense_gf2nx)



#endif
