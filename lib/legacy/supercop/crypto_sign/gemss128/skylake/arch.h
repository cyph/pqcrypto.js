#ifndef _ARCH_H
#define _ARCH_H

#include <stdint.h>

/* For the moment, these two parameters cannot be modified */
/** Type of an UINT. */
typedef uint64_t UINT;
/** Number of bits of an UINT. */
#define NB_BITS_UINT 64U

/** Print an UINT. */
#define PRINT_UINT(a) printf("0x%lx",a);


/** A reserved variable to do a for loop on a buffer of UINT. */
#define RESERVED_VARIABLE reserved_variable

#define FOR_LOOP(OP,SIZE) \
    {unsigned int RESERVED_VARIABLE; \
        for(RESERVED_VARIABLE=0U;RESERVED_VARIABLE<(SIZE);++RESERVED_VARIABLE) \
        { \
            OP;\
        } \
    }

#define FOR_LOOP_COMPLETE(INIT,CMP,INC,OP) \
    {unsigned int RESERVED_VARIABLE; \
        for(RESERVED_VARIABLE=INIT;CMP;INC) \
        { \
            OP;\
        } \
    }



/********************** Security **********************/

/** To enable a constant-time implementation (when it is possible) */
#define CONSTANT_TIME 1


/****************** C++ compatibility ******************/

/** Compatibility with C++. */
#ifdef __cplusplus
    #define BEGIN_EXTERNC extern "C" {
    #define END_EXTERNC }
#else
    #define BEGIN_EXTERNC
    #define END_EXTERNC
#endif


/****************** To choose vectorial optimizations ******************/

/* Choice of the processor */
#define INTEL_PROCESSOR 1

/* To choose the best algorithm in function of the processor */
#define PROC_HASWELL 0
#define PROC_SKYLAKE 1


#ifdef __SSE__
    #include <xmmintrin.h>
    /** To use sse. */
    #define ENABLED_SSE
#endif

#ifdef __SSE2__
    #include <emmintrin.h>
    /** To use sse2. */
    #define ENABLED_SSE2
#endif

#ifdef __SSSE3__
    #include <tmmintrin.h>
    /** To use ssse3. */
    #define ENABLED_SSSE3
#endif

#ifdef __SSE4_1__
    #include <smmintrin.h>
    /** To use sse4.1. */
    #define ENABLED_SSE4_1
#endif

#ifdef __AVX__
    #include <immintrin.h>
    /** To use avx. */
    #define ENABLED_AVX
#endif

#ifdef __AVX2__
    /** To use avx2. */
    #define ENABLED_AVX2
#endif

#if (defined(__PCLMUL__) && defined(ENABLED_SSE2))
    #include <wmmintrin.h>
    /** To use multiplication in binary field with PCLMULQDQ and sse2. */
    #define ENABLED_PCLMUL
#endif

#if (defined(ENABLED_PCLMUL) && defined(ENABLED_AVX2))
    #include <wmmintrin.h>
    /** To use multiplication in binary field with PCLMULQDQ and avx2. */
    #define ENABLED_PCLMUL_AVX2
#endif


#ifdef __POPCNT__
    #include <nmmintrin.h>
    /** Improve the computation of the number of bits set to 1 in a 64-bit 
     *  integer. */
    #define ENABLED_POPCNT
#endif


/****************** To use multiplication of gf2x library ******************/

/** Set to 1 to use the multiplication in GF(2)[x] of the gf2x library. 
 *  Set to 0 to use a new implementation using the PCLMULQDQ instruction. */
#ifdef ENABLED_PCLMUL
    /* The multiplication uses PCLMULQDQ instruction */
    /* In this case, the installation of gf2x is not required */
    #define ENABLED_GF2X 0
#else
    /* The multiplication uses the gf2x library */
    #define ENABLED_GF2X 1
#endif


/****************** Memory alignment ******************/

#include <string.h>


/** Free an aligned memory. */
#ifdef ENABLED_SSE
    #define ALIGNED_FREE(p) _mm_free(p);
#else
    #define ALIGNED_FREE(p) free(p);
#endif


/** Align the data on 16 bytes, useful for sse2. */
#define ALIGNED16 __attribute__((aligned(16)))

#ifdef ENABLED_SSE
    #define ALIGNED16_MALLOC(p,type,nmemb,size) \
                p=(type)_mm_malloc(nmemb*size,16);
#else
    #define ALIGNED16_MALLOC(p,type,nmemb,size) \
                if(posix_memalign((void**)(&p),16,nmemb*size))\
                {\
                    exit(1);\
                }
#endif

#define ALIGNED16_CALLOC(p,type,nmemb,size) \
            ALIGNED16_MALLOC(p,type,nmemb,size);\
            memset((void*)p,0,nmemb*size)


/** Align the data on 32 bytes, useful for avx. */
#define ALIGNED32 __attribute__((aligned(32)))

#ifdef ENABLED_SSE
    #define ALIGNED32_MALLOC(p,type,nmemb,size) \
                p=(type)_mm_malloc(nmemb*size,32);
#else
    #define ALIGNED32_MALLOC(p,type,nmemb,size) \
                if(posix_memalign((void**)(&p),32,nmemb*size))\
                {\
                    exit(1);\
                }
#endif

#define ALIGNED32_CALLOC(p,type,nmemb,size) \
            ALIGNED32_MALLOC(p,type,nmemb,size);\
            memset((void*)p,0,nmemb*size)


#define NO_ALIGNED_MALLOC(p,type,nmemb,size) p=(type)malloc(nmemb*size);
#define NO_ALIGNED_CALLOC(p,type,nmemb,size) p=(type)calloc(nmemb,size);
/** Free a no aligned memory. */
#define NO_ALIGNED_FREE(p) free(p);



#endif
