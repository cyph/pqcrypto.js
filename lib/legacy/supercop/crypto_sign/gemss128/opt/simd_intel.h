#ifndef _SIMD_INTEL_H
#define _SIMD_INTEL_H

/* Intel intrinsics */

#include "arch.h"
#include "tools_gf2n.h"


#ifdef ENABLED_SSE2
    #define PXOR_(A,B) _mm_xor_si128(A,B)
    #define PAND_(A,B) _mm_and_si128(A,B)

    #define PXOR1_(C,A,B) C=PXOR_(A,B);
    #define PXOR2_(C1,C2,A1,A2,B1,B2) \
        PXOR1_(C1,A1,B1)\
        PXOR1_(C2,A2,B2)
    #define PXOR3_(C1,C2,C3,A1,A2,A3,B1,B2,B3) \
        PXOR1_(C1,A1,B1)\
        PXOR1_(C2,A2,B2)\
        PXOR1_(C3,A3,B3)
    #define PXOR4_(C1,C2,C3,C4,A1,A2,A3,A4,B1,B2,B3,B4) \
        PXOR1_(C1,A1,B1)\
        PXOR1_(C2,A2,B2)\
        PXOR1_(C3,A3,B3)\
        PXOR1_(C4,A4,B4)

    #define PXOR1_2(C,A) C=PXOR_(A,C);
    #define PXOR2_2(C1,C2,A1,A2) \
        PXOR1_2(C1,A1)\
        PXOR1_2(C2,A2)
    #define PXOR3_2(C1,C2,C3,A1,A2,A3) \
        PXOR1_2(C1,A1)\
        PXOR1_2(C2,A2)\
        PXOR1_2(C3,A3)
    #define PXOR4_2(C1,C2,C3,C4,A1,A2,A3,A4) \
        PXOR1_2(C1,A1)\
        PXOR1_2(C2,A2)\
        PXOR1_2(C3,A3)\
        PXOR1_2(C4,A4)
    #define PXOR5_2(C1,C2,C3,C4,C5,A1,A2,A3,A4,A5) \
        PXOR1_2(C1,A1)\
        PXOR1_2(C2,A2)\
        PXOR1_2(C3,A3)\
        PXOR1_2(C4,A4)\
        PXOR1_2(C5,A5)


    #define LOAD64(A) _mm_loadl_epi64((__m128i*)(A))
    #define LOADU128(A) _mm_loadu_si128((__m128i*)(A))
    #define PLOAD(A) _mm_load_si128((__m128i*)(A))

    #define STORE64(res,A) _mm_storel_epi64((__m128i*)(res),A)
    #define STOREU128(res,A) _mm_storeu_si128((__m128i*)(res),A);
    #define PSTORE(res,A) _mm_store_si128((__m128i*)(res),A);

    /* TODO: this part should be move in tools_gf2n 
             (cf. macro SIZE_ALIGNED_GFqn) */
    /*XXX TODO: set this option to 16 or 32 is not stable XXX */
    #define IS_ALIGNED 0

    #if IS_ALIGNED
        #define LOAD128 PLOAD
        #define STORE128 PSTORE
    #else
        #define LOAD128 LOADU128
        #define STORE128 STOREU128
    #endif

    #define INIT64(x,A) x=LOAD64(A);
    #define INIT128(x,A) x=LOAD128(A);

    #define INIT192(x1,x2,A) \
        INIT128(x1,A); \
        INIT64(x2,A+2);

    #define INIT256(x1,x2,A) \
        INIT128(x1,A); \
        INIT128(x2,A+2);

    #define INIT320(x1,x2,x3,A) \
        INIT256(x1,x2,A); \
        INIT64(x3,A+4);

    #define INIT384(x1,x2,x3,A) \
        INIT256(x1,x2,A); \
        INIT128(x3,A+4);

    #define INIT448(x1,x2,x3,x4,A) \
        INIT256(x1,x2,A); \
        INIT192(x3,x4,A+4);

    #define INIT512(x1,x2,x3,x4,A) \
        INIT256(x1,x2,A); \
        INIT256(x3,x4,A+4);

    #define INIT576(x1,x2,x3,x4,x5,A) \
        INIT512(x1,x2,x3,x4,A); \
        INIT64(x5,A+8);



    #define STORE192(A,x1,x2) \
        STORE128(A,x1); \
        STORE64(A+2,x2);

    #define STORE2x128(A,x1,x2) \
        STORE128(A,x1); \
        STORE128(A+2,x2);

    #define STORE320(A,x1,x2,x3) \
        STORE2x128(A,x1,x2); \
        STORE64(A+4,x3);

    #define STORE384(A,x1,x2,x3) \
        STORE2x128(A,x1,x2); \
        STORE128(A+4,x3);

    #define STORE448(A,x1,x2,x3,x4) \
        STORE2x128(A,x1,x2); \
        STORE192(A+4,x3,x4);

    #define STORE512(A,x1,x2,x3,x4) \
        STORE2x128(A,x1,x2); \
        STORE2x128(A+4,x3,x4);

    #define STORE576(A,x1,x2,x3,x4,x5) \
        STORE512(A,x1,x2,x3,x4);\
        STORE64(A+8,x5);

    #define STORE640(A,x1,x2,x3,x4,x5) \
        STORE512(A,x1,x2,x3,x4);\
        STORE128(A+8,x5);

    #define STORE768(A,x1,x2,x3,x4,x5,x6) \
        STORE512(A,x1,x2,x3,x4);\
        STORE2x128(A+8,x5,x6);

    #define STORE896(A,x1,x2,x3,x4,x5,x6,x7) \
        STORE512(A,x1,x2,x3,x4);\
        STORE384(A+8,x5,x6,x7);

    #define STORE1024(A,x1,x2,x3,x4,x5,x6,x7,x8) \
        STORE512(A,x1,x2,x3,x4);\
        STORE512(A+8,x5,x6,x7,x8);

    #define STORE1152(A,x1,x2,x3,x4,x5,x6,x7,x8,x9) \
        STORE1024(A,x1,x2,x3,x4,x5,x6,x7,x8);\
        STORE64(A+16,x9);


    #define INIT128_ZERO(x) x=_mm_setzero_si128();
    #define INIT128_BROADCAST64(x,A) x=_mm_set1_epi64x(A);


    /* It is logic to use shift for the next operations, but maybe other intrinsics are faster */

    /* Right shift without 0 padding */
    #define RIGHT_MOVE64(x) _mm_srli_si128(x,8)
    /*#define RIGHT_MOVE64(x) _mm_shuffle_epi32(x,78)*/

    /* Right shift with 0 padding */
    #define RIGHT_SHIFT64(x) _mm_srli_si128(x,8)

    /* Left shift with 0 padding */
    #define LEFT_SHIFT64(x) _mm_slli_si128(x,8)

    #ifdef ENABLED_SSSE3
        #define MIDDLE128(A1,A2) _mm_alignr_epi8(A2,A1,8)
        #define PALIGNR(A2,A1,NB_BYTES) _mm_alignr_epi8(A2,A1,NB_BYTES)
    #else
        #define MIDDLE128(A1,A2) _mm_unpacklo_epi64(RIGHT_MOVE64(A1),A2)
        #define PALIGNR(A2,A1,NB_BYTES) \
            _mm_xor_si128(_mm_srli_si128(A1,NB_BYTES),_mm_slli_si128(A2,16-NB_BYTES))
    #endif

    #define XOR128(a,b) _mm_xor_si128(a,b)
