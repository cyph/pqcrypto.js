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
 * @file aspas_merge_avx2.tcc
 *
 * Definiation of the merge function in AVX2 instruction sets.
 *
 */

#include <immintrin.h>
#include <type_traits>
#include <cstdint>

#include "aspas.h" 
#include "util/extintrin.h"

namespace aspas
{

/**
 * Integer vector (__m256i) version:
 * This method performs the in-register merge of two sorted vectors.
 *
 * @param v0 v1 sorted vector registers
 * @return sorted data stored horizontally in the two registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_merge(T& v0, T& v1)
{
    __m256i l1,h1,l1p,h1p,l2,h2,l2p,h2p,l3,h3,l3p,h3p,l4,h4;
    __m256i ext,ext1,ext2;
    __m128i sl1, sh1, sl2, sh2;
    __m128i max1, min1, max2, min2;

    // reverse register v1 
    ext = _mm256_shuffle_epi32(v1, _MM_PERM_ABCD);
    v1  = _mm256_permute2x128_si256(ext, ext, 0x03);

    // level 1 comparison
    l1  = _mm256_min_epi32(v0, v1);
    h1  = _mm256_max_epi32(v0, v1); 

    // level 2 comparison
    l1p = _mm256_permute2x128_si256(l1, h1, 0x30);
    h1p = _mm256_permute2x128_si256(l1, h1, 0x21); 
    l2  = _mm256_min_epi32(l1p, h1p);
    h2  = _mm256_max_epi32(l1p, h1p); 

    // level 3 comparison
    l2p = (__m256i)_mm256_shuffle_ps((__m256)l2, (__m256)h2, _MM_SHUFFLE(3,2,1,0));
    h2p = (__m256i)_mm256_shuffle_ps((__m256)l2, (__m256)h2, _MM_SHUFFLE(1,0,3,2));
    l3  = _mm256_min_epi32(l2p, h2p);
    h3  = _mm256_max_epi32(l2p, h2p);

    // level 4 comparison
    l3p = (__m256i)_mm256_blend_ps((__m256)l3, (__m256)h3, 0xAA);
    ext = (__m256i)_mm256_blend_ps((__m256)l3, (__m256)h3, 0x55);
    h3p = (__m256i)_mm256_shuffle_ps((__m256)ext, (__m256)ext, _MM_SHUFFLE(2,3,0,1));
    l4  = _mm256_min_epi32(l3p, h3p);
    h4  = _mm256_max_epi32(l3p, h3p); 

    // final permute/shuffle
    ext1 = (__m256i)_mm256_unpacklo_ps((__m256)l4, (__m256)h4);
    ext2 = (__m256i)_mm256_unpackhi_ps((__m256)l4, (__m256)h4);
    v0 = (__m256i)_mm256_permute2f128_ps((__m256)ext1, (__m256)ext2, 0x20);
    v1 = (__m256i)_mm256_permute2f128_ps((__m256)ext1, (__m256)ext2, 0x31);
}

template <typename T> // int key int index
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_merge_key(T& v0, T& v1, __m256i &pv0, __m256i &pv1)
{
    __m256i l1,h1,l1p,h1p,l2,h2,l2p,h2p,l3,h3,l3p,h3p,l4,h4;
    __m256i pl1,ph1,pl1p,ph1p,pl2,ph2,pl2p,ph2p,pl3,ph3,pl3p,ph3p,pl4,ph4;
    __m256i ext,ext1,ext2;
    __m256i pext,pext1,pext2;
    __m128i sl1, sh1, sl2, sh2;
    __m128i max1, min1, max2, min2;
    __m256 m0;

    // reverse register v1 
    ext = (__m256i)_mm256_shuffle_ps((__m256)v1, (__m256)v1, _MM_SHUFFLE(0,1,2,3));
    v1  = (__m256i)_mm256_permute2f128_ps((__m256)ext, (__m256)ext, 0x03);
    pext = (__m256i)_mm256_shuffle_ps((__m256)pv1, (__m256)pv1, _MM_SHUFFLE(0,1,2,3));
    pv1  = (__m256i)_mm256_permute2f128_ps((__m256)pext, (__m256)pext, 0x03);

    // level 1 comparison
    // l1  = util::_my_mm256_min_epi32(v0, v1);
    // h1  = util::_my_mm256_max_epi32(v0, v1); 
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v1);
    l1 = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v1, m0);
    h1 = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v0, m0);
    pl1 = (__m256i)_mm256_blendv_ps((__m256)pv0, (__m256)pv1, m0);
    ph1 = (__m256i)_mm256_blendv_ps((__m256)pv1, (__m256)pv0, m0);

    // level 2 comparison
    l1p = (__m256i)_mm256_permute2f128_ps((__m256)l1, (__m256)h1, 0x30);
    h1p = (__m256i)_mm256_permute2f128_ps((__m256)l1, (__m256)h1, 0x21); 
    pl1p = (__m256i)_mm256_permute2f128_ps((__m256)pl1, (__m256)ph1, 0x30);
    ph1p = (__m256i)_mm256_permute2f128_ps((__m256)pl1, (__m256)ph1, 0x21); 
    // l2  = util::_my_mm256_min_epi32(l1p, h1p);
    // h2  = util::_my_mm256_max_epi32(l1p, h1p); 
    m0 = (__m256)_mm256_cmpgt_epi32(l1p, h1p);
    l2 = (__m256i)_mm256_blendv_ps((__m256)l1p, (__m256)h1p, m0);
    h2 = (__m256i)_mm256_blendv_ps((__m256)h1p, (__m256)l1p, m0);
    pl2 = (__m256i)_mm256_blendv_ps((__m256)pl1p, (__m256)ph1p, m0);
    ph2 = (__m256i)_mm256_blendv_ps((__m256)ph1p, (__m256)pl1p, m0);

    // level 3 comparison
    l2p = (__m256i)_mm256_shuffle_ps((__m256)l2, (__m256)h2, _MM_SHUFFLE(3,2,1,0));
    h2p = (__m256i)_mm256_shuffle_ps((__m256)l2, (__m256)h2, _MM_SHUFFLE(1,0,3,2));
    pl2p = (__m256i)_mm256_shuffle_ps((__m256)pl2, (__m256)ph2, _MM_SHUFFLE(3,2,1,0));
    ph2p = (__m256i)_mm256_shuffle_ps((__m256)pl2, (__m256)ph2, _MM_SHUFFLE(1,0,3,2));
    // l3  = util::_my_mm256_min_epi32(l2p, h2p);
    // h3  = util::_my_mm256_max_epi32(l2p, h2p);
    m0 = (__m256)_mm256_cmpgt_epi32(l2p, h2p);
    l3 = (__m256i)_mm256_blendv_ps((__m256)l2p, (__m256)h2p, m0);
    h3 = (__m256i)_mm256_blendv_ps((__m256)h2p, (__m256)l2p, m0);
    pl3 = (__m256i)_mm256_blendv_ps((__m256)pl2p, (__m256)ph2p, m0);
    ph3 = (__m256i)_mm256_blendv_ps((__m256)ph2p, (__m256)pl2p, m0);

    // level 4 comparison
    l3p = (__m256i)_mm256_blend_ps((__m256)l3, (__m256)h3, 0xAA);
    ext = (__m256i)_mm256_blend_ps((__m256)l3, (__m256)h3, 0x55);
    h3p = (__m256i)_mm256_shuffle_ps((__m256)ext, (__m256)ext, _MM_SHUFFLE(2,3,0,1));
    pl3p = (__m256i)_mm256_blend_ps((__m256)pl3, (__m256)ph3, 0xAA);
    pext = (__m256i)_mm256_blend_ps((__m256)pl3, (__m256)ph3, 0x55);
    ph3p = (__m256i)_mm256_shuffle_ps((__m256)pext, (__m256)pext, _MM_SHUFFLE(2,3,0,1));
    // l4  = util::_my_mm256_min_epi32(l3p, h3p);
    // h4  = util::_my_mm256_max_epi32(l3p, h3p); 
    m0 = (__m256)_mm256_cmpgt_epi32(l3p, h3p);
    l4 = (__m256i)_mm256_blendv_ps((__m256)l3p, (__m256)h3p, m0);
    h4 = (__m256i)_mm256_blendv_ps((__m256)h3p, (__m256)l3p, m0);
    pl4 = (__m256i)_mm256_blendv_ps((__m256)pl3p, (__m256)ph3p, m0);
    ph4 = (__m256i)_mm256_blendv_ps((__m256)ph3p, (__m256)pl3p, m0);

    // final permute/shuffle
    ext1 = (__m256i)_mm256_unpacklo_ps((__m256)l4, (__m256)h4);
    ext2 = (__m256i)_mm256_unpackhi_ps((__m256)l4, (__m256)h4);
    v0 = (__m256i)_mm256_permute2f128_ps((__m256)ext1, (__m256)ext2, 0x20);
    v1 = (__m256i)_mm256_permute2f128_ps((__m256)ext1, (__m256)ext2, 0x31);
    pext1 = (__m256i)_mm256_unpacklo_ps((__m256)pl4, (__m256)ph4);
    pext2 = (__m256i)_mm256_unpackhi_ps((__m256)pl4, (__m256)ph4);
    pv0 = (__m256i)_mm256_permute2f128_ps((__m256)pext1, (__m256)pext2, 0x20);
    pv1 = (__m256i)_mm256_permute2f128_ps((__m256)pext1, (__m256)pext2, 0x31);
}

template <typename T> // int key long index
typename std::enable_if<std::is_same<T, __m256i>::value>::type
in_register_merge_key(T& v0, T& v1, __m256i &pvl0, __m256i &pvh0, __m256i &pvl1, __m256i &pvh1)
{
    __m256i l1,h1,l1p,h1p,l2,h2,l2p,h2p,l3,h3,l3p,h3p,l4,h4;
    __m256i pll1,plh1,pll1p,plh1p,pll2,plh2,pll2p,plh2p,pll3,plh3,pll3p,plh3p,pll4,plh4;
    __m256i phl1,phh1,phl1p,phh1p,phl2,phh2,phl2p,phh2p,phl3,phh3,phl3p,phh3p,phl4,phh4;
    __m256i ext,ext1,ext2;
    __m256i plext,plext1,plext2;
    __m256i phext,phext1,phext2;
    __m128i sl1, sh1, sl2, sh2;
    __m128i max1, min1, max2, min2;
    __m256 m0;
    __m128i sl, sh;
    __m256d d_sl, d_sh;

    // reverse register v1 
    ext = (__m256i)_mm256_shuffle_ps((__m256)v1, (__m256)v1, _MM_SHUFFLE(0,1,2,3));
    v1  = (__m256i)_mm256_permute2f128_ps((__m256)ext, (__m256)ext, 0x03);
    plext = (__m256i)_mm256_shuffle_pd((__m256d)pvl1, (__m256d)pvl1, 0x5);
    phext = (__m256i)_mm256_shuffle_pd((__m256d)pvh1, (__m256d)pvh1, 0x5);
    pvh1 = (__m256i)_mm256_permute2f128_pd((__m256d)plext, (__m256d)plext, 0x01);
    pvl1 = (__m256i)_mm256_permute2f128_pd((__m256d)phext, (__m256d)phext, 0x01);

    // level 1 comparison
    // l1  = util::_my_mm256_min_epi32(v0, v1);
    // h1  = util::_my_mm256_max_epi32(v0, v1); 
    m0 = (__m256)_mm256_cmpgt_epi32(v0, v1);
    l1 = (__m256i)_mm256_blendv_ps((__m256)v0, (__m256)v1, m0);
    h1 = (__m256i)_mm256_blendv_ps((__m256)v1, (__m256)v0, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll1 = (__m256i)_mm256_blendv_pd((__m256d)pvl0, (__m256d)pvl1, d_sl);
    phl1 = (__m256i)_mm256_blendv_pd((__m256d)pvh0, (__m256d)pvh1, d_sh);
    plh1 = (__m256i)_mm256_blendv_pd((__m256d)pvl1, (__m256d)pvl0, d_sl); 
    phh1 = (__m256i)_mm256_blendv_pd((__m256d)pvh1, (__m256d)pvh0, d_sh);     

    // level 2 comparison
    l1p = (__m256i)_mm256_permute2f128_ps((__m256)l1, (__m256)h1, 0x30);
    h1p = (__m256i)_mm256_permute2f128_ps((__m256)l1, (__m256)h1, 0x21); 
    pll1p = pll1;
    phl1p = phh1; 
    plh1p = phl1;
    phh1p = plh1;

    // l2  = util::_my_mm256_min_epi32(l1p, h1p);
    // h2  = util::_my_mm256_max_epi32(l1p, h1p); 
    m0 = (__m256)_mm256_cmpgt_epi32(l1p, h1p);
    l2 = (__m256i)_mm256_blendv_ps((__m256)l1p, (__m256)h1p, m0);
    h2 = (__m256i)_mm256_blendv_ps((__m256)h1p, (__m256)l1p, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll2 = (__m256i)_mm256_blendv_pd((__m256d)pll1p, (__m256d)plh1p, d_sl);
    phl2 = (__m256i)_mm256_blendv_pd((__m256d)phl1p, (__m256d)phh1p, d_sh);
    plh2 = (__m256i)_mm256_blendv_pd((__m256d)plh1p, (__m256d)pll1p, d_sl); 
    phh2 = (__m256i)_mm256_blendv_pd((__m256d)phh1p, (__m256d)phl1p, d_sh);


    // level 3 comparison
    l2p = (__m256i)_mm256_shuffle_ps((__m256)l2, (__m256)h2, _MM_SHUFFLE(3,2,1,0));
    h2p = (__m256i)_mm256_shuffle_ps((__m256)l2, (__m256)h2, _MM_SHUFFLE(1,0,3,2));

    pll2p = (__m256i)_mm256_permute2f128_pd((__m256d)pll2, (__m256d)plh2, 0x30);
    phl2p = (__m256i)_mm256_permute2f128_pd((__m256d)phl2, (__m256d)phh2, 0x30);
    plh2p = (__m256i)_mm256_permute2f128_pd((__m256d)pll2, (__m256d)plh2, 0x21);
    phh2p = (__m256i)_mm256_permute2f128_pd((__m256d)phl2, (__m256d)phh2, 0x21);

    // l3  = util::_my_mm256_min_epi32(l2p, h2p);
    // h3  = util::_my_mm256_max_epi32(l2p, h2p);
    m0 = (__m256)_mm256_cmpgt_epi32(l2p, h2p);
    l3 = (__m256i)_mm256_blendv_ps((__m256)l2p, (__m256)h2p, m0);
    h3 = (__m256i)_mm256_blendv_ps((__m256)h2p, (__m256)l2p, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll3 = (__m256i)_mm256_blendv_pd((__m256d)pll2p, (__m256d)plh2p, d_sl);
    phl3 = (__m256i)_mm256_blendv_pd((__m256d)phl2p, (__m256d)phh2p, d_sh);
    plh3 = (__m256i)_mm256_blendv_pd((__m256d)plh2p, (__m256d)pll2p, d_sl); 
    phh3 = (__m256i)_mm256_blendv_pd((__m256d)phh2p, (__m256d)phl2p, d_sh);

    // level 4 comparison
    // for(int i=0;i<8;i++) std::cout << ((int*)&l3)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<8;i++) std::cout << ((int*)&h3)[i] << " "; std::cout << std::endl;
    l3p = (__m256i)_mm256_blend_ps((__m256)l3, (__m256)h3, 0xAA);
    ext = (__m256i)_mm256_blend_ps((__m256)l3, (__m256)h3, 0x55);
    h3p = (__m256i)_mm256_shuffle_ps((__m256)ext, (__m256)ext, _MM_SHUFFLE(2,3,0,1));
    // for(int i=0;i<8;i++) std::cout << ((int*)&l3p)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<8;i++) std::cout << ((int*)&h3p)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4;i++) std::cout << ((long*)&pll3)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&phl3)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4;i++) std::cout << ((long*)&plh3)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&phh3)[i] << " "; std::cout << std::endl;
    pll3p = (__m256i)_mm256_blend_pd((__m256d)pll3, (__m256d)plh3, 0xa);
    phl3p = (__m256i)_mm256_blend_pd((__m256d)phl3, (__m256d)phh3, 0xa);
    plext = (__m256i)_mm256_blend_pd((__m256d)pll3, (__m256d)plh3, 0x5);
    phext = (__m256i)_mm256_blend_pd((__m256d)phl3, (__m256d)phh3, 0x5);
    plh3p = (__m256i)_mm256_shuffle_pd((__m256d)plext, (__m256d)plext, 0x5);
    phh3p = (__m256i)_mm256_shuffle_pd((__m256d)phext, (__m256d)phext, 0x5);
    // for(int i=0;i<4;i++) std::cout << ((long*)&pll3p)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&phl3p)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4;i++) std::cout << ((long*)&plh3p)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&phh3p)[i] << " "; std::cout << std::endl;


    // l4  = util::_my_mm256_min_epi32(l3p, h3p);
    // h4  = util::_my_mm256_max_epi32(l3p, h3p); 
    m0 = (__m256)_mm256_cmpgt_epi32(l3p, h3p);
    l4 = (__m256i)_mm256_blendv_ps((__m256)l3p, (__m256)h3p, m0);
    h4 = (__m256i)_mm256_blendv_ps((__m256)h3p, (__m256)l3p, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll4 = (__m256i)_mm256_blendv_pd((__m256d)pll3p, (__m256d)plh3p, d_sl);
    phl4 = (__m256i)_mm256_blendv_pd((__m256d)phl3p, (__m256d)phh3p, d_sh);
    plh4 = (__m256i)_mm256_blendv_pd((__m256d)plh3p, (__m256d)pll3p, d_sl); 
    phh4 = (__m256i)_mm256_blendv_pd((__m256d)phh3p, (__m256d)phl3p, d_sh);

    // final permute/shuffle
    ext1 = (__m256i)_mm256_unpacklo_ps((__m256)l4, (__m256)h4);
    ext2 = (__m256i)_mm256_unpackhi_ps((__m256)l4, (__m256)h4);
    v0 = (__m256i)_mm256_permute2f128_ps((__m256)ext1, (__m256)ext2, 0x20);
    v1 = (__m256i)_mm256_permute2f128_ps((__m256)ext1, (__m256)ext2, 0x31);
    plext1 = (__m256i)_mm256_unpacklo_pd((__m256d)pll4, (__m256d)plh4);
    phext1 = (__m256i)_mm256_unpacklo_pd((__m256d)phl4, (__m256d)phh4);
    plext2 = (__m256i)_mm256_unpackhi_pd((__m256d)pll4, (__m256d)plh4);
    phext2 = (__m256i)_mm256_unpackhi_pd((__m256d)phl4, (__m256d)phh4);
    pvl0 = (__m256i)_mm256_permute2f128_pd((__m256d)plext1, (__m256d)plext2, 0x20);
    pvl1 = (__m256i)_mm256_permute2f128_pd((__m256d)phext1, (__m256d)phext2, 0x20);
    pvh0 = (__m256i)_mm256_permute2f128_pd((__m256d)plext1, (__m256d)plext2, 0x31);
    pvh1 = (__m256i)_mm256_permute2f128_pd((__m256d)phext1, (__m256d)phext2, 0x31);
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvl0)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvh0)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvl1)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvh1)[i] << " "; std::cout << std::endl;
}
/**
 * Float vector (__m256) version:
 * This method performs the in-register merge of two sorted vectors.
 *
 * @param v0 v1 sorted vector registers
 * @return sorted data stored horizontally in the two registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_merge(T& v0, T& v1)
{
    __m256 l1,h1,l1p,h1p,l2,h2,l2p,h2p,l3,h3,l3p,h3p,l4,h4;
    __m256 ext,ext1,ext2;
    __m128 sl1, sh1, sl2, sh2;
    __m128 max1, min1, max2, min2;

    // reverse register v1 
    ext = _mm256_shuffle_ps(v1, v1, _MM_SHUFFLE(0,1,2,3));
    v1  = _mm256_permute2f128_ps(ext, ext, 0x03);

    // level 1 comparison
    l1  = _mm256_min_ps(v0, v1);
    h1  = _mm256_max_ps(v0, v1); 

    // level 2 comparison
    l1p = _mm256_permute2f128_ps(l1, h1, 0x30);
    h1p = _mm256_permute2f128_ps(l1, h1, 0x21); 
    l2  = _mm256_min_ps(l1p, h1p);
    h2  = _mm256_max_ps(l1p, h1p); 

    // level 3 comparison
    l2p = _mm256_shuffle_ps(l2, h2, _MM_SHUFFLE(3,2,1,0));
    h2p = _mm256_shuffle_ps(l2, h2, _MM_SHUFFLE(1,0,3,2));
    l3  = _mm256_min_ps(l2p, h2p);
    h3  = _mm256_max_ps(l2p, h2p);

    // level 4 comparison
    l3p = _mm256_blend_ps(l3, h3, 0xAA);
    ext = _mm256_blend_ps(l3, h3, 0x55);
    h3p = _mm256_shuffle_ps(ext, ext, _MM_SHUFFLE(2,3,0,1));
    l4  = _mm256_min_ps(l3p, h3p);
    h4  = _mm256_max_ps(l3p, h3p); 

    // final permute/shuffle
    ext1 = _mm256_unpacklo_ps(l4, h4);
    ext2 = _mm256_unpackhi_ps(l4, h4);
    v0 = _mm256_permute2f128_ps(ext1, ext2, 0x20);
    v1 = _mm256_permute2f128_ps(ext1, ext2, 0x31);
}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_merge_key(T& v0, T& v1, __m256i &pv0, __m256i &pv1)
{
    __m256 l1,h1,l1p,h1p,l2,h2,l2p,h2p,l3,h3,l3p,h3p,l4,h4;
    __m256i pl1,ph1,pl1p,ph1p,pl2,ph2,pl2p,ph2p,pl3,ph3,pl3p,ph3p,pl4,ph4;
    __m256 ext,ext1,ext2;
    __m256i pext,pext1,pext2;
    __m128 sl1, sh1, sl2, sh2;
    __m128 max1, min1, max2, min2;
    __m256 m0;

    // reverse register v1 
    ext = _mm256_shuffle_ps(v1, v1, _MM_SHUFFLE(0,1,2,3));
    v1  = _mm256_permute2f128_ps(ext, ext, 0x03);
    pext = (__m256i)_mm256_shuffle_ps((__m256)pv1, (__m256)pv1, _MM_SHUFFLE(0,1,2,3));
    pv1  = (__m256i)_mm256_permute2f128_ps((__m256)pext, (__m256)pext, 0x03);

    // level 1 comparison
    // l1  = _mm256_min_ps(v0, v1);
    // h1  = _mm256_max_ps(v0, v1); 
    m0 = _mm256_cmp_ps(v0, v1, _CMP_GT_OS);
    l1 = _mm256_blendv_ps(v0, v1, m0);
    h1 = _mm256_blendv_ps(v1, v0, m0);
    pl1 = (__m256i)_mm256_blendv_ps((__m256)pv0, (__m256)pv1, m0);
    ph1 = (__m256i)_mm256_blendv_ps((__m256)pv1, (__m256)pv0, m0);

    // level 2 comparison
    l1p = _mm256_permute2f128_ps(l1, h1, 0x30);
    h1p = _mm256_permute2f128_ps(l1, h1, 0x21); 
    pl1p = (__m256i)_mm256_permute2f128_ps((__m256)pl1, (__m256)ph1, 0x30);
    ph1p = (__m256i)_mm256_permute2f128_ps((__m256)pl1, (__m256)ph1, 0x21); 
    // l2  = _mm256_min_ps(l1p, h1p);
    // h2  = _mm256_max_ps(l1p, h1p); 
    m0 = _mm256_cmp_ps(l1p, h1p, _CMP_GT_OS);
    l2 = _mm256_blendv_ps(l1p, h1p, m0);
    h2 = _mm256_blendv_ps(h1p, l1p, m0);
    pl2 = (__m256i)_mm256_blendv_ps((__m256)pl1p, (__m256)ph1p, m0);
    ph2 = (__m256i)_mm256_blendv_ps((__m256)ph1p, (__m256)pl1p, m0);

    // level 3 comparison
    l2p = _mm256_shuffle_ps(l2, h2, _MM_SHUFFLE(3,2,1,0));
    h2p = _mm256_shuffle_ps(l2, h2, _MM_SHUFFLE(1,0,3,2));
    pl2p = (__m256i)_mm256_shuffle_ps((__m256)pl2, (__m256)ph2, _MM_SHUFFLE(3,2,1,0));
    ph2p = (__m256i)_mm256_shuffle_ps((__m256)pl2, (__m256)ph2, _MM_SHUFFLE(1,0,3,2));
    // l3  = _mm256_min_ps(l2p, h2p);
    // h3  = _mm256_max_ps(l2p, h2p);
    m0 = _mm256_cmp_ps(l2p, h2p, _CMP_GT_OS);
    l3 = _mm256_blendv_ps(l2p, h2p, m0);
    h3 = _mm256_blendv_ps(h2p, l2p, m0);
    pl3 = (__m256i)_mm256_blendv_ps((__m256)pl2p, (__m256)ph2p, m0);
    ph3 = (__m256i)_mm256_blendv_ps((__m256)ph2p, (__m256)pl2p, m0);

    // level 4 comparison
    l3p = _mm256_blend_ps(l3, h3, 0xAA);
    ext = _mm256_blend_ps(l3, h3, 0x55);
    h3p = _mm256_shuffle_ps(ext, ext, _MM_SHUFFLE(2,3,0,1));
    pl3p = (__m256i)_mm256_blend_ps((__m256)pl3, (__m256)ph3, 0xAA);
    pext = (__m256i)_mm256_blend_ps((__m256)pl3, (__m256)ph3, 0x55);
    ph3p = (__m256i)_mm256_shuffle_ps((__m256)pext, (__m256)pext, _MM_SHUFFLE(2,3,0,1));
    // l4  = _mm256_min_ps(l3p, h3p);
    // h4  = _mm256_max_ps(l3p, h3p); 
    m0 = _mm256_cmp_ps(l3p, h3p, _CMP_GT_OS);
    l4 = _mm256_blendv_ps(l3p, h3p, m0);
    h4 = _mm256_blendv_ps(h3p, l3p, m0);
    pl4 = (__m256i)_mm256_blendv_ps((__m256)pl3p, (__m256)ph3p, m0);
    ph4 = (__m256i)_mm256_blendv_ps((__m256)ph3p, (__m256)pl3p, m0);

    // final permute/shuffle
    ext1 = _mm256_unpacklo_ps(l4, h4);
    ext2 = _mm256_unpackhi_ps(l4, h4);
    v0 = _mm256_permute2f128_ps(ext1, ext2, 0x20);
    v1 = _mm256_permute2f128_ps(ext1, ext2, 0x31);
    pext1 = (__m256i)_mm256_unpacklo_ps((__m256)pl4, (__m256)ph4);
    pext2 = (__m256i)_mm256_unpackhi_ps((__m256)pl4, (__m256)ph4);
    pv0 = (__m256i)_mm256_permute2f128_ps((__m256)pext1, (__m256)pext2, 0x20);
    pv1 = (__m256i)_mm256_permute2f128_ps((__m256)pext1, (__m256)pext2, 0x31);
}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, __m256>::value>::type
in_register_merge_key(T& v0, T& v1, __m256i &pvl0, __m256i &pvh0, __m256i &pvl1, __m256i &pvh1)
{
    __m256 l1,h1,l1p,h1p,l2,h2,l2p,h2p,l3,h3,l3p,h3p,l4,h4;
    __m256i pll1,plh1,pll1p,plh1p,pll2,plh2,pll2p,plh2p,pll3,plh3,pll3p,plh3p,pll4,plh4;
    __m256i phl1,phh1,phl1p,phh1p,phl2,phh2,phl2p,phh2p,phl3,phh3,phl3p,phh3p,phl4,phh4;
    __m256 ext,ext1,ext2;
    __m256i plext,plext1,plext2;
    __m256i phext,phext1,phext2;
    __m128 sl1, sh1, sl2, sh2;
    __m128 max1, min1, max2, min2;
    __m256 m0;
    __m128i sl, sh;
    __m256d d_sl, d_sh;

    // reverse register v1 
    ext = _mm256_shuffle_ps(v1, v1, _MM_SHUFFLE(0,1,2,3));
    v1  = _mm256_permute2f128_ps(ext, ext, 0x03);
    // pext = (__m256i)_mm256_shuffle_ps((__m256)pv1, (__m256)pv1, _MM_SHUFFLE(0,1,2,3));
    // pv1  = (__m256i)_mm256_permute2f128_ps((__m256)pext, (__m256)pext, 0x03);
    plext = (__m256i)_mm256_shuffle_pd((__m256d)pvl1, (__m256d)pvl1, 0x5);
    phext = (__m256i)_mm256_shuffle_pd((__m256d)pvh1, (__m256d)pvh1, 0x5);
    pvh1 = (__m256i)_mm256_permute2f128_pd((__m256d)plext, (__m256d)plext, 0x01);
    pvl1 = (__m256i)_mm256_permute2f128_pd((__m256d)phext, (__m256d)phext, 0x01);

    // level 1 comparison
    // l1  = _mm256_min_ps(v0, v1);
    // h1  = _mm256_max_ps(v0, v1); 
    m0 = _mm256_cmp_ps(v0, v1, _CMP_GT_OS);
    l1 = _mm256_blendv_ps(v0, v1, m0);
    h1 = _mm256_blendv_ps(v1, v0, m0);
    // pl1 = (__m256i)_mm256_blendv_ps((__m256)pv0, (__m256)pv1, m0);
    // ph1 = (__m256i)_mm256_blendv_ps((__m256)pv1, (__m256)pv0, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll1 = (__m256i)_mm256_blendv_pd((__m256d)pvl0, (__m256d)pvl1, d_sl);
    phl1 = (__m256i)_mm256_blendv_pd((__m256d)pvh0, (__m256d)pvh1, d_sh);
    plh1 = (__m256i)_mm256_blendv_pd((__m256d)pvl1, (__m256d)pvl0, d_sl); 
    phh1 = (__m256i)_mm256_blendv_pd((__m256d)pvh1, (__m256d)pvh0, d_sh);     

    // level 2 comparison
    l1p = _mm256_permute2f128_ps(l1, h1, 0x30);
    h1p = _mm256_permute2f128_ps(l1, h1, 0x21); 
    // pl1p = (__m256i)_mm256_permute2f128_ps((__m256)pl1, (__m256)ph1, 0x30);
    // ph1p = (__m256i)_mm256_permute2f128_ps((__m256)pl1, (__m256)ph1, 0x21); 
    pll1p = pll1;
    phl1p = phh1; 
    plh1p = phl1;
    phh1p = plh1;
    // l2  = _mm256_min_ps(l1p, h1p);
    // h2  = _mm256_max_ps(l1p, h1p); 
    m0 = _mm256_cmp_ps(l1p, h1p, _CMP_GT_OS);
    l2 = _mm256_blendv_ps(l1p, h1p, m0);
    h2 = _mm256_blendv_ps(h1p, l1p, m0);
    // pl2 = (__m256i)_mm256_blendv_ps((__m256)pl1p, (__m256)ph1p, m0);
    // ph2 = (__m256i)_mm256_blendv_ps((__m256)ph1p, (__m256)pl1p, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll2 = (__m256i)_mm256_blendv_pd((__m256d)pll1p, (__m256d)plh1p, d_sl);
    phl2 = (__m256i)_mm256_blendv_pd((__m256d)phl1p, (__m256d)phh1p, d_sh);
    plh2 = (__m256i)_mm256_blendv_pd((__m256d)plh1p, (__m256d)pll1p, d_sl); 
    phh2 = (__m256i)_mm256_blendv_pd((__m256d)phh1p, (__m256d)phl1p, d_sh);

    // level 3 comparison
    l2p = _mm256_shuffle_ps(l2, h2, _MM_SHUFFLE(3,2,1,0));
    h2p = _mm256_shuffle_ps(l2, h2, _MM_SHUFFLE(1,0,3,2));
    // pl2p = (__m256i)_mm256_shuffle_ps((__m256)pl2, (__m256)ph2, _MM_SHUFFLE(3,2,1,0));
    // ph2p = (__m256i)_mm256_shuffle_ps((__m256)pl2, (__m256)ph2, _MM_SHUFFLE(1,0,3,2));
    pll2p = (__m256i)_mm256_permute2f128_pd((__m256d)pll2, (__m256d)plh2, 0x30);
    phl2p = (__m256i)_mm256_permute2f128_pd((__m256d)phl2, (__m256d)phh2, 0x30);
    plh2p = (__m256i)_mm256_permute2f128_pd((__m256d)pll2, (__m256d)plh2, 0x21);
    phh2p = (__m256i)_mm256_permute2f128_pd((__m256d)phl2, (__m256d)phh2, 0x21);
    // l3  = _mm256_min_ps(l2p, h2p);
    // h3  = _mm256_max_ps(l2p, h2p);
    m0 = _mm256_cmp_ps(l2p, h2p, _CMP_GT_OS);
    l3 = _mm256_blendv_ps(l2p, h2p, m0);
    h3 = _mm256_blendv_ps(h2p, l2p, m0);
    // pl3 = (__m256i)_mm256_blendv_ps((__m256)pl2p, (__m256)ph2p, m0);
    // ph3 = (__m256i)_mm256_blendv_ps((__m256)ph2p, (__m256)pl2p, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll3 = (__m256i)_mm256_blendv_pd((__m256d)pll2p, (__m256d)plh2p, d_sl);
    phl3 = (__m256i)_mm256_blendv_pd((__m256d)phl2p, (__m256d)phh2p, d_sh);
    plh3 = (__m256i)_mm256_blendv_pd((__m256d)plh2p, (__m256d)pll2p, d_sl); 
    phh3 = (__m256i)_mm256_blendv_pd((__m256d)phh2p, (__m256d)phl2p, d_sh);

    // level 4 comparison
    l3p = _mm256_blend_ps(l3, h3, 0xAA);
    ext = _mm256_blend_ps(l3, h3, 0x55);
    h3p = _mm256_shuffle_ps(ext, ext, _MM_SHUFFLE(2,3,0,1));
    // pl3p = (__m256i)_mm256_blend_ps((__m256)pl3, (__m256)ph3, 0xAA);
    // pext = (__m256i)_mm256_blend_ps((__m256)pl3, (__m256)ph3, 0x55);
    // ph3p = (__m256i)_mm256_shuffle_ps((__m256)pext, (__m256)pext, _MM_SHUFFLE(2,3,0,1));
    pll3p = (__m256i)_mm256_blend_pd((__m256d)pll3, (__m256d)plh3, 0xa);
    phl3p = (__m256i)_mm256_blend_pd((__m256d)phl3, (__m256d)phh3, 0xa);
    plext = (__m256i)_mm256_blend_pd((__m256d)pll3, (__m256d)plh3, 0x5);
    phext = (__m256i)_mm256_blend_pd((__m256d)phl3, (__m256d)phh3, 0x5);
    plh3p = (__m256i)_mm256_shuffle_pd((__m256d)plext, (__m256d)plext, 0x5);
    phh3p = (__m256i)_mm256_shuffle_pd((__m256d)phext, (__m256d)phext, 0x5);
    // l4  = _mm256_min_ps(l3p, h3p);
    // h4  = _mm256_max_ps(l3p, h3p); 
    m0 = _mm256_cmp_ps(l3p, h3p, _CMP_GT_OS);
    l4 = _mm256_blendv_ps(l3p, h3p, m0);
    h4 = _mm256_blendv_ps(h3p, l3p, m0);
    // pl4 = (__m256i)_mm256_blendv_ps((__m256)pl3p, (__m256)ph3p, m0);
    // ph4 = (__m256i)_mm256_blendv_ps((__m256)ph3p, (__m256)pl3p, m0);
    sl = _mm256_extractf128_si256((__m256i)m0, 0);
    sh = _mm256_extractf128_si256((__m256i)m0, 1);
    d_sl = _mm256_cvtepi32_pd(sl);
    d_sh = _mm256_cvtepi32_pd(sh);
    pll4 = (__m256i)_mm256_blendv_pd((__m256d)pll3p, (__m256d)plh3p, d_sl);
    phl4 = (__m256i)_mm256_blendv_pd((__m256d)phl3p, (__m256d)phh3p, d_sh);
    plh4 = (__m256i)_mm256_blendv_pd((__m256d)plh3p, (__m256d)pll3p, d_sl); 
    phh4 = (__m256i)_mm256_blendv_pd((__m256d)phh3p, (__m256d)phl3p, d_sh);

    // final permute/shuffle
    ext1 = _mm256_unpacklo_ps(l4, h4);
    ext2 = _mm256_unpackhi_ps(l4, h4);
    v0 = _mm256_permute2f128_ps(ext1, ext2, 0x20);
    v1 = _mm256_permute2f128_ps(ext1, ext2, 0x31);
    // pext1 = (__m256i)_mm256_unpacklo_ps((__m256)pl4, (__m256)ph4);
    // pext2 = (__m256i)_mm256_unpackhi_ps((__m256)pl4, (__m256)ph4);
    // pv0 = (__m256i)_mm256_permute2f128_ps((__m256)pext1, (__m256)pext2, 0x20);
    // pv1 = (__m256i)_mm256_permute2f128_ps((__m256)pext1, (__m256)pext2, 0x31);
    plext1 = (__m256i)_mm256_unpacklo_pd((__m256d)pll4, (__m256d)plh4);
    phext1 = (__m256i)_mm256_unpacklo_pd((__m256d)phl4, (__m256d)phh4);
    plext2 = (__m256i)_mm256_unpackhi_pd((__m256d)pll4, (__m256d)plh4);
    phext2 = (__m256i)_mm256_unpackhi_pd((__m256d)phl4, (__m256d)phh4);
    pvl0 = (__m256i)_mm256_permute2f128_pd((__m256d)plext1, (__m256d)plext2, 0x20);
    pvl1 = (__m256i)_mm256_permute2f128_pd((__m256d)phext1, (__m256d)phext2, 0x20);
    pvh0 = (__m256i)_mm256_permute2f128_pd((__m256d)plext1, (__m256d)plext2, 0x31);
    pvh1 = (__m256i)_mm256_permute2f128_pd((__m256d)phext1, (__m256d)phext2, 0x31);
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvl0)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvl1)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvh0)[i] << " "; 
    // for(int i=0;i<4;i++) std::cout << ((long*)&pvh1)[i] << " "; std::cout << std::endl;
}
/**
 * Double vector (__m256d) version:
 * This method performs the in-register merge of two sorted vectors.
 *
 * @param v0 v1 sorted vector registers
 * @return sorted data stored horizontally in the two registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_merge(T& v0, T& v1)
{
    __m256d ext, l1, h1, l1p, h1p, l2, h2, l2p, h2p, l3, h3;
    __m256d ext1, ext2;
    // reverse register v1 
    // ext = _mm256_shuffle_pd(v1, v1, 0x5);
    // v1  = _mm256_permute2f128_pd(ext, ext, 0x03);
    v1  = _mm256_permute4x64_pd(v1, _MM_PERM_ABCD);
    // level 1 comparison
    l1  = _mm256_min_pd(v0, v1);
    h1  = _mm256_max_pd(v0, v1); 
    // level 2 comparison
    l1p = _mm256_permute2f128_pd(l1, h1, 0x30);
    h1p = _mm256_permute2f128_pd(l1, h1, 0x21); 
    l2  = _mm256_min_pd(l1p, h1p);
    h2  = _mm256_max_pd(l1p, h1p); 
    // level 3 comparison
    l2p = _mm256_shuffle_pd(l2, h2, 0x0);
    h2p = _mm256_shuffle_pd(l2, h2, 0xf);
    l3  = _mm256_min_pd(l2p, h2p);
    h3  = _mm256_max_pd(l2p, h2p);
    // final permute/shuffle
    ext1 = _mm256_unpacklo_pd(l3, h3);
    ext2 = _mm256_unpackhi_pd(l3, h3);
    v0 = _mm256_permute2f128_pd(ext1, ext2, 0x20);
    v1 = _mm256_permute2f128_pd(ext1, ext2, 0x31); 
}

template <typename T> // double key int index
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_merge_key(T& v0, T& v1, __m128i &pv0, __m128i &pv1)
{
    __m256d ext, l1, h1, l1p, h1p, l2, h2, l2p, h2p, l3, h3;
    __m128 pext, pl1, ph1, pl1p, ph1p, pl2, ph2, pl2p, ph2p, pl3, ph3;
    __m256d ext1, ext2;
    __m128 pext1, pext2;
    __m256d m0;
    __m128 m0i;
    // reverse register v1 
    ext = _mm256_shuffle_pd(v1, v1, 0x5);
    v1  = _mm256_permute2f128_pd(ext, ext, 0x03);
    // pext = (__m256i)_mm256_shuffle_pd((__m256d)pv1, (__m256d)pv1, 0x5);
    // pv1  = (__m256i)_mm256_permute2f128_pd((__m256d)pext, (__m256d)pext, 0x03);
    pv1 = _mm_shuffle_epi32(pv1, 0x1b);
    // level 1 comparison
    // l1  = _mm256_min_pd(v0, v1);
    // h1  = _mm256_max_pd(v0, v1); 
    m0 = _mm256_cmp_pd(v0, v1, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l1 = _mm256_blendv_pd(v0, v1, m0);
    h1 = _mm256_blendv_pd(v1, v0, m0);
    // pl1 = (__m256i)_mm256_blendv_pd((__m256d)pv0, (__m256d)pv1, m0);
    // ph1 = (__m256i)_mm256_blendv_pd((__m256d)pv1, (__m256d)pv0, m0);
    pl1 = _mm_blendv_ps((__m128)pv0, (__m128)pv1, m0i);
    ph1 = _mm_blendv_ps((__m128)pv1, (__m128)pv0, m0i);

    // level 2 comparison
    l1p = _mm256_permute2f128_pd(l1, h1, 0x30);
    h1p = _mm256_permute2f128_pd(l1, h1, 0x21); 
    pl1p = _mm_shuffle_ps(pl1, ph1, 0xe4);
    ph1p = _mm_shuffle_ps(pl1, ph1, 0x4e); 
    // l2  = _mm256_min_pd(l1p, h1p);
    // h2  = _mm256_max_pd(l1p, h1p); 
    m0 = _mm256_cmp_pd(l1p, h1p, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l2 = _mm256_blendv_pd(l1p, h1p, m0);
    h2 = _mm256_blendv_pd(h1p, l1p, m0);
    // pl2 = (__m256i)_mm256_blendv_pd((__m256d)pl1p, (__m256d)ph1p, m0);
    // ph2 = (__m256i)_mm256_blendv_pd((__m256d)ph1p, (__m256d)pl1p, m0);
    pl2 = _mm_blendv_ps((__m128)pl1p, (__m128)ph1p, m0i);
    ph2 = _mm_blendv_ps((__m128)ph1p, (__m128)pl1p, m0i);

    // level 3 comparison
    l2p = _mm256_shuffle_pd(l2, h2, 0x0);
    h2p = _mm256_shuffle_pd(l2, h2, 0xf);
    // pl2p = (__m256i)_mm256_shuffle_pd((__m256d)pl2, (__m256d)ph2, 0x0);
    // ph2p = (__m256i)_mm256_shuffle_pd((__m256d)pl2, (__m256d)ph2, 0xf);
    pl2p = _mm_permute_ps(_mm_shuffle_ps(pl2, ph2, 0x88), 0xd8);
    ph2p = _mm_permute_ps(_mm_shuffle_ps(pl2, ph2, 0xdd), 0xd8); 
    // l3  = _mm256_min_pd(l2p, h2p);
    // h3  = _mm256_max_pd(l2p, h2p);
    m0 = _mm256_cmp_pd(l2p, h2p, _CMP_GT_OS);
    m0i = (__m128)_mm256_cvtpd_epi32(m0); 
    l3 = _mm256_blendv_pd(l2p, h2p, m0);
    h3 = _mm256_blendv_pd(h2p, l2p, m0);
    // pl3 = (__m256i)_mm256_blendv_pd((__m256d)pl2p, (__m256d)ph2p, m0);
    // ph3 = (__m256i)_mm256_blendv_pd((__m256d)ph2p, (__m256d)pl2p, m0);
    pl3 = _mm_blendv_ps((__m128)pl2p, (__m128)ph2p, m0i);
    ph3 = _mm_blendv_ps((__m128)ph2p, (__m128)pl2p, m0i);
    // final permute/shuffle
    // for(int i=0;i<4; i++) std::cout << ((double*)&l3)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4; i++) std::cout << ((double*)&h3)[i] << " "; std::cout << std::endl;
    ext1 = _mm256_unpacklo_pd(l3, h3);
    ext2 = _mm256_unpackhi_pd(l3, h3);
    v0 = _mm256_permute2f128_pd(ext1, ext2, 0x20);
    v1 = _mm256_permute2f128_pd(ext1, ext2, 0x31); 

    // for(int i=0;i<4; i++) std::cout << ((double*)&v0)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4; i++) std::cout << ((double*)&v1)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4; i++) std::cout << ((int*)&pl3)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4; i++) std::cout << ((int*)&ph3)[i] << " "; std::cout << std::endl;
    // pext1 = (__m256i)_mm256_unpacklo_pd((__m256d)pl3, (__m256d)ph3);
    // pext2 = (__m256i)_mm256_unpackhi_pd((__m256d)pl3, (__m256d)ph3);
    // pv0 = (__m256i)_mm256_permute2f128_pd((__m256d)pext1, (__m256d)pext2, 0x20);
    // pv1 = (__m256i)_mm256_permute2f128_pd((__m256d)pext1, (__m256d)pext2, 0x31); 
    pv0 = (__m128i)_mm_permute_ps(_mm_shuffle_ps(pl3, ph3, 0x44), 0xd8);
    pv1 = (__m128i)_mm_permute_ps(_mm_shuffle_ps(pl3, ph3, 0xee), 0xd8); 
    // for(int i=0;i<4; i++) std::cout << ((int*)&pv0)[i] << " "; std::cout << std::endl;
    // for(int i=0;i<4; i++) std::cout << ((int*)&pv1)[i] << " "; std::cout << std::endl;
    /*
    */
}
template <typename T> // double key long index
typename std::enable_if<std::is_same<T, __m256d>::value>::type
in_register_merge_key(T& v0, T& v1, __m256i &pv0, __m256i &pv1)
{
    __m256d ext, l1, h1, l1p, h1p, l2, h2, l2p, h2p, l3, h3;
    __m256i pext, pl1, ph1, pl1p, ph1p, pl2, ph2, pl2p, ph2p, pl3, ph3;
    __m256d ext1, ext2;
    __m256i pext1, pext2;
    __m256d m0;
    // reverse register v1 
    ext = _mm256_shuffle_pd(v1, v1, 0x5);
    v1  = _mm256_permute2f128_pd(ext, ext, 0x03);
    pext = (__m256i)_mm256_shuffle_pd((__m256d)pv1, (__m256d)pv1, 0x5);
    pv1  = (__m256i)_mm256_permute2f128_pd((__m256d)pext, (__m256d)pext, 0x03);
    // level 1 comparison
    // l1  = _mm256_min_pd(v0, v1);
    // h1  = _mm256_max_pd(v0, v1); 
    m0 = _mm256_cmp_pd(v0, v1, _CMP_GT_OS);
    l1 = _mm256_blendv_pd(v0, v1, m0);
    h1 = _mm256_blendv_pd(v1, v0, m0);
    pl1 = (__m256i)_mm256_blendv_pd((__m256d)pv0, (__m256d)pv1, m0);
    ph1 = (__m256i)_mm256_blendv_pd((__m256d)pv1, (__m256d)pv0, m0);

    // level 2 comparison
    l1p = _mm256_permute2f128_pd(l1, h1, 0x30);
    h1p = _mm256_permute2f128_pd(l1, h1, 0x21); 
    pl1p = (__m256i)_mm256_permute2f128_pd((__m256d)pl1, (__m256d)ph1, 0x30);
    ph1p = (__m256i)_mm256_permute2f128_pd((__m256d)pl1, (__m256d)ph1, 0x21); 
    // l2  = _mm256_min_pd(l1p, h1p);
    // h2  = _mm256_max_pd(l1p, h1p); 
    m0 = _mm256_cmp_pd(l1p, h1p, _CMP_GT_OS);
    l2 = _mm256_blendv_pd(l1p, h1p, m0);
    h2 = _mm256_blendv_pd(h1p, l1p, m0);
    pl2 = (__m256i)_mm256_blendv_pd((__m256d)pl1p, (__m256d)ph1p, m0);
    ph2 = (__m256i)_mm256_blendv_pd((__m256d)ph1p, (__m256d)pl1p, m0);

    // level 3 comparison
    l2p = _mm256_shuffle_pd(l2, h2, 0x0);
    h2p = _mm256_shuffle_pd(l2, h2, 0xf);
    pl2p = (__m256i)_mm256_shuffle_pd((__m256d)pl2, (__m256d)ph2, 0x0);
    ph2p = (__m256i)_mm256_shuffle_pd((__m256d)pl2, (__m256d)ph2, 0xf);
    // l3  = _mm256_min_pd(l2p, h2p);
    // h3  = _mm256_max_pd(l2p, h2p);
    m0 = _mm256_cmp_pd(l2p, h2p, _CMP_GT_OS);
    l3 = _mm256_blendv_pd(l2p, h2p, m0);
    h3 = _mm256_blendv_pd(h2p, l2p, m0);
    pl3 = (__m256i)_mm256_blendv_pd((__m256d)pl2p, (__m256d)ph2p, m0);
    ph3 = (__m256i)_mm256_blendv_pd((__m256d)ph2p, (__m256d)pl2p, m0);
    // final permute/shuffle
    ext1 = _mm256_unpacklo_pd(l3, h3);
    ext2 = _mm256_unpackhi_pd(l3, h3);
    v0 = _mm256_permute2f128_pd(ext1, ext2, 0x20);
    v1 = _mm256_permute2f128_pd(ext1, ext2, 0x31); 
    pext1 = (__m256i)_mm256_unpacklo_pd((__m256d)pl3, (__m256d)ph3);
    pext2 = (__m256i)_mm256_unpackhi_pd((__m256d)pl3, (__m256d)ph3);
    pv0 = (__m256i)_mm256_permute2f128_pd((__m256d)pext1, (__m256d)pext2, 0x20);
    pv1 = (__m256i)_mm256_permute2f128_pd((__m256d)pext1, (__m256d)pext2, 0x31); 
}

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output)
{
    __m256i vec0;
    __m256i vec1;
    
    uint8_t stride = (uint8_t)simd_width::AVX_INT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    int buffer[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_si256((__m256i*)inputA);
        vec1  = _mm256_loadu_si256((__m256i*)inputB);

        in_register_merge(vec0, vec1);

        _mm256_storeu_si256((__m256i*)(output + iout), vec0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputA+i0));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputB+i1));
                i1 += stride;
            }
            in_register_merge(vec0, vec1);
            _mm256_storeu_si256((__m256i*)(output + iout), vec0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputA+i0));
                i0 += stride;
                in_register_merge(vec0, vec1);
                _mm256_storeu_si256((__m256i*)(output + iout), vec0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputB+i1));
                i1 += stride;
                in_register_merge(vec0, vec1);
                _mm256_storeu_si256((__m256i*)(output + iout), vec0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_si256((__m256i*)buffer, vec1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            i1++;
            iout++;
        }
    }
}

