#include "genCanonicalBasis_gf2n.h"
#include "arith_gf2n.h"



/* Input:
    alpha_vec a matrix with HFEDegI+1 rows and HFEn-1 columns, in GF(2^n)

  Output:
    alpha_vec where the ith row is the power 2^i of the canonical base of GF(2^n)
    the rows go from 0 to HFEDegI
    the first row is (a,a^2,...,a^(n-1)), it does not store a^0

  Requirement: 
    the elements of alpha_vec must be initialised to 0
    for the vectorial version, alpha_vec must be aligned
*/
void PREFIX_NAME(genCanonicalBasis_gf2n)(vec_gf2n alpha_vec)
{
    vec_gf2n a_vec,a_vec2;
    unsigned int i,j;

    /* Current row */
    a_vec=alpha_vec;

    /* First row of alpha_vec : init to (a,a^2,...,a^(n-1)) */
    /* The element a^0 = 1 is implicit, so it is not stored */
    /* n = q*NB_BITS_UINT + r */
    /* Each element has the form a^(i*NB_BITS_UINT + j) */

    #if(NB_WORD_GFqn==1)
        for(j=1;j<HFEn;++j)
        {
            /* It is a^j */
            *a_vec=1UL<<j;
            ++a_vec;
        }
    #else

        /* i=0 */
        /* j=0 is the case a^0 */
        /* Put the bit 1 at the position j */
        for(j=1;j<NB_BITS_UINT;++j)
        {
            /* It is a^(i*NB_BITS_UINT + j) */
            *a_vec=1UL<<j;
            a_vec+=NB_WORD_GFqn;
        }
        ++a_vec;

        for(i=1;i<HFEnq;++i)
        {
            *a_vec=1UL;
            a_vec+=NB_WORD_GFqn;
            /* Put the bit 1 at the position j */
            for(j=1;j<NB_BITS_UINT;++j)
            {
                /* It is a^(i*NB_BITS_UINT + j) */
                *a_vec=1UL<<j;
                a_vec+=NB_WORD_GFqn;
            }
            ++a_vec;
        }
        /* i = NB_WORD_GFqn-1 */
        #if (HFEnr)
            *a_vec=1UL;
            a_vec+=NB_WORD_GFqn;
            for(j=1;j<HFEnr;++j)
            {
                *a_vec=1UL<<j;
                a_vec+=NB_WORD_GFqn;
            }
            ++a_vec;
        #endif
        a_vec-=NB_WORD_GFqn;
    #endif

    /* Here, a_vec = row 1 */
    /* row before current row, init at row 0 */
    a_vec2=alpha_vec;
    /* For the row i+1 : (a^(2^i),a^(2*(2^i)),...,a^((n-1)*(2^i))) */
    #if(HFEDegI!=HFEDegJ)
    for(i=0;i<HFEDegI;++i)
    #else
    for(i=0;i<=HFEDegI;++i)
    #endif
    {
        /* j=0 is the case (a^0)^(2^i)=1, it is not stored */
        for(j=1;j<HFEn;++j)
        {
            /* a^(2^i) = (a^(2^(i-1)))^2 */
            sqr_gf2n(a_vec,a_vec2);
            a_vec+=NB_WORD_GFqn;
            a_vec2+=NB_WORD_GFqn;
        }
    }

}


/* Input:
    alpha_vec a matrix with HFEn-1 rows and HFEDegI+1 columns, in GF(2^n)

  Output:
    alpha_vec where the ith column is the power 2^i of the canonical base of GF(2^n)
    the columns go from 0 to HFEDegI
    the first column is (a,a^2,...,a^(n-1)), it does not store a^0

  Requirement: 
    the elements of alpha_vec must be initialised to 0
    for the vectorial version, alpha_vec must be aligned
*/
void PREFIX_NAME(genCanonicalBasisVertical_gf2n)(vec_gf2n alpha_vec)
{
    unsigned int i;
    #if(HFEDeg!=1)
        unsigned int j;
    #endif

    /* for each element of the canonical basis */
    for(i=1;i<HFEn;++i)
    {
        /* j=0: alpha^i */
        alpha_vec[i>>6]=1UL<<(i&63);

        #if(HFEDeg!=1)

        /* Compute (alpha^i)^(2^j) */
        #if(HFEDegI!=HFEDegJ)
        for(j=0;j<HFEDegI;++j)
        #else
        for(j=0;j<=HFEDegI;++j)
        #endif
        {
            sqr_gf2n(alpha_vec+NB_WORD_GFqn,alpha_vec);
            alpha_vec+=NB_WORD_GFqn;
        }

        #endif
        alpha_vec+=NB_WORD_GFqn;
    }
}



