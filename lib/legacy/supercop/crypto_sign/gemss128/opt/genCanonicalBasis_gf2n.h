#ifndef _GENCANONICALBASIS_GF2N_H
#define _GENCANONICALBASIS_GF2N_H

#include "prefix_name.h"
#include "matrix_gf2n.h"


void PREFIX_NAME(genCanonicalBasis_gf2n)(vec_gf2n alpha_vec);
void PREFIX_NAME(genCanonicalBasisVertical_gf2n)(vec_gf2n alpha_vec);

#define genCanonicalBasis_gf2n PREFIX_NAME(genCanonicalBasis_gf2n)
#define genCanonicalBasisVertical_gf2n PREFIX_NAME(genCanonicalBasisVertical_gf2n)

#endif

