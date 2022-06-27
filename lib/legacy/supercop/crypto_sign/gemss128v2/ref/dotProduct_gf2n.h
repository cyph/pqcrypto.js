#ifndef _DOTPRODUCT_GF2N_H
#define _DOTPRODUCT_GF2N_H


#include "prefix_name.h"
#include "tools_gf2n.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
/* Really important include for the definition of 
   __TRINOMIAL_GF2N__ and __PENTANOMIAL_GF2N__ */
#include "rem_gf2n.h"


/* Choice the best dotProduct */
#define dotProduct_gf2n dotProduct_no_simd_gf2n
#define doubleDotProduct_gf2n doubleDotProduct_no_simd_gf2n


void PREFIX_NAME(dotProduct_no_simd_gf2n)(uint64_t res[NB_WORD_GFqn],
                     const UINT* vec_x, const UINT* vec_y, unsigned int len);
void PREFIX_NAME(doubleDotProduct_no_simd_gf2n)(uint64_t res[NB_WORD_GFqn],
                     const UINT* vec_x, const UINT* vec_y,
                     const UINT* vec2_x, const UINT* vec2_y, unsigned int len);

#define dotProduct_no_simd_gf2n PREFIX_NAME(dotProduct_no_simd_gf2n)
#define doubleDotProduct_no_simd_gf2n PREFIX_NAME(doubleDotProduct_no_simd_gf2n)


#endif

