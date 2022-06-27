#include "dotProduct_gf2n.h"
#include "mul_gf2x.h"
#include "arith_gf2n.h"



void PREFIX_NAME(dotProduct_noSIMD_gf2n)(uint64_t res[NB_WORD_GFqn], \
                     const UINT* vec_x, const UINT* vec_y, unsigned int len)
{
    static_gf2n acc[NB_WORD_MUL] ALIGNED, tmp_mul[NB_WORD_MUL] ALIGNED;
    unsigned int i;

    /* i=0 */
    mul_gf2x(acc,vec_x,vec_y);

    for(i=1;i<len;++i)
    {
        vec_x+=NB_WORD_GFqn;
        vec_y+=NB_WORD_GFqn;
        mul_gf2x(tmp_mul,vec_x,vec_y);
        add2mul_gf2n(acc,tmp_mul);
    }

    rem_gf2n(res,acc);
}

void PREFIX_NAME(doubleDotProduct_noSIMD_gf2n)(uint64_t res[NB_WORD_GFqn], \
                     const UINT* vec_x, const UINT* vec_y, \
                     const UINT* vec2_x, const UINT* vec2_y, unsigned int len)
{
    static_gf2n acc[NB_WORD_MUL] ALIGNED, tmp_mul[NB_WORD_MUL] ALIGNED;
    unsigned int i;

    /* i=0 */
    mul_gf2x(acc,vec_x,vec_y);

    for(i=1;i<len;++i)
    {
        vec_x+=NB_WORD_GFqn;
        vec_y+=NB_WORD_GFqn;
        mul_gf2x(tmp_mul,vec_x,vec_y);
        add2mul_gf2n(acc,tmp_mul);
    }

    for(i=0;i<len;++i)
    {
        mul_gf2x(tmp_mul,vec2_x,vec2_y);
        add2mul_gf2n(acc,tmp_mul);
        vec2_x+=NB_WORD_GFqn;
        vec2_y+=NB_WORD_GFqn;
    }

    rem_gf2n(res,acc);
}



