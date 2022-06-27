#include "gcd_gf2nx.h"
#include "div_gf2nx.h"

#include "tools_gf2n.h"
#include "inv_gf2n.h"
#include "arith_gf2n.h"




/* Input: A an element of GF(2^n)[X], of degree da
          B an element of GF(2^n)[X], of degree db
   Output: If *b is 1, swap A and B after to have used this function.

           B is the last rem, and A is the rem previous B.
           Necessarily, the degree of B is 0 (or (-1)).
           So, we return the degree of A.
           If B is zero, A is the gcd and its degree is da.
           Else B is a non zero constant, so B is the gcd and its degree is 0.
*/
/* Requirement: 
    da>db
    for the vectorial version, A and B must be aligned
*/
/* In place */
unsigned int PREFIX_NAME(gcd_gf2nx)(unsigned int* b, gf2nx A, unsigned int da, \
                                        gf2nx B, unsigned int db)
{
    static_gf2n inv[NB_WORD_GFqn] ALIGNED;
    gf2nx tmp;
    unsigned int i;

    /* *b = 0: B is the last remainder
       *b = 1: A is the last remainder */
    *b=0;

    while(db)
    {
        /* Computation of A = A mod B, of degree da */

        /* Minimizes the number of multiplication by an inverse */
        /* 2db > da */
        if( (db<<1) > da )
        {
            /* At most da-db+1 multiplications by an inverse */
            da=div_r_not_monic_gf2nx(A,da,B,db);
        } else
        {
            /* B becomes monic: db multiplications by an inverse */
            inv_gf2n(inv,B+db*NB_WORD_GFqn);
            set1_gf2n(B+db*NB_WORD_GFqn);
            for(i=db-1;i!=(unsigned int)(-1);--i)
            {
                mul_gf2n(B+i*NB_WORD_GFqn,B+i*NB_WORD_GFqn,inv);
            }
            da=div_r_gf2nx(A,da,B,db);
        }

        /* Swaps A and B */
        tmp=A;
        A=B;
        B=tmp;
        /* Swaps da and db */
        PERMUTATION64(da,db);
        /* 0 becomes 1 and 1 becomes 0 */
        *b=(1-(*b));
    }

    return da;
}


