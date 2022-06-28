#pragma once

#if defined(__MMX__)    && defined(__SSE__)       && defined(__SSE_MATH__) && \
    defined(__SSE2__)   && defined(__SSE2_MATH__) && defined(__SSE3__)     && \
    defined(__SSE4_1__) && defined(__SSE4_2__)    && defined(__POPCNT__)   && \
    defined(__PCLMUL__) && defined(__AES__)       && defined(__AVX__)      && \
    defined(__amd64__)

#define HIGH_COMPATIBILITY_X86_64
    
#include <x86intrin.h>
#include <wmmintrin.h>
#include <immintrin.h>
#include <stdalign.h>
    
#endif

#if defined(HIGH_COMPATIBILITY_X86_64)            && \
    defined(__BMI__)  && defined(__BMI2__) && \
    defined(__AVX2__) && defined(__LZCNT__)

#define HIGH_PERFORMANCE_X86_64
#include <pmmintrin.h>

#endif

/*
#if !defined(HIGH_PERFORMANCE_X86_64)
#warning AVX2 NOT enabled
#endif
*/
