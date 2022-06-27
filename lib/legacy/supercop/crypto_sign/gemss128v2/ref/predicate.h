#ifndef _PREDICATE_H
#define _PREDICATE_H

#include "prefix_name.h"
#include "arch.h"


/* Compare two 64-bit words in constant-time */
#define CMP_LT_U64(a,b) \
            (((((a)>>63U)^((b)>>63U))    &((((a)>>63U)-((b)>>63U))>>63U))\
        ^    ((((a)>>63U)^((b)>>63U)^ONE64)\
              &((((a)&((uint64_t)0x7FFFFFFFFFFFFFFF))\
                -((b)&((uint64_t)0x7FFFFFFFFFFFFFFF)))>>63U)))

/* Compare two 32-bit words in constant-time */
#define CMP_LT_U32(a,b) \
            (((((a)>>31U)^((b)>>31U))    &((((a)>>31U)-((b)>>31U))>>31U))\
        ^    ((((a)>>31U)^((b)>>31U)^ONE32)\
              &((((a)&((uint32_t)0x7FFFFFFF))\
                -((b)&((uint32_t)0x7FFFFFFF)))>>31U)))

/* Compare two UINT in constant-time */
#define CMP_LT_UINT CONCAT(CMP_LT_U,NB_BITS_UINT)


/* Constant-time version */
int PREFIX_NAME(ISZERO)(const UINT* a, unsigned int size);
int PREFIX_NAME(ISNOTZERO)(const UINT* a, unsigned int size);
int PREFIX_NAME(ISONE)(const UINT* a, unsigned int size);
int PREFIX_NAME(ISEQUAL)(const UINT* a, const UINT* b, unsigned int size);
int PREFIX_NAME(CMP_LT)(const UINT* a, const UINT* b, unsigned int size);


#define f_ISZERO PREFIX_NAME(ISZERO)
#define f_ISNOTZERO PREFIX_NAME(ISNOTZERO)
#define f_ISONE PREFIX_NAME(ISONE)
#define f_ISEQUAL PREFIX_NAME(ISEQUAL)
#define f_CMP_LT PREFIX_NAME(CMP_LT)
#define f_CMP_GT(a,b,size) f_CMP_LT(b,a,size)


/* Variable-time version */
int PREFIX_NAME(ISZERO_NOCST)(const UINT* a, unsigned int size);
int PREFIX_NAME(ISONE_NOCST)(const UINT* a, unsigned int size);
int PREFIX_NAME(ISEQUAL_NOCST)(const UINT* a, const UINT* b, unsigned int size);
int PREFIX_NAME(CMP_LT_NOCST)(const UINT* a, const UINT* b, unsigned int size);


#define f_ISZERO_NOCST PREFIX_NAME(ISZERO_NOCST)
#define f_ISONE_NOCST PREFIX_NAME(ISONE_NOCST)
#define f_ISEQUAL_NOCST PREFIX_NAME(ISEQUAL_NOCST)
#define f_CMP_LT_NOCST PREFIX_NAME(CMP_LT_NOCST)
#define f_CMP_GT_NOCST(a,b,size) f_CMP_LT_NOCST(b,a,size)


/* Inlined version */
/* Equal to 0 */
#define ISZERO1_NOCST(a) ((a)[0]==0)
#define ISZERO2_NOCST(a) (ISZERO1_NOCST(a)&&((a)[1]==0))
#define ISZERO3_NOCST(a) (ISZERO2_NOCST(a)&&((a)[2]==0))
#define ISZERO4_NOCST(a) (ISZERO3_NOCST(a)&&((a)[3]==0))
#define ISZERO5_NOCST(a) (ISZERO4_NOCST(a)&&((a)[4]==0))
#define ISZERO6_NOCST(a) (ISZERO5_NOCST(a)&&((a)[5]==0))
#define ISZERO7_NOCST(a) (ISZERO6_NOCST(a)&&((a)[6]==0))
#define ISZERO8_NOCST(a) (ISZERO7_NOCST(a)&&((a)[7]==0))
#define ISZERO9_NOCST(a) (ISZERO8_NOCST(a)&&((a)[8]==0))


