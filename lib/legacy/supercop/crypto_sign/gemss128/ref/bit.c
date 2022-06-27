#include "bit.h"



#if (NB_BITS_UINT!=64U)
    int ISZERO(const UINT* a,unsigned int size)
    {
        unsigned int i;
        for(i=0;i<size;++i)
        {
            if(a[i])
            {
                return 1;
            }
        }
        return 0;
    }

    int ISEQUAL(const UINT* a, const UINT* b,unsigned int size)
    {
        unsigned int i;
        for(i=0;i<size;++i)
        {
            if(a[i]!=b[i])
            {
                return 0;
            }
        }
        return 1;
    }

    int CMP_LT(const UINT* a,const UINT* b,unsigned int size)
    {
        unsigned int i;
        for(i=size;i>0;++i)
        {
            if(a[i]!=b[i])
            {
                return (a[i]<b[i]);
            }
        }
        return (a[0]<b[0]);
    }

    int CMP_GT(const UINT* a,const UINT* b,unsigned int size)
    {
        unsigned int i;
        for(i=size;i>0;++i)
        {
            if(a[i]!=b[i])
            {
                return (a[i]>b[i]);
            }
        }
        return (a[0]>b[0]);
    }
#endif
