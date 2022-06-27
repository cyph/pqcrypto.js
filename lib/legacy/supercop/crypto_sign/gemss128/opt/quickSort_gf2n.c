#include "quickSort_gf2n.h"
#include "tools_gf2n.h"



void PREFIX_NAME(quickSort_gf2n)(vec_gf2n tab,int d,int f)
{
    /* If the length is null */
    if(d>=f)
    {
        return;
    }

    int left=d;
    int right=f;

    while(1)
    {
        while(cmp_lt_gf2n(tab+left,tab+d))
        {
            left+=NB_WORD_GFqn;
        }
        while(cmp_gt_gf2n(tab+right,tab+d))
        {
            right-=NB_WORD_GFqn;
        }

        if(left<right)
        {
            swap_gf2n(tab+left,tab+right);
        } else
        {
            break;
        }
    }

    quickSort_gf2n(tab,d,right);
    quickSort_gf2n(tab,right+NB_WORD_GFqn,f);
}

