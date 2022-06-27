#include "initListDifferences_gf2nx.h"
#include "parameters_HFE.h"
#include "tools_gf2n.h"
#include "gf2nx.h"


/* Initialize L to the list of the successive differences of exposants of the 
   monomials of a HFE polynomial, multiplied by NB_WORD_GFqn */
void PREFIX_NAME(initListDifferences_gf2nx)(unsigned int* L)
{
    #if(HFEDeg!=1)
        unsigned int i,j,k=2U;
    #endif

    L[0]=0U;
    #if(HFEDeg!=1)
        L[1]=NB_WORD_GFqn;
        for(i=0;i<HFEDegI;++i)
        {
            #if ENABLED_REMOVE_ODD_DEGREE
            if(((1U<<i)+1U)<=HFE_odd_degree)
            {
                /* j=0 */
                L[k]=NB_WORD_GFqn;
                ++k;

                /* j=1 to j=i */
                for(j=0;j<i;++j)
                {
                    L[k]=NB_WORD_GFqn<<j;
                    ++k;
                }
            } else
            {
                /* j=0 */
                if(i)
                {
                    L[k]=NB_WORD_GFqn<<1;
                    ++k;
                }

                /* j=1 to j=i */
                for(j=1;j<i;++j)
                {
                    L[k]=NB_WORD_GFqn<<j;
                    ++k;
                }
            }
            #else
            /* j=0 */
            L[k]=NB_WORD_GFqn;
            ++k;

            /* j=1 to j=i */
            for(j=0;j<i;++j)
            {
                L[k]=NB_WORD_GFqn<<j;
                ++k;
            }
            #endif
        }
        #if HFEDegJ
            #if ENABLED_REMOVE_ODD_DEGREE
            if(((1U<<i)+1U)<=HFE_odd_degree)
            {
                /* j=0 */
                L[k]=NB_WORD_GFqn;
                ++k;

                /* j=1 to j=i */
                for(j=0;j<(HFEDegJ-1);++j)
                {
                    L[k]=NB_WORD_GFqn<<j;
                    ++k;
                }
            } else
            {
                /* j=0 */
                #if (HFEDegJ!=1)
                    L[k]=NB_WORD_GFqn<<1;
                    ++k;
                #endif

                /* j=1 to j=i */
                for(j=1;j<(HFEDegJ-1);++j)
                {
                    L[k]=NB_WORD_GFqn<<j;
                    ++k;
                }
            }
            #else
            /* j=0*/
            L[k]=NB_WORD_GFqn;
            ++k;

            /* j=1 to j=HFEDegJ-1 */
            for(j=0;j<(HFEDegJ-1);++j)
            {
                L[k]=NB_WORD_GFqn<<j;
                ++k;
            }
            #endif
        #endif
    #endif
}
