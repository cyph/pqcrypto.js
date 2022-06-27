#ifndef _FINDROOTSSPLIT_GF2NX_H
#define _FINDROOTSSPLIT_GF2NX_H


#include "prefix_name.h"
#include "matrix_gf2n.h"
#include "gf2nx.h"


void PREFIX_NAME(findRootsSplit_gf2nx)(vec_gf2n roots, gf2nx f, unsigned int deg);
#define findRootsSplit_gf2nx PREFIX_NAME(findRootsSplit_gf2nx)


#endif
