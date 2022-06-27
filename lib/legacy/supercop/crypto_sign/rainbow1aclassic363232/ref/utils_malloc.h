/// @file utils_malloc.h
/// @brief the interface for adapting malloc functions.
///
///

#ifndef _UTILS_MALLOC_H_
#define _UTILS_MALLOC_H_


#include <stdlib.h>


#if !defined( _ALIGN_ )
#if defined(__GNUC__) || defined(__clang__)
#define _ALIGN_(x)  __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define _ALIGN_(x)  __declspec(align(x))
#else
#define _ALIGN_(x)
#endif
#endif



//#define _HAS_ALIGNED_ALLOC_


#ifdef  __cplusplus
extern  "C" {
#endif


static inline
void * adapted_alloc( size_t alignment, size_t size )
{
#if defined(_HAS_ALIGNED_ALLOC_)
  return aligned_alloc( alignment, size );
#else
  (void)(alignment);
  return malloc( size );
#endif
}



#ifdef  __cplusplus
}
#endif



#endif // _UTILS_MALLOC_H_


