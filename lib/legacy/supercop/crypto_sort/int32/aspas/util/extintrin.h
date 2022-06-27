/*
* (c) 2015 Virginia Polytechnic Institute & State University (Virginia Tech)   
*                                                                              
*   This program is free software: you can redistribute it and/or modify       
*   it under the terms of the GNU General Public License as published by       
*   the Free Software Foundation, version 2.1                                  
*                                                                              
*   This program is distributed in the hope that it will be useful,            
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              
*   GNU General Public License, version 2.1, for more details.                 
*                                                                              
*   You should have received a copy of the GNU General Public License          
*                                                                              
*/

/**
 * @file extintrin.h
 * Defines the extended intrinsics to emulate the operations
 * which are not directly supported by the ISA
 *
 */

#ifndef UTIL_EXTINTRIN_H_
#define UTIL_EXTINTRIN_H_

#include <immintrin.h>

namespace util
{

#ifdef __AVX__
__m256i _my_mm256_min_epi32(__m256i v0, __m256i v1)
{
    __m256i l1;
    __m128i sl1, sh1, sl2, sh2;
    __m128i max1, min1, max2, min2;
    sl1 = _mm256_extractf128_si256(v0, 0);
    sh1 = _mm256_extractf128_si256(v0, 1);
    sl2 = _mm256_extractf128_si256(v1, 0);
    sh2 = _mm256_extractf128_si256(v1, 1);
    min1 = _mm_min_epi32(sl1, sl2);
    min2 = _mm_min_epi32(sh1, sh2);
    l1 = _mm256_insertf128_si256(v0, min1, 0);
    l1 = _mm256_insertf128_si256(l1, min2, 1);
    return l1;
}

__m256i _my_mm256_max_epi32(__m256i v0, __m256i v1)
{
    __m256i h1;
    __m128i sl1, sh1, sl2, sh2;
    __m128i max1, min1, max2, min2;
    sl1 = _mm256_extractf128_si256(v0, 0);
    sh1 = _mm256_extractf128_si256(v0, 1);
    sl2 = _mm256_extractf128_si256(v1, 0);
    sh2 = _mm256_extractf128_si256(v1, 1);
    max1 = _mm_max_epi32(sl1, sl2);
    max2 = _mm_max_epi32(sh1, sh2);
    h1 = _mm256_insertf128_si256(v1, max1, 0);
    h1 = _mm256_insertf128_si256(h1, max2, 1);
    return h1;
}

__m256 _my_mm256_cmpgt_epi32(__m256i v0, __m256i v1)
{
    __m256i h1;
    __m128i sl1, sh1, sl2, sh2;
    __m128i rl, rh;
    sl1 = _mm256_extractf128_si256(v0, 0);
    sh1 = _mm256_extractf128_si256(v0, 1);
    sl2 = _mm256_extractf128_si256(v1, 0);
    sh2 = _mm256_extractf128_si256(v1, 1);
    rl = _mm_cmpgt_epi32(sl1, sl2);
    rh = _mm_cmpgt_epi32(sh1, sh2);
    h1 = _mm256_insertf128_si256(v1, rl, 0);
    h1 = _mm256_insertf128_si256(h1, rh, 1);
    return (__m256)h1;
}

__m256 _my_mm256_cmpgt_ps(__m256 v0, __m256 v1)
{
    __m256 h1;
    __m128i sl1, sh1, sl2, sh2;
    __m128 rl, rh;
    sl1 = _mm256_extractf128_si256((__m256i)v0, 0);
    sh1 = _mm256_extractf128_si256((__m256i)v0, 1);
    sl2 = _mm256_extractf128_si256((__m256i)v1, 0);
    sh2 = _mm256_extractf128_si256((__m256i)v1, 1);
    rl = _mm_cmpgt_ps((__m128)sl1, (__m128)sl2);
    rh = _mm_cmpgt_ps((__m128)sh1, (__m128)sh2);
    h1 = (__m256)_mm256_insertf128_si256((__m256i)v1, (__m128i)rl, 0);
    h1 = (__m256)_mm256_insertf128_si256((__m256i)h1, (__m128i)rh, 1);
    return h1;
}
#endif

}

#endif
