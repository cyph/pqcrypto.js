#ifndef _FROBENIUSMAP_GF2NX_H
#define _FROBENIUSMAP_GF2NX_H


#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "gf2nx.h"


unsigned int PREFIX_NAME(frobeniusMap_HFE_gf2nx)(gf2nx Xqn, const
                                    complete_sparse_monic_gf2nx F, cst_gf2n U);
#define frobeniusMap_HFE_gf2nx PREFIX_NAME(frobeniusMap_HFE_gf2nx)


#define best_frobeniusMap_HFE_gf2nx frobeniusMap_HFE_gf2nx


#endif

