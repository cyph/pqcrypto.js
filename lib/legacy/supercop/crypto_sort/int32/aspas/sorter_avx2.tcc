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
 * @file sorter_avx.tcc
 * Definition of sorting functions in the segment-by-segment style
 * This file uses AVX instruction sets.
 *
 */

#include <immintrin.h> 
#include <type_traits> 

#include "util/extintrin.h"

namespace aspas
{

namespace internal
{

/**
 * This method compares two values from index i and j.
 * If value at i is larger than j, do the swap.
 *
 * @param a data array
 * @param i first index 
 * @param j second index 
 * @return the values in i and j are sorted
 *
 */
template <typename T>
void swap(T* a, uint32_t i, uint32_t j) 
{
    if(a[i] > a[j]) 
    {
        T tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

template <typename T>
void swap_key(T* a, int *ptr, uint32_t i, uint32_t j) 
{
    if(a[i] > a[j]) 
    {
        T tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;

        int tmp_ptr = ptr[i];
        ptr[i] = ptr[j];
        ptr[j] = tmp_ptr;
    }
}

template <typename T>
void swap_key(T* a, long *ptr, uint32_t i, uint32_t j) 
{
    if(a[i] > a[j]) 
    {
        T tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;

        long tmp_ptr = ptr[i];
        ptr[i] = ptr[j];
        ptr[j] = tmp_ptr;
    }
}

/**
 * Float vector version (__m256):
 * This method performs the in-register sort. The sorted elements 
 * are stored vertically accross the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored vertically among the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_sort(T& v0, T& v1, T& v2, T& v3, 
                 T& v4, T& v5, T& v6, T& v7)
{
    __m256 l, h;

    /** odd-even sorting network */ 
    /** step 1 */
    l = _mm256_min_ps(v0, v1);
    h = _mm256_max_ps(v0, v1); v0 = l; v1 = h;
    l = _mm256_min_ps(v2, v3);
    h = _mm256_max_ps(v2, v3); v2 = l; v3 = h;
    l = _mm256_min_ps(v4, v5);
    h = _mm256_max_ps(v4, v5); v4 = l; v5 = h;
    l = _mm256_min_ps(v6, v7);
    h = _mm256_max_ps(v6, v7); v6 = l; v7 = h;
    /** step 2 */
    l = _mm256_min_ps(v0, v2);
    h = _mm256_max_ps(v0, v2); v0 = l; v2 = h;
    l = _mm256_min_ps(v1, v3);         
    h = _mm256_max_ps(v1, v3); v1 = l; v3 = h;
    l = _mm256_min_ps(v4, v6);         
    h = _mm256_max_ps(v4, v6); v4 = l; v6 = h;
    l = _mm256_min_ps(v5, v7);         
    h = _mm256_max_ps(v5, v7); v5 = l; v7 = h;
    /** step 3 */
    l = _mm256_min_ps(v1, v2);
    h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    l = _mm256_min_ps(v5, v6);
    h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
    /** step 4 */
    l = _mm256_min_ps(v0, v4);
    h = _mm256_max_ps(v0, v4); v0 = l; v4 = h;
    l = _mm256_min_ps(v1, v5);
    h = _mm256_max_ps(v1, v5); v1 = l; v5 = h;
    l = _mm256_min_ps(v2, v6);
    h = _mm256_max_ps(v2, v6); v2 = l; v6 = h;
    l = _mm256_min_ps(v3, v7);
    h = _mm256_max_ps(v3, v7); v3 = l; v7 = h;
    /** step 5 */
    l = _mm256_min_ps(v2, v4);
    h = _mm256_max_ps(v2, v4); v2 = l; v4 = h;
    l = _mm256_min_ps(v3, v5);
    h = _mm256_max_ps(v3, v5); v3 = l; v5 = h;
    /** step 6 */
    l = _mm256_min_ps(v1, v2);
    h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    l = _mm256_min_ps(v3, v4);
    h = _mm256_max_ps(v3, v4); v3 = l; v4 = h;
    l = _mm256_min_ps(v5, v6);
    h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_sort_key(T& v0, T& v1, T& v2, T& v3, 
                 T& v4, T& v5, T& v6, T& v7,
        __m256i &vp0, __m256i &vp1, __m256i &vp2, __m256i &vp3, 
        __m256i &vp4, __m256i &vp5, __m256i &vp6, __m256i &vp7)
{
    __m256 l, h;
    __m256i pl, ph;
    __m256 m0;

    /** odd-even sorting network */ 
    /** step 1 */
    // l = _mm256_min_ps(v0, v1);
    // h = _mm256_max_ps(v0, v1); v0 = l; v1 = h;
    m0 = _mm256_cmp_ps(v0, v1, _CMP_GT_OS);
    l = _mm256_blendv_ps(v0, v1, m0);
    h = _mm256_blendv_ps(v1, v0, m0); v0 = l; v1 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp1, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp0, m0); vp0 = pl; vp1 = ph;
    
    // l = _mm256_min_ps(v2, v3);
    // h = _mm256_max_ps(v2, v3); v2 = l; v3 = h;
    m0 = _mm256_cmp_ps(v2, v3, _CMP_GT_OS);
    l = _mm256_blendv_ps(v2, v3, m0);
    h = _mm256_blendv_ps(v3, v2, m0); v2 = l; v3 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp3, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp2, m0); vp2 = pl; vp3 = ph;

    // l = _mm256_min_ps(v4, v5);
    // h = _mm256_max_ps(v4, v5); v4 = l; v5 = h;
    m0 = _mm256_cmp_ps(v4, v5, _CMP_GT_OS);
    l = _mm256_blendv_ps(v4, v5, m0);
    h = _mm256_blendv_ps(v5, v4, m0); v4 = l; v5 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp5, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp4, m0); vp4 = pl; vp5 = ph;

    // l = _mm256_min_ps(v6, v7);
    // h = _mm256_max_ps(v6, v7); v6 = l; v7 = h;
    m0 = _mm256_cmp_ps(v6, v7, _CMP_GT_OS);
    l = _mm256_blendv_ps(v6, v7, m0);
    h = _mm256_blendv_ps(v7, v6, m0); v6 = l; v7 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp7, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp6, m0); vp6 = pl; vp7 = ph;

    /** step 2 */
    // l = _mm256_min_ps(v0, v2);
    // h = _mm256_max_ps(v0, v2); v0 = l; v2 = h;
    m0 = _mm256_cmp_ps(v0, v2, _CMP_GT_OS);
    l = _mm256_blendv_ps(v0, v2, m0);
    h = _mm256_blendv_ps(v2, v0, m0); v0 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp2, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp0, m0); vp0 = pl; vp2 = ph;

    // l = _mm256_min_ps(v1, v3);         
    // h = _mm256_max_ps(v1, v3); v1 = l; v3 = h;
    m0 = _mm256_cmp_ps(v1, v3, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v3, m0);
    h = _mm256_blendv_ps(v3, v1, m0); v1 = l; v3 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp3, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp1, m0); vp1 = pl; vp3 = ph;

    // l = _mm256_min_ps(v4, v6);         
    // h = _mm256_max_ps(v4, v6); v4 = l; v6 = h;
    m0 = _mm256_cmp_ps(v4, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v4, v6, m0);
    h = _mm256_blendv_ps(v6, v4, m0); v4 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp4, m0); vp4 = pl; vp6 = ph;

    // l = _mm256_min_ps(v5, v7);         
    // h = _mm256_max_ps(v5, v7); v5 = l; v7 = h;
    m0 = _mm256_cmp_ps(v5, v7, _CMP_GT_OS);
    l = _mm256_blendv_ps(v5, v7, m0);
    h = _mm256_blendv_ps(v7, v5, m0); v5 = l; v7 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp7, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp5, m0); vp5 = pl; vp7 = ph;

    /** step 3 */
    // l = _mm256_min_ps(v1, v2);
    // h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    m0 = _mm256_cmp_ps(v1, v2, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v2, m0);
    h = _mm256_blendv_ps(v2, v1, m0); v1 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;

    // l = _mm256_min_ps(v5, v6);
    // h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
    m0 = _mm256_cmp_ps(v5, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v5, v6, m0);
    h = _mm256_blendv_ps(v6, v5, m0); v5 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;

    /** step 4 */
    // l = _mm256_min_ps(v0, v4);
    // h = _mm256_max_ps(v0, v4); v0 = l; v4 = h;
    m0 = _mm256_cmp_ps(v0, v4, _CMP_GT_OS);
    l = _mm256_blendv_ps(v0, v4, m0);
    h = _mm256_blendv_ps(v4, v0, m0); v0 = l; v4 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp4, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp0, m0); vp0 = pl; vp4 = ph;
    // l = _mm256_min_ps(v1, v5);
    // h = _mm256_max_ps(v1, v5); v1 = l; v5 = h;
    m0 = _mm256_cmp_ps(v1, v5, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v5, m0);
    h = _mm256_blendv_ps(v5, v1, m0); v1 = l; v5 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp5, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp1, m0); vp1 = pl; vp5 = ph;
    // l = _mm256_min_ps(v2, v6);
    // h = _mm256_max_ps(v2, v6); v2 = l; v6 = h;
    m0 = _mm256_cmp_ps(v2, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v2, v6, m0);
    h = _mm256_blendv_ps(v6, v2, m0); v2 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp2, m0); vp2 = pl; vp6 = ph;
    // l = _mm256_min_ps(v3, v7);
    // h = _mm256_max_ps(v3, v7); v3 = l; v7 = h;
    m0 = _mm256_cmp_ps(v3, v7, _CMP_GT_OS);
    l = _mm256_blendv_ps(v3, v7, m0);
    h = _mm256_blendv_ps(v7, v3, m0); v3 = l; v7 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp7, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp3, m0); vp3 = pl; vp7 = ph;
    /** step 5 */
    // l = _mm256_min_ps(v2, v4);
    // h = _mm256_max_ps(v2, v4); v2 = l; v4 = h;
    m0 = _mm256_cmp_ps(v2, v4, _CMP_GT_OS);
    l = _mm256_blendv_ps(v2, v4, m0);
    h = _mm256_blendv_ps(v4, v2, m0); v2 = l; v4 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp4, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp2, m0); vp2 = pl; vp4 = ph;

    // l = _mm256_min_ps(v3, v5);
    // h = _mm256_max_ps(v3, v5); v3 = l; v5 = h;
    m0 = _mm256_cmp_ps(v3, v5, _CMP_GT_OS);
    l = _mm256_blendv_ps(v3, v5, m0);
    h = _mm256_blendv_ps(v5, v3, m0); v3 = l; v5 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp5, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp3, m0); vp3 = pl; vp5 = ph;

    /** step 6 */
    // l = _mm256_min_ps(v1, v2);
    // h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    m0 = _mm256_cmp_ps(v1, v2, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v2, m0);
    h = _mm256_blendv_ps(v2, v1, m0); v1 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;

    // l = _mm256_min_ps(v3, v4);
    // h = _mm256_max_ps(v3, v4); v3 = l; v4 = h;
    m0 = _mm256_cmp_ps(v3, v4, _CMP_GT_OS);
    l = _mm256_blendv_ps(v3, v4, m0);
    h = _mm256_blendv_ps(v4, v3, m0); v3 = l; v4 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp4, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp3, m0); vp3 = pl; vp4 = ph;

    // l = _mm256_min_ps(v5, v6);
    // h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
    m0 = _mm256_cmp_ps(v5, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v5, v6, m0);
    h = _mm256_blendv_ps(v6, v5, m0); v5 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;
}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_sort_key(T& v0, T& v1, T& v2, T& v3, 
                 T& v4, T& v5, T& v6, T& v7,
        __m256i &vpl0 , __m256i &vph0 , __m256i &vpl1 , __m256i &vph1 , 
        __m256i &vpl2 , __m256i &vph2 , __m256i &vpl3 , __m256i &vph3 ,
        __m256i &vpl4 , __m256i &vph4 , __m256i &vpl5 , __m256i &vph5 , 
        __m256i &vpl6 , __m256i &vph6 , __m256i &vpl7 , __m256i &vph7 )
{
    __m256 l, h;
    __m256i pll, phl;
    __m256i plh, phh;
    __m256 m0;
    __m128i sl, sh;
    __m256d d_sl, d_sh;

    /** odd-even sorting network */ 
    /** step 1 */
    // l = _mm256_min_ps(v0, v1);
    // h = _mm256_max_ps(v0, v1); v0 = l; v1 = h;
    m0 = _mm256_cmp_ps(v0, v1, _CMP_GT_OS);
    l = _mm256_blendv_ps(v0, v1, m0);
    h = _mm256_blendv_ps(v1, v0, m0); v0 = l; v1 = h;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl0, (__m256d)vpl1, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph0, (__m256d)vph1, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl0, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph0, d_sh); vpl0 = pll; vph0 = phl; vpl1 = plh; vph1 = phh;
    
    // l = _mm256_min_ps(v2, v3);
    // h = _mm256_max_ps(v2, v3); v2 = l; v3 = h;
    m0 = _mm256_cmp_ps(v2, v3, _CMP_GT_OS);
    l = _mm256_blendv_ps(v2, v3, m0);
    h = _mm256_blendv_ps(v3, v2, m0); v2 = l; v3 = h;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl3, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph3, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl2, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph2, d_sh); vpl2 = pll; vph2 = phl; vpl3 = plh; vph3 = phh;

    // l = _mm256_min_ps(v4, v5);
    // h = _mm256_max_ps(v4, v5); v4 = l; v5 = h;
    m0 = _mm256_cmp_ps(v4, v5, _CMP_GT_OS);
    l = _mm256_blendv_ps(v4, v5, m0);
    h = _mm256_blendv_ps(v5, v4, m0); v4 = l; v5 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp5, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp4, m0); vp4 = pl; vp5 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl5, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph5, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl4, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph4, d_sh); vpl4 = pll; vph4 = phl; vpl5 = plh; vph5 = phh;

    // l = _mm256_min_ps(v6, v7);
    // h = _mm256_max_ps(v6, v7); v6 = l; v7 = h;
    m0 = _mm256_cmp_ps(v6, v7, _CMP_GT_OS);
    l = _mm256_blendv_ps(v6, v7, m0);
    h = _mm256_blendv_ps(v7, v6, m0); v6 = l; v7 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp7, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp6, m0); vp6 = pl; vp7 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl7, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph7, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl7, (__m256d)vpl6, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph7, (__m256d)vph6, d_sh); vpl6 = pll; vph6 = phl; vpl7 = plh; vph7 = phh;

    /** step 2 */
    // l = _mm256_min_ps(v0, v2);
    // h = _mm256_max_ps(v0, v2); v0 = l; v2 = h;
    m0 = _mm256_cmp_ps(v0, v2, _CMP_GT_OS);
    l = _mm256_blendv_ps(v0, v2, m0);
    h = _mm256_blendv_ps(v2, v0, m0); v0 = l; v2 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp2, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp0, m0); vp0 = pl; vp2 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl0, (__m256d)vpl2, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph0, (__m256d)vph2, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl0, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph0, d_sh); vpl0 = pll; vph0 = phl; vpl2 = plh; vph2 = phh;

    // l = _mm256_min_ps(v1, v3);         
    // h = _mm256_max_ps(v1, v3); v1 = l; v3 = h;
    m0 = _mm256_cmp_ps(v1, v3, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v3, m0);
    h = _mm256_blendv_ps(v3, v1, m0); v1 = l; v3 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp3, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp1, m0); vp1 = pl; vp3 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl3, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph3, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl3 = plh; vph3 = phh;

    // l = _mm256_min_ps(v4, v6);         
    // h = _mm256_max_ps(v4, v6); v4 = l; v6 = h;
    m0 = _mm256_cmp_ps(v4, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v4, v6, m0);
    h = _mm256_blendv_ps(v6, v4, m0); v4 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp4, m0); vp4 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl4, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph4, d_sh); vpl4 = pll; vph4 = phl; vpl6 = plh; vph6 = phh;

    // l = _mm256_min_ps(v5, v7);         
    // h = _mm256_max_ps(v5, v7); v5 = l; v7 = h;
    m0 = _mm256_cmp_ps(v5, v7, _CMP_GT_OS);
    l = _mm256_blendv_ps(v5, v7, m0);
    h = _mm256_blendv_ps(v7, v5, m0); v5 = l; v7 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp7, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp5, m0); vp5 = pl; vp7 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl7, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph7, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl7, (__m256d)vpl5, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph7, (__m256d)vph5, d_sh); vpl5 = pll; vph5 = phl; vpl7 = plh; vph7 = phh;

    /** step 3 */
    // l = _mm256_min_ps(v1, v2);
    // h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    m0 = _mm256_cmp_ps(v1, v2, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v2, m0);
    h = _mm256_blendv_ps(v2, v1, m0); v1 = l; v2 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl2, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph2, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl2 = plh; vph2 = phh;

    // l = _mm256_min_ps(v5, v6);
    // h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
    m0 = _mm256_cmp_ps(v5, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v5, v6, m0);
    h = _mm256_blendv_ps(v6, v5, m0); v5 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl5, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph5, d_sh); vpl5 = pll; vph5 = phl; vpl6 = plh; vph6 = phh;

    /** step 4 */
    // l = _mm256_min_ps(v0, v4);
    // h = _mm256_max_ps(v0, v4); v0 = l; v4 = h;
    m0 = _mm256_cmp_ps(v0, v4, _CMP_GT_OS);
    l = _mm256_blendv_ps(v0, v4, m0);
    h = _mm256_blendv_ps(v4, v0, m0); v0 = l; v4 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp4, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp0, m0); vp0 = pl; vp4 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl0, (__m256d)vpl4, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph0, (__m256d)vph4, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl0, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph0, d_sh); vpl0 = pll; vph0 = phl; vpl4 = plh; vph4 = phh;
    // l = _mm256_min_ps(v1, v5);
    // h = _mm256_max_ps(v1, v5); v1 = l; v5 = h;
    m0 = _mm256_cmp_ps(v1, v5, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v5, m0);
    h = _mm256_blendv_ps(v5, v1, m0); v1 = l; v5 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp5, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp1, m0); vp1 = pl; vp5 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl5, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph5, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl5 = plh; vph5 = phh;
    // l = _mm256_min_ps(v2, v6);
    // h = _mm256_max_ps(v2, v6); v2 = l; v6 = h;
    m0 = _mm256_cmp_ps(v2, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v2, v6, m0);
    h = _mm256_blendv_ps(v6, v2, m0); v2 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp2, m0); vp2 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl2, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph2, d_sh); vpl2 = pll; vph2 = phl; vpl6 = plh; vph6 = phh;
    // l = _mm256_min_ps(v3, v7);
    // h = _mm256_max_ps(v3, v7); v3 = l; v7 = h;
    m0 = _mm256_cmp_ps(v3, v7, _CMP_GT_OS);
    l = _mm256_blendv_ps(v3, v7, m0);
    h = _mm256_blendv_ps(v7, v3, m0); v3 = l; v7 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp7, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp3, m0); vp3 = pl; vp7 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl7, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph7, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl7, (__m256d)vpl3, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph7, (__m256d)vph3, d_sh); vpl3 = pll; vph3 = phl; vpl7 = plh; vph7 = phh;
    /** step 5 */
    // l = _mm256_min_ps(v2, v4);
    // h = _mm256_max_ps(v2, v4); v2 = l; v4 = h;
    m0 = _mm256_cmp_ps(v2, v4, _CMP_GT_OS);
    l = _mm256_blendv_ps(v2, v4, m0);
    h = _mm256_blendv_ps(v4, v2, m0); v2 = l; v4 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp4, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp2, m0); vp2 = pl; vp4 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl4, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph4, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl2, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph2, d_sh); vpl2 = pll; vph2 = phl; vpl4 = plh; vph4 = phh;

    // l = _mm256_min_ps(v3, v5);
    // h = _mm256_max_ps(v3, v5); v3 = l; v5 = h;
    m0 = _mm256_cmp_ps(v3, v5, _CMP_GT_OS);
    l = _mm256_blendv_ps(v3, v5, m0);
    h = _mm256_blendv_ps(v5, v3, m0); v3 = l; v5 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp5, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp3, m0); vp3 = pl; vp5 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl5, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph5, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl3, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph3, d_sh); vpl3 = pll; vph3 = phl; vpl5 = plh; vph5 = phh;

    /** step 6 */
    // l = _mm256_min_ps(v1, v2);
    // h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    m0 = _mm256_cmp_ps(v1, v2, _CMP_GT_OS);
    l = _mm256_blendv_ps(v1, v2, m0);
    h = _mm256_blendv_ps(v2, v1, m0); v1 = l; v2 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl2, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph2, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl2 = plh; vph2 = phh;

    // l = _mm256_min_ps(v3, v4);
    // h = _mm256_max_ps(v3, v4); v3 = l; v4 = h;
    m0 = _mm256_cmp_ps(v3, v4, _CMP_GT_OS);
    l = _mm256_blendv_ps(v3, v4, m0);
    h = _mm256_blendv_ps(v4, v3, m0); v3 = l; v4 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp4, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp3, m0); vp3 = pl; vp4 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl4, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph4, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl3, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph3, d_sh); vpl3 = pll; vph3 = phl; vpl4 = plh; vph4 = phh;

    // l = _mm256_min_ps(v5, v6);
    // h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
    m0 = _mm256_cmp_ps(v5, v6, _CMP_GT_OS);
    l = _mm256_blendv_ps(v5, v6, m0);
    h = _mm256_blendv_ps(v6, v5, m0); v5 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl5, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph5, d_sh); vpl5 = pll; vph5 = phl; vpl6 = plh; vph6 = phh;

}

