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
 * @file sorter_mic.tcc
 * Definition of sorting functions in the segment-by-segment style
 * This version uses AVX-512 instruction sets.
 *
 */

#include <immintrin.h> 

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
 * Integer vector version (__m512i):
 * This method performs the in-register sort. The sorted elements 
 * are stored vertically accross the registers.
 *
 * @param v0-v15 vector data registers
 * @return sorted data stored vertically among the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_sort(T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
                 T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15)
{
    __m512i t0 ;
    __m512i t1 ;
    __m512i t2 ;
    __m512i t3 ;
    __m512i t4 ;
    __m512i t5 ;
    __m512i t6 ;
    __m512i t7 ;
    __m512i t8 ;
    __m512i t9 ;
    __m512i t10;
    __m512i t11;
    __m512i t12;
    __m512i t13;
    __m512i t14;
    __m512i t15;
    // step 1 
    t1  = _mm512_max_epi32(v1 , v0 );
    t0  = _mm512_min_epi32(v1 , v0 );
    t3  = _mm512_max_epi32(v3 , v2 );
    t2  = _mm512_min_epi32(v3 , v2 );
    t5  = _mm512_max_epi32(v5 , v4 );
    t4  = _mm512_min_epi32(v5 , v4 );
    t7  = _mm512_max_epi32(v7 , v6 );
    t6  = _mm512_min_epi32(v7 , v6 );
    t9  = _mm512_max_epi32(v9 , v8 );
    t8  = _mm512_min_epi32(v9 , v8 );
    t11 = _mm512_max_epi32(v11, v10);
    t10 = _mm512_min_epi32(v11, v10);
    t13 = _mm512_max_epi32(v13, v12);
    t12 = _mm512_min_epi32(v13, v12);
    t15 = _mm512_max_epi32(v15, v14);
    t14 = _mm512_min_epi32(v15, v14);
    // step 2 
    v2  = _mm512_max_epi32(t2 , t0 );
    v0  = _mm512_min_epi32(t2 , t0 );
    v6  = _mm512_max_epi32(t6 , t4 );
    v4  = _mm512_min_epi32(t6 , t4 );
    v10 = _mm512_max_epi32(t10, t8 );
    v8  = _mm512_min_epi32(t10, t8 );
    v14 = _mm512_max_epi32(t14, t12);
    v12 = _mm512_min_epi32(t14, t12);
    v3  = _mm512_max_epi32(t3 , t1 );
    v1  = _mm512_min_epi32(t3 , t1 );
    v7  = _mm512_max_epi32(t7 , t5 );
    v5  = _mm512_min_epi32(t7 , t5 );
    v11 = _mm512_max_epi32(t11, t9 );
    v9  = _mm512_min_epi32(t11, t9 );
    v15 = _mm512_max_epi32(t15, t13);
    v13 = _mm512_min_epi32(t15, t13);
    // step 3 
    t4  = _mm512_max_epi32(v4 , v0 );
    t0  = _mm512_min_epi32(v4 , v0 );
    t12 = _mm512_max_epi32(v12, v8 );
    t8  = _mm512_min_epi32(v12, v8 );
    t5  = _mm512_max_epi32(v5 , v1 );
    t1  = _mm512_min_epi32(v5 , v1 );
    t13 = _mm512_max_epi32(v13, v9 );
    t9  = _mm512_min_epi32(v13, v9 );
    t6  = _mm512_max_epi32(v6 , v2 );
    t2  = _mm512_min_epi32(v6 , v2 );
    t14 = _mm512_max_epi32(v14, v10);
    t10 = _mm512_min_epi32(v14, v10);
    t7  = _mm512_max_epi32(v7 , v3 );
    t3  = _mm512_min_epi32(v7 , v3 );
    t15 = _mm512_max_epi32(v15, v11);
    t11 = _mm512_min_epi32(v15, v11);
    // step 4 
    v8  = _mm512_max_epi32(t8 , t0 );
    v0  = _mm512_min_epi32(t8 , t0 );
    v9  = _mm512_max_epi32(t9 , t1 );
    v1  = _mm512_min_epi32(t9 , t1 );
    v10 = _mm512_max_epi32(t10, t2 );
    v2  = _mm512_min_epi32(t10, t2 );
    v11 = _mm512_max_epi32(t11, t3 );
    v3  = _mm512_min_epi32(t11, t3 );
    v12 = _mm512_max_epi32(t12, t4 );
    v4  = _mm512_min_epi32(t12, t4 );
    v13 = _mm512_max_epi32(t13, t5 );
    v5  = _mm512_min_epi32(t13, t5 );
    v14 = _mm512_max_epi32(t14, t6 );
    v6  = _mm512_min_epi32(t14, t6 );
    v15 = _mm512_max_epi32(t15, t7 );
    v7  = _mm512_min_epi32(t15, t7 );
    // step 5 
    t10 = _mm512_max_epi32(v10, v5 );
    t5  = _mm512_min_epi32(v10, v5 );
    t9  = _mm512_max_epi32(v9 , v6 );
    t6  = _mm512_min_epi32(v9 , v6 );
    t12 = _mm512_max_epi32(v12, v3 );
    t3  = _mm512_min_epi32(v12, v3 );
    t14 = _mm512_max_epi32(v14, v13);
    t13 = _mm512_min_epi32(v14, v13);
    t11 = _mm512_max_epi32(v11, v7 );
    t7  = _mm512_min_epi32(v11, v7 );
    t2  = _mm512_max_epi32(v2 , v1 );
    t1  = _mm512_min_epi32(v2 , v1 );
    t8  = _mm512_max_epi32(v8 , v4 );
    t4  = _mm512_min_epi32(v8 , v4 );
    t0  = v0 ;
    t15 = v15;
    // step 6 
    v4  = _mm512_max_epi32(t4 , t1 );
    v1  = _mm512_min_epi32(t4 , t1 );
    v13 = _mm512_max_epi32(t13, t7 );
    v7  = _mm512_min_epi32(t13, t7 );
    v8  = _mm512_max_epi32(t8 , t2 );
    v2  = _mm512_min_epi32(t8 , t2 );
    v14 = _mm512_max_epi32(t14, t11);
    v11 = _mm512_min_epi32(t14, t11);
    v6  = _mm512_max_epi32(t6 , t5 );
    v5  = _mm512_min_epi32(t6 , t5 );
    v10 = _mm512_max_epi32(t10, t9 );
    v9  = _mm512_min_epi32(t10, t9 );
    v0  = t0 ;
    v3  = t3 ;
    v12 = t12;
    v15 = t15;
    // step 7 
    t4  = _mm512_max_epi32(v4 , v2 );
    t2  = _mm512_min_epi32(v4 , v2 );
    t13 = _mm512_max_epi32(v13, v11);
    t11 = _mm512_min_epi32(v13, v11);
    t8  = _mm512_max_epi32(v8 , v3 );
    t3  = _mm512_min_epi32(v8 , v3 );
    t12 = _mm512_max_epi32(v12, v7 );
    t7  = _mm512_min_epi32(v12, v7 );
    t0  = v0 ;
    t1  = v1 ;
    t5  = v5 ;
    t6  = v6 ;
    t9  = v9 ;
    t10 = v10;
    t14 = v14;
    t15 = v15;
    // step 8 
    v8  = _mm512_max_epi32(t8 , t6 );
    v6  = _mm512_min_epi32(t8 , t6 );
    v12 = _mm512_max_epi32(t12, t10);
    v10 = _mm512_min_epi32(t12, t10);
    v5  = _mm512_max_epi32(t5 , t3 );
    v3  = _mm512_min_epi32(t5 , t3 );
    v9  = _mm512_max_epi32(t9 , t7 );
    v7  = _mm512_min_epi32(t9 , t7 );
    v0  = t0;
    v1  = t1;
    v2  = t2;
    v4  = t4;
    v11 = t11;
    v13 = t13;
    v14 = t14;
    v15 = t15;
    // step 9 
    t4  = _mm512_max_epi32(v4 , v3 );
    t3  = _mm512_min_epi32(v4 , v3 );
    t6  = _mm512_max_epi32(v6 , v5 );
    t5  = _mm512_min_epi32(v6 , v5 );
    t8  = _mm512_max_epi32(v8 , v7 );
    t7  = _mm512_min_epi32(v8 , v7 );
    t10 = _mm512_max_epi32(v10, v9 );
    t9  = _mm512_min_epi32(v10, v9 );
    t12 = _mm512_max_epi32(v12, v11);
    t11 = _mm512_min_epi32(v12, v11);
    t0  = v0;
    t1  = v1;
    t2  = v2;
    t13 = v13;
    t14 = v14;
    t15 = v15;
    // step 10 
    v7  = _mm512_max_epi32(t7 , t6 );
    v6  = _mm512_min_epi32(t7 , t6 );
    v9  = _mm512_max_epi32(t9 , t8 );
    v8  = _mm512_min_epi32(t9 , t8 );
    v0  = t0 ;
    v1  = t1 ;
    v2  = t2 ;
    v3  = t3 ;
    v4  = t4 ;
    v5  = t5 ;
    v10 = t10;
    v11 = t11;
    v12 = t12;
    v13 = t13;
    v14 = t14;
    v15 = t15;

}

/**
 * Integer vector version (__m512i) with indices:
 * This method performs the in-register sort with keys. The sorted elements 
 * are stored vertically accross the registers.
 *
 * @param v0-v15 vector data registers
 * @param vpl0-vpl15 vector pointer registers
 * @param vph0-vph15 vector pointer registers
 * @return sorted data stored vertically among the registers coupled with pointers
 *
 */
template <typename T> // int key int index
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_sort_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3 , 
        __m512i &vp4 , __m512i &vp5 , __m512i &vp6 , __m512i &vp7 , 
        __m512i &vp8 , __m512i &vp9 , __m512i &vp10, __m512i &vp11, 
        __m512i &vp12, __m512i &vp13, __m512i &vp14, __m512i &vp15)
{
    __m512i t0 ;
    __m512i t1 ;
    __m512i t2 ;
    __m512i t3 ;
    __m512i t4 ;
    __m512i t5 ;
    __m512i t6 ;
    __m512i t7 ;
    __m512i t8 ;
    __m512i t9 ;
    __m512i t10;
    __m512i t11;
    __m512i t12;
    __m512i t13;
    __m512i t14;
    __m512i t15;

    __m512i tp0 ;
    __m512i tp1 ;
    __m512i tp2 ;
    __m512i tp3 ;
    __m512i tp4 ;
    __m512i tp5 ;
    __m512i tp6 ;
    __m512i tp7 ;
    __m512i tp8 ;
    __m512i tp9 ;
    __m512i tp10;
    __m512i tp11;
    __m512i tp12;
    __m512i tp13;
    __m512i tp14;
    __m512i tp15;

    __mmask16 m0 ;
    __mmask16 m1 ;
    __mmask16 m2 ;
    __mmask16 m3 ;
    __mmask16 m4 ;
    __mmask16 m5 ;
    __mmask16 m6 ;
    __mmask16 m7 ;

    // step 1 
    // t1  = _mm512_max_epi32(v1 , v0 );
    // t0  = _mm512_min_epi32(v1 , v0 );
    // t3  = _mm512_max_epi32(v3 , v2 );
    // t2  = _mm512_min_epi32(v3 , v2 );
    // t5  = _mm512_max_epi32(v5 , v4 );
    // t4  = _mm512_min_epi32(v5 , v4 );
    // t7  = _mm512_max_epi32(v7 , v6 );
    // t6  = _mm512_min_epi32(v7 , v6 );
    // t9  = _mm512_max_epi32(v9 , v8 );
    // t8  = _mm512_min_epi32(v9 , v8 );
    // t11 = _mm512_max_epi32(v11, v10);
    // t10 = _mm512_min_epi32(v11, v10);
    // t13 = _mm512_max_epi32(v13, v12);
    // t12 = _mm512_min_epi32(v13, v12);
    // t15 = _mm512_max_epi32(v15, v14);
    // t14 = _mm512_min_epi32(v15, v14);

    m0 = _mm512_cmp_epi32_mask(v0 , v1 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v2 , v3 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v4 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v6 , v7 , _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v8 , v9 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(v10, v11, _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(v12, v13, _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(v14, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_epi32(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_epi32(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_epi32(v2 , m1, v3 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m1, v2 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_epi32(v5 , m2, v4 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m3, v6 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m4, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m4, v8 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m5, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m5, v10, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m6, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m6, v12, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_epi32(v14, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_epi32(v15, m7, v14, _MM_SWIZ_REG_NONE);

    tp0  = _mm512_mask_swizzle_epi32(vp0 , m0, vp1 , _MM_SWIZ_REG_NONE);
    tp1  = _mm512_mask_swizzle_epi32(vp1 , m0, vp0 , _MM_SWIZ_REG_NONE);
    tp2  = _mm512_mask_swizzle_epi32(vp2 , m1, vp3 , _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m1, vp2 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m2, vp5 , _MM_SWIZ_REG_NONE);
    tp5  = _mm512_mask_swizzle_epi32(vp5 , m2, vp4 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m3, vp7 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m3, vp6 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m4, vp9 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m4, vp8 , _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m5, vp11, _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m5, vp10, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m6, vp13, _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m6, vp12, _MM_SWIZ_REG_NONE);
    tp14 = _mm512_mask_swizzle_epi32(vp14, m7, vp15, _MM_SWIZ_REG_NONE);
    tp15 = _mm512_mask_swizzle_epi32(vp15, m7, vp14, _MM_SWIZ_REG_NONE);

    // step 2 
    // v2  = _mm512_max_epi32(t2 , t0 );
    // v0  = _mm512_min_epi32(t2 , t0 );
    // v6  = _mm512_max_epi32(t6 , t4 );
    // v4  = _mm512_min_epi32(t6 , t4 );
    // v10 = _mm512_max_epi32(t10, t8 );
    // v8  = _mm512_min_epi32(t10, t8 );
    // v14 = _mm512_max_epi32(t14, t12);
    // v12 = _mm512_min_epi32(t14, t12);
    // v3  = _mm512_max_epi32(t3 , t1 );
    // v1  = _mm512_min_epi32(t3 , t1 );
    // v7  = _mm512_max_epi32(t7 , t5 );
    // v5  = _mm512_min_epi32(t7 , t5 );
    // v11 = _mm512_max_epi32(t11, t9 );
    // v9  = _mm512_min_epi32(t11, t9 );
    // v15 = _mm512_max_epi32(t15, t13);
    // v13 = _mm512_min_epi32(t15, t13);

    m0 = _mm512_cmp_epi32_mask(t0 , t2 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t4 , t6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t8 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t12, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(t1 , t3 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(t5 , t7 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(t9 , t11, _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(t13, t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_epi32(t0 , m0, t2 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_epi32(t2 , m0, t0 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_epi32(t4 , m1, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_epi32(t6 , m1, t4 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m2, t8 , _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_epi32(t12, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_epi32(t14, m3, t12, _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_epi32(t1 , m4, t3 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_epi32(t3 , m4, t1 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m5, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m5, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m6, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_epi32(t11, m6, t9 , _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_epi32(t13, m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_epi32(t15, m7, t13, _MM_SWIZ_REG_NONE);

    vp0  = _mm512_mask_swizzle_epi32(tp0 , m0, tp2 , _MM_SWIZ_REG_NONE);
    vp2  = _mm512_mask_swizzle_epi32(tp2 , m0, tp0 , _MM_SWIZ_REG_NONE);
    vp4  = _mm512_mask_swizzle_epi32(tp4 , m1, tp6 , _MM_SWIZ_REG_NONE);
    vp6  = _mm512_mask_swizzle_epi32(tp6 , m1, tp4 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m2, tp10, _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m2, tp8 , _MM_SWIZ_REG_NONE);
    vp12 = _mm512_mask_swizzle_epi32(tp12, m3, tp14, _MM_SWIZ_REG_NONE);
    vp14 = _mm512_mask_swizzle_epi32(tp14, m3, tp12, _MM_SWIZ_REG_NONE);
    vp1  = _mm512_mask_swizzle_epi32(tp1 , m4, tp3 , _MM_SWIZ_REG_NONE);
    vp3  = _mm512_mask_swizzle_epi32(tp3 , m4, tp1 , _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m5, tp7 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m5, tp5 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m6, tp11, _MM_SWIZ_REG_NONE);
    vp11 = _mm512_mask_swizzle_epi32(tp11, m6, tp9 , _MM_SWIZ_REG_NONE);
    vp13 = _mm512_mask_swizzle_epi32(tp13, m7, tp15, _MM_SWIZ_REG_NONE);
    vp15 = _mm512_mask_swizzle_epi32(tp15, m7, tp13, _MM_SWIZ_REG_NONE);

    // step 3 
    // t4  = _mm512_max_epi32(v4 , v0 );
    // t0  = _mm512_min_epi32(v4 , v0 );
    // t12 = _mm512_max_epi32(v12, v8 );
    // t8  = _mm512_min_epi32(v12, v8 );
    // t5  = _mm512_max_epi32(v5 , v1 );
    // t1  = _mm512_min_epi32(v5 , v1 );
    // t13 = _mm512_max_epi32(v13, v9 );
    // t9  = _mm512_min_epi32(v13, v9 );
    // t6  = _mm512_max_epi32(v6 , v2 );
    // t2  = _mm512_min_epi32(v6 , v2 );
    // t14 = _mm512_max_epi32(v14, v10);
    // t10 = _mm512_min_epi32(v14, v10);
    // t7  = _mm512_max_epi32(v7 , v3 );
    // t3  = _mm512_min_epi32(v7 , v3 );
    // t15 = _mm512_max_epi32(v15, v11);
    // t11 = _mm512_min_epi32(v15, v11);

    m0 = _mm512_cmp_epi32_mask(v0 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v8 , v12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v1 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v9 , v13, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v2 , v6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(v10, v14, _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(v3 , v7 , _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(v11, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_epi32(v0 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m0, v0 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m1, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m1, v8 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_epi32(v1 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_epi32(v5 , m2, v1 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m3, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m3, v9 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_epi32(v2 , m4, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m4, v2 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m5, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_epi32(v14, m5, v10, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m6, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m6, v3 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_epi32(v15, m7, v11, _MM_SWIZ_REG_NONE);

    tp0  = _mm512_mask_swizzle_epi32(vp0 , m0, vp4 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m0, vp0 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m1, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m1, vp8 , _MM_SWIZ_REG_NONE);
    tp1  = _mm512_mask_swizzle_epi32(vp1 , m2, vp5 , _MM_SWIZ_REG_NONE);
    tp5  = _mm512_mask_swizzle_epi32(vp5 , m2, vp1 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m3, vp13, _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m3, vp9 , _MM_SWIZ_REG_NONE);
    tp2  = _mm512_mask_swizzle_epi32(vp2 , m4, vp6 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m4, vp2 , _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m5, vp14, _MM_SWIZ_REG_NONE);
    tp14 = _mm512_mask_swizzle_epi32(vp14, m5, vp10, _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m6, vp7 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m6, vp3 , _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m7, vp15, _MM_SWIZ_REG_NONE);
    tp15 = _mm512_mask_swizzle_epi32(vp15, m7, vp11, _MM_SWIZ_REG_NONE);
    // step 4 
    // v8  = _mm512_max_epi32(t8 , t0 );
    // v0  = _mm512_min_epi32(t8 , t0 );
    // v9  = _mm512_max_epi32(t9 , t1 );
    // v1  = _mm512_min_epi32(t9 , t1 );
    // v10 = _mm512_max_epi32(t10, t2 );
    // v2  = _mm512_min_epi32(t10, t2 );
    // v11 = _mm512_max_epi32(t11, t3 );
    // v3  = _mm512_min_epi32(t11, t3 );
    // v12 = _mm512_max_epi32(t12, t4 );
    // v4  = _mm512_min_epi32(t12, t4 );
    // v13 = _mm512_max_epi32(t13, t5 );
    // v5  = _mm512_min_epi32(t13, t5 );
    // v14 = _mm512_max_epi32(t14, t6 );
    // v6  = _mm512_min_epi32(t14, t6 );
    // v15 = _mm512_max_epi32(t15, t7 );
    // v7  = _mm512_min_epi32(t15, t7 );

    m0 = _mm512_cmp_epi32_mask(t0 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t1 , t9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t2 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t3 , t11, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(t4 , t12, _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(t5 , t13, _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(t6 , t14, _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(t7 , t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_epi32(t0 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m0, t0 , _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_epi32(t1 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m1, t1 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_epi32(t2 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m2, t2 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_epi32(t3 , m3, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_epi32(t11, m3, t3 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_epi32(t4 , m4, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_epi32(t12, m4, t4 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m5, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_epi32(t13, m5, t5 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_epi32(t6 , m6, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_epi32(t14, m6, t6 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_epi32(t15, m7, t7 , _MM_SWIZ_REG_NONE);

    vp0  = _mm512_mask_swizzle_epi32(tp0 , m0, tp8 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m0, tp0 , _MM_SWIZ_REG_NONE);
    vp1  = _mm512_mask_swizzle_epi32(tp1 , m1, tp9 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m1, tp1 , _MM_SWIZ_REG_NONE);
    vp2  = _mm512_mask_swizzle_epi32(tp2 , m2, tp10, _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m2, tp2 , _MM_SWIZ_REG_NONE);
    vp3  = _mm512_mask_swizzle_epi32(tp3 , m3, tp11, _MM_SWIZ_REG_NONE);
    vp11 = _mm512_mask_swizzle_epi32(tp11, m3, tp3 , _MM_SWIZ_REG_NONE);
    vp4  = _mm512_mask_swizzle_epi32(tp4 , m4, tp12, _MM_SWIZ_REG_NONE);
    vp12 = _mm512_mask_swizzle_epi32(tp12, m4, tp4 , _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m5, tp13, _MM_SWIZ_REG_NONE);
    vp13 = _mm512_mask_swizzle_epi32(tp13, m5, tp5 , _MM_SWIZ_REG_NONE);
    vp6  = _mm512_mask_swizzle_epi32(tp6 , m6, tp14, _MM_SWIZ_REG_NONE);
    vp14 = _mm512_mask_swizzle_epi32(tp14, m6, tp6 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m7, tp15, _MM_SWIZ_REG_NONE);
    vp15 = _mm512_mask_swizzle_epi32(tp15, m7, tp7 , _MM_SWIZ_REG_NONE);

    // step 5 
    // t10 = _mm512_max_epi32(v10, v5 );
    // t5  = _mm512_min_epi32(v10, v5 );
    // t9  = _mm512_max_epi32(v9 , v6 );
    // t6  = _mm512_min_epi32(v9 , v6 );
    // t12 = _mm512_max_epi32(v12, v3 );
    // t3  = _mm512_min_epi32(v12, v3 );
    // t14 = _mm512_max_epi32(v14, v13);
    // t13 = _mm512_min_epi32(v14, v13);
    // t11 = _mm512_max_epi32(v11, v7 );
    // t7  = _mm512_min_epi32(v11, v7 );
    // t2  = _mm512_max_epi32(v2 , v1 );
    // t1  = _mm512_min_epi32(v2 , v1 );
    // t8  = _mm512_max_epi32(v8 , v4 );
    // t4  = _mm512_min_epi32(v8 , v4 );
    // t0  = v0 ;
    // t15 = v15;

    m0 = _mm512_cmp_epi32_mask(v5 , v10, _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v6 , v9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v3 , v12, _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v13, v14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v7 , v11, _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(v1 , v2 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(v4 , v8 , _MM_CMPINT_GT);

    t5  = _mm512_mask_swizzle_epi32(v5 , m0, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m0, v5 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m1, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m1, v6 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m2, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m2, v3 , _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m3, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_epi32(v14, m3, v13, _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m4, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m4, v7 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_epi32(v1 , m5, v2 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_epi32(v2 , m5, v1 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m6, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m6, v4 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t15 = v15;

    tp5  = _mm512_mask_swizzle_epi32(vp5 , m0, vp10, _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m0, vp5 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m1, vp9 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m1, vp6 , _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m2, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m2, vp3 , _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m3, vp14, _MM_SWIZ_REG_NONE);
    tp14 = _mm512_mask_swizzle_epi32(vp14, m3, vp13, _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m4, vp11, _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m4, vp7 , _MM_SWIZ_REG_NONE);
    tp1  = _mm512_mask_swizzle_epi32(vp1 , m5, vp2 , _MM_SWIZ_REG_NONE);
    tp2  = _mm512_mask_swizzle_epi32(vp2 , m5, vp1 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m6, vp8 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m6, vp4 , _MM_SWIZ_REG_NONE);
    tp0  = vp0 ;
    tp15 = vp15;

    // step 6 
    // v4  = _mm512_max_epi32(t4 , t1 );
    // v1  = _mm512_min_epi32(t4 , t1 );
    // v13 = _mm512_max_epi32(t13, t7 );
    // v7  = _mm512_min_epi32(t13, t7 );
    // v8  = _mm512_max_epi32(t8 , t2 );
    // v2  = _mm512_min_epi32(t8 , t2 );
    // v14 = _mm512_max_epi32(t14, t11);
    // v11 = _mm512_min_epi32(t14, t11);
    // v6  = _mm512_max_epi32(t6 , t5 );
    // v5  = _mm512_min_epi32(t6 , t5 );
    // v10 = _mm512_max_epi32(t10, t9 );
    // v9  = _mm512_min_epi32(t10, t9 );
    // v0  = t0 ;
    // v3  = t3 ;
    // v12 = t12;
    // v15 = t15;

    m0 = _mm512_cmp_epi32_mask(t1 , t4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t7 , t13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t2 , t8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t11, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(t5 , t6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(t9 , t10, _MM_CMPINT_GT);

    v1  = _mm512_mask_swizzle_epi32(t1 , m0, t4 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_epi32(t4 , m0, t1 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m1, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_epi32(t13, m1, t7 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_epi32(t2 , m2, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m2, t2 , _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_epi32(t11, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_epi32(t14, m3, t11, _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m4, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_epi32(t6 , m4, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m5, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m5, t9 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v3  = t3 ;
    v12 = t12;
    v15 = t15;

    vp1  = _mm512_mask_swizzle_epi32(tp1 , m0, tp4 , _MM_SWIZ_REG_NONE);
    vp4  = _mm512_mask_swizzle_epi32(tp4 , m0, tp1 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m1, tp13, _MM_SWIZ_REG_NONE);
    vp13 = _mm512_mask_swizzle_epi32(tp13, m1, tp7 , _MM_SWIZ_REG_NONE);
    vp2  = _mm512_mask_swizzle_epi32(tp2 , m2, tp8 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m2, tp2 , _MM_SWIZ_REG_NONE);
    vp11 = _mm512_mask_swizzle_epi32(tp11, m3, tp14, _MM_SWIZ_REG_NONE);
    vp14 = _mm512_mask_swizzle_epi32(tp14, m3, tp11, _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m4, tp6 , _MM_SWIZ_REG_NONE);
    vp6  = _mm512_mask_swizzle_epi32(tp6 , m4, tp5 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m5, tp10, _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m5, tp9 , _MM_SWIZ_REG_NONE);
    vp0  = tp0 ;
    vp3  = tp3 ;
    vp12 = tp12;
    vp15 = tp15;

    // step 7 
    // t4  = _mm512_max_epi32(v4 , v2 );
    // t2  = _mm512_min_epi32(v4 , v2 );
    // t13 = _mm512_max_epi32(v13, v11);
    // t11 = _mm512_min_epi32(v13, v11);
    // t8  = _mm512_max_epi32(v8 , v3 );
    // t3  = _mm512_min_epi32(v8 , v3 );
    // t12 = _mm512_max_epi32(v12, v7 );
    // t7  = _mm512_min_epi32(v12, v7 );
    // t0  = v0 ;
    // t1  = v1 ;
    // t5  = v5 ;
    // t6  = v6 ;
    // t9  = v9 ;
    // t10 = v10;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_epi32_mask(v2 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v11, v13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v3 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v7 , v12, _MM_CMPINT_GT);

    t2  = _mm512_mask_swizzle_epi32(v2 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m0, v2 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m1, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m1, v11, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m2, v3 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m3, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m3, v7 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t1  = v1 ;
    t5  = v5 ;
    t6  = v6 ;
    t9  = v9 ;
    t10 = v10;
    t14 = v14;
    t15 = v15;

    tp2  = _mm512_mask_swizzle_epi32(vp2 , m0, vp4 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m0, vp2 , _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m1, vp13, _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m1, vp11, _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m2, vp8 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m2, vp3 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m3, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m3, vp7 , _MM_SWIZ_REG_NONE);
    tp0  = vp0 ;
    tp1  = vp1 ;
    tp5  = vp5 ;
    tp6  = vp6 ;
    tp9  = vp9 ;
    tp10 = vp10;
    tp14 = vp14;
    tp15 = vp15;

    // step 8 
    // v8  = _mm512_max_epi32(t8 , t6 );
    // v6  = _mm512_min_epi32(t8 , t6 );
    // v12 = _mm512_max_epi32(t12, t10);
    // v10 = _mm512_min_epi32(t12, t10);
    // v5  = _mm512_max_epi32(t5 , t3 );
    // v3  = _mm512_min_epi32(t5 , t3 );
    // v9  = _mm512_max_epi32(t9 , t7 );
    // v7  = _mm512_min_epi32(t9 , t7 );
    // v0  = t0;
    // v1  = t1;
    // v2  = t2;
    // v4  = t4;
    // v11 = t11;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_epi32_mask(t6 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t10, t12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t3 , t5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t7 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_epi32(t6 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m0, t6 , _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m1, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_epi32(t12, m1, t10, _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_epi32(t3 , m2, t5 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m2, t3 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m3, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m3, t7 , _MM_SWIZ_REG_NONE);
    v0  = t0;
    v1  = t1;
    v2  = t2;
    v4  = t4;
    v11 = t11;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vp6  = _mm512_mask_swizzle_epi32(tp6 , m0, tp8 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m0, tp6 , _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m1, tp12, _MM_SWIZ_REG_NONE);
    vp12 = _mm512_mask_swizzle_epi32(tp12, m1, tp10, _MM_SWIZ_REG_NONE);
    vp3  = _mm512_mask_swizzle_epi32(tp3 , m2, tp5 , _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m2, tp3 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m3, tp9 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m3, tp7 , _MM_SWIZ_REG_NONE);
    vp0  = tp0;
    vp1  = tp1;
    vp2  = tp2;
    vp4  = tp4;
    vp11 = tp11;
    vp13 = tp13;
    vp14 = tp14;
    vp15 = tp15;

    // step 9 
    // t4  = _mm512_max_epi32(v4 , v3 );
    // t3  = _mm512_min_epi32(v4 , v3 );
    // t6  = _mm512_max_epi32(v6 , v5 );
    // t5  = _mm512_min_epi32(v6 , v5 );
    // t8  = _mm512_max_epi32(v8 , v7 );
    // t7  = _mm512_min_epi32(v8 , v7 );
    // t10 = _mm512_max_epi32(v10, v9 );
    // t9  = _mm512_min_epi32(v10, v9 );
    // t12 = _mm512_max_epi32(v12, v11);
    // t11 = _mm512_min_epi32(v12, v11);
    // t0  = v0;
    // t1  = v1;
    // t2  = v2;
    // t13 = v13;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_epi32_mask(v3 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v5 , v6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v7 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v9 , v10, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v11, v12, _MM_CMPINT_GT);

    t3  = _mm512_mask_swizzle_epi32(v3 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m0, v3 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_epi32(v5 , m1, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m1, v5 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m2, v7 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m3, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m3, v9 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m4, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m4, v11, _MM_SWIZ_REG_NONE);
    t0  = v0;
    t1  = v1;
    t2  = v2;
    t13 = v13;
    t14 = v14;
    t15 = v15;

    tp3  = _mm512_mask_swizzle_epi32(vp3 , m0, vp4 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m0, vp3 , _MM_SWIZ_REG_NONE);
    tp5  = _mm512_mask_swizzle_epi32(vp5 , m1, vp6 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m1, vp5 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m2, vp8 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m2, vp7 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m3, vp10, _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m3, vp9 , _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m4, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m4, vp11, _MM_SWIZ_REG_NONE);
    tp0  = vp0;
    tp1  = vp1;
    tp2  = vp2;
    tp13 = vp13;
    tp14 = vp14;
    tp15 = vp15;
    // step 10 
    // v7  = _mm512_max_epi32(t7 , t6 );
    // v6  = _mm512_min_epi32(t7 , t6 );
    // v9  = _mm512_max_epi32(t9 , t8 );
    // v8  = _mm512_min_epi32(t9 , t8 );
    // v0  = t0 ;
    // v1  = t1 ;
    // v2  = t2 ;
    // v3  = t3 ;
    // v4  = t4 ;
    // v5  = t5 ;
    // v10 = t10;
    // v11 = t11;
    // v12 = t12;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_epi32_mask(t6 , t7 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t8 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_epi32(t6 , m0, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m0, t6 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m1, t8 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v1  = t1 ;
    v2  = t2 ;
    v3  = t3 ;
    v4  = t4 ;
    v5  = t5 ;
    v10 = t10;
    v11 = t11;
    v12 = t12;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vp6  = _mm512_mask_swizzle_epi32(tp6 , m0, tp7 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m0, tp6 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m1, tp9 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m1, tp8 , _MM_SWIZ_REG_NONE);
    vp0  = tp0 ;
    vp1  = tp1 ;
    vp2  = tp2 ;
    vp3  = tp3 ;
    vp4  = tp4 ;
    vp5  = tp5 ;
    vp10 = tp10;
    vp11 = tp11;
    vp12 = tp12;
    vp13 = tp13;
    vp14 = tp14;
    vp15 = tp15;

}

template <typename T> // int key long index
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_sort_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vlp0 , __m512i &vlp1 , __m512i &vlp2 , __m512i &vlp3 , 
        __m512i &vlp4 , __m512i &vlp5 , __m512i &vlp6 , __m512i &vlp7 , 
        __m512i &vlp8 , __m512i &vlp9 , __m512i &vlp10, __m512i &vlp11, 
        __m512i &vlp12, __m512i &vlp13, __m512i &vlp14, __m512i &vlp15, 
        __m512i &vhp0 , __m512i &vhp1 , __m512i &vhp2 , __m512i &vhp3 , 
        __m512i &vhp4 , __m512i &vhp5 , __m512i &vhp6 , __m512i &vhp7 , 
        __m512i &vhp8 , __m512i &vhp9 , __m512i &vhp10, __m512i &vhp11, 
        __m512i &vhp12, __m512i &vhp13, __m512i &vhp14, __m512i &vhp15)
{
    __m512i t0 ;
    __m512i t1 ;
    __m512i t2 ;
    __m512i t3 ;
    __m512i t4 ;
    __m512i t5 ;
    __m512i t6 ;
    __m512i t7 ;
    __m512i t8 ;
    __m512i t9 ;
    __m512i t10;
    __m512i t11;
    __m512i t12;
    __m512i t13;
    __m512i t14;
    __m512i t15;

    __m512i tlp0 ;
    __m512i tlp1 ;
    __m512i tlp2 ;
    __m512i tlp3 ;
    __m512i tlp4 ;
    __m512i tlp5 ;
    __m512i tlp6 ;
    __m512i tlp7 ;
    __m512i tlp8 ;
    __m512i tlp9 ;
    __m512i tlp10;
    __m512i tlp11;
    __m512i tlp12;
    __m512i tlp13;
    __m512i tlp14;
    __m512i tlp15;

    __m512i thp0 ;
    __m512i thp1 ;
    __m512i thp2 ;
    __m512i thp3 ;
    __m512i thp4 ;
    __m512i thp5 ;
    __m512i thp6 ;
    __m512i thp7 ;
    __m512i thp8 ;
    __m512i thp9 ;
    __m512i thp10;
    __m512i thp11;
    __m512i thp12;
    __m512i thp13;
    __m512i thp14;
    __m512i thp15;

    __mmask16 m0 ;
    __mmask16 m1 ;
    __mmask16 m2 ;
    __mmask16 m3 ;
    __mmask16 m4 ;
    __mmask16 m5 ;
    __mmask16 m6 ;
    __mmask16 m7 ;

    // step 1 
    // t1  = _mm512_max_epi32(v1 , v0 );
    // t0  = _mm512_min_epi32(v1 , v0 );
    // t3  = _mm512_max_epi32(v3 , v2 );
    // t2  = _mm512_min_epi32(v3 , v2 );
    // t5  = _mm512_max_epi32(v5 , v4 );
    // t4  = _mm512_min_epi32(v5 , v4 );
    // t7  = _mm512_max_epi32(v7 , v6 );
    // t6  = _mm512_min_epi32(v7 , v6 );
    // t9  = _mm512_max_epi32(v9 , v8 );
    // t8  = _mm512_min_epi32(v9 , v8 );
    // t11 = _mm512_max_epi32(v11, v10);
    // t10 = _mm512_min_epi32(v11, v10);
    // t13 = _mm512_max_epi32(v13, v12);
    // t12 = _mm512_min_epi32(v13, v12);
    // t15 = _mm512_max_epi32(v15, v14);
    // t14 = _mm512_min_epi32(v15, v14);

    m0 = _mm512_cmp_epi32_mask(v0 , v1 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v2 , v3 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v4 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v6 , v7 , _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v8 , v9 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(v10, v11, _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(v12, v13, _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(v14, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_epi32(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_epi32(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_epi32(v2 , m1, v3 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m1, v2 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_epi32(v5 , m2, v4 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m3, v6 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m4, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m4, v8 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m5, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m5, v10, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m6, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m6, v12, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_epi32(v14, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_epi32(v15, m7, v14, _MM_SWIZ_REG_NONE);

    tlp0  = _mm512_mask_swizzle_epi64(vlp0 , m0, vlp1 , _MM_SWIZ_REG_NONE);
    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , m0, vlp0 , _MM_SWIZ_REG_NONE);
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m1, vlp3 , _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m1, vlp2 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m2, vlp5 , _MM_SWIZ_REG_NONE);
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m2, vlp4 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m3, vlp7 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m3, vlp6 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m4, vlp9 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m4, vlp8 , _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m5, vlp11, _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m5, vlp10, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m6, vlp13, _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m6, vlp12, _MM_SWIZ_REG_NONE);
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, m7, vlp15, _MM_SWIZ_REG_NONE);
    tlp15 = _mm512_mask_swizzle_epi64(vlp15, m7, vlp14, _MM_SWIZ_REG_NONE);

    thp0  = _mm512_mask_swizzle_epi64(vhp0 , m0>>8, vhp1 , _MM_SWIZ_REG_NONE);
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , m0>>8, vhp0 , _MM_SWIZ_REG_NONE);
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m1>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m1>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m2>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m2>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m3>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m3>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m4>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m4>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m5>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m5>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m6>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m6>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp14 = _mm512_mask_swizzle_epi64(vhp14, m7>>8, vhp15, _MM_SWIZ_REG_NONE);
    thp15 = _mm512_mask_swizzle_epi64(vhp15, m7>>8, vhp14, _MM_SWIZ_REG_NONE);

    // step 2 
    // v2  = _mm512_max_epi32(t2 , t0 );
    // v0  = _mm512_min_epi32(t2 , t0 );
    // v6  = _mm512_max_epi32(t6 , t4 );
    // v4  = _mm512_min_epi32(t6 , t4 );
    // v10 = _mm512_max_epi32(t10, t8 );
    // v8  = _mm512_min_epi32(t10, t8 );
    // v14 = _mm512_max_epi32(t14, t12);
    // v12 = _mm512_min_epi32(t14, t12);
    // v3  = _mm512_max_epi32(t3 , t1 );
    // v1  = _mm512_min_epi32(t3 , t1 );
    // v7  = _mm512_max_epi32(t7 , t5 );
    // v5  = _mm512_min_epi32(t7 , t5 );
    // v11 = _mm512_max_epi32(t11, t9 );
    // v9  = _mm512_min_epi32(t11, t9 );
    // v15 = _mm512_max_epi32(t15, t13);
    // v13 = _mm512_min_epi32(t15, t13);

    m0 = _mm512_cmp_epi32_mask(t0 , t2 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t4 , t6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t8 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t12, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(t1 , t3 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(t5 , t7 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(t9 , t11, _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(t13, t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_epi32(t0 , m0, t2 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_epi32(t2 , m0, t0 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_epi32(t4 , m1, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_epi32(t6 , m1, t4 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m2, t8 , _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_epi32(t12, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_epi32(t14, m3, t12, _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_epi32(t1 , m4, t3 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_epi32(t3 , m4, t1 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m5, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m5, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m6, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_epi32(t11, m6, t9 , _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_epi32(t13, m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_epi32(t15, m7, t13, _MM_SWIZ_REG_NONE);

    vlp0  = _mm512_mask_swizzle_epi64(tlp0 , m0, tlp2 , _MM_SWIZ_REG_NONE);
    vlp2  = _mm512_mask_swizzle_epi64(tlp2 , m0, tlp0 , _MM_SWIZ_REG_NONE);
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , m1, tlp6 , _MM_SWIZ_REG_NONE);
    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m1, tlp4 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m2, tlp10, _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m2, tlp8 , _MM_SWIZ_REG_NONE);
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, m3, tlp14, _MM_SWIZ_REG_NONE);
    vlp14 = _mm512_mask_swizzle_epi64(tlp14, m3, tlp12, _MM_SWIZ_REG_NONE);
    vlp1  = _mm512_mask_swizzle_epi64(tlp1 , m4, tlp3 , _MM_SWIZ_REG_NONE);
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , m4, tlp1 , _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m5, tlp7 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m5, tlp5 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m6, tlp11, _MM_SWIZ_REG_NONE);
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, m6, tlp9 , _MM_SWIZ_REG_NONE);
    vlp13 = _mm512_mask_swizzle_epi64(tlp13, m7, tlp15, _MM_SWIZ_REG_NONE);
    vlp15 = _mm512_mask_swizzle_epi64(tlp15, m7, tlp13, _MM_SWIZ_REG_NONE);

    vhp0  = _mm512_mask_swizzle_epi64(thp0 , m0>>8, thp2 , _MM_SWIZ_REG_NONE);
    vhp2  = _mm512_mask_swizzle_epi64(thp2 , m0>>8, thp0 , _MM_SWIZ_REG_NONE);
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , m1>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m1>>8, thp4 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m2>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m2>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp12 = _mm512_mask_swizzle_epi64(thp12, m3>>8, thp14, _MM_SWIZ_REG_NONE);
    vhp14 = _mm512_mask_swizzle_epi64(thp14, m3>>8, thp12, _MM_SWIZ_REG_NONE);
    vhp1  = _mm512_mask_swizzle_epi64(thp1 , m4>>8, thp3 , _MM_SWIZ_REG_NONE);
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , m4>>8, thp1 , _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m5>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m5>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m6>>8, thp11, _MM_SWIZ_REG_NONE);
    vhp11 = _mm512_mask_swizzle_epi64(thp11, m6>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp13 = _mm512_mask_swizzle_epi64(thp13, m7>>8, thp15, _MM_SWIZ_REG_NONE);
    vhp15 = _mm512_mask_swizzle_epi64(thp15, m7>>8, thp13, _MM_SWIZ_REG_NONE);

    // step 3 
    // t4  = _mm512_max_epi32(v4 , v0 );
    // t0  = _mm512_min_epi32(v4 , v0 );
    // t12 = _mm512_max_epi32(v12, v8 );
    // t8  = _mm512_min_epi32(v12, v8 );
    // t5  = _mm512_max_epi32(v5 , v1 );
    // t1  = _mm512_min_epi32(v5 , v1 );
    // t13 = _mm512_max_epi32(v13, v9 );
    // t9  = _mm512_min_epi32(v13, v9 );
    // t6  = _mm512_max_epi32(v6 , v2 );
    // t2  = _mm512_min_epi32(v6 , v2 );
    // t14 = _mm512_max_epi32(v14, v10);
    // t10 = _mm512_min_epi32(v14, v10);
    // t7  = _mm512_max_epi32(v7 , v3 );
    // t3  = _mm512_min_epi32(v7 , v3 );
    // t15 = _mm512_max_epi32(v15, v11);
    // t11 = _mm512_min_epi32(v15, v11);

    m0 = _mm512_cmp_epi32_mask(v0 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v8 , v12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v1 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v9 , v13, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v2 , v6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(v10, v14, _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(v3 , v7 , _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(v11, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_epi32(v0 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m0, v0 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m1, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m1, v8 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_epi32(v1 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_epi32(v5 , m2, v1 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m3, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m3, v9 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_epi32(v2 , m4, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m4, v2 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m5, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_epi32(v14, m5, v10, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m6, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m6, v3 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_epi32(v15, m7, v11, _MM_SWIZ_REG_NONE);

    tlp0  = _mm512_mask_swizzle_epi64(vlp0 , m0, vlp4 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m0, vlp0 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m1, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m1, vlp8 , _MM_SWIZ_REG_NONE);
    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , m2, vlp5 , _MM_SWIZ_REG_NONE);
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m2, vlp1 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m3, vlp13, _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m3, vlp9 , _MM_SWIZ_REG_NONE);
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m4, vlp6 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m4, vlp2 , _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m5, vlp14, _MM_SWIZ_REG_NONE);
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, m5, vlp10, _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m6, vlp7 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m6, vlp3 , _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m7, vlp15, _MM_SWIZ_REG_NONE);
    tlp15 = _mm512_mask_swizzle_epi64(vlp15, m7, vlp11, _MM_SWIZ_REG_NONE);

    thp0  = _mm512_mask_swizzle_epi64(vhp0 , m0>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m0>>8, vhp0 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m1>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m1>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , m2>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m2>>8, vhp1 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m3>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m3>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m4>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m4>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m5>>8, vhp14, _MM_SWIZ_REG_NONE);
    thp14 = _mm512_mask_swizzle_epi64(vhp14, m5>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m6>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m6>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m7>>8, vhp15, _MM_SWIZ_REG_NONE);
    thp15 = _mm512_mask_swizzle_epi64(vhp15, m7>>8, vhp11, _MM_SWIZ_REG_NONE);
    // step 4 
    // v8  = _mm512_max_epi32(t8 , t0 );
    // v0  = _mm512_min_epi32(t8 , t0 );
    // v9  = _mm512_max_epi32(t9 , t1 );
    // v1  = _mm512_min_epi32(t9 , t1 );
    // v10 = _mm512_max_epi32(t10, t2 );
    // v2  = _mm512_min_epi32(t10, t2 );
    // v11 = _mm512_max_epi32(t11, t3 );
    // v3  = _mm512_min_epi32(t11, t3 );
    // v12 = _mm512_max_epi32(t12, t4 );
    // v4  = _mm512_min_epi32(t12, t4 );
    // v13 = _mm512_max_epi32(t13, t5 );
    // v5  = _mm512_min_epi32(t13, t5 );
    // v14 = _mm512_max_epi32(t14, t6 );
    // v6  = _mm512_min_epi32(t14, t6 );
    // v15 = _mm512_max_epi32(t15, t7 );
    // v7  = _mm512_min_epi32(t15, t7 );

    m0 = _mm512_cmp_epi32_mask(t0 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t1 , t9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t2 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t3 , t11, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(t4 , t12, _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(t5 , t13, _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(t6 , t14, _MM_CMPINT_GT);
    m7 = _mm512_cmp_epi32_mask(t7 , t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_epi32(t0 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m0, t0 , _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_epi32(t1 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m1, t1 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_epi32(t2 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m2, t2 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_epi32(t3 , m3, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_epi32(t11, m3, t3 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_epi32(t4 , m4, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_epi32(t12, m4, t4 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m5, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_epi32(t13, m5, t5 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_epi32(t6 , m6, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_epi32(t14, m6, t6 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_epi32(t15, m7, t7 , _MM_SWIZ_REG_NONE);

    vlp0  = _mm512_mask_swizzle_epi64(tlp0 , m0, tlp8 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m0, tlp0 , _MM_SWIZ_REG_NONE);
    vlp1  = _mm512_mask_swizzle_epi64(tlp1 , m1, tlp9 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m1, tlp1 , _MM_SWIZ_REG_NONE);
    vlp2  = _mm512_mask_swizzle_epi64(tlp2 , m2, tlp10, _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m2, tlp2 , _MM_SWIZ_REG_NONE);
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , m3, tlp11, _MM_SWIZ_REG_NONE);
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, m3, tlp3 , _MM_SWIZ_REG_NONE);
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , m4, tlp12, _MM_SWIZ_REG_NONE);
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, m4, tlp4 , _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m5, tlp13, _MM_SWIZ_REG_NONE);
    vlp13 = _mm512_mask_swizzle_epi64(tlp13, m5, tlp5 , _MM_SWIZ_REG_NONE);
    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m6, tlp14, _MM_SWIZ_REG_NONE);
    vlp14 = _mm512_mask_swizzle_epi64(tlp14, m6, tlp6 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m7, tlp15, _MM_SWIZ_REG_NONE);
    vlp15 = _mm512_mask_swizzle_epi64(tlp15, m7, tlp7 , _MM_SWIZ_REG_NONE);

    vhp0  = _mm512_mask_swizzle_epi64(thp0 , m0>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m0>>8, thp0 , _MM_SWIZ_REG_NONE);
    vhp1  = _mm512_mask_swizzle_epi64(thp1 , m1>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m1>>8, thp1 , _MM_SWIZ_REG_NONE);
    vhp2  = _mm512_mask_swizzle_epi64(thp2 , m2>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m2>>8, thp2 , _MM_SWIZ_REG_NONE);
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , m3>>8, thp11, _MM_SWIZ_REG_NONE);
    vhp11 = _mm512_mask_swizzle_epi64(thp11, m3>>8, thp3 , _MM_SWIZ_REG_NONE);
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , m4>>8, thp12, _MM_SWIZ_REG_NONE);
    vhp12 = _mm512_mask_swizzle_epi64(thp12, m4>>8, thp4 , _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m5>>8, thp13, _MM_SWIZ_REG_NONE);
    vhp13 = _mm512_mask_swizzle_epi64(thp13, m5>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m6>>8, thp14, _MM_SWIZ_REG_NONE);
    vhp14 = _mm512_mask_swizzle_epi64(thp14, m6>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m7>>8, thp15, _MM_SWIZ_REG_NONE);
    vhp15 = _mm512_mask_swizzle_epi64(thp15, m7>>8, thp7 , _MM_SWIZ_REG_NONE);
    // step 5 
    // t10 = _mm512_max_epi32(v10, v5 );
    // t5  = _mm512_min_epi32(v10, v5 );
    // t9  = _mm512_max_epi32(v9 , v6 );
    // t6  = _mm512_min_epi32(v9 , v6 );
    // t12 = _mm512_max_epi32(v12, v3 );
    // t3  = _mm512_min_epi32(v12, v3 );
    // t14 = _mm512_max_epi32(v14, v13);
    // t13 = _mm512_min_epi32(v14, v13);
    // t11 = _mm512_max_epi32(v11, v7 );
    // t7  = _mm512_min_epi32(v11, v7 );
    // t2  = _mm512_max_epi32(v2 , v1 );
    // t1  = _mm512_min_epi32(v2 , v1 );
    // t8  = _mm512_max_epi32(v8 , v4 );
    // t4  = _mm512_min_epi32(v8 , v4 );
    // t0  = v0 ;
    // t15 = v15;

    m0 = _mm512_cmp_epi32_mask(v5 , v10, _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v6 , v9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v3 , v12, _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v13, v14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v7 , v11, _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(v1 , v2 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_epi32_mask(v4 , v8 , _MM_CMPINT_GT);

    t5  = _mm512_mask_swizzle_epi32(v5 , m0, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m0, v5 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m1, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m1, v6 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m2, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m2, v3 , _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m3, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_epi32(v14, m3, v13, _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m4, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m4, v7 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_epi32(v1 , m5, v2 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_epi32(v2 , m5, v1 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m6, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m6, v4 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t15 = v15;

    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m0, vlp10, _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m0, vlp5 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m1, vlp9 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m1, vlp6 , _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m2, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m2, vlp3 , _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m3, vlp14, _MM_SWIZ_REG_NONE);
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, m3, vlp13, _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m4, vlp11, _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m4, vlp7 , _MM_SWIZ_REG_NONE);
    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , m5, vlp2 , _MM_SWIZ_REG_NONE);
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m5, vlp1 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m6, vlp8 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m6, vlp4 , _MM_SWIZ_REG_NONE);
    tlp0  = vlp0 ;
    tlp15 = vlp15;

    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m0>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m0>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m1>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m1>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m2>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m2>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m3>>8, vhp14, _MM_SWIZ_REG_NONE);
    thp14 = _mm512_mask_swizzle_epi64(vhp14, m3>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m4>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m4>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , m5>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m5>>8, vhp1 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m6>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m6>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp0  = vhp0 ;
    thp15 = vhp15;
    // step 6 
    // v4  = _mm512_max_epi32(t4 , t1 );
    // v1  = _mm512_min_epi32(t4 , t1 );
    // v13 = _mm512_max_epi32(t13, t7 );
    // v7  = _mm512_min_epi32(t13, t7 );
    // v8  = _mm512_max_epi32(t8 , t2 );
    // v2  = _mm512_min_epi32(t8 , t2 );
    // v14 = _mm512_max_epi32(t14, t11);
    // v11 = _mm512_min_epi32(t14, t11);
    // v6  = _mm512_max_epi32(t6 , t5 );
    // v5  = _mm512_min_epi32(t6 , t5 );
    // v10 = _mm512_max_epi32(t10, t9 );
    // v9  = _mm512_min_epi32(t10, t9 );
    // v0  = t0 ;
    // v3  = t3 ;
    // v12 = t12;
    // v15 = t15;

    m0 = _mm512_cmp_epi32_mask(t1 , t4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t7 , t13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t2 , t8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t11, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(t5 , t6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_epi32_mask(t9 , t10, _MM_CMPINT_GT);

    v1  = _mm512_mask_swizzle_epi32(t1 , m0, t4 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_epi32(t4 , m0, t1 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m1, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_epi32(t13, m1, t7 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_epi32(t2 , m2, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m2, t2 , _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_epi32(t11, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_epi32(t14, m3, t11, _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m4, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_epi32(t6 , m4, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m5, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m5, t9 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v3  = t3 ;
    v12 = t12;
    v15 = t15;

    vlp1  = _mm512_mask_swizzle_epi64(tlp1 , m0, tlp4 , _MM_SWIZ_REG_NONE);
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , m0, tlp1 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m1, tlp13, _MM_SWIZ_REG_NONE);
    vlp13 = _mm512_mask_swizzle_epi64(tlp13, m1, tlp7 , _MM_SWIZ_REG_NONE);
    vlp2  = _mm512_mask_swizzle_epi64(tlp2 , m2, tlp8 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m2, tlp2 , _MM_SWIZ_REG_NONE);
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, m3, tlp14, _MM_SWIZ_REG_NONE);
    vlp14 = _mm512_mask_swizzle_epi64(tlp14, m3, tlp11, _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m4, tlp6 , _MM_SWIZ_REG_NONE);
    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m4, tlp5 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m5, tlp10, _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m5, tlp9 , _MM_SWIZ_REG_NONE);
    vlp0  = tlp0 ;
    vlp3  = tlp3 ;
    vlp12 = tlp12;
    vlp15 = tlp15;

    vhp1  = _mm512_mask_swizzle_epi64(thp1 , m0>>8, thp4 , _MM_SWIZ_REG_NONE);
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , m0>>8, thp1 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m1>>8, thp13, _MM_SWIZ_REG_NONE);
    vhp13 = _mm512_mask_swizzle_epi64(thp13, m1>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp2  = _mm512_mask_swizzle_epi64(thp2 , m2>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m2>>8, thp2 , _MM_SWIZ_REG_NONE);
    vhp11 = _mm512_mask_swizzle_epi64(thp11, m3>>8, thp14, _MM_SWIZ_REG_NONE);
    vhp14 = _mm512_mask_swizzle_epi64(thp14, m3>>8, thp11, _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m4>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m4>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m5>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m5>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp0  = thp0 ;
    vhp3  = thp3 ;
    vhp12 = thp12;
    vhp15 = thp15;
    // step 7 
    // t4  = _mm512_max_epi32(v4 , v2 );
    // t2  = _mm512_min_epi32(v4 , v2 );
    // t13 = _mm512_max_epi32(v13, v11);
    // t11 = _mm512_min_epi32(v13, v11);
    // t8  = _mm512_max_epi32(v8 , v3 );
    // t3  = _mm512_min_epi32(v8 , v3 );
    // t12 = _mm512_max_epi32(v12, v7 );
    // t7  = _mm512_min_epi32(v12, v7 );
    // t0  = v0 ;
    // t1  = v1 ;
    // t5  = v5 ;
    // t6  = v6 ;
    // t9  = v9 ;
    // t10 = v10;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_epi32_mask(v2 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v11, v13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v3 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v7 , v12, _MM_CMPINT_GT);

    t2  = _mm512_mask_swizzle_epi32(v2 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m0, v2 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m1, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_epi32(v13, m1, v11, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_epi32(v3 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m2, v3 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m3, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m3, v7 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t1  = v1 ;
    t5  = v5 ;
    t6  = v6 ;
    t9  = v9 ;
    t10 = v10;
    t14 = v14;
    t15 = v15;

    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m0, vlp4 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m0, vlp2 , _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m1, vlp13, _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m1, vlp11, _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m2, vlp8 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m2, vlp3 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m3, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m3, vlp7 , _MM_SWIZ_REG_NONE);
    tlp0  = vlp0 ;
    tlp1  = vlp1 ;
    tlp5  = vlp5 ;
    tlp6  = vlp6 ;
    tlp9  = vlp9 ;
    tlp10 = vlp10;
    tlp14 = vlp14;
    tlp15 = vlp15;

    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m0>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m0>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m1>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m1>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m2>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m2>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m3>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m3>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp0  = vhp0 ;
    thp1  = vhp1 ;
    thp5  = vhp5 ;
    thp6  = vhp6 ;
    thp9  = vhp9 ;
    thp10 = vhp10;
    thp14 = vhp14;
    thp15 = vhp15;
    // step 8 
    // v8  = _mm512_max_epi32(t8 , t6 );
    // v6  = _mm512_min_epi32(t8 , t6 );
    // v12 = _mm512_max_epi32(t12, t10);
    // v10 = _mm512_min_epi32(t12, t10);
    // v5  = _mm512_max_epi32(t5 , t3 );
    // v3  = _mm512_min_epi32(t5 , t3 );
    // v9  = _mm512_max_epi32(t9 , t7 );
    // v7  = _mm512_min_epi32(t9 , t7 );
    // v0  = t0;
    // v1  = t1;
    // v2  = t2;
    // v4  = t4;
    // v11 = t11;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_epi32_mask(t6 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t10, t12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(t3 , t5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(t7 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_epi32(t6 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m0, t6 , _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_epi32(t10, m1, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_epi32(t12, m1, t10, _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_epi32(t3 , m2, t5 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_epi32(t5 , m2, t3 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m3, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m3, t7 , _MM_SWIZ_REG_NONE);
    v0  = t0;
    v1  = t1;
    v2  = t2;
    v4  = t4;
    v11 = t11;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m0, tlp8 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m0, tlp6 , _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m1, tlp12, _MM_SWIZ_REG_NONE);
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, m1, tlp10, _MM_SWIZ_REG_NONE);
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , m2, tlp5 , _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m2, tlp3 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m3, tlp9 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m3, tlp7 , _MM_SWIZ_REG_NONE);
    vlp0  = tlp0;
    vlp1  = tlp1;
    vlp2  = tlp2;
    vlp4  = tlp4;
    vlp11 = tlp11;
    vlp13 = tlp13;
    vlp14 = tlp14;
    vlp15 = tlp15;

    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m0>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m0>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m1>>8, thp12, _MM_SWIZ_REG_NONE);
    vhp12 = _mm512_mask_swizzle_epi64(thp12, m1>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , m2>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m2>>8, thp3 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m3>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m3>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp0  = thp0;
    vhp1  = thp1;
    vhp2  = thp2;
    vhp4  = thp4;
    vhp11 = thp11;
    vhp13 = thp13;
    vhp14 = thp14;
    vhp15 = thp15;
    // step 9 
    // t4  = _mm512_max_epi32(v4 , v3 );
    // t3  = _mm512_min_epi32(v4 , v3 );
    // t6  = _mm512_max_epi32(v6 , v5 );
    // t5  = _mm512_min_epi32(v6 , v5 );
    // t8  = _mm512_max_epi32(v8 , v7 );
    // t7  = _mm512_min_epi32(v8 , v7 );
    // t10 = _mm512_max_epi32(v10, v9 );
    // t9  = _mm512_min_epi32(v10, v9 );
    // t12 = _mm512_max_epi32(v12, v11);
    // t11 = _mm512_min_epi32(v12, v11);
    // t0  = v0;
    // t1  = v1;
    // t2  = v2;
    // t13 = v13;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_epi32_mask(v3 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(v5 , v6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_epi32_mask(v7 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_epi32_mask(v9 , v10, _MM_CMPINT_GT);
    m4 = _mm512_cmp_epi32_mask(v11, v12, _MM_CMPINT_GT);

    t3  = _mm512_mask_swizzle_epi32(v3 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_epi32(v4 , m0, v3 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_epi32(v5 , m1, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_epi32(v6 , m1, v5 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_epi32(v7 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_epi32(v8 , m2, v7 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_epi32(v9 , m3, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_epi32(v10, m3, v9 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_epi32(v11, m4, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_epi32(v12, m4, v11, _MM_SWIZ_REG_NONE);
    t0  = v0;
    t1  = v1;
    t2  = v2;
    t13 = v13;
    t14 = v14;
    t15 = v15;

    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m0, vlp4 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m0, vlp3 , _MM_SWIZ_REG_NONE);
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m1, vlp6 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m1, vlp5 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m2, vlp8 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m2, vlp7 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m3, vlp10, _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m3, vlp9 , _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m4, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m4, vlp11, _MM_SWIZ_REG_NONE);
    tlp0  = vlp0;
    tlp1  = vlp1;
    tlp2  = vlp2;
    tlp13 = vlp13;
    tlp14 = vlp14;
    tlp15 = vlp15;

    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m0>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m0>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m1>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m1>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m2>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m2>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m3>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m3>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m4>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m4>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp0  = vhp0;
    thp1  = vhp1;
    thp2  = vhp2;
    thp13 = vhp13;
    thp14 = vhp14;
    thp15 = vhp15;
    // step 10 
    // v7  = _mm512_max_epi32(t7 , t6 );
    // v6  = _mm512_min_epi32(t7 , t6 );
    // v9  = _mm512_max_epi32(t9 , t8 );
    // v8  = _mm512_min_epi32(t9 , t8 );
    // v0  = t0 ;
    // v1  = t1 ;
    // v2  = t2 ;
    // v3  = t3 ;
    // v4  = t4 ;
    // v5  = t5 ;
    // v10 = t10;
    // v11 = t11;
    // v12 = t12;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_epi32_mask(t6 , t7 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_epi32_mask(t8 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_epi32(t6 , m0, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_epi32(t7 , m0, t6 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_epi32(t8 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_epi32(t9 , m1, t8 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v1  = t1 ;
    v2  = t2 ;
    v3  = t3 ;
    v4  = t4 ;
    v5  = t5 ;
    v10 = t10;
    v11 = t11;
    v12 = t12;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m0, tlp7 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m0, tlp6 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m1, tlp9 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m1, tlp8 , _MM_SWIZ_REG_NONE);
    vlp0  = tlp0 ;
    vlp1  = tlp1 ;
    vlp2  = tlp2 ;
    vlp3  = tlp3 ;
    vlp4  = tlp4 ;
    vlp5  = tlp5 ;
    vlp10 = tlp10;
    vlp11 = tlp11;
    vlp12 = tlp12;
    vlp13 = tlp13;
    vlp14 = tlp14;
    vlp15 = tlp15;

    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m0>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m0>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m1>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m1>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp0  = thp0 ;
    vhp1  = thp1 ;
    vhp2  = thp2 ;
    vhp3  = thp3 ;
    vhp4  = thp4 ;
    vhp5  = thp5 ;
    vhp10 = thp10;
    vhp11 = thp11;
    vhp12 = thp12;
    vhp13 = thp13;
    vhp14 = thp14;
    vhp15 = thp15;

}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_sort_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3 , 
        __m512i &vp4 , __m512i &vp5 , __m512i &vp6 , __m512i &vp7 , 
        __m512i &vp8 , __m512i &vp9 , __m512i &vp10, __m512i &vp11, 
        __m512i &vp12, __m512i &vp13, __m512i &vp14, __m512i &vp15)
{
    __m512 t0 ;
    __m512 t1 ;
    __m512 t2 ;
    __m512 t3 ;
    __m512 t4 ;
    __m512 t5 ;
    __m512 t6 ;
    __m512 t7 ;
    __m512 t8 ;
    __m512 t9 ;
    __m512 t10;
    __m512 t11;
    __m512 t12;
    __m512 t13;
    __m512 t14;
    __m512 t15;

    __m512i tp0 ;
    __m512i tp1 ;
    __m512i tp2 ;
    __m512i tp3 ;
    __m512i tp4 ;
    __m512i tp5 ;
    __m512i tp6 ;
    __m512i tp7 ;
    __m512i tp8 ;
    __m512i tp9 ;
    __m512i tp10;
    __m512i tp11;
    __m512i tp12;
    __m512i tp13;
    __m512i tp14;
    __m512i tp15;

    __mmask16 m0 ;
    __mmask16 m1 ;
    __mmask16 m2 ;
    __mmask16 m3 ;
    __mmask16 m4 ;
    __mmask16 m5 ;
    __mmask16 m6 ;
    __mmask16 m7 ;

    // step 1 
    // t1  = _mm512_max_ps(v1 , v0 );
    // t0  = _mm512_min_ps(v1 , v0 );
    // t3  = _mm512_max_ps(v3 , v2 );
    // t2  = _mm512_min_ps(v3 , v2 );
    // t5  = _mm512_max_ps(v5 , v4 );
    // t4  = _mm512_min_ps(v5 , v4 );
    // t7  = _mm512_max_ps(v7 , v6 );
    // t6  = _mm512_min_ps(v7 , v6 );
    // t9  = _mm512_max_ps(v9 , v8 );
    // t8  = _mm512_min_ps(v9 , v8 );
    // t11 = _mm512_max_ps(v11, v10);
    // t10 = _mm512_min_ps(v11, v10);
    // t13 = _mm512_max_ps(v13, v12);
    // t12 = _mm512_min_ps(v13, v12);
    // t15 = _mm512_max_ps(v15, v14);
    // t14 = _mm512_min_ps(v15, v14);

    m0 = _mm512_cmp_ps_mask(v0 , v1 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v2 , v3 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v4 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v6 , v7 , _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v8 , v9 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(v10, v11, _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(v12, v13, _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(v14, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_ps(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_ps(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_ps(v2 , m1, v3 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m1, v2 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_ps(v5 , m2, v4 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m3, v6 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m4, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m4, v8 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m5, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m5, v10, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m6, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m6, v12, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_ps(v14, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_ps(v15, m7, v14, _MM_SWIZ_REG_NONE);

    tp0  = _mm512_mask_swizzle_epi32(vp0 , m0, vp1 , _MM_SWIZ_REG_NONE);
    tp1  = _mm512_mask_swizzle_epi32(vp1 , m0, vp0 , _MM_SWIZ_REG_NONE);
    tp2  = _mm512_mask_swizzle_epi32(vp2 , m1, vp3 , _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m1, vp2 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m2, vp5 , _MM_SWIZ_REG_NONE);
    tp5  = _mm512_mask_swizzle_epi32(vp5 , m2, vp4 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m3, vp7 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m3, vp6 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m4, vp9 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m4, vp8 , _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m5, vp11, _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m5, vp10, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m6, vp13, _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m6, vp12, _MM_SWIZ_REG_NONE);
    tp14 = _mm512_mask_swizzle_epi32(vp14, m7, vp15, _MM_SWIZ_REG_NONE);
    tp15 = _mm512_mask_swizzle_epi32(vp15, m7, vp14, _MM_SWIZ_REG_NONE);

    // step 2 
    // v2  = _mm512_max_ps(t2 , t0 );
    // v0  = _mm512_min_ps(t2 , t0 );
    // v6  = _mm512_max_ps(t6 , t4 );
    // v4  = _mm512_min_ps(t6 , t4 );
    // v10 = _mm512_max_ps(t10, t8 );
    // v8  = _mm512_min_ps(t10, t8 );
    // v14 = _mm512_max_ps(t14, t12);
    // v12 = _mm512_min_ps(t14, t12);
    // v3  = _mm512_max_ps(t3 , t1 );
    // v1  = _mm512_min_ps(t3 , t1 );
    // v7  = _mm512_max_ps(t7 , t5 );
    // v5  = _mm512_min_ps(t7 , t5 );
    // v11 = _mm512_max_ps(t11, t9 );
    // v9  = _mm512_min_ps(t11, t9 );
    // v15 = _mm512_max_ps(t15, t13);
    // v13 = _mm512_min_ps(t15, t13);

    m0 = _mm512_cmp_ps_mask(t0 , t2 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t4 , t6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t8 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t12, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(t1 , t3 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(t5 , t7 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(t9 , t11, _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(t13, t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_ps(t0 , m0, t2 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_ps(t2 , m0, t0 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_ps(t4 , m1, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_ps(t6 , m1, t4 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m2, t8 , _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_ps(t12, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_ps(t14, m3, t12, _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_ps(t1 , m4, t3 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_ps(t3 , m4, t1 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m5, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m5, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m6, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_ps(t11, m6, t9 , _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_ps(t13, m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_ps(t15, m7, t13, _MM_SWIZ_REG_NONE);

    vp0  = _mm512_mask_swizzle_epi32(tp0 , m0, tp2 , _MM_SWIZ_REG_NONE);
    vp2  = _mm512_mask_swizzle_epi32(tp2 , m0, tp0 , _MM_SWIZ_REG_NONE);
    vp4  = _mm512_mask_swizzle_epi32(tp4 , m1, tp6 , _MM_SWIZ_REG_NONE);
    vp6  = _mm512_mask_swizzle_epi32(tp6 , m1, tp4 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m2, tp10, _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m2, tp8 , _MM_SWIZ_REG_NONE);
    vp12 = _mm512_mask_swizzle_epi32(tp12, m3, tp14, _MM_SWIZ_REG_NONE);
    vp14 = _mm512_mask_swizzle_epi32(tp14, m3, tp12, _MM_SWIZ_REG_NONE);
    vp1  = _mm512_mask_swizzle_epi32(tp1 , m4, tp3 , _MM_SWIZ_REG_NONE);
    vp3  = _mm512_mask_swizzle_epi32(tp3 , m4, tp1 , _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m5, tp7 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m5, tp5 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m6, tp11, _MM_SWIZ_REG_NONE);
    vp11 = _mm512_mask_swizzle_epi32(tp11, m6, tp9 , _MM_SWIZ_REG_NONE);
    vp13 = _mm512_mask_swizzle_epi32(tp13, m7, tp15, _MM_SWIZ_REG_NONE);
    vp15 = _mm512_mask_swizzle_epi32(tp15, m7, tp13, _MM_SWIZ_REG_NONE);

    // step 3 
    // t4  = _mm512_max_ps(v4 , v0 );
    // t0  = _mm512_min_ps(v4 , v0 );
    // t12 = _mm512_max_ps(v12, v8 );
    // t8  = _mm512_min_ps(v12, v8 );
    // t5  = _mm512_max_ps(v5 , v1 );
    // t1  = _mm512_min_ps(v5 , v1 );
    // t13 = _mm512_max_ps(v13, v9 );
    // t9  = _mm512_min_ps(v13, v9 );
    // t6  = _mm512_max_ps(v6 , v2 );
    // t2  = _mm512_min_ps(v6 , v2 );
    // t14 = _mm512_max_ps(v14, v10);
    // t10 = _mm512_min_ps(v14, v10);
    // t7  = _mm512_max_ps(v7 , v3 );
    // t3  = _mm512_min_ps(v7 , v3 );
    // t15 = _mm512_max_ps(v15, v11);
    // t11 = _mm512_min_ps(v15, v11);

    m0 = _mm512_cmp_ps_mask(v0 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v8 , v12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v1 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v9 , v13, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v2 , v6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(v10, v14, _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(v3 , v7 , _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(v11, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_ps(v0 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m0, v0 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m1, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m1, v8 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_ps(v1 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_ps(v5 , m2, v1 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m3, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m3, v9 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_ps(v2 , m4, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m4, v2 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m5, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_ps(v14, m5, v10, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m6, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m6, v3 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_ps(v15, m7, v11, _MM_SWIZ_REG_NONE);

    tp0  = _mm512_mask_swizzle_epi32(vp0 , m0, vp4 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m0, vp0 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m1, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m1, vp8 , _MM_SWIZ_REG_NONE);
    tp1  = _mm512_mask_swizzle_epi32(vp1 , m2, vp5 , _MM_SWIZ_REG_NONE);
    tp5  = _mm512_mask_swizzle_epi32(vp5 , m2, vp1 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m3, vp13, _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m3, vp9 , _MM_SWIZ_REG_NONE);
    tp2  = _mm512_mask_swizzle_epi32(vp2 , m4, vp6 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m4, vp2 , _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m5, vp14, _MM_SWIZ_REG_NONE);
    tp14 = _mm512_mask_swizzle_epi32(vp14, m5, vp10, _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m6, vp7 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m6, vp3 , _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m7, vp15, _MM_SWIZ_REG_NONE);
    tp15 = _mm512_mask_swizzle_epi32(vp15, m7, vp11, _MM_SWIZ_REG_NONE);
    // step 4 
    // v8  = _mm512_max_ps(t8 , t0 );
    // v0  = _mm512_min_ps(t8 , t0 );
    // v9  = _mm512_max_ps(t9 , t1 );
    // v1  = _mm512_min_ps(t9 , t1 );
    // v10 = _mm512_max_ps(t10, t2 );
    // v2  = _mm512_min_ps(t10, t2 );
    // v11 = _mm512_max_ps(t11, t3 );
    // v3  = _mm512_min_ps(t11, t3 );
    // v12 = _mm512_max_ps(t12, t4 );
    // v4  = _mm512_min_ps(t12, t4 );
    // v13 = _mm512_max_ps(t13, t5 );
    // v5  = _mm512_min_ps(t13, t5 );
    // v14 = _mm512_max_ps(t14, t6 );
    // v6  = _mm512_min_ps(t14, t6 );
    // v15 = _mm512_max_ps(t15, t7 );
    // v7  = _mm512_min_ps(t15, t7 );

    m0 = _mm512_cmp_ps_mask(t0 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t1 , t9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t2 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t3 , t11, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(t4 , t12, _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(t5 , t13, _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(t6 , t14, _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(t7 , t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_ps(t0 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m0, t0 , _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_ps(t1 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m1, t1 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_ps(t2 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m2, t2 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_ps(t3 , m3, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_ps(t11, m3, t3 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_ps(t4 , m4, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_ps(t12, m4, t4 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m5, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_ps(t13, m5, t5 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_ps(t6 , m6, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_ps(t14, m6, t6 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_ps(t15, m7, t7 , _MM_SWIZ_REG_NONE);

    vp0  = _mm512_mask_swizzle_epi32(tp0 , m0, tp8 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m0, tp0 , _MM_SWIZ_REG_NONE);
    vp1  = _mm512_mask_swizzle_epi32(tp1 , m1, tp9 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m1, tp1 , _MM_SWIZ_REG_NONE);
    vp2  = _mm512_mask_swizzle_epi32(tp2 , m2, tp10, _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m2, tp2 , _MM_SWIZ_REG_NONE);
    vp3  = _mm512_mask_swizzle_epi32(tp3 , m3, tp11, _MM_SWIZ_REG_NONE);
    vp11 = _mm512_mask_swizzle_epi32(tp11, m3, tp3 , _MM_SWIZ_REG_NONE);
    vp4  = _mm512_mask_swizzle_epi32(tp4 , m4, tp12, _MM_SWIZ_REG_NONE);
    vp12 = _mm512_mask_swizzle_epi32(tp12, m4, tp4 , _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m5, tp13, _MM_SWIZ_REG_NONE);
    vp13 = _mm512_mask_swizzle_epi32(tp13, m5, tp5 , _MM_SWIZ_REG_NONE);
    vp6  = _mm512_mask_swizzle_epi32(tp6 , m6, tp14, _MM_SWIZ_REG_NONE);
    vp14 = _mm512_mask_swizzle_epi32(tp14, m6, tp6 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m7, tp15, _MM_SWIZ_REG_NONE);
    vp15 = _mm512_mask_swizzle_epi32(tp15, m7, tp7 , _MM_SWIZ_REG_NONE);

    // step 5 
    // t10 = _mm512_max_ps(v10, v5 );
    // t5  = _mm512_min_ps(v10, v5 );
    // t9  = _mm512_max_ps(v9 , v6 );
    // t6  = _mm512_min_ps(v9 , v6 );
    // t12 = _mm512_max_ps(v12, v3 );
    // t3  = _mm512_min_ps(v12, v3 );
    // t14 = _mm512_max_ps(v14, v13);
    // t13 = _mm512_min_ps(v14, v13);
    // t11 = _mm512_max_ps(v11, v7 );
    // t7  = _mm512_min_ps(v11, v7 );
    // t2  = _mm512_max_ps(v2 , v1 );
    // t1  = _mm512_min_ps(v2 , v1 );
    // t8  = _mm512_max_ps(v8 , v4 );
    // t4  = _mm512_min_ps(v8 , v4 );
    // t0  = v0 ;
    // t15 = v15;

    m0 = _mm512_cmp_ps_mask(v5 , v10, _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v6 , v9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v3 , v12, _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v13, v14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v7 , v11, _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(v1 , v2 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(v4 , v8 , _MM_CMPINT_GT);

    t5  = _mm512_mask_swizzle_ps(v5 , m0, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m0, v5 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m1, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m1, v6 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m2, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m2, v3 , _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m3, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_ps(v14, m3, v13, _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m4, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m4, v7 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_ps(v1 , m5, v2 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_ps(v2 , m5, v1 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m6, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m6, v4 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t15 = v15;

    tp5  = _mm512_mask_swizzle_epi32(vp5 , m0, vp10, _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m0, vp5 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m1, vp9 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m1, vp6 , _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m2, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m2, vp3 , _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m3, vp14, _MM_SWIZ_REG_NONE);
    tp14 = _mm512_mask_swizzle_epi32(vp14, m3, vp13, _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m4, vp11, _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m4, vp7 , _MM_SWIZ_REG_NONE);
    tp1  = _mm512_mask_swizzle_epi32(vp1 , m5, vp2 , _MM_SWIZ_REG_NONE);
    tp2  = _mm512_mask_swizzle_epi32(vp2 , m5, vp1 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m6, vp8 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m6, vp4 , _MM_SWIZ_REG_NONE);
    tp0  = vp0 ;
    tp15 = vp15;

    // step 6 
    // v4  = _mm512_max_ps(t4 , t1 );
    // v1  = _mm512_min_ps(t4 , t1 );
    // v13 = _mm512_max_ps(t13, t7 );
    // v7  = _mm512_min_ps(t13, t7 );
    // v8  = _mm512_max_ps(t8 , t2 );
    // v2  = _mm512_min_ps(t8 , t2 );
    // v14 = _mm512_max_ps(t14, t11);
    // v11 = _mm512_min_ps(t14, t11);
    // v6  = _mm512_max_ps(t6 , t5 );
    // v5  = _mm512_min_ps(t6 , t5 );
    // v10 = _mm512_max_ps(t10, t9 );
    // v9  = _mm512_min_ps(t10, t9 );
    // v0  = t0 ;
    // v3  = t3 ;
    // v12 = t12;
    // v15 = t15;

    m0 = _mm512_cmp_ps_mask(t1 , t4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t7 , t13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t2 , t8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t11, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(t5 , t6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(t9 , t10, _MM_CMPINT_GT);

    v1  = _mm512_mask_swizzle_ps(t1 , m0, t4 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_ps(t4 , m0, t1 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m1, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_ps(t13, m1, t7 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_ps(t2 , m2, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m2, t2 , _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_ps(t11, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_ps(t14, m3, t11, _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m4, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_ps(t6 , m4, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m5, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m5, t9 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v3  = t3 ;
    v12 = t12;
    v15 = t15;

    vp1  = _mm512_mask_swizzle_epi32(tp1 , m0, tp4 , _MM_SWIZ_REG_NONE);
    vp4  = _mm512_mask_swizzle_epi32(tp4 , m0, tp1 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m1, tp13, _MM_SWIZ_REG_NONE);
    vp13 = _mm512_mask_swizzle_epi32(tp13, m1, tp7 , _MM_SWIZ_REG_NONE);
    vp2  = _mm512_mask_swizzle_epi32(tp2 , m2, tp8 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m2, tp2 , _MM_SWIZ_REG_NONE);
    vp11 = _mm512_mask_swizzle_epi32(tp11, m3, tp14, _MM_SWIZ_REG_NONE);
    vp14 = _mm512_mask_swizzle_epi32(tp14, m3, tp11, _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m4, tp6 , _MM_SWIZ_REG_NONE);
    vp6  = _mm512_mask_swizzle_epi32(tp6 , m4, tp5 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m5, tp10, _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m5, tp9 , _MM_SWIZ_REG_NONE);
    vp0  = tp0 ;
    vp3  = tp3 ;
    vp12 = tp12;
    vp15 = tp15;

    // step 7 
    // t4  = _mm512_max_ps(v4 , v2 );
    // t2  = _mm512_min_ps(v4 , v2 );
    // t13 = _mm512_max_ps(v13, v11);
    // t11 = _mm512_min_ps(v13, v11);
    // t8  = _mm512_max_ps(v8 , v3 );
    // t3  = _mm512_min_ps(v8 , v3 );
    // t12 = _mm512_max_ps(v12, v7 );
    // t7  = _mm512_min_ps(v12, v7 );
    // t0  = v0 ;
    // t1  = v1 ;
    // t5  = v5 ;
    // t6  = v6 ;
    // t9  = v9 ;
    // t10 = v10;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_ps_mask(v2 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v11, v13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v3 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v7 , v12, _MM_CMPINT_GT);

    t2  = _mm512_mask_swizzle_ps(v2 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m0, v2 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m1, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m1, v11, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m2, v3 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m3, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m3, v7 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t1  = v1 ;
    t5  = v5 ;
    t6  = v6 ;
    t9  = v9 ;
    t10 = v10;
    t14 = v14;
    t15 = v15;

    tp2  = _mm512_mask_swizzle_epi32(vp2 , m0, vp4 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m0, vp2 , _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m1, vp13, _MM_SWIZ_REG_NONE);
    tp13 = _mm512_mask_swizzle_epi32(vp13, m1, vp11, _MM_SWIZ_REG_NONE);
    tp3  = _mm512_mask_swizzle_epi32(vp3 , m2, vp8 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m2, vp3 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m3, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m3, vp7 , _MM_SWIZ_REG_NONE);
    tp0  = vp0 ;
    tp1  = vp1 ;
    tp5  = vp5 ;
    tp6  = vp6 ;
    tp9  = vp9 ;
    tp10 = vp10;
    tp14 = vp14;
    tp15 = vp15;

    // step 8 
    // v8  = _mm512_max_ps(t8 , t6 );
    // v6  = _mm512_min_ps(t8 , t6 );
    // v12 = _mm512_max_ps(t12, t10);
    // v10 = _mm512_min_ps(t12, t10);
    // v5  = _mm512_max_ps(t5 , t3 );
    // v3  = _mm512_min_ps(t5 , t3 );
    // v9  = _mm512_max_ps(t9 , t7 );
    // v7  = _mm512_min_ps(t9 , t7 );
    // v0  = t0;
    // v1  = t1;
    // v2  = t2;
    // v4  = t4;
    // v11 = t11;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_ps_mask(t6 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t10, t12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t3 , t5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t7 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_ps(t6 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m0, t6 , _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m1, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_ps(t12, m1, t10, _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_ps(t3 , m2, t5 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m2, t3 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m3, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m3, t7 , _MM_SWIZ_REG_NONE);
    v0  = t0;
    v1  = t1;
    v2  = t2;
    v4  = t4;
    v11 = t11;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vp6  = _mm512_mask_swizzle_epi32(tp6 , m0, tp8 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m0, tp6 , _MM_SWIZ_REG_NONE);
    vp10 = _mm512_mask_swizzle_epi32(tp10, m1, tp12, _MM_SWIZ_REG_NONE);
    vp12 = _mm512_mask_swizzle_epi32(tp12, m1, tp10, _MM_SWIZ_REG_NONE);
    vp3  = _mm512_mask_swizzle_epi32(tp3 , m2, tp5 , _MM_SWIZ_REG_NONE);
    vp5  = _mm512_mask_swizzle_epi32(tp5 , m2, tp3 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m3, tp9 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m3, tp7 , _MM_SWIZ_REG_NONE);
    vp0  = tp0;
    vp1  = tp1;
    vp2  = tp2;
    vp4  = tp4;
    vp11 = tp11;
    vp13 = tp13;
    vp14 = tp14;
    vp15 = tp15;

    // step 9 
    // t4  = _mm512_max_ps(v4 , v3 );
    // t3  = _mm512_min_ps(v4 , v3 );
    // t6  = _mm512_max_ps(v6 , v5 );
    // t5  = _mm512_min_ps(v6 , v5 );
    // t8  = _mm512_max_ps(v8 , v7 );
    // t7  = _mm512_min_ps(v8 , v7 );
    // t10 = _mm512_max_ps(v10, v9 );
    // t9  = _mm512_min_ps(v10, v9 );
    // t12 = _mm512_max_ps(v12, v11);
    // t11 = _mm512_min_ps(v12, v11);
    // t0  = v0;
    // t1  = v1;
    // t2  = v2;
    // t13 = v13;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_ps_mask(v3 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v5 , v6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v7 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v9 , v10, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v11, v12, _MM_CMPINT_GT);

    t3  = _mm512_mask_swizzle_ps(v3 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m0, v3 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_ps(v5 , m1, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m1, v5 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m2, v7 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m3, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m3, v9 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m4, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m4, v11, _MM_SWIZ_REG_NONE);
    t0  = v0;
    t1  = v1;
    t2  = v2;
    t13 = v13;
    t14 = v14;
    t15 = v15;

    tp3  = _mm512_mask_swizzle_epi32(vp3 , m0, vp4 , _MM_SWIZ_REG_NONE);
    tp4  = _mm512_mask_swizzle_epi32(vp4 , m0, vp3 , _MM_SWIZ_REG_NONE);
    tp5  = _mm512_mask_swizzle_epi32(vp5 , m1, vp6 , _MM_SWIZ_REG_NONE);
    tp6  = _mm512_mask_swizzle_epi32(vp6 , m1, vp5 , _MM_SWIZ_REG_NONE);
    tp7  = _mm512_mask_swizzle_epi32(vp7 , m2, vp8 , _MM_SWIZ_REG_NONE);
    tp8  = _mm512_mask_swizzle_epi32(vp8 , m2, vp7 , _MM_SWIZ_REG_NONE);
    tp9  = _mm512_mask_swizzle_epi32(vp9 , m3, vp10, _MM_SWIZ_REG_NONE);
    tp10 = _mm512_mask_swizzle_epi32(vp10, m3, vp9 , _MM_SWIZ_REG_NONE);
    tp11 = _mm512_mask_swizzle_epi32(vp11, m4, vp12, _MM_SWIZ_REG_NONE);
    tp12 = _mm512_mask_swizzle_epi32(vp12, m4, vp11, _MM_SWIZ_REG_NONE);
    tp0  = vp0;
    tp1  = vp1;
    tp2  = vp2;
    tp13 = vp13;
    tp14 = vp14;
    tp15 = vp15;
    // step 10 
    // v7  = _mm512_max_ps(t7 , t6 );
    // v6  = _mm512_min_ps(t7 , t6 );
    // v9  = _mm512_max_ps(t9 , t8 );
    // v8  = _mm512_min_ps(t9 , t8 );
    // v0  = t0 ;
    // v1  = t1 ;
    // v2  = t2 ;
    // v3  = t3 ;
    // v4  = t4 ;
    // v5  = t5 ;
    // v10 = t10;
    // v11 = t11;
    // v12 = t12;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_ps_mask(t6 , t7 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t8 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_ps(t6 , m0, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m0, t6 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m1, t8 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v1  = t1 ;
    v2  = t2 ;
    v3  = t3 ;
    v4  = t4 ;
    v5  = t5 ;
    v10 = t10;
    v11 = t11;
    v12 = t12;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vp6  = _mm512_mask_swizzle_epi32(tp6 , m0, tp7 , _MM_SWIZ_REG_NONE);
    vp7  = _mm512_mask_swizzle_epi32(tp7 , m0, tp6 , _MM_SWIZ_REG_NONE);
    vp8  = _mm512_mask_swizzle_epi32(tp8 , m1, tp9 , _MM_SWIZ_REG_NONE);
    vp9  = _mm512_mask_swizzle_epi32(tp9 , m1, tp8 , _MM_SWIZ_REG_NONE);
    vp0  = tp0 ;
    vp1  = tp1 ;
    vp2  = tp2 ;
    vp3  = tp3 ;
    vp4  = tp4 ;
    vp5  = tp5 ;
    vp10 = tp10;
    vp11 = tp11;
    vp12 = tp12;
    vp13 = tp13;
    vp14 = tp14;
    vp15 = tp15;

}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_sort_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vlp0 , __m512i &vlp1 , __m512i &vlp2 , __m512i &vlp3 , 
        __m512i &vlp4 , __m512i &vlp5 , __m512i &vlp6 , __m512i &vlp7 , 
        __m512i &vlp8 , __m512i &vlp9 , __m512i &vlp10, __m512i &vlp11, 
        __m512i &vlp12, __m512i &vlp13, __m512i &vlp14, __m512i &vlp15, 
        __m512i &vhp0 , __m512i &vhp1 , __m512i &vhp2 , __m512i &vhp3 , 
        __m512i &vhp4 , __m512i &vhp5 , __m512i &vhp6 , __m512i &vhp7 , 
        __m512i &vhp8 , __m512i &vhp9 , __m512i &vhp10, __m512i &vhp11, 
        __m512i &vhp12, __m512i &vhp13, __m512i &vhp14, __m512i &vhp15)
{
    __m512 t0 ;
    __m512 t1 ;
    __m512 t2 ;
    __m512 t3 ;
    __m512 t4 ;
    __m512 t5 ;
    __m512 t6 ;
    __m512 t7 ;
    __m512 t8 ;
    __m512 t9 ;
    __m512 t10;
    __m512 t11;
    __m512 t12;
    __m512 t13;
    __m512 t14;
    __m512 t15;

    __m512i tlp0 ;
    __m512i tlp1 ;
    __m512i tlp2 ;
    __m512i tlp3 ;
    __m512i tlp4 ;
    __m512i tlp5 ;
    __m512i tlp6 ;
    __m512i tlp7 ;
    __m512i tlp8 ;
    __m512i tlp9 ;
    __m512i tlp10;
    __m512i tlp11;
    __m512i tlp12;
    __m512i tlp13;
    __m512i tlp14;
    __m512i tlp15;

    __m512i thp0 ;
    __m512i thp1 ;
    __m512i thp2 ;
    __m512i thp3 ;
    __m512i thp4 ;
    __m512i thp5 ;
    __m512i thp6 ;
    __m512i thp7 ;
    __m512i thp8 ;
    __m512i thp9 ;
    __m512i thp10;
    __m512i thp11;
    __m512i thp12;
    __m512i thp13;
    __m512i thp14;
    __m512i thp15;

    __mmask16 m0 ;
    __mmask16 m1 ;
    __mmask16 m2 ;
    __mmask16 m3 ;
    __mmask16 m4 ;
    __mmask16 m5 ;
    __mmask16 m6 ;
    __mmask16 m7 ;

    // step 1 
    // t1  = _mm512_max_ps(v1 , v0 );
    // t0  = _mm512_min_ps(v1 , v0 );
    // t3  = _mm512_max_ps(v3 , v2 );
    // t2  = _mm512_min_ps(v3 , v2 );
    // t5  = _mm512_max_ps(v5 , v4 );
    // t4  = _mm512_min_ps(v5 , v4 );
    // t7  = _mm512_max_ps(v7 , v6 );
    // t6  = _mm512_min_ps(v7 , v6 );
    // t9  = _mm512_max_ps(v9 , v8 );
    // t8  = _mm512_min_ps(v9 , v8 );
    // t11 = _mm512_max_ps(v11, v10);
    // t10 = _mm512_min_ps(v11, v10);
    // t13 = _mm512_max_ps(v13, v12);
    // t12 = _mm512_min_ps(v13, v12);
    // t15 = _mm512_max_ps(v15, v14);
    // t14 = _mm512_min_ps(v15, v14);

    m0 = _mm512_cmp_ps_mask(v0 , v1 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v2 , v3 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v4 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v6 , v7 , _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v8 , v9 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(v10, v11, _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(v12, v13, _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(v14, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_ps(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_ps(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_ps(v2 , m1, v3 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m1, v2 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_ps(v5 , m2, v4 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m3, v6 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m4, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m4, v8 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m5, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m5, v10, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m6, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m6, v12, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_ps(v14, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_ps(v15, m7, v14, _MM_SWIZ_REG_NONE);

    tlp0  = _mm512_mask_swizzle_epi64(vlp0 , m0, vlp1 , _MM_SWIZ_REG_NONE);
    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , m0, vlp0 , _MM_SWIZ_REG_NONE);
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m1, vlp3 , _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m1, vlp2 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m2, vlp5 , _MM_SWIZ_REG_NONE);
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m2, vlp4 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m3, vlp7 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m3, vlp6 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m4, vlp9 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m4, vlp8 , _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m5, vlp11, _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m5, vlp10, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m6, vlp13, _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m6, vlp12, _MM_SWIZ_REG_NONE);
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, m7, vlp15, _MM_SWIZ_REG_NONE);
    tlp15 = _mm512_mask_swizzle_epi64(vlp15, m7, vlp14, _MM_SWIZ_REG_NONE);

    thp0  = _mm512_mask_swizzle_epi64(vhp0 , m0>>8, vhp1 , _MM_SWIZ_REG_NONE);
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , m0>>8, vhp0 , _MM_SWIZ_REG_NONE);
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m1>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m1>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m2>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m2>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m3>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m3>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m4>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m4>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m5>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m5>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m6>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m6>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp14 = _mm512_mask_swizzle_epi64(vhp14, m7>>8, vhp15, _MM_SWIZ_REG_NONE);
    thp15 = _mm512_mask_swizzle_epi64(vhp15, m7>>8, vhp14, _MM_SWIZ_REG_NONE);

    // step 2 
    // v2  = _mm512_max_ps(t2 , t0 );
    // v0  = _mm512_min_ps(t2 , t0 );
    // v6  = _mm512_max_ps(t6 , t4 );
    // v4  = _mm512_min_ps(t6 , t4 );
    // v10 = _mm512_max_ps(t10, t8 );
    // v8  = _mm512_min_ps(t10, t8 );
    // v14 = _mm512_max_ps(t14, t12);
    // v12 = _mm512_min_ps(t14, t12);
    // v3  = _mm512_max_ps(t3 , t1 );
    // v1  = _mm512_min_ps(t3 , t1 );
    // v7  = _mm512_max_ps(t7 , t5 );
    // v5  = _mm512_min_ps(t7 , t5 );
    // v11 = _mm512_max_ps(t11, t9 );
    // v9  = _mm512_min_ps(t11, t9 );
    // v15 = _mm512_max_ps(t15, t13);
    // v13 = _mm512_min_ps(t15, t13);

    m0 = _mm512_cmp_ps_mask(t0 , t2 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t4 , t6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t8 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t12, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(t1 , t3 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(t5 , t7 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(t9 , t11, _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(t13, t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_ps(t0 , m0, t2 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_ps(t2 , m0, t0 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_ps(t4 , m1, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_ps(t6 , m1, t4 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m2, t8 , _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_ps(t12, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_ps(t14, m3, t12, _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_ps(t1 , m4, t3 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_ps(t3 , m4, t1 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m5, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m5, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m6, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_ps(t11, m6, t9 , _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_ps(t13, m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_ps(t15, m7, t13, _MM_SWIZ_REG_NONE);

    vlp0  = _mm512_mask_swizzle_epi64(tlp0 , m0, tlp2 , _MM_SWIZ_REG_NONE);
    vlp2  = _mm512_mask_swizzle_epi64(tlp2 , m0, tlp0 , _MM_SWIZ_REG_NONE);
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , m1, tlp6 , _MM_SWIZ_REG_NONE);
    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m1, tlp4 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m2, tlp10, _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m2, tlp8 , _MM_SWIZ_REG_NONE);
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, m3, tlp14, _MM_SWIZ_REG_NONE);
    vlp14 = _mm512_mask_swizzle_epi64(tlp14, m3, tlp12, _MM_SWIZ_REG_NONE);
    vlp1  = _mm512_mask_swizzle_epi64(tlp1 , m4, tlp3 , _MM_SWIZ_REG_NONE);
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , m4, tlp1 , _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m5, tlp7 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m5, tlp5 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m6, tlp11, _MM_SWIZ_REG_NONE);
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, m6, tlp9 , _MM_SWIZ_REG_NONE);
    vlp13 = _mm512_mask_swizzle_epi64(tlp13, m7, tlp15, _MM_SWIZ_REG_NONE);
    vlp15 = _mm512_mask_swizzle_epi64(tlp15, m7, tlp13, _MM_SWIZ_REG_NONE);

    vhp0  = _mm512_mask_swizzle_epi64(thp0 , m0>>8, thp2 , _MM_SWIZ_REG_NONE);
    vhp2  = _mm512_mask_swizzle_epi64(thp2 , m0>>8, thp0 , _MM_SWIZ_REG_NONE);
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , m1>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m1>>8, thp4 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m2>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m2>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp12 = _mm512_mask_swizzle_epi64(thp12, m3>>8, thp14, _MM_SWIZ_REG_NONE);
    vhp14 = _mm512_mask_swizzle_epi64(thp14, m3>>8, thp12, _MM_SWIZ_REG_NONE);
    vhp1  = _mm512_mask_swizzle_epi64(thp1 , m4>>8, thp3 , _MM_SWIZ_REG_NONE);
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , m4>>8, thp1 , _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m5>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m5>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m6>>8, thp11, _MM_SWIZ_REG_NONE);
    vhp11 = _mm512_mask_swizzle_epi64(thp11, m6>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp13 = _mm512_mask_swizzle_epi64(thp13, m7>>8, thp15, _MM_SWIZ_REG_NONE);
    vhp15 = _mm512_mask_swizzle_epi64(thp15, m7>>8, thp13, _MM_SWIZ_REG_NONE);
    // step 3 
    // t4  = _mm512_max_ps(v4 , v0 );
    // t0  = _mm512_min_ps(v4 , v0 );
    // t12 = _mm512_max_ps(v12, v8 );
    // t8  = _mm512_min_ps(v12, v8 );
    // t5  = _mm512_max_ps(v5 , v1 );
    // t1  = _mm512_min_ps(v5 , v1 );
    // t13 = _mm512_max_ps(v13, v9 );
    // t9  = _mm512_min_ps(v13, v9 );
    // t6  = _mm512_max_ps(v6 , v2 );
    // t2  = _mm512_min_ps(v6 , v2 );
    // t14 = _mm512_max_ps(v14, v10);
    // t10 = _mm512_min_ps(v14, v10);
    // t7  = _mm512_max_ps(v7 , v3 );
    // t3  = _mm512_min_ps(v7 , v3 );
    // t15 = _mm512_max_ps(v15, v11);
    // t11 = _mm512_min_ps(v15, v11);

    m0 = _mm512_cmp_ps_mask(v0 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v8 , v12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v1 , v5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v9 , v13, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v2 , v6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(v10, v14, _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(v3 , v7 , _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(v11, v15, _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_ps(v0 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m0, v0 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m1, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m1, v8 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_ps(v1 , m2, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_ps(v5 , m2, v1 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m3, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m3, v9 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_ps(v2 , m4, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m4, v2 , _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m5, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_ps(v14, m5, v10, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m6, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m6, v3 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m7, v15, _MM_SWIZ_REG_NONE);
    t15 = _mm512_mask_swizzle_ps(v15, m7, v11, _MM_SWIZ_REG_NONE);

    tlp0  = _mm512_mask_swizzle_epi64(vlp0 , m0, vlp4 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m0, vlp0 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m1, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m1, vlp8 , _MM_SWIZ_REG_NONE);
    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , m2, vlp5 , _MM_SWIZ_REG_NONE);
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m2, vlp1 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m3, vlp13, _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m3, vlp9 , _MM_SWIZ_REG_NONE);
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m4, vlp6 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m4, vlp2 , _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m5, vlp14, _MM_SWIZ_REG_NONE);
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, m5, vlp10, _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m6, vlp7 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m6, vlp3 , _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m7, vlp15, _MM_SWIZ_REG_NONE);
    tlp15 = _mm512_mask_swizzle_epi64(vlp15, m7, vlp11, _MM_SWIZ_REG_NONE);

    thp0  = _mm512_mask_swizzle_epi64(vhp0 , m0>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m0>>8, vhp0 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m1>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m1>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , m2>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m2>>8, vhp1 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m3>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m3>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m4>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m4>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m5>>8, vhp14, _MM_SWIZ_REG_NONE);
    thp14 = _mm512_mask_swizzle_epi64(vhp14, m5>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m6>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m6>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m7>>8, vhp15, _MM_SWIZ_REG_NONE);
    thp15 = _mm512_mask_swizzle_epi64(vhp15, m7>>8, vhp11, _MM_SWIZ_REG_NONE);
    // step 4 
    // v8  = _mm512_max_ps(t8 , t0 );
    // v0  = _mm512_min_ps(t8 , t0 );
    // v9  = _mm512_max_ps(t9 , t1 );
    // v1  = _mm512_min_ps(t9 , t1 );
    // v10 = _mm512_max_ps(t10, t2 );
    // v2  = _mm512_min_ps(t10, t2 );
    // v11 = _mm512_max_ps(t11, t3 );
    // v3  = _mm512_min_ps(t11, t3 );
    // v12 = _mm512_max_ps(t12, t4 );
    // v4  = _mm512_min_ps(t12, t4 );
    // v13 = _mm512_max_ps(t13, t5 );
    // v5  = _mm512_min_ps(t13, t5 );
    // v14 = _mm512_max_ps(t14, t6 );
    // v6  = _mm512_min_ps(t14, t6 );
    // v15 = _mm512_max_ps(t15, t7 );
    // v7  = _mm512_min_ps(t15, t7 );

    m0 = _mm512_cmp_ps_mask(t0 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t1 , t9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t2 , t10, _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t3 , t11, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(t4 , t12, _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(t5 , t13, _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(t6 , t14, _MM_CMPINT_GT);
    m7 = _mm512_cmp_ps_mask(t7 , t15, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_ps(t0 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m0, t0 , _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_ps(t1 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m1, t1 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_ps(t2 , m2, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m2, t2 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_ps(t3 , m3, t11, _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_ps(t11, m3, t3 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_ps(t4 , m4, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_ps(t12, m4, t4 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m5, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_ps(t13, m5, t5 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_ps(t6 , m6, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_ps(t14, m6, t6 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m7, t15, _MM_SWIZ_REG_NONE);
    v15 = _mm512_mask_swizzle_ps(t15, m7, t7 , _MM_SWIZ_REG_NONE);

    vlp0  = _mm512_mask_swizzle_epi64(tlp0 , m0, tlp8 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m0, tlp0 , _MM_SWIZ_REG_NONE);
    vlp1  = _mm512_mask_swizzle_epi64(tlp1 , m1, tlp9 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m1, tlp1 , _MM_SWIZ_REG_NONE);
    vlp2  = _mm512_mask_swizzle_epi64(tlp2 , m2, tlp10, _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m2, tlp2 , _MM_SWIZ_REG_NONE);
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , m3, tlp11, _MM_SWIZ_REG_NONE);
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, m3, tlp3 , _MM_SWIZ_REG_NONE);
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , m4, tlp12, _MM_SWIZ_REG_NONE);
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, m4, tlp4 , _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m5, tlp13, _MM_SWIZ_REG_NONE);
    vlp13 = _mm512_mask_swizzle_epi64(tlp13, m5, tlp5 , _MM_SWIZ_REG_NONE);
    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m6, tlp14, _MM_SWIZ_REG_NONE);
    vlp14 = _mm512_mask_swizzle_epi64(tlp14, m6, tlp6 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m7, tlp15, _MM_SWIZ_REG_NONE);
    vlp15 = _mm512_mask_swizzle_epi64(tlp15, m7, tlp7 , _MM_SWIZ_REG_NONE);

    vhp0  = _mm512_mask_swizzle_epi64(thp0 , m0>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m0>>8, thp0 , _MM_SWIZ_REG_NONE);
    vhp1  = _mm512_mask_swizzle_epi64(thp1 , m1>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m1>>8, thp1 , _MM_SWIZ_REG_NONE);
    vhp2  = _mm512_mask_swizzle_epi64(thp2 , m2>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m2>>8, thp2 , _MM_SWIZ_REG_NONE);
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , m3>>8, thp11, _MM_SWIZ_REG_NONE);
    vhp11 = _mm512_mask_swizzle_epi64(thp11, m3>>8, thp3 , _MM_SWIZ_REG_NONE);
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , m4>>8, thp12, _MM_SWIZ_REG_NONE);
    vhp12 = _mm512_mask_swizzle_epi64(thp12, m4>>8, thp4 , _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m5>>8, thp13, _MM_SWIZ_REG_NONE);
    vhp13 = _mm512_mask_swizzle_epi64(thp13, m5>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m6>>8, thp14, _MM_SWIZ_REG_NONE);
    vhp14 = _mm512_mask_swizzle_epi64(thp14, m6>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m7>>8, thp15, _MM_SWIZ_REG_NONE);
    vhp15 = _mm512_mask_swizzle_epi64(thp15, m7>>8, thp7 , _MM_SWIZ_REG_NONE);

    // step 5 
    // t10 = _mm512_max_ps(v10, v5 );
    // t5  = _mm512_min_ps(v10, v5 );
    // t9  = _mm512_max_ps(v9 , v6 );
    // t6  = _mm512_min_ps(v9 , v6 );
    // t12 = _mm512_max_ps(v12, v3 );
    // t3  = _mm512_min_ps(v12, v3 );
    // t14 = _mm512_max_ps(v14, v13);
    // t13 = _mm512_min_ps(v14, v13);
    // t11 = _mm512_max_ps(v11, v7 );
    // t7  = _mm512_min_ps(v11, v7 );
    // t2  = _mm512_max_ps(v2 , v1 );
    // t1  = _mm512_min_ps(v2 , v1 );
    // t8  = _mm512_max_ps(v8 , v4 );
    // t4  = _mm512_min_ps(v8 , v4 );
    // t0  = v0 ;
    // t15 = v15;

    m0 = _mm512_cmp_ps_mask(v5 , v10, _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v6 , v9 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v3 , v12, _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v13, v14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v7 , v11, _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(v1 , v2 , _MM_CMPINT_GT);
    m6 = _mm512_cmp_ps_mask(v4 , v8 , _MM_CMPINT_GT);

    t5  = _mm512_mask_swizzle_ps(v5 , m0, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m0, v5 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m1, v9 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m1, v6 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m2, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m2, v3 , _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m3, v14, _MM_SWIZ_REG_NONE);
    t14 = _mm512_mask_swizzle_ps(v14, m3, v13, _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m4, v11, _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m4, v7 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_ps(v1 , m5, v2 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_ps(v2 , m5, v1 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m6, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m6, v4 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t15 = v15;

    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m0, vlp10, _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m0, vlp5 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m1, vlp9 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m1, vlp6 , _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m2, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m2, vlp3 , _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m3, vlp14, _MM_SWIZ_REG_NONE);
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, m3, vlp13, _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m4, vlp11, _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m4, vlp7 , _MM_SWIZ_REG_NONE);
    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , m5, vlp2 , _MM_SWIZ_REG_NONE);
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m5, vlp1 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m6, vlp8 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m6, vlp4 , _MM_SWIZ_REG_NONE);
    tlp0  = vlp0 ;
    tlp15 = vlp15;

    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m0>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m0>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m1>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m1>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m2>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m2>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m3>>8, vhp14, _MM_SWIZ_REG_NONE);
    thp14 = _mm512_mask_swizzle_epi64(vhp14, m3>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m4>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m4>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , m5>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m5>>8, vhp1 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m6>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m6>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp0  = vhp0 ;
    thp15 = vhp15;

    // step 6 
    // v4  = _mm512_max_ps(t4 , t1 );
    // v1  = _mm512_min_ps(t4 , t1 );
    // v13 = _mm512_max_ps(t13, t7 );
    // v7  = _mm512_min_ps(t13, t7 );
    // v8  = _mm512_max_ps(t8 , t2 );
    // v2  = _mm512_min_ps(t8 , t2 );
    // v14 = _mm512_max_ps(t14, t11);
    // v11 = _mm512_min_ps(t14, t11);
    // v6  = _mm512_max_ps(t6 , t5 );
    // v5  = _mm512_min_ps(t6 , t5 );
    // v10 = _mm512_max_ps(t10, t9 );
    // v9  = _mm512_min_ps(t10, t9 );
    // v0  = t0 ;
    // v3  = t3 ;
    // v12 = t12;
    // v15 = t15;

    m0 = _mm512_cmp_ps_mask(t1 , t4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t7 , t13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t2 , t8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t11, t14, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(t5 , t6 , _MM_CMPINT_GT);
    m5 = _mm512_cmp_ps_mask(t9 , t10, _MM_CMPINT_GT);

    v1  = _mm512_mask_swizzle_ps(t1 , m0, t4 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_ps(t4 , m0, t1 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m1, t13, _MM_SWIZ_REG_NONE);
    v13 = _mm512_mask_swizzle_ps(t13, m1, t7 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_ps(t2 , m2, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m2, t2 , _MM_SWIZ_REG_NONE);
    v11 = _mm512_mask_swizzle_ps(t11, m3, t14, _MM_SWIZ_REG_NONE);
    v14 = _mm512_mask_swizzle_ps(t14, m3, t11, _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m4, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_ps(t6 , m4, t5 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m5, t10, _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m5, t9 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v3  = t3 ;
    v12 = t12;
    v15 = t15;

    vlp1  = _mm512_mask_swizzle_epi64(tlp1 , m0, tlp4 , _MM_SWIZ_REG_NONE);
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , m0, tlp1 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m1, tlp13, _MM_SWIZ_REG_NONE);
    vlp13 = _mm512_mask_swizzle_epi64(tlp13, m1, tlp7 , _MM_SWIZ_REG_NONE);
    vlp2  = _mm512_mask_swizzle_epi64(tlp2 , m2, tlp8 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m2, tlp2 , _MM_SWIZ_REG_NONE);
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, m3, tlp14, _MM_SWIZ_REG_NONE);
    vlp14 = _mm512_mask_swizzle_epi64(tlp14, m3, tlp11, _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m4, tlp6 , _MM_SWIZ_REG_NONE);
    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m4, tlp5 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m5, tlp10, _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m5, tlp9 , _MM_SWIZ_REG_NONE);
    vlp0  = tlp0 ;
    vlp3  = tlp3 ;
    vlp12 = tlp12;
    vlp15 = tlp15;

    vhp1  = _mm512_mask_swizzle_epi64(thp1 , m0>>8, thp4 , _MM_SWIZ_REG_NONE);
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , m0>>8, thp1 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m1>>8, thp13, _MM_SWIZ_REG_NONE);
    vhp13 = _mm512_mask_swizzle_epi64(thp13, m1>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp2  = _mm512_mask_swizzle_epi64(thp2 , m2>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m2>>8, thp2 , _MM_SWIZ_REG_NONE);
    vhp11 = _mm512_mask_swizzle_epi64(thp11, m3>>8, thp14, _MM_SWIZ_REG_NONE);
    vhp14 = _mm512_mask_swizzle_epi64(thp14, m3>>8, thp11, _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m4>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m4>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m5>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m5>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp0  = thp0 ;
    vhp3  = thp3 ;
    vhp12 = thp12;
    vhp15 = thp15;

    // step 7 
    // t4  = _mm512_max_ps(v4 , v2 );
    // t2  = _mm512_min_ps(v4 , v2 );
    // t13 = _mm512_max_ps(v13, v11);
    // t11 = _mm512_min_ps(v13, v11);
    // t8  = _mm512_max_ps(v8 , v3 );
    // t3  = _mm512_min_ps(v8 , v3 );
    // t12 = _mm512_max_ps(v12, v7 );
    // t7  = _mm512_min_ps(v12, v7 );
    // t0  = v0 ;
    // t1  = v1 ;
    // t5  = v5 ;
    // t6  = v6 ;
    // t9  = v9 ;
    // t10 = v10;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_ps_mask(v2 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v11, v13, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v3 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v7 , v12, _MM_CMPINT_GT);

    t2  = _mm512_mask_swizzle_ps(v2 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m0, v2 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m1, v13, _MM_SWIZ_REG_NONE);
    t13 = _mm512_mask_swizzle_ps(v13, m1, v11, _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_ps(v3 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m2, v3 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m3, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m3, v7 , _MM_SWIZ_REG_NONE);
    t0  = v0 ;
    t1  = v1 ;
    t5  = v5 ;
    t6  = v6 ;
    t9  = v9 ;
    t10 = v10;
    t14 = v14;
    t15 = v15;

    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , m0, vlp4 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m0, vlp2 , _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m1, vlp13, _MM_SWIZ_REG_NONE);
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, m1, vlp11, _MM_SWIZ_REG_NONE);
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m2, vlp8 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m2, vlp3 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m3, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m3, vlp7 , _MM_SWIZ_REG_NONE);
    tlp0  = vlp0 ;
    tlp1  = vlp1 ;
    tlp5  = vlp5 ;
    tlp6  = vlp6 ;
    tlp9  = vlp9 ;
    tlp10 = vlp10;
    tlp14 = vlp14;
    tlp15 = vlp15;

    thp2  = _mm512_mask_swizzle_epi64(vhp2 , m0>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m0>>8, vhp2 , _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m1>>8, vhp13, _MM_SWIZ_REG_NONE);
    thp13 = _mm512_mask_swizzle_epi64(vhp13, m1>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m2>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m2>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m3>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m3>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp0  = vhp0 ;
    thp1  = vhp1 ;
    thp5  = vhp5 ;
    thp6  = vhp6 ;
    thp9  = vhp9 ;
    thp10 = vhp10;
    thp14 = vhp14;
    thp15 = vhp15;

    // step 8 
    // v8  = _mm512_max_ps(t8 , t6 );
    // v6  = _mm512_min_ps(t8 , t6 );
    // v12 = _mm512_max_ps(t12, t10);
    // v10 = _mm512_min_ps(t12, t10);
    // v5  = _mm512_max_ps(t5 , t3 );
    // v3  = _mm512_min_ps(t5 , t3 );
    // v9  = _mm512_max_ps(t9 , t7 );
    // v7  = _mm512_min_ps(t9 , t7 );
    // v0  = t0;
    // v1  = t1;
    // v2  = t2;
    // v4  = t4;
    // v11 = t11;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_ps_mask(t6 , t8 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t10, t12, _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(t3 , t5 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(t7 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_ps(t6 , m0, t8 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m0, t6 , _MM_SWIZ_REG_NONE);
    v10 = _mm512_mask_swizzle_ps(t10, m1, t12, _MM_SWIZ_REG_NONE);
    v12 = _mm512_mask_swizzle_ps(t12, m1, t10, _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_ps(t3 , m2, t5 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_ps(t5 , m2, t3 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m3, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m3, t7 , _MM_SWIZ_REG_NONE);
    v0  = t0;
    v1  = t1;
    v2  = t2;
    v4  = t4;
    v11 = t11;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m0, tlp8 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m0, tlp6 , _MM_SWIZ_REG_NONE);
    vlp10 = _mm512_mask_swizzle_epi64(tlp10, m1, tlp12, _MM_SWIZ_REG_NONE);
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, m1, tlp10, _MM_SWIZ_REG_NONE);
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , m2, tlp5 , _MM_SWIZ_REG_NONE);
    vlp5  = _mm512_mask_swizzle_epi64(tlp5 , m2, tlp3 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m3, tlp9 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m3, tlp7 , _MM_SWIZ_REG_NONE);
    vlp0  = tlp0;
    vlp1  = tlp1;
    vlp2  = tlp2;
    vlp4  = tlp4;
    vlp11 = tlp11;
    vlp13 = tlp13;
    vlp14 = tlp14;
    vlp15 = tlp15;

    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m0>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m0>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp10 = _mm512_mask_swizzle_epi64(thp10, m1>>8, thp12, _MM_SWIZ_REG_NONE);
    vhp12 = _mm512_mask_swizzle_epi64(thp12, m1>>8, thp10, _MM_SWIZ_REG_NONE);
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , m2>>8, thp5 , _MM_SWIZ_REG_NONE);
    vhp5  = _mm512_mask_swizzle_epi64(thp5 , m2>>8, thp3 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m3>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m3>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp0  = thp0;
    vhp1  = thp1;
    vhp2  = thp2;
    vhp4  = thp4;
    vhp11 = thp11;
    vhp13 = thp13;
    vhp14 = thp14;
    vhp15 = thp15;

    // step 9 
    // t4  = _mm512_max_ps(v4 , v3 );
    // t3  = _mm512_min_ps(v4 , v3 );
    // t6  = _mm512_max_ps(v6 , v5 );
    // t5  = _mm512_min_ps(v6 , v5 );
    // t8  = _mm512_max_ps(v8 , v7 );
    // t7  = _mm512_min_ps(v8 , v7 );
    // t10 = _mm512_max_ps(v10, v9 );
    // t9  = _mm512_min_ps(v10, v9 );
    // t12 = _mm512_max_ps(v12, v11);
    // t11 = _mm512_min_ps(v12, v11);
    // t0  = v0;
    // t1  = v1;
    // t2  = v2;
    // t13 = v13;
    // t14 = v14;
    // t15 = v15;

    m0 = _mm512_cmp_ps_mask(v3 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(v5 , v6 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_ps_mask(v7 , v8 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_ps_mask(v9 , v10, _MM_CMPINT_GT);
    m4 = _mm512_cmp_ps_mask(v11, v12, _MM_CMPINT_GT);

    t3  = _mm512_mask_swizzle_ps(v3 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_ps(v4 , m0, v3 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_ps(v5 , m1, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_ps(v6 , m1, v5 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_ps(v7 , m2, v8 , _MM_SWIZ_REG_NONE);
    t8  = _mm512_mask_swizzle_ps(v8 , m2, v7 , _MM_SWIZ_REG_NONE);
    t9  = _mm512_mask_swizzle_ps(v9 , m3, v10, _MM_SWIZ_REG_NONE);
    t10 = _mm512_mask_swizzle_ps(v10, m3, v9 , _MM_SWIZ_REG_NONE);
    t11 = _mm512_mask_swizzle_ps(v11, m4, v12, _MM_SWIZ_REG_NONE);
    t12 = _mm512_mask_swizzle_ps(v12, m4, v11, _MM_SWIZ_REG_NONE);
    t0  = v0;
    t1  = v1;
    t2  = v2;
    t13 = v13;
    t14 = v14;
    t15 = v15;

    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , m0, vlp4 , _MM_SWIZ_REG_NONE);
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , m0, vlp3 , _MM_SWIZ_REG_NONE);
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , m1, vlp6 , _MM_SWIZ_REG_NONE);
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , m1, vlp5 , _MM_SWIZ_REG_NONE);
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , m2, vlp8 , _MM_SWIZ_REG_NONE);
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , m2, vlp7 , _MM_SWIZ_REG_NONE);
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , m3, vlp10, _MM_SWIZ_REG_NONE);
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, m3, vlp9 , _MM_SWIZ_REG_NONE);
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, m4, vlp12, _MM_SWIZ_REG_NONE);
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, m4, vlp11, _MM_SWIZ_REG_NONE);
    tlp0  = vlp0;
    tlp1  = vlp1;
    tlp2  = vlp2;
    tlp13 = vlp13;
    tlp14 = vlp14;
    tlp15 = vlp15;

    thp3  = _mm512_mask_swizzle_epi64(vhp3 , m0>>8, vhp4 , _MM_SWIZ_REG_NONE);
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , m0>>8, vhp3 , _MM_SWIZ_REG_NONE);
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , m1>>8, vhp6 , _MM_SWIZ_REG_NONE);
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , m1>>8, vhp5 , _MM_SWIZ_REG_NONE);
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , m2>>8, vhp8 , _MM_SWIZ_REG_NONE);
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , m2>>8, vhp7 , _MM_SWIZ_REG_NONE);
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , m3>>8, vhp10, _MM_SWIZ_REG_NONE);
    thp10 = _mm512_mask_swizzle_epi64(vhp10, m3>>8, vhp9 , _MM_SWIZ_REG_NONE);
    thp11 = _mm512_mask_swizzle_epi64(vhp11, m4>>8, vhp12, _MM_SWIZ_REG_NONE);
    thp12 = _mm512_mask_swizzle_epi64(vhp12, m4>>8, vhp11, _MM_SWIZ_REG_NONE);
    thp0  = vhp0;
    thp1  = vhp1;
    thp2  = vhp2;
    thp13 = vhp13;
    thp14 = vhp14;
    thp15 = vhp15;
    // step 10 
    // v7  = _mm512_max_ps(t7 , t6 );
    // v6  = _mm512_min_ps(t7 , t6 );
    // v9  = _mm512_max_ps(t9 , t8 );
    // v8  = _mm512_min_ps(t9 , t8 );
    // v0  = t0 ;
    // v1  = t1 ;
    // v2  = t2 ;
    // v3  = t3 ;
    // v4  = t4 ;
    // v5  = t5 ;
    // v10 = t10;
    // v11 = t11;
    // v12 = t12;
    // v13 = t13;
    // v14 = t14;
    // v15 = t15;

    m0 = _mm512_cmp_ps_mask(t6 , t7 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_ps_mask(t8 , t9 , _MM_CMPINT_GT);

    v6  = _mm512_mask_swizzle_ps(t6 , m0, t7 , _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_ps(t7 , m0, t6 , _MM_SWIZ_REG_NONE);
    v8  = _mm512_mask_swizzle_ps(t8 , m1, t9 , _MM_SWIZ_REG_NONE);
    v9  = _mm512_mask_swizzle_ps(t9 , m1, t8 , _MM_SWIZ_REG_NONE);
    v0  = t0 ;
    v1  = t1 ;
    v2  = t2 ;
    v3  = t3 ;
    v4  = t4 ;
    v5  = t5 ;
    v10 = t10;
    v11 = t11;
    v12 = t12;
    v13 = t13;
    v14 = t14;
    v15 = t15;

    vlp6  = _mm512_mask_swizzle_epi64(tlp6 , m0, tlp7 , _MM_SWIZ_REG_NONE);
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , m0, tlp6 , _MM_SWIZ_REG_NONE);
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , m1, tlp9 , _MM_SWIZ_REG_NONE);
    vlp9  = _mm512_mask_swizzle_epi64(tlp9 , m1, tlp8 , _MM_SWIZ_REG_NONE);
    vlp0  = tlp0 ;
    vlp1  = tlp1 ;
    vlp2  = tlp2 ;
    vlp3  = tlp3 ;
    vlp4  = tlp4 ;
    vlp5  = tlp5 ;
    vlp10 = tlp10;
    vlp11 = tlp11;
    vlp12 = tlp12;
    vlp13 = tlp13;
    vlp14 = tlp14;
    vlp15 = tlp15;

    vhp6  = _mm512_mask_swizzle_epi64(thp6 , m0>>8, thp7 , _MM_SWIZ_REG_NONE);
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , m0>>8, thp6 , _MM_SWIZ_REG_NONE);
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , m1>>8, thp9 , _MM_SWIZ_REG_NONE);
    vhp9  = _mm512_mask_swizzle_epi64(thp9 , m1>>8, thp8 , _MM_SWIZ_REG_NONE);
    vhp0  = thp0 ;
    vhp1  = thp1 ;
    vhp2  = thp2 ;
    vhp3  = thp3 ;
    vhp4  = thp4 ;
    vhp5  = thp5 ;
    vhp10 = thp10;
    vhp11 = thp11;
    vhp12 = thp12;
    vhp13 = thp13;
    vhp14 = thp14;
    vhp15 = thp15;

}

template <typename T> // double key int index
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_sort_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3 )
{
    __m512d t0 ;
    __m512d t1 ;
    __m512d t2 ;
    __m512d t3 ;
    __m512d t4 ;
    __m512d t5 ;
    __m512d t6 ;
    __m512d t7 ;

    __m512i tp0 ;
    __m512i tp1 ;
    __m512i tp2 ;
    __m512i tp3 ;
    __m512i tp4 ;
    __m512i tp5 ;
    __m512i tp6 ;
    __m512i tp7 ;

    __m512i ap0 ;
    __m512i ap1 ;
    __m512i ap2 ;
    __m512i ap3 ;
    __m512i ap4 ;
    __m512i ap5 ;
    __m512i ap6 ;
    __m512i ap7 ;

    __mmask8 m0 ;
    __mmask8 m1 ;
    __mmask8 m2 ;
    __mmask8 m3 ;
    // __mmask8 m4 ;
    // __mmask8 m5 ;
    // __mmask8 m6 ;
    // __mmask8 m7 ;

    tp0 = vp0;
    tp1 = _mm512_permute4f128_epi32(vp0, _MM_PERM_BADC);
    tp2 = vp1;
    tp3 = _mm512_permute4f128_epi32(vp1, _MM_PERM_BADC);
    tp4 = vp2;
    tp5 = _mm512_permute4f128_epi32(vp2, _MM_PERM_BADC);
    tp6 = vp3;
    tp7 = _mm512_permute4f128_epi32(vp3, _MM_PERM_BADC);

    // step 1 
    // t4 = _mm512_gmax_pd(v4, v0);
    // t0 = _mm512_gmin_pd(v4, v0);
    // t5 = _mm512_gmax_pd(v5, v1);
    // t1 = _mm512_gmin_pd(v5, v1);
    // t6 = _mm512_gmax_pd(v6, v2);
    // t2 = _mm512_gmin_pd(v6, v2);
    // t7 = _mm512_gmax_pd(v7, v3);
    // t3 = _mm512_gmin_pd(v7, v3);

    m0 = _mm512_cmp_pd_mask(v0 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(v1 , v5 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(v2 , v6 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_pd_mask(v3 , v7 , _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_pd(v0 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_pd(v4 , m0, v0 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m1, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_pd(v5 , m1, v1 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_pd(v2 , m2, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_pd(v6 , m2, v2 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_pd(v3 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_pd(v7 , m3, v3 , _MM_SWIZ_REG_NONE);

    ap0  = _mm512_mask_blend_epi32(m0, tp0 ,tp4);
    ap4  = _mm512_mask_blend_epi32(m0, tp4 ,tp0);
    ap1  = _mm512_mask_blend_epi32(m1, tp1 ,tp5);
    ap5  = _mm512_mask_blend_epi32(m1, tp5 ,tp1);
    ap2  = _mm512_mask_blend_epi32(m2, tp2 ,tp6);
    ap6  = _mm512_mask_blend_epi32(m2, tp6 ,tp2);
    ap3  = _mm512_mask_blend_epi32(m3, tp3 ,tp7);
    ap7  = _mm512_mask_blend_epi32(m3, tp7 ,tp3);

    // vp0 = _mm512_mask_permute4f128_epi32(ap0, 0xff00, ap1, _MM_PERM_BADC);
    // vp1 = _mm512_mask_permute4f128_epi32(ap2, 0xff00, ap3, _MM_PERM_BADC);
    // vp2 = _mm512_mask_permute4f128_epi32(ap4, 0xff00, ap5, _MM_PERM_BADC);
    // vp3 = _mm512_mask_permute4f128_epi32(ap6, 0xff00, ap7, _MM_PERM_BADC);
    
    // step 2 
    // v2 = _mm512_gmax_pd(t2, t0);
    // v0 = _mm512_gmin_pd(t2, t0);
    // v3 = _mm512_gmax_pd(t3, t1);
    // v1 = _mm512_gmin_pd(t3, t1);
    // v6 = _mm512_gmax_pd(t6, t4);
    // v4 = _mm512_gmin_pd(t6, t4);
    // v7 = _mm512_gmax_pd(t7, t5);
    // v5 = _mm512_gmin_pd(t7, t5);

    m0 = _mm512_cmp_pd_mask(t0, t2, _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(t1, t3, _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(t4, t6, _MM_CMPINT_GT);
    m3 = _mm512_cmp_pd_mask(t5, t7, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_pd(t0, m0, t2, _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_pd(t2, m0, t0, _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_pd(t1, m1, t3, _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_pd(t3, m1, t1, _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_pd(t4, m2, t6, _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_pd(t6, m2, t4, _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_pd(t5, m3, t7, _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_pd(t7, m3, t5, _MM_SWIZ_REG_NONE);

    tp0  = _mm512_mask_blend_epi32(m0, ap0, ap2);
    tp2  = _mm512_mask_blend_epi32(m0, ap2, ap0);
    tp1  = _mm512_mask_blend_epi32(m1, ap1, ap3);
    tp3  = _mm512_mask_blend_epi32(m1, ap3, ap1);
    tp4  = _mm512_mask_blend_epi32(m2, ap4, ap6);
    tp6  = _mm512_mask_blend_epi32(m2, ap6, ap4);
    tp5  = _mm512_mask_blend_epi32(m3, ap5, ap7);
    tp7  = _mm512_mask_blend_epi32(m3, ap7, ap5);

    // step 3 
    // t4 = _mm512_gmax_pd(v4, v2);
    // t2 = _mm512_gmin_pd(v4, v2);
    // t5 = _mm512_gmax_pd(v5, v3);
    // t3 = _mm512_gmin_pd(v5, v3);
    // t1 = _mm512_gmax_pd(v1, v0);
    // t0 = _mm512_gmin_pd(v1, v0);
    // t7 = _mm512_gmax_pd(v7, v6);
    // t6 = _mm512_gmin_pd(v7, v6);

    m0 = _mm512_cmp_pd_mask(v2 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(v3 , v5 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(v0 , v1 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_pd_mask(v6 , v7 , _MM_CMPINT_GT);

    t2  = _mm512_mask_swizzle_pd(v2 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_pd(v4 , m0, v2 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_pd(v3 , m1, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_pd(v5 , m1, v3 , _MM_SWIZ_REG_NONE);
    t0  = _mm512_mask_swizzle_pd(v0 , m2, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m2, v0 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_pd(v6 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_pd(v7 , m3, v6 , _MM_SWIZ_REG_NONE);

    ap2  = _mm512_mask_blend_epi32(m0, tp2, tp4);
    ap4  = _mm512_mask_blend_epi32(m0, tp4, tp2);
    ap3  = _mm512_mask_blend_epi32(m1, tp3, tp5);
    ap5  = _mm512_mask_blend_epi32(m1, tp5, tp3);
    ap0  = _mm512_mask_blend_epi32(m2, tp0, tp1);
    ap1  = _mm512_mask_blend_epi32(m2, tp1, tp0);
    ap6  = _mm512_mask_blend_epi32(m3, tp6, tp7);
    ap7  = _mm512_mask_blend_epi32(m3, tp7, tp6);
    // step 4 
    // v3 = _mm512_gmax_pd(t3, t2);
    // v2 = _mm512_gmin_pd(t3, t2);
    // v5 = _mm512_gmax_pd(t5, t4);
    // v4 = _mm512_gmin_pd(t5, t4);
    // v0 = t0;
    // v1 = t1;
    // v6 = t6;
    // v7 = t7;

    m0 = _mm512_cmp_pd_mask(t2 , t3 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(t4 , t5 , _MM_CMPINT_GT);

    v2  = _mm512_mask_swizzle_pd(t2 , m0, t3 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_pd(t3 , m0, t2 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_pd(t4 , m1, t5 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_pd(t5 , m1, t4 , _MM_SWIZ_REG_NONE);
    v0 = t0;
    v1 = t1;
    v6 = t6;
    v7 = t7;

    tp2  = _mm512_mask_blend_epi32(m0, ap2, ap3);
    tp3  = _mm512_mask_blend_epi32(m0, ap3, ap2);
    tp4  = _mm512_mask_blend_epi32(m1, ap4, ap5);
    tp5  = _mm512_mask_blend_epi32(m1, ap5, ap4);
    tp0  = ap0;
    tp1  = ap1;
    tp6  = ap6;
    tp7  = ap7;
    // step 5 
    // t4 = _mm512_gmax_pd(v4, v1);
    // t1 = _mm512_gmin_pd(v4, v1);
    // t6 = _mm512_gmax_pd(v6, v3);
    // t3 = _mm512_gmin_pd(v6, v3);
    // t0 = v0;
    // t2 = v2;
    // t5 = v5;
    // t7 = v7;

    m0 = _mm512_cmp_pd_mask(v1 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(v3 , v6 , _MM_CMPINT_GT);

    t1  = _mm512_mask_swizzle_pd(v1 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_pd(v4 , m0, v1 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_pd(v3 , m1, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_pd(v6 , m1, v3 , _MM_SWIZ_REG_NONE);
    t0 = v0;
    t2 = v2;
    t5 = v5;
    t7 = v7;

    ap1  = _mm512_mask_blend_epi32(m0, tp1, tp4);
    ap4  = _mm512_mask_blend_epi32(m0, tp4, tp1);
    ap3  = _mm512_mask_blend_epi32(m1, tp3, tp6);
    ap6  = _mm512_mask_blend_epi32(m1, tp6, tp3);
    ap0  = tp0;
    ap2  = tp2;
    ap5  = tp5;
    ap7  = tp7;
    // step 6 
    // v2 = _mm512_gmax_pd(t2, t1);
    // v1 = _mm512_gmin_pd(t2, t1);
    // v4 = _mm512_gmax_pd(t4, t3);
    // v3 = _mm512_gmin_pd(t4, t3);
    // v6 = _mm512_gmax_pd(t6, t5);
    // v5 = _mm512_gmin_pd(t6, t5);
    // v0 = t0;
    // v7 = t7;

    m0 = _mm512_cmp_pd_mask(t1 , t2 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(t3 , t4 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(t5 , t6 , _MM_CMPINT_GT);

    v1  = _mm512_mask_swizzle_pd(t1 , m0, t2 , _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_pd(t2 , m0, t1 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_pd(t3 , m1, t4 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_pd(t4 , m1, t3 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_pd(t5 , m2, t6 , _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_pd(t6 , m2, t5 , _MM_SWIZ_REG_NONE);
    v0 = t0;
    v7 = t7;

    tp1  = _mm512_mask_blend_epi32(m0, ap1, ap2);
    tp2  = _mm512_mask_blend_epi32(m0, ap2, ap1);
    tp3  = _mm512_mask_blend_epi32(m1, ap3, ap4);
    tp4  = _mm512_mask_blend_epi32(m1, ap4, ap3);
    tp5  = _mm512_mask_blend_epi32(m2, ap5, ap6);
    tp6  = _mm512_mask_blend_epi32(m2, ap6, ap5);
    tp0  = ap0;
    tp7  = ap7;

    vp0 = _mm512_mask_permute4f128_epi32(tp0, 0xff00, tp1, _MM_PERM_BADC);
    vp1 = _mm512_mask_permute4f128_epi32(tp2, 0xff00, tp3, _MM_PERM_BADC);
    vp2 = _mm512_mask_permute4f128_epi32(tp4, 0xff00, tp5, _MM_PERM_BADC);
    vp3 = _mm512_mask_permute4f128_epi32(tp6, 0xff00, tp7, _MM_PERM_BADC);

}

template <typename T> // double key long index
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_sort_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3,
        __m512i &vp4 , __m512i &vp5 , __m512i &vp6 , __m512i &vp7)
{
    __m512d t0 ;
    __m512d t1 ;
    __m512d t2 ;
    __m512d t3 ;
    __m512d t4 ;
    __m512d t5 ;
    __m512d t6 ;
    __m512d t7 ;

    __m512i tp0 ;
    __m512i tp1 ;
    __m512i tp2 ;
    __m512i tp3 ;
    __m512i tp4 ;
    __m512i tp5 ;
    __m512i tp6 ;
    __m512i tp7 ;

    __mmask8 m0 ;
    __mmask8 m1 ;
    __mmask8 m2 ;
    __mmask8 m3 ;

    // step 1 
    // t4 = _mm512_gmax_pd(v4, v0);
    // t0 = _mm512_gmin_pd(v4, v0);
    // t5 = _mm512_gmax_pd(v5, v1);
    // t1 = _mm512_gmin_pd(v5, v1);
    // t6 = _mm512_gmax_pd(v6, v2);
    // t2 = _mm512_gmin_pd(v6, v2);
    // t7 = _mm512_gmax_pd(v7, v3);
    // t3 = _mm512_gmin_pd(v7, v3);

    m0 = _mm512_cmp_pd_mask(v0 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(v1 , v5 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(v2 , v6 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_pd_mask(v3 , v7 , _MM_CMPINT_GT);

    t0  = _mm512_mask_swizzle_pd(v0 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_pd(v4 , m0, v0 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m1, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_pd(v5 , m1, v1 , _MM_SWIZ_REG_NONE);
    t2  = _mm512_mask_swizzle_pd(v2 , m2, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_pd(v6 , m2, v2 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_pd(v3 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_pd(v7 , m3, v3 , _MM_SWIZ_REG_NONE);

    tp0  = _mm512_mask_blend_epi64(m0, vp0 ,vp4);
    tp4  = _mm512_mask_blend_epi64(m0, vp4 ,vp0);
    tp1  = _mm512_mask_blend_epi64(m1, vp1 ,vp5);
    tp5  = _mm512_mask_blend_epi64(m1, vp5 ,vp1);
    tp2  = _mm512_mask_blend_epi64(m2, vp2 ,vp6);
    tp6  = _mm512_mask_blend_epi64(m2, vp6 ,vp2);
    tp3  = _mm512_mask_blend_epi64(m3, vp3 ,vp7);
    tp7  = _mm512_mask_blend_epi64(m3, vp7 ,vp3);

    // step 2 
    // v2 = _mm512_gmax_pd(t2, t0);
    // v0 = _mm512_gmin_pd(t2, t0);
    // v3 = _mm512_gmax_pd(t3, t1);
    // v1 = _mm512_gmin_pd(t3, t1);
    // v6 = _mm512_gmax_pd(t6, t4);
    // v4 = _mm512_gmin_pd(t6, t4);
    // v7 = _mm512_gmax_pd(t7, t5);
    // v5 = _mm512_gmin_pd(t7, t5);

    m0 = _mm512_cmp_pd_mask(t0, t2, _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(t1, t3, _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(t4, t6, _MM_CMPINT_GT);
    m3 = _mm512_cmp_pd_mask(t5, t7, _MM_CMPINT_GT);

    v0  = _mm512_mask_swizzle_pd(t0, m0, t2, _MM_SWIZ_REG_NONE);
    v2  = _mm512_mask_swizzle_pd(t2, m0, t0, _MM_SWIZ_REG_NONE);
    v1  = _mm512_mask_swizzle_pd(t1, m1, t3, _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_pd(t3, m1, t1, _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_pd(t4, m2, t6, _MM_SWIZ_REG_NONE);
    v6  = _mm512_mask_swizzle_pd(t6, m2, t4, _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_pd(t5, m3, t7, _MM_SWIZ_REG_NONE);
    v7  = _mm512_mask_swizzle_pd(t7, m3, t5, _MM_SWIZ_REG_NONE);

    vp0  = _mm512_mask_blend_epi64(m0, tp0, tp2);
    vp2  = _mm512_mask_blend_epi64(m0, tp2, tp0);
    vp1  = _mm512_mask_blend_epi64(m1, tp1, tp3);
    vp3  = _mm512_mask_blend_epi64(m1, tp3, tp1);
    vp4  = _mm512_mask_blend_epi64(m2, tp4, tp6);
    vp6  = _mm512_mask_blend_epi64(m2, tp6, tp4);
    vp5  = _mm512_mask_blend_epi64(m3, tp5, tp7);
    vp7  = _mm512_mask_blend_epi64(m3, tp7, tp5);

    // step 3 
    // t4 = _mm512_gmax_pd(v4, v2);
    // t2 = _mm512_gmin_pd(v4, v2);
    // t5 = _mm512_gmax_pd(v5, v3);
    // t3 = _mm512_gmin_pd(v5, v3);
    // t1 = _mm512_gmax_pd(v1, v0);
    // t0 = _mm512_gmin_pd(v1, v0);
    // t7 = _mm512_gmax_pd(v7, v6);
    // t6 = _mm512_gmin_pd(v7, v6);

    m0 = _mm512_cmp_pd_mask(v2 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(v3 , v5 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(v0 , v1 , _MM_CMPINT_GT);
    m3 = _mm512_cmp_pd_mask(v6 , v7 , _MM_CMPINT_GT);

    t2  = _mm512_mask_swizzle_pd(v2 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_pd(v4 , m0, v2 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_pd(v3 , m1, v5 , _MM_SWIZ_REG_NONE);
    t5  = _mm512_mask_swizzle_pd(v5 , m1, v3 , _MM_SWIZ_REG_NONE);
    t0  = _mm512_mask_swizzle_pd(v0 , m2, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m2, v0 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_pd(v6 , m3, v7 , _MM_SWIZ_REG_NONE);
    t7  = _mm512_mask_swizzle_pd(v7 , m3, v6 , _MM_SWIZ_REG_NONE);

    tp2  = _mm512_mask_blend_epi64(m0, vp2, vp4);
    tp4  = _mm512_mask_blend_epi64(m0, vp4, vp2);
    tp3  = _mm512_mask_blend_epi64(m1, vp3, vp5);
    tp5  = _mm512_mask_blend_epi64(m1, vp5, vp3);
    tp0  = _mm512_mask_blend_epi64(m2, vp0, vp1);
    tp1  = _mm512_mask_blend_epi64(m2, vp1, vp0);
    tp6  = _mm512_mask_blend_epi64(m3, vp6, vp7);
    tp7  = _mm512_mask_blend_epi64(m3, vp7, vp6);
    // step 4 
    // v3 = _mm512_gmax_pd(t3, t2);
    // v2 = _mm512_gmin_pd(t3, t2);
    // v5 = _mm512_gmax_pd(t5, t4);
    // v4 = _mm512_gmin_pd(t5, t4);
    // v0 = t0;
    // v1 = t1;
    // v6 = t6;
    // v7 = t7;

    m0 = _mm512_cmp_pd_mask(t2 , t3 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(t4 , t5 , _MM_CMPINT_GT);

    v2  = _mm512_mask_swizzle_pd(t2 , m0, t3 , _MM_SWIZ_REG_NONE);
    v3  = _mm512_mask_swizzle_pd(t3 , m0, t2 , _MM_SWIZ_REG_NONE);
    v4  = _mm512_mask_swizzle_pd(t4 , m1, t5 , _MM_SWIZ_REG_NONE);
    v5  = _mm512_mask_swizzle_pd(t5 , m1, t4 , _MM_SWIZ_REG_NONE);
    v0 = t0;
    v1 = t1;
    v6 = t6;
    v7 = t7;

    vp2  = _mm512_mask_blend_epi64(m0, tp2, tp3);
    vp3  = _mm512_mask_blend_epi64(m0, tp3, tp2);
    vp4  = _mm512_mask_blend_epi64(m1, tp4, tp5);
    vp5  = _mm512_mask_blend_epi64(m1, tp5, tp4);
    vp0  = tp0;
    vp1  = tp1;
    vp6  = tp6;
    vp7  = tp7;
    // step 5 
    // t4 = _mm512_gmax_pd(v4, v1);
    // t1 = _mm512_gmin_pd(v4, v1);
    // t6 = _mm512_gmax_pd(v6, v3);
    // t3 = _mm512_gmin_pd(v6, v3);
    // t0 = v0;
    // t2 = v2;
    // t5 = v5;
    // t7 = v7;

    m0 = _mm512_cmp_pd_mask(v1 , v4 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(v3 , v6 , _MM_CMPINT_GT);

    t1  = _mm512_mask_swizzle_pd(v1 , m0, v4 , _MM_SWIZ_REG_NONE);
    t4  = _mm512_mask_swizzle_pd(v4 , m0, v1 , _MM_SWIZ_REG_NONE);
    t3  = _mm512_mask_swizzle_pd(v3 , m1, v6 , _MM_SWIZ_REG_NONE);
    t6  = _mm512_mask_swizzle_pd(v6 , m1, v3 , _MM_SWIZ_REG_NONE);
    t0 = v0;
    t2 = v2;
    t5 = v5;
    t7 = v7;

    tp1  = _mm512_mask_blend_epi64(m0, vp1, vp4);
    tp4  = _mm512_mask_blend_epi64(m0, vp4, vp1);
    tp3  = _mm512_mask_blend_epi64(m1, vp3, vp6);
    tp6  = _mm512_mask_blend_epi64(m1, vp6, vp3);
    tp0  = vp0;
    tp2  = vp2;
    tp5  = vp5;
    tp7  = vp7;
    // step 6 
    // v2 = _mm512_gmax_pd(t2, t1);
    // v1 = _mm512_gmin_pd(t2, t1);
    // v4 = _mm512_gmax_pd(t4, t3);
    // v3 = _mm512_gmin_pd(t4, t3);
    // v6 = _mm512_gmax_pd(t6, t5);
    // v5 = _mm512_gmin_pd(t6, t5);
    // v0 = t0;
    // v7 = t7;

    m0 = _mm512_cmp_pd_mask(t1 , t2 , _MM_CMPINT_GT);
    m1 = _mm512_cmp_pd_mask(t3 , t4 , _MM_CMPINT_GT);
    m2 = _mm512_cmp_pd_mask(t5 , t6 , _MM_CMPINT_GT);

    v1 = _mm512_mask_swizzle_pd(t1 , m0, t2 , _MM_SWIZ_REG_NONE);
    v2 = _mm512_mask_swizzle_pd(t2 , m0, t1 , _MM_SWIZ_REG_NONE);
    v3 = _mm512_mask_swizzle_pd(t3 , m1, t4 , _MM_SWIZ_REG_NONE);
    v4 = _mm512_mask_swizzle_pd(t4 , m1, t3 , _MM_SWIZ_REG_NONE);
    v5 = _mm512_mask_swizzle_pd(t5 , m2, t6 , _MM_SWIZ_REG_NONE);
    v6 = _mm512_mask_swizzle_pd(t6 , m2, t5 , _MM_SWIZ_REG_NONE);
    v0 = t0;
    v7 = t7;

    vp1  = _mm512_mask_blend_epi64(m0, tp1, tp2);
    vp2  = _mm512_mask_blend_epi64(m0, tp2, tp1);
    vp3  = _mm512_mask_blend_epi64(m1, tp3, tp4);
    vp4  = _mm512_mask_blend_epi64(m1, tp4, tp3);
    vp5  = _mm512_mask_blend_epi64(m2, tp5, tp6);
    vp6  = _mm512_mask_blend_epi64(m2, tp6, tp5);
    vp0  = tp0;
    vp7  = tp7;

}
/**
 * Float vector version (__m512):
 * This method performs the in-register sort. The sorted elements 
 * are stored vertically accross the registers.
 *
 * @param v0-v15 vector data registers
 * @return sorted data stored vertically among the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_sort(T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
                 T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15)
{
    __m512 t0 ;
    __m512 t1 ;
    __m512 t2 ;
    __m512 t3 ;
    __m512 t4 ;
    __m512 t5 ;
    __m512 t6 ;
    __m512 t7 ;
    __m512 t8 ;
    __m512 t9 ;
    __m512 t10;
    __m512 t11;
    __m512 t12;
    __m512 t13;
    __m512 t14;
    __m512 t15;
    // step 1 
    t1  = _mm512_gmax_ps(v1 , v0 );
    t0  = _mm512_gmin_ps(v1 , v0 );
    t3  = _mm512_gmax_ps(v3 , v2 );
    t2  = _mm512_gmin_ps(v3 , v2 );
    t5  = _mm512_gmax_ps(v5 , v4 );
    t4  = _mm512_gmin_ps(v5 , v4 );
    t7  = _mm512_gmax_ps(v7 , v6 );
    t6  = _mm512_gmin_ps(v7 , v6 );
    t9  = _mm512_gmax_ps(v9 , v8 );
    t8  = _mm512_gmin_ps(v9 , v8 );
    t11 = _mm512_gmax_ps(v11, v10);
    t10 = _mm512_gmin_ps(v11, v10);
    t13 = _mm512_gmax_ps(v13, v12);
    t12 = _mm512_gmin_ps(v13, v12);
    t15 = _mm512_gmax_ps(v15, v14);
    t14 = _mm512_gmin_ps(v15, v14);
    // step 2 
    v2  = _mm512_gmax_ps(t2 , t0 );
    v0  = _mm512_gmin_ps(t2 , t0 );
    v6  = _mm512_gmax_ps(t6 , t4 );
    v4  = _mm512_gmin_ps(t6 , t4 );
    v10 = _mm512_gmax_ps(t10, t8 );
    v8  = _mm512_gmin_ps(t10, t8 );
    v14 = _mm512_gmax_ps(t14, t12);
    v12 = _mm512_gmin_ps(t14, t12);
    v3  = _mm512_gmax_ps(t3 , t1 );
    v1  = _mm512_gmin_ps(t3 , t1 );
    v7  = _mm512_gmax_ps(t7 , t5 );
    v5  = _mm512_gmin_ps(t7 , t5 );
    v11 = _mm512_gmax_ps(t11, t9 );
    v9  = _mm512_gmin_ps(t11, t9 );
    v15 = _mm512_gmax_ps(t15, t13);
    v13 = _mm512_gmin_ps(t15, t13);
    // step 3 
    t4  = _mm512_gmax_ps(v4 , v0 );
    t0  = _mm512_gmin_ps(v4 , v0 );
    t12 = _mm512_gmax_ps(v12, v8 );
    t8  = _mm512_gmin_ps(v12, v8 );
    t5  = _mm512_gmax_ps(v5 , v1 );
    t1  = _mm512_gmin_ps(v5 , v1 );
    t13 = _mm512_gmax_ps(v13, v9 );
    t9  = _mm512_gmin_ps(v13, v9 );
    t6  = _mm512_gmax_ps(v6 , v2 );
    t2  = _mm512_gmin_ps(v6 , v2 );
    t14 = _mm512_gmax_ps(v14, v10);
    t10 = _mm512_gmin_ps(v14, v10);
    t7  = _mm512_gmax_ps(v7 , v3 );
    t3  = _mm512_gmin_ps(v7 , v3 );
    t15 = _mm512_gmax_ps(v15, v11);
    t11 = _mm512_gmin_ps(v15, v11);
    // step 4 
    v8  = _mm512_gmax_ps(t8 , t0 );
    v0  = _mm512_gmin_ps(t8 , t0 );
    v9  = _mm512_gmax_ps(t9 , t1 );
    v1  = _mm512_gmin_ps(t9 , t1 );
    v10 = _mm512_gmax_ps(t10, t2 );
    v2  = _mm512_gmin_ps(t10, t2 );
    v11 = _mm512_gmax_ps(t11, t3 );
    v3  = _mm512_gmin_ps(t11, t3 );
    v12 = _mm512_gmax_ps(t12, t4 );
    v4  = _mm512_gmin_ps(t12, t4 );
    v13 = _mm512_gmax_ps(t13, t5 );
    v5  = _mm512_gmin_ps(t13, t5 );
    v14 = _mm512_gmax_ps(t14, t6 );
    v6  = _mm512_gmin_ps(t14, t6 );
    v15 = _mm512_gmax_ps(t15, t7 );
    v7  = _mm512_gmin_ps(t15, t7 );
    // step 5 
    t10 = _mm512_gmax_ps(v10, v5 );
    t5  = _mm512_gmin_ps(v10, v5 );
    t9  = _mm512_gmax_ps(v9 , v6 );
    t6  = _mm512_gmin_ps(v9 , v6 );
    t12 = _mm512_gmax_ps(v12, v3 );
    t3  = _mm512_gmin_ps(v12, v3 );
    t14 = _mm512_gmax_ps(v14, v13);
    t13 = _mm512_gmin_ps(v14, v13);
    t11 = _mm512_gmax_ps(v11, v7 );
    t7  = _mm512_gmin_ps(v11, v7 );
    t2  = _mm512_gmax_ps(v2 , v1 );
    t1  = _mm512_gmin_ps(v2 , v1 );
    t8  = _mm512_gmax_ps(v8 , v4 );
    t4  = _mm512_gmin_ps(v8 , v4 );
    t0  = v0 ;
    t15 = v15;
    // step 6 
    v4  = _mm512_gmax_ps(t4 , t1 );
    v1  = _mm512_gmin_ps(t4 , t1 );
    v13 = _mm512_gmax_ps(t13, t7 );
    v7  = _mm512_gmin_ps(t13, t7 );
    v8  = _mm512_gmax_ps(t8 , t2 );
    v2  = _mm512_gmin_ps(t8 , t2 );
    v14 = _mm512_gmax_ps(t14, t11);
    v11 = _mm512_gmin_ps(t14, t11);
    v6  = _mm512_gmax_ps(t6 , t5 );
    v5  = _mm512_gmin_ps(t6 , t5 );
    v10 = _mm512_gmax_ps(t10, t9 );
    v9  = _mm512_gmin_ps(t10, t9 );
    v0  = t0 ;
    v3  = t3 ;
    v12 = t12;
    v15 = t15;
    // step 7 
    t4  = _mm512_gmax_ps(v4 , v2 );
    t2  = _mm512_gmin_ps(v4 , v2 );
    t13 = _mm512_gmax_ps(v13, v11);
    t11 = _mm512_gmin_ps(v13, v11);
    t8  = _mm512_gmax_ps(v8 , v3 );
    t3  = _mm512_gmin_ps(v8 , v3 );
    t12 = _mm512_gmax_ps(v12, v7 );
    t7  = _mm512_gmin_ps(v12, v7 );
    t0  = v0 ;
    t1  = v1 ;
    t5  = v5 ;
    t6  = v6 ;
    t9  = v9 ;
    t10 = v10;
    t14 = v14;
    t15 = v15;
    // step 8 
    v8  = _mm512_gmax_ps(t8 , t6 );
    v6  = _mm512_gmin_ps(t8 , t6 );
    v12 = _mm512_gmax_ps(t12, t10);
    v10 = _mm512_gmin_ps(t12, t10);
    v5  = _mm512_gmax_ps(t5 , t3 );
    v3  = _mm512_gmin_ps(t5 , t3 );
    v9  = _mm512_gmax_ps(t9 , t7 );
    v7  = _mm512_gmin_ps(t9 , t7 );
    v0  = t0;
    v1  = t1;
    v2  = t2;
    v4  = t4;
    v11 = t11;
    v13 = t13;
    v14 = t14;
    v15 = t15;
    // step 9 
    t4  = _mm512_gmax_ps(v4 , v3 );
    t3  = _mm512_gmin_ps(v4 , v3 );
    t6  = _mm512_gmax_ps(v6 , v5 );
    t5  = _mm512_gmin_ps(v6 , v5 );
    t8  = _mm512_gmax_ps(v8 , v7 );
    t7  = _mm512_gmin_ps(v8 , v7 );
    t10 = _mm512_gmax_ps(v10, v9 );
    t9  = _mm512_gmin_ps(v10, v9 );
    t12 = _mm512_gmax_ps(v12, v11);
    t11 = _mm512_gmin_ps(v12, v11);
    t0  = v0;
    t1  = v1;
    t2  = v2;
    t13 = v13;
    t14 = v14;
    t15 = v15;
    // step 10 
    v7  = _mm512_gmax_ps(t7 , t6 );
    v6  = _mm512_gmin_ps(t7 , t6 );
    v9  = _mm512_gmax_ps(t9 , t8 );
    v8  = _mm512_gmin_ps(t9 , t8 );
    v0  = t0 ;
    v1  = t1 ;
    v2  = t2 ;
    v3  = t3 ;
    v4  = t4 ;
    v5  = t5 ;
    v10 = t10;
    v11 = t11;
    v12 = t12;
    v13 = t13;
    v14 = t14;
    v15 = t15;

}

/**
 * Double vector version (__m512d):
 * This method performs the in-register sort. The sorted elements 
 * are stored vertically accross the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored vertically among the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_sort(T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7)
{
    __m512d t0 ;
    __m512d t1 ;
    __m512d t2 ;
    __m512d t3 ;
    __m512d t4 ;
    __m512d t5 ;
    __m512d t6 ;
    __m512d t7 ;
    // step 1 
    t4 = _mm512_gmax_pd(v4, v0);
    t0 = _mm512_gmin_pd(v4, v0);
    t5 = _mm512_gmax_pd(v5, v1);
    t1 = _mm512_gmin_pd(v5, v1);
    t6 = _mm512_gmax_pd(v6, v2);
    t2 = _mm512_gmin_pd(v6, v2);
    t7 = _mm512_gmax_pd(v7, v3);
    t3 = _mm512_gmin_pd(v7, v3);
    // step 2 
    v2 = _mm512_gmax_pd(t2, t0);
    v0 = _mm512_gmin_pd(t2, t0);
    v3 = _mm512_gmax_pd(t3, t1);
    v1 = _mm512_gmin_pd(t3, t1);
    v6 = _mm512_gmax_pd(t6, t4);
    v4 = _mm512_gmin_pd(t6, t4);
    v7 = _mm512_gmax_pd(t7, t5);
    v5 = _mm512_gmin_pd(t7, t5);
    // step 3 
    t4 = _mm512_gmax_pd(v4, v2);
    t2 = _mm512_gmin_pd(v4, v2);
    t5 = _mm512_gmax_pd(v5, v3);
    t3 = _mm512_gmin_pd(v5, v3);
    t1 = _mm512_gmax_pd(v1, v0);
    t0 = _mm512_gmin_pd(v1, v0);
    t7 = _mm512_gmax_pd(v7, v6);
    t6 = _mm512_gmin_pd(v7, v6);
    // step 4 
    v3 = _mm512_gmax_pd(t3, t2);
    v2 = _mm512_gmin_pd(t3, t2);
    v5 = _mm512_gmax_pd(t5, t4);
    v4 = _mm512_gmin_pd(t5, t4);
    v0 = t0;
    v1 = t1;
    v6 = t6;
    v7 = t7;
    // step 5 
    t4 = _mm512_gmax_pd(v4, v1);
    t1 = _mm512_gmin_pd(v4, v1);
    t6 = _mm512_gmax_pd(v6, v3);
    t3 = _mm512_gmin_pd(v6, v3);
    t0 = v0;
    t2 = v2;
    t5 = v5;
    t7 = v7;
    // step 6 
    v2 = _mm512_gmax_pd(t2, t1);
    v1 = _mm512_gmin_pd(t2, t1);
    v4 = _mm512_gmax_pd(t4, t3);
    v3 = _mm512_gmin_pd(t4, t3);
    v6 = _mm512_gmax_pd(t6, t5);
    v5 = _mm512_gmin_pd(t6, t5);
    v0 = t0;
    v7 = t7;
}

/**
 * Integer vector version (__m512i):
 * This method performs the in-register transpose. The sorted elements 
 * are stored horizontally within the registers.
 *
 * @param v0-v15 vector data registers
 * @return sorted data stored horizontally in the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_transpose(T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
                      T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15)
{
    __m512i t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
    __mmask16 mask1;
    mask1 = 0xaaaa;
    t0  = _mm512_mask_swizzle_epi32(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_epi32(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_epi32(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_epi32(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 
    t8  = _mm512_mask_swizzle_epi32(v8 , mask1, v9 , _MM_SWIZ_REG_CDAB); 
    t10 = _mm512_mask_swizzle_epi32(v10, mask1, v11, _MM_SWIZ_REG_CDAB); 
    t12 = _mm512_mask_swizzle_epi32(v12, mask1, v13, _MM_SWIZ_REG_CDAB); 
    t14 = _mm512_mask_swizzle_epi32(v14, mask1, v15, _MM_SWIZ_REG_CDAB); 

    mask1 = 0x5555;
    t1  = _mm512_mask_swizzle_epi32(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_epi32(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_epi32(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_epi32(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 
    t9  = _mm512_mask_swizzle_epi32(v9 , mask1, v8 , _MM_SWIZ_REG_CDAB); 
    t11 = _mm512_mask_swizzle_epi32(v11, mask1, v10, _MM_SWIZ_REG_CDAB); 
    t13 = _mm512_mask_swizzle_epi32(v13, mask1, v12, _MM_SWIZ_REG_CDAB); 
    t15 = _mm512_mask_swizzle_epi32(v15, mask1, v14, _MM_SWIZ_REG_CDAB); 

    mask1 = 0xcccc;
    v0  = _mm512_mask_swizzle_epi32(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_epi32(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_epi32(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_epi32(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 
    v8  = _mm512_mask_swizzle_epi32(t8 , mask1, t10, _MM_SWIZ_REG_BADC); 
    v10 = _mm512_mask_swizzle_epi32(t9 , mask1, t11, _MM_SWIZ_REG_BADC); 
    v12 = _mm512_mask_swizzle_epi32(t12, mask1, t14, _MM_SWIZ_REG_BADC); 
    v14 = _mm512_mask_swizzle_epi32(t13, mask1, t15, _MM_SWIZ_REG_BADC); 

    mask1 = 0x3333;
    v1  = _mm512_mask_swizzle_epi32(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_epi32(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_epi32(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_epi32(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 
    v9  = _mm512_mask_swizzle_epi32(t10, mask1, t8 , _MM_SWIZ_REG_BADC); 
    v11 = _mm512_mask_swizzle_epi32(t11, mask1, t9 , _MM_SWIZ_REG_BADC); 
    v13 = _mm512_mask_swizzle_epi32(t14, mask1, t12, _MM_SWIZ_REG_BADC); 
    v15 = _mm512_mask_swizzle_epi32(t15, mask1, t13, _MM_SWIZ_REG_BADC); 

    mask1 = 0xf0f0;
    t0  = _mm512_mask_permute4f128_epi32(v0 , mask1, v4 , _MM_PERM_CDAB); 
    t1  = _mm512_mask_permute4f128_epi32(v1 , mask1, v5 , _MM_PERM_CDAB); 
    t2  = _mm512_mask_permute4f128_epi32(v2 , mask1, v6 , _MM_PERM_CDAB); 
    t3  = _mm512_mask_permute4f128_epi32(v3 , mask1, v7 , _MM_PERM_CDAB); 
    t8  = _mm512_mask_permute4f128_epi32(v8 , mask1, v12, _MM_PERM_CDAB); 
    t9  = _mm512_mask_permute4f128_epi32(v9 , mask1, v13, _MM_PERM_CDAB); 
    t10 = _mm512_mask_permute4f128_epi32(v10, mask1, v14, _MM_PERM_CDAB); 
    t11 = _mm512_mask_permute4f128_epi32(v11, mask1, v15, _MM_PERM_CDAB); 

    mask1 = 0x0f0f;
    t4  = _mm512_mask_permute4f128_epi32(v4 , mask1, v0 , _MM_PERM_CDAB); 
    t5  = _mm512_mask_permute4f128_epi32(v5 , mask1, v1 , _MM_PERM_CDAB); 
    t6  = _mm512_mask_permute4f128_epi32(v6 , mask1, v2 , _MM_PERM_CDAB); 
    t7  = _mm512_mask_permute4f128_epi32(v7 , mask1, v3 , _MM_PERM_CDAB); 
    t12 = _mm512_mask_permute4f128_epi32(v12, mask1, v8 , _MM_PERM_CDAB); 
    t13 = _mm512_mask_permute4f128_epi32(v13, mask1, v9 , _MM_PERM_CDAB); 
    t14 = _mm512_mask_permute4f128_epi32(v14, mask1, v10, _MM_PERM_CDAB); 
    t15 = _mm512_mask_permute4f128_epi32(v15, mask1, v11, _MM_PERM_CDAB); 

    mask1 = 0xff00;
    v0 = _mm512_mask_permute4f128_epi32(t0, mask1, t8 , _MM_PERM_BADC); 
    v2 = _mm512_mask_permute4f128_epi32(t1, mask1, t9 , _MM_PERM_BADC); 
    v1 = _mm512_mask_permute4f128_epi32(t2, mask1, t10, _MM_PERM_BADC); 
    v3 = _mm512_mask_permute4f128_epi32(t3, mask1, t11, _MM_PERM_BADC); 
    v4 = _mm512_mask_permute4f128_epi32(t4, mask1, t12, _MM_PERM_BADC); 
    v6 = _mm512_mask_permute4f128_epi32(t5, mask1, t13, _MM_PERM_BADC); 
    v5 = _mm512_mask_permute4f128_epi32(t6, mask1, t14, _MM_PERM_BADC); 
    v7 = _mm512_mask_permute4f128_epi32(t7, mask1, t15, _MM_PERM_BADC); 

    mask1 = 0x00ff;
    v8  = _mm512_mask_permute4f128_epi32(t8 , mask1, t0, _MM_PERM_BADC); 
    v10 = _mm512_mask_permute4f128_epi32(t9 , mask1, t1, _MM_PERM_BADC); 
    v9  = _mm512_mask_permute4f128_epi32(t10, mask1, t2, _MM_PERM_BADC); 
    v11 = _mm512_mask_permute4f128_epi32(t11, mask1, t3, _MM_PERM_BADC); 
    v12 = _mm512_mask_permute4f128_epi32(t12, mask1, t4, _MM_PERM_BADC); 
    v14 = _mm512_mask_permute4f128_epi32(t13, mask1, t5, _MM_PERM_BADC); 
    v13 = _mm512_mask_permute4f128_epi32(t14, mask1, t6, _MM_PERM_BADC); 
    v15 = _mm512_mask_permute4f128_epi32(t15, mask1, t7, _MM_PERM_BADC);
}


/**
 * Integer vector version (__m512i):
 * This method performs the in-register transpose. The sorted elements 
 * are stored horizontally within the registers.
 *
 * @param v0-v15 vector data registers
 * @return sorted data stored horizontally in the registers
 *
 */
template <typename T> // int key int index
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_transpose_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3 , 
        __m512i &vp4 , __m512i &vp5 , __m512i &vp6 , __m512i &vp7 , 
        __m512i &vp8 , __m512i &vp9 , __m512i &vp10, __m512i &vp11, 
        __m512i &vp12, __m512i &vp13, __m512i &vp14, __m512i &vp15)

{
    __m512i t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
    __m512i tp0, tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8, tp9, tp10, tp11, tp12, tp13, tp14, tp15;
    __mmask16 mask1;
    mask1 = 0xaaaa;
    t0  = _mm512_mask_swizzle_epi32(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_epi32(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_epi32(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_epi32(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 
    t8  = _mm512_mask_swizzle_epi32(v8 , mask1, v9 , _MM_SWIZ_REG_CDAB); 
    t10 = _mm512_mask_swizzle_epi32(v10, mask1, v11, _MM_SWIZ_REG_CDAB); 
    t12 = _mm512_mask_swizzle_epi32(v12, mask1, v13, _MM_SWIZ_REG_CDAB); 
    t14 = _mm512_mask_swizzle_epi32(v14, mask1, v15, _MM_SWIZ_REG_CDAB); 

    tp0  = _mm512_mask_swizzle_epi32(vp0 , mask1, vp1 , _MM_SWIZ_REG_CDAB); 
    tp2  = _mm512_mask_swizzle_epi32(vp2 , mask1, vp3 , _MM_SWIZ_REG_CDAB); 
    tp4  = _mm512_mask_swizzle_epi32(vp4 , mask1, vp5 , _MM_SWIZ_REG_CDAB); 
    tp6  = _mm512_mask_swizzle_epi32(vp6 , mask1, vp7 , _MM_SWIZ_REG_CDAB); 
    tp8  = _mm512_mask_swizzle_epi32(vp8 , mask1, vp9 , _MM_SWIZ_REG_CDAB); 
    tp10 = _mm512_mask_swizzle_epi32(vp10, mask1, vp11, _MM_SWIZ_REG_CDAB); 
    tp12 = _mm512_mask_swizzle_epi32(vp12, mask1, vp13, _MM_SWIZ_REG_CDAB); 
    tp14 = _mm512_mask_swizzle_epi32(vp14, mask1, vp15, _MM_SWIZ_REG_CDAB); 
    mask1 = 0x5555;
    t1  = _mm512_mask_swizzle_epi32(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_epi32(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_epi32(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_epi32(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 
    t9  = _mm512_mask_swizzle_epi32(v9 , mask1, v8 , _MM_SWIZ_REG_CDAB); 
    t11 = _mm512_mask_swizzle_epi32(v11, mask1, v10, _MM_SWIZ_REG_CDAB); 
    t13 = _mm512_mask_swizzle_epi32(v13, mask1, v12, _MM_SWIZ_REG_CDAB); 
    t15 = _mm512_mask_swizzle_epi32(v15, mask1, v14, _MM_SWIZ_REG_CDAB); 

    tp1  = _mm512_mask_swizzle_epi32(vp1 , mask1, vp0 , _MM_SWIZ_REG_CDAB); 
    tp3  = _mm512_mask_swizzle_epi32(vp3 , mask1, vp2 , _MM_SWIZ_REG_CDAB); 
    tp5  = _mm512_mask_swizzle_epi32(vp5 , mask1, vp4 , _MM_SWIZ_REG_CDAB); 
    tp7  = _mm512_mask_swizzle_epi32(vp7 , mask1, vp6 , _MM_SWIZ_REG_CDAB); 
    tp9  = _mm512_mask_swizzle_epi32(vp9 , mask1, vp8 , _MM_SWIZ_REG_CDAB); 
    tp11 = _mm512_mask_swizzle_epi32(vp11, mask1, vp10, _MM_SWIZ_REG_CDAB); 
    tp13 = _mm512_mask_swizzle_epi32(vp13, mask1, vp12, _MM_SWIZ_REG_CDAB); 
    tp15 = _mm512_mask_swizzle_epi32(vp15, mask1, vp14, _MM_SWIZ_REG_CDAB); 
    mask1 = 0xcccc;
    v0  = _mm512_mask_swizzle_epi32(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_epi32(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_epi32(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_epi32(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 
    v8  = _mm512_mask_swizzle_epi32(t8 , mask1, t10, _MM_SWIZ_REG_BADC); 
    v10 = _mm512_mask_swizzle_epi32(t9 , mask1, t11, _MM_SWIZ_REG_BADC); 
    v12 = _mm512_mask_swizzle_epi32(t12, mask1, t14, _MM_SWIZ_REG_BADC); 
    v14 = _mm512_mask_swizzle_epi32(t13, mask1, t15, _MM_SWIZ_REG_BADC); 

    vp0  = _mm512_mask_swizzle_epi32(tp0 , mask1, tp2 , _MM_SWIZ_REG_BADC); 
    vp2  = _mm512_mask_swizzle_epi32(tp1 , mask1, tp3 , _MM_SWIZ_REG_BADC); 
    vp4  = _mm512_mask_swizzle_epi32(tp4 , mask1, tp6 , _MM_SWIZ_REG_BADC); 
    vp6  = _mm512_mask_swizzle_epi32(tp5 , mask1, tp7 , _MM_SWIZ_REG_BADC); 
    vp8  = _mm512_mask_swizzle_epi32(tp8 , mask1, tp10, _MM_SWIZ_REG_BADC); 
    vp10 = _mm512_mask_swizzle_epi32(tp9 , mask1, tp11, _MM_SWIZ_REG_BADC); 
    vp12 = _mm512_mask_swizzle_epi32(tp12, mask1, tp14, _MM_SWIZ_REG_BADC); 
    vp14 = _mm512_mask_swizzle_epi32(tp13, mask1, tp15, _MM_SWIZ_REG_BADC); 
    mask1 = 0x3333;
    v1  = _mm512_mask_swizzle_epi32(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_epi32(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_epi32(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_epi32(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 
    v9  = _mm512_mask_swizzle_epi32(t10, mask1, t8 , _MM_SWIZ_REG_BADC); 
    v11 = _mm512_mask_swizzle_epi32(t11, mask1, t9 , _MM_SWIZ_REG_BADC); 
    v13 = _mm512_mask_swizzle_epi32(t14, mask1, t12, _MM_SWIZ_REG_BADC); 
    v15 = _mm512_mask_swizzle_epi32(t15, mask1, t13, _MM_SWIZ_REG_BADC); 

    vp1  = _mm512_mask_swizzle_epi32(tp2 , mask1, tp0 , _MM_SWIZ_REG_BADC); 
    vp3  = _mm512_mask_swizzle_epi32(tp3 , mask1, tp1 , _MM_SWIZ_REG_BADC); 
    vp5  = _mm512_mask_swizzle_epi32(tp6 , mask1, tp4 , _MM_SWIZ_REG_BADC); 
    vp7  = _mm512_mask_swizzle_epi32(tp7 , mask1, tp5 , _MM_SWIZ_REG_BADC); 
    vp9  = _mm512_mask_swizzle_epi32(tp10, mask1, tp8 , _MM_SWIZ_REG_BADC); 
    vp11 = _mm512_mask_swizzle_epi32(tp11, mask1, tp9 , _MM_SWIZ_REG_BADC); 
    vp13 = _mm512_mask_swizzle_epi32(tp14, mask1, tp12, _MM_SWIZ_REG_BADC); 
    vp15 = _mm512_mask_swizzle_epi32(tp15, mask1, tp13, _MM_SWIZ_REG_BADC); 
    mask1 = 0xf0f0;
    t0  = _mm512_mask_permute4f128_epi32(v0 , mask1, v4 , _MM_PERM_CDAB); 
    t1  = _mm512_mask_permute4f128_epi32(v1 , mask1, v5 , _MM_PERM_CDAB); 
    t2  = _mm512_mask_permute4f128_epi32(v2 , mask1, v6 , _MM_PERM_CDAB); 
    t3  = _mm512_mask_permute4f128_epi32(v3 , mask1, v7 , _MM_PERM_CDAB); 
    t8  = _mm512_mask_permute4f128_epi32(v8 , mask1, v12, _MM_PERM_CDAB); 
    t9  = _mm512_mask_permute4f128_epi32(v9 , mask1, v13, _MM_PERM_CDAB); 
    t10 = _mm512_mask_permute4f128_epi32(v10, mask1, v14, _MM_PERM_CDAB); 
    t11 = _mm512_mask_permute4f128_epi32(v11, mask1, v15, _MM_PERM_CDAB); 

    tp0  = _mm512_mask_permute4f128_epi32(vp0 , mask1, vp4 , _MM_PERM_CDAB); 
    tp1  = _mm512_mask_permute4f128_epi32(vp1 , mask1, vp5 , _MM_PERM_CDAB); 
    tp2  = _mm512_mask_permute4f128_epi32(vp2 , mask1, vp6 , _MM_PERM_CDAB); 
    tp3  = _mm512_mask_permute4f128_epi32(vp3 , mask1, vp7 , _MM_PERM_CDAB); 
    tp8  = _mm512_mask_permute4f128_epi32(vp8 , mask1, vp12, _MM_PERM_CDAB); 
    tp9  = _mm512_mask_permute4f128_epi32(vp9 , mask1, vp13, _MM_PERM_CDAB); 
    tp10 = _mm512_mask_permute4f128_epi32(vp10, mask1, vp14, _MM_PERM_CDAB); 
    tp11 = _mm512_mask_permute4f128_epi32(vp11, mask1, vp15, _MM_PERM_CDAB); 
    mask1 = 0x0f0f;
    t4  = _mm512_mask_permute4f128_epi32(v4 , mask1, v0 , _MM_PERM_CDAB); 
    t5  = _mm512_mask_permute4f128_epi32(v5 , mask1, v1 , _MM_PERM_CDAB); 
    t6  = _mm512_mask_permute4f128_epi32(v6 , mask1, v2 , _MM_PERM_CDAB); 
    t7  = _mm512_mask_permute4f128_epi32(v7 , mask1, v3 , _MM_PERM_CDAB); 
    t12 = _mm512_mask_permute4f128_epi32(v12, mask1, v8 , _MM_PERM_CDAB); 
    t13 = _mm512_mask_permute4f128_epi32(v13, mask1, v9 , _MM_PERM_CDAB); 
    t14 = _mm512_mask_permute4f128_epi32(v14, mask1, v10, _MM_PERM_CDAB); 
    t15 = _mm512_mask_permute4f128_epi32(v15, mask1, v11, _MM_PERM_CDAB); 

    tp4  = _mm512_mask_permute4f128_epi32(vp4 , mask1, vp0 , _MM_PERM_CDAB); 
    tp5  = _mm512_mask_permute4f128_epi32(vp5 , mask1, vp1 , _MM_PERM_CDAB); 
    tp6  = _mm512_mask_permute4f128_epi32(vp6 , mask1, vp2 , _MM_PERM_CDAB); 
    tp7  = _mm512_mask_permute4f128_epi32(vp7 , mask1, vp3 , _MM_PERM_CDAB); 
    tp12 = _mm512_mask_permute4f128_epi32(vp12, mask1, vp8 , _MM_PERM_CDAB); 
    tp13 = _mm512_mask_permute4f128_epi32(vp13, mask1, vp9 , _MM_PERM_CDAB); 
    tp14 = _mm512_mask_permute4f128_epi32(vp14, mask1, vp10, _MM_PERM_CDAB); 
    tp15 = _mm512_mask_permute4f128_epi32(vp15, mask1, vp11, _MM_PERM_CDAB); 
    mask1 = 0xff00;
    v0 = _mm512_mask_permute4f128_epi32(t0, mask1, t8 , _MM_PERM_BADC); 
    v2 = _mm512_mask_permute4f128_epi32(t1, mask1, t9 , _MM_PERM_BADC); 
    v1 = _mm512_mask_permute4f128_epi32(t2, mask1, t10, _MM_PERM_BADC); 
    v3 = _mm512_mask_permute4f128_epi32(t3, mask1, t11, _MM_PERM_BADC); 
    v4 = _mm512_mask_permute4f128_epi32(t4, mask1, t12, _MM_PERM_BADC); 
    v6 = _mm512_mask_permute4f128_epi32(t5, mask1, t13, _MM_PERM_BADC); 
    v5 = _mm512_mask_permute4f128_epi32(t6, mask1, t14, _MM_PERM_BADC); 
    v7 = _mm512_mask_permute4f128_epi32(t7, mask1, t15, _MM_PERM_BADC); 

    vp0 = _mm512_mask_permute4f128_epi32(tp0, mask1, tp8 , _MM_PERM_BADC); 
    vp2 = _mm512_mask_permute4f128_epi32(tp1, mask1, tp9 , _MM_PERM_BADC); 
    vp1 = _mm512_mask_permute4f128_epi32(tp2, mask1, tp10, _MM_PERM_BADC); 
    vp3 = _mm512_mask_permute4f128_epi32(tp3, mask1, tp11, _MM_PERM_BADC); 
    vp4 = _mm512_mask_permute4f128_epi32(tp4, mask1, tp12, _MM_PERM_BADC); 
    vp6 = _mm512_mask_permute4f128_epi32(tp5, mask1, tp13, _MM_PERM_BADC); 
    vp5 = _mm512_mask_permute4f128_epi32(tp6, mask1, tp14, _MM_PERM_BADC); 
    vp7 = _mm512_mask_permute4f128_epi32(tp7, mask1, tp15, _MM_PERM_BADC); 
    mask1 = 0x00ff;
    v8  = _mm512_mask_permute4f128_epi32(t8 , mask1, t0, _MM_PERM_BADC); 
    v10 = _mm512_mask_permute4f128_epi32(t9 , mask1, t1, _MM_PERM_BADC); 
    v9  = _mm512_mask_permute4f128_epi32(t10, mask1, t2, _MM_PERM_BADC); 
    v11 = _mm512_mask_permute4f128_epi32(t11, mask1, t3, _MM_PERM_BADC); 
    v12 = _mm512_mask_permute4f128_epi32(t12, mask1, t4, _MM_PERM_BADC); 
    v14 = _mm512_mask_permute4f128_epi32(t13, mask1, t5, _MM_PERM_BADC); 
    v13 = _mm512_mask_permute4f128_epi32(t14, mask1, t6, _MM_PERM_BADC); 
    v15 = _mm512_mask_permute4f128_epi32(t15, mask1, t7, _MM_PERM_BADC);

    vp8  = _mm512_mask_permute4f128_epi32(tp8 , mask1, tp0, _MM_PERM_BADC); 
    vp10 = _mm512_mask_permute4f128_epi32(tp9 , mask1, tp1, _MM_PERM_BADC); 
    vp9  = _mm512_mask_permute4f128_epi32(tp10, mask1, tp2, _MM_PERM_BADC); 
    vp11 = _mm512_mask_permute4f128_epi32(tp11, mask1, tp3, _MM_PERM_BADC); 
    vp12 = _mm512_mask_permute4f128_epi32(tp12, mask1, tp4, _MM_PERM_BADC); 
    vp14 = _mm512_mask_permute4f128_epi32(tp13, mask1, tp5, _MM_PERM_BADC); 
    vp13 = _mm512_mask_permute4f128_epi32(tp14, mask1, tp6, _MM_PERM_BADC); 
    vp15 = _mm512_mask_permute4f128_epi32(tp15, mask1, tp7, _MM_PERM_BADC);
}

template <typename T> // int key long index
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_transpose_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vlp0 , __m512i &vlp1 , __m512i &vlp2 , __m512i &vlp3 , 
        __m512i &vlp4 , __m512i &vlp5 , __m512i &vlp6 , __m512i &vlp7 , 
        __m512i &vlp8 , __m512i &vlp9 , __m512i &vlp10, __m512i &vlp11, 
        __m512i &vlp12, __m512i &vlp13, __m512i &vlp14, __m512i &vlp15,
        __m512i &vhp0 , __m512i &vhp1 , __m512i &vhp2 , __m512i &vhp3 , 
        __m512i &vhp4 , __m512i &vhp5 , __m512i &vhp6 , __m512i &vhp7 , 
        __m512i &vhp8 , __m512i &vhp9 , __m512i &vhp10, __m512i &vhp11, 
        __m512i &vhp12, __m512i &vhp13, __m512i &vhp14, __m512i &vhp15)

{
    __m512i t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
    __m512i tlp0, tlp1, tlp2, tlp3, tlp4, tlp5, tlp6, tlp7, tlp8, tlp9, tlp10, tlp11, tlp12, tlp13, tlp14, tlp15;
    __m512i thp0, thp1, thp2, thp3, thp4, thp5, thp6, thp7, thp8, thp9, thp10, thp11, thp12, thp13, thp14, thp15;
    __mmask16 mask1;
    mask1 = 0xaaaa;
    t0  = _mm512_mask_swizzle_epi32(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_epi32(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_epi32(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_epi32(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 
    t8  = _mm512_mask_swizzle_epi32(v8 , mask1, v9 , _MM_SWIZ_REG_CDAB); 
    t10 = _mm512_mask_swizzle_epi32(v10, mask1, v11, _MM_SWIZ_REG_CDAB); 
    t12 = _mm512_mask_swizzle_epi32(v12, mask1, v13, _MM_SWIZ_REG_CDAB); 
    t14 = _mm512_mask_swizzle_epi32(v14, mask1, v15, _MM_SWIZ_REG_CDAB); 

    tlp0  = _mm512_mask_swizzle_epi64(vlp0 , mask1, vlp1 , _MM_SWIZ_REG_CDAB); 
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , mask1, vlp3 , _MM_SWIZ_REG_CDAB); 
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , mask1, vlp5 , _MM_SWIZ_REG_CDAB); 
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , mask1, vlp7 , _MM_SWIZ_REG_CDAB); 
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , mask1, vlp9 , _MM_SWIZ_REG_CDAB); 
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, mask1, vlp11, _MM_SWIZ_REG_CDAB); 
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, mask1, vlp13, _MM_SWIZ_REG_CDAB); 
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, mask1, vlp15, _MM_SWIZ_REG_CDAB); 

    thp0  = _mm512_mask_swizzle_epi64(vhp0 , mask1, vhp1 , _MM_SWIZ_REG_CDAB); 
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , mask1, vhp3 , _MM_SWIZ_REG_CDAB); 
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , mask1, vhp5 , _MM_SWIZ_REG_CDAB); 
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , mask1, vhp7 , _MM_SWIZ_REG_CDAB); 
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , mask1, vhp9 , _MM_SWIZ_REG_CDAB); 
    thp10 = _mm512_mask_swizzle_epi64(vhp10, mask1, vhp11, _MM_SWIZ_REG_CDAB); 
    thp12 = _mm512_mask_swizzle_epi64(vhp12, mask1, vhp13, _MM_SWIZ_REG_CDAB); 
    thp14 = _mm512_mask_swizzle_epi64(vhp14, mask1, vhp15, _MM_SWIZ_REG_CDAB); 

    mask1 = 0x5555;
    t1  = _mm512_mask_swizzle_epi32(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_epi32(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_epi32(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_epi32(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 
    t9  = _mm512_mask_swizzle_epi32(v9 , mask1, v8 , _MM_SWIZ_REG_CDAB); 
    t11 = _mm512_mask_swizzle_epi32(v11, mask1, v10, _MM_SWIZ_REG_CDAB); 
    t13 = _mm512_mask_swizzle_epi32(v13, mask1, v12, _MM_SWIZ_REG_CDAB); 
    t15 = _mm512_mask_swizzle_epi32(v15, mask1, v14, _MM_SWIZ_REG_CDAB); 

    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , mask1, vlp0 , _MM_SWIZ_REG_CDAB); 
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , mask1, vlp2 , _MM_SWIZ_REG_CDAB); 
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , mask1, vlp4 , _MM_SWIZ_REG_CDAB); 
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , mask1, vlp6 , _MM_SWIZ_REG_CDAB); 
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , mask1, vlp8 , _MM_SWIZ_REG_CDAB); 
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, mask1, vlp10, _MM_SWIZ_REG_CDAB); 
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, mask1, vlp12, _MM_SWIZ_REG_CDAB); 
    tlp15 = _mm512_mask_swizzle_epi64(vlp15, mask1, vlp14, _MM_SWIZ_REG_CDAB); 
    
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , mask1, vhp0 , _MM_SWIZ_REG_CDAB); 
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , mask1, vhp2 , _MM_SWIZ_REG_CDAB); 
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , mask1, vhp4 , _MM_SWIZ_REG_CDAB); 
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , mask1, vhp6 , _MM_SWIZ_REG_CDAB); 
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , mask1, vhp8 , _MM_SWIZ_REG_CDAB); 
    thp11 = _mm512_mask_swizzle_epi64(vhp11, mask1, vhp10, _MM_SWIZ_REG_CDAB); 
    thp13 = _mm512_mask_swizzle_epi64(vhp13, mask1, vhp12, _MM_SWIZ_REG_CDAB); 
    thp15 = _mm512_mask_swizzle_epi64(vhp15, mask1, vhp14, _MM_SWIZ_REG_CDAB); 
    mask1 = 0xcccc;
    v0  = _mm512_mask_swizzle_epi32(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_epi32(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_epi32(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_epi32(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 
    v8  = _mm512_mask_swizzle_epi32(t8 , mask1, t10, _MM_SWIZ_REG_BADC); 
    v10 = _mm512_mask_swizzle_epi32(t9 , mask1, t11, _MM_SWIZ_REG_BADC); 
    v12 = _mm512_mask_swizzle_epi32(t12, mask1, t14, _MM_SWIZ_REG_BADC); 
    v14 = _mm512_mask_swizzle_epi32(t13, mask1, t15, _MM_SWIZ_REG_BADC); 

    vlp0  = _mm512_mask_swizzle_epi64(tlp0 , mask1, tlp2 , _MM_SWIZ_REG_BADC); 
    vlp2  = _mm512_mask_swizzle_epi64(tlp1 , mask1, tlp3 , _MM_SWIZ_REG_BADC); 
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , mask1, tlp6 , _MM_SWIZ_REG_BADC); 
    vlp6  = _mm512_mask_swizzle_epi64(tlp5 , mask1, tlp7 , _MM_SWIZ_REG_BADC); 
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , mask1, tlp10, _MM_SWIZ_REG_BADC); 
    vlp10 = _mm512_mask_swizzle_epi64(tlp9 , mask1, tlp11, _MM_SWIZ_REG_BADC); 
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, mask1, tlp14, _MM_SWIZ_REG_BADC); 
    vlp14 = _mm512_mask_swizzle_epi64(tlp13, mask1, tlp15, _MM_SWIZ_REG_BADC); 

    vhp0  = _mm512_mask_swizzle_epi64(thp0 , mask1, thp2 , _MM_SWIZ_REG_BADC); 
    vhp2  = _mm512_mask_swizzle_epi64(thp1 , mask1, thp3 , _MM_SWIZ_REG_BADC); 
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , mask1, thp6 , _MM_SWIZ_REG_BADC); 
    vhp6  = _mm512_mask_swizzle_epi64(thp5 , mask1, thp7 , _MM_SWIZ_REG_BADC); 
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , mask1, thp10, _MM_SWIZ_REG_BADC); 
    vhp10 = _mm512_mask_swizzle_epi64(thp9 , mask1, thp11, _MM_SWIZ_REG_BADC); 
    vhp12 = _mm512_mask_swizzle_epi64(thp12, mask1, thp14, _MM_SWIZ_REG_BADC); 
    vhp14 = _mm512_mask_swizzle_epi64(thp13, mask1, thp15, _MM_SWIZ_REG_BADC); 
    mask1 = 0x3333;
    v1  = _mm512_mask_swizzle_epi32(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_epi32(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_epi32(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_epi32(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 
    v9  = _mm512_mask_swizzle_epi32(t10, mask1, t8 , _MM_SWIZ_REG_BADC); 
    v11 = _mm512_mask_swizzle_epi32(t11, mask1, t9 , _MM_SWIZ_REG_BADC); 
    v13 = _mm512_mask_swizzle_epi32(t14, mask1, t12, _MM_SWIZ_REG_BADC); 
    v15 = _mm512_mask_swizzle_epi32(t15, mask1, t13, _MM_SWIZ_REG_BADC); 

    vlp1  = _mm512_mask_swizzle_epi64(tlp2 , mask1, tlp0 , _MM_SWIZ_REG_BADC); 
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , mask1, tlp1 , _MM_SWIZ_REG_BADC); 
    vlp5  = _mm512_mask_swizzle_epi64(tlp6 , mask1, tlp4 , _MM_SWIZ_REG_BADC); 
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , mask1, tlp5 , _MM_SWIZ_REG_BADC); 
    vlp9  = _mm512_mask_swizzle_epi64(tlp10, mask1, tlp8 , _MM_SWIZ_REG_BADC); 
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, mask1, tlp9 , _MM_SWIZ_REG_BADC); 
    vlp13 = _mm512_mask_swizzle_epi64(tlp14, mask1, tlp12, _MM_SWIZ_REG_BADC); 
    vlp15 = _mm512_mask_swizzle_epi64(tlp15, mask1, tlp13, _MM_SWIZ_REG_BADC); 

    vhp1  = _mm512_mask_swizzle_epi64(thp2 , mask1, thp0 , _MM_SWIZ_REG_BADC); 
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , mask1, thp1 , _MM_SWIZ_REG_BADC); 
    vhp5  = _mm512_mask_swizzle_epi64(thp6 , mask1, thp4 , _MM_SWIZ_REG_BADC); 
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , mask1, thp5 , _MM_SWIZ_REG_BADC); 
    vhp9  = _mm512_mask_swizzle_epi64(thp10, mask1, thp8 , _MM_SWIZ_REG_BADC); 
    vhp11 = _mm512_mask_swizzle_epi64(thp11, mask1, thp9 , _MM_SWIZ_REG_BADC); 
    vhp13 = _mm512_mask_swizzle_epi64(thp14, mask1, thp12, _MM_SWIZ_REG_BADC); 
    vhp15 = _mm512_mask_swizzle_epi64(thp15, mask1, thp13, _MM_SWIZ_REG_BADC); 
    mask1 = 0xf0f0;
    t0  = _mm512_mask_permute4f128_epi32(v0 , mask1, v4 , _MM_PERM_CDAB); 
    t1  = _mm512_mask_permute4f128_epi32(v1 , mask1, v5 , _MM_PERM_CDAB); 
    t2  = _mm512_mask_permute4f128_epi32(v2 , mask1, v6 , _MM_PERM_CDAB); 
    t3  = _mm512_mask_permute4f128_epi32(v3 , mask1, v7 , _MM_PERM_CDAB); 
    t8  = _mm512_mask_permute4f128_epi32(v8 , mask1, v12, _MM_PERM_CDAB); 
    t9  = _mm512_mask_permute4f128_epi32(v9 , mask1, v13, _MM_PERM_CDAB); 
    t10 = _mm512_mask_permute4f128_epi32(v10, mask1, v14, _MM_PERM_CDAB); 
    t11 = _mm512_mask_permute4f128_epi32(v11, mask1, v15, _MM_PERM_CDAB); 

    tlp0  = _mm512_mask_permute4f128_epi32(vlp0 , 0xff00, vlp4 , _MM_PERM_BADC); 
    tlp1  = _mm512_mask_permute4f128_epi32(vlp1 , 0xff00, vlp5 , _MM_PERM_BADC); 
    tlp2  = _mm512_mask_permute4f128_epi32(vlp2 , 0xff00, vlp6 , _MM_PERM_BADC); 
    tlp3  = _mm512_mask_permute4f128_epi32(vlp3 , 0xff00, vlp7 , _MM_PERM_BADC); 
    tlp8  = _mm512_mask_permute4f128_epi32(vlp8 , 0xff00, vlp12, _MM_PERM_BADC); 
    tlp9  = _mm512_mask_permute4f128_epi32(vlp9 , 0xff00, vlp13, _MM_PERM_BADC); 
    tlp10 = _mm512_mask_permute4f128_epi32(vlp10, 0xff00, vlp14, _MM_PERM_BADC); 
    tlp11 = _mm512_mask_permute4f128_epi32(vlp11, 0xff00, vlp15, _MM_PERM_BADC); 

    thp0  = _mm512_mask_permute4f128_epi32(vhp0 , 0xff00, vhp4 , _MM_PERM_BADC); 
    thp1  = _mm512_mask_permute4f128_epi32(vhp1 , 0xff00, vhp5 , _MM_PERM_BADC); 
    thp2  = _mm512_mask_permute4f128_epi32(vhp2 , 0xff00, vhp6 , _MM_PERM_BADC); 
    thp3  = _mm512_mask_permute4f128_epi32(vhp3 , 0xff00, vhp7 , _MM_PERM_BADC); 
    thp8  = _mm512_mask_permute4f128_epi32(vhp8 , 0xff00, vhp12, _MM_PERM_BADC); 
    thp9  = _mm512_mask_permute4f128_epi32(vhp9 , 0xff00, vhp13, _MM_PERM_BADC); 
    thp10 = _mm512_mask_permute4f128_epi32(vhp10, 0xff00, vhp14, _MM_PERM_BADC); 
    thp11 = _mm512_mask_permute4f128_epi32(vhp11, 0xff00, vhp15, _MM_PERM_BADC); 
    mask1 = 0x0f0f;
    t4  = _mm512_mask_permute4f128_epi32(v4 , mask1, v0 , _MM_PERM_CDAB); 
    t5  = _mm512_mask_permute4f128_epi32(v5 , mask1, v1 , _MM_PERM_CDAB); 
    t6  = _mm512_mask_permute4f128_epi32(v6 , mask1, v2 , _MM_PERM_CDAB); 
    t7  = _mm512_mask_permute4f128_epi32(v7 , mask1, v3 , _MM_PERM_CDAB); 
    t12 = _mm512_mask_permute4f128_epi32(v12, mask1, v8 , _MM_PERM_CDAB); 
    t13 = _mm512_mask_permute4f128_epi32(v13, mask1, v9 , _MM_PERM_CDAB); 
    t14 = _mm512_mask_permute4f128_epi32(v14, mask1, v10, _MM_PERM_CDAB); 
    t15 = _mm512_mask_permute4f128_epi32(v15, mask1, v11, _MM_PERM_CDAB); 

    tlp4  = _mm512_mask_permute4f128_epi32(vlp4 , 0x00ff, vlp0 , _MM_PERM_BADC); 
    tlp5  = _mm512_mask_permute4f128_epi32(vlp5 , 0x00ff, vlp1 , _MM_PERM_BADC); 
    tlp6  = _mm512_mask_permute4f128_epi32(vlp6 , 0x00ff, vlp2 , _MM_PERM_BADC); 
    tlp7  = _mm512_mask_permute4f128_epi32(vlp7 , 0x00ff, vlp3 , _MM_PERM_BADC); 
    tlp12 = _mm512_mask_permute4f128_epi32(vlp12, 0x00ff, vlp8 , _MM_PERM_BADC); 
    tlp13 = _mm512_mask_permute4f128_epi32(vlp13, 0x00ff, vlp9 , _MM_PERM_BADC); 
    tlp14 = _mm512_mask_permute4f128_epi32(vlp14, 0x00ff, vlp10, _MM_PERM_BADC); 
    tlp15 = _mm512_mask_permute4f128_epi32(vlp15, 0x00ff, vlp11, _MM_PERM_BADC); 

    thp4  = _mm512_mask_permute4f128_epi32(vhp4 , 0x00ff, vhp0 , _MM_PERM_BADC); 
    thp5  = _mm512_mask_permute4f128_epi32(vhp5 , 0x00ff, vhp1 , _MM_PERM_BADC); 
    thp6  = _mm512_mask_permute4f128_epi32(vhp6 , 0x00ff, vhp2 , _MM_PERM_BADC); 
    thp7  = _mm512_mask_permute4f128_epi32(vhp7 , 0x00ff, vhp3 , _MM_PERM_BADC); 
    thp12 = _mm512_mask_permute4f128_epi32(vhp12, 0x00ff, vhp8 , _MM_PERM_BADC); 
    thp13 = _mm512_mask_permute4f128_epi32(vhp13, 0x00ff, vhp9 , _MM_PERM_BADC); 
    thp14 = _mm512_mask_permute4f128_epi32(vhp14, 0x00ff, vhp10, _MM_PERM_BADC); 
    thp15 = _mm512_mask_permute4f128_epi32(vhp15, 0x00ff, vhp11, _MM_PERM_BADC); 
    mask1 = 0xff00;
    v0 = _mm512_mask_permute4f128_epi32(t0, mask1, t8 , _MM_PERM_BADC); 
    v2 = _mm512_mask_permute4f128_epi32(t1, mask1, t9 , _MM_PERM_BADC); 
    v1 = _mm512_mask_permute4f128_epi32(t2, mask1, t10, _MM_PERM_BADC); 
    v3 = _mm512_mask_permute4f128_epi32(t3, mask1, t11, _MM_PERM_BADC); 
    v4 = _mm512_mask_permute4f128_epi32(t4, mask1, t12, _MM_PERM_BADC); 
    v6 = _mm512_mask_permute4f128_epi32(t5, mask1, t13, _MM_PERM_BADC); 
    v5 = _mm512_mask_permute4f128_epi32(t6, mask1, t14, _MM_PERM_BADC); 
    v7 = _mm512_mask_permute4f128_epi32(t7, mask1, t15, _MM_PERM_BADC); 

    vlp0 = tlp0; 
    vlp2 = tlp1; 
    vlp1 = tlp2; 
    vlp3 = tlp3; 
    vlp4 = tlp4; 
    vlp6 = tlp5; 
    vlp5 = tlp6; 
    vlp7 = tlp7; 

    vhp0 = tlp8 ; 
    vhp2 = tlp9 ; 
    vhp1 = tlp10; 
    vhp3 = tlp11; 
    vhp4 = tlp12; 
    vhp6 = tlp13; 
    vhp5 = tlp14; 
    vhp7 = tlp15; 
    mask1 = 0x00ff;
    v8  = _mm512_mask_permute4f128_epi32(t8 , mask1, t0, _MM_PERM_BADC); 
    v10 = _mm512_mask_permute4f128_epi32(t9 , mask1, t1, _MM_PERM_BADC); 
    v9  = _mm512_mask_permute4f128_epi32(t10, mask1, t2, _MM_PERM_BADC); 
    v11 = _mm512_mask_permute4f128_epi32(t11, mask1, t3, _MM_PERM_BADC); 
    v12 = _mm512_mask_permute4f128_epi32(t12, mask1, t4, _MM_PERM_BADC); 
    v14 = _mm512_mask_permute4f128_epi32(t13, mask1, t5, _MM_PERM_BADC); 
    v13 = _mm512_mask_permute4f128_epi32(t14, mask1, t6, _MM_PERM_BADC); 
    v15 = _mm512_mask_permute4f128_epi32(t15, mask1, t7, _MM_PERM_BADC);

    vlp8  = thp0; 
    vlp10 = thp1; 
    vlp9  = thp2; 
    vlp11 = thp3; 
    vlp12 = thp4; 
    vlp14 = thp5; 
    vlp13 = thp6; 
    vlp15 = thp7;

    vhp8  = thp8 ; 
    vhp10 = thp9 ; 
    vhp9  = thp10; 
    vhp11 = thp11; 
    vhp12 = thp12; 
    vhp14 = thp13; 
    vhp13 = thp14; 
    vhp15 = thp15;
}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_transpose_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3 , 
        __m512i &vp4 , __m512i &vp5 , __m512i &vp6 , __m512i &vp7 , 
        __m512i &vp8 , __m512i &vp9 , __m512i &vp10, __m512i &vp11, 
        __m512i &vp12, __m512i &vp13, __m512i &vp14, __m512i &vp15)

{
    __m512 t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
    __m512i tp0, tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8, tp9, tp10, tp11, tp12, tp13, tp14, tp15;
    __mmask16 mask1;
    mask1 = 0xaaaa;
    t0  = _mm512_mask_swizzle_ps(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_ps(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_ps(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_ps(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 
    t8  = _mm512_mask_swizzle_ps(v8 , mask1, v9 , _MM_SWIZ_REG_CDAB); 
    t10 = _mm512_mask_swizzle_ps(v10, mask1, v11, _MM_SWIZ_REG_CDAB); 
    t12 = _mm512_mask_swizzle_ps(v12, mask1, v13, _MM_SWIZ_REG_CDAB); 
    t14 = _mm512_mask_swizzle_ps(v14, mask1, v15, _MM_SWIZ_REG_CDAB); 

    tp0  = _mm512_mask_swizzle_epi32(vp0 , mask1, vp1 , _MM_SWIZ_REG_CDAB); 
    tp2  = _mm512_mask_swizzle_epi32(vp2 , mask1, vp3 , _MM_SWIZ_REG_CDAB); 
    tp4  = _mm512_mask_swizzle_epi32(vp4 , mask1, vp5 , _MM_SWIZ_REG_CDAB); 
    tp6  = _mm512_mask_swizzle_epi32(vp6 , mask1, vp7 , _MM_SWIZ_REG_CDAB); 
    tp8  = _mm512_mask_swizzle_epi32(vp8 , mask1, vp9 , _MM_SWIZ_REG_CDAB); 
    tp10 = _mm512_mask_swizzle_epi32(vp10, mask1, vp11, _MM_SWIZ_REG_CDAB); 
    tp12 = _mm512_mask_swizzle_epi32(vp12, mask1, vp13, _MM_SWIZ_REG_CDAB); 
    tp14 = _mm512_mask_swizzle_epi32(vp14, mask1, vp15, _MM_SWIZ_REG_CDAB); 
    mask1 = 0x5555;
    t1  = _mm512_mask_swizzle_ps(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_ps(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_ps(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_ps(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 
    t9  = _mm512_mask_swizzle_ps(v9 , mask1, v8 , _MM_SWIZ_REG_CDAB); 
    t11 = _mm512_mask_swizzle_ps(v11, mask1, v10, _MM_SWIZ_REG_CDAB); 
    t13 = _mm512_mask_swizzle_ps(v13, mask1, v12, _MM_SWIZ_REG_CDAB); 
    t15 = _mm512_mask_swizzle_ps(v15, mask1, v14, _MM_SWIZ_REG_CDAB); 

    tp1  = _mm512_mask_swizzle_epi32(vp1 , mask1, vp0 , _MM_SWIZ_REG_CDAB); 
    tp3  = _mm512_mask_swizzle_epi32(vp3 , mask1, vp2 , _MM_SWIZ_REG_CDAB); 
    tp5  = _mm512_mask_swizzle_epi32(vp5 , mask1, vp4 , _MM_SWIZ_REG_CDAB); 
    tp7  = _mm512_mask_swizzle_epi32(vp7 , mask1, vp6 , _MM_SWIZ_REG_CDAB); 
    tp9  = _mm512_mask_swizzle_epi32(vp9 , mask1, vp8 , _MM_SWIZ_REG_CDAB); 
    tp11 = _mm512_mask_swizzle_epi32(vp11, mask1, vp10, _MM_SWIZ_REG_CDAB); 
    tp13 = _mm512_mask_swizzle_epi32(vp13, mask1, vp12, _MM_SWIZ_REG_CDAB); 
    tp15 = _mm512_mask_swizzle_epi32(vp15, mask1, vp14, _MM_SWIZ_REG_CDAB); 
    mask1 = 0xcccc;
    v0  = _mm512_mask_swizzle_ps(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_ps(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_ps(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_ps(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 
    v8  = _mm512_mask_swizzle_ps(t8 , mask1, t10, _MM_SWIZ_REG_BADC); 
    v10 = _mm512_mask_swizzle_ps(t9 , mask1, t11, _MM_SWIZ_REG_BADC); 
    v12 = _mm512_mask_swizzle_ps(t12, mask1, t14, _MM_SWIZ_REG_BADC); 
    v14 = _mm512_mask_swizzle_ps(t13, mask1, t15, _MM_SWIZ_REG_BADC); 

    vp0  = _mm512_mask_swizzle_epi32(tp0 , mask1, tp2 , _MM_SWIZ_REG_BADC); 
    vp2  = _mm512_mask_swizzle_epi32(tp1 , mask1, tp3 , _MM_SWIZ_REG_BADC); 
    vp4  = _mm512_mask_swizzle_epi32(tp4 , mask1, tp6 , _MM_SWIZ_REG_BADC); 
    vp6  = _mm512_mask_swizzle_epi32(tp5 , mask1, tp7 , _MM_SWIZ_REG_BADC); 
    vp8  = _mm512_mask_swizzle_epi32(tp8 , mask1, tp10, _MM_SWIZ_REG_BADC); 
    vp10 = _mm512_mask_swizzle_epi32(tp9 , mask1, tp11, _MM_SWIZ_REG_BADC); 
    vp12 = _mm512_mask_swizzle_epi32(tp12, mask1, tp14, _MM_SWIZ_REG_BADC); 
    vp14 = _mm512_mask_swizzle_epi32(tp13, mask1, tp15, _MM_SWIZ_REG_BADC); 
    mask1 = 0x3333;
    v1  = _mm512_mask_swizzle_ps(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_ps(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_ps(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_ps(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 
    v9  = _mm512_mask_swizzle_ps(t10, mask1, t8 , _MM_SWIZ_REG_BADC); 
    v11 = _mm512_mask_swizzle_ps(t11, mask1, t9 , _MM_SWIZ_REG_BADC); 
    v13 = _mm512_mask_swizzle_ps(t14, mask1, t12, _MM_SWIZ_REG_BADC); 
    v15 = _mm512_mask_swizzle_ps(t15, mask1, t13, _MM_SWIZ_REG_BADC); 

    vp1  = _mm512_mask_swizzle_epi32(tp2 , mask1, tp0 , _MM_SWIZ_REG_BADC); 
    vp3  = _mm512_mask_swizzle_epi32(tp3 , mask1, tp1 , _MM_SWIZ_REG_BADC); 
    vp5  = _mm512_mask_swizzle_epi32(tp6 , mask1, tp4 , _MM_SWIZ_REG_BADC); 
    vp7  = _mm512_mask_swizzle_epi32(tp7 , mask1, tp5 , _MM_SWIZ_REG_BADC); 
    vp9  = _mm512_mask_swizzle_epi32(tp10, mask1, tp8 , _MM_SWIZ_REG_BADC); 
    vp11 = _mm512_mask_swizzle_epi32(tp11, mask1, tp9 , _MM_SWIZ_REG_BADC); 
    vp13 = _mm512_mask_swizzle_epi32(tp14, mask1, tp12, _MM_SWIZ_REG_BADC); 
    vp15 = _mm512_mask_swizzle_epi32(tp15, mask1, tp13, _MM_SWIZ_REG_BADC); 
    mask1 = 0xf0f0;
    t0  = _mm512_mask_permute4f128_ps(v0 , mask1, v4 , _MM_PERM_CDAB); 
    t1  = _mm512_mask_permute4f128_ps(v1 , mask1, v5 , _MM_PERM_CDAB); 
    t2  = _mm512_mask_permute4f128_ps(v2 , mask1, v6 , _MM_PERM_CDAB); 
    t3  = _mm512_mask_permute4f128_ps(v3 , mask1, v7 , _MM_PERM_CDAB); 
    t8  = _mm512_mask_permute4f128_ps(v8 , mask1, v12, _MM_PERM_CDAB); 
    t9  = _mm512_mask_permute4f128_ps(v9 , mask1, v13, _MM_PERM_CDAB); 
    t10 = _mm512_mask_permute4f128_ps(v10, mask1, v14, _MM_PERM_CDAB); 
    t11 = _mm512_mask_permute4f128_ps(v11, mask1, v15, _MM_PERM_CDAB); 

    tp0  = _mm512_mask_permute4f128_epi32(vp0 , mask1, vp4 , _MM_PERM_CDAB); 
    tp1  = _mm512_mask_permute4f128_epi32(vp1 , mask1, vp5 , _MM_PERM_CDAB); 
    tp2  = _mm512_mask_permute4f128_epi32(vp2 , mask1, vp6 , _MM_PERM_CDAB); 
    tp3  = _mm512_mask_permute4f128_epi32(vp3 , mask1, vp7 , _MM_PERM_CDAB); 
    tp8  = _mm512_mask_permute4f128_epi32(vp8 , mask1, vp12, _MM_PERM_CDAB); 
    tp9  = _mm512_mask_permute4f128_epi32(vp9 , mask1, vp13, _MM_PERM_CDAB); 
    tp10 = _mm512_mask_permute4f128_epi32(vp10, mask1, vp14, _MM_PERM_CDAB); 
    tp11 = _mm512_mask_permute4f128_epi32(vp11, mask1, vp15, _MM_PERM_CDAB); 
    mask1 = 0x0f0f;
    t4  = _mm512_mask_permute4f128_ps(v4 , mask1, v0 , _MM_PERM_CDAB); 
    t5  = _mm512_mask_permute4f128_ps(v5 , mask1, v1 , _MM_PERM_CDAB); 
    t6  = _mm512_mask_permute4f128_ps(v6 , mask1, v2 , _MM_PERM_CDAB); 
    t7  = _mm512_mask_permute4f128_ps(v7 , mask1, v3 , _MM_PERM_CDAB); 
    t12 = _mm512_mask_permute4f128_ps(v12, mask1, v8 , _MM_PERM_CDAB); 
    t13 = _mm512_mask_permute4f128_ps(v13, mask1, v9 , _MM_PERM_CDAB); 
    t14 = _mm512_mask_permute4f128_ps(v14, mask1, v10, _MM_PERM_CDAB); 
    t15 = _mm512_mask_permute4f128_ps(v15, mask1, v11, _MM_PERM_CDAB); 

    tp4  = _mm512_mask_permute4f128_epi32(vp4 , mask1, vp0 , _MM_PERM_CDAB); 
    tp5  = _mm512_mask_permute4f128_epi32(vp5 , mask1, vp1 , _MM_PERM_CDAB); 
    tp6  = _mm512_mask_permute4f128_epi32(vp6 , mask1, vp2 , _MM_PERM_CDAB); 
    tp7  = _mm512_mask_permute4f128_epi32(vp7 , mask1, vp3 , _MM_PERM_CDAB); 
    tp12 = _mm512_mask_permute4f128_epi32(vp12, mask1, vp8 , _MM_PERM_CDAB); 
    tp13 = _mm512_mask_permute4f128_epi32(vp13, mask1, vp9 , _MM_PERM_CDAB); 
    tp14 = _mm512_mask_permute4f128_epi32(vp14, mask1, vp10, _MM_PERM_CDAB); 
    tp15 = _mm512_mask_permute4f128_epi32(vp15, mask1, vp11, _MM_PERM_CDAB); 
    mask1 = 0xff00;
    v0 = _mm512_mask_permute4f128_ps(t0, mask1, t8 , _MM_PERM_BADC); 
    v2 = _mm512_mask_permute4f128_ps(t1, mask1, t9 , _MM_PERM_BADC); 
    v1 = _mm512_mask_permute4f128_ps(t2, mask1, t10, _MM_PERM_BADC); 
    v3 = _mm512_mask_permute4f128_ps(t3, mask1, t11, _MM_PERM_BADC); 
    v4 = _mm512_mask_permute4f128_ps(t4, mask1, t12, _MM_PERM_BADC); 
    v6 = _mm512_mask_permute4f128_ps(t5, mask1, t13, _MM_PERM_BADC); 
    v5 = _mm512_mask_permute4f128_ps(t6, mask1, t14, _MM_PERM_BADC); 
    v7 = _mm512_mask_permute4f128_ps(t7, mask1, t15, _MM_PERM_BADC); 

    vp0 = _mm512_mask_permute4f128_epi32(tp0, mask1, tp8 , _MM_PERM_BADC); 
    vp2 = _mm512_mask_permute4f128_epi32(tp1, mask1, tp9 , _MM_PERM_BADC); 
    vp1 = _mm512_mask_permute4f128_epi32(tp2, mask1, tp10, _MM_PERM_BADC); 
    vp3 = _mm512_mask_permute4f128_epi32(tp3, mask1, tp11, _MM_PERM_BADC); 
    vp4 = _mm512_mask_permute4f128_epi32(tp4, mask1, tp12, _MM_PERM_BADC); 
    vp6 = _mm512_mask_permute4f128_epi32(tp5, mask1, tp13, _MM_PERM_BADC); 
    vp5 = _mm512_mask_permute4f128_epi32(tp6, mask1, tp14, _MM_PERM_BADC); 
    vp7 = _mm512_mask_permute4f128_epi32(tp7, mask1, tp15, _MM_PERM_BADC); 
    mask1 = 0x00ff;
    v8  = _mm512_mask_permute4f128_ps(t8 , mask1, t0, _MM_PERM_BADC); 
    v10 = _mm512_mask_permute4f128_ps(t9 , mask1, t1, _MM_PERM_BADC); 
    v9  = _mm512_mask_permute4f128_ps(t10, mask1, t2, _MM_PERM_BADC); 
    v11 = _mm512_mask_permute4f128_ps(t11, mask1, t3, _MM_PERM_BADC); 
    v12 = _mm512_mask_permute4f128_ps(t12, mask1, t4, _MM_PERM_BADC); 
    v14 = _mm512_mask_permute4f128_ps(t13, mask1, t5, _MM_PERM_BADC); 
    v13 = _mm512_mask_permute4f128_ps(t14, mask1, t6, _MM_PERM_BADC); 
    v15 = _mm512_mask_permute4f128_ps(t15, mask1, t7, _MM_PERM_BADC);

    vp8  = _mm512_mask_permute4f128_epi32(tp8 , mask1, tp0, _MM_PERM_BADC); 
    vp10 = _mm512_mask_permute4f128_epi32(tp9 , mask1, tp1, _MM_PERM_BADC); 
    vp9  = _mm512_mask_permute4f128_epi32(tp10, mask1, tp2, _MM_PERM_BADC); 
    vp11 = _mm512_mask_permute4f128_epi32(tp11, mask1, tp3, _MM_PERM_BADC); 
    vp12 = _mm512_mask_permute4f128_epi32(tp12, mask1, tp4, _MM_PERM_BADC); 
    vp14 = _mm512_mask_permute4f128_epi32(tp13, mask1, tp5, _MM_PERM_BADC); 
    vp13 = _mm512_mask_permute4f128_epi32(tp14, mask1, tp6, _MM_PERM_BADC); 
    vp15 = _mm512_mask_permute4f128_epi32(tp15, mask1, tp7, _MM_PERM_BADC);
}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_transpose_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15,
        __m512i &vlp0 , __m512i &vlp1 , __m512i &vlp2 , __m512i &vlp3 , 
        __m512i &vlp4 , __m512i &vlp5 , __m512i &vlp6 , __m512i &vlp7 , 
        __m512i &vlp8 , __m512i &vlp9 , __m512i &vlp10, __m512i &vlp11, 
        __m512i &vlp12, __m512i &vlp13, __m512i &vlp14, __m512i &vlp15,
        __m512i &vhp0 , __m512i &vhp1 , __m512i &vhp2 , __m512i &vhp3 , 
        __m512i &vhp4 , __m512i &vhp5 , __m512i &vhp6 , __m512i &vhp7 , 
        __m512i &vhp8 , __m512i &vhp9 , __m512i &vhp10, __m512i &vhp11, 
        __m512i &vhp12, __m512i &vhp13, __m512i &vhp14, __m512i &vhp15)
{
    __m512 t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
    __m512i tlp0, tlp1, tlp2, tlp3, tlp4, tlp5, tlp6, tlp7, tlp8, tlp9, tlp10, tlp11, tlp12, tlp13, tlp14, tlp15;
    __m512i thp0, thp1, thp2, thp3, thp4, thp5, thp6, thp7, thp8, thp9, thp10, thp11, thp12, thp13, thp14, thp15;
    __mmask16 mask1;
    mask1 = 0xaaaa;
    t0  = _mm512_mask_swizzle_ps(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_ps(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_ps(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_ps(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 
    t8  = _mm512_mask_swizzle_ps(v8 , mask1, v9 , _MM_SWIZ_REG_CDAB); 
    t10 = _mm512_mask_swizzle_ps(v10, mask1, v11, _MM_SWIZ_REG_CDAB); 
    t12 = _mm512_mask_swizzle_ps(v12, mask1, v13, _MM_SWIZ_REG_CDAB); 
    t14 = _mm512_mask_swizzle_ps(v14, mask1, v15, _MM_SWIZ_REG_CDAB); 

    tlp0  = _mm512_mask_swizzle_epi64(vlp0 , mask1, vlp1 , _MM_SWIZ_REG_CDAB); 
    tlp2  = _mm512_mask_swizzle_epi64(vlp2 , mask1, vlp3 , _MM_SWIZ_REG_CDAB); 
    tlp4  = _mm512_mask_swizzle_epi64(vlp4 , mask1, vlp5 , _MM_SWIZ_REG_CDAB); 
    tlp6  = _mm512_mask_swizzle_epi64(vlp6 , mask1, vlp7 , _MM_SWIZ_REG_CDAB); 
    tlp8  = _mm512_mask_swizzle_epi64(vlp8 , mask1, vlp9 , _MM_SWIZ_REG_CDAB); 
    tlp10 = _mm512_mask_swizzle_epi64(vlp10, mask1, vlp11, _MM_SWIZ_REG_CDAB); 
    tlp12 = _mm512_mask_swizzle_epi64(vlp12, mask1, vlp13, _MM_SWIZ_REG_CDAB); 
    tlp14 = _mm512_mask_swizzle_epi64(vlp14, mask1, vlp15, _MM_SWIZ_REG_CDAB); 

    thp0  = _mm512_mask_swizzle_epi64(vhp0 , mask1, vhp1 , _MM_SWIZ_REG_CDAB); 
    thp2  = _mm512_mask_swizzle_epi64(vhp2 , mask1, vhp3 , _MM_SWIZ_REG_CDAB); 
    thp4  = _mm512_mask_swizzle_epi64(vhp4 , mask1, vhp5 , _MM_SWIZ_REG_CDAB); 
    thp6  = _mm512_mask_swizzle_epi64(vhp6 , mask1, vhp7 , _MM_SWIZ_REG_CDAB); 
    thp8  = _mm512_mask_swizzle_epi64(vhp8 , mask1, vhp9 , _MM_SWIZ_REG_CDAB); 
    thp10 = _mm512_mask_swizzle_epi64(vhp10, mask1, vhp11, _MM_SWIZ_REG_CDAB); 
    thp12 = _mm512_mask_swizzle_epi64(vhp12, mask1, vhp13, _MM_SWIZ_REG_CDAB); 
    thp14 = _mm512_mask_swizzle_epi64(vhp14, mask1, vhp15, _MM_SWIZ_REG_CDAB); 
    mask1 = 0x5555;
    t1  = _mm512_mask_swizzle_ps(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_ps(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_ps(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_ps(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 
    t9  = _mm512_mask_swizzle_ps(v9 , mask1, v8 , _MM_SWIZ_REG_CDAB); 
    t11 = _mm512_mask_swizzle_ps(v11, mask1, v10, _MM_SWIZ_REG_CDAB); 
    t13 = _mm512_mask_swizzle_ps(v13, mask1, v12, _MM_SWIZ_REG_CDAB); 
    t15 = _mm512_mask_swizzle_ps(v15, mask1, v14, _MM_SWIZ_REG_CDAB); 

    tlp1  = _mm512_mask_swizzle_epi64(vlp1 , mask1, vlp0 , _MM_SWIZ_REG_CDAB); 
    tlp3  = _mm512_mask_swizzle_epi64(vlp3 , mask1, vlp2 , _MM_SWIZ_REG_CDAB); 
    tlp5  = _mm512_mask_swizzle_epi64(vlp5 , mask1, vlp4 , _MM_SWIZ_REG_CDAB); 
    tlp7  = _mm512_mask_swizzle_epi64(vlp7 , mask1, vlp6 , _MM_SWIZ_REG_CDAB); 
    tlp9  = _mm512_mask_swizzle_epi64(vlp9 , mask1, vlp8 , _MM_SWIZ_REG_CDAB); 
    tlp11 = _mm512_mask_swizzle_epi64(vlp11, mask1, vlp10, _MM_SWIZ_REG_CDAB); 
    tlp13 = _mm512_mask_swizzle_epi64(vlp13, mask1, vlp12, _MM_SWIZ_REG_CDAB); 
    tlp15 = _mm512_mask_swizzle_epi64(vlp15, mask1, vlp14, _MM_SWIZ_REG_CDAB); 
    
    thp1  = _mm512_mask_swizzle_epi64(vhp1 , mask1, vhp0 , _MM_SWIZ_REG_CDAB); 
    thp3  = _mm512_mask_swizzle_epi64(vhp3 , mask1, vhp2 , _MM_SWIZ_REG_CDAB); 
    thp5  = _mm512_mask_swizzle_epi64(vhp5 , mask1, vhp4 , _MM_SWIZ_REG_CDAB); 
    thp7  = _mm512_mask_swizzle_epi64(vhp7 , mask1, vhp6 , _MM_SWIZ_REG_CDAB); 
    thp9  = _mm512_mask_swizzle_epi64(vhp9 , mask1, vhp8 , _MM_SWIZ_REG_CDAB); 
    thp11 = _mm512_mask_swizzle_epi64(vhp11, mask1, vhp10, _MM_SWIZ_REG_CDAB); 
    thp13 = _mm512_mask_swizzle_epi64(vhp13, mask1, vhp12, _MM_SWIZ_REG_CDAB); 
    thp15 = _mm512_mask_swizzle_epi64(vhp15, mask1, vhp14, _MM_SWIZ_REG_CDAB); 
    mask1 = 0xcccc;
    v0  = _mm512_mask_swizzle_ps(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_ps(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_ps(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_ps(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 
    v8  = _mm512_mask_swizzle_ps(t8 , mask1, t10, _MM_SWIZ_REG_BADC); 
    v10 = _mm512_mask_swizzle_ps(t9 , mask1, t11, _MM_SWIZ_REG_BADC); 
    v12 = _mm512_mask_swizzle_ps(t12, mask1, t14, _MM_SWIZ_REG_BADC); 
    v14 = _mm512_mask_swizzle_ps(t13, mask1, t15, _MM_SWIZ_REG_BADC); 

    vlp0  = _mm512_mask_swizzle_epi64(tlp0 , mask1, tlp2 , _MM_SWIZ_REG_BADC); 
    vlp2  = _mm512_mask_swizzle_epi64(tlp1 , mask1, tlp3 , _MM_SWIZ_REG_BADC); 
    vlp4  = _mm512_mask_swizzle_epi64(tlp4 , mask1, tlp6 , _MM_SWIZ_REG_BADC); 
    vlp6  = _mm512_mask_swizzle_epi64(tlp5 , mask1, tlp7 , _MM_SWIZ_REG_BADC); 
    vlp8  = _mm512_mask_swizzle_epi64(tlp8 , mask1, tlp10, _MM_SWIZ_REG_BADC); 
    vlp10 = _mm512_mask_swizzle_epi64(tlp9 , mask1, tlp11, _MM_SWIZ_REG_BADC); 
    vlp12 = _mm512_mask_swizzle_epi64(tlp12, mask1, tlp14, _MM_SWIZ_REG_BADC); 
    vlp14 = _mm512_mask_swizzle_epi64(tlp13, mask1, tlp15, _MM_SWIZ_REG_BADC); 

    vhp0  = _mm512_mask_swizzle_epi64(thp0 , mask1, thp2 , _MM_SWIZ_REG_BADC); 
    vhp2  = _mm512_mask_swizzle_epi64(thp1 , mask1, thp3 , _MM_SWIZ_REG_BADC); 
    vhp4  = _mm512_mask_swizzle_epi64(thp4 , mask1, thp6 , _MM_SWIZ_REG_BADC); 
    vhp6  = _mm512_mask_swizzle_epi64(thp5 , mask1, thp7 , _MM_SWIZ_REG_BADC); 
    vhp8  = _mm512_mask_swizzle_epi64(thp8 , mask1, thp10, _MM_SWIZ_REG_BADC); 
    vhp10 = _mm512_mask_swizzle_epi64(thp9 , mask1, thp11, _MM_SWIZ_REG_BADC); 
    vhp12 = _mm512_mask_swizzle_epi64(thp12, mask1, thp14, _MM_SWIZ_REG_BADC); 
    vhp14 = _mm512_mask_swizzle_epi64(thp13, mask1, thp15, _MM_SWIZ_REG_BADC); 
    mask1 = 0x3333;
    v1  = _mm512_mask_swizzle_ps(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_ps(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_ps(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_ps(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 
    v9  = _mm512_mask_swizzle_ps(t10, mask1, t8 , _MM_SWIZ_REG_BADC); 
    v11 = _mm512_mask_swizzle_ps(t11, mask1, t9 , _MM_SWIZ_REG_BADC); 
    v13 = _mm512_mask_swizzle_ps(t14, mask1, t12, _MM_SWIZ_REG_BADC); 
    v15 = _mm512_mask_swizzle_ps(t15, mask1, t13, _MM_SWIZ_REG_BADC); 

    vlp1  = _mm512_mask_swizzle_epi64(tlp2 , mask1, tlp0 , _MM_SWIZ_REG_BADC); 
    vlp3  = _mm512_mask_swizzle_epi64(tlp3 , mask1, tlp1 , _MM_SWIZ_REG_BADC); 
    vlp5  = _mm512_mask_swizzle_epi64(tlp6 , mask1, tlp4 , _MM_SWIZ_REG_BADC); 
    vlp7  = _mm512_mask_swizzle_epi64(tlp7 , mask1, tlp5 , _MM_SWIZ_REG_BADC); 
    vlp9  = _mm512_mask_swizzle_epi64(tlp10, mask1, tlp8 , _MM_SWIZ_REG_BADC); 
    vlp11 = _mm512_mask_swizzle_epi64(tlp11, mask1, tlp9 , _MM_SWIZ_REG_BADC); 
    vlp13 = _mm512_mask_swizzle_epi64(tlp14, mask1, tlp12, _MM_SWIZ_REG_BADC); 
    vlp15 = _mm512_mask_swizzle_epi64(tlp15, mask1, tlp13, _MM_SWIZ_REG_BADC); 

    vhp1  = _mm512_mask_swizzle_epi64(thp2 , mask1, thp0 , _MM_SWIZ_REG_BADC); 
    vhp3  = _mm512_mask_swizzle_epi64(thp3 , mask1, thp1 , _MM_SWIZ_REG_BADC); 
    vhp5  = _mm512_mask_swizzle_epi64(thp6 , mask1, thp4 , _MM_SWIZ_REG_BADC); 
    vhp7  = _mm512_mask_swizzle_epi64(thp7 , mask1, thp5 , _MM_SWIZ_REG_BADC); 
    vhp9  = _mm512_mask_swizzle_epi64(thp10, mask1, thp8 , _MM_SWIZ_REG_BADC); 
    vhp11 = _mm512_mask_swizzle_epi64(thp11, mask1, thp9 , _MM_SWIZ_REG_BADC); 
    vhp13 = _mm512_mask_swizzle_epi64(thp14, mask1, thp12, _MM_SWIZ_REG_BADC); 
    vhp15 = _mm512_mask_swizzle_epi64(thp15, mask1, thp13, _MM_SWIZ_REG_BADC); 
    mask1 = 0xf0f0;
    t0  = _mm512_mask_permute4f128_ps(v0 , mask1, v4 , _MM_PERM_CDAB); 
    t1  = _mm512_mask_permute4f128_ps(v1 , mask1, v5 , _MM_PERM_CDAB); 
    t2  = _mm512_mask_permute4f128_ps(v2 , mask1, v6 , _MM_PERM_CDAB); 
    t3  = _mm512_mask_permute4f128_ps(v3 , mask1, v7 , _MM_PERM_CDAB); 
    t8  = _mm512_mask_permute4f128_ps(v8 , mask1, v12, _MM_PERM_CDAB); 
    t9  = _mm512_mask_permute4f128_ps(v9 , mask1, v13, _MM_PERM_CDAB); 
    t10 = _mm512_mask_permute4f128_ps(v10, mask1, v14, _MM_PERM_CDAB); 
    t11 = _mm512_mask_permute4f128_ps(v11, mask1, v15, _MM_PERM_CDAB); 

    tlp0  = _mm512_mask_permute4f128_epi32(vlp0 , 0xff00, vlp4 , _MM_PERM_BADC); 
    tlp1  = _mm512_mask_permute4f128_epi32(vlp1 , 0xff00, vlp5 , _MM_PERM_BADC); 
    tlp2  = _mm512_mask_permute4f128_epi32(vlp2 , 0xff00, vlp6 , _MM_PERM_BADC); 
    tlp3  = _mm512_mask_permute4f128_epi32(vlp3 , 0xff00, vlp7 , _MM_PERM_BADC); 
    tlp8  = _mm512_mask_permute4f128_epi32(vlp8 , 0xff00, vlp12, _MM_PERM_BADC); 
    tlp9  = _mm512_mask_permute4f128_epi32(vlp9 , 0xff00, vlp13, _MM_PERM_BADC); 
    tlp10 = _mm512_mask_permute4f128_epi32(vlp10, 0xff00, vlp14, _MM_PERM_BADC); 
    tlp11 = _mm512_mask_permute4f128_epi32(vlp11, 0xff00, vlp15, _MM_PERM_BADC); 

    thp0  = _mm512_mask_permute4f128_epi32(vhp0 , 0xff00, vhp4 , _MM_PERM_BADC); 
    thp1  = _mm512_mask_permute4f128_epi32(vhp1 , 0xff00, vhp5 , _MM_PERM_BADC); 
    thp2  = _mm512_mask_permute4f128_epi32(vhp2 , 0xff00, vhp6 , _MM_PERM_BADC); 
    thp3  = _mm512_mask_permute4f128_epi32(vhp3 , 0xff00, vhp7 , _MM_PERM_BADC); 
    thp8  = _mm512_mask_permute4f128_epi32(vhp8 , 0xff00, vhp12, _MM_PERM_BADC); 
    thp9  = _mm512_mask_permute4f128_epi32(vhp9 , 0xff00, vhp13, _MM_PERM_BADC); 
    thp10 = _mm512_mask_permute4f128_epi32(vhp10, 0xff00, vhp14, _MM_PERM_BADC); 
    thp11 = _mm512_mask_permute4f128_epi32(vhp11, 0xff00, vhp15, _MM_PERM_BADC); 
    mask1 = 0x0f0f;
    t4  = _mm512_mask_permute4f128_ps(v4 , mask1, v0 , _MM_PERM_CDAB); 
    t5  = _mm512_mask_permute4f128_ps(v5 , mask1, v1 , _MM_PERM_CDAB); 
    t6  = _mm512_mask_permute4f128_ps(v6 , mask1, v2 , _MM_PERM_CDAB); 
    t7  = _mm512_mask_permute4f128_ps(v7 , mask1, v3 , _MM_PERM_CDAB); 
    t12 = _mm512_mask_permute4f128_ps(v12, mask1, v8 , _MM_PERM_CDAB); 
    t13 = _mm512_mask_permute4f128_ps(v13, mask1, v9 , _MM_PERM_CDAB); 
    t14 = _mm512_mask_permute4f128_ps(v14, mask1, v10, _MM_PERM_CDAB); 
    t15 = _mm512_mask_permute4f128_ps(v15, mask1, v11, _MM_PERM_CDAB); 

    tlp4  = _mm512_mask_permute4f128_epi32(vlp4 , 0x00ff, vlp0 , _MM_PERM_BADC); 
    tlp5  = _mm512_mask_permute4f128_epi32(vlp5 , 0x00ff, vlp1 , _MM_PERM_BADC); 
    tlp6  = _mm512_mask_permute4f128_epi32(vlp6 , 0x00ff, vlp2 , _MM_PERM_BADC); 
    tlp7  = _mm512_mask_permute4f128_epi32(vlp7 , 0x00ff, vlp3 , _MM_PERM_BADC); 
    tlp12 = _mm512_mask_permute4f128_epi32(vlp12, 0x00ff, vlp8 , _MM_PERM_BADC); 
    tlp13 = _mm512_mask_permute4f128_epi32(vlp13, 0x00ff, vlp9 , _MM_PERM_BADC); 
    tlp14 = _mm512_mask_permute4f128_epi32(vlp14, 0x00ff, vlp10, _MM_PERM_BADC); 
    tlp15 = _mm512_mask_permute4f128_epi32(vlp15, 0x00ff, vlp11, _MM_PERM_BADC); 

    thp4  = _mm512_mask_permute4f128_epi32(vhp4 , 0x00ff, vhp0 , _MM_PERM_BADC); 
    thp5  = _mm512_mask_permute4f128_epi32(vhp5 , 0x00ff, vhp1 , _MM_PERM_BADC); 
    thp6  = _mm512_mask_permute4f128_epi32(vhp6 , 0x00ff, vhp2 , _MM_PERM_BADC); 
    thp7  = _mm512_mask_permute4f128_epi32(vhp7 , 0x00ff, vhp3 , _MM_PERM_BADC); 
    thp12 = _mm512_mask_permute4f128_epi32(vhp12, 0x00ff, vhp8 , _MM_PERM_BADC); 
    thp13 = _mm512_mask_permute4f128_epi32(vhp13, 0x00ff, vhp9 , _MM_PERM_BADC); 
    thp14 = _mm512_mask_permute4f128_epi32(vhp14, 0x00ff, vhp10, _MM_PERM_BADC); 
    thp15 = _mm512_mask_permute4f128_epi32(vhp15, 0x00ff, vhp11, _MM_PERM_BADC); 
    mask1 = 0xff00;
    v0 = _mm512_mask_permute4f128_ps(t0, mask1, t8 , _MM_PERM_BADC); 
    v2 = _mm512_mask_permute4f128_ps(t1, mask1, t9 , _MM_PERM_BADC); 
    v1 = _mm512_mask_permute4f128_ps(t2, mask1, t10, _MM_PERM_BADC); 
    v3 = _mm512_mask_permute4f128_ps(t3, mask1, t11, _MM_PERM_BADC); 
    v4 = _mm512_mask_permute4f128_ps(t4, mask1, t12, _MM_PERM_BADC); 
    v6 = _mm512_mask_permute4f128_ps(t5, mask1, t13, _MM_PERM_BADC); 
    v5 = _mm512_mask_permute4f128_ps(t6, mask1, t14, _MM_PERM_BADC); 
    v7 = _mm512_mask_permute4f128_ps(t7, mask1, t15, _MM_PERM_BADC); 

    vlp0 = tlp0; 
    vlp2 = tlp1; 
    vlp1 = tlp2; 
    vlp3 = tlp3; 
    vlp4 = tlp4; 
    vlp6 = tlp5; 
    vlp5 = tlp6; 
    vlp7 = tlp7; 

    vhp0 = tlp8 ; 
    vhp2 = tlp9 ; 
    vhp1 = tlp10; 
    vhp3 = tlp11; 
    vhp4 = tlp12; 
    vhp6 = tlp13; 
    vhp5 = tlp14; 
    vhp7 = tlp15; 
    mask1 = 0x00ff;
    v8  = _mm512_mask_permute4f128_ps(t8 , mask1, t0, _MM_PERM_BADC); 
    v10 = _mm512_mask_permute4f128_ps(t9 , mask1, t1, _MM_PERM_BADC); 
    v9  = _mm512_mask_permute4f128_ps(t10, mask1, t2, _MM_PERM_BADC); 
    v11 = _mm512_mask_permute4f128_ps(t11, mask1, t3, _MM_PERM_BADC); 
    v12 = _mm512_mask_permute4f128_ps(t12, mask1, t4, _MM_PERM_BADC); 
    v14 = _mm512_mask_permute4f128_ps(t13, mask1, t5, _MM_PERM_BADC); 
    v13 = _mm512_mask_permute4f128_ps(t14, mask1, t6, _MM_PERM_BADC); 
    v15 = _mm512_mask_permute4f128_ps(t15, mask1, t7, _MM_PERM_BADC);

    vlp8  = thp0; 
    vlp10 = thp1; 
    vlp9  = thp2; 
    vlp11 = thp3; 
    vlp12 = thp4; 
    vlp14 = thp5; 
    vlp13 = thp6; 
    vlp15 = thp7;

    vhp8  = thp8 ; 
    vhp10 = thp9 ; 
    vhp9  = thp10; 
    vhp11 = thp11; 
    vhp12 = thp12; 
    vhp14 = thp13; 
    vhp13 = thp14; 
    vhp15 = thp15;
}

template <typename T>
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_transpose_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3 )

{
    __m512d t0; 
    __m512d t1; 
    __m512d t2; 
    __m512d t3; 
    __m512d t4; 
    __m512d t5; 
    __m512d t6; 
    __m512d t7;
    __mmask8 mask1;
    __mmask16 mask2;

    __m512i tp0; 
    __m512i tp1; 
    __m512i tp2; 
    __m512i tp3; 
    __m512i tp4; 
    __m512i tp5; 
    __m512i tp6; 
    __m512i tp7;

    __m512i ap0; 
    __m512i ap1; 
    __m512i ap2; 
    __m512i ap3; 
    __m512i ap4; 
    __m512i ap5; 
    __m512i ap6; 
    __m512i ap7;
    tp0 = vp0;
    tp1 = _mm512_permute4f128_epi32(vp0, _MM_PERM_BADC);
    tp2 = vp1;
    tp3 = _mm512_permute4f128_epi32(vp1, _MM_PERM_BADC);
    tp4 = vp2;
    tp5 = _mm512_permute4f128_epi32(vp2, _MM_PERM_BADC);
    tp6 = vp3;
    tp7 = _mm512_permute4f128_epi32(vp3, _MM_PERM_BADC);

    mask1 = 0xaa;
    t0  = _mm512_mask_swizzle_pd(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_pd(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_pd(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_pd(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 

    ap0  = _mm512_mask_swizzle_epi32(tp0 , mask1, tp1 , _MM_SWIZ_REG_CDAB); 
    ap2  = _mm512_mask_swizzle_epi32(tp2 , mask1, tp3 , _MM_SWIZ_REG_CDAB); 
    ap4  = _mm512_mask_swizzle_epi32(tp4 , mask1, tp5 , _MM_SWIZ_REG_CDAB); 
    ap6  = _mm512_mask_swizzle_epi32(tp6 , mask1, tp7 , _MM_SWIZ_REG_CDAB); 
    mask1 = 0x55;
    t1  = _mm512_mask_swizzle_pd(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_pd(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_pd(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_pd(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 

    ap1  = _mm512_mask_swizzle_epi32(tp1 , mask1, tp0 , _MM_SWIZ_REG_CDAB); 
    ap3  = _mm512_mask_swizzle_epi32(tp3 , mask1, tp2 , _MM_SWIZ_REG_CDAB); 
    ap5  = _mm512_mask_swizzle_epi32(tp5 , mask1, tp4 , _MM_SWIZ_REG_CDAB); 
    ap7  = _mm512_mask_swizzle_epi32(tp7 , mask1, tp6 , _MM_SWIZ_REG_CDAB); 
    mask1 = 0xcc;
    v0  = _mm512_mask_swizzle_pd(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_pd(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_pd(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_pd(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 

    tp0  = _mm512_mask_swizzle_epi32(ap0 , mask1, ap2 , _MM_SWIZ_REG_BADC); 
    tp2  = _mm512_mask_swizzle_epi32(ap1 , mask1, ap3 , _MM_SWIZ_REG_BADC); 
    tp4  = _mm512_mask_swizzle_epi32(ap4 , mask1, ap6 , _MM_SWIZ_REG_BADC); 
    tp6  = _mm512_mask_swizzle_epi32(ap5 , mask1, ap7 , _MM_SWIZ_REG_BADC); 
    mask1 = 0x33;
    v1  = _mm512_mask_swizzle_pd(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_pd(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_pd(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_pd(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 

    tp1  = _mm512_mask_swizzle_epi32(ap2 , mask1, ap0 , _MM_SWIZ_REG_BADC); 
    tp3  = _mm512_mask_swizzle_epi32(ap3 , mask1, ap1 , _MM_SWIZ_REG_BADC); 
    tp5  = _mm512_mask_swizzle_epi32(ap6 , mask1, ap4 , _MM_SWIZ_REG_BADC); 
    tp7  = _mm512_mask_swizzle_epi32(ap7 , mask1, ap5 , _MM_SWIZ_REG_BADC); 

    mask2 = 0xff00;
    t0  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v0 , mask2, (__m512i)v4 , _MM_PERM_BADC); 
    t1  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v1 , mask2, (__m512i)v5 , _MM_PERM_BADC); 
    t2  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v2 , mask2, (__m512i)v6 , _MM_PERM_BADC); 
    t3  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v3 , mask2, (__m512i)v7 , _MM_PERM_BADC); 

    ap0  = _mm512_mask_permute4f128_epi32(tp0 , 0xf0, tp4 , _MM_PERM_CDAB); 
    ap1  = _mm512_mask_permute4f128_epi32(tp1 , 0xf0, tp5 , _MM_PERM_CDAB); 
    ap2  = _mm512_mask_permute4f128_epi32(tp2 , 0xf0, tp6 , _MM_PERM_CDAB); 
    ap3  = _mm512_mask_permute4f128_epi32(tp3 , 0xf0, tp7 , _MM_PERM_CDAB); 
    mask2 = 0x00ff;                                                       
    t4  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v4 , mask2, (__m512i)v0 , _MM_PERM_BADC); 
    t5  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v5 , mask2, (__m512i)v1 , _MM_PERM_BADC); 
    t6  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v6 , mask2, (__m512i)v2 , _MM_PERM_BADC); 
    t7  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v7 , mask2, (__m512i)v3 , _MM_PERM_BADC); 

    ap4  = _mm512_mask_permute4f128_epi32(tp4 , 0x0f, tp0 , _MM_PERM_CDAB); 
    ap5  = _mm512_mask_permute4f128_epi32(tp5 , 0x0f, tp1 , _MM_PERM_CDAB); 
    ap6  = _mm512_mask_permute4f128_epi32(tp6 , 0x0f, tp2 , _MM_PERM_CDAB); 
    ap7  = _mm512_mask_permute4f128_epi32(tp7 , 0x0f, tp3 , _MM_PERM_CDAB); 

    v0 = t0;
    v1 = t1;
    v2 = t2;
    v3 = t3;
    v4 = t4;
    v5 = t5;
    v6 = t6;
    v7 = t7;

    vp0 = _mm512_mask_permute4f128_epi32(ap0, 0xff00, ap1, _MM_PERM_BADC);
    vp1 = _mm512_mask_permute4f128_epi32(ap2, 0xff00, ap3, _MM_PERM_BADC);
    vp2 = _mm512_mask_permute4f128_epi32(ap4, 0xff00, ap5, _MM_PERM_BADC);
    vp3 = _mm512_mask_permute4f128_epi32(ap6, 0xff00, ap7, _MM_PERM_BADC);

}
 
template <typename T>
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_transpose_key(
        T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
        __m512i &vp0 , __m512i &vp1 , __m512i &vp2 , __m512i &vp3,
        __m512i &vp4 , __m512i &vp5 , __m512i &vp6 , __m512i &vp7)

{
    __m512d t0; 
    __m512d t1; 
    __m512d t2; 
    __m512d t3; 
    __m512d t4; 
    __m512d t5; 
    __m512d t6; 
    __m512d t7;
    __mmask8 mask1;
    __mmask16 mask2;

    __m512i tp0; 
    __m512i tp1; 
    __m512i tp2; 
    __m512i tp3; 
    __m512i tp4; 
    __m512i tp5; 
    __m512i tp6; 
    __m512i tp7;

    mask1 = 0xaa;
    t0  = _mm512_mask_swizzle_pd(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_pd(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_pd(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_pd(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 

    tp0  = _mm512_mask_swizzle_epi64(vp0 , mask1, vp1 , _MM_SWIZ_REG_CDAB); 
    tp2  = _mm512_mask_swizzle_epi64(vp2 , mask1, vp3 , _MM_SWIZ_REG_CDAB); 
    tp4  = _mm512_mask_swizzle_epi64(vp4 , mask1, vp5 , _MM_SWIZ_REG_CDAB); 
    tp6  = _mm512_mask_swizzle_epi64(vp6 , mask1, vp7 , _MM_SWIZ_REG_CDAB); 
    mask1 = 0x55;
    t1  = _mm512_mask_swizzle_pd(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_pd(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_pd(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_pd(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 

    tp1  = _mm512_mask_swizzle_epi64(vp1 , mask1, vp0 , _MM_SWIZ_REG_CDAB); 
    tp3  = _mm512_mask_swizzle_epi64(vp3 , mask1, vp2 , _MM_SWIZ_REG_CDAB); 
    tp5  = _mm512_mask_swizzle_epi64(vp5 , mask1, vp4 , _MM_SWIZ_REG_CDAB); 
    tp7  = _mm512_mask_swizzle_epi64(vp7 , mask1, vp6 , _MM_SWIZ_REG_CDAB); 
    mask1 = 0xcc;
    v0  = _mm512_mask_swizzle_pd(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_pd(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_pd(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_pd(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 

    vp0  = _mm512_mask_swizzle_epi64(tp0 , mask1, tp2 , _MM_SWIZ_REG_BADC); 
    vp2  = _mm512_mask_swizzle_epi64(tp1 , mask1, tp3 , _MM_SWIZ_REG_BADC); 
    vp4  = _mm512_mask_swizzle_epi64(tp4 , mask1, tp6 , _MM_SWIZ_REG_BADC); 
    vp6  = _mm512_mask_swizzle_epi64(tp5 , mask1, tp7 , _MM_SWIZ_REG_BADC); 
    mask1 = 0x33;
    v1  = _mm512_mask_swizzle_pd(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_pd(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_pd(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_pd(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 

    vp1  = _mm512_mask_swizzle_epi64(tp2 , mask1, tp0 , _MM_SWIZ_REG_BADC); 
    vp3  = _mm512_mask_swizzle_epi64(tp3 , mask1, tp1 , _MM_SWIZ_REG_BADC); 
    vp5  = _mm512_mask_swizzle_epi64(tp6 , mask1, tp4 , _MM_SWIZ_REG_BADC); 
    vp7  = _mm512_mask_swizzle_epi64(tp7 , mask1, tp5 , _MM_SWIZ_REG_BADC); 

    mask2 = 0xff00;
    t0  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v0 , mask2, (__m512i)v4 , _MM_PERM_BADC); 
    t1  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v1 , mask2, (__m512i)v5 , _MM_PERM_BADC); 
    t2  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v2 , mask2, (__m512i)v6 , _MM_PERM_BADC); 
    t3  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v3 , mask2, (__m512i)v7 , _MM_PERM_BADC); 

    tp0  = _mm512_mask_permute4f128_epi32(vp0 , mask2, vp4 , _MM_PERM_BADC); 
    tp1  = _mm512_mask_permute4f128_epi32(vp1 , mask2, vp5 , _MM_PERM_BADC); 
    tp2  = _mm512_mask_permute4f128_epi32(vp2 , mask2, vp6 , _MM_PERM_BADC); 
    tp3  = _mm512_mask_permute4f128_epi32(vp3 , mask2, vp7 , _MM_PERM_BADC); 
    mask2 = 0x00ff;                                                       
    t4  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v4 , mask2, (__m512i)v0 , _MM_PERM_BADC); 
    t5  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v5 , mask2, (__m512i)v1 , _MM_PERM_BADC); 
    t6  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v6 , mask2, (__m512i)v2 , _MM_PERM_BADC); 
    t7  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v7 , mask2, (__m512i)v3 , _MM_PERM_BADC); 

    tp4  = _mm512_mask_permute4f128_epi32(vp4 , mask2, vp0 , _MM_PERM_BADC); 
    tp5  = _mm512_mask_permute4f128_epi32(vp5 , mask2, vp1 , _MM_PERM_BADC); 
    tp6  = _mm512_mask_permute4f128_epi32(vp6 , mask2, vp2 , _MM_PERM_BADC); 
    tp7  = _mm512_mask_permute4f128_epi32(vp7 , mask2, vp3 , _MM_PERM_BADC); 

    v0 = t0;
    v1 = t1;
    v2 = t2;
    v3 = t3;
    v4 = t4;
    v5 = t5;
    v6 = t6;
    v7 = t7;

    vp0 = tp0;
    vp1 = tp1;
    vp2 = tp2;
    vp3 = tp3;
    vp4 = tp4;
    vp5 = tp5;
    vp6 = tp6;
    vp7 = tp7;
}
/**
 * Float vector version (__m512):
 * This method performs the in-register transpose. The sorted elements 
 * are stored horizontally within the registers.
 *
 * @param v0-v15 vector data registers
 * @return sorted data stored horizontally in the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_transpose(T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7 , 
                      T& v8 , T& v9 , T& v10, T& v11, T& v12, T& v13, T& v14, T& v15)
{
    __m512i t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
    __mmask16 mask1;
    mask1 = 0xaaaa;
    t0  = _mm512_mask_swizzle_epi32((__m512i)v0 , mask1, (__m512i)v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_epi32((__m512i)v2 , mask1, (__m512i)v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_epi32((__m512i)v4 , mask1, (__m512i)v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_epi32((__m512i)v6 , mask1, (__m512i)v7 , _MM_SWIZ_REG_CDAB); 
    t8  = _mm512_mask_swizzle_epi32((__m512i)v8 , mask1, (__m512i)v9 , _MM_SWIZ_REG_CDAB); 
    t10 = _mm512_mask_swizzle_epi32((__m512i)v10, mask1, (__m512i)v11, _MM_SWIZ_REG_CDAB); 
    t12 = _mm512_mask_swizzle_epi32((__m512i)v12, mask1, (__m512i)v13, _MM_SWIZ_REG_CDAB); 
    t14 = _mm512_mask_swizzle_epi32((__m512i)v14, mask1, (__m512i)v15, _MM_SWIZ_REG_CDAB); 

    mask1 = 0x5555;
    t1  = _mm512_mask_swizzle_epi32((__m512i)v1 , mask1, (__m512i)v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_epi32((__m512i)v3 , mask1, (__m512i)v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_epi32((__m512i)v5 , mask1, (__m512i)v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_epi32((__m512i)v7 , mask1, (__m512i)v6 , _MM_SWIZ_REG_CDAB); 
    t9  = _mm512_mask_swizzle_epi32((__m512i)v9 , mask1, (__m512i)v8 , _MM_SWIZ_REG_CDAB); 
    t11 = _mm512_mask_swizzle_epi32((__m512i)v11, mask1, (__m512i)v10, _MM_SWIZ_REG_CDAB); 
    t13 = _mm512_mask_swizzle_epi32((__m512i)v13, mask1, (__m512i)v12, _MM_SWIZ_REG_CDAB); 
    t15 = _mm512_mask_swizzle_epi32((__m512i)v15, mask1, (__m512i)v14, _MM_SWIZ_REG_CDAB); 

    mask1 = 0xcccc;
    v0  = (__m512)_mm512_mask_swizzle_epi32(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = (__m512)_mm512_mask_swizzle_epi32(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = (__m512)_mm512_mask_swizzle_epi32(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = (__m512)_mm512_mask_swizzle_epi32(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 
    v8  = (__m512)_mm512_mask_swizzle_epi32(t8 , mask1, t10, _MM_SWIZ_REG_BADC); 
    v10 = (__m512)_mm512_mask_swizzle_epi32(t9 , mask1, t11, _MM_SWIZ_REG_BADC); 
    v12 = (__m512)_mm512_mask_swizzle_epi32(t12, mask1, t14, _MM_SWIZ_REG_BADC); 
    v14 = (__m512)_mm512_mask_swizzle_epi32(t13, mask1, t15, _MM_SWIZ_REG_BADC); 

    mask1 = 0x3333;
    v1  = (__m512)_mm512_mask_swizzle_epi32(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = (__m512)_mm512_mask_swizzle_epi32(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = (__m512)_mm512_mask_swizzle_epi32(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = (__m512)_mm512_mask_swizzle_epi32(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 
    v9  = (__m512)_mm512_mask_swizzle_epi32(t10, mask1, t8 , _MM_SWIZ_REG_BADC); 
    v11 = (__m512)_mm512_mask_swizzle_epi32(t11, mask1, t9 , _MM_SWIZ_REG_BADC); 
    v13 = (__m512)_mm512_mask_swizzle_epi32(t14, mask1, t12, _MM_SWIZ_REG_BADC); 
    v15 = (__m512)_mm512_mask_swizzle_epi32(t15, mask1, t13, _MM_SWIZ_REG_BADC); 

    mask1 = 0xf0f0;
    t0  = _mm512_mask_permute4f128_epi32((__m512i)v0 , mask1, (__m512i)v4 , _MM_PERM_CDAB); 
    t1  = _mm512_mask_permute4f128_epi32((__m512i)v1 , mask1, (__m512i)v5 , _MM_PERM_CDAB); 
    t2  = _mm512_mask_permute4f128_epi32((__m512i)v2 , mask1, (__m512i)v6 , _MM_PERM_CDAB); 
    t3  = _mm512_mask_permute4f128_epi32((__m512i)v3 , mask1, (__m512i)v7 , _MM_PERM_CDAB); 
    t8  = _mm512_mask_permute4f128_epi32((__m512i)v8 , mask1, (__m512i)v12, _MM_PERM_CDAB); 
    t9  = _mm512_mask_permute4f128_epi32((__m512i)v9 , mask1, (__m512i)v13, _MM_PERM_CDAB); 
    t10 = _mm512_mask_permute4f128_epi32((__m512i)v10, mask1, (__m512i)v14, _MM_PERM_CDAB); 
    t11 = _mm512_mask_permute4f128_epi32((__m512i)v11, mask1, (__m512i)v15, _MM_PERM_CDAB); 

    mask1 = 0x0f0f;
    t4  = _mm512_mask_permute4f128_epi32((__m512i)v4 , mask1, (__m512i)v0 , _MM_PERM_CDAB); 
    t5  = _mm512_mask_permute4f128_epi32((__m512i)v5 , mask1, (__m512i)v1 , _MM_PERM_CDAB); 
    t6  = _mm512_mask_permute4f128_epi32((__m512i)v6 , mask1, (__m512i)v2 , _MM_PERM_CDAB); 
    t7  = _mm512_mask_permute4f128_epi32((__m512i)v7 , mask1, (__m512i)v3 , _MM_PERM_CDAB); 
    t12 = _mm512_mask_permute4f128_epi32((__m512i)v12, mask1, (__m512i)v8 , _MM_PERM_CDAB); 
    t13 = _mm512_mask_permute4f128_epi32((__m512i)v13, mask1, (__m512i)v9 , _MM_PERM_CDAB); 
    t14 = _mm512_mask_permute4f128_epi32((__m512i)v14, mask1, (__m512i)v10, _MM_PERM_CDAB); 
    t15 = _mm512_mask_permute4f128_epi32((__m512i)v15, mask1, (__m512i)v11, _MM_PERM_CDAB); 

    mask1 = 0xff00;
    v0 = (__m512)_mm512_mask_permute4f128_epi32(t0, mask1, t8 , _MM_PERM_BADC); 
    v2 = (__m512)_mm512_mask_permute4f128_epi32(t1, mask1, t9 , _MM_PERM_BADC); 
    v1 = (__m512)_mm512_mask_permute4f128_epi32(t2, mask1, t10, _MM_PERM_BADC); 
    v3 = (__m512)_mm512_mask_permute4f128_epi32(t3, mask1, t11, _MM_PERM_BADC); 
    v4 = (__m512)_mm512_mask_permute4f128_epi32(t4, mask1, t12, _MM_PERM_BADC); 
    v6 = (__m512)_mm512_mask_permute4f128_epi32(t5, mask1, t13, _MM_PERM_BADC); 
    v5 = (__m512)_mm512_mask_permute4f128_epi32(t6, mask1, t14, _MM_PERM_BADC); 
    v7 = (__m512)_mm512_mask_permute4f128_epi32(t7, mask1, t15, _MM_PERM_BADC); 

    mask1 = 0x00ff;
    v8  = (__m512)_mm512_mask_permute4f128_epi32(t8 , mask1, t0, _MM_PERM_BADC); 
    v10 = (__m512)_mm512_mask_permute4f128_epi32(t9 , mask1, t1, _MM_PERM_BADC); 
    v9  = (__m512)_mm512_mask_permute4f128_epi32(t10, mask1, t2, _MM_PERM_BADC); 
    v11 = (__m512)_mm512_mask_permute4f128_epi32(t11, mask1, t3, _MM_PERM_BADC); 
    v12 = (__m512)_mm512_mask_permute4f128_epi32(t12, mask1, t4, _MM_PERM_BADC); 
    v14 = (__m512)_mm512_mask_permute4f128_epi32(t13, mask1, t5, _MM_PERM_BADC); 
    v13 = (__m512)_mm512_mask_permute4f128_epi32(t14, mask1, t6, _MM_PERM_BADC); 
    v15 = (__m512)_mm512_mask_permute4f128_epi32(t15, mask1, t7, _MM_PERM_BADC);
}

/**
 * Double vector version (__m512d):
 * This method performs the in-register transpose. The sorted elements 
 * are stored horizontally within the registers.
 *
 * @param v0-v7 vector data registers
 * @return sorted data stored horizontally in the registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_transpose(T& v0 , T& v1 , T& v2 , T& v3 , T& v4 , T& v5 , T& v6 , T& v7)
{
    __m512d t0; 
    __m512d t1; 
    __m512d t2; 
    __m512d t3; 
    __m512d t4; 
    __m512d t5; 
    __m512d t6; 
    __m512d t7;
    __mmask16 mask1;
    mask1 = 0xaa;
    t0  = _mm512_mask_swizzle_pd(v0 , mask1, v1 , _MM_SWIZ_REG_CDAB); 
    t2  = _mm512_mask_swizzle_pd(v2 , mask1, v3 , _MM_SWIZ_REG_CDAB); 
    t4  = _mm512_mask_swizzle_pd(v4 , mask1, v5 , _MM_SWIZ_REG_CDAB); 
    t6  = _mm512_mask_swizzle_pd(v6 , mask1, v7 , _MM_SWIZ_REG_CDAB); 
    mask1 = 0x55;
    t1  = _mm512_mask_swizzle_pd(v1 , mask1, v0 , _MM_SWIZ_REG_CDAB); 
    t3  = _mm512_mask_swizzle_pd(v3 , mask1, v2 , _MM_SWIZ_REG_CDAB); 
    t5  = _mm512_mask_swizzle_pd(v5 , mask1, v4 , _MM_SWIZ_REG_CDAB); 
    t7  = _mm512_mask_swizzle_pd(v7 , mask1, v6 , _MM_SWIZ_REG_CDAB); 

    mask1 = 0xcc;
    v0  = _mm512_mask_swizzle_pd(t0 , mask1, t2 , _MM_SWIZ_REG_BADC); 
    v2  = _mm512_mask_swizzle_pd(t1 , mask1, t3 , _MM_SWIZ_REG_BADC); 
    v4  = _mm512_mask_swizzle_pd(t4 , mask1, t6 , _MM_SWIZ_REG_BADC); 
    v6  = _mm512_mask_swizzle_pd(t5 , mask1, t7 , _MM_SWIZ_REG_BADC); 
    mask1 = 0x33;
    v1  = _mm512_mask_swizzle_pd(t2 , mask1, t0 , _MM_SWIZ_REG_BADC); 
    v3  = _mm512_mask_swizzle_pd(t3 , mask1, t1 , _MM_SWIZ_REG_BADC); 
    v5  = _mm512_mask_swizzle_pd(t6 , mask1, t4 , _MM_SWIZ_REG_BADC); 
    v7  = _mm512_mask_swizzle_pd(t7 , mask1, t5 , _MM_SWIZ_REG_BADC); 

    mask1 = 0xff00;
    t0  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v0 , mask1, (__m512i)v4 , _MM_PERM_BADC); 
    t1  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v1 , mask1, (__m512i)v5 , _MM_PERM_BADC); 
    t2  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v2 , mask1, (__m512i)v6 , _MM_PERM_BADC); 
    t3  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v3 , mask1, (__m512i)v7 , _MM_PERM_BADC); 
    mask1 = 0x00ff;                                                       
    t4  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v4 , mask1, (__m512i)v0 , _MM_PERM_BADC); 
    t5  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v5 , mask1, (__m512i)v1 , _MM_PERM_BADC); 
    t6  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v6 , mask1, (__m512i)v2 , _MM_PERM_BADC); 
    t7  = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)v7 , mask1, (__m512i)v3 , _MM_PERM_BADC); 

    v0 = t0;
    v1 = t1;
    v2 = t2;
    v3 = t3;
    v4 = t4;
    v5 = t5;
    v6 = t6;
    v7 = t7;
}

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
sorter(T*& orig, uint32_t size)
{
    uint32_t i, j;
    __m512i vec0 ;
    __m512i vec1 ;
    __m512i vec2 ;
    __m512i vec3 ;
    __m512i vec4 ;
    __m512i vec5 ;
    __m512i vec6 ;
    __m512i vec7 ;
    __m512i vec8 ;
    __m512i vec9 ;
    __m512i vec10;
    __m512i vec11;
    __m512i vec12;
    __m512i vec13;
    __m512i vec14;
    __m512i vec15;

    uint8_t stride = (uint8_t)simd_width::AVX512_INT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_epi32(vec0 , orig + i + 0  * stride     );
        vec0  = _mm512_loadunpackhi_epi32(vec0 , orig + i + 0  * stride + 16);
        vec1  = _mm512_loadunpacklo_epi32(vec1 , orig + i + 1  * stride     );
        vec1  = _mm512_loadunpackhi_epi32(vec1 , orig + i + 1  * stride + 16);
        vec2  = _mm512_loadunpacklo_epi32(vec2 , orig + i + 2  * stride     );
        vec2  = _mm512_loadunpackhi_epi32(vec2 , orig + i + 2  * stride + 16);
        vec3  = _mm512_loadunpacklo_epi32(vec3 , orig + i + 3  * stride     );
        vec3  = _mm512_loadunpackhi_epi32(vec3 , orig + i + 3  * stride + 16);
        vec4  = _mm512_loadunpacklo_epi32(vec4 , orig + i + 4  * stride     );
        vec4  = _mm512_loadunpackhi_epi32(vec4 , orig + i + 4  * stride + 16);
        vec5  = _mm512_loadunpacklo_epi32(vec5 , orig + i + 5  * stride     );
        vec5  = _mm512_loadunpackhi_epi32(vec5 , orig + i + 5  * stride + 16);
        vec6  = _mm512_loadunpacklo_epi32(vec6 , orig + i + 6  * stride     );
        vec6  = _mm512_loadunpackhi_epi32(vec6 , orig + i + 6  * stride + 16);
        vec7  = _mm512_loadunpacklo_epi32(vec7 , orig + i + 7  * stride     );
        vec7  = _mm512_loadunpackhi_epi32(vec7 , orig + i + 7  * stride + 16);
        vec8  = _mm512_loadunpacklo_epi32(vec8 , orig + i + 8  * stride     );
        vec8  = _mm512_loadunpackhi_epi32(vec8 , orig + i + 8  * stride + 16);
        vec9  = _mm512_loadunpacklo_epi32(vec9 , orig + i + 9  * stride     );
        vec9  = _mm512_loadunpackhi_epi32(vec9 , orig + i + 9  * stride + 16);
        vec10 = _mm512_loadunpacklo_epi32(vec10, orig + i + 10 * stride     );
        vec10 = _mm512_loadunpackhi_epi32(vec10, orig + i + 10 * stride + 16);
        vec11 = _mm512_loadunpacklo_epi32(vec11, orig + i + 11 * stride     );
        vec11 = _mm512_loadunpackhi_epi32(vec11, orig + i + 11 * stride + 16);
        vec12 = _mm512_loadunpacklo_epi32(vec12, orig + i + 12 * stride     );
        vec12 = _mm512_loadunpackhi_epi32(vec12, orig + i + 12 * stride + 16);
        vec13 = _mm512_loadunpacklo_epi32(vec13, orig + i + 13 * stride     );
        vec13 = _mm512_loadunpackhi_epi32(vec13, orig + i + 13 * stride + 16);
        vec14 = _mm512_loadunpacklo_epi32(vec14, orig + i + 14 * stride     );
        vec14 = _mm512_loadunpackhi_epi32(vec14, orig + i + 14 * stride + 16);
        vec15 = _mm512_loadunpacklo_epi32(vec15, orig + i + 15 * stride     );
        vec15 = _mm512_loadunpackhi_epi32(vec15, orig + i + 15 * stride + 16);

        in_register_sort(
                vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
                vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15);
        in_register_transpose(
                vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
                vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15);

        _mm512_packstorelo_epi32(orig + i + 0  * stride     , vec0 );
        _mm512_packstorehi_epi32(orig + i + 0  * stride + 16, vec0 );
        _mm512_packstorelo_epi32(orig + i + 1  * stride     , vec1 );
        _mm512_packstorehi_epi32(orig + i + 1  * stride + 16, vec1 );
        _mm512_packstorelo_epi32(orig + i + 2  * stride     , vec2 );
        _mm512_packstorehi_epi32(orig + i + 2  * stride + 16, vec2 );
        _mm512_packstorelo_epi32(orig + i + 3  * stride     , vec3 );
        _mm512_packstorehi_epi32(orig + i + 3  * stride + 16, vec3 );
        _mm512_packstorelo_epi32(orig + i + 4  * stride     , vec4 );
        _mm512_packstorehi_epi32(orig + i + 4  * stride + 16, vec4 );
        _mm512_packstorelo_epi32(orig + i + 5  * stride     , vec5 );
        _mm512_packstorehi_epi32(orig + i + 5  * stride + 16, vec5 );
        _mm512_packstorelo_epi32(orig + i + 6  * stride     , vec6 );
        _mm512_packstorehi_epi32(orig + i + 6  * stride + 16, vec6 );
        _mm512_packstorelo_epi32(orig + i + 7  * stride     , vec7 );
        _mm512_packstorehi_epi32(orig + i + 7  * stride + 16, vec7 );
        _mm512_packstorelo_epi32(orig + i + 8  * stride     , vec8 );
        _mm512_packstorehi_epi32(orig + i + 8  * stride + 16, vec8 );
        _mm512_packstorelo_epi32(orig + i + 9  * stride     , vec9 );
        _mm512_packstorehi_epi32(orig + i + 9  * stride + 16, vec9 );
        _mm512_packstorelo_epi32(orig + i + 10 * stride     , vec10);
        _mm512_packstorehi_epi32(orig + i + 10 * stride + 16, vec10);
        _mm512_packstorelo_epi32(orig + i + 11 * stride     , vec11);
        _mm512_packstorehi_epi32(orig + i + 11 * stride + 16, vec11);
        _mm512_packstorelo_epi32(orig + i + 12 * stride     , vec12);
        _mm512_packstorehi_epi32(orig + i + 12 * stride + 16, vec12);
        _mm512_packstorelo_epi32(orig + i + 13 * stride     , vec13);
        _mm512_packstorehi_epi32(orig + i + 13 * stride + 16, vec13);
        _mm512_packstorelo_epi32(orig + i + 14 * stride     , vec14);
        _mm512_packstorehi_epi32(orig + i + 14 * stride + 16, vec14);
        _mm512_packstorelo_epi32(orig + i + 15 * stride     , vec15);
        _mm512_packstorehi_epi32(orig + i + 15 * stride + 16, vec15);
    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap(orig, i+0 , i+1 );
        swap(orig, i+2 , i+3 );
        swap(orig, i+4 , i+5 );
        swap(orig, i+6 , i+7 );
        swap(orig, i+8 , i+9 );
        swap(orig, i+10, i+11);
        swap(orig, i+12, i+13);
        swap(orig, i+14, i+15);
        swap(orig, i+0 , i+2 );
        swap(orig, i+4 , i+6 );
        swap(orig, i+8 , i+10);
        swap(orig, i+12, i+14);
        swap(orig, i+1 , i+3 );
        swap(orig, i+5 , i+7 );
        swap(orig, i+9 , i+11);
        swap(orig, i+13, i+15);
        swap(orig, i+0 , i+4 );
        swap(orig, i+8 , i+12);
        swap(orig, i+1 , i+5 );
        swap(orig, i+9 , i+13);
        swap(orig, i+2 , i+6 );
        swap(orig, i+10, i+14);
        swap(orig, i+3 , i+7 );
        swap(orig, i+11, i+15);
        swap(orig, i+0 , i+8 );
        swap(orig, i+1 , i+9 );
        swap(orig, i+2 , i+10);
        swap(orig, i+3 , i+11);
        swap(orig, i+4 , i+12);
        swap(orig, i+5 , i+13);
        swap(orig, i+6 , i+14);
        swap(orig, i+7 , i+15);
        swap(orig, i+5 , i+10);
        swap(orig, i+6 , i+9 );
        swap(orig, i+3 , i+12);
        swap(orig, i+13, i+14);
        swap(orig, i+7 , i+11);
        swap(orig, i+1 , i+2 );
        swap(orig, i+4 , i+8 );
        swap(orig, i+1 , i+4 );
        swap(orig, i+7 , i+13);
        swap(orig, i+2 , i+8 );
        swap(orig, i+11, i+14);
        swap(orig, i+2 , i+4 );
        swap(orig, i+5 , i+6 );
        swap(orig, i+9 , i+10);
        swap(orig, i+11, i+13);
        swap(orig, i+3 , i+8 );
        swap(orig, i+7 , i+12);
        swap(orig, i+6 , i+8 );
        swap(orig, i+10, i+12);
        swap(orig, i+3 , i+5 );
        swap(orig, i+7 , i+9 );
        swap(orig, i+3 , i+4 );
        swap(orig, i+5 , i+6 );
        swap(orig, i+7 , i+8 );
        swap(orig, i+9 , i+10);
        swap(orig, i+11, i+12);
        swap(orig, i+6 , i+7 );
        swap(orig, i+8 , i+9 );
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

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
sorter_key(T*& orig, int *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m512i vec0 ;
    __m512i vec1 ;
    __m512i vec2 ;
    __m512i vec3 ;
    __m512i vec4 ;
    __m512i vec5 ;
    __m512i vec6 ;
    __m512i vec7 ;
    __m512i vec8 ;
    __m512i vec9 ;
    __m512i vec10;
    __m512i vec11;
    __m512i vec12;
    __m512i vec13;
    __m512i vec14;
    __m512i vec15;

    // p* holds the index vectors (16 32-bit indices)
    __m512i p0 ;
    __m512i p1 ;
    __m512i p2 ;
    __m512i p3 ;
    __m512i p4 ;
    __m512i p5 ;
    __m512i p6 ;
    __m512i p7 ;
    __m512i p8 ;
    __m512i p9 ;
    __m512i p10;
    __m512i p11;
    __m512i p12;
    __m512i p13;
    __m512i p14;
    __m512i p15;

    uint8_t stride = (uint8_t)simd_width::AVX512_INT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_epi32(vec0 , orig + i + 0  * stride     );
        vec0  = _mm512_loadunpackhi_epi32(vec0 , orig + i + 0  * stride + 16);
        vec1  = _mm512_loadunpacklo_epi32(vec1 , orig + i + 1  * stride     );
        vec1  = _mm512_loadunpackhi_epi32(vec1 , orig + i + 1  * stride + 16);
        vec2  = _mm512_loadunpacklo_epi32(vec2 , orig + i + 2  * stride     );
        vec2  = _mm512_loadunpackhi_epi32(vec2 , orig + i + 2  * stride + 16);
        vec3  = _mm512_loadunpacklo_epi32(vec3 , orig + i + 3  * stride     );
        vec3  = _mm512_loadunpackhi_epi32(vec3 , orig + i + 3  * stride + 16);
        vec4  = _mm512_loadunpacklo_epi32(vec4 , orig + i + 4  * stride     );
        vec4  = _mm512_loadunpackhi_epi32(vec4 , orig + i + 4  * stride + 16);
        vec5  = _mm512_loadunpacklo_epi32(vec5 , orig + i + 5  * stride     );
        vec5  = _mm512_loadunpackhi_epi32(vec5 , orig + i + 5  * stride + 16);
        vec6  = _mm512_loadunpacklo_epi32(vec6 , orig + i + 6  * stride     );
        vec6  = _mm512_loadunpackhi_epi32(vec6 , orig + i + 6  * stride + 16);
        vec7  = _mm512_loadunpacklo_epi32(vec7 , orig + i + 7  * stride     );
        vec7  = _mm512_loadunpackhi_epi32(vec7 , orig + i + 7  * stride + 16);
        vec8  = _mm512_loadunpacklo_epi32(vec8 , orig + i + 8  * stride     );
        vec8  = _mm512_loadunpackhi_epi32(vec8 , orig + i + 8  * stride + 16);
        vec9  = _mm512_loadunpacklo_epi32(vec9 , orig + i + 9  * stride     );
        vec9  = _mm512_loadunpackhi_epi32(vec9 , orig + i + 9  * stride + 16);
        vec10 = _mm512_loadunpacklo_epi32(vec10, orig + i + 10 * stride     );
        vec10 = _mm512_loadunpackhi_epi32(vec10, orig + i + 10 * stride + 16);
        vec11 = _mm512_loadunpacklo_epi32(vec11, orig + i + 11 * stride     );
        vec11 = _mm512_loadunpackhi_epi32(vec11, orig + i + 11 * stride + 16);
        vec12 = _mm512_loadunpacklo_epi32(vec12, orig + i + 12 * stride     );
        vec12 = _mm512_loadunpackhi_epi32(vec12, orig + i + 12 * stride + 16);
        vec13 = _mm512_loadunpacklo_epi32(vec13, orig + i + 13 * stride     );
        vec13 = _mm512_loadunpackhi_epi32(vec13, orig + i + 13 * stride + 16);
        vec14 = _mm512_loadunpacklo_epi32(vec14, orig + i + 14 * stride     );
        vec14 = _mm512_loadunpackhi_epi32(vec14, orig + i + 14 * stride + 16);
        vec15 = _mm512_loadunpacklo_epi32(vec15, orig + i + 15 * stride     );
        vec15 = _mm512_loadunpackhi_epi32(vec15, orig + i + 15 * stride + 16);

        p0  = _mm512_loadunpacklo_epi32(p0 , ptr + i + 0  * stride     );
        p0  = _mm512_loadunpackhi_epi32(p0 , ptr + i + 0  * stride + 16);
        p1  = _mm512_loadunpacklo_epi32(p1 , ptr + i + 1  * stride     );
        p1  = _mm512_loadunpackhi_epi32(p1 , ptr + i + 1  * stride + 16);
        p2  = _mm512_loadunpacklo_epi32(p2 , ptr + i + 2  * stride     );
        p2  = _mm512_loadunpackhi_epi32(p2 , ptr + i + 2  * stride + 16);
        p3  = _mm512_loadunpacklo_epi32(p3 , ptr + i + 3  * stride     );
        p3  = _mm512_loadunpackhi_epi32(p3 , ptr + i + 3  * stride + 16);
        p4  = _mm512_loadunpacklo_epi32(p4 , ptr + i + 4  * stride     );
        p4  = _mm512_loadunpackhi_epi32(p4 , ptr + i + 4  * stride + 16);
        p5  = _mm512_loadunpacklo_epi32(p5 , ptr + i + 5  * stride     );
        p5  = _mm512_loadunpackhi_epi32(p5 , ptr + i + 5  * stride + 16);
        p6  = _mm512_loadunpacklo_epi32(p6 , ptr + i + 6  * stride     );
        p6  = _mm512_loadunpackhi_epi32(p6 , ptr + i + 6  * stride + 16);
        p7  = _mm512_loadunpacklo_epi32(p7 , ptr + i + 7  * stride     );
        p7  = _mm512_loadunpackhi_epi32(p7 , ptr + i + 7  * stride + 16);
        p8  = _mm512_loadunpacklo_epi32(p8 , ptr + i + 8  * stride     );
        p8  = _mm512_loadunpackhi_epi32(p8 , ptr + i + 8  * stride + 16);
        p9  = _mm512_loadunpacklo_epi32(p9 , ptr + i + 9  * stride     );
        p9  = _mm512_loadunpackhi_epi32(p9 , ptr + i + 9  * stride + 16);
        p10 = _mm512_loadunpacklo_epi32(p10, ptr + i + 10 * stride     );
        p10 = _mm512_loadunpackhi_epi32(p10, ptr + i + 10 * stride + 16);
        p11 = _mm512_loadunpacklo_epi32(p11, ptr + i + 11 * stride     );
        p11 = _mm512_loadunpackhi_epi32(p11, ptr + i + 11 * stride + 16);
        p12 = _mm512_loadunpacklo_epi32(p12, ptr + i + 12 * stride     );
        p12 = _mm512_loadunpackhi_epi32(p12, ptr + i + 12 * stride + 16);
        p13 = _mm512_loadunpacklo_epi32(p13, ptr + i + 13 * stride     );
        p13 = _mm512_loadunpackhi_epi32(p13, ptr + i + 13 * stride + 16);
        p14 = _mm512_loadunpacklo_epi32(p14, ptr + i + 14 * stride     );
        p14 = _mm512_loadunpackhi_epi32(p14, ptr + i + 14 * stride + 16);
        p15 = _mm512_loadunpacklo_epi32(p15, ptr + i + 15 * stride     );
        p15 = _mm512_loadunpackhi_epi32(p15, ptr + i + 15 * stride + 16);

        in_register_sort_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);

        in_register_transpose_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);

        _mm512_packstorelo_epi32(orig + i + 0  * stride     , vec0 );
        _mm512_packstorehi_epi32(orig + i + 0  * stride + 16, vec0 );
        _mm512_packstorelo_epi32(orig + i + 1  * stride     , vec1 );
        _mm512_packstorehi_epi32(orig + i + 1  * stride + 16, vec1 );
        _mm512_packstorelo_epi32(orig + i + 2  * stride     , vec2 );
        _mm512_packstorehi_epi32(orig + i + 2  * stride + 16, vec2 );
        _mm512_packstorelo_epi32(orig + i + 3  * stride     , vec3 );
        _mm512_packstorehi_epi32(orig + i + 3  * stride + 16, vec3 );
        _mm512_packstorelo_epi32(orig + i + 4  * stride     , vec4 );
        _mm512_packstorehi_epi32(orig + i + 4  * stride + 16, vec4 );
        _mm512_packstorelo_epi32(orig + i + 5  * stride     , vec5 );
        _mm512_packstorehi_epi32(orig + i + 5  * stride + 16, vec5 );
        _mm512_packstorelo_epi32(orig + i + 6  * stride     , vec6 );
        _mm512_packstorehi_epi32(orig + i + 6  * stride + 16, vec6 );
        _mm512_packstorelo_epi32(orig + i + 7  * stride     , vec7 );
        _mm512_packstorehi_epi32(orig + i + 7  * stride + 16, vec7 );
        _mm512_packstorelo_epi32(orig + i + 8  * stride     , vec8 );
        _mm512_packstorehi_epi32(orig + i + 8  * stride + 16, vec8 );
        _mm512_packstorelo_epi32(orig + i + 9  * stride     , vec9 );
        _mm512_packstorehi_epi32(orig + i + 9  * stride + 16, vec9 );
        _mm512_packstorelo_epi32(orig + i + 10 * stride     , vec10);
        _mm512_packstorehi_epi32(orig + i + 10 * stride + 16, vec10);
        _mm512_packstorelo_epi32(orig + i + 11 * stride     , vec11);
        _mm512_packstorehi_epi32(orig + i + 11 * stride + 16, vec11);
        _mm512_packstorelo_epi32(orig + i + 12 * stride     , vec12);
        _mm512_packstorehi_epi32(orig + i + 12 * stride + 16, vec12);
        _mm512_packstorelo_epi32(orig + i + 13 * stride     , vec13);
        _mm512_packstorehi_epi32(orig + i + 13 * stride + 16, vec13);
        _mm512_packstorelo_epi32(orig + i + 14 * stride     , vec14);
        _mm512_packstorehi_epi32(orig + i + 14 * stride + 16, vec14);
        _mm512_packstorelo_epi32(orig + i + 15 * stride     , vec15);
        _mm512_packstorehi_epi32(orig + i + 15 * stride + 16, vec15);

        _mm512_packstorelo_epi32(ptr + i + 0  * stride     , p0 );
        _mm512_packstorehi_epi32(ptr + i + 0  * stride + 16, p0 );
        _mm512_packstorelo_epi32(ptr + i + 1  * stride     , p1 );
        _mm512_packstorehi_epi32(ptr + i + 1  * stride + 16, p1 );
        _mm512_packstorelo_epi32(ptr + i + 2  * stride     , p2 );
        _mm512_packstorehi_epi32(ptr + i + 2  * stride + 16, p2 );
        _mm512_packstorelo_epi32(ptr + i + 3  * stride     , p3 );
        _mm512_packstorehi_epi32(ptr + i + 3  * stride + 16, p3 );
        _mm512_packstorelo_epi32(ptr + i + 4  * stride     , p4 );
        _mm512_packstorehi_epi32(ptr + i + 4  * stride + 16, p4 );
        _mm512_packstorelo_epi32(ptr + i + 5  * stride     , p5 );
        _mm512_packstorehi_epi32(ptr + i + 5  * stride + 16, p5 );
        _mm512_packstorelo_epi32(ptr + i + 6  * stride     , p6 );
        _mm512_packstorehi_epi32(ptr + i + 6  * stride + 16, p6 );
        _mm512_packstorelo_epi32(ptr + i + 7  * stride     , p7 );
        _mm512_packstorehi_epi32(ptr + i + 7  * stride + 16, p7 );
        _mm512_packstorelo_epi32(ptr + i + 8  * stride     , p8 );
        _mm512_packstorehi_epi32(ptr + i + 8  * stride + 16, p8 );
        _mm512_packstorelo_epi32(ptr + i + 9  * stride     , p9 );
        _mm512_packstorehi_epi32(ptr + i + 9  * stride + 16, p9 );
        _mm512_packstorelo_epi32(ptr + i + 10 * stride     , p10);
        _mm512_packstorehi_epi32(ptr + i + 10 * stride + 16, p10);
        _mm512_packstorelo_epi32(ptr + i + 11 * stride     , p11);
        _mm512_packstorehi_epi32(ptr + i + 11 * stride + 16, p11);
        _mm512_packstorelo_epi32(ptr + i + 12 * stride     , p12);
        _mm512_packstorehi_epi32(ptr + i + 12 * stride + 16, p12);
        _mm512_packstorelo_epi32(ptr + i + 13 * stride     , p13);
        _mm512_packstorehi_epi32(ptr + i + 13 * stride + 16, p13);
        _mm512_packstorelo_epi32(ptr + i + 14 * stride     , p14);
        _mm512_packstorehi_epi32(ptr + i + 14 * stride + 16, p14);
        _mm512_packstorelo_epi32(ptr + i + 15 * stride     , p15);
        _mm512_packstorehi_epi32(ptr + i + 15 * stride + 16, p15);
    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i+0 , i+1 );
        swap_key(orig, ptr, i+2 , i+3 );
        swap_key(orig, ptr, i+4 , i+5 );
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
        swap_key(orig, ptr, i+10, i+11);
        swap_key(orig, ptr, i+12, i+13);
        swap_key(orig, ptr, i+14, i+15);
        swap_key(orig, ptr, i+0 , i+2 );
        swap_key(orig, ptr, i+4 , i+6 );
        swap_key(orig, ptr, i+8 , i+10);
        swap_key(orig, ptr, i+12, i+14);
        swap_key(orig, ptr, i+1 , i+3 );
        swap_key(orig, ptr, i+5 , i+7 );
        swap_key(orig, ptr, i+9 , i+11);
        swap_key(orig, ptr, i+13, i+15);
        swap_key(orig, ptr, i+0 , i+4 );
        swap_key(orig, ptr, i+8 , i+12);
        swap_key(orig, ptr, i+1 , i+5 );
        swap_key(orig, ptr, i+9 , i+13);
        swap_key(orig, ptr, i+2 , i+6 );
        swap_key(orig, ptr, i+10, i+14);
        swap_key(orig, ptr, i+3 , i+7 );
        swap_key(orig, ptr, i+11, i+15);
        swap_key(orig, ptr, i+0 , i+8 );
        swap_key(orig, ptr, i+1 , i+9 );
        swap_key(orig, ptr, i+2 , i+10);
        swap_key(orig, ptr, i+3 , i+11);
        swap_key(orig, ptr, i+4 , i+12);
        swap_key(orig, ptr, i+5 , i+13);
        swap_key(orig, ptr, i+6 , i+14);
        swap_key(orig, ptr, i+7 , i+15);
        swap_key(orig, ptr, i+5 , i+10);
        swap_key(orig, ptr, i+6 , i+9 );
        swap_key(orig, ptr, i+3 , i+12);
        swap_key(orig, ptr, i+13, i+14);
        swap_key(orig, ptr, i+7 , i+11);
        swap_key(orig, ptr, i+1 , i+2 );
        swap_key(orig, ptr, i+4 , i+8 );
        swap_key(orig, ptr, i+1 , i+4 );
        swap_key(orig, ptr, i+7 , i+13);
        swap_key(orig, ptr, i+2 , i+8 );
        swap_key(orig, ptr, i+11, i+14);
        swap_key(orig, ptr, i+2 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+13);
        swap_key(orig, ptr, i+3 , i+8 );
        swap_key(orig, ptr, i+7 , i+12);
        swap_key(orig, ptr, i+6 , i+8 );
        swap_key(orig, ptr, i+10, i+12);
        swap_key(orig, ptr, i+3 , i+5 );
        swap_key(orig, ptr, i+7 , i+9 );
        swap_key(orig, ptr, i+3 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+7 , i+8 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+12);
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
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
typename std::enable_if<std::is_same<T, int>::value>::type
sorter_key(T*& orig, long *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m512i vec0 ;
    __m512i vec1 ;
    __m512i vec2 ;
    __m512i vec3 ;
    __m512i vec4 ;
    __m512i vec5 ;
    __m512i vec6 ;
    __m512i vec7 ;
    __m512i vec8 ;
    __m512i vec9 ;
    __m512i vec10;
    __m512i vec11;
    __m512i vec12;
    __m512i vec13;
    __m512i vec14;
    __m512i vec15;

    // p* holds the index vectors (16 64-bit indices)
    __m512i pl0 ;
    __m512i pl1 ;
    __m512i pl2 ;
    __m512i pl3 ;
    __m512i pl4 ;
    __m512i pl5 ;
    __m512i pl6 ;
    __m512i pl7 ;
    __m512i pl8 ;
    __m512i pl9 ;
    __m512i pl10;
    __m512i pl11;
    __m512i pl12;
    __m512i pl13;
    __m512i pl14;
    __m512i pl15;

    __m512i ph0 ;
    __m512i ph1 ;
    __m512i ph2 ;
    __m512i ph3 ;
    __m512i ph4 ;
    __m512i ph5 ;
    __m512i ph6 ;
    __m512i ph7 ;
    __m512i ph8 ;
    __m512i ph9 ;
    __m512i ph10;
    __m512i ph11;
    __m512i ph12;
    __m512i ph13;
    __m512i ph14;
    __m512i ph15;

    uint8_t stride = (uint8_t)simd_width::AVX512_INT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_epi32(vec0 , orig + i + 0  * stride     );
        vec0  = _mm512_loadunpackhi_epi32(vec0 , orig + i + 0  * stride + 16);
        vec1  = _mm512_loadunpacklo_epi32(vec1 , orig + i + 1  * stride     );
        vec1  = _mm512_loadunpackhi_epi32(vec1 , orig + i + 1  * stride + 16);
        vec2  = _mm512_loadunpacklo_epi32(vec2 , orig + i + 2  * stride     );
        vec2  = _mm512_loadunpackhi_epi32(vec2 , orig + i + 2  * stride + 16);
        vec3  = _mm512_loadunpacklo_epi32(vec3 , orig + i + 3  * stride     );
        vec3  = _mm512_loadunpackhi_epi32(vec3 , orig + i + 3  * stride + 16);
        vec4  = _mm512_loadunpacklo_epi32(vec4 , orig + i + 4  * stride     );
        vec4  = _mm512_loadunpackhi_epi32(vec4 , orig + i + 4  * stride + 16);
        vec5  = _mm512_loadunpacklo_epi32(vec5 , orig + i + 5  * stride     );
        vec5  = _mm512_loadunpackhi_epi32(vec5 , orig + i + 5  * stride + 16);
        vec6  = _mm512_loadunpacklo_epi32(vec6 , orig + i + 6  * stride     );
        vec6  = _mm512_loadunpackhi_epi32(vec6 , orig + i + 6  * stride + 16);
        vec7  = _mm512_loadunpacklo_epi32(vec7 , orig + i + 7  * stride     );
        vec7  = _mm512_loadunpackhi_epi32(vec7 , orig + i + 7  * stride + 16);
        vec8  = _mm512_loadunpacklo_epi32(vec8 , orig + i + 8  * stride     );
        vec8  = _mm512_loadunpackhi_epi32(vec8 , orig + i + 8  * stride + 16);
        vec9  = _mm512_loadunpacklo_epi32(vec9 , orig + i + 9  * stride     );
        vec9  = _mm512_loadunpackhi_epi32(vec9 , orig + i + 9  * stride + 16);
        vec10 = _mm512_loadunpacklo_epi32(vec10, orig + i + 10 * stride     );
        vec10 = _mm512_loadunpackhi_epi32(vec10, orig + i + 10 * stride + 16);
        vec11 = _mm512_loadunpacklo_epi32(vec11, orig + i + 11 * stride     );
        vec11 = _mm512_loadunpackhi_epi32(vec11, orig + i + 11 * stride + 16);
        vec12 = _mm512_loadunpacklo_epi32(vec12, orig + i + 12 * stride     );
        vec12 = _mm512_loadunpackhi_epi32(vec12, orig + i + 12 * stride + 16);
        vec13 = _mm512_loadunpacklo_epi32(vec13, orig + i + 13 * stride     );
        vec13 = _mm512_loadunpackhi_epi32(vec13, orig + i + 13 * stride + 16);
        vec14 = _mm512_loadunpacklo_epi32(vec14, orig + i + 14 * stride     );
        vec14 = _mm512_loadunpackhi_epi32(vec14, orig + i + 14 * stride + 16);
        vec15 = _mm512_loadunpacklo_epi32(vec15, orig + i + 15 * stride     );
        vec15 = _mm512_loadunpackhi_epi32(vec15, orig + i + 15 * stride + 16);

        pl0  = _mm512_loadunpacklo_epi64(pl0 , ptr + i + 0  * stride     );
        pl0  = _mm512_loadunpackhi_epi64(pl0 , ptr + i + 0  * stride + 8 );
        ph0  = _mm512_loadunpacklo_epi64(ph0 , ptr + i + 0  * stride + 8 );
        ph0  = _mm512_loadunpackhi_epi64(ph0 , ptr + i + 0  * stride + 16);
        pl1  = _mm512_loadunpacklo_epi64(pl1 , ptr + i + 1  * stride     );
        pl1  = _mm512_loadunpackhi_epi64(pl1 , ptr + i + 1  * stride + 8 );
        ph1  = _mm512_loadunpacklo_epi64(ph1 , ptr + i + 1  * stride + 8 );
        ph1  = _mm512_loadunpackhi_epi64(ph1 , ptr + i + 1  * stride + 16);
        pl2  = _mm512_loadunpacklo_epi64(pl2 , ptr + i + 2  * stride     );
        pl2  = _mm512_loadunpackhi_epi64(pl2 , ptr + i + 2  * stride + 8 );
        ph2  = _mm512_loadunpacklo_epi64(ph2 , ptr + i + 2  * stride + 8 );
        ph2  = _mm512_loadunpackhi_epi64(ph2 , ptr + i + 2  * stride + 16);
        pl3  = _mm512_loadunpacklo_epi64(pl3 , ptr + i + 3  * stride     );
        pl3  = _mm512_loadunpackhi_epi64(pl3 , ptr + i + 3  * stride + 8 );
        ph3  = _mm512_loadunpacklo_epi64(ph3 , ptr + i + 3  * stride + 8 );
        ph3  = _mm512_loadunpackhi_epi64(ph3 , ptr + i + 3  * stride + 16);
        pl4  = _mm512_loadunpacklo_epi64(pl4 , ptr + i + 4  * stride     );
        pl4  = _mm512_loadunpackhi_epi64(pl4 , ptr + i + 4  * stride + 8 );
        ph4  = _mm512_loadunpacklo_epi64(ph4 , ptr + i + 4  * stride + 8 );
        ph4  = _mm512_loadunpackhi_epi64(ph4 , ptr + i + 4  * stride + 16);
        pl5  = _mm512_loadunpacklo_epi64(pl5 , ptr + i + 5  * stride     );
        pl5  = _mm512_loadunpackhi_epi64(pl5 , ptr + i + 5  * stride + 8 );
        ph5  = _mm512_loadunpacklo_epi64(ph5 , ptr + i + 5  * stride + 8 );
        ph5  = _mm512_loadunpackhi_epi64(ph5 , ptr + i + 5  * stride + 16);
        pl6  = _mm512_loadunpacklo_epi64(pl6 , ptr + i + 6  * stride     );
        pl6  = _mm512_loadunpackhi_epi64(pl6 , ptr + i + 6  * stride + 8 );
        ph6  = _mm512_loadunpacklo_epi64(ph6 , ptr + i + 6  * stride + 8 );
        ph6  = _mm512_loadunpackhi_epi64(ph6 , ptr + i + 6  * stride + 16);
        pl7  = _mm512_loadunpacklo_epi64(pl7 , ptr + i + 7  * stride     );
        pl7  = _mm512_loadunpackhi_epi64(pl7 , ptr + i + 7  * stride + 8 );
        ph7  = _mm512_loadunpacklo_epi64(ph7 , ptr + i + 7  * stride + 8 );
        ph7  = _mm512_loadunpackhi_epi64(ph7 , ptr + i + 7  * stride + 16);
        pl8  = _mm512_loadunpacklo_epi64(pl8 , ptr + i + 8  * stride     );
        pl8  = _mm512_loadunpackhi_epi64(pl8 , ptr + i + 8  * stride + 8 );
        ph8  = _mm512_loadunpacklo_epi64(ph8 , ptr + i + 8  * stride + 8 );
        ph8  = _mm512_loadunpackhi_epi64(ph8 , ptr + i + 8  * stride + 16);
        pl9  = _mm512_loadunpacklo_epi64(pl9 , ptr + i + 9  * stride     );
        pl9  = _mm512_loadunpackhi_epi64(pl9 , ptr + i + 9  * stride + 8 );
        ph9  = _mm512_loadunpacklo_epi64(ph9 , ptr + i + 9  * stride + 8 );
        ph9  = _mm512_loadunpackhi_epi64(ph9 , ptr + i + 9  * stride + 16);
        pl10 = _mm512_loadunpacklo_epi64(pl10, ptr + i + 10 * stride     );
        pl10 = _mm512_loadunpackhi_epi64(pl10, ptr + i + 10 * stride + 8 );
        ph10 = _mm512_loadunpacklo_epi64(ph10, ptr + i + 10 * stride + 8 );
        ph10 = _mm512_loadunpackhi_epi64(ph10, ptr + i + 10 * stride + 16);
        pl11 = _mm512_loadunpacklo_epi64(pl11, ptr + i + 11 * stride     );
        pl11 = _mm512_loadunpackhi_epi64(pl11, ptr + i + 11 * stride + 8 );
        ph11 = _mm512_loadunpacklo_epi64(ph11, ptr + i + 11 * stride + 8 );
        ph11 = _mm512_loadunpackhi_epi64(ph11, ptr + i + 11 * stride + 16);
        pl12 = _mm512_loadunpacklo_epi64(pl12, ptr + i + 12 * stride     );
        pl12 = _mm512_loadunpackhi_epi64(pl12, ptr + i + 12 * stride + 8 );
        ph12 = _mm512_loadunpacklo_epi64(ph12, ptr + i + 12 * stride + 8 );
        ph12 = _mm512_loadunpackhi_epi64(ph12, ptr + i + 12 * stride + 16);
        pl13 = _mm512_loadunpacklo_epi64(pl13, ptr + i + 13 * stride     );
        pl13 = _mm512_loadunpackhi_epi64(pl13, ptr + i + 13 * stride + 8 );
        ph13 = _mm512_loadunpacklo_epi64(ph13, ptr + i + 13 * stride + 8 );
        ph13 = _mm512_loadunpackhi_epi64(ph13, ptr + i + 13 * stride + 16);
        pl14 = _mm512_loadunpacklo_epi64(pl14, ptr + i + 14 * stride     );
        pl14 = _mm512_loadunpackhi_epi64(pl14, ptr + i + 14 * stride + 8 );
        ph14 = _mm512_loadunpacklo_epi64(ph14, ptr + i + 14 * stride + 8 );
        ph14 = _mm512_loadunpackhi_epi64(ph14, ptr + i + 14 * stride + 16);
        pl15 = _mm512_loadunpacklo_epi64(pl15, ptr + i + 15 * stride     );
        pl15 = _mm512_loadunpackhi_epi64(pl15, ptr + i + 15 * stride + 8 );
        ph15 = _mm512_loadunpacklo_epi64(ph15, ptr + i + 15 * stride + 8 );
        ph15 = _mm512_loadunpackhi_epi64(ph15, ptr + i + 15 * stride + 16);

        // std::cout << "fa" << std::endl;
        in_register_sort_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            pl0, pl1, pl2, pl3, pl4, pl5, pl6, pl7, pl8, pl9, pl10, pl11, pl12, pl13, pl14, pl15,
            ph0, ph1, ph2, ph3, ph4, ph5, ph6, ph7, ph8, ph9, ph10, ph11, ph12, ph13, ph14, ph15);

        in_register_transpose_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            pl0, pl1, pl2, pl3, pl4, pl5, pl6, pl7, pl8, pl9, pl10, pl11, pl12, pl13, pl14, pl15,
            ph0, ph1, ph2, ph3, ph4, ph5, ph6, ph7, ph8, ph9, ph10, ph11, ph12, ph13, ph14, ph15);

        _mm512_packstorelo_epi32(orig + i + 0  * stride     , vec0 );
        _mm512_packstorehi_epi32(orig + i + 0  * stride + 16, vec0 );
        _mm512_packstorelo_epi32(orig + i + 1  * stride     , vec1 );
        _mm512_packstorehi_epi32(orig + i + 1  * stride + 16, vec1 );
        _mm512_packstorelo_epi32(orig + i + 2  * stride     , vec2 );
        _mm512_packstorehi_epi32(orig + i + 2  * stride + 16, vec2 );
        _mm512_packstorelo_epi32(orig + i + 3  * stride     , vec3 );
        _mm512_packstorehi_epi32(orig + i + 3  * stride + 16, vec3 );
        _mm512_packstorelo_epi32(orig + i + 4  * stride     , vec4 );
        _mm512_packstorehi_epi32(orig + i + 4  * stride + 16, vec4 );
        _mm512_packstorelo_epi32(orig + i + 5  * stride     , vec5 );
        _mm512_packstorehi_epi32(orig + i + 5  * stride + 16, vec5 );
        _mm512_packstorelo_epi32(orig + i + 6  * stride     , vec6 );
        _mm512_packstorehi_epi32(orig + i + 6  * stride + 16, vec6 );
        _mm512_packstorelo_epi32(orig + i + 7  * stride     , vec7 );
        _mm512_packstorehi_epi32(orig + i + 7  * stride + 16, vec7 );
        _mm512_packstorelo_epi32(orig + i + 8  * stride     , vec8 );
        _mm512_packstorehi_epi32(orig + i + 8  * stride + 16, vec8 );
        _mm512_packstorelo_epi32(orig + i + 9  * stride     , vec9 );
        _mm512_packstorehi_epi32(orig + i + 9  * stride + 16, vec9 );
        _mm512_packstorelo_epi32(orig + i + 10 * stride     , vec10);
        _mm512_packstorehi_epi32(orig + i + 10 * stride + 16, vec10);
        _mm512_packstorelo_epi32(orig + i + 11 * stride     , vec11);
        _mm512_packstorehi_epi32(orig + i + 11 * stride + 16, vec11);
        _mm512_packstorelo_epi32(orig + i + 12 * stride     , vec12);
        _mm512_packstorehi_epi32(orig + i + 12 * stride + 16, vec12);
        _mm512_packstorelo_epi32(orig + i + 13 * stride     , vec13);
        _mm512_packstorehi_epi32(orig + i + 13 * stride + 16, vec13);
        _mm512_packstorelo_epi32(orig + i + 14 * stride     , vec14);
        _mm512_packstorehi_epi32(orig + i + 14 * stride + 16, vec14);
        _mm512_packstorelo_epi32(orig + i + 15 * stride     , vec15);
        _mm512_packstorehi_epi32(orig + i + 15 * stride + 16, vec15);

        _mm512_packstorelo_epi32(ptr + i + 0  * stride     , pl0 );
        _mm512_packstorehi_epi32(ptr + i + 0  * stride + 8 , pl0 );
        _mm512_packstorelo_epi32(ptr + i + 0  * stride + 8 , ph0 );
        _mm512_packstorehi_epi32(ptr + i + 0  * stride + 16, ph0 );
        _mm512_packstorelo_epi32(ptr + i + 1  * stride     , pl1 );
        _mm512_packstorehi_epi32(ptr + i + 1  * stride + 8 , pl1 );
        _mm512_packstorelo_epi32(ptr + i + 1  * stride + 8 , ph1 );
        _mm512_packstorehi_epi32(ptr + i + 1  * stride + 16, ph1 );
        _mm512_packstorelo_epi32(ptr + i + 2  * stride     , pl2 );
        _mm512_packstorehi_epi32(ptr + i + 2  * stride + 8 , pl2 );
        _mm512_packstorelo_epi32(ptr + i + 2  * stride + 8 , ph2 );
        _mm512_packstorehi_epi32(ptr + i + 2  * stride + 16, ph2 );
        _mm512_packstorelo_epi32(ptr + i + 3  * stride     , pl3 );
        _mm512_packstorehi_epi32(ptr + i + 3  * stride + 8 , pl3 );
        _mm512_packstorelo_epi32(ptr + i + 3  * stride + 8 , ph3 );
        _mm512_packstorehi_epi32(ptr + i + 3  * stride + 16, ph3 );
        _mm512_packstorelo_epi32(ptr + i + 4  * stride     , pl4 );
        _mm512_packstorehi_epi32(ptr + i + 4  * stride + 8 , pl4 );
        _mm512_packstorelo_epi32(ptr + i + 4  * stride + 8 , ph4 );
        _mm512_packstorehi_epi32(ptr + i + 4  * stride + 16, ph4 );
        _mm512_packstorelo_epi32(ptr + i + 5  * stride     , pl5 );
        _mm512_packstorehi_epi32(ptr + i + 5  * stride + 8 , pl5 );
        _mm512_packstorelo_epi32(ptr + i + 5  * stride + 8 , ph5 );
        _mm512_packstorehi_epi32(ptr + i + 5  * stride + 16, ph5 );
        _mm512_packstorelo_epi32(ptr + i + 6  * stride     , pl6 );
        _mm512_packstorehi_epi32(ptr + i + 6  * stride + 8 , pl6 );
        _mm512_packstorelo_epi32(ptr + i + 6  * stride + 8 , ph6 );
        _mm512_packstorehi_epi32(ptr + i + 6  * stride + 16, ph6 );
        _mm512_packstorelo_epi32(ptr + i + 7  * stride     , pl7 );
        _mm512_packstorehi_epi32(ptr + i + 7  * stride + 8 , pl7 );
        _mm512_packstorelo_epi32(ptr + i + 7  * stride + 8 , ph7 );
        _mm512_packstorehi_epi32(ptr + i + 7  * stride + 16, ph7 );
        _mm512_packstorelo_epi32(ptr + i + 8  * stride     , pl8 );
        _mm512_packstorehi_epi32(ptr + i + 8  * stride + 8 , pl8 );
        _mm512_packstorelo_epi32(ptr + i + 8  * stride + 8 , ph8 );
        _mm512_packstorehi_epi32(ptr + i + 8  * stride + 16, ph8 );
        _mm512_packstorelo_epi32(ptr + i + 9  * stride     , pl9 );
        _mm512_packstorehi_epi32(ptr + i + 9  * stride + 8 , pl9 );
        _mm512_packstorelo_epi32(ptr + i + 9  * stride + 8 , ph9 );
        _mm512_packstorehi_epi32(ptr + i + 9  * stride + 16, ph9 );
        _mm512_packstorelo_epi32(ptr + i + 10 * stride     , pl10);
        _mm512_packstorehi_epi32(ptr + i + 10 * stride + 8 , pl10);
        _mm512_packstorelo_epi32(ptr + i + 10 * stride + 8 , ph10);
        _mm512_packstorehi_epi32(ptr + i + 10 * stride + 16, ph10);
        _mm512_packstorelo_epi32(ptr + i + 11 * stride     , pl11);
        _mm512_packstorehi_epi32(ptr + i + 11 * stride + 8 , pl11);
        _mm512_packstorelo_epi32(ptr + i + 11 * stride + 8 , ph11);
        _mm512_packstorehi_epi32(ptr + i + 11 * stride + 16, ph11);
        _mm512_packstorelo_epi32(ptr + i + 12 * stride     , pl12);
        _mm512_packstorehi_epi32(ptr + i + 12 * stride + 8 , pl12);
        _mm512_packstorelo_epi32(ptr + i + 12 * stride + 8 , ph12);
        _mm512_packstorehi_epi32(ptr + i + 12 * stride + 16, ph12);
        _mm512_packstorelo_epi32(ptr + i + 13 * stride     , pl13);
        _mm512_packstorehi_epi32(ptr + i + 13 * stride + 8 , pl13);
        _mm512_packstorelo_epi32(ptr + i + 13 * stride + 8 , ph13);
        _mm512_packstorehi_epi32(ptr + i + 13 * stride + 16, ph13);
        _mm512_packstorelo_epi32(ptr + i + 14 * stride     , pl14);
        _mm512_packstorehi_epi32(ptr + i + 14 * stride + 8 , pl14);
        _mm512_packstorelo_epi32(ptr + i + 14 * stride + 8 , ph14);
        _mm512_packstorehi_epi32(ptr + i + 14 * stride + 16, ph14);
        _mm512_packstorelo_epi32(ptr + i + 15 * stride     , pl15);
        _mm512_packstorehi_epi32(ptr + i + 15 * stride + 8 , pl15);
        _mm512_packstorelo_epi32(ptr + i + 15 * stride + 8 , ph15);
        _mm512_packstorehi_epi32(ptr + i + 15 * stride + 16, ph15);
    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i+0 , i+1 );
        swap_key(orig, ptr, i+2 , i+3 );
        swap_key(orig, ptr, i+4 , i+5 );
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
        swap_key(orig, ptr, i+10, i+11);
        swap_key(orig, ptr, i+12, i+13);
        swap_key(orig, ptr, i+14, i+15);
        swap_key(orig, ptr, i+0 , i+2 );
        swap_key(orig, ptr, i+4 , i+6 );
        swap_key(orig, ptr, i+8 , i+10);
        swap_key(orig, ptr, i+12, i+14);
        swap_key(orig, ptr, i+1 , i+3 );
        swap_key(orig, ptr, i+5 , i+7 );
        swap_key(orig, ptr, i+9 , i+11);
        swap_key(orig, ptr, i+13, i+15);
        swap_key(orig, ptr, i+0 , i+4 );
        swap_key(orig, ptr, i+8 , i+12);
        swap_key(orig, ptr, i+1 , i+5 );
        swap_key(orig, ptr, i+9 , i+13);
        swap_key(orig, ptr, i+2 , i+6 );
        swap_key(orig, ptr, i+10, i+14);
        swap_key(orig, ptr, i+3 , i+7 );
        swap_key(orig, ptr, i+11, i+15);
        swap_key(orig, ptr, i+0 , i+8 );
        swap_key(orig, ptr, i+1 , i+9 );
        swap_key(orig, ptr, i+2 , i+10);
        swap_key(orig, ptr, i+3 , i+11);
        swap_key(orig, ptr, i+4 , i+12);
        swap_key(orig, ptr, i+5 , i+13);
        swap_key(orig, ptr, i+6 , i+14);
        swap_key(orig, ptr, i+7 , i+15);
        swap_key(orig, ptr, i+5 , i+10);
        swap_key(orig, ptr, i+6 , i+9 );
        swap_key(orig, ptr, i+3 , i+12);
        swap_key(orig, ptr, i+13, i+14);
        swap_key(orig, ptr, i+7 , i+11);
        swap_key(orig, ptr, i+1 , i+2 );
        swap_key(orig, ptr, i+4 , i+8 );
        swap_key(orig, ptr, i+1 , i+4 );
        swap_key(orig, ptr, i+7 , i+13);
        swap_key(orig, ptr, i+2 , i+8 );
        swap_key(orig, ptr, i+11, i+14);
        swap_key(orig, ptr, i+2 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+13);
        swap_key(orig, ptr, i+3 , i+8 );
        swap_key(orig, ptr, i+7 , i+12);
        swap_key(orig, ptr, i+6 , i+8 );
        swap_key(orig, ptr, i+10, i+12);
        swap_key(orig, ptr, i+3 , i+5 );
        swap_key(orig, ptr, i+7 , i+9 );
        swap_key(orig, ptr, i+3 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+7 , i+8 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+12);
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
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
sorter_key(T*& orig, int *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m512 vec0 ;
    __m512 vec1 ;
    __m512 vec2 ;
    __m512 vec3 ;
    __m512 vec4 ;
    __m512 vec5 ;
    __m512 vec6 ;
    __m512 vec7 ;
    __m512 vec8 ;
    __m512 vec9 ;
    __m512 vec10;
    __m512 vec11;
    __m512 vec12;
    __m512 vec13;
    __m512 vec14;
    __m512 vec15;

    // p* holds the index vectors (16 32-bit indices)
    __m512i p0 ;
    __m512i p1 ;
    __m512i p2 ;
    __m512i p3 ;
    __m512i p4 ;
    __m512i p5 ;
    __m512i p6 ;
    __m512i p7 ;
    __m512i p8 ;
    __m512i p9 ;
    __m512i p10;
    __m512i p11;
    __m512i p12;
    __m512i p13;
    __m512i p14;
    __m512i p15;

    uint8_t stride = (uint8_t)simd_width::AVX512_FLOAT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_ps(vec0 , orig + i + 0  * stride     );
        vec0  = _mm512_loadunpackhi_ps(vec0 , orig + i + 0  * stride + 16);
        vec1  = _mm512_loadunpacklo_ps(vec1 , orig + i + 1  * stride     );
        vec1  = _mm512_loadunpackhi_ps(vec1 , orig + i + 1  * stride + 16);
        vec2  = _mm512_loadunpacklo_ps(vec2 , orig + i + 2  * stride     );
        vec2  = _mm512_loadunpackhi_ps(vec2 , orig + i + 2  * stride + 16);
        vec3  = _mm512_loadunpacklo_ps(vec3 , orig + i + 3  * stride     );
        vec3  = _mm512_loadunpackhi_ps(vec3 , orig + i + 3  * stride + 16);
        vec4  = _mm512_loadunpacklo_ps(vec4 , orig + i + 4  * stride     );
        vec4  = _mm512_loadunpackhi_ps(vec4 , orig + i + 4  * stride + 16);
        vec5  = _mm512_loadunpacklo_ps(vec5 , orig + i + 5  * stride     );
        vec5  = _mm512_loadunpackhi_ps(vec5 , orig + i + 5  * stride + 16);
        vec6  = _mm512_loadunpacklo_ps(vec6 , orig + i + 6  * stride     );
        vec6  = _mm512_loadunpackhi_ps(vec6 , orig + i + 6  * stride + 16);
        vec7  = _mm512_loadunpacklo_ps(vec7 , orig + i + 7  * stride     );
        vec7  = _mm512_loadunpackhi_ps(vec7 , orig + i + 7  * stride + 16);
        vec8  = _mm512_loadunpacklo_ps(vec8 , orig + i + 8  * stride     );
        vec8  = _mm512_loadunpackhi_ps(vec8 , orig + i + 8  * stride + 16);
        vec9  = _mm512_loadunpacklo_ps(vec9 , orig + i + 9  * stride     );
        vec9  = _mm512_loadunpackhi_ps(vec9 , orig + i + 9  * stride + 16);
        vec10 = _mm512_loadunpacklo_ps(vec10, orig + i + 10 * stride     );
        vec10 = _mm512_loadunpackhi_ps(vec10, orig + i + 10 * stride + 16);
        vec11 = _mm512_loadunpacklo_ps(vec11, orig + i + 11 * stride     );
        vec11 = _mm512_loadunpackhi_ps(vec11, orig + i + 11 * stride + 16);
        vec12 = _mm512_loadunpacklo_ps(vec12, orig + i + 12 * stride     );
        vec12 = _mm512_loadunpackhi_ps(vec12, orig + i + 12 * stride + 16);
        vec13 = _mm512_loadunpacklo_ps(vec13, orig + i + 13 * stride     );
        vec13 = _mm512_loadunpackhi_ps(vec13, orig + i + 13 * stride + 16);
        vec14 = _mm512_loadunpacklo_ps(vec14, orig + i + 14 * stride     );
        vec14 = _mm512_loadunpackhi_ps(vec14, orig + i + 14 * stride + 16);
        vec15 = _mm512_loadunpacklo_ps(vec15, orig + i + 15 * stride     );
        vec15 = _mm512_loadunpackhi_ps(vec15, orig + i + 15 * stride + 16);

        p0  = _mm512_loadunpacklo_epi32(p0 , ptr + i + 0  * stride     );
        p0  = _mm512_loadunpackhi_epi32(p0 , ptr + i + 0  * stride + 16);
        p1  = _mm512_loadunpacklo_epi32(p1 , ptr + i + 1  * stride     );
        p1  = _mm512_loadunpackhi_epi32(p1 , ptr + i + 1  * stride + 16);
        p2  = _mm512_loadunpacklo_epi32(p2 , ptr + i + 2  * stride     );
        p2  = _mm512_loadunpackhi_epi32(p2 , ptr + i + 2  * stride + 16);
        p3  = _mm512_loadunpacklo_epi32(p3 , ptr + i + 3  * stride     );
        p3  = _mm512_loadunpackhi_epi32(p3 , ptr + i + 3  * stride + 16);
        p4  = _mm512_loadunpacklo_epi32(p4 , ptr + i + 4  * stride     );
        p4  = _mm512_loadunpackhi_epi32(p4 , ptr + i + 4  * stride + 16);
        p5  = _mm512_loadunpacklo_epi32(p5 , ptr + i + 5  * stride     );
        p5  = _mm512_loadunpackhi_epi32(p5 , ptr + i + 5  * stride + 16);
        p6  = _mm512_loadunpacklo_epi32(p6 , ptr + i + 6  * stride     );
        p6  = _mm512_loadunpackhi_epi32(p6 , ptr + i + 6  * stride + 16);
        p7  = _mm512_loadunpacklo_epi32(p7 , ptr + i + 7  * stride     );
        p7  = _mm512_loadunpackhi_epi32(p7 , ptr + i + 7  * stride + 16);
        p8  = _mm512_loadunpacklo_epi32(p8 , ptr + i + 8  * stride     );
        p8  = _mm512_loadunpackhi_epi32(p8 , ptr + i + 8  * stride + 16);
        p9  = _mm512_loadunpacklo_epi32(p9 , ptr + i + 9  * stride     );
        p9  = _mm512_loadunpackhi_epi32(p9 , ptr + i + 9  * stride + 16);
        p10 = _mm512_loadunpacklo_epi32(p10, ptr + i + 10 * stride     );
        p10 = _mm512_loadunpackhi_epi32(p10, ptr + i + 10 * stride + 16);
        p11 = _mm512_loadunpacklo_epi32(p11, ptr + i + 11 * stride     );
        p11 = _mm512_loadunpackhi_epi32(p11, ptr + i + 11 * stride + 16);
        p12 = _mm512_loadunpacklo_epi32(p12, ptr + i + 12 * stride     );
        p12 = _mm512_loadunpackhi_epi32(p12, ptr + i + 12 * stride + 16);
        p13 = _mm512_loadunpacklo_epi32(p13, ptr + i + 13 * stride     );
        p13 = _mm512_loadunpackhi_epi32(p13, ptr + i + 13 * stride + 16);
        p14 = _mm512_loadunpacklo_epi32(p14, ptr + i + 14 * stride     );
        p14 = _mm512_loadunpackhi_epi32(p14, ptr + i + 14 * stride + 16);
        p15 = _mm512_loadunpacklo_epi32(p15, ptr + i + 15 * stride     );
        p15 = _mm512_loadunpackhi_epi32(p15, ptr + i + 15 * stride + 16);

        in_register_sort_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);

        in_register_transpose_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);

        _mm512_packstorelo_ps(orig + i + 0  * stride     , vec0 );
        _mm512_packstorehi_ps(orig + i + 0  * stride + 16, vec0 );
        _mm512_packstorelo_ps(orig + i + 1  * stride     , vec1 );
        _mm512_packstorehi_ps(orig + i + 1  * stride + 16, vec1 );
        _mm512_packstorelo_ps(orig + i + 2  * stride     , vec2 );
        _mm512_packstorehi_ps(orig + i + 2  * stride + 16, vec2 );
        _mm512_packstorelo_ps(orig + i + 3  * stride     , vec3 );
        _mm512_packstorehi_ps(orig + i + 3  * stride + 16, vec3 );
        _mm512_packstorelo_ps(orig + i + 4  * stride     , vec4 );
        _mm512_packstorehi_ps(orig + i + 4  * stride + 16, vec4 );
        _mm512_packstorelo_ps(orig + i + 5  * stride     , vec5 );
        _mm512_packstorehi_ps(orig + i + 5  * stride + 16, vec5 );
        _mm512_packstorelo_ps(orig + i + 6  * stride     , vec6 );
        _mm512_packstorehi_ps(orig + i + 6  * stride + 16, vec6 );
        _mm512_packstorelo_ps(orig + i + 7  * stride     , vec7 );
        _mm512_packstorehi_ps(orig + i + 7  * stride + 16, vec7 );
        _mm512_packstorelo_ps(orig + i + 8  * stride     , vec8 );
        _mm512_packstorehi_ps(orig + i + 8  * stride + 16, vec8 );
        _mm512_packstorelo_ps(orig + i + 9  * stride     , vec9 );
        _mm512_packstorehi_ps(orig + i + 9  * stride + 16, vec9 );
        _mm512_packstorelo_ps(orig + i + 10 * stride     , vec10);
        _mm512_packstorehi_ps(orig + i + 10 * stride + 16, vec10);
        _mm512_packstorelo_ps(orig + i + 11 * stride     , vec11);
        _mm512_packstorehi_ps(orig + i + 11 * stride + 16, vec11);
        _mm512_packstorelo_ps(orig + i + 12 * stride     , vec12);
        _mm512_packstorehi_ps(orig + i + 12 * stride + 16, vec12);
        _mm512_packstorelo_ps(orig + i + 13 * stride     , vec13);
        _mm512_packstorehi_ps(orig + i + 13 * stride + 16, vec13);
        _mm512_packstorelo_ps(orig + i + 14 * stride     , vec14);
        _mm512_packstorehi_ps(orig + i + 14 * stride + 16, vec14);
        _mm512_packstorelo_ps(orig + i + 15 * stride     , vec15);
        _mm512_packstorehi_ps(orig + i + 15 * stride + 16, vec15);

        _mm512_packstorelo_epi32(ptr + i + 0  * stride     , p0 );
        _mm512_packstorehi_epi32(ptr + i + 0  * stride + 16, p0 );
        _mm512_packstorelo_epi32(ptr + i + 1  * stride     , p1 );
        _mm512_packstorehi_epi32(ptr + i + 1  * stride + 16, p1 );
        _mm512_packstorelo_epi32(ptr + i + 2  * stride     , p2 );
        _mm512_packstorehi_epi32(ptr + i + 2  * stride + 16, p2 );
        _mm512_packstorelo_epi32(ptr + i + 3  * stride     , p3 );
        _mm512_packstorehi_epi32(ptr + i + 3  * stride + 16, p3 );
        _mm512_packstorelo_epi32(ptr + i + 4  * stride     , p4 );
        _mm512_packstorehi_epi32(ptr + i + 4  * stride + 16, p4 );
        _mm512_packstorelo_epi32(ptr + i + 5  * stride     , p5 );
        _mm512_packstorehi_epi32(ptr + i + 5  * stride + 16, p5 );
        _mm512_packstorelo_epi32(ptr + i + 6  * stride     , p6 );
        _mm512_packstorehi_epi32(ptr + i + 6  * stride + 16, p6 );
        _mm512_packstorelo_epi32(ptr + i + 7  * stride     , p7 );
        _mm512_packstorehi_epi32(ptr + i + 7  * stride + 16, p7 );
        _mm512_packstorelo_epi32(ptr + i + 8  * stride     , p8 );
        _mm512_packstorehi_epi32(ptr + i + 8  * stride + 16, p8 );
        _mm512_packstorelo_epi32(ptr + i + 9  * stride     , p9 );
        _mm512_packstorehi_epi32(ptr + i + 9  * stride + 16, p9 );
        _mm512_packstorelo_epi32(ptr + i + 10 * stride     , p10);
        _mm512_packstorehi_epi32(ptr + i + 10 * stride + 16, p10);
        _mm512_packstorelo_epi32(ptr + i + 11 * stride     , p11);
        _mm512_packstorehi_epi32(ptr + i + 11 * stride + 16, p11);
        _mm512_packstorelo_epi32(ptr + i + 12 * stride     , p12);
        _mm512_packstorehi_epi32(ptr + i + 12 * stride + 16, p12);
        _mm512_packstorelo_epi32(ptr + i + 13 * stride     , p13);
        _mm512_packstorehi_epi32(ptr + i + 13 * stride + 16, p13);
        _mm512_packstorelo_epi32(ptr + i + 14 * stride     , p14);
        _mm512_packstorehi_epi32(ptr + i + 14 * stride + 16, p14);
        _mm512_packstorelo_epi32(ptr + i + 15 * stride     , p15);
        _mm512_packstorehi_epi32(ptr + i + 15 * stride + 16, p15);
    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i+0 , i+1 );
        swap_key(orig, ptr, i+2 , i+3 );
        swap_key(orig, ptr, i+4 , i+5 );
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
        swap_key(orig, ptr, i+10, i+11);
        swap_key(orig, ptr, i+12, i+13);
        swap_key(orig, ptr, i+14, i+15);
        swap_key(orig, ptr, i+0 , i+2 );
        swap_key(orig, ptr, i+4 , i+6 );
        swap_key(orig, ptr, i+8 , i+10);
        swap_key(orig, ptr, i+12, i+14);
        swap_key(orig, ptr, i+1 , i+3 );
        swap_key(orig, ptr, i+5 , i+7 );
        swap_key(orig, ptr, i+9 , i+11);
        swap_key(orig, ptr, i+13, i+15);
        swap_key(orig, ptr, i+0 , i+4 );
        swap_key(orig, ptr, i+8 , i+12);
        swap_key(orig, ptr, i+1 , i+5 );
        swap_key(orig, ptr, i+9 , i+13);
        swap_key(orig, ptr, i+2 , i+6 );
        swap_key(orig, ptr, i+10, i+14);
        swap_key(orig, ptr, i+3 , i+7 );
        swap_key(orig, ptr, i+11, i+15);
        swap_key(orig, ptr, i+0 , i+8 );
        swap_key(orig, ptr, i+1 , i+9 );
        swap_key(orig, ptr, i+2 , i+10);
        swap_key(orig, ptr, i+3 , i+11);
        swap_key(orig, ptr, i+4 , i+12);
        swap_key(orig, ptr, i+5 , i+13);
        swap_key(orig, ptr, i+6 , i+14);
        swap_key(orig, ptr, i+7 , i+15);
        swap_key(orig, ptr, i+5 , i+10);
        swap_key(orig, ptr, i+6 , i+9 );
        swap_key(orig, ptr, i+3 , i+12);
        swap_key(orig, ptr, i+13, i+14);
        swap_key(orig, ptr, i+7 , i+11);
        swap_key(orig, ptr, i+1 , i+2 );
        swap_key(orig, ptr, i+4 , i+8 );
        swap_key(orig, ptr, i+1 , i+4 );
        swap_key(orig, ptr, i+7 , i+13);
        swap_key(orig, ptr, i+2 , i+8 );
        swap_key(orig, ptr, i+11, i+14);
        swap_key(orig, ptr, i+2 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+13);
        swap_key(orig, ptr, i+3 , i+8 );
        swap_key(orig, ptr, i+7 , i+12);
        swap_key(orig, ptr, i+6 , i+8 );
        swap_key(orig, ptr, i+10, i+12);
        swap_key(orig, ptr, i+3 , i+5 );
        swap_key(orig, ptr, i+7 , i+9 );
        swap_key(orig, ptr, i+3 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+7 , i+8 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+12);
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
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
sorter_key(T*& orig, long *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m512 vec0 ;
    __m512 vec1 ;
    __m512 vec2 ;
    __m512 vec3 ;
    __m512 vec4 ;
    __m512 vec5 ;
    __m512 vec6 ;
    __m512 vec7 ;
    __m512 vec8 ;
    __m512 vec9 ;
    __m512 vec10;
    __m512 vec11;
    __m512 vec12;
    __m512 vec13;
    __m512 vec14;
    __m512 vec15;

    // p* holds the index vectors (16 64-bit indices)
    __m512i pl0 ;
    __m512i pl1 ;
    __m512i pl2 ;
    __m512i pl3 ;
    __m512i pl4 ;
    __m512i pl5 ;
    __m512i pl6 ;
    __m512i pl7 ;
    __m512i pl8 ;
    __m512i pl9 ;
    __m512i pl10;
    __m512i pl11;
    __m512i pl12;
    __m512i pl13;
    __m512i pl14;
    __m512i pl15;

    __m512i ph0 ;
    __m512i ph1 ;
    __m512i ph2 ;
    __m512i ph3 ;
    __m512i ph4 ;
    __m512i ph5 ;
    __m512i ph6 ;
    __m512i ph7 ;
    __m512i ph8 ;
    __m512i ph9 ;
    __m512i ph10;
    __m512i ph11;
    __m512i ph12;
    __m512i ph13;
    __m512i ph14;
    __m512i ph15;

    uint8_t stride = (uint8_t)simd_width::AVX512_FLOAT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_ps(vec0 , orig + i + 0  * stride     );
        vec0  = _mm512_loadunpackhi_ps(vec0 , orig + i + 0  * stride + 16);
        vec1  = _mm512_loadunpacklo_ps(vec1 , orig + i + 1  * stride     );
        vec1  = _mm512_loadunpackhi_ps(vec1 , orig + i + 1  * stride + 16);
        vec2  = _mm512_loadunpacklo_ps(vec2 , orig + i + 2  * stride     );
        vec2  = _mm512_loadunpackhi_ps(vec2 , orig + i + 2  * stride + 16);
        vec3  = _mm512_loadunpacklo_ps(vec3 , orig + i + 3  * stride     );
        vec3  = _mm512_loadunpackhi_ps(vec3 , orig + i + 3  * stride + 16);
        vec4  = _mm512_loadunpacklo_ps(vec4 , orig + i + 4  * stride     );
        vec4  = _mm512_loadunpackhi_ps(vec4 , orig + i + 4  * stride + 16);
        vec5  = _mm512_loadunpacklo_ps(vec5 , orig + i + 5  * stride     );
        vec5  = _mm512_loadunpackhi_ps(vec5 , orig + i + 5  * stride + 16);
        vec6  = _mm512_loadunpacklo_ps(vec6 , orig + i + 6  * stride     );
        vec6  = _mm512_loadunpackhi_ps(vec6 , orig + i + 6  * stride + 16);
        vec7  = _mm512_loadunpacklo_ps(vec7 , orig + i + 7  * stride     );
        vec7  = _mm512_loadunpackhi_ps(vec7 , orig + i + 7  * stride + 16);
        vec8  = _mm512_loadunpacklo_ps(vec8 , orig + i + 8  * stride     );
        vec8  = _mm512_loadunpackhi_ps(vec8 , orig + i + 8  * stride + 16);
        vec9  = _mm512_loadunpacklo_ps(vec9 , orig + i + 9  * stride     );
        vec9  = _mm512_loadunpackhi_ps(vec9 , orig + i + 9  * stride + 16);
        vec10 = _mm512_loadunpacklo_ps(vec10, orig + i + 10 * stride     );
        vec10 = _mm512_loadunpackhi_ps(vec10, orig + i + 10 * stride + 16);
        vec11 = _mm512_loadunpacklo_ps(vec11, orig + i + 11 * stride     );
        vec11 = _mm512_loadunpackhi_ps(vec11, orig + i + 11 * stride + 16);
        vec12 = _mm512_loadunpacklo_ps(vec12, orig + i + 12 * stride     );
        vec12 = _mm512_loadunpackhi_ps(vec12, orig + i + 12 * stride + 16);
        vec13 = _mm512_loadunpacklo_ps(vec13, orig + i + 13 * stride     );
        vec13 = _mm512_loadunpackhi_ps(vec13, orig + i + 13 * stride + 16);
        vec14 = _mm512_loadunpacklo_ps(vec14, orig + i + 14 * stride     );
        vec14 = _mm512_loadunpackhi_ps(vec14, orig + i + 14 * stride + 16);
        vec15 = _mm512_loadunpacklo_ps(vec15, orig + i + 15 * stride     );
        vec15 = _mm512_loadunpackhi_ps(vec15, orig + i + 15 * stride + 16);

        pl0  = _mm512_loadunpacklo_epi64(pl0 , ptr + i + 0  * stride     );
        pl0  = _mm512_loadunpackhi_epi64(pl0 , ptr + i + 0  * stride + 8 );
        ph0  = _mm512_loadunpacklo_epi64(ph0 , ptr + i + 0  * stride + 8 );
        ph0  = _mm512_loadunpackhi_epi64(ph0 , ptr + i + 0  * stride + 16);
        pl1  = _mm512_loadunpacklo_epi64(pl1 , ptr + i + 1  * stride     );
        pl1  = _mm512_loadunpackhi_epi64(pl1 , ptr + i + 1  * stride + 8 );
        ph1  = _mm512_loadunpacklo_epi64(ph1 , ptr + i + 1  * stride + 8 );
        ph1  = _mm512_loadunpackhi_epi64(ph1 , ptr + i + 1  * stride + 16);
        pl2  = _mm512_loadunpacklo_epi64(pl2 , ptr + i + 2  * stride     );
        pl2  = _mm512_loadunpackhi_epi64(pl2 , ptr + i + 2  * stride + 8 );
        ph2  = _mm512_loadunpacklo_epi64(ph2 , ptr + i + 2  * stride + 8 );
        ph2  = _mm512_loadunpackhi_epi64(ph2 , ptr + i + 2  * stride + 16);
        pl3  = _mm512_loadunpacklo_epi64(pl3 , ptr + i + 3  * stride     );
        pl3  = _mm512_loadunpackhi_epi64(pl3 , ptr + i + 3  * stride + 8 );
        ph3  = _mm512_loadunpacklo_epi64(ph3 , ptr + i + 3  * stride + 8 );
        ph3  = _mm512_loadunpackhi_epi64(ph3 , ptr + i + 3  * stride + 16);
        pl4  = _mm512_loadunpacklo_epi64(pl4 , ptr + i + 4  * stride     );
        pl4  = _mm512_loadunpackhi_epi64(pl4 , ptr + i + 4  * stride + 8 );
        ph4  = _mm512_loadunpacklo_epi64(ph4 , ptr + i + 4  * stride + 8 );
        ph4  = _mm512_loadunpackhi_epi64(ph4 , ptr + i + 4  * stride + 16);
        pl5  = _mm512_loadunpacklo_epi64(pl5 , ptr + i + 5  * stride     );
        pl5  = _mm512_loadunpackhi_epi64(pl5 , ptr + i + 5  * stride + 8 );
        ph5  = _mm512_loadunpacklo_epi64(ph5 , ptr + i + 5  * stride + 8 );
        ph5  = _mm512_loadunpackhi_epi64(ph5 , ptr + i + 5  * stride + 16);
        pl6  = _mm512_loadunpacklo_epi64(pl6 , ptr + i + 6  * stride     );
        pl6  = _mm512_loadunpackhi_epi64(pl6 , ptr + i + 6  * stride + 8 );
        ph6  = _mm512_loadunpacklo_epi64(ph6 , ptr + i + 6  * stride + 8 );
        ph6  = _mm512_loadunpackhi_epi64(ph6 , ptr + i + 6  * stride + 16);
        pl7  = _mm512_loadunpacklo_epi64(pl7 , ptr + i + 7  * stride     );
        pl7  = _mm512_loadunpackhi_epi64(pl7 , ptr + i + 7  * stride + 8 );
        ph7  = _mm512_loadunpacklo_epi64(ph7 , ptr + i + 7  * stride + 8 );
        ph7  = _mm512_loadunpackhi_epi64(ph7 , ptr + i + 7  * stride + 16);
        pl8  = _mm512_loadunpacklo_epi64(pl8 , ptr + i + 8  * stride     );
        pl8  = _mm512_loadunpackhi_epi64(pl8 , ptr + i + 8  * stride + 8 );
        ph8  = _mm512_loadunpacklo_epi64(ph8 , ptr + i + 8  * stride + 8 );
        ph8  = _mm512_loadunpackhi_epi64(ph8 , ptr + i + 8  * stride + 16);
        pl9  = _mm512_loadunpacklo_epi64(pl9 , ptr + i + 9  * stride     );
        pl9  = _mm512_loadunpackhi_epi64(pl9 , ptr + i + 9  * stride + 8 );
        ph9  = _mm512_loadunpacklo_epi64(ph9 , ptr + i + 9  * stride + 8 );
        ph9  = _mm512_loadunpackhi_epi64(ph9 , ptr + i + 9  * stride + 16);
        pl10 = _mm512_loadunpacklo_epi64(pl10, ptr + i + 10 * stride     );
        pl10 = _mm512_loadunpackhi_epi64(pl10, ptr + i + 10 * stride + 8 );
        ph10 = _mm512_loadunpacklo_epi64(ph10, ptr + i + 10 * stride + 8 );
        ph10 = _mm512_loadunpackhi_epi64(ph10, ptr + i + 10 * stride + 16);
        pl11 = _mm512_loadunpacklo_epi64(pl11, ptr + i + 11 * stride     );
        pl11 = _mm512_loadunpackhi_epi64(pl11, ptr + i + 11 * stride + 8 );
        ph11 = _mm512_loadunpacklo_epi64(ph11, ptr + i + 11 * stride + 8 );
        ph11 = _mm512_loadunpackhi_epi64(ph11, ptr + i + 11 * stride + 16);
        pl12 = _mm512_loadunpacklo_epi64(pl12, ptr + i + 12 * stride     );
        pl12 = _mm512_loadunpackhi_epi64(pl12, ptr + i + 12 * stride + 8 );
        ph12 = _mm512_loadunpacklo_epi64(ph12, ptr + i + 12 * stride + 8 );
        ph12 = _mm512_loadunpackhi_epi64(ph12, ptr + i + 12 * stride + 16);
        pl13 = _mm512_loadunpacklo_epi64(pl13, ptr + i + 13 * stride     );
        pl13 = _mm512_loadunpackhi_epi64(pl13, ptr + i + 13 * stride + 8 );
        ph13 = _mm512_loadunpacklo_epi64(ph13, ptr + i + 13 * stride + 8 );
        ph13 = _mm512_loadunpackhi_epi64(ph13, ptr + i + 13 * stride + 16);
        pl14 = _mm512_loadunpacklo_epi64(pl14, ptr + i + 14 * stride     );
        pl14 = _mm512_loadunpackhi_epi64(pl14, ptr + i + 14 * stride + 8 );
        ph14 = _mm512_loadunpacklo_epi64(ph14, ptr + i + 14 * stride + 8 );
        ph14 = _mm512_loadunpackhi_epi64(ph14, ptr + i + 14 * stride + 16);
        pl15 = _mm512_loadunpacklo_epi64(pl15, ptr + i + 15 * stride     );
        pl15 = _mm512_loadunpackhi_epi64(pl15, ptr + i + 15 * stride + 8 );
        ph15 = _mm512_loadunpacklo_epi64(ph15, ptr + i + 15 * stride + 8 );
        ph15 = _mm512_loadunpackhi_epi64(ph15, ptr + i + 15 * stride + 16);

        in_register_sort_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            pl0, pl1, pl2, pl3, pl4, pl5, pl6, pl7, pl8, pl9, pl10, pl11, pl12, pl13, pl14, pl15,
            ph0, ph1, ph2, ph3, ph4, ph5, ph6, ph7, ph8, ph9, ph10, ph11, ph12, ph13, ph14, ph15);

        in_register_transpose_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15,
            pl0, pl1, pl2, pl3, pl4, pl5, pl6, pl7, pl8, pl9, pl10, pl11, pl12, pl13, pl14, pl15,
            ph0, ph1, ph2, ph3, ph4, ph5, ph6, ph7, ph8, ph9, ph10, ph11, ph12, ph13, ph14, ph15);

        _mm512_packstorelo_ps(orig + i + 0  * stride     , vec0 );
        _mm512_packstorehi_ps(orig + i + 0  * stride + 16, vec0 );
        _mm512_packstorelo_ps(orig + i + 1  * stride     , vec1 );
        _mm512_packstorehi_ps(orig + i + 1  * stride + 16, vec1 );
        _mm512_packstorelo_ps(orig + i + 2  * stride     , vec2 );
        _mm512_packstorehi_ps(orig + i + 2  * stride + 16, vec2 );
        _mm512_packstorelo_ps(orig + i + 3  * stride     , vec3 );
        _mm512_packstorehi_ps(orig + i + 3  * stride + 16, vec3 );
        _mm512_packstorelo_ps(orig + i + 4  * stride     , vec4 );
        _mm512_packstorehi_ps(orig + i + 4  * stride + 16, vec4 );
        _mm512_packstorelo_ps(orig + i + 5  * stride     , vec5 );
        _mm512_packstorehi_ps(orig + i + 5  * stride + 16, vec5 );
        _mm512_packstorelo_ps(orig + i + 6  * stride     , vec6 );
        _mm512_packstorehi_ps(orig + i + 6  * stride + 16, vec6 );
        _mm512_packstorelo_ps(orig + i + 7  * stride     , vec7 );
        _mm512_packstorehi_ps(orig + i + 7  * stride + 16, vec7 );
        _mm512_packstorelo_ps(orig + i + 8  * stride     , vec8 );
        _mm512_packstorehi_ps(orig + i + 8  * stride + 16, vec8 );
        _mm512_packstorelo_ps(orig + i + 9  * stride     , vec9 );
        _mm512_packstorehi_ps(orig + i + 9  * stride + 16, vec9 );
        _mm512_packstorelo_ps(orig + i + 10 * stride     , vec10);
        _mm512_packstorehi_ps(orig + i + 10 * stride + 16, vec10);
        _mm512_packstorelo_ps(orig + i + 11 * stride     , vec11);
        _mm512_packstorehi_ps(orig + i + 11 * stride + 16, vec11);
        _mm512_packstorelo_ps(orig + i + 12 * stride     , vec12);
        _mm512_packstorehi_ps(orig + i + 12 * stride + 16, vec12);
        _mm512_packstorelo_ps(orig + i + 13 * stride     , vec13);
        _mm512_packstorehi_ps(orig + i + 13 * stride + 16, vec13);
        _mm512_packstorelo_ps(orig + i + 14 * stride     , vec14);
        _mm512_packstorehi_ps(orig + i + 14 * stride + 16, vec14);
        _mm512_packstorelo_ps(orig + i + 15 * stride     , vec15);
        _mm512_packstorehi_ps(orig + i + 15 * stride + 16, vec15);

        _mm512_packstorelo_epi32(ptr + i + 0  * stride     , pl0 );
        _mm512_packstorehi_epi32(ptr + i + 0  * stride + 8 , pl0 );
        _mm512_packstorelo_epi32(ptr + i + 0  * stride + 8 , ph0 );
        _mm512_packstorehi_epi32(ptr + i + 0  * stride + 16, ph0 );
        _mm512_packstorelo_epi32(ptr + i + 1  * stride     , pl1 );
        _mm512_packstorehi_epi32(ptr + i + 1  * stride + 8 , pl1 );
        _mm512_packstorelo_epi32(ptr + i + 1  * stride + 8 , ph1 );
        _mm512_packstorehi_epi32(ptr + i + 1  * stride + 16, ph1 );
        _mm512_packstorelo_epi32(ptr + i + 2  * stride     , pl2 );
        _mm512_packstorehi_epi32(ptr + i + 2  * stride + 8 , pl2 );
        _mm512_packstorelo_epi32(ptr + i + 2  * stride + 8 , ph2 );
        _mm512_packstorehi_epi32(ptr + i + 2  * stride + 16, ph2 );
        _mm512_packstorelo_epi32(ptr + i + 3  * stride     , pl3 );
        _mm512_packstorehi_epi32(ptr + i + 3  * stride + 8 , pl3 );
        _mm512_packstorelo_epi32(ptr + i + 3  * stride + 8 , ph3 );
        _mm512_packstorehi_epi32(ptr + i + 3  * stride + 16, ph3 );
        _mm512_packstorelo_epi32(ptr + i + 4  * stride     , pl4 );
        _mm512_packstorehi_epi32(ptr + i + 4  * stride + 8 , pl4 );
        _mm512_packstorelo_epi32(ptr + i + 4  * stride + 8 , ph4 );
        _mm512_packstorehi_epi32(ptr + i + 4  * stride + 16, ph4 );
        _mm512_packstorelo_epi32(ptr + i + 5  * stride     , pl5 );
        _mm512_packstorehi_epi32(ptr + i + 5  * stride + 8 , pl5 );
        _mm512_packstorelo_epi32(ptr + i + 5  * stride + 8 , ph5 );
        _mm512_packstorehi_epi32(ptr + i + 5  * stride + 16, ph5 );
        _mm512_packstorelo_epi32(ptr + i + 6  * stride     , pl6 );
        _mm512_packstorehi_epi32(ptr + i + 6  * stride + 8 , pl6 );
        _mm512_packstorelo_epi32(ptr + i + 6  * stride + 8 , ph6 );
        _mm512_packstorehi_epi32(ptr + i + 6  * stride + 16, ph6 );
        _mm512_packstorelo_epi32(ptr + i + 7  * stride     , pl7 );
        _mm512_packstorehi_epi32(ptr + i + 7  * stride + 8 , pl7 );
        _mm512_packstorelo_epi32(ptr + i + 7  * stride + 8 , ph7 );
        _mm512_packstorehi_epi32(ptr + i + 7  * stride + 16, ph7 );
        _mm512_packstorelo_epi32(ptr + i + 8  * stride     , pl8 );
        _mm512_packstorehi_epi32(ptr + i + 8  * stride + 8 , pl8 );
        _mm512_packstorelo_epi32(ptr + i + 8  * stride + 8 , ph8 );
        _mm512_packstorehi_epi32(ptr + i + 8  * stride + 16, ph8 );
        _mm512_packstorelo_epi32(ptr + i + 9  * stride     , pl9 );
        _mm512_packstorehi_epi32(ptr + i + 9  * stride + 8 , pl9 );
        _mm512_packstorelo_epi32(ptr + i + 9  * stride + 8 , ph9 );
        _mm512_packstorehi_epi32(ptr + i + 9  * stride + 16, ph9 );
        _mm512_packstorelo_epi32(ptr + i + 10 * stride     , pl10);
        _mm512_packstorehi_epi32(ptr + i + 10 * stride + 8 , pl10);
        _mm512_packstorelo_epi32(ptr + i + 10 * stride + 8 , ph10);
        _mm512_packstorehi_epi32(ptr + i + 10 * stride + 16, ph10);
        _mm512_packstorelo_epi32(ptr + i + 11 * stride     , pl11);
        _mm512_packstorehi_epi32(ptr + i + 11 * stride + 8 , pl11);
        _mm512_packstorelo_epi32(ptr + i + 11 * stride + 8 , ph11);
        _mm512_packstorehi_epi32(ptr + i + 11 * stride + 16, ph11);
        _mm512_packstorelo_epi32(ptr + i + 12 * stride     , pl12);
        _mm512_packstorehi_epi32(ptr + i + 12 * stride + 8 , pl12);
        _mm512_packstorelo_epi32(ptr + i + 12 * stride + 8 , ph12);
        _mm512_packstorehi_epi32(ptr + i + 12 * stride + 16, ph12);
        _mm512_packstorelo_epi32(ptr + i + 13 * stride     , pl13);
        _mm512_packstorehi_epi32(ptr + i + 13 * stride + 8 , pl13);
        _mm512_packstorelo_epi32(ptr + i + 13 * stride + 8 , ph13);
        _mm512_packstorehi_epi32(ptr + i + 13 * stride + 16, ph13);
        _mm512_packstorelo_epi32(ptr + i + 14 * stride     , pl14);
        _mm512_packstorehi_epi32(ptr + i + 14 * stride + 8 , pl14);
        _mm512_packstorelo_epi32(ptr + i + 14 * stride + 8 , ph14);
        _mm512_packstorehi_epi32(ptr + i + 14 * stride + 16, ph14);
        _mm512_packstorelo_epi32(ptr + i + 15 * stride     , pl15);
        _mm512_packstorehi_epi32(ptr + i + 15 * stride + 8 , pl15);
        _mm512_packstorelo_epi32(ptr + i + 15 * stride + 8 , ph15);
        _mm512_packstorehi_epi32(ptr + i + 15 * stride + 16, ph15);

    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i+0 , i+1 );
        swap_key(orig, ptr, i+2 , i+3 );
        swap_key(orig, ptr, i+4 , i+5 );
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
        swap_key(orig, ptr, i+10, i+11);
        swap_key(orig, ptr, i+12, i+13);
        swap_key(orig, ptr, i+14, i+15);
        swap_key(orig, ptr, i+0 , i+2 );
        swap_key(orig, ptr, i+4 , i+6 );
        swap_key(orig, ptr, i+8 , i+10);
        swap_key(orig, ptr, i+12, i+14);
        swap_key(orig, ptr, i+1 , i+3 );
        swap_key(orig, ptr, i+5 , i+7 );
        swap_key(orig, ptr, i+9 , i+11);
        swap_key(orig, ptr, i+13, i+15);
        swap_key(orig, ptr, i+0 , i+4 );
        swap_key(orig, ptr, i+8 , i+12);
        swap_key(orig, ptr, i+1 , i+5 );
        swap_key(orig, ptr, i+9 , i+13);
        swap_key(orig, ptr, i+2 , i+6 );
        swap_key(orig, ptr, i+10, i+14);
        swap_key(orig, ptr, i+3 , i+7 );
        swap_key(orig, ptr, i+11, i+15);
        swap_key(orig, ptr, i+0 , i+8 );
        swap_key(orig, ptr, i+1 , i+9 );
        swap_key(orig, ptr, i+2 , i+10);
        swap_key(orig, ptr, i+3 , i+11);
        swap_key(orig, ptr, i+4 , i+12);
        swap_key(orig, ptr, i+5 , i+13);
        swap_key(orig, ptr, i+6 , i+14);
        swap_key(orig, ptr, i+7 , i+15);
        swap_key(orig, ptr, i+5 , i+10);
        swap_key(orig, ptr, i+6 , i+9 );
        swap_key(orig, ptr, i+3 , i+12);
        swap_key(orig, ptr, i+13, i+14);
        swap_key(orig, ptr, i+7 , i+11);
        swap_key(orig, ptr, i+1 , i+2 );
        swap_key(orig, ptr, i+4 , i+8 );
        swap_key(orig, ptr, i+1 , i+4 );
        swap_key(orig, ptr, i+7 , i+13);
        swap_key(orig, ptr, i+2 , i+8 );
        swap_key(orig, ptr, i+11, i+14);
        swap_key(orig, ptr, i+2 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+13);
        swap_key(orig, ptr, i+3 , i+8 );
        swap_key(orig, ptr, i+7 , i+12);
        swap_key(orig, ptr, i+6 , i+8 );
        swap_key(orig, ptr, i+10, i+12);
        swap_key(orig, ptr, i+3 , i+5 );
        swap_key(orig, ptr, i+7 , i+9 );
        swap_key(orig, ptr, i+3 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
        swap_key(orig, ptr, i+7 , i+8 );
        swap_key(orig, ptr, i+9 , i+10);
        swap_key(orig, ptr, i+11, i+12);
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+8 , i+9 );
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
sorter_key(T*& orig, int *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m512d vec0 ;
    __m512d vec1 ;
    __m512d vec2 ;
    __m512d vec3 ;
    __m512d vec4 ;
    __m512d vec5 ;
    __m512d vec6 ;
    __m512d vec7 ;

    // p* holds the index vectors (8 64-bit indices)
    __m512i p0 ;
    __m512i p1 ;
    __m512i p2 ;
    __m512i p3 ;
    // __m512i p4 ;
    // __m512i p5 ;
    // __m512i p6 ;
    // __m512i p7 ;
    // __m512i p8 ;
    // __m512i p9 ;
    // __m512i p10;
    // __m512i p11;
    // __m512i p12;
    // __m512i p13;
    // __m512i p14;
    // __m512i p15;

    uint8_t stride = (uint8_t)simd_width::AVX512_DOUBLE;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_pd(vec0 , orig + i + 0  * stride    );
        vec0  = _mm512_loadunpackhi_pd(vec0 , orig + i + 0  * stride + 8);
        vec1  = _mm512_loadunpacklo_pd(vec1 , orig + i + 1  * stride    );
        vec1  = _mm512_loadunpackhi_pd(vec1 , orig + i + 1  * stride + 8);
        vec2  = _mm512_loadunpacklo_pd(vec2 , orig + i + 2  * stride    );
        vec2  = _mm512_loadunpackhi_pd(vec2 , orig + i + 2  * stride + 8);
        vec3  = _mm512_loadunpacklo_pd(vec3 , orig + i + 3  * stride    );
        vec3  = _mm512_loadunpackhi_pd(vec3 , orig + i + 3  * stride + 8);
        vec4  = _mm512_loadunpacklo_pd(vec4 , orig + i + 4  * stride    );
        vec4  = _mm512_loadunpackhi_pd(vec4 , orig + i + 4  * stride + 8);
        vec5  = _mm512_loadunpacklo_pd(vec5 , orig + i + 5  * stride    );
        vec5  = _mm512_loadunpackhi_pd(vec5 , orig + i + 5  * stride + 8);
        vec6  = _mm512_loadunpacklo_pd(vec6 , orig + i + 6  * stride    );
        vec6  = _mm512_loadunpackhi_pd(vec6 , orig + i + 6  * stride + 8);
        vec7  = _mm512_loadunpacklo_pd(vec7 , orig + i + 7  * stride    );
        vec7  = _mm512_loadunpackhi_pd(vec7 , orig + i + 7  * stride + 8);

        p0  = _mm512_loadunpacklo_epi32(p0 , ptr + i + 0  * stride * 2     );
        p0  = _mm512_loadunpackhi_epi32(p0 , ptr + i + 0  * stride * 2 + 16);
        p1  = _mm512_loadunpacklo_epi32(p1 , ptr + i + 1  * stride * 2     );
        p1  = _mm512_loadunpackhi_epi32(p1 , ptr + i + 1  * stride * 2 + 16);
        p2  = _mm512_loadunpacklo_epi32(p2 , ptr + i + 2  * stride * 2     );
        p2  = _mm512_loadunpackhi_epi32(p2 , ptr + i + 2  * stride * 2 + 16);
        p3  = _mm512_loadunpacklo_epi32(p3 , ptr + i + 3  * stride * 2     );
        p3  = _mm512_loadunpackhi_epi32(p3 , ptr + i + 3  * stride * 2 + 16);
        // p4  = _mm512_loadunpacklo_epi32(p4 , ptr + i + 4  * stride     );
        // p4  = _mm512_loadunpackhi_epi32(p4 , ptr + i + 4  * stride + 16);
        // p5  = _mm512_loadunpacklo_epi32(p5 , ptr + i + 5  * stride     );
        // p5  = _mm512_loadunpackhi_epi32(p5 , ptr + i + 5  * stride + 16);
        // p6  = _mm512_loadunpacklo_epi32(p6 , ptr + i + 6  * stride     );
        // p6  = _mm512_loadunpackhi_epi32(p6 , ptr + i + 6  * stride + 16);
        // p7  = _mm512_loadunpacklo_epi32(p7 , ptr + i + 7  * stride     );
        // p7  = _mm512_loadunpackhi_epi32(p7 , ptr + i + 7  * stride + 16);
        // p8  = _mm512_loadunpacklo_epi32(p8 , ptr + i + 8  * stride     );
        // p8  = _mm512_loadunpackhi_epi32(p8 , ptr + i + 8  * stride + 16);
        // p9  = _mm512_loadunpacklo_epi32(p9 , ptr + i + 9  * stride     );
        // p9  = _mm512_loadunpackhi_epi32(p9 , ptr + i + 9  * stride + 16);
        // p10 = _mm512_loadunpacklo_epi32(p10, ptr + i + 10 * stride     );
        // p10 = _mm512_loadunpackhi_epi32(p10, ptr + i + 10 * stride + 16);
        // p11 = _mm512_loadunpacklo_epi32(p11, ptr + i + 11 * stride     );
        // p11 = _mm512_loadunpackhi_epi32(p11, ptr + i + 11 * stride + 16);
        // p12 = _mm512_loadunpacklo_epi32(p12, ptr + i + 12 * stride     );
        // p12 = _mm512_loadunpackhi_epi32(p12, ptr + i + 12 * stride + 16);
        // p13 = _mm512_loadunpacklo_epi32(p13, ptr + i + 13 * stride     );
        // p13 = _mm512_loadunpackhi_epi32(p13, ptr + i + 13 * stride + 16);
        // p14 = _mm512_loadunpacklo_epi32(p14, ptr + i + 14 * stride     );
        // p14 = _mm512_loadunpackhi_epi32(p14, ptr + i + 14 * stride + 16);
        // p15 = _mm512_loadunpacklo_epi32(p15, ptr + i + 15 * stride     );
        // p15 = _mm512_loadunpackhi_epi32(p15, ptr + i + 15 * stride + 16);

        in_register_sort_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            p0, p1, p2, p3);

        in_register_transpose_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            p0, p1, p2, p3);

        _mm512_packstorelo_pd(orig + i + 0  * stride    , vec0 );
        _mm512_packstorehi_pd(orig + i + 0  * stride + 8, vec0 );
        _mm512_packstorelo_pd(orig + i + 1  * stride    , vec1 );
        _mm512_packstorehi_pd(orig + i + 1  * stride + 8, vec1 );
        _mm512_packstorelo_pd(orig + i + 2  * stride    , vec2 );
        _mm512_packstorehi_pd(orig + i + 2  * stride + 8, vec2 );
        _mm512_packstorelo_pd(orig + i + 3  * stride    , vec3 );
        _mm512_packstorehi_pd(orig + i + 3  * stride + 8, vec3 );
        _mm512_packstorelo_pd(orig + i + 4  * stride    , vec4 );
        _mm512_packstorehi_pd(orig + i + 4  * stride + 8, vec4 );
        _mm512_packstorelo_pd(orig + i + 5  * stride    , vec5 );
        _mm512_packstorehi_pd(orig + i + 5  * stride + 8, vec5 );
        _mm512_packstorelo_pd(orig + i + 6  * stride    , vec6 );
        _mm512_packstorehi_pd(orig + i + 6  * stride + 8, vec6 );
        _mm512_packstorelo_pd(orig + i + 7  * stride    , vec7 );
        _mm512_packstorehi_pd(orig + i + 7  * stride + 8, vec7 );
        // _mm512_packstorelo_pd(orig + i + 8  * stride     , vec8 );
        // _mm512_packstorehi_pd(orig + i + 8  * stride + 16, vec8 );
        // _mm512_packstorelo_pd(orig + i + 9  * stride     , vec9 );
        // _mm512_packstorehi_pd(orig + i + 9  * stride + 16, vec9 );
        // _mm512_packstorelo_pd(orig + i + 10 * stride     , vec10);
        // _mm512_packstorehi_pd(orig + i + 10 * stride + 16, vec10);
        // _mm512_packstorelo_pd(orig + i + 11 * stride     , vec11);
        // _mm512_packstorehi_pd(orig + i + 11 * stride + 16, vec11);
        // _mm512_packstorelo_pd(orig + i + 12 * stride     , vec12);
        // _mm512_packstorehi_pd(orig + i + 12 * stride + 16, vec12);
        // _mm512_packstorelo_pd(orig + i + 13 * stride     , vec13);
        // _mm512_packstorehi_pd(orig + i + 13 * stride + 16, vec13);
        // _mm512_packstorelo_pd(orig + i + 14 * stride     , vec14);
        // _mm512_packstorehi_pd(orig + i + 14 * stride + 16, vec14);
        // _mm512_packstorelo_pd(orig + i + 15 * stride     , vec15);
        // _mm512_packstorehi_pd(orig + i + 15 * stride + 16, vec15);

        _mm512_packstorelo_epi32(ptr + i + 0  * stride * 2     , p0 );
        _mm512_packstorehi_epi32(ptr + i + 0  * stride * 2 + 16, p0 );
        _mm512_packstorelo_epi32(ptr + i + 1  * stride * 2     , p1 );
        _mm512_packstorehi_epi32(ptr + i + 1  * stride * 2 + 16, p1 );
        _mm512_packstorelo_epi32(ptr + i + 2  * stride * 2     , p2 );
        _mm512_packstorehi_epi32(ptr + i + 2  * stride * 2 + 16, p2 );
        _mm512_packstorelo_epi32(ptr + i + 3  * stride * 2     , p3 );
        _mm512_packstorehi_epi32(ptr + i + 3  * stride * 2 + 16, p3 );
        // _mm512_packstorelo_epi32(ptr + i + 4  * stride     , p4 );
        // _mm512_packstorehi_epi32(ptr + i + 4  * stride + 16, p4 );
        // _mm512_packstorelo_epi32(ptr + i + 5  * stride     , p5 );
        // _mm512_packstorehi_epi32(ptr + i + 5  * stride + 16, p5 );
        // _mm512_packstorelo_epi32(ptr + i + 6  * stride     , p6 );
        // _mm512_packstorehi_epi32(ptr + i + 6  * stride + 16, p6 );
        // _mm512_packstorelo_epi32(ptr + i + 7  * stride     , p7 );
        // _mm512_packstorehi_epi32(ptr + i + 7  * stride + 16, p7 );
        // _mm512_packstorelo_epi32(ptr + i + 8  * stride     , p8 );
        // _mm512_packstorehi_epi32(ptr + i + 8  * stride + 16, p8 );
        // _mm512_packstorelo_epi32(ptr + i + 9  * stride     , p9 );
        // _mm512_packstorehi_epi32(ptr + i + 9  * stride + 16, p9 );
        // _mm512_packstorelo_epi32(ptr + i + 10 * stride     , p10);
        // _mm512_packstorehi_epi32(ptr + i + 10 * stride + 16, p10);
        // _mm512_packstorelo_epi32(ptr + i + 11 * stride     , p11);
        // _mm512_packstorehi_epi32(ptr + i + 11 * stride + 16, p11);
        // _mm512_packstorelo_epi32(ptr + i + 12 * stride     , p12);
        // _mm512_packstorehi_epi32(ptr + i + 12 * stride + 16, p12);
        // _mm512_packstorelo_epi32(ptr + i + 13 * stride     , p13);
        // _mm512_packstorehi_epi32(ptr + i + 13 * stride + 16, p13);
        // _mm512_packstorelo_epi32(ptr + i + 14 * stride     , p14);
        // _mm512_packstorehi_epi32(ptr + i + 14 * stride + 16, p14);
        // _mm512_packstorelo_epi32(ptr + i + 15 * stride     , p15);
        // _mm512_packstorehi_epi32(ptr + i + 15 * stride + 16, p15);
    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i+0 , i+4 );
        swap_key(orig, ptr, i+1 , i+5 );
        swap_key(orig, ptr, i+2 , i+6 );
        swap_key(orig, ptr, i+3 , i+7 );
        swap_key(orig, ptr, i+0 , i+2 );
        swap_key(orig, ptr, i+1 , i+3 );
        swap_key(orig, ptr, i+4 , i+6 );
        swap_key(orig, ptr, i+5 , i+7 );
        swap_key(orig, ptr, i+2 , i+4 );
        swap_key(orig, ptr, i+3 , i+5 );
        swap_key(orig, ptr, i+0 , i+1 );
        swap_key(orig, ptr, i+2 , i+3 );
        swap_key(orig, ptr, i+4 , i+5 );
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+1 , i+4 );
        swap_key(orig, ptr, i+3 , i+6 );
        swap_key(orig, ptr, i+1 , i+2 );
        swap_key(orig, ptr, i+3 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
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
sorter_key(T*& orig, long *&ptr, uint32_t size)
{
    uint32_t i, j;
    __m512d vec0;
    __m512d vec1;
    __m512d vec2;
    __m512d vec3;
    __m512d vec4;
    __m512d vec5;
    __m512d vec6;
    __m512d vec7;

    // p* holds the index vectors (8 64-bit indices)
    __m512i p0;
    __m512i p1;
    __m512i p2;
    __m512i p3;
    __m512i p4;
    __m512i p5;
    __m512i p6;
    __m512i p7;

    uint8_t stride = (uint8_t)simd_width::AVX512_DOUBLE;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_pd(vec0 , orig + i + 0  * stride    );
        vec0  = _mm512_loadunpackhi_pd(vec0 , orig + i + 0  * stride + 8);
        vec1  = _mm512_loadunpacklo_pd(vec1 , orig + i + 1  * stride    );
        vec1  = _mm512_loadunpackhi_pd(vec1 , orig + i + 1  * stride + 8);
        vec2  = _mm512_loadunpacklo_pd(vec2 , orig + i + 2  * stride    );
        vec2  = _mm512_loadunpackhi_pd(vec2 , orig + i + 2  * stride + 8);
        vec3  = _mm512_loadunpacklo_pd(vec3 , orig + i + 3  * stride    );
        vec3  = _mm512_loadunpackhi_pd(vec3 , orig + i + 3  * stride + 8);
        vec4  = _mm512_loadunpacklo_pd(vec4 , orig + i + 4  * stride    );
        vec4  = _mm512_loadunpackhi_pd(vec4 , orig + i + 4  * stride + 8);
        vec5  = _mm512_loadunpacklo_pd(vec5 , orig + i + 5  * stride    );
        vec5  = _mm512_loadunpackhi_pd(vec5 , orig + i + 5  * stride + 8);
        vec6  = _mm512_loadunpacklo_pd(vec6 , orig + i + 6  * stride    );
        vec6  = _mm512_loadunpackhi_pd(vec6 , orig + i + 6  * stride + 8);
        vec7  = _mm512_loadunpacklo_pd(vec7 , orig + i + 7  * stride    );
        vec7  = _mm512_loadunpackhi_pd(vec7 , orig + i + 7  * stride + 8);

        p0  = _mm512_loadunpacklo_epi64(p0 , ptr + i + 0 * stride    );
        p0  = _mm512_loadunpackhi_epi64(p0 , ptr + i + 0 * stride + 8);
        p1  = _mm512_loadunpacklo_epi64(p1 , ptr + i + 1 * stride    );
        p1  = _mm512_loadunpackhi_epi64(p1 , ptr + i + 1 * stride + 8);
        p2  = _mm512_loadunpacklo_epi64(p2 , ptr + i + 2 * stride    );
        p2  = _mm512_loadunpackhi_epi64(p2 , ptr + i + 2 * stride + 8);
        p3  = _mm512_loadunpacklo_epi64(p3 , ptr + i + 3 * stride    );
        p3  = _mm512_loadunpackhi_epi64(p3 , ptr + i + 3 * stride + 8);
        p4  = _mm512_loadunpacklo_epi64(p4 , ptr + i + 4 * stride    );
        p4  = _mm512_loadunpackhi_epi64(p4 , ptr + i + 4 * stride + 8);
        p5  = _mm512_loadunpacklo_epi64(p5 , ptr + i + 5 * stride    );
        p5  = _mm512_loadunpackhi_epi64(p5 , ptr + i + 5 * stride + 8);
        p6  = _mm512_loadunpacklo_epi64(p6 , ptr + i + 6 * stride    );
        p6  = _mm512_loadunpackhi_epi64(p6 , ptr + i + 6 * stride + 8);
        p7  = _mm512_loadunpacklo_epi64(p7 , ptr + i + 7 * stride    );
        p7  = _mm512_loadunpackhi_epi64(p7 , ptr + i + 7 * stride + 8);

        in_register_sort_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            p0, p1, p2, p3, p4, p5, p6, p7);

        in_register_transpose_key(
            vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
            p0, p1, p2, p3, p4, p5, p6, p7);

        _mm512_packstorelo_pd(orig + i + 0  * stride    , vec0 );
        _mm512_packstorehi_pd(orig + i + 0  * stride + 8, vec0 );
        _mm512_packstorelo_pd(orig + i + 1  * stride    , vec1 );
        _mm512_packstorehi_pd(orig + i + 1  * stride + 8, vec1 );
        _mm512_packstorelo_pd(orig + i + 2  * stride    , vec2 );
        _mm512_packstorehi_pd(orig + i + 2  * stride + 8, vec2 );
        _mm512_packstorelo_pd(orig + i + 3  * stride    , vec3 );
        _mm512_packstorehi_pd(orig + i + 3  * stride + 8, vec3 );
        _mm512_packstorelo_pd(orig + i + 4  * stride    , vec4 );
        _mm512_packstorehi_pd(orig + i + 4  * stride + 8, vec4 );
        _mm512_packstorelo_pd(orig + i + 5  * stride    , vec5 );
        _mm512_packstorehi_pd(orig + i + 5  * stride + 8, vec5 );
        _mm512_packstorelo_pd(orig + i + 6  * stride    , vec6 );
        _mm512_packstorehi_pd(orig + i + 6  * stride + 8, vec6 );
        _mm512_packstorelo_pd(orig + i + 7  * stride    , vec7 );
        _mm512_packstorehi_pd(orig + i + 7  * stride + 8, vec7 );

        _mm512_packstorelo_epi64(ptr + i + 0 * stride    , p0);
        _mm512_packstorehi_epi64(ptr + i + 0 * stride + 8, p0);
        _mm512_packstorelo_epi64(ptr + i + 1 * stride    , p1);
        _mm512_packstorehi_epi64(ptr + i + 1 * stride + 8, p1);
        _mm512_packstorelo_epi64(ptr + i + 2 * stride    , p2);
        _mm512_packstorehi_epi64(ptr + i + 2 * stride + 8, p2);
        _mm512_packstorelo_epi64(ptr + i + 3 * stride    , p3);
        _mm512_packstorehi_epi64(ptr + i + 3 * stride + 8, p3);
        _mm512_packstorelo_epi64(ptr + i + 4 * stride    , p4);
        _mm512_packstorehi_epi64(ptr + i + 4 * stride + 8, p4);
        _mm512_packstorelo_epi64(ptr + i + 5 * stride    , p5);
        _mm512_packstorehi_epi64(ptr + i + 5 * stride + 8, p5);
        _mm512_packstorelo_epi64(ptr + i + 6 * stride    , p6);
        _mm512_packstorehi_epi64(ptr + i + 6 * stride + 8, p6);
        _mm512_packstorelo_epi64(ptr + i + 7 * stride    , p7);
        _mm512_packstorehi_epi64(ptr + i + 7 * stride + 8, p7);
    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap_key(orig, ptr, i+0 , i+4 );
        swap_key(orig, ptr, i+1 , i+5 );
        swap_key(orig, ptr, i+2 , i+6 );
        swap_key(orig, ptr, i+3 , i+7 );
        swap_key(orig, ptr, i+0 , i+2 );
        swap_key(orig, ptr, i+1 , i+3 );
        swap_key(orig, ptr, i+4 , i+6 );
        swap_key(orig, ptr, i+5 , i+7 );
        swap_key(orig, ptr, i+2 , i+4 );
        swap_key(orig, ptr, i+3 , i+5 );
        swap_key(orig, ptr, i+0 , i+1 );
        swap_key(orig, ptr, i+2 , i+3 );
        swap_key(orig, ptr, i+4 , i+5 );
        swap_key(orig, ptr, i+6 , i+7 );
        swap_key(orig, ptr, i+1 , i+4 );
        swap_key(orig, ptr, i+3 , i+6 );
        swap_key(orig, ptr, i+1 , i+2 );
        swap_key(orig, ptr, i+3 , i+4 );
        swap_key(orig, ptr, i+5 , i+6 );
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
    __m512 vec0 ;
    __m512 vec1 ;
    __m512 vec2 ;
    __m512 vec3 ;
    __m512 vec4 ;
    __m512 vec5 ;
    __m512 vec6 ;
    __m512 vec7 ;
    __m512 vec8 ;
    __m512 vec9 ;
    __m512 vec10;
    __m512 vec11;
    __m512 vec12;
    __m512 vec13;
    __m512 vec14;
    __m512 vec15;
    uint8_t stride = (uint8_t)simd_width::AVX512_FLOAT;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_ps(vec0 , orig + i + 0  * stride     );
        vec0  = _mm512_loadunpackhi_ps(vec0 , orig + i + 0  * stride + 16);
        vec1  = _mm512_loadunpacklo_ps(vec1 , orig + i + 1  * stride     );
        vec1  = _mm512_loadunpackhi_ps(vec1 , orig + i + 1  * stride + 16);
        vec2  = _mm512_loadunpacklo_ps(vec2 , orig + i + 2  * stride     );
        vec2  = _mm512_loadunpackhi_ps(vec2 , orig + i + 2  * stride + 16);
        vec3  = _mm512_loadunpacklo_ps(vec3 , orig + i + 3  * stride     );
        vec3  = _mm512_loadunpackhi_ps(vec3 , orig + i + 3  * stride + 16);
        vec4  = _mm512_loadunpacklo_ps(vec4 , orig + i + 4  * stride     );
        vec4  = _mm512_loadunpackhi_ps(vec4 , orig + i + 4  * stride + 16);
        vec5  = _mm512_loadunpacklo_ps(vec5 , orig + i + 5  * stride     );
        vec5  = _mm512_loadunpackhi_ps(vec5 , orig + i + 5  * stride + 16);
        vec6  = _mm512_loadunpacklo_ps(vec6 , orig + i + 6  * stride     );
        vec6  = _mm512_loadunpackhi_ps(vec6 , orig + i + 6  * stride + 16);
        vec7  = _mm512_loadunpacklo_ps(vec7 , orig + i + 7  * stride     );
        vec7  = _mm512_loadunpackhi_ps(vec7 , orig + i + 7  * stride + 16);
        vec8  = _mm512_loadunpacklo_ps(vec8 , orig + i + 8  * stride     );
        vec8  = _mm512_loadunpackhi_ps(vec8 , orig + i + 8  * stride + 16);
        vec9  = _mm512_loadunpacklo_ps(vec9 , orig + i + 9  * stride     );
        vec9  = _mm512_loadunpackhi_ps(vec9 , orig + i + 9  * stride + 16);
        vec10 = _mm512_loadunpacklo_ps(vec10, orig + i + 10 * stride     );
        vec10 = _mm512_loadunpackhi_ps(vec10, orig + i + 10 * stride + 16);
        vec11 = _mm512_loadunpacklo_ps(vec11, orig + i + 11 * stride     );
        vec11 = _mm512_loadunpackhi_ps(vec11, orig + i + 11 * stride + 16);
        vec12 = _mm512_loadunpacklo_ps(vec12, orig + i + 12 * stride     );
        vec12 = _mm512_loadunpackhi_ps(vec12, orig + i + 12 * stride + 16);
        vec13 = _mm512_loadunpacklo_ps(vec13, orig + i + 13 * stride     );
        vec13 = _mm512_loadunpackhi_ps(vec13, orig + i + 13 * stride + 16);
        vec14 = _mm512_loadunpacklo_ps(vec14, orig + i + 14 * stride     );
        vec14 = _mm512_loadunpackhi_ps(vec14, orig + i + 14 * stride + 16);
        vec15 = _mm512_loadunpacklo_ps(vec15, orig + i + 15 * stride     );
        vec15 = _mm512_loadunpackhi_ps(vec15, orig + i + 15 * stride + 16);

        in_register_sort(
                vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
                vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15);
        in_register_transpose(
                vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 ,
                vec8, vec9, vec10, vec11, vec12, vec13, vec14, vec15);

        _mm512_packstorelo_ps(orig + i + 0  * stride     , vec0 );
        _mm512_packstorehi_ps(orig + i + 0  * stride + 16, vec0 );
        _mm512_packstorelo_ps(orig + i + 1  * stride     , vec1 );
        _mm512_packstorehi_ps(orig + i + 1  * stride + 16, vec1 );
        _mm512_packstorelo_ps(orig + i + 2  * stride     , vec2 );
        _mm512_packstorehi_ps(orig + i + 2  * stride + 16, vec2 );
        _mm512_packstorelo_ps(orig + i + 3  * stride     , vec3 );
        _mm512_packstorehi_ps(orig + i + 3  * stride + 16, vec3 );
        _mm512_packstorelo_ps(orig + i + 4  * stride     , vec4 );
        _mm512_packstorehi_ps(orig + i + 4  * stride + 16, vec4 );
        _mm512_packstorelo_ps(orig + i + 5  * stride     , vec5 );
        _mm512_packstorehi_ps(orig + i + 5  * stride + 16, vec5 );
        _mm512_packstorelo_ps(orig + i + 6  * stride     , vec6 );
        _mm512_packstorehi_ps(orig + i + 6  * stride + 16, vec6 );
        _mm512_packstorelo_ps(orig + i + 7  * stride     , vec7 );
        _mm512_packstorehi_ps(orig + i + 7  * stride + 16, vec7 );
        _mm512_packstorelo_ps(orig + i + 8  * stride     , vec8 );
        _mm512_packstorehi_ps(orig + i + 8  * stride + 16, vec8 );
        _mm512_packstorelo_ps(orig + i + 9  * stride     , vec9 );
        _mm512_packstorehi_ps(orig + i + 9  * stride + 16, vec9 );
        _mm512_packstorelo_ps(orig + i + 10 * stride     , vec10);
        _mm512_packstorehi_ps(orig + i + 10 * stride + 16, vec10);
        _mm512_packstorelo_ps(orig + i + 11 * stride     , vec11);
        _mm512_packstorehi_ps(orig + i + 11 * stride + 16, vec11);
        _mm512_packstorelo_ps(orig + i + 12 * stride     , vec12);
        _mm512_packstorehi_ps(orig + i + 12 * stride + 16, vec12);
        _mm512_packstorelo_ps(orig + i + 13 * stride     , vec13);
        _mm512_packstorehi_ps(orig + i + 13 * stride + 16, vec13);
        _mm512_packstorelo_ps(orig + i + 14 * stride     , vec14);
        _mm512_packstorehi_ps(orig + i + 14 * stride + 16, vec14);
        _mm512_packstorelo_ps(orig + i + 15 * stride     , vec15);
        _mm512_packstorehi_ps(orig + i + 15 * stride + 16, vec15);
    }

    // "best" sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap(orig, i+0 , i+1 );
        swap(orig, i+2 , i+3 );
        swap(orig, i+4 , i+5 );
        swap(orig, i+6 , i+7 );
        swap(orig, i+8 , i+9 );
        swap(orig, i+10, i+11);
        swap(orig, i+12, i+13);
        swap(orig, i+14, i+15);
        swap(orig, i+0 , i+2 );
        swap(orig, i+4 , i+6 );
        swap(orig, i+8 , i+10);
        swap(orig, i+12, i+14);
        swap(orig, i+1 , i+3 );
        swap(orig, i+5 , i+7 );
        swap(orig, i+9 , i+11);
        swap(orig, i+13, i+15);
        swap(orig, i+0 , i+4 );
        swap(orig, i+8 , i+12);
        swap(orig, i+1 , i+5 );
        swap(orig, i+9 , i+13);
        swap(orig, i+2 , i+6 );
        swap(orig, i+10, i+14);
        swap(orig, i+3 , i+7 );
        swap(orig, i+11, i+15);
        swap(orig, i+0 , i+8 );
        swap(orig, i+1 , i+9 );
        swap(orig, i+2 , i+10);
        swap(orig, i+3 , i+11);
        swap(orig, i+4 , i+12);
        swap(orig, i+5 , i+13);
        swap(orig, i+6 , i+14);
        swap(orig, i+7 , i+15);
        swap(orig, i+5 , i+10);
        swap(orig, i+6 , i+9 );
        swap(orig, i+3 , i+12);
        swap(orig, i+13, i+14);
        swap(orig, i+7 , i+11);
        swap(orig, i+1 , i+2 );
        swap(orig, i+4 , i+8 );
        swap(orig, i+1 , i+4 );
        swap(orig, i+7 , i+13);
        swap(orig, i+2 , i+8 );
        swap(orig, i+11, i+14);
        swap(orig, i+2 , i+4 );
        swap(orig, i+5 , i+6 );
        swap(orig, i+9 , i+10);
        swap(orig, i+11, i+13);
        swap(orig, i+3 , i+8 );
        swap(orig, i+7 , i+12);
        swap(orig, i+6 , i+8 );
        swap(orig, i+10, i+12);
        swap(orig, i+3 , i+5 );
        swap(orig, i+7 , i+9 );
        swap(orig, i+3 , i+4 );
        swap(orig, i+5 , i+6 );
        swap(orig, i+7 , i+8 );
        swap(orig, i+9 , i+10);
        swap(orig, i+11, i+12);
        swap(orig, i+6 , i+7 );
        swap(orig, i+8 , i+9 );
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

template <typename T>
typename std::enable_if<std::is_same<T, double>::value>::type
sorter(T*& orig, uint32_t size)
{
    uint32_t i, j;
    __m512d vec0 ;
    __m512d vec1 ;
    __m512d vec2 ;
    __m512d vec3 ;
    __m512d vec4 ;
    __m512d vec5 ;
    __m512d vec6 ;
    __m512d vec7 ;
    uint8_t stride = (uint8_t)simd_width::AVX512_DOUBLE;
    for(i = 0; i + stride * stride - 1 < size; i += stride * stride) 
    {
        vec0  = _mm512_loadunpacklo_pd(vec0 , orig + i + 0  * stride         );
        vec0  = _mm512_loadunpackhi_pd(vec0 , orig + i + 0  * stride + stride);
        vec1  = _mm512_loadunpacklo_pd(vec1 , orig + i + 1  * stride         );
        vec1  = _mm512_loadunpackhi_pd(vec1 , orig + i + 1  * stride + stride);
        vec2  = _mm512_loadunpacklo_pd(vec2 , orig + i + 2  * stride         );
        vec2  = _mm512_loadunpackhi_pd(vec2 , orig + i + 2  * stride + stride);
        vec3  = _mm512_loadunpacklo_pd(vec3 , orig + i + 3  * stride         );
        vec3  = _mm512_loadunpackhi_pd(vec3 , orig + i + 3  * stride + stride);
        vec4  = _mm512_loadunpacklo_pd(vec4 , orig + i + 4  * stride         );
        vec4  = _mm512_loadunpackhi_pd(vec4 , orig + i + 4  * stride + stride);
        vec5  = _mm512_loadunpacklo_pd(vec5 , orig + i + 5  * stride         );
        vec5  = _mm512_loadunpackhi_pd(vec5 , orig + i + 5  * stride + stride);
        vec6  = _mm512_loadunpacklo_pd(vec6 , orig + i + 6  * stride         );
        vec6  = _mm512_loadunpackhi_pd(vec6 , orig + i + 6  * stride + stride);
        vec7  = _mm512_loadunpacklo_pd(vec7 , orig + i + 7  * stride         );
        vec7  = _mm512_loadunpackhi_pd(vec7 , orig + i + 7  * stride + stride);

        in_register_sort(
                vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 );
        in_register_transpose(
                vec0, vec1, vec2 , vec3 , vec4 , vec5 , vec6 , vec7 );

        _mm512_packstorelo_pd(orig + i + 0  * stride         , vec0 );
        _mm512_packstorehi_pd(orig + i + 0  * stride + stride, vec0 );
        _mm512_packstorelo_pd(orig + i + 1  * stride         , vec1 );
        _mm512_packstorehi_pd(orig + i + 1  * stride + stride, vec1 );
        _mm512_packstorelo_pd(orig + i + 2  * stride         , vec2 );
        _mm512_packstorehi_pd(orig + i + 2  * stride + stride, vec2 );
        _mm512_packstorelo_pd(orig + i + 3  * stride         , vec3 );
        _mm512_packstorehi_pd(orig + i + 3  * stride + stride, vec3 );
        _mm512_packstorelo_pd(orig + i + 4  * stride         , vec4 );
        _mm512_packstorehi_pd(orig + i + 4  * stride + stride, vec4 );
        _mm512_packstorelo_pd(orig + i + 5  * stride         , vec5 );
        _mm512_packstorehi_pd(orig + i + 5  * stride + stride, vec5 );
        _mm512_packstorelo_pd(orig + i + 6  * stride         , vec6 );
        _mm512_packstorehi_pd(orig + i + 6  * stride + stride, vec6 );
        _mm512_packstorelo_pd(orig + i + 7  * stride         , vec7 );
        _mm512_packstorehi_pd(orig + i + 7  * stride + stride, vec7 );
    }

    // Batcher's Merge-Exchange sorting network
    for(/*cont'd*/; i + stride - 1 < size; i += stride) 
    {
        swap(orig, i+0 , i+4 );
        swap(orig, i+1 , i+5 );
        swap(orig, i+2 , i+6 );
        swap(orig, i+3 , i+7 );
        swap(orig, i+0 , i+2 );
        swap(orig, i+1 , i+3 );
        swap(orig, i+4 , i+6 );
        swap(orig, i+5 , i+7 );
        swap(orig, i+2 , i+4 );
        swap(orig, i+3 , i+5 );
        swap(orig, i+0 , i+1 );
        swap(orig, i+2 , i+3 );
        swap(orig, i+4 , i+5 );
        swap(orig, i+6 , i+7 );
        swap(orig, i+1 , i+4 );
        swap(orig, i+3 , i+6 );
        swap(orig, i+1 , i+2 );
        swap(orig, i+3 , i+4 );
        swap(orig, i+5 , i+6 );
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

} // end namespace internal

} // end namespace aspas
