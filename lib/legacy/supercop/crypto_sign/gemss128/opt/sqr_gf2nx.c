#include "sqr_gf2nx.h"

#include "tools_gf2n.h"
#include "arith_gf2n.h"


/* Input: poly an element of GF(2^n)[X], of degree d
   Output: the square of poly
*/
/* In place */
void PREFIX_NAME(sqr_gf2nx)(gf2nx poly, unsigned int d)
{
    gf2nx poly_2i;
    /* Beginning: last coefficient */
    unsigned int i=NB_WORD_GFqn*d;
    poly+=i;
    poly_2i=poly+i;
    /* for each coefficient */
    for(i=0;i<d;++i)
    {
        sqr_gf2n(poly_2i,poly);
        poly-=NB_WORD_GFqn;
        poly_2i-=NB_WORD_GFqn;
/*        STORE64(poly_2i+2,_mm_setzero_si128()); */
/*        STORE128(poly_2i,_mm_setzero_si128()); */
        set0_gf2n(poly_2i);
        poly_2i-=NB_WORD_GFqn;
    }

    /* Constant coefficient */
    sqr_gf2n(poly,poly);
}


/* Input: poly an element of GF(2^n)[X], of degree at most HFEDeg-1
   Output: the square of poly

   Requirement: 
    for the vectorial version, poly must be aligned
*/
/* In place */
void PREFIX_NAME(sqr_HFE_gf2nx)(gf2nx poly)
{
    gf2nx poly_2i;
    /* Beginning: last coefficient */
    unsigned int i=NB_WORD_GFqn*(HFEDeg-1);
    poly+=i;
    poly_2i=poly+i;
    /* for each coefficient */
    for(i=0;i<(HFEDeg-1);++i)
    {
        sqr_gf2n(poly_2i,poly);
        poly-=NB_WORD_GFqn;
        poly_2i-=NB_WORD_GFqn;
        set0_gf2n(poly_2i);
        poly_2i-=NB_WORD_GFqn;
    }

    /* Constant coefficient */
    sqr_gf2n(poly,poly);
}