/**
 * Integer vector version (__m256i):
 * This method performs the in-register sort. The sorted elements 
 * are stored vertically accross the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored vertically among the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_sort(T& v0, T& v1, T& v2, T& v3, 
                 T& v4, T& v5, T& v6, T& v7)
{
    __m256i l, h;

    /** odd-even sorting network */ 
    /** step 1 */
    l = _mm256_min_epi32(v0, v1);
    h = _mm256_max_epi32(v0, v1); v0 = l; v1 = h;
    l = _mm256_min_epi32(v2, v3);
    h = _mm256_max_epi32(v2, v3); v2 = l; v3 = h;
    l = _mm256_min_epi32(v4, v5);
    h = _mm256_max_epi32(v4, v5); v4 = l; v5 = h;
    l = _mm256_min_epi32(v6, v7);
    h = _mm256_max_epi32(v6, v7); v6 = l; v7 = h;
    /** step 2 */
    l = _mm256_min_epi32(v0, v2);
    h = _mm256_max_epi32(v0, v2); v0 = l; v2 = h;
    l = _mm256_min_epi32(v1, v3);         
    h = _mm256_max_epi32(v1, v3); v1 = l; v3 = h;
    l = _mm256_min_epi32(v4, v6);         
    h = _mm256_max_epi32(v4, v6); v4 = l; v6 = h;
    l = _mm256_min_epi32(v5, v7);         
    h = _mm256_max_epi32(v5, v7); v5 = l; v7 = h;
    /** step 3 */
    l = _mm256_min_epi32(v1, v2);
    h = _mm256_max_epi32(v1, v2); v1 = l; v2 = h;
    l = _mm256_min_epi32(v5, v6);
    h = _mm256_max_epi32(v5, v6); v5 = l; v6 = h;
    /** step 4 */
    l = _mm256_min_epi32(v0, v4);
    h = _mm256_max_epi32(v0, v4); v0 = l; v4 = h;
    l = _mm256_min_epi32(v1, v5);
    h = _mm256_max_epi32(v1, v5); v1 = l; v5 = h;
    l = _mm256_min_epi32(v2, v6);
    h = _mm256_max_epi32(v2, v6); v2 = l; v6 = h;
    l = _mm256_min_epi32(v3, v7);
    h = _mm256_max_epi32(v3, v7); v3 = l; v7 = h;
    /** step 5 */
    l = _mm256_min_epi32(v2, v4);
    h = _mm256_max_epi32(v2, v4); v2 = l; v4 = h;
    l = _mm256_min_epi32(v3, v5);
    h = _mm256_max_epi32(v3, v5); v3 = l; v5 = h;
    /** step 6 */
    l = _mm256_min_epi32(v1, v2);
    h = _mm256_max_epi32(v1, v2); v1 = l; v2 = h;
    l = _mm256_min_epi32(v3, v4);
    h = _mm256_max_epi32(v3, v4); v3 = l; v4 = h;
    l = _mm256_min_epi32(v5, v6);
    h = _mm256_max_epi32(v5, v6); v5 = l; v6 = h;
}

template <typename T> // int key int index
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_sort_key(T& v0, T& v1, T& v2, T& v3, 
                 T& v4, T& v5, T& v6, T& v7,
        __m256i &vp0, __m256i &vp1, __m256i &vp2, __m256i &vp3, 
        __m256i &vp4, __m256i &vp5, __m256i &vp6, __m256i &vp7)
{
    __m256i l, h;
    __m256i pl, ph;
    __m256 m0;

    /** odd-even sorting network */ 
    /** step 1 */
    // l = util::_my_mm256_min_epi32(v0, v1);
    // h = util::_my_mm256_max_epi32(v0, v1); v0 = l; v1 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v1);
    l = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v1, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v0, m0); v0 = l; v1 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp1, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp0, m0); vp0 = pl; vp1 = ph;
    // l = util::_my_mm256_min_epi32(v2, v3);
    // h = util::_my_mm256_max_epi32(v2, v3); v2 = l; v3 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v2, v3);
    l = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v3, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v2, m0); v2 = l; v3 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp3, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp2, m0); vp2 = pl; vp3 = ph;
    // l = util::_my_mm256_min_epi32(v4, v5);
    // h = util::_my_mm256_max_epi32(v4, v5); v4 = l; v5 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v4, v5);
    l = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v5, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v4, m0); v4 = l; v5 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp5, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp4, m0); vp4 = pl; vp5 = ph;
    // l = util::_my_mm256_min_epi32(v6, v7);
    // h = util::_my_mm256_max_epi32(v6, v7); v6 = l; v7 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v6, v7);
    l = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v7, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v7, (__m256)v6, m0); v6 = l; v7 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp7, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp6, m0); vp6 = pl; vp7 = ph;
    /** step 2 */
    // l = util::_my_mm256_min_epi32(v0, v2);
    // h = util::_my_mm256_max_epi32(v0, v2); v0 = l; v2 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v2);
    l = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v2, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v0, m0); v0 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp2, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp0, m0); vp0 = pl; vp2 = ph;
    // l = util::_my_mm256_min_epi32(v1, v3);         
    // h = util::_my_mm256_max_epi32(v1, v3); v1 = l; v3 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v3);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v3, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v1, m0); v1 = l; v3 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp3, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp1, m0); vp1 = pl; vp3 = ph;
    // l = util::_my_mm256_min_epi32(v4, v6);         
    // h = util::_my_mm256_max_epi32(v4, v6); v4 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v4, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v4, m0); v4 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp4, m0); vp4 = pl; vp6 = ph;
    // l = util::_my_mm256_min_epi32(v5, v7);         
    // h = util::_my_mm256_max_epi32(v5, v7); v5 = l; v7 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v5, v7);
    l = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v7, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v7, (__m256)v5, m0); v5 = l; v7 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp7, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp5, m0); vp5 = pl; vp7 = ph;
    /** step 3 */
    // l = util::_my_mm256_min_epi32(v1, v2);
    // h = util::_my_mm256_max_epi32(v1, v2); v1 = l; v2 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v2);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v2, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v1, m0); v1 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;
    // l = util::_my_mm256_min_epi32(v5, v6);
    // h = util::_my_mm256_max_epi32(v5, v6); v5 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v5, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v5, m0); v5 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;
    /** step 4 */
    // l = util::_my_mm256_min_epi32(v0, v4);
    // h = util::_my_mm256_max_epi32(v0, v4); v0 = l; v4 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v4);
    l = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v4, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v0, m0); v0 = l; v4 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp4, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp0, m0); vp0 = pl; vp4 = ph;
    // l = util::_my_mm256_min_epi32(v1, v5);
    // h = util::_my_mm256_max_epi32(v1, v5); v1 = l; v5 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v5);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v5, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v1, m0); v1 = l; v5 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp5, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp1, m0); vp1 = pl; vp5 = ph;
    // l = util::_my_mm256_min_epi32(v2, v6);
    // h = util::_my_mm256_max_epi32(v2, v6); v2 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v2, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v2, m0); v2 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp2, m0); vp2 = pl; vp6 = ph;
    // l = util::_my_mm256_min_epi32(v3, v7);
    // h = util::_my_mm256_max_epi32(v3, v7); v3 = l; v7 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v3, v7);
    l = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v7, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v7, (__m256)v3, m0); v3 = l; v7 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp7, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp3, m0); vp3 = pl; vp7 = ph;
    /** step 5 */
    // l = util::_my_mm256_min_epi32(v2, v4);
    // h = util::_my_mm256_max_epi32(v2, v4); v2 = l; v4 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v2, v4);
    l = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v4, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v2, m0); v2 = l; v4 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp4, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp2, m0); vp2 = pl; vp4 = ph;
    // l = util::_my_mm256_min_epi32(v3, v5);
    // h = util::_my_mm256_max_epi32(v3, v5); v3 = l; v5 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v3, v5);
    l = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v5, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v3, m0); v3 = l; v5 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp5, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp3, m0); vp3 = pl; vp5 = ph;
    /** step 6 */
    // l = util::_my_mm256_min_epi32(v1, v2);
    // h = util::_my_mm256_max_epi32(v1, v2); v1 = l; v2 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v2);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v2, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v1, m0); v1 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;
    // l = util::_my_mm256_min_epi32(v3, v4);
    // h = util::_my_mm256_max_epi32(v3, v4); v3 = l; v4 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v3, v4);
    l = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v4, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v3, m0); v3 = l; v4 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp4, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp3, m0); vp3 = pl; vp4 = ph;
    // l = util::_my_mm256_min_epi32(v5, v6);
    // h = util::_my_mm256_max_epi32(v5, v6); v5 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v5, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v5, m0); v5 = l; v6 = h;
    pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;
}

