#include "predicate.h"



/**
 * @brief   Test if a buffer is the multi-precision integer zero.
 * @param[in]   a  A buffer of UINT.
 * @param[in]   size    The length in UINT of a.
 * @return  One if the buffer is the multi-precision integer zero, else zero.
 */
int PREFIX_NAME(ISZERO)(const UINT* a,unsigned int size)
{
    unsigned int i;
    for(i=0;i<size;++i)
    {
        if(a[i])
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief   Test if a buffer is the multi-precision integer one.
 * @param[in]   a  A buffer of UINT.
 * @param[in]   size    The length in UINT of a.
 * @return  One if the buffer is the multi-precision integer one, else zero.
 */
int PREFIX_NAME(ISONE)(const UINT* a,unsigned int size)
{
    unsigned int i;
    if(a[0]!=1UL)
    {
        return 0;
    }
    for(i=1;i<size;++i)
    {
        if(a[i])
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief   Test if two buffers are equal.
 * @param[in]   a  A buffer of UINT.
 * @param[in]   b  A buffer of UINT.
 * @param[in]   size    The length in UINT of a and b.
 * @return  One if the buffers are equal, else zero.
 */
int PREFIX_NAME(ISEQUAL)(const UINT* a, const UINT* b, unsigned int size)
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

/**
 * @brief   Test if a multi-precision integer is strictly less than an other.
 * @param[in]   a  A buffer of UINT.
 * @param[in]   b  A buffer of UINT.
 * @param[in]   size    The length in UINT of a and b.
 * @return  Different of zero if a<b, else zero.
 */
int PREFIX_NAME(CMP_LT)(const UINT* a, const UINT* b, unsigned int size)
{
    unsigned int i;
    for(i=size-1;i>0;--i)
    {
        if(a[i]!=b[i])
        {
            return (a[i]<b[i]);
        }
    }
    return (a[0]<b[0]);
}

/**
 * @brief   Test if a multi-precision integer is strictly greater than an other.
 * @param[in]   a  A buffer of UINT.
 * @param[in]   b  A buffer of UINT.
 * @param[in]   size    The length in UINT of a and b.
 * @return  Different of zero if a>b, else zero.
 */
int PREFIX_NAME(CMP_GT)(const UINT* a,const UINT* b,unsigned int size)
{
    unsigned int i;
    for(i=size-1;i>0;--i)
    {
        if(a[i]!=b[i])
        {
            return (a[i]>b[i]);
        }
    }
    return (a[0]>b[0]);
}
