#ifndef _EXTRACT_H
#define _EXTRACT_H


/* set to 1 is optimal */
#define STORE 1

#ifdef __SSE4_1__

    #include <smmintrin.h>
    #define EXTRACT64(A,i) _mm_extract_epi64(A,i)

    #if STORE
        #define EXTRACT128(res,A) _mm_storeu_si128((__m128i*)(res),A);
    #else
        #define EXTRACT128(res,A) \
            (res)[0]=EXTRACT64(A,0);\
            (res)[1]=EXTRACT64(A,1);
    #endif


#endif

#endif