template <typename T> // int key long index
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_sort_key(T& v0, T& v1, T& v2, T& v3, 
                 T& v4, T& v5, T& v6, T& v7,
        __m256i &vpl0 , __m256i &vph0 , __m256i &vpl1 , __m256i &vph1 , 
        __m256i &vpl2 , __m256i &vph2 , __m256i &vpl3 , __m256i &vph3 ,
        __m256i &vpl4 , __m256i &vph4 , __m256i &vpl5 , __m256i &vph5 , 
        __m256i &vpl6 , __m256i &vph6 , __m256i &vpl7 , __m256i &vph7 )
{
    __m256i l, h;
    __m256i pll, phl;
    __m256i plh, phh;
    __m256 m0;
    __m128i sl, sh;
    __m256d d_sl, d_sh;

    /** odd-even sorting network */ 
    /** step 1 */
    // l = _mm256_min_ps(v0, v1);
    // h = _mm256_max_ps(v0, v1); v0 = l; v1 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v1);
    l = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v1, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v1,(__m256) v0, m0); v0 = l; v1 = h;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl0, (__m256d)vpl1, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph0, (__m256d)vph1, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl0, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph0, d_sh); vpl0 = pll; vph0 = phl; vpl1 = plh; vph1 = phh;
    
    // l = _mm256_min_ps(v2, v3);
    // h = _mm256_max_ps(v2, v3); v2 = l; v3 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v2, v3);
    l = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v3, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v2, m0); v2 = l; v3 = h;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl3, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph3, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl2, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph2, d_sh); vpl2 = pll; vph2 = phl; vpl3 = plh; vph3 = phh;

    // l = _mm256_min_ps(v4, v5);
    // h = _mm256_max_ps(v4, v5); v4 = l; v5 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v4, v5);
    l = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v5, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v4, m0); v4 = l; v5 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp5, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp4, m0); vp4 = pl; vp5 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl5, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph5, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl4, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph4, d_sh); vpl4 = pll; vph4 = phl; vpl5 = plh; vph5 = phh;

    // l = _mm256_min_ps(v6, v7);
    // h = _mm256_max_ps(v6, v7); v6 = l; v7 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v6, v7);
    l = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v7, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v7, (__m256)v6, m0); v6 = l; v7 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp7, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp6, m0); vp6 = pl; vp7 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl7, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph7, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl7, (__m256d)vpl6, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph7, (__m256d)vph6, d_sh); vpl6 = pll; vph6 = phl; vpl7 = plh; vph7 = phh;

    /** step 2 */
    // l = _mm256_min_ps(v0, v2);
    // h = _mm256_max_ps(v0, v2); v0 = l; v2 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v2);
    l = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v2, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v0, m0); v0 = l; v2 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp2, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp0, m0); vp0 = pl; vp2 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl0, (__m256d)vpl2, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph0, (__m256d)vph2, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl0, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph0, d_sh); vpl0 = pll; vph0 = phl; vpl2 = plh; vph2 = phh;

    // l = _mm256_min_ps(v1, v3);         
    // h = _mm256_max_ps(v1, v3); v1 = l; v3 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v3);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v3, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v1, m0); v1 = l; v3 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp3, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp1, m0); vp1 = pl; vp3 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl3, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph3, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl3 = plh; vph3 = phh;

    // l = _mm256_min_ps(v4, v6);         
    // h = _mm256_max_ps(v4, v6); v4 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v4, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v4, m0); v4 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp4, m0); vp4 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl4, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph4, d_sh); vpl4 = pll; vph4 = phl; vpl6 = plh; vph6 = phh;

    // l = _mm256_min_ps(v5, v7);         
    // h = _mm256_max_ps(v5, v7); v5 = l; v7 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v5, v7);
    l = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v7, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v7, (__m256)v5, m0); v5 = l; v7 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp7, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp5, m0); vp5 = pl; vp7 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl7, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph7, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl7, (__m256d)vpl5, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph7, (__m256d)vph5, d_sh); vpl5 = pll; vph5 = phl; vpl7 = plh; vph7 = phh;

    /** step 3 */
    // l = _mm256_min_ps(v1, v2);
    // h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v2);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v2, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v1, m0); v1 = l; v2 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl2, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph2, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl2 = plh; vph2 = phh;

    // l = _mm256_min_ps(v5, v6);
    // h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v5, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v5, m0); v5 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl5, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph5, d_sh); vpl5 = pll; vph5 = phl; vpl6 = plh; vph6 = phh;

    /** step 4 */
    // l = _mm256_min_ps(v0, v4);
    // h = _mm256_max_ps(v0, v4); v0 = l; v4 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v4);
    l = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v4, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v0, m0); v0 = l; v4 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp0, (__m256)vp4, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp0, m0); vp0 = pl; vp4 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl0, (__m256d)vpl4, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph0, (__m256d)vph4, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl0, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph0, d_sh); vpl0 = pll; vph0 = phl; vpl4 = plh; vph4 = phh;
    // l = _mm256_min_ps(v1, v5);
    // h = _mm256_max_ps(v1, v5); v1 = l; v5 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v5);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v5, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v1, m0); v1 = l; v5 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp5, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp1, m0); vp1 = pl; vp5 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl5, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph5, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl5 = plh; vph5 = phh;
    // l = _mm256_min_ps(v2, v6);
    // h = _mm256_max_ps(v2, v6); v2 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v2, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v2, m0); v2 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp2, m0); vp2 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl2, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph2, d_sh); vpl2 = pll; vph2 = phl; vpl6 = plh; vph6 = phh;
    // l = _mm256_min_ps(v3, v7);
    // h = _mm256_max_ps(v3, v7); v3 = l; v7 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v3, v7);
    l = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v7, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v7, (__m256)v3, m0); v3 = l; v7 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp7, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp7, (__m256)vp3, m0); vp3 = pl; vp7 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl7, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph7, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl7, (__m256d)vpl3, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph7, (__m256d)vph3, d_sh); vpl3 = pll; vph3 = phl; vpl7 = plh; vph7 = phh;
    /** step 5 */
    // l = _mm256_min_ps(v2, v4);
    // h = _mm256_max_ps(v2, v4); v2 = l; v4 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v2, v4);
    l = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v4, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v2, m0); v2 = l; v4 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp4, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp2, m0); vp2 = pl; vp4 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl4, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph4, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl2, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph2, d_sh); vpl2 = pll; vph2 = phl; vpl4 = plh; vph4 = phh;

    // l = _mm256_min_ps(v3, v5);
    // h = _mm256_max_ps(v3, v5); v3 = l; v5 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v3, v5);
    l = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v5, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v3, m0); v3 = l; v5 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp5, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp3, m0); vp3 = pl; vp5 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl5, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph5, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl3, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph3, d_sh); vpl3 = pll; vph3 = phl; vpl5 = plh; vph5 = phh;

    /** step 6 */
    // l = _mm256_min_ps(v1, v2);
    // h = _mm256_max_ps(v1, v2); v1 = l; v2 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v1, v2);
    l = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v2, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v2, (__m256)v1, m0); v1 = l; v2 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp1, (__m256)vp2, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp2, (__m256)vp1, m0); vp1 = pl; vp2 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl1, (__m256d)vpl2, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph1, (__m256d)vph2, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl2, (__m256d)vpl1, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph2, (__m256d)vph1, d_sh); vpl1 = pll; vph1 = phl; vpl2 = plh; vph2 = phh;

    // l = _mm256_min_ps(v3, v4);
    // h = _mm256_max_ps(v3, v4); v3 = l; v4 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v3, v4);
    l = (__m256i)_mm256_blendv_ps((__m256)v3, (__m256)v4, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v4, (__m256)v3, m0); v3 = l; v4 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp3, (__m256)vp4, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp4, (__m256)vp3, m0); vp3 = pl; vp4 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl3, (__m256d)vpl4, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph3, (__m256d)vph4, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl4, (__m256d)vpl3, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph4, (__m256d)vph3, d_sh); vpl3 = pll; vph3 = phl; vpl4 = plh; vph4 = phh;

    // l = _mm256_min_ps(v5, v6);
    // h = _mm256_max_ps(v5, v6); v5 = l; v6 = h;
    m0 = (__m256)_mm256_cmpgt_epi32(v5, v6);
    l = (__m256i)_mm256_blendv_ps((__m256)v5, (__m256)v6, m0);
    h = (__m256i)_mm256_blendv_ps((__m256)v6, (__m256)v5, m0); v5 = l; v6 = h;
    // pl = (__m256i)_mm256_blendv_ps((__m256)vp5, (__m256)vp6, m0);
    // ph = (__m256i)_mm256_blendv_ps((__m256)vp6, (__m256)vp5, m0); vp5 = pl; vp6 = ph;
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll = (__m256i)_mm256_blendv_pd((__m256d)vpl5, (__m256d)vpl6, d_sl);
    phl = (__m256i)_mm256_blendv_pd((__m256d)vph5, (__m256d)vph6, d_sh);
    plh = (__m256i)_mm256_blendv_pd((__m256d)vpl6, (__m256d)vpl5, d_sl); 
    phh = (__m256i)_mm256_blendv_pd((__m256d)vph6, (__m256d)vph5, d_sh); vpl5 = pll; vph5 = phl; vpl6 = plh; vph6 = phh;

}

/**
 * Double vector version (__m256d):
 * This method performs the in-register sort. The sorted elements 
 * are stored vertically accross the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored vertically among the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_sort(T& v0, T& v1, T& v2, T& v3)
{
    __m256d l, h;
    /** odd-even sorting network */ 
    /** step 1 */
    l = _mm256_min_pd(v0, v1);
    h = _mm256_max_pd(v0, v1); v0 = l; v1 = h;
    l = _mm256_min_pd(v2, v3);
    h = _mm256_max_pd(v2, v3); v2 = l; v3 = h;
    /** step 2 */
    l = _mm256_min_pd(v0, v2);
    h = _mm256_max_pd(v0, v2); v0 = l; v2 = h;
    l = _mm256_min_pd(v1, v3);
    h = _mm256_max_pd(v1, v3); v1 = l; v3 = h;
    /** step 3 */
    l = _mm256_min_pd(v1, v2);
    h = _mm256_max_pd(v1, v2); v1 = l; v2 = h;
}

