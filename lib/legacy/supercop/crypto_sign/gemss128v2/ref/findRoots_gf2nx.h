#ifndef _FINDROOTS_GF2NX_H
#define _FINDROOTS_GF2NX_H

#include "prefix_name.h"
#include "matrix_gf2n.h"
#include "tools_gf2n.h"
#include "gf2nx.h"


#include "conv_gf2nx.h"


/* GCD(F-U, X^(2^n)-X mod (F-U)) */
/* To know the number of roots of F-U */
#define firstStepHFE_gf2nx(l,i,poly,poly2,F,U) \
    /* X^(2^n) - X mod (F-U) */\
    l=best_frobeniusMap_HFE_gf2nx(poly,F,U);\
    /* Initialize to F */\
    convHFEpolynomialSparseToDense_gf2nx(poly2,F);\
    /* Initialize to F-U */\
    add2_gf2n(poly2,U);\
    l=gcd_gf2nx(&i,poly2,d2,poly,l);


int PREFIX_NAME(findRootsHFE_gf2nx)(vec_gf2n* roots,
                                    const complete_sparse_monic_gf2nx F,
                                    cst_gf2n U);
int PREFIX_NAME(findUniqRootHFE_gf2nx)(gf2n root,
                                       const complete_sparse_monic_gf2nx F,
                                       cst_gf2n U);
#define findRootsHFE_gf2nx PREFIX_NAME(findRootsHFE_gf2nx)
#define findUniqRootHFE_gf2nx PREFIX_NAME(findUniqRootHFE_gf2nx)


#endif