template <typename T> // int key int index
typename std::enable_if<std::is_same<T, int>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr)
{
    __m256i vec0;
    __m256i vec1;
    
    __m256i p0;
    __m256i p1;
    uint8_t stride = (uint8_t)simd_width::AVX_INT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    int buffer[stride];
    int buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_si256((__m256i*)inputA);
        vec1  = _mm256_loadu_si256((__m256i*)inputB);

        p0  = _mm256_loadu_si256((__m256i*)ptrA);
        p1  = _mm256_loadu_si256((__m256i*)ptrB);

        in_register_merge_key(vec0, vec1, p0, p1);

        _mm256_storeu_si256((__m256i*)(output + iout), vec0);
        _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputA+i0));
                p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputB+i1));
                p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, p0, p1);
            _mm256_storeu_si256((__m256i*)(output + iout), vec0);
            _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputA+i0));
                p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                i0 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_si256((__m256i*)(output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputB+i1));
                p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                i1 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_si256((__m256i*)(output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_si256((__m256i*)buffer, vec1);
        _mm256_storeu_si256((__m256i*)buffer_ptr, p1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            output_ptr[iout]=buffer_ptr[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
    }
}

template <typename T> // int key long index
typename std::enable_if<std::is_same<T, int>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr)
{
    __m256i vec0;
    __m256i vec1;
    
    __m256i pl0;
    __m256i ph0;
    __m256i pl1;
    __m256i ph1;
    uint8_t stride = (uint8_t)simd_width::AVX_INT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    int buffer[stride];
    long buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_si256((__m256i*)inputA);
        vec1  = _mm256_loadu_si256((__m256i*)inputB);

        pl0 = _mm256_loadu_si256((__m256i*)(ptrA    ));
        ph0 = _mm256_loadu_si256((__m256i*)(ptrA + 4));
        pl1 = _mm256_loadu_si256((__m256i*)(ptrB    ));
        ph1 = _mm256_loadu_si256((__m256i*)(ptrB + 4));

        in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);

        _mm256_storeu_si256((__m256i*)(output + iout), vec0);
        _mm256_storeu_si256((__m256i*)(output_ptr + iout    ), pl0);
        _mm256_storeu_si256((__m256i*)(output_ptr + iout + 4), ph0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputA+i0));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrA+i0  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrA+i0+4));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputB+i1));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrB+i1  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrB+i1+4));
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);
            _mm256_storeu_si256((__m256i*)(output + iout), vec0);
            _mm256_storeu_si256((__m256i*)(output_ptr + iout  ), pl0);
            _mm256_storeu_si256((__m256i*)(output_ptr + iout+4), ph0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputA+i0));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrA+i0  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrA+i0+4));
                i0 += stride;
                in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);
                _mm256_storeu_si256((__m256i*)(output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout  ), pl0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout+4), ph0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_si256((__m256i*)(inputB+i1));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrB+i1  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrB+i1+4));
                i1 += stride;
                in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);
                _mm256_storeu_si256((__m256i*)(output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout    ), pl0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout + 4), ph0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_si256((__m256i*)buffer, vec1);
        _mm256_storeu_si256((__m256i*)(buffer_ptr  ), pl1);
        _mm256_storeu_si256((__m256i*)(buffer_ptr+4), ph1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            output_ptr[iout]=buffer_ptr[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
    }
}

