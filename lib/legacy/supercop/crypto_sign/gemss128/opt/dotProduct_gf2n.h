#ifndef _DOTPRODUCT_GF2N_H
#define _DOTPRODUCT_GF2N_H


#include "prefix_name.h"
#include "arch.h"
#include "parameters_HFE.h"
#include "macro.h"
#include "rem_gf2n.h"



void PREFIX_NAME(dotProduct_noSIMD_gf2n)(uint64_t res[NB_WORD_GFqn], \
                     const UINT* vec_x, const UINT* vec_y, unsigned int len);
void PREFIX_NAME(doubleDotProduct_noSIMD_gf2n)(uint64_t res[NB_WORD_GFqn], \
                     const UINT* vec_x, const UINT* vec_y, \
                     const UINT* vec2_x, const UINT* vec2_y, unsigned int len);

#define dotProduct_noSIMD_gf2n PREFIX_NAME(dotProduct_noSIMD_gf2n)
#define doubleDotProduct_noSIMD_gf2n PREFIX_NAME(doubleDotProduct_noSIMD_gf2n)



#define dotProduct_gf2n dotProduct_noSIMD_gf2n
#define doubleDotProduct_gf2n doubleDotProduct_noSIMD_gf2n



#endif
