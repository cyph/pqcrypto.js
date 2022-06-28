#include "init_list_differences.h"


/* Initialize L to the list of the successive differences of exposants of the 
   monoms of a HFE polynomial, multiplied by NB_WORD_GFqn */
void init_list_differences(unsigned int* L)
{
    #if(HFEDeg!=1)
        unsigned int i,j,k=2U;
    #endif

    L[0]=0U;
    #if(HFEDeg!=1)
        L[1]=NB_WORD_GFqn;
        for(i=0;i<HFEDegI;++i)
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
        }
        #if HFEDegJ
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
}