#endif


#ifdef ENABLED_SSE4_1
    #define EXTRACT64(A) _mm_extract_epi64(A,1)
#elif defined(ENABLED_SSE2)
    #define EXTRACT64(A) _mm_cvtsi128_si64x(RIGHT_MOVE64(A))
#endif


#ifdef ENABLED_AVX
    #define LOADU256(A) _mm256_loadu_si256((__m256i*)(A))
    #define INITU256(res,A) res=LOADU256(A);
    #define STOREU256(res,A) _mm256_storeu_si256((__m256i*)(res),A);

    #if (IS_ALIGNED==32)
        #define LOAD256(A) _mm256_load_si256((__m256i*)(A))
        #define STORE256(res,A) _mm256_store_si256((__m256i*)(res),A);
    #else
        #define LOAD256 LOADU256
        #define STORE256 STOREU256
    #endif
    #define INIT_R256(x,A) x=LOAD256(A);
#endif


#ifdef ENABLED_AVX2
    #define VPXOR_(A,B) _mm256_xor_si256(A,B)
    #define VPAND_(A,B) _mm256_and_si256(A,B)

    #define PMASKLOAD(data,mask) \
        _mm_maskload_epi64((long long int*)(data),mask)
    #define VPMASKLOAD(data,mask) \
        _mm256_maskload_epi64((long long int*)(data),mask)

    #define PERMUTE4x64(A,i) \
        _mm256_permute4x64_epi64(A,i)

    #define VPEXTRACT128(A) _mm256_extracti128_si256(A,1)
#elif defined(ENABLED_AVX)
    #define VPXOR_(A,B) ((__m256i)_mm256_xor_pd((__m256d)A,(__m256d)B))
    #define VPAND_(A,B) ((__m256i)_mm256_and_pd((__m256d)A,(__m256d)B))

    #define PMASKLOAD(data,mask) \
        ((__m128i)_mm_maskload_pd((double*)(data),mask))
    #define VPMASKLOAD(data,mask) \
        ((__m256i)_mm256_maskload_pd((double*)(data),mask))

    #define VPEXTRACT128(A) ((__m128i)_mm256_extractf128_pd((__m256d)A,1))
#endif


#if (defined(ENABLED_AVX)||defined(ENABLED_AVX2))
    #define VPXOR1_(C,A,B) C=VPXOR_(A,B);
    #define VPXOR2_(C1,C2,A1,A2,B1,B2) \
        VPXOR1_(C1,A1,B1)\
        VPXOR1_(C2,A2,B2)
    #define VPXOR3_(C1,C2,C3,A1,A2,A3,B1,B2,B3) \
        VPXOR1_(C1,A1,B1)\
        VPXOR1_(C2,A2,B2)\
        VPXOR1_(C3,A3,B3)
    #define VPXOR4_(C1,C2,C3,C4,A1,A2,A3,A4,B1,B2,B3,B4) \
        VPXOR1_(C1,A1,B1)\
        VPXOR1_(C2,A2,B2)\
        VPXOR1_(C3,A3,B3)\
        VPXOR1_(C4,A4,B4)

    #define VPXOR1_2(C,A) C=VPXOR_(A,C);
    #define VPXOR2_2(C1,C2,A1,A2) \
        VPXOR1_2(C1,A1)\
        VPXOR1_2(C2,A2)
    #define VPXOR3_2(C1,C2,C3,A1,A2,A3) \
        VPXOR1_2(C1,A1)\
        VPXOR1_2(C2,A2)\
        VPXOR1_2(C3,A3)
    #define VPXOR4_2(C1,C2,C3,C4,A1,A2,A3,A4) \
        VPXOR1_2(C1,A1)\
        VPXOR1_2(C2,A2)\
        VPXOR1_2(C3,A3)\
        VPXOR1_2(C4,A4)
#endif


#endif