template <typename T>
typename std::enable_if<std::is_same<T, float>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output)
{
    __m256 vec0;
    __m256 vec1;
    
    uint8_t stride = (uint8_t)simd_width::AVX_FLOAT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    float buffer[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_ps(inputA);
        vec1  = _mm256_loadu_ps(inputB);

        in_register_merge(vec0, vec1);

        _mm256_storeu_ps((output + iout), vec0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_ps((inputA+i0));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_ps((inputB+i1));
                i1 += stride;
            }
            in_register_merge(vec0, vec1);
            _mm256_storeu_ps((output + iout), vec0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_ps((inputA+i0));
                i0 += stride;
                in_register_merge(vec0, vec1);
                _mm256_storeu_ps((output + iout), vec0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_ps((inputB+i1));
                i1 += stride;
                in_register_merge(vec0, vec1);
                _mm256_storeu_ps((output + iout), vec0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_ps(buffer, vec1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            i1++;
            iout++;
        }
    }
}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, float>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr)
{
    __m256 vec0;
    __m256 vec1;
    
    __m256i p0;
    __m256i p1;
    uint8_t stride = (uint8_t)simd_width::AVX_FLOAT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    float buffer[stride];
    int buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_ps(inputA);
        vec1  = _mm256_loadu_ps(inputB);

        p0  = _mm256_loadu_si256((__m256i*)ptrA);
        p1  = _mm256_loadu_si256((__m256i*)ptrB);

        in_register_merge_key(vec0, vec1, p0, p1);

        _mm256_storeu_ps((output + iout), vec0);
        _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_ps((inputA+i0));
                p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_ps((inputB+i1));
                p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, p0, p1);
            _mm256_storeu_ps((output + iout), vec0);
            _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_ps((inputA+i0));
                p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                i0 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_ps((output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_ps((inputB+i1));
                p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                i1 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_ps((output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_ps(buffer, vec1);
        _mm256_storeu_si256((__m256i*)buffer_ptr, p1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            output_ptr[iout]=buffer_ptr[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
    }
}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, float>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr)
{
    __m256 vec0;
    __m256 vec1;
    
    // __m256i p0;
    // __m256i p1;
    __m256i pl0;
    __m256i ph0;
    __m256i pl1;
    __m256i ph1;
    uint8_t stride = (uint8_t)simd_width::AVX_FLOAT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    float buffer[stride];
    // int buffer_ptr[stride];
    long buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_ps(inputA);
        vec1  = _mm256_loadu_ps(inputB);

        // p0  = _mm256_loadu_si256((__m256i*)ptrA);
        // p1  = _mm256_loadu_si256((__m256i*)ptrB);
        pl0 = _mm256_loadu_si256((__m256i*)(ptrA    ));
        ph0 = _mm256_loadu_si256((__m256i*)(ptrA + 4));
        pl1 = _mm256_loadu_si256((__m256i*)(ptrB    ));
        ph1 = _mm256_loadu_si256((__m256i*)(ptrB + 4));

        // in_register_merge_key(vec0, vec1, p0, p1);
        in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);

        _mm256_storeu_ps((output + iout), vec0);
        // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
        _mm256_storeu_si256((__m256i*)(output_ptr + iout    ), pl0);
        _mm256_storeu_si256((__m256i*)(output_ptr + iout + 4), ph0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_ps((inputA+i0));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrA+i0  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrA+i0+4));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_ps((inputB+i1));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrB+i1  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrB+i1+4));
                i1 += stride;
            }
            // in_register_merge_key(vec0, vec1, p0, p1);
            in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);
            _mm256_storeu_ps((output + iout), vec0);
            // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
            _mm256_storeu_si256((__m256i*)(output_ptr + iout  ), pl0);
            _mm256_storeu_si256((__m256i*)(output_ptr + iout+4), ph0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_ps((inputA+i0));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrA+i0  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrA+i0+4));
                i0 += stride;
                // in_register_merge_key(vec0, vec1, p0, p1);
                in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);
                _mm256_storeu_ps((output + iout), vec0);
                // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout  ), pl0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout+4), ph0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_ps((inputB+i1));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                pl0  = _mm256_loadu_si256((__m256i*)(ptrB+i1  ));
                ph0  = _mm256_loadu_si256((__m256i*)(ptrB+i1+4));
                i1 += stride;
                // in_register_merge_key(vec0, vec1, p0, p1);
                in_register_merge_key(vec0, vec1, pl0, ph0, pl1, ph1);
                _mm256_storeu_ps((output + iout), vec0);
                // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout    ), pl0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout + 4), ph0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_ps(buffer, vec1);
        // _mm256_storeu_si256((__m256i*)buffer_ptr, p1);
        _mm256_storeu_si256((__m256i*)(buffer_ptr  ), pl1);
        _mm256_storeu_si256((__m256i*)(buffer_ptr+4), ph1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            output_ptr[iout]=buffer_ptr[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
    }
}