template <typename T> // double key int index
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_sort_key(T& v0, T& v1, T& v2, T& v3,
        __m256i &vp0, __m256i &vp1)
{
    __m256d l, h;
    __m128 p0 = _mm256_castps256_ps128((__m256)vp0);
    __m128 p1 = _mm256_castps256_ps128(_mm256_permute2f128_ps((__m256)vp0, (__m256)vp0, 0x01));
    __m128 p2 = _mm256_castps256_ps128((__m256)vp1);
    __m128 p3 = _mm256_castps256_ps128(_mm256_permute2f128_ps((__m256)vp1, (__m256)vp1, 0x01));
    __m128 pl, ph;
    __m256d m0;
    __m128 m0i;
    /** odd-even sorting network */ 
    /** step 1 */
    // l = _mm256_min_pd(v0, v1);
    // h = _mm256_max_pd(v0, v1); v0 = l; v1 = h;
    m0 = _mm256_cmp_pd(v0, v1, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l = _mm256_blendv_pd(v0, v1, m0);
    h = _mm256_blendv_pd(v1, v0, m0); v0 = l; v1 = h;
    pl = _mm_blendv_ps(p0, p1, m0i);
    ph = _mm_blendv_ps(p1, p0, m0i); p0 = pl; p1 = ph;

    // l = _mm256_min_pd(v2, v3);
    // h = _mm256_max_pd(v2, v3); v2 = l; v3 = h;
    m0 = _mm256_cmp_pd(v2, v3, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l = _mm256_blendv_pd(v2, v3, m0);
    h = _mm256_blendv_pd(v3, v2, m0); v2 = l; v3 = h;
    pl = _mm_blendv_ps(p2, p3, m0i);
    ph = _mm_blendv_ps(p3, p2, m0i); p2 = pl; p3 = ph;
    /** step 2 */
    // l = _mm256_min_pd(v0, v2);
    // h = _mm256_max_pd(v0, v2); v0 = l; v2 = h;
    m0 = _mm256_cmp_pd(v0, v2, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l = _mm256_blendv_pd(v0, v2, m0);
    h = _mm256_blendv_pd(v2, v0, m0); v0 = l; v2 = h;
    pl = _mm_blendv_ps(p0, p2, m0i);
    ph = _mm_blendv_ps(p2, p0, m0i); p0 = pl; p2 = ph;
    // l = _mm256_min_pd(v1, v3);
    // h = _mm256_max_pd(v1, v3); v1 = l; v3 = h;
    m0 = _mm256_cmp_pd(v1, v3, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l = _mm256_blendv_pd(v1, v3, m0);
    h = _mm256_blendv_pd(v3, v1, m0); v1 = l; v3 = h;
    pl = _mm_blendv_ps(p1, p3, m0i);
    ph = _mm_blendv_ps(p3, p1, m0i); p1 = pl; p3 = ph;
    /** step 3 */
    // l = _mm256_min_pd(v1, v2);
    // h = _mm256_max_pd(v1, v2); v1 = l; v2 = h;
    m0 = _mm256_cmp_pd(v1, v2, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l = _mm256_blendv_pd(v1, v2, m0);
    h = _mm256_blendv_pd(v2, v1, m0); v1 = l; v2 = h;
    pl = _mm_blendv_ps(p1, p2, m0i);
    ph = _mm_blendv_ps(p2, p1, m0i); p1 = pl; p2 = ph;

    vp0 = _mm256_insertf128_si256(vp0, (__m128i)p0, 0);
    vp0 = _mm256_insertf128_si256(vp0, (__m128i)p1, 1);
    vp1 = _mm256_insertf128_si256(vp1, (__m128i)p2, 0);
    vp1 = _mm256_insertf128_si256(vp1, (__m128i)p3, 1);
}

template <typename T> // double key long index
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_sort_key(T& v0, T& v1, T& v2, T& v3,
        __m256i &vp0, __m256i &vp1, __m256i &vp2, __m256i &vp3)
{
    __m256d l, h;
    __m256i pl, ph;
    __m256d m0;
    /** odd-even sorting network */ 
    /** step 1 */
    // l = _mm256_min_pd(v0, v1);
    // h = _mm256_max_pd(v0, v1); v0 = l; v1 = h;
    m0 = _mm256_cmp_pd(v0, v1, _CMP_GT_OS);
    l = _mm256_blendv_pd(v0, v1, m0);
    h = _mm256_blendv_pd(v1, v0, m0); v0 = l; v1 = h;
    pl = (__m256i)_mm256_blendv_pd((__m256d)vp0, (__m256d)vp1, m0);
    ph = (__m256i)_mm256_blendv_pd((__m256d)vp1, (__m256d)vp0, m0); vp0 = pl; vp1 = ph;

    // l = _mm256_min_pd(v2, v3);
    // h = _mm256_max_pd(v2, v3); v2 = l; v3 = h;
    m0 = _mm256_cmp_pd(v2, v3, _CMP_GT_OS);
    l = _mm256_blendv_pd(v2, v3, m0);
    h = _mm256_blendv_pd(v3, v2, m0); v2 = l; v3 = h;
    pl = (__m256i)_mm256_blendv_pd((__m256d)vp2, (__m256d)vp3, m0);
    ph = (__m256i)_mm256_blendv_pd((__m256d)vp3, (__m256d)vp2, m0); vp2 = pl; vp3 = ph;
    /** step 2 */
    // l = _mm256_min_pd(v0, v2);
    // h = _mm256_max_pd(v0, v2); v0 = l; v2 = h;
    m0 = _mm256_cmp_pd(v0, v2, _CMP_GT_OS);
    l = _mm256_blendv_pd(v0, v2, m0);
    h = _mm256_blendv_pd(v2, v0, m0); v0 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_pd((__m256d)vp0, (__m256d)vp2, m0);
    ph = (__m256i)_mm256_blendv_pd((__m256d)vp2, (__m256d)vp0, m0); vp0 = pl; vp2 = ph;
    // l = _mm256_min_pd(v1, v3);
    // h = _mm256_max_pd(v1, v3); v1 = l; v3 = h;
    m0 = _mm256_cmp_pd(v1, v3, _CMP_GT_OS);
    l = _mm256_blendv_pd(v1, v3, m0);
    h = _mm256_blendv_pd(v3, v1, m0); v1 = l; v3 = h;
    pl = (__m256i)_mm256_blendv_pd((__m256d)vp1, (__m256d)vp3, m0);
    ph = (__m256i)_mm256_blendv_pd((__m256d)vp3, (__m256d)vp1, m0); vp1 = pl; vp3 = ph;
    /** step 3 */
    // l = _mm256_min_pd(v1, v2);
    // h = _mm256_max_pd(v1, v2); v1 = l; v2 = h;
    m0 = _mm256_cmp_pd(v1, v2, _CMP_GT_OS);
    l = _mm256_blendv_pd(v1, v2, m0);
    h = _mm256_blendv_pd(v2, v1, m0); v1 = l; v2 = h;
    pl = (__m256i)_mm256_blendv_pd((__m256d)vp1, (__m256d)vp2, m0);
    ph = (__m256i)_mm256_blendv_pd((__m256d)vp2, (__m256d)vp1, m0); vp1 = pl; vp2 = ph;

}
/**
 * Float vector version (__m256):
 * This method performs the in-register transpose. The sorted elements 
 * are stored horizontally within the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored horizontally in the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_transpose(T& v0, T& v1, T& v2, T& v3, 
                      T& v4, T& v5, T& v6, T& v7)
{
    __m256 __t0, __t1, __t2, __t3, __t4, __t5, __t6, __t7;
    __m256 __tt0, __tt1, __tt2, __tt3, __tt4, __tt5, __tt6, __tt7;
    __t0 = _mm256_unpacklo_ps(v0, v1);
    __t1 = _mm256_unpackhi_ps(v0, v1);
    __t2 = _mm256_unpacklo_ps(v2, v3);
    __t3 = _mm256_unpackhi_ps(v2, v3);
    __t4 = _mm256_unpacklo_ps(v4, v5);
    __t5 = _mm256_unpackhi_ps(v4, v5);
    __t6 = _mm256_unpacklo_ps(v6, v7);
    __t7 = _mm256_unpackhi_ps(v6, v7);
    __tt0 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(1,0,1,0));
    __tt1 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(3,2,3,2));
    __tt2 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(1,0,1,0));
    __tt3 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(3,2,3,2));
    __tt4 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(1,0,1,0));
    __tt5 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(3,2,3,2));
    __tt6 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(1,0,1,0));
    __tt7 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(3,2,3,2));
    v0 = _mm256_permute2f128_ps(__tt0, __tt4, 0x20);
    v1 = _mm256_permute2f128_ps(__tt1, __tt5, 0x20);
    v2 = _mm256_permute2f128_ps(__tt2, __tt6, 0x20);
    v3 = _mm256_permute2f128_ps(__tt3, __tt7, 0x20);
    v4 = _mm256_permute2f128_ps(__tt0, __tt4, 0x31);
    v5 = _mm256_permute2f128_ps(__tt1, __tt5, 0x31);
    v6 = _mm256_permute2f128_ps(__tt2, __tt6, 0x31);
    v7 = _mm256_permute2f128_ps(__tt3, __tt7, 0x31);
}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_transpose_key(T& v0, T& v1, T& v2, T& v3, 
                      T& v4, T& v5, T& v6, T& v7,
        __m256i &vp0, __m256i &vp1, __m256i &vp2, __m256i &vp3, 
        __m256i &vp4, __m256i &vp5, __m256i &vp6, __m256i &vp7)
{
    __m256 __t0, __t1, __t2, __t3, __t4, __t5, __t6, __t7;
    __m256 __tp0, __tp1, __tp2, __tp3, __tp4, __tp5, __tp6, __tp7;
    __m256 __tt0, __tt1, __tt2, __tt3, __tt4, __tt5, __tt6, __tt7;
    __m256 __ttp0, __ttp1, __ttp2, __ttp3, __ttp4, __ttp5, __ttp6, __ttp7;
    __t0 = _mm256_unpacklo_ps(v0, v1);
    __t1 = _mm256_unpackhi_ps(v0, v1);
    __t2 = _mm256_unpacklo_ps(v2, v3);
    __t3 = _mm256_unpackhi_ps(v2, v3);
    __t4 = _mm256_unpacklo_ps(v4, v5);
    __t5 = _mm256_unpackhi_ps(v4, v5);
    __t6 = _mm256_unpacklo_ps(v6, v7);
    __t7 = _mm256_unpackhi_ps(v6, v7);
    __tp0 = _mm256_unpacklo_ps((__m256)vp0, (__m256)vp1);
    __tp1 = _mm256_unpackhi_ps((__m256)vp0, (__m256)vp1);
    __tp2 = _mm256_unpacklo_ps((__m256)vp2, (__m256)vp3);
    __tp3 = _mm256_unpackhi_ps((__m256)vp2, (__m256)vp3);
    __tp4 = _mm256_unpacklo_ps((__m256)vp4, (__m256)vp5);
    __tp5 = _mm256_unpackhi_ps((__m256)vp4, (__m256)vp5);
    __tp6 = _mm256_unpacklo_ps((__m256)vp6, (__m256)vp7);
    __tp7 = _mm256_unpackhi_ps((__m256)vp6, (__m256)vp7);

    __tt0 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(1,0,1,0));
    __tt1 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(3,2,3,2));
    __tt2 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(1,0,1,0));
    __tt3 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(3,2,3,2));
    __tt4 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(1,0,1,0));
    __tt5 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(3,2,3,2));
    __tt6 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(1,0,1,0));
    __tt7 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(3,2,3,2));
    __ttp0 = _mm256_shuffle_ps(__tp0,__tp2,_MM_SHUFFLE(1,0,1,0));
    __ttp1 = _mm256_shuffle_ps(__tp0,__tp2,_MM_SHUFFLE(3,2,3,2));
    __ttp2 = _mm256_shuffle_ps(__tp1,__tp3,_MM_SHUFFLE(1,0,1,0));
    __ttp3 = _mm256_shuffle_ps(__tp1,__tp3,_MM_SHUFFLE(3,2,3,2));
    __ttp4 = _mm256_shuffle_ps(__tp4,__tp6,_MM_SHUFFLE(1,0,1,0));
    __ttp5 = _mm256_shuffle_ps(__tp4,__tp6,_MM_SHUFFLE(3,2,3,2));
    __ttp6 = _mm256_shuffle_ps(__tp5,__tp7,_MM_SHUFFLE(1,0,1,0));
    __ttp7 = _mm256_shuffle_ps(__tp5,__tp7,_MM_SHUFFLE(3,2,3,2));

    v0 = _mm256_permute2f128_ps(__tt0, __tt4, 0x20);
    v1 = _mm256_permute2f128_ps(__tt1, __tt5, 0x20);
    v2 = _mm256_permute2f128_ps(__tt2, __tt6, 0x20);
    v3 = _mm256_permute2f128_ps(__tt3, __tt7, 0x20);
    v4 = _mm256_permute2f128_ps(__tt0, __tt4, 0x31);
    v5 = _mm256_permute2f128_ps(__tt1, __tt5, 0x31);
    v6 = _mm256_permute2f128_ps(__tt2, __tt6, 0x31);
    v7 = _mm256_permute2f128_ps(__tt3, __tt7, 0x31);
    vp0 = (__m256i)_mm256_permute2f128_ps(__ttp0, __ttp4, 0x20);
    vp1 = (__m256i)_mm256_permute2f128_ps(__ttp1, __ttp5, 0x20);
    vp2 = (__m256i)_mm256_permute2f128_ps(__ttp2, __ttp6, 0x20);
    vp3 = (__m256i)_mm256_permute2f128_ps(__ttp3, __ttp7, 0x20);
    vp4 = (__m256i)_mm256_permute2f128_ps(__ttp0, __ttp4, 0x31);
    vp5 = (__m256i)_mm256_permute2f128_ps(__ttp1, __ttp5, 0x31);
    vp6 = (__m256i)_mm256_permute2f128_ps(__ttp2, __ttp6, 0x31);
    vp7 = (__m256i)_mm256_permute2f128_ps(__ttp3, __ttp7, 0x31);
}

__m256d _my_unpacklo_pd(__m256i vpl0, __m256i vpl1)
{
    __m256d tpl0 = _mm256_permute2f128_pd((__m256d)vpl0, (__m256d)vpl1, 0x20);
    __m256d trl0 = _mm256_permute_pd(_mm256_permute2f128_pd(tpl0, tpl0, 0x21), 0x5);
    tpl0 = _mm256_blend_pd(tpl0, trl0, 0x6);
    return tpl0;
}

__m256d _my_unpackhi_pd(__m256i vpl0, __m256i vpl1)
{
    __m256d tpl1 = _mm256_permute2f128_pd((__m256d)vpl0, (__m256d)vpl1, 0x31);
    __m256d trl1 = _mm256_permute_pd(_mm256_permute2f128_pd(tpl1, tpl1, 0x21), 0x5);
    tpl1 = _mm256_blend_pd(tpl1, trl1, 0x6);
    return tpl1;
}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_transpose_key(T& v0, T& v1, T& v2, T& v3, 
                      T& v4, T& v5, T& v6, T& v7,
        __m256i &vpl0 , __m256i &vph0 , __m256i &vpl1 , __m256i &vph1 , 
        __m256i &vpl2 , __m256i &vph2 , __m256i &vpl3 , __m256i &vph3 ,
        __m256i &vpl4 , __m256i &vph4 , __m256i &vpl5 , __m256i &vph5 , 
        __m256i &vpl6 , __m256i &vph6 , __m256i &vpl7 , __m256i &vph7 )
{
    __m256 __t0, __t1, __t2, __t3, __t4, __t5, __t6, __t7;
    __m256 __tt0, __tt1, __tt2, __tt3, __tt4, __tt5, __tt6, __tt7;
    __m256d tpl0, tpl1, tpl2, tpl3, tpl4, tpl5, tpl6, tpl7;
    __m256d tph0, tph1, tph2, tph3, tph4, tph5, tph6, tph7;
    __m256d ttpl0, ttpl1, ttpl2, ttpl3, ttpl4, ttpl5, ttpl6, ttpl7;
    __m256d ttph0, ttph1, ttph2, ttph3, ttph4, ttph5, ttph6, ttph7;
    __t0 = _mm256_unpacklo_ps(v0, v1);
    __t1 = _mm256_unpackhi_ps(v0, v1);
    __t2 = _mm256_unpacklo_ps(v2, v3);
    __t3 = _mm256_unpackhi_ps(v2, v3);
    __t4 = _mm256_unpacklo_ps(v4, v5);
    __t5 = _mm256_unpackhi_ps(v4, v5);
    __t6 = _mm256_unpacklo_ps(v6, v7);
    __t7 = _mm256_unpackhi_ps(v6, v7);

    tpl0 = _my_unpacklo_pd(vpl0, vpl1);
    tph0 = _my_unpacklo_pd(vph0, vph1);
    tpl1 = _my_unpackhi_pd(vpl0, vpl1);
    tph1 = _my_unpackhi_pd(vph0, vph1);
    tpl2 = _my_unpacklo_pd(vpl2, vpl3);
    tph2 = _my_unpacklo_pd(vph2, vph3);
    tpl3 = _my_unpackhi_pd(vpl2, vpl3);
    tph3 = _my_unpackhi_pd(vph2, vph3);
    tpl4 = _my_unpacklo_pd(vpl4, vpl5);
    tph4 = _my_unpacklo_pd(vph4, vph5);
    tpl5 = _my_unpackhi_pd(vpl4, vpl5);
    tph5 = _my_unpackhi_pd(vph4, vph5);
    tpl6 = _my_unpacklo_pd(vpl6, vpl7);
    tph6 = _my_unpacklo_pd(vph6, vph7);
    tpl7 = _my_unpackhi_pd(vpl6, vpl7);
    tph7 = _my_unpackhi_pd(vph6, vph7);

    __tt0 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(1,0,1,0));
    __tt1 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(3,2,3,2));
    __tt2 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(1,0,1,0));
    __tt3 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(3,2,3,2));
    __tt4 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(1,0,1,0));
    __tt5 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(3,2,3,2));
    __tt6 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(1,0,1,0));
    __tt7 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(3,2,3,2));

    ttpl0 = _mm256_permute2f128_pd(tpl0, tpl2, 0x20);
    ttph0 = _mm256_permute2f128_pd(tph0, tph2, 0x20);
    ttpl1 = _mm256_permute2f128_pd(tpl0, tpl2, 0x31);
    ttph1 = _mm256_permute2f128_pd(tph0, tph2, 0x31);
    ttpl2 = _mm256_permute2f128_pd(tpl1, tpl3, 0x20);
    ttph2 = _mm256_permute2f128_pd(tph1, tph3, 0x20);
    ttpl3 = _mm256_permute2f128_pd(tpl1, tpl3, 0x31);
    ttph3 = _mm256_permute2f128_pd(tph1, tph3, 0x31);
    ttpl4 = _mm256_permute2f128_pd(tpl4, tpl6, 0x20);
    ttph4 = _mm256_permute2f128_pd(tph4, tph6, 0x20);
    ttpl5 = _mm256_permute2f128_pd(tpl4, tpl6, 0x31);
    ttph5 = _mm256_permute2f128_pd(tph4, tph6, 0x31);
    ttpl6 = _mm256_permute2f128_pd(tpl5, tpl7, 0x20);
    ttph6 = _mm256_permute2f128_pd(tph5, tph7, 0x20);
    ttpl7 = _mm256_permute2f128_pd(tpl5, tpl7, 0x31);
    ttph7 = _mm256_permute2f128_pd(tph5, tph7, 0x31);

    v0 = _mm256_permute2f128_ps(__tt0, __tt4, 0x20);
    v1 = _mm256_permute2f128_ps(__tt1, __tt5, 0x20);
    v2 = _mm256_permute2f128_ps(__tt2, __tt6, 0x20);
    v3 = _mm256_permute2f128_ps(__tt3, __tt7, 0x20);
    v4 = _mm256_permute2f128_ps(__tt0, __tt4, 0x31);
    v5 = _mm256_permute2f128_ps(__tt1, __tt5, 0x31);
    v6 = _mm256_permute2f128_ps(__tt2, __tt6, 0x31);
    v7 = _mm256_permute2f128_ps(__tt3, __tt7, 0x31);

    vpl0 = (__m256i)ttpl0;
    vph0 = (__m256i)ttpl4;
    vpl1 = (__m256i)ttpl1;
    vph1 = (__m256i)ttpl5;
    vpl2 = (__m256i)ttpl2;
    vph2 = (__m256i)ttpl6;
    vpl3 = (__m256i)ttpl3;
    vph3 = (__m256i)ttpl7;
    vpl4 = (__m256i)ttph0;
    vph4 = (__m256i)ttph4;
    vpl5 = (__m256i)ttph1;
    vph5 = (__m256i)ttph5;
    vpl6 = (__m256i)ttph2;
    vph6 = (__m256i)ttph6;
    vpl7 = (__m256i)ttph3;
    vph7 = (__m256i)ttph7;
}
/**
 * Integer vector version (__m256i):
 * This method performs the in-register transpose. The sorted elements 
 * are stored horizontally within the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored horizontally in the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_transpose(T& v0, T& v1, T& v2, T& v3, 
                      T& v4, T& v5, T& v6, T& v7)
{
    __m256 __t0, __t1, __t2, __t3, __t4, __t5, __t6, __t7;
    __m256 __tt0, __tt1, __tt2, __tt3, __tt4, __tt5, __tt6, __tt7;
    __t0 = _mm256_unpacklo_ps((__m256)v0, (__m256)v1);
    __t1 = _mm256_unpackhi_ps((__m256)v0, (__m256)v1);
    __t2 = _mm256_unpacklo_ps((__m256)v2, (__m256)v3);
    __t3 = _mm256_unpackhi_ps((__m256)v2, (__m256)v3);
    __t4 = _mm256_unpacklo_ps((__m256)v4, (__m256)v5);
    __t5 = _mm256_unpackhi_ps((__m256)v4, (__m256)v5);
    __t6 = _mm256_unpacklo_ps((__m256)v6, (__m256)v7);
    __t7 = _mm256_unpackhi_ps((__m256)v6, (__m256)v7);
    __tt0 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(1,0,1,0));
    __tt1 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(3,2,3,2));
    __tt2 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(1,0,1,0));
    __tt3 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(3,2,3,2));
    __tt4 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(1,0,1,0));
    __tt5 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(3,2,3,2));
    __tt6 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(1,0,1,0));
    __tt7 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(3,2,3,2));
    v0 = (__m256i)_mm256_permute2f128_ps(__tt0, __tt4, 0x20);
    v1 = (__m256i)_mm256_permute2f128_ps(__tt1, __tt5, 0x20);
    v2 = (__m256i)_mm256_permute2f128_ps(__tt2, __tt6, 0x20);
    v3 = (__m256i)_mm256_permute2f128_ps(__tt3, __tt7, 0x20);
    v4 = (__m256i)_mm256_permute2f128_ps(__tt0, __tt4, 0x31);
    v5 = (__m256i)_mm256_permute2f128_ps(__tt1, __tt5, 0x31);
    v6 = (__m256i)_mm256_permute2f128_ps(__tt2, __tt6, 0x31);
    v7 = (__m256i)_mm256_permute2f128_ps(__tt3, __tt7, 0x31);
}

template <typename T> // int key int index
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_transpose_key(T& v0, T& v1, T& v2, T& v3, 
                      T& v4, T& v5, T& v6, T& v7,
        __m256i &vp0, __m256i &vp1, __m256i &vp2, __m256i &vp3, 
        __m256i &vp4, __m256i &vp5, __m256i &vp6, __m256i &vp7)
{
    __m256 __t0, __t1, __t2, __t3, __t4, __t5, __t6, __t7;
    __m256 __tp0, __tp1, __tp2, __tp3, __tp4, __tp5, __tp6, __tp7;
    __m256 __tt0, __tt1, __tt2, __tt3, __tt4, __tt5, __tt6, __tt7;
    __m256 __ttp0, __ttp1, __ttp2, __ttp3, __ttp4, __ttp5, __ttp6, __ttp7;
    __t0 = _mm256_unpacklo_ps((__m256)v0, (__m256)v1);
    __t1 = _mm256_unpackhi_ps((__m256)v0, (__m256)v1);
    __t2 = _mm256_unpacklo_ps((__m256)v2, (__m256)v3);
    __t3 = _mm256_unpackhi_ps((__m256)v2, (__m256)v3);
    __t4 = _mm256_unpacklo_ps((__m256)v4, (__m256)v5);
    __t5 = _mm256_unpackhi_ps((__m256)v4, (__m256)v5);
    __t6 = _mm256_unpacklo_ps((__m256)v6, (__m256)v7);
    __t7 = _mm256_unpackhi_ps((__m256)v6, (__m256)v7);
    __tp0 = _mm256_unpacklo_ps((__m256)vp0, (__m256)vp1);
    __tp1 = _mm256_unpackhi_ps((__m256)vp0, (__m256)vp1);
    __tp2 = _mm256_unpacklo_ps((__m256)vp2, (__m256)vp3);
    __tp3 = _mm256_unpackhi_ps((__m256)vp2, (__m256)vp3);
    __tp4 = _mm256_unpacklo_ps((__m256)vp4, (__m256)vp5);
    __tp5 = _mm256_unpackhi_ps((__m256)vp4, (__m256)vp5);
    __tp6 = _mm256_unpacklo_ps((__m256)vp6, (__m256)vp7);
    __tp7 = _mm256_unpackhi_ps((__m256)vp6, (__m256)vp7);

    __tt0 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(1,0,1,0));
    __tt1 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(3,2,3,2));
    __tt2 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(1,0,1,0));
    __tt3 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(3,2,3,2));
    __tt4 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(1,0,1,0));
    __tt5 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(3,2,3,2));
    __tt6 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(1,0,1,0));
    __tt7 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(3,2,3,2));
    __ttp0 = _mm256_shuffle_ps(__tp0,__tp2,_MM_SHUFFLE(1,0,1,0));
    __ttp1 = _mm256_shuffle_ps(__tp0,__tp2,_MM_SHUFFLE(3,2,3,2));
    __ttp2 = _mm256_shuffle_ps(__tp1,__tp3,_MM_SHUFFLE(1,0,1,0));
    __ttp3 = _mm256_shuffle_ps(__tp1,__tp3,_MM_SHUFFLE(3,2,3,2));
    __ttp4 = _mm256_shuffle_ps(__tp4,__tp6,_MM_SHUFFLE(1,0,1,0));
    __ttp5 = _mm256_shuffle_ps(__tp4,__tp6,_MM_SHUFFLE(3,2,3,2));
    __ttp6 = _mm256_shuffle_ps(__tp5,__tp7,_MM_SHUFFLE(1,0,1,0));
    __ttp7 = _mm256_shuffle_ps(__tp5,__tp7,_MM_SHUFFLE(3,2,3,2));

    v0 = (__m256i)_mm256_permute2f128_ps(__tt0, __tt4, 0x20);
    v1 = (__m256i)_mm256_permute2f128_ps(__tt1, __tt5, 0x20);
    v2 = (__m256i)_mm256_permute2f128_ps(__tt2, __tt6, 0x20);
    v3 = (__m256i)_mm256_permute2f128_ps(__tt3, __tt7, 0x20);
    v4 = (__m256i)_mm256_permute2f128_ps(__tt0, __tt4, 0x31);
    v5 = (__m256i)_mm256_permute2f128_ps(__tt1, __tt5, 0x31);
    v6 = (__m256i)_mm256_permute2f128_ps(__tt2, __tt6, 0x31);
    v7 = (__m256i)_mm256_permute2f128_ps(__tt3, __tt7, 0x31);
    vp0 = (__m256i)_mm256_permute2f128_ps(__ttp0, __ttp4, 0x20);
    vp1 = (__m256i)_mm256_permute2f128_ps(__ttp1, __ttp5, 0x20);
    vp2 = (__m256i)_mm256_permute2f128_ps(__ttp2, __ttp6, 0x20);
    vp3 = (__m256i)_mm256_permute2f128_ps(__ttp3, __ttp7, 0x20);
    vp4 = (__m256i)_mm256_permute2f128_ps(__ttp0, __ttp4, 0x31);
    vp5 = (__m256i)_mm256_permute2f128_ps(__ttp1, __ttp5, 0x31);
    vp6 = (__m256i)_mm256_permute2f128_ps(__ttp2, __ttp6, 0x31);
    vp7 = (__m256i)_mm256_permute2f128_ps(__ttp3, __ttp7, 0x31);
}

template <typename T> // int key long index
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_transpose_key(T& v0, T& v1, T& v2, T& v3, 
                      T& v4, T& v5, T& v6, T& v7,
        __m256i &vpl0 , __m256i &vph0 , __m256i &vpl1 , __m256i &vph1 , 
        __m256i &vpl2 , __m256i &vph2 , __m256i &vpl3 , __m256i &vph3 ,
        __m256i &vpl4 , __m256i &vph4 , __m256i &vpl5 , __m256i &vph5 , 
        __m256i &vpl6 , __m256i &vph6 , __m256i &vpl7 , __m256i &vph7 )
{
    __m256 __t0, __t1, __t2, __t3, __t4, __t5, __t6, __t7;
    __m256 __tt0, __tt1, __tt2, __tt3, __tt4, __tt5, __tt6, __tt7;
    __m256d tpl0, tpl1, tpl2, tpl3, tpl4, tpl5, tpl6, tpl7;
    __m256d tph0, tph1, tph2, tph3, tph4, tph5, tph6, tph7;
    __m256d ttpl0, ttpl1, ttpl2, ttpl3, ttpl4, ttpl5, ttpl6, ttpl7;
    __m256d ttph0, ttph1, ttph2, ttph3, ttph4, ttph5, ttph6, ttph7;
    __t0 = _mm256_unpacklo_ps((__m256)v0, (__m256)v1);
    __t1 = _mm256_unpackhi_ps((__m256)v0, (__m256)v1);
    __t2 = _mm256_unpacklo_ps((__m256)v2, (__m256)v3);
    __t3 = _mm256_unpackhi_ps((__m256)v2, (__m256)v3);
    __t4 = _mm256_unpacklo_ps((__m256)v4, (__m256)v5);
    __t5 = _mm256_unpackhi_ps((__m256)v4, (__m256)v5);
    __t6 = _mm256_unpacklo_ps((__m256)v6, (__m256)v7);
    __t7 = _mm256_unpackhi_ps((__m256)v6, (__m256)v7);
    tpl0 = _my_unpacklo_pd(vpl0, vpl1);
    tph0 = _my_unpacklo_pd(vph0, vph1);
    tpl1 = _my_unpackhi_pd(vpl0, vpl1);
    tph1 = _my_unpackhi_pd(vph0, vph1);
    tpl2 = _my_unpacklo_pd(vpl2, vpl3);
    tph2 = _my_unpacklo_pd(vph2, vph3);
    tpl3 = _my_unpackhi_pd(vpl2, vpl3);
    tph3 = _my_unpackhi_pd(vph2, vph3);
    tpl4 = _my_unpacklo_pd(vpl4, vpl5);
    tph4 = _my_unpacklo_pd(vph4, vph5);
    tpl5 = _my_unpackhi_pd(vpl4, vpl5);
    tph5 = _my_unpackhi_pd(vph4, vph5);
    tpl6 = _my_unpacklo_pd(vpl6, vpl7);
    tph6 = _my_unpacklo_pd(vph6, vph7);
    tpl7 = _my_unpackhi_pd(vpl6, vpl7);
    tph7 = _my_unpackhi_pd(vph6, vph7);

    __tt0 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(1,0,1,0));
    __tt1 = _mm256_shuffle_ps(__t0,__t2,_MM_SHUFFLE(3,2,3,2));
    __tt2 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(1,0,1,0));
    __tt3 = _mm256_shuffle_ps(__t1,__t3,_MM_SHUFFLE(3,2,3,2));
    __tt4 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(1,0,1,0));
    __tt5 = _mm256_shuffle_ps(__t4,__t6,_MM_SHUFFLE(3,2,3,2));
    __tt6 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(1,0,1,0));
    __tt7 = _mm256_shuffle_ps(__t5,__t7,_MM_SHUFFLE(3,2,3,2));
    ttpl0 = _mm256_permute2f128_pd(tpl0, tpl2, 0x20);
    ttph0 = _mm256_permute2f128_pd(tph0, tph2, 0x20);
    ttpl1 = _mm256_permute2f128_pd(tpl0, tpl2, 0x31);
    ttph1 = _mm256_permute2f128_pd(tph0, tph2, 0x31);
    ttpl2 = _mm256_permute2f128_pd(tpl1, tpl3, 0x20);
    ttph2 = _mm256_permute2f128_pd(tph1, tph3, 0x20);
    ttpl3 = _mm256_permute2f128_pd(tpl1, tpl3, 0x31);
    ttph3 = _mm256_permute2f128_pd(tph1, tph3, 0x31);
    ttpl4 = _mm256_permute2f128_pd(tpl4, tpl6, 0x20);
    ttph4 = _mm256_permute2f128_pd(tph4, tph6, 0x20);
    ttpl5 = _mm256_permute2f128_pd(tpl4, tpl6, 0x31);
    ttph5 = _mm256_permute2f128_pd(tph4, tph6, 0x31);
    ttpl6 = _mm256_permute2f128_pd(tpl5, tpl7, 0x20);
    ttph6 = _mm256_permute2f128_pd(tph5, tph7, 0x20);
    ttpl7 = _mm256_permute2f128_pd(tpl5, tpl7, 0x31);
    ttph7 = _mm256_permute2f128_pd(tph5, tph7, 0x31);

    v0 = (__m256i)_mm256_permute2f128_ps(__tt0, __tt4, 0x20);
    v1 = (__m256i)_mm256_permute2f128_ps(__tt1, __tt5, 0x20);
    v2 = (__m256i)_mm256_permute2f128_ps(__tt2, __tt6, 0x20);
    v3 = (__m256i)_mm256_permute2f128_ps(__tt3, __tt7, 0x20);
    v4 = (__m256i)_mm256_permute2f128_ps(__tt0, __tt4, 0x31);
    v5 = (__m256i)_mm256_permute2f128_ps(__tt1, __tt5, 0x31);
    v6 = (__m256i)_mm256_permute2f128_ps(__tt2, __tt6, 0x31);
    v7 = (__m256i)_mm256_permute2f128_ps(__tt3, __tt7, 0x31);

    vpl0 = (__m256i)ttpl0;
    vph0 = (__m256i)ttpl4;
    vpl1 = (__m256i)ttpl1;
    vph1 = (__m256i)ttpl5;
    vpl2 = (__m256i)ttpl2;
    vph2 = (__m256i)ttpl6;
    vpl3 = (__m256i)ttpl3;
    vph3 = (__m256i)ttpl7;
    vpl4 = (__m256i)ttph0;
    vph4 = (__m256i)ttph4;
    vpl5 = (__m256i)ttph1;
    vph5 = (__m256i)ttph5;
    vpl6 = (__m256i)ttph2;
    vph6 = (__m256i)ttph6;
    vpl7 = (__m256i)ttph3;
    vph7 = (__m256i)ttph7;
}
/**
 * Double vector version (__m256d):
 * This method performs the in-register transpose. The sorted elements 
 * are stored horizontally within the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored horizontally in the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_transpose(T& v0, T& v1, T& v2, T& v3)
{
    __m256d __t0, __t1, __t2, __t3;
    __t0 = _mm256_unpacklo_pd(v0, v1);
    __t1 = _mm256_unpackhi_pd(v0, v1);
    __t2 = _mm256_unpacklo_pd(v2, v3);
    __t3 = _mm256_unpackhi_pd(v2, v3);
    v0 = _mm256_permute2f128_pd(__t0, __t2, 0x20);
    v1 = _mm256_permute2f128_pd(__t1, __t3, 0x20);
    v2 = _mm256_permute2f128_pd(__t0, __t2, 0x31);
    v3 = _mm256_permute2f128_pd(__t1, __t3, 0x31);
}

template <typename T> // double key int index
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_transpose_key(T& v0, T& v1, T& v2, T& v3,
        __m256i &vp0, __m256i &vp1)
{
    __m256d __t0, __t1, __t2, __t3;
    __m128 p0 = (__m128)_mm256_castps256_ps128((__m256)vp0);
    __m128 p1 = (__m128)_mm256_castps256_ps128(_mm256_permute2f128_ps((__m256)vp0, (__m256)vp0, 0x01));
    __m128 p2 = (__m128)_mm256_castps256_ps128((__m256)vp1);
    __m128 p3 = (__m128)_mm256_castps256_ps128(_mm256_permute2f128_ps((__m256)vp1, (__m256)vp1, 0x01));
    __m128 __tp0, __tp1, __tp2, __tp3;
    __m128 ttp0, ttp1, ttp2, ttp3;

    __t0 = _mm256_unpacklo_pd(v0, v1);
    __t1 = _mm256_unpackhi_pd(v0, v1);
    __t2 = _mm256_unpacklo_pd(v2, v3);
    __t3 = _mm256_unpackhi_pd(v2, v3);
    ttp0 = _mm_shuffle_ps(p0, p1, 0x88);
    ttp1 = _mm_shuffle_ps(p0, p1, 0xdd);
    ttp2 = _mm_shuffle_ps(p2, p3, 0x88);
    ttp3 = _mm_shuffle_ps(p2, p3, 0xdd);
    __tp0 = _mm_shuffle_ps(ttp0, ttp0, 0xd8);
    __tp1 = _mm_shuffle_ps(ttp1, ttp1, 0xd8);
    __tp2 = _mm_shuffle_ps(ttp2, ttp2, 0xd8);
    __tp3 = _mm_shuffle_ps(ttp3, ttp3, 0xd8);

    v0 = _mm256_permute2f128_pd(__t0, __t2, 0x20);
    v1 = _mm256_permute2f128_pd(__t1, __t3, 0x20);
    v2 = _mm256_permute2f128_pd(__t0, __t2, 0x31);
    v3 = _mm256_permute2f128_pd(__t1, __t3, 0x31);

    p0 = _mm_shuffle_ps(__tp0, __tp2, 0x44);
    p1 = _mm_shuffle_ps(__tp1, __tp3, 0x44);
    p2 = _mm_shuffle_ps(__tp0, __tp2, 0xee);
    p3 = _mm_shuffle_ps(__tp1, __tp3, 0xee);

    vp0 = _mm256_insertf128_si256(vp0, (__m128i)p0, 0);
    vp0 = _mm256_insertf128_si256(vp0, (__m128i)p1, 1);
    vp1 = _mm256_insertf128_si256(vp1, (__m128i)p2, 0);
    vp1 = _mm256_insertf128_si256(vp1, (__m128i)p3, 1);

}

template <typename T> // double key long index
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_transpose_key(T& v0, T& v1, T& v2, T& v3,
        __m256i &vp0, __m256i &vp1, __m256i &vp2, __m256i &vp3)

{
    __m256d __t0, __t1, __t2, __t3;
    __m256d __tp0, __tp1, __tp2, __tp3;
    __t0 = _mm256_unpacklo_pd(v0, v1);
    __t1 = _mm256_unpackhi_pd(v0, v1);
    __t2 = _mm256_unpacklo_pd(v2, v3);
    __t3 = _mm256_unpackhi_pd(v2, v3);
    __tp0 = _mm256_unpacklo_pd((__m256d)vp0, (__m256d)vp1);
    __tp1 = _mm256_unpackhi_pd((__m256d)vp0, (__m256d)vp1);
    __tp2 = _mm256_unpacklo_pd((__m256d)vp2, (__m256d)vp3);
    __tp3 = _mm256_unpackhi_pd((__m256d)vp2, (__m256d)vp3);

    v0 = _mm256_permute2f128_pd(__t0, __t2, 0x20);
    v1 = _mm256_permute2f128_pd(__t1, __t3, 0x20);
    v2 = _mm256_permute2f128_pd(__t0, __t2, 0x31);
    v3 = _mm256_permute2f128_pd(__t1, __t3, 0x31);
    vp0 = (__m256i)_mm256_permute2f128_pd(__tp0, __tp2, 0x20);
    vp1 = (__m256i)_mm256_permute2f128_pd(__tp1, __tp3, 0x20);
    vp2 = (__m256i)_mm256_permute2f128_pd(__tp0, __tp2, 0x31);
    vp3 = (__m256i)_mm256_permute2f128_pd(__tp1, __tp3, 0x31);
}

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
sorter(T*& orig, uint32_t size)
{
    uint32_t i, j;
    __m256i vec0;
    __m256i vec1;
    __m256i vec2;
    __m256i vec3;
    __m256i vec4;
    __m256i vec5;
    __m256i vec6;
    __m256i vec7;
    uint8_t stride = (uint8_t)simd_width::AVX_INT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_si256((__m256i *)(orig + i + 0 * stride));
        vec1 = _mm256_loadu_si256((__m256i *)(orig + i + 1 * stride));
        vec2 = _mm256_loadu_si256((__m256i *)(orig + i + 2 * stride));
        vec3 = _mm256_loadu_si256((__m256i *)(orig + i + 3 * stride));
        vec4 = _mm256_loadu_si256((__m256i *)(orig + i + 4 * stride));
        vec5 = _mm256_loadu_si256((__m256i *)(orig + i + 5 * stride));
        vec6 = _mm256_loadu_si256((__m256i *)(orig + i + 6 * stride));
        vec7 = _mm256_loadu_si256((__m256i *)(orig + i + 7 * stride));

        in_register_sort(vec0, vec1, vec2, vec3, 
                         vec4, vec5, vec6, vec7);

        in_register_transpose(vec0, vec1, vec2, vec3, 
                              vec4, vec5, vec6, vec7);

        _mm256_storeu_si256((__m256i *)(orig + i + 0 * stride), vec0);
        _mm256_storeu_si256((__m256i *)(orig + i + 1 * stride), vec1);
        _mm256_storeu_si256((__m256i *)(orig + i + 2 * stride), vec2);
        _mm256_storeu_si256((__m256i *)(orig + i + 3 * stride), vec3);
        _mm256_storeu_si256((__m256i *)(orig + i + 4 * stride), vec4);
        _mm256_storeu_si256((__m256i *)(orig + i + 5 * stride), vec5);
        _mm256_storeu_si256((__m256i *)(orig + i + 6 * stride), vec6);
        _mm256_storeu_si256((__m256i *)(orig + i + 7 * stride), vec7);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap(orig, i  , i+1);
        swap(orig, i+2, i+3);
        swap(orig, i+4, i+5);
        swap(orig, i+6, i+7);
        swap(orig, i  , i+2);
        swap(orig, i+1, i+3);
        swap(orig, i+4, i+6);
        swap(orig, i+5, i+7);
        swap(orig, i+1, i+2);
        swap(orig, i+5, i+6);
        swap(orig, i  , i+4);
        swap(orig, i+1, i+5);
        swap(orig, i+2, i+6);
        swap(orig, i+3, i+7);
        swap(orig, i+2, i+4);
        swap(orig, i+3, i+5);
        swap(orig, i+1, i+2);
        swap(orig, i+3, i+4);
        swap(orig, i+5, i+6);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap(orig, i, j);
        }
    }
}

template <typename T> // int key int index
typename std::enable_if<std::is_same<T, int>::value>::type
sorter_key(T*& orig, int *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m256i vec0;
    __m256i vec1;
    __m256i vec2;
    __m256i vec3;
    __m256i vec4;
    __m256i vec5;
    __m256i vec6;
    __m256i vec7;

    __m256i p0 ;
    __m256i p1 ;
    __m256i p2 ;
    __m256i p3 ;
    __m256i p4 ;
    __m256i p5 ;
    __m256i p6 ;
    __m256i p7 ;
    uint8_t stride = (uint8_t)simd_width::AVX_INT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_si256((__m256i *)(orig + i + 0 * stride));
        vec1 = _mm256_loadu_si256((__m256i *)(orig + i + 1 * stride));
        vec2 = _mm256_loadu_si256((__m256i *)(orig + i + 2 * stride));
        vec3 = _mm256_loadu_si256((__m256i *)(orig + i + 3 * stride));
        vec4 = _mm256_loadu_si256((__m256i *)(orig + i + 4 * stride));
        vec5 = _mm256_loadu_si256((__m256i *)(orig + i + 5 * stride));
        vec6 = _mm256_loadu_si256((__m256i *)(orig + i + 6 * stride));
        vec7 = _mm256_loadu_si256((__m256i *)(orig + i + 7 * stride));

        p0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride));
        p1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride));
        p2 = _mm256_loadu_si256((__m256i *)(ptr + i + 2 * stride));
        p3 = _mm256_loadu_si256((__m256i *)(ptr + i + 3 * stride));
        p4 = _mm256_loadu_si256((__m256i *)(ptr + i + 4 * stride));
        p5 = _mm256_loadu_si256((__m256i *)(ptr + i + 5 * stride));
        p6 = _mm256_loadu_si256((__m256i *)(ptr + i + 6 * stride));
        p7 = _mm256_loadu_si256((__m256i *)(ptr + i + 7 * stride));

        in_register_sort_key(vec0, vec1, vec2, vec3, 
                         vec4, vec5, vec6, vec7, 
                         p0, p1, p2, p3, 
                         p4, p5, p6, p7);

        in_register_transpose_key(vec0, vec1, vec2, vec3, 
                                  vec4, vec5, vec6, vec7,
                                  p0, p1, p2, p3, 
                                  p4, p5, p6, p7);

        _mm256_storeu_si256((__m256i *)(orig + i + 0 * stride), vec0);
        _mm256_storeu_si256((__m256i *)(orig + i + 1 * stride), vec1);
        _mm256_storeu_si256((__m256i *)(orig + i + 2 * stride), vec2);
        _mm256_storeu_si256((__m256i *)(orig + i + 3 * stride), vec3);
        _mm256_storeu_si256((__m256i *)(orig + i + 4 * stride), vec4);
        _mm256_storeu_si256((__m256i *)(orig + i + 5 * stride), vec5);
        _mm256_storeu_si256((__m256i *)(orig + i + 6 * stride), vec6);
        _mm256_storeu_si256((__m256i *)(orig + i + 7 * stride), vec7);

        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride), p0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride), p1);
        _mm256_storeu_si256((__m256i *)(ptr + i + 2 * stride), p2);
        _mm256_storeu_si256((__m256i *)(ptr + i + 3 * stride), p3);
        _mm256_storeu_si256((__m256i *)(ptr + i + 4 * stride), p4);
        _mm256_storeu_si256((__m256i *)(ptr + i + 5 * stride), p5);
        _mm256_storeu_si256((__m256i *)(ptr + i + 6 * stride), p6);
        _mm256_storeu_si256((__m256i *)(ptr + i + 7 * stride), p7);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i  , i+1);
        swap_key(orig, ptr, i+2, i+3);
        swap_key(orig, ptr, i+4, i+5);
        swap_key(orig, ptr, i+6, i+7);
        swap_key(orig, ptr, i  , i+2);
        swap_key(orig, ptr, i+1, i+3);
        swap_key(orig, ptr, i+4, i+6);
        swap_key(orig, ptr, i+5, i+7);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+5, i+6);
        swap_key(orig, ptr, i  , i+4);
        swap_key(orig, ptr, i+1, i+5);
        swap_key(orig, ptr, i+2, i+6);
        swap_key(orig, ptr, i+3, i+7);
        swap_key(orig, ptr, i+2, i+4);
        swap_key(orig, ptr, i+3, i+5);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+3, i+4);
        swap_key(orig, ptr, i+5, i+6);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap_key(orig, ptr, i, j);
        }
    }
}

template <typename T> // int key long index
typename std::enable_if<std::is_same<T, int>::value>::type
sorter_key(T*& orig, long *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m256i vec0;
    __m256i vec1;
    __m256i vec2;
    __m256i vec3;
    __m256i vec4;
    __m256i vec5;
    __m256i vec6;
    __m256i vec7;

    __m256i pl0 ;
    __m256i pl1 ;
    __m256i pl2 ;
    __m256i pl3 ;
    __m256i pl4 ;
    __m256i pl5 ;
    __m256i pl6 ;
    __m256i pl7 ;

    __m256i ph0 ;
    __m256i ph1 ;
    __m256i ph2 ;
    __m256i ph3 ;
    __m256i ph4 ;
    __m256i ph5 ;
    __m256i ph6 ;
    __m256i ph7 ;
    uint8_t stride = (uint8_t)simd_width::AVX_INT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_si256((__m256i *)(orig + i + 0 * stride));
        vec1 = _mm256_loadu_si256((__m256i *)(orig + i + 1 * stride));
        vec2 = _mm256_loadu_si256((__m256i *)(orig + i + 2 * stride));
        vec3 = _mm256_loadu_si256((__m256i *)(orig + i + 3 * stride));
        vec4 = _mm256_loadu_si256((__m256i *)(orig + i + 4 * stride));
        vec5 = _mm256_loadu_si256((__m256i *)(orig + i + 5 * stride));
        vec6 = _mm256_loadu_si256((__m256i *)(orig + i + 6 * stride));
        vec7 = _mm256_loadu_si256((__m256i *)(orig + i + 7 * stride));

        pl0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride    ));
        ph0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride + 4));
        pl1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride    ));
        ph1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride + 4));
        pl2 = _mm256_loadu_si256((__m256i *)(ptr + i + 2 * stride    ));
        ph2 = _mm256_loadu_si256((__m256i *)(ptr + i + 2 * stride + 4));
        pl3 = _mm256_loadu_si256((__m256i *)(ptr + i + 3 * stride    ));
        ph3 = _mm256_loadu_si256((__m256i *)(ptr + i + 3 * stride + 4));
        pl4 = _mm256_loadu_si256((__m256i *)(ptr + i + 4 * stride    ));
        ph4 = _mm256_loadu_si256((__m256i *)(ptr + i + 4 * stride + 4));
        pl5 = _mm256_loadu_si256((__m256i *)(ptr + i + 5 * stride    ));
        ph5 = _mm256_loadu_si256((__m256i *)(ptr + i + 5 * stride + 4));
        pl6 = _mm256_loadu_si256((__m256i *)(ptr + i + 6 * stride    ));
        ph6 = _mm256_loadu_si256((__m256i *)(ptr + i + 6 * stride + 4));
        pl7 = _mm256_loadu_si256((__m256i *)(ptr + i + 7 * stride    ));
        ph7 = _mm256_loadu_si256((__m256i *)(ptr + i + 7 * stride + 4));

        in_register_sort_key(vec0, vec1, vec2, vec3, 
                         vec4, vec5, vec6, vec7, 
                         pl0, ph0, pl1, ph1,  
                         pl2, ph2, pl3, ph3, 
                         pl4, ph4, pl5, ph5, 
                         pl6, ph6, pl7, ph7);

        in_register_transpose_key(vec0, vec1, vec2, vec3, 
                                  vec4, vec5, vec6, vec7,
                                  pl0, ph0, pl1, ph1,  
                                  pl2, ph2, pl3, ph3, 
                                  pl4, ph4, pl5, ph5, 
                                  pl6, ph6, pl7, ph7);

        _mm256_storeu_si256((__m256i *)(orig + i + 0 * stride), vec0);
        _mm256_storeu_si256((__m256i *)(orig + i + 1 * stride), vec1);
        _mm256_storeu_si256((__m256i *)(orig + i + 2 * stride), vec2);
        _mm256_storeu_si256((__m256i *)(orig + i + 3 * stride), vec3);
        _mm256_storeu_si256((__m256i *)(orig + i + 4 * stride), vec4);
        _mm256_storeu_si256((__m256i *)(orig + i + 5 * stride), vec5);
        _mm256_storeu_si256((__m256i *)(orig + i + 6 * stride), vec6);
        _mm256_storeu_si256((__m256i *)(orig + i + 7 * stride), vec7);

        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride    ), pl0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride + 4), ph0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride    ), pl1);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride + 4), ph1);
        _mm256_storeu_si256((__m256i *)(ptr + i + 2 * stride    ), pl2);
        _mm256_storeu_si256((__m256i *)(ptr + i + 2 * stride + 4), ph2);
        _mm256_storeu_si256((__m256i *)(ptr + i + 3 * stride    ), pl3);
        _mm256_storeu_si256((__m256i *)(ptr + i + 3 * stride + 4), ph3);
        _mm256_storeu_si256((__m256i *)(ptr + i + 4 * stride    ), pl4);
        _mm256_storeu_si256((__m256i *)(ptr + i + 4 * stride + 4), ph4);
        _mm256_storeu_si256((__m256i *)(ptr + i + 5 * stride    ), pl5);
        _mm256_storeu_si256((__m256i *)(ptr + i + 5 * stride + 4), ph5);
        _mm256_storeu_si256((__m256i *)(ptr + i + 6 * stride    ), pl6);
        _mm256_storeu_si256((__m256i *)(ptr + i + 6 * stride + 4), ph6);
        _mm256_storeu_si256((__m256i *)(ptr + i + 7 * stride    ), pl7);
        _mm256_storeu_si256((__m256i *)(ptr + i + 7 * stride + 4), ph7);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i  , i+1);
        swap_key(orig, ptr, i+2, i+3);
        swap_key(orig, ptr, i+4, i+5);
        swap_key(orig, ptr, i+6, i+7);
        swap_key(orig, ptr, i  , i+2);
        swap_key(orig, ptr, i+1, i+3);
        swap_key(orig, ptr, i+4, i+6);
        swap_key(orig, ptr, i+5, i+7);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+5, i+6);
        swap_key(orig, ptr, i  , i+4);
        swap_key(orig, ptr, i+1, i+5);
        swap_key(orig, ptr, i+2, i+6);
        swap_key(orig, ptr, i+3, i+7);
        swap_key(orig, ptr, i+2, i+4);
        swap_key(orig, ptr, i+3, i+5);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+3, i+4);
        swap_key(orig, ptr, i+5, i+6);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap_key(orig, ptr, i, j);
        }
    }
}

template <typename T>
typename std::enable_if<std::is_same<T, float>::value>::type
sorter(T*& orig, uint32_t size)
{
    uint32_t i, j;
    __m256 vec0;
    __m256 vec1;
    __m256 vec2;
    __m256 vec3;
    __m256 vec4;
    __m256 vec5;
    __m256 vec6;
    __m256 vec7;
    uint8_t stride = (uint8_t)simd_width::AVX_FLOAT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_ps((orig + i + 0 * stride));
        vec1 = _mm256_loadu_ps((orig + i + 1 * stride));
        vec2 = _mm256_loadu_ps((orig + i + 2 * stride));
        vec3 = _mm256_loadu_ps((orig + i + 3 * stride));
        vec4 = _mm256_loadu_ps((orig + i + 4 * stride));
        vec5 = _mm256_loadu_ps((orig + i + 5 * stride));
        vec6 = _mm256_loadu_ps((orig + i + 6 * stride));
        vec7 = _mm256_loadu_ps((orig + i + 7 * stride));

        in_register_sort(vec0, vec1, vec2, vec3, 
                         vec4, vec5, vec6, vec7);

        in_register_transpose(vec0, vec1, vec2, vec3, 
                              vec4, vec5, vec6, vec7);

        _mm256_storeu_ps((orig + i + 0 * stride), vec0);
        _mm256_storeu_ps((orig + i + 1 * stride), vec1);
        _mm256_storeu_ps((orig + i + 2 * stride), vec2);
        _mm256_storeu_ps((orig + i + 3 * stride), vec3);
        _mm256_storeu_ps((orig + i + 4 * stride), vec4);
        _mm256_storeu_ps((orig + i + 5 * stride), vec5);
        _mm256_storeu_ps((orig + i + 6 * stride), vec6);
        _mm256_storeu_ps((orig + i + 7 * stride), vec7);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap(orig, i  , i+1);
        swap(orig, i+2, i+3);
        swap(orig, i+4, i+5);
        swap(orig, i+6, i+7);
        swap(orig, i  , i+2);
        swap(orig, i+1, i+3);
        swap(orig, i+4, i+6);
        swap(orig, i+5, i+7);
        swap(orig, i+1, i+2);
        swap(orig, i+5, i+6);
        swap(orig, i  , i+4);
        swap(orig, i+1, i+5);
        swap(orig, i+2, i+6);
        swap(orig, i+3, i+7);
        swap(orig, i+2, i+4);
        swap(orig, i+3, i+5);
        swap(orig, i+1, i+2);
        swap(orig, i+3, i+4);
        swap(orig, i+5, i+6);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap(orig, i, j);
        }
    }
}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, float>::value>::type
sorter_key(T*& orig, int *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m256 vec0;
    __m256 vec1;
    __m256 vec2;
    __m256 vec3;
    __m256 vec4;
    __m256 vec5;
    __m256 vec6;
    __m256 vec7;

    __m256i p0 ;
    __m256i p1 ;
    __m256i p2 ;
    __m256i p3 ;
    __m256i p4 ;
    __m256i p5 ;
    __m256i p6 ;
    __m256i p7 ;
    uint8_t stride = (uint8_t)simd_width::AVX_FLOAT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_ps((orig + i + 0 * stride));
        vec1 = _mm256_loadu_ps((orig + i + 1 * stride));
        vec2 = _mm256_loadu_ps((orig + i + 2 * stride));
        vec3 = _mm256_loadu_ps((orig + i + 3 * stride));
        vec4 = _mm256_loadu_ps((orig + i + 4 * stride));
        vec5 = _mm256_loadu_ps((orig + i + 5 * stride));
        vec6 = _mm256_loadu_ps((orig + i + 6 * stride));
        vec7 = _mm256_loadu_ps((orig + i + 7 * stride));

        p0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride));
        p1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride));
        p2 = _mm256_loadu_si256((__m256i *)(ptr + i + 2 * stride));
        p3 = _mm256_loadu_si256((__m256i *)(ptr + i + 3 * stride));
        p4 = _mm256_loadu_si256((__m256i *)(ptr + i + 4 * stride));
        p5 = _mm256_loadu_si256((__m256i *)(ptr + i + 5 * stride));
        p6 = _mm256_loadu_si256((__m256i *)(ptr + i + 6 * stride));
        p7 = _mm256_loadu_si256((__m256i *)(ptr + i + 7 * stride));

        in_register_sort_key(vec0, vec1, vec2, vec3, 
                         vec4, vec5, vec6, vec7,
                         p0, p1, p2, p3, 
                         p4, p5, p6, p7);

        in_register_transpose_key(vec0, vec1, vec2, vec3, 
                              vec4, vec5, vec6, vec7,
                              p0, p1, p2, p3, 
                              p4, p5, p6, p7);

        _mm256_storeu_ps((orig + i + 0 * stride), vec0);
        _mm256_storeu_ps((orig + i + 1 * stride), vec1);
        _mm256_storeu_ps((orig + i + 2 * stride), vec2);
        _mm256_storeu_ps((orig + i + 3 * stride), vec3);
        _mm256_storeu_ps((orig + i + 4 * stride), vec4);
        _mm256_storeu_ps((orig + i + 5 * stride), vec5);
        _mm256_storeu_ps((orig + i + 6 * stride), vec6);
        _mm256_storeu_ps((orig + i + 7 * stride), vec7);

        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride), p0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride), p1);
        _mm256_storeu_si256((__m256i *)(ptr + i + 2 * stride), p2);
        _mm256_storeu_si256((__m256i *)(ptr + i + 3 * stride), p3);
        _mm256_storeu_si256((__m256i *)(ptr + i + 4 * stride), p4);
        _mm256_storeu_si256((__m256i *)(ptr + i + 5 * stride), p5);
        _mm256_storeu_si256((__m256i *)(ptr + i + 6 * stride), p6);
        _mm256_storeu_si256((__m256i *)(ptr + i + 7 * stride), p7);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i  , i+1);
        swap_key(orig, ptr, i+2, i+3);
        swap_key(orig, ptr, i+4, i+5);
        swap_key(orig, ptr, i+6, i+7);
        swap_key(orig, ptr, i  , i+2);
        swap_key(orig, ptr, i+1, i+3);
        swap_key(orig, ptr, i+4, i+6);
        swap_key(orig, ptr, i+5, i+7);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+5, i+6);
        swap_key(orig, ptr, i  , i+4);
        swap_key(orig, ptr, i+1, i+5);
        swap_key(orig, ptr, i+2, i+6);
        swap_key(orig, ptr, i+3, i+7);
        swap_key(orig, ptr, i+2, i+4);
        swap_key(orig, ptr, i+3, i+5);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+3, i+4);
        swap_key(orig, ptr, i+5, i+6);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap_key(orig, ptr, i, j);
        }
    }
}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, float>::value>::type
sorter_key(T*& orig, long *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m256 vec0;
    __m256 vec1;
    __m256 vec2;
    __m256 vec3;
    __m256 vec4;
    __m256 vec5;
    __m256 vec6;
    __m256 vec7;

    __m256i pl0 ;
    __m256i pl1 ;
    __m256i pl2 ;
    __m256i pl3 ;
    __m256i pl4 ;
    __m256i pl5 ;
    __m256i pl6 ;
    __m256i pl7 ;

    __m256i ph0 ;
    __m256i ph1 ;
    __m256i ph2 ;
    __m256i ph3 ;
    __m256i ph4 ;
    __m256i ph5 ;
    __m256i ph6 ;
    __m256i ph7 ;
    uint8_t stride = (uint8_t)simd_width::AVX_FLOAT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_ps((orig + i + 0 * stride));
        vec1 = _mm256_loadu_ps((orig + i + 1 * stride));
        vec2 = _mm256_loadu_ps((orig + i + 2 * stride));
        vec3 = _mm256_loadu_ps((orig + i + 3 * stride));
        vec4 = _mm256_loadu_ps((orig + i + 4 * stride));
        vec5 = _mm256_loadu_ps((orig + i + 5 * stride));
        vec6 = _mm256_loadu_ps((orig + i + 6 * stride));
        vec7 = _mm256_loadu_ps((orig + i + 7 * stride));

        pl0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride    ));
        ph0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride + 4));
        pl1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride    ));
        ph1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride + 4));
        pl2 = _mm256_loadu_si256((__m256i *)(ptr + i + 2 * stride    ));
        ph2 = _mm256_loadu_si256((__m256i *)(ptr + i + 2 * stride + 4));
        pl3 = _mm256_loadu_si256((__m256i *)(ptr + i + 3 * stride    ));
        ph3 = _mm256_loadu_si256((__m256i *)(ptr + i + 3 * stride + 4));
        pl4 = _mm256_loadu_si256((__m256i *)(ptr + i + 4 * stride    ));
        ph4 = _mm256_loadu_si256((__m256i *)(ptr + i + 4 * stride + 4));
        pl5 = _mm256_loadu_si256((__m256i *)(ptr + i + 5 * stride    ));
        ph5 = _mm256_loadu_si256((__m256i *)(ptr + i + 5 * stride + 4));
        pl6 = _mm256_loadu_si256((__m256i *)(ptr + i + 6 * stride    ));
        ph6 = _mm256_loadu_si256((__m256i *)(ptr + i + 6 * stride + 4));
        pl7 = _mm256_loadu_si256((__m256i *)(ptr + i + 7 * stride    ));
        ph7 = _mm256_loadu_si256((__m256i *)(ptr + i + 7 * stride + 4));

        in_register_sort_key(vec0, vec1, vec2, vec3, 
                         vec4, vec5, vec6, vec7,
                         pl0, ph0, pl1, ph1,  
                         pl2, ph2, pl3, ph3, 
                         pl4, ph4, pl5, ph5, 
                         pl6, ph6, pl7, ph7);

        in_register_transpose_key(vec0, vec1, vec2, vec3, 
                              vec4, vec5, vec6, vec7,
                              pl0, ph0, pl1, ph1,  
                              pl2, ph2, pl3, ph3, 
                              pl4, ph4, pl5, ph5, 
                              pl6, ph6, pl7, ph7);

        _mm256_storeu_ps((orig + i + 0 * stride), vec0);
        _mm256_storeu_ps((orig + i + 1 * stride), vec1);
        _mm256_storeu_ps((orig + i + 2 * stride), vec2);
        _mm256_storeu_ps((orig + i + 3 * stride), vec3);
        _mm256_storeu_ps((orig + i + 4 * stride), vec4);
        _mm256_storeu_ps((orig + i + 5 * stride), vec5);
        _mm256_storeu_ps((orig + i + 6 * stride), vec6);
        _mm256_storeu_ps((orig + i + 7 * stride), vec7);

        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride    ), pl0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride + 4), ph0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride    ), pl1);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride + 4), ph1);
        _mm256_storeu_si256((__m256i *)(ptr + i + 2 * stride    ), pl2);
        _mm256_storeu_si256((__m256i *)(ptr + i + 2 * stride + 4), ph2);
        _mm256_storeu_si256((__m256i *)(ptr + i + 3 * stride    ), pl3);
        _mm256_storeu_si256((__m256i *)(ptr + i + 3 * stride + 4), ph3);
        _mm256_storeu_si256((__m256i *)(ptr + i + 4 * stride    ), pl4);
        _mm256_storeu_si256((__m256i *)(ptr + i + 4 * stride + 4), ph4);
        _mm256_storeu_si256((__m256i *)(ptr + i + 5 * stride    ), pl5);
        _mm256_storeu_si256((__m256i *)(ptr + i + 5 * stride + 4), ph5);
        _mm256_storeu_si256((__m256i *)(ptr + i + 6 * stride    ), pl6);
        _mm256_storeu_si256((__m256i *)(ptr + i + 6 * stride + 4), ph6);
        _mm256_storeu_si256((__m256i *)(ptr + i + 7 * stride    ), pl7);
        _mm256_storeu_si256((__m256i *)(ptr + i + 7 * stride + 4), ph7);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i  , i+1);
        swap_key(orig, ptr, i+2, i+3);
        swap_key(orig, ptr, i+4, i+5);
        swap_key(orig, ptr, i+6, i+7);
        swap_key(orig, ptr, i  , i+2);
        swap_key(orig, ptr, i+1, i+3);
        swap_key(orig, ptr, i+4, i+6);
        swap_key(orig, ptr, i+5, i+7);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+5, i+6);
        swap_key(orig, ptr, i  , i+4);
        swap_key(orig, ptr, i+1, i+5);
        swap_key(orig, ptr, i+2, i+6);
        swap_key(orig, ptr, i+3, i+7);
        swap_key(orig, ptr, i+2, i+4);
        swap_key(orig, ptr, i+3, i+5);
        swap_key(orig, ptr, i+1, i+2);
        swap_key(orig, ptr, i+3, i+4);
        swap_key(orig, ptr, i+5, i+6);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap_key(orig, ptr, i, j);
        }
    }
}
template <typename T>
typename std::enable_if<std::is_same<T, double>::value>::type
sorter(T*& orig, uint32_t size)
{
    uint32_t i, j;
    __m256d vec0;
    __m256d vec1;
    __m256d vec2;
    __m256d vec3;
    uint8_t stride = (uint8_t)simd_width::AVX_DOUBLE;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_pd((orig + i + 0 * stride));
        vec1 = _mm256_loadu_pd((orig + i + 1 * stride));
        vec2 = _mm256_loadu_pd((orig + i + 2 * stride));
        vec3 = _mm256_loadu_pd((orig + i + 3 * stride));

        in_register_sort(vec0, vec1, vec2, vec3);

        in_register_transpose(vec0, vec1, vec2, vec3);

        _mm256_storeu_pd((orig + i + 0 * stride), vec0);
        _mm256_storeu_pd((orig + i + 1 * stride), vec1);
        _mm256_storeu_pd((orig + i + 2 * stride), vec2);
        _mm256_storeu_pd((orig + i + 3 * stride), vec3);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap(orig, i  , i+1);
        swap(orig, i+2, i+3);
        swap(orig, i  , i+2);
        swap(orig, i+1, i+3);
        swap(orig, i+1, i+2);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap(orig, i, j);
        }
    }
}

template <typename T> // double key int index
typename std::enable_if<std::is_same<T, double>::value>::type
sorter_key(T*& orig, int *&ptr,  uint32_t size)
{
    uint32_t i, j;
    __m256d vec0;
    __m256d vec1;
    __m256d vec2;
    __m256d vec3;

    __m256i p0 ;
    __m256i p1 ;
    uint8_t stride = (uint8_t)simd_width::AVX_DOUBLE;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_pd((orig + i + 0 * stride));
        vec1 = _mm256_loadu_pd((orig + i + 1 * stride));
        vec2 = _mm256_loadu_pd((orig + i + 2 * stride));
        vec3 = _mm256_loadu_pd((orig + i + 3 * stride));

        p0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride * 2));
        p1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride * 2));

        in_register_sort_key(vec0, vec1, vec2, vec3, p0, p1);

        in_register_transpose_key(vec0, vec1, vec2, vec3, p0, p1);

        _mm256_storeu_pd((orig + i + 0 * stride), vec0);
        _mm256_storeu_pd((orig + i + 1 * stride), vec1);
        _mm256_storeu_pd((orig + i + 2 * stride), vec2);
        _mm256_storeu_pd((orig + i + 3 * stride), vec3);

        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride * 2), p0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride * 2), p1);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i  , i+1);
        swap_key(orig, ptr, i+2, i+3);
        swap_key(orig, ptr, i  , i+2);
        swap_key(orig, ptr, i+1, i+3);
        swap_key(orig, ptr, i+1, i+2);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap_key(orig, ptr, i, j);
        }
    }
}

template <typename T> // double key long index
typename std::enable_if<std::is_same<T, double>::value>::type
sorter_key(T*& orig, long *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m256d vec0;
    __m256d vec1;
    __m256d vec2;
    __m256d vec3;

    __m256i p0 ;
    __m256i p1 ;
    __m256i p2 ;
    __m256i p3 ;
    uint8_t stride = (uint8_t)simd_width::AVX_DOUBLE;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) {
        vec0 = _mm256_loadu_pd((orig + i + 0 * stride));
        vec1 = _mm256_loadu_pd((orig + i + 1 * stride));
        vec2 = _mm256_loadu_pd((orig + i + 2 * stride));
        vec3 = _mm256_loadu_pd((orig + i + 3 * stride));

        p0 = _mm256_loadu_si256((__m256i *)(ptr + i + 0 * stride));
        p1 = _mm256_loadu_si256((__m256i *)(ptr + i + 1 * stride));
        p2 = _mm256_loadu_si256((__m256i *)(ptr + i + 2 * stride));
        p3 = _mm256_loadu_si256((__m256i *)(ptr + i + 3 * stride));

        in_register_sort_key(vec0, vec1, vec2, vec3, p0, p1, p2, p3);

        in_register_transpose_key(vec0, vec1, vec2, vec3, p0, p1, p2, p3);

        _mm256_storeu_pd((orig + i + 0 * stride), vec0);
        _mm256_storeu_pd((orig + i + 1 * stride), vec1);
        _mm256_storeu_pd((orig + i + 2 * stride), vec2);
        _mm256_storeu_pd((orig + i + 3 * stride), vec3);

        _mm256_storeu_si256((__m256i *)(ptr + i + 0 * stride), p0);
        _mm256_storeu_si256((__m256i *)(ptr + i + 1 * stride), p1);
        _mm256_storeu_si256((__m256i *)(ptr + i + 2 * stride), p2);
        _mm256_storeu_si256((__m256i *)(ptr + i + 3 * stride), p3);
    }

    // Batcher odd-even mergesort
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i  , i+1);
        swap_key(orig, ptr, i+2, i+3);
        swap_key(orig, ptr, i  , i+2);
        swap_key(orig, ptr, i+1, i+3);
        swap_key(orig, ptr, i+1, i+2);
    }

    // bubble sort 
    for(/*cont'd*/; i < size; i++) 
    {
        for(j = i + 1; j < size; j++) 
        {
            swap_key(orig, ptr, i, j);
        }
    }
}

} // end namespace internal

} // end namespace aspas