/* Equal to 1 */
#define ISONE1_NOCST(a) ((a)[0]==1)
#define ISONE2_NOCST(a) (ISONE1_NOCST(a)&&((a)[1]==0))
#define ISONE3_NOCST(a) (ISONE2_NOCST(a)&&((a)[2]==0))
#define ISONE4_NOCST(a) (ISONE3_NOCST(a)&&((a)[3]==0))
#define ISONE5_NOCST(a) (ISONE4_NOCST(a)&&((a)[4]==0))
#define ISONE6_NOCST(a) (ISONE5_NOCST(a)&&((a)[5]==0))
#define ISONE7_NOCST(a) (ISONE6_NOCST(a)&&((a)[6]==0))
#define ISONE8_NOCST(a) (ISONE7_NOCST(a)&&((a)[7]==0))
#define ISONE9_NOCST(a) (ISONE8_NOCST(a)&&((a)[8]==0))


/* Equality */
#define ISEQUAL1_NOCST(a,b) ((a)[0]==(b)[0])
#define ISEQUAL2_NOCST(a,b) (ISEQUAL1_NOCST(a,b)&&((a)[1]==(b)[1]))
#define ISEQUAL3_NOCST(a,b) (ISEQUAL2_NOCST(a,b)&&((a)[2]==(b)[2]))
#define ISEQUAL4_NOCST(a,b) (ISEQUAL3_NOCST(a,b)&&((a)[3]==(b)[3]))
#define ISEQUAL5_NOCST(a,b) (ISEQUAL4_NOCST(a,b)&&((a)[4]==(b)[4]))
#define ISEQUAL6_NOCST(a,b) (ISEQUAL5_NOCST(a,b)&&((a)[5]==(b)[5]))
#define ISEQUAL7_NOCST(a,b) (ISEQUAL6_NOCST(a,b)&&((a)[6]==(b)[6]))
#define ISEQUAL8_NOCST(a,b) (ISEQUAL7_NOCST(a,b)&&((a)[7]==(b)[7]))
#define ISEQUAL9_NOCST(a,b) (ISEQUAL8_NOCST(a,b)&&((a)[8]==(b)[8]))


/* Comparison, less than */
#define CMP_LT1_NOCST(a,b) ((a)[0]<(b)[0])
#define CMP_LT2_NOCST(a,b) (((a)[1]==(b)[1])?CMP_LT1_NOCST(a,b)\
                                              :((a)[1]<(b)[1]))
#define CMP_LT3_NOCST(a,b) (((a)[2]==(b)[2])?CMP_LT2_NOCST(a,b)\
                                              :((a)[2]<(b)[2]))
#define CMP_LT4_NOCST(a,b) (((a)[3]==(b)[3])?CMP_LT3_NOCST(a,b)\
                                              :((a)[3]<(b)[3]))
#define CMP_LT5_NOCST(a,b) (((a)[4]==(b)[4])?CMP_LT4_NOCST(a,b)\
                                              :((a)[4]<(b)[4]))
#define CMP_LT6_NOCST(a,b) (((a)[5]==(b)[5])?CMP_LT5_NOCST(a,b)\
                                              :((a)[5]<(b)[5]))
#define CMP_LT7_NOCST(a,b) (((a)[6]==(b)[6])?CMP_LT6_NOCST(a,b)\
                                              :((a)[6]<(b)[6]))
#define CMP_LT8_NOCST(a,b) (((a)[7]==(b)[7])?CMP_LT7_NOCST(a,b)\
                                              :((a)[7]<(b)[7]))
#define CMP_LT9_NOCST(a,b) (((a)[8]==(b)[8])?CMP_LT8_NOCST(a,b)\
                                              :((a)[8]<(b)[8]))


#endif