template <typename T>
typename std::enable_if<std::is_same<T, double>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output)
{
    __m256d vec0;
    __m256d vec1;
    
    uint8_t stride = (uint8_t)simd_width::AVX_DOUBLE;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    double buffer[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_pd(inputA);
        vec1  = _mm256_loadu_pd(inputB);

        in_register_merge(vec0, vec1);

        _mm256_storeu_pd((output + iout), vec0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_pd((inputA+i0));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_pd((inputB+i1));
                i1 += stride;
            }
            in_register_merge(vec0, vec1);
            _mm256_storeu_pd((output + iout), vec0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_pd((inputA+i0));
                i0 += stride;
                in_register_merge(vec0, vec1);
                _mm256_storeu_pd((output + iout), vec0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_pd((inputB+i1));
                i1 += stride;
                in_register_merge(vec0, vec1);
                _mm256_storeu_pd((output + iout), vec0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_pd(buffer, vec1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            i1++;
            iout++;
        }
    }
}

template <typename T> // double key int index
typename std::enable_if<std::is_same<T, double>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr)
{
    __m256d vec0;
    __m256d vec1;

    __m128i p0;
    __m128i p1;
    uint8_t stride = (uint8_t)simd_width::AVX_DOUBLE;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    double buffer[stride];
    int buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_pd(inputA);
        vec1  = _mm256_loadu_pd(inputB);

        p0  = _mm_lddqu_si128((__m128i*)ptrA);
        p1  = _mm_lddqu_si128((__m128i*)ptrB);

        in_register_merge_key(vec0, vec1, p0, p1);

        _mm256_storeu_pd((output + iout), vec0);
        // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
        _mm_storeu_si128((__m128i*)(output_ptr + iout), p0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_pd((inputA+i0));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                p0  = _mm_lddqu_si128((__m128i*)(ptrA+i0));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_pd((inputB+i1));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                p0  = _mm_lddqu_si128((__m128i*)(ptrB+i1));
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, p0, p1);
            _mm256_storeu_pd((output + iout), vec0);
            // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
            _mm_storeu_si128((__m128i*)(output_ptr + iout), p0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_pd((inputA+i0));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                p0  = _mm_lddqu_si128((__m128i*)(ptrA+i0));
                i0 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_pd((output + iout), vec0);
                // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                _mm_storeu_si128((__m128i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_pd((inputB+i1));
                // p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                p0  = _mm_lddqu_si128((__m128i*)(ptrB+i1));
                i1 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_pd((output + iout), vec0);
                // _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                _mm_storeu_si128((__m128i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_pd(buffer, vec1);
        // _mm256_storeu_si256((__m256i*)buffer_ptr, p1);
        _mm_storeu_si128((__m128i*)buffer_ptr, p1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            output_ptr[iout]=buffer_ptr[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
    }
}
template <typename T> // double key long index
typename std::enable_if<std::is_same<T, double>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr)
{
    __m256d vec0;
    __m256d vec1;

    __m256i p0;
    __m256i p1;
    uint8_t stride = (uint8_t)simd_width::AVX_DOUBLE;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    double buffer[stride];
    long buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm256_loadu_pd(inputA);
        vec1  = _mm256_loadu_pd(inputB);

        p0  = _mm256_loadu_si256((__m256i*)ptrA);
        p1  = _mm256_loadu_si256((__m256i*)ptrB);

        in_register_merge_key(vec0, vec1, p0, p1);

        _mm256_storeu_pd((output + iout), vec0);
        _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm256_loadu_pd((inputA+i0));
                p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                i0 += stride;
            }else
            {
                vec0  = _mm256_loadu_pd((inputB+i1));
                p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, p0, p1);
            _mm256_storeu_pd((output + iout), vec0);
            _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm256_loadu_pd((inputA+i0));
                p0  = _mm256_loadu_si256((__m256i*)(ptrA+i0));
                i0 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_pd((output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm256_loadu_pd((inputB+i1));
                p0  = _mm256_loadu_si256((__m256i*)(ptrB+i1));
                i1 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm256_storeu_pd((output + iout), vec0);
                _mm256_storeu_si256((__m256i*)(output_ptr + iout), p0);
                iout += stride;
            }else
                break;
        }
        _mm256_storeu_pd(buffer, vec1);
        _mm256_storeu_si256((__m256i*)buffer_ptr, p1);

        while(i0 < sizeA && i1 < sizeB && i3 < stride)
        {
            if(inputA[i0]<=inputB[i1] && inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else if(inputB[i1]<=inputA[i0] && inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else if(buffer[i3]<=inputA[i0] && buffer[i3]<=inputB[i1])
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i1 < sizeB && i3 < stride)
        {
            if(inputB[i1]<=buffer[i3])
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }

        }
        while(i0 < sizeA && i3 < stride)
        {
            if(inputA[i0]<=buffer[i3])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else
            {
                output[iout]=buffer[i3];
                output_ptr[iout]=buffer_ptr[i3];
                i3++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
        while(i3 < stride)
        {
            output[iout]=buffer[i3];
            output_ptr[iout]=buffer_ptr[i3];
            i3++;
            iout++;
        }

    } else
    {
        while(i0 < sizeA && i1 < sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                output[iout]=inputA[i0];
                output_ptr[iout]=ptrA[i0];
                i0++;
                iout++;
            }else 
            {
                output[iout]=inputB[i1];
                output_ptr[iout]=ptrB[i1];
                i1++;
                iout++;
            }
        }
        while(i0 < sizeA)
        {
            output[iout]=inputA[i0];
            output_ptr[iout]=ptrA[i0];
            i0++;
            iout++;
        }
        while(i1 < sizeB)
        {
            output[iout]=inputB[i1];
            output_ptr[iout]=ptrB[i1];
            i1++;
            iout++;
        }
    }
}

} // end namespace aspas
