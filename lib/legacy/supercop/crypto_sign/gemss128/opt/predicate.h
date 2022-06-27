#ifndef _PREDICATE_H
#define _PREDICATE_H

#include "prefix_name.h"
#include "arch.h"


int PREFIX_NAME(ISZERO)(const UINT* a,unsigned int size);
int PREFIX_NAME(ISONE)(const UINT* a,unsigned int size);
int PREFIX_NAME(ISEQUAL)(const UINT* a, const UINT* b,unsigned int size);
int PREFIX_NAME(CMP_LT)(const UINT* a,const UINT* b,unsigned int size);
int PREFIX_NAME(CMP_GT)(const UINT* a,const UINT* b,unsigned int size);


#define f_ISZERO PREFIX_NAME(ISZERO)
#define f_ISONE PREFIX_NAME(ISONE)
#define f_ISEQUAL PREFIX_NAME(ISEQUAL)
#define f_CMP_LT PREFIX_NAME(CMP_LT)
#define f_CMP_GT PREFIX_NAME(CMP_GT)


#if (NB_BITS_UINT!=64U)
    #define ISEQUAL256(a,b) f_ISEQUAL(a,b,256/NB_BITS_UINT)
    #define ISEQUAL384(a,b) f_ISEQUAL(a,b,384/NB_BITS_UINT)
    #define ISEQUAL512(a,b) f_ISEQUAL(a,b,512/NB_BITS_UINT)
#else



/* Equal 0 */
#define ISZERO64(a) ((a)[0]==0UL)
#define ISZERO128(a) (ISZERO64(a)&&((a)[1]==0UL))
#define ISZERO192(a) (ISZERO128(a)&&((a)[2]==0UL))
#define ISZERO256(a) (ISZERO192(a)&&((a)[3]==0UL))
#define ISZERO320(a) (ISZERO256(a)&&((a)[4]==0UL))
#define ISZERO384(a) (ISZERO320(a)&&((a)[5]==0UL))



/* Equal 1 */
#define ISONE64(a) ((a)[0]==1UL)
#define ISONE128(a) (ISONE64(a)&&((a)[1]==0UL))
#define ISONE192(a) (ISONE128(a)&&((a)[2]==0UL))
#define ISONE256(a) (ISONE192(a)&&((a)[3]==0UL))
#define ISONE320(a) (ISONE256(a)&&((a)[4]==0UL))
#define ISONE384(a) (ISONE320(a)&&((a)[5]==0UL))



/* Equality */
#define ISEQUAL64(a,b) ((a)[0]==(b)[0])
#define ISEQUAL128(a,b) (ISEQUAL64(a,b)&&((a)[1]==(b)[1]))
#define ISEQUAL192(a,b) (ISEQUAL128(a,b)&&((a)[2]==(b)[2]))
#define ISEQUAL256(a,b) (ISEQUAL192(a,b)&&((a)[3]==(b)[3]))
#define ISEQUAL320(a,b) (ISEQUAL256(a,b)&&((a)[4]==(b)[4]))
#define ISEQUAL384(a,b) (ISEQUAL320(a,b)&&((a)[5]==(b)[5]))
#define ISEQUAL448(a,b) (ISEQUAL384(a,b)&&((a)[6]==(b)[6]))
#define ISEQUAL512(a,b) (ISEQUAL448(a,b)&&((a)[7]==(b)[7]))



/* Comparison */
#define CMP_LT64(a,b) ((a)[0]<(b)[0])
#define CMP_LT128(a,b) (((a)[1]==(b)[1])?CMP_LT64(a,b):((a)[1]<(b)[1]))
#define CMP_LT192(a,b) (((a)[2]==(b)[2])?CMP_LT128(a,b):((a)[2]<(b)[2]))
#define CMP_LT256(a,b) (((a)[3]==(b)[3])?CMP_LT192(a,b):((a)[3]<(b)[3]))
#define CMP_LT320(a,b) (((a)[4]==(b)[4])?CMP_LT256(a,b):((a)[4]<(b)[4]))
#define CMP_LT384(a,b) (((a)[5]==(b)[5])?CMP_LT320(a,b):((a)[5]<(b)[5]))

#define CMP_GT64(a,b) ((a)[0]>(b)[0])
#define CMP_GT128(a,b) (((a)[1]==(b)[1])?CMP_GT64(a,b):((a)[1]>(b)[1]))
#define CMP_GT192(a,b) (((a)[2]==(b)[2])?CMP_GT128(a,b):((a)[2]>(b)[2]))
#define CMP_GT256(a,b) (((a)[3]==(b)[3])?CMP_GT192(a,b):((a)[3]>(b)[3]))
#define CMP_GT320(a,b) (((a)[4]==(b)[4])?CMP_GT256(a,b):((a)[4]>(b)[4]))
#define CMP_GT384(a,b) (((a)[5]==(b)[5])?CMP_GT320(a,b):((a)[5]>(b)[5]))



#endif



#endif
