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
 * @file aspas_merge_mic.tcc
 * Definiation of the merge function in AVX512 instruction sets.
 *
 */

#include <immintrin.h>
#include <cstdint>

#include "aspas.h" 
#include "util/extintrin.h"

namespace aspas
{

/**
 * Integer vector (__m512i) version:
 * This method performs the in-register merge of two sorted vectors.
 *
 * @param v0 v1 sorted vector registers
 * @return sorted data stored horizontally in the two registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_merge(T& v0, T& v1)
{
    __m512i t0, l, h, l_inter, h_inter, l_shuffle, h_shuffle, r1, r2;
    __m512i reverse;
    t0 = _mm512_permute4f128_epi32(v1, _MM_PERM_ABCD);
    reverse = _mm512_shuffle_epi32(t0, _MM_PERM_ABCD);
    l = _mm512_min_epi32(v0, reverse);
    h = _mm512_max_epi32(v0, reverse); // level 1 comparison

    l_inter = _mm512_mask_permute4f128_epi32(l, 0xff00, h, _MM_PERM_BADC);
    h_inter = _mm512_mask_permute4f128_epi32(h, 0x00ff, l, _MM_PERM_BADC);
    l = _mm512_min_epi32(l_inter, h_inter);
    h = _mm512_max_epi32(l_inter, h_inter); // level 2 comparison

    l_inter = _mm512_mask_permute4f128_epi32(l, 0xf0f0, h, _MM_PERM_CDAB);
    h_inter = _mm512_mask_permute4f128_epi32(h, 0x0f0f, l, _MM_PERM_CDAB);
    l = _mm512_min_epi32(l_inter, h_inter);
    h = _mm512_max_epi32(l_inter, h_inter); // level 4 comparison

    l_inter = _mm512_mask_swizzle_epi32(l, 0xcccc, h, _MM_SWIZ_REG_BADC);
    h_inter = _mm512_mask_swizzle_epi32(h, 0x3333, l, _MM_SWIZ_REG_BADC);
    l = _mm512_min_epi32(l_inter, h_inter);
    h = _mm512_max_epi32(l_inter, h_inter); // level 5 comparison

    l_inter = _mm512_mask_swizzle_epi32(l, 0xaaaa, h, _MM_SWIZ_REG_CDAB);
    h_inter = _mm512_mask_swizzle_epi32(h, 0x5555, l, _MM_SWIZ_REG_CDAB);
    l = _mm512_min_epi32(l_inter, h_inter);
    h = _mm512_max_epi32(l_inter, h_inter); // level 6 comparison

    __m512i t1;
    l = _mm512_permute4f128_epi32(l, _MM_PERM_BDAC);
    h = _mm512_permute4f128_epi32(h, _MM_PERM_BDAC);
    t0  = _mm512_mask_swizzle_epi32(h, 0xcccc, l, _MM_SWIZ_REG_BADC); 
    t1  = _mm512_mask_swizzle_epi32(l, 0x3333, h, _MM_SWIZ_REG_BADC); 
    l = _mm512_mask_permute4f128_epi32(t1 , 0x0f0f, t0 , _MM_PERM_CDAB); 
    h = _mm512_mask_permute4f128_epi32(t0 , 0xf0f0, t1 , _MM_PERM_CDAB); 
    r1 = _mm512_shuffle_epi32(l, _MM_PERM_BDAC);
    r2 = _mm512_shuffle_epi32(h, _MM_PERM_BDAC);

    v0 = r1;
    v1 = r2;
}

/**
 * Integer vector (__m512i) version:
 * This method performs the in-register merge of two sorted vectors.
 *
 * @param v0 v1 sorted vector registers
 * @return sorted data stored horizontally in the two registers
 *
 */
template <typename T> // int key int index
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_merge_key(T& v0, T& v1, __m512i &p0, __m512i &p1)
{
    __m512i t0, l, h, l_inter, h_inter, l_shuffle, h_shuffle, r1, r2;
    __m512i pt0, pl, ph, pl_inter, ph_inter, pl_shuffle, ph_shuffle, pr1, pr2;
    __m512i reverse;
    __m512i preverse;
    __mmask16 m0 ;
    t0 = _mm512_permute4f128_epi32(v1, _MM_PERM_ABCD);
    reverse = _mm512_shuffle_epi32(t0, _MM_PERM_ABCD);
    pt0 = _mm512_permute4f128_epi32(p1, _MM_PERM_ABCD);
    preverse = _mm512_shuffle_epi32(pt0, _MM_PERM_ABCD);

    // l = _mm512_min_epi32(v0, reverse);
    // h = _mm512_max_epi32(v0, reverse); // level 1 comparison
    m0 = _mm512_cmp_epi32_mask(v0 , reverse, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_epi32(v0 , m0, reverse, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(reverse, m0, v0, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(p0 , m0, preverse, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(preverse, m0, p0, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_permute4f128_epi32(l, 0xff00, h, _MM_PERM_BADC);
    h_inter = _mm512_mask_permute4f128_epi32(h, 0x00ff, l, _MM_PERM_BADC);
    pl_inter = _mm512_mask_permute4f128_epi32(pl, 0xff00, ph, _MM_PERM_BADC);
    ph_inter = _mm512_mask_permute4f128_epi32(ph, 0x00ff, pl, _MM_PERM_BADC);

    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 2 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_permute4f128_epi32(l, 0xf0f0, h, _MM_PERM_CDAB);
    h_inter = _mm512_mask_permute4f128_epi32(h, 0x0f0f, l, _MM_PERM_CDAB);
    pl_inter = _mm512_mask_permute4f128_epi32(pl, 0xf0f0, ph, _MM_PERM_CDAB);
    ph_inter = _mm512_mask_permute4f128_epi32(ph, 0x0f0f, pl, _MM_PERM_CDAB);
    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 4 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_swizzle_epi32(l, 0xcccc, h, _MM_SWIZ_REG_BADC);
    h_inter = _mm512_mask_swizzle_epi32(h, 0x3333, l, _MM_SWIZ_REG_BADC);
    pl_inter = _mm512_mask_swizzle_epi32(pl, 0xcccc, ph, _MM_SWIZ_REG_BADC);
    ph_inter = _mm512_mask_swizzle_epi32(ph, 0x3333, pl, _MM_SWIZ_REG_BADC);
    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 5 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_swizzle_epi32(l, 0xaaaa, h, _MM_SWIZ_REG_CDAB);
    h_inter = _mm512_mask_swizzle_epi32(h, 0x5555, l, _MM_SWIZ_REG_CDAB);
    pl_inter = _mm512_mask_swizzle_epi32(pl, 0xaaaa, ph, _MM_SWIZ_REG_CDAB);
    ph_inter = _mm512_mask_swizzle_epi32(ph, 0x5555, pl, _MM_SWIZ_REG_CDAB);
    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 6 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    __m512i t1;
    __m512i pt1;
    l = _mm512_permute4f128_epi32(l, _MM_PERM_BDAC);
    h = _mm512_permute4f128_epi32(h, _MM_PERM_BDAC);
    pl = _mm512_permute4f128_epi32(pl, _MM_PERM_BDAC);
    ph = _mm512_permute4f128_epi32(ph, _MM_PERM_BDAC);
    t0  = _mm512_mask_swizzle_epi32(h, 0xcccc, l, _MM_SWIZ_REG_BADC); 
    t1  = _mm512_mask_swizzle_epi32(l, 0x3333, h, _MM_SWIZ_REG_BADC); 
    pt0  = _mm512_mask_swizzle_epi32(ph, 0xcccc, pl, _MM_SWIZ_REG_BADC); 
    pt1  = _mm512_mask_swizzle_epi32(pl, 0x3333, ph, _MM_SWIZ_REG_BADC); 
    l = _mm512_mask_permute4f128_epi32(t1 , 0x0f0f, t0 , _MM_PERM_CDAB); 
    h = _mm512_mask_permute4f128_epi32(t0 , 0xf0f0, t1 , _MM_PERM_CDAB); 
    pl = _mm512_mask_permute4f128_epi32(pt1 , 0x0f0f, pt0 , _MM_PERM_CDAB); 
    ph = _mm512_mask_permute4f128_epi32(pt0 , 0xf0f0, pt1 , _MM_PERM_CDAB); 
    r1 = _mm512_shuffle_epi32(l, _MM_PERM_BDAC);
    r2 = _mm512_shuffle_epi32(h, _MM_PERM_BDAC);
    pr1 = _mm512_shuffle_epi32(pl, _MM_PERM_BDAC);
    pr2 = _mm512_shuffle_epi32(ph, _MM_PERM_BDAC);

    v0 = r1;
    v1 = r2;
    p0 = pr1;
    p1 = pr2;
}

void show_lh(__m512i x, __m512i y, std::string a)
{
    std::cout << a << std::endl;
    for(int i=0; i< 8; i++)
        std::cout <<((long*)&x)[i] << " ";
    // std::cout << std::endl;
    for(int i=0; i< 8; i++)
        std::cout <<((long*)&y)[i] << " ";
    std::cout << std::endl;
}
void show_bs(__m512i x, std::string a)
{
    std::cout << a << std::endl;
    for(int i=0; i< 16; i++)
        std::cout <<((int*)&x)[i] << " ";
    std::cout << std::endl;
}
template <typename T> // int key long index
typename std::enable_if<std::is_same<T, __m512i>::value>::type
in_register_merge_key(T& v0, T& v1, __m512i &pl0, __m512i &pl1, __m512i &ph0, __m512i &ph1)
{
    __m512i t0, l, h, l_inter, h_inter, l_shuffle, h_shuffle, r1, r2;
    __m512i plt0, pll, plh, pll_inter, plh_inter, pll_shuffle, plh_shuffle, plr1, plr2;
    __m512i pht0, phl, phh, phl_inter, phh_inter, phl_shuffle, phh_shuffle, phr1, phr2;
    __m512i reverse;
    __m512i plreverse;
    __m512i phreverse;
    __mmask16 m0 ;

    // key
    t0 = _mm512_permute4f128_epi32(v1, _MM_PERM_ABCD);
    reverse = _mm512_shuffle_epi32(t0, _MM_PERM_ABCD);

    // index
    // show_lh(pl1, ph1, "x");
    pht0 = _mm512_permute4f128_epi32(ph1, _MM_PERM_ABCD);
    plreverse = _mm512_shuffle_epi32(pht0, _MM_PERM_BADC);
    plt0 = _mm512_permute4f128_epi32(pl1, _MM_PERM_ABCD);
    phreverse = _mm512_shuffle_epi32(plt0, _MM_PERM_BADC);
    // show_lh(plreverse, phreverse, "y");

    // l = _mm512_min_epi32(v0, reverse);
    // h = _mm512_max_epi32(v0, reverse); // level 1 comparison
    m0 = _mm512_cmp_epi32_mask(v0 , reverse, _MM_CMPINT_GT);
    // key
    l  = _mm512_mask_swizzle_epi32(v0 , m0, reverse, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(reverse, m0, v0, _MM_SWIZ_REG_NONE);
    // index
    pll  = _mm512_mask_swizzle_epi64(pl0 , m0   , plreverse, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(ph0 , m0>>8, phreverse, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plreverse, m0   , pl0, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phreverse, m0>>8, ph0, _MM_SWIZ_REG_NONE);

    // key
    // show_bs(l, "ba");
    // show_bs(h, "ba");
    l_inter = _mm512_mask_permute4f128_epi32(l, 0xff00, h, _MM_PERM_BADC);
    h_inter = _mm512_mask_permute4f128_epi32(h, 0x00ff, l, _MM_PERM_BADC);
    // show_bs(l_inter, "bb");
    // show_bs(h_inter, "bb");
    // index
    // show_lh(pll, phl, "ta");
    // show_lh(plh, phh, "ta");
    pll_inter = pll;
    phl_inter = plh;
    plh_inter = phl;
    phh_inter = phh;
    // show_lh(pll_inter, phl_inter, "tb");
    // show_lh(plh_inter, phh_inter, "tb");

    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 2 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    // key
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    // index
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    // show_bs(l, "ba");
    // show_bs(h, "ba");
    l_inter = _mm512_mask_permute4f128_epi32(l, 0xf0f0, h, _MM_PERM_CDAB);
    h_inter = _mm512_mask_permute4f128_epi32(h, 0x0f0f, l, _MM_PERM_CDAB);
    // show_bs(l_inter, "bb");
    // show_bs(h_inter, "bb");
    // show_lh(pll, phl, "ta");
    // show_lh(plh, phh, "ta");
    pll_inter = _mm512_mask_permute4f128_epi32(pll, 0xff00, plh, _MM_PERM_BADC);
    phl_inter = _mm512_mask_permute4f128_epi32(phl, 0xff00, phh, _MM_PERM_BADC);
    plh_inter = _mm512_mask_permute4f128_epi32(plh, 0x00ff, pll, _MM_PERM_BADC);
    phh_inter = _mm512_mask_permute4f128_epi32(phh, 0x00ff, phl, _MM_PERM_BADC);
    // show_lh(pll_inter, phl_inter, "tb");
    // show_lh(plh_inter, phh_inter, "tb");

    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 4 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    // key
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    // index
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    // show_bs(l, "ba");
    // show_bs(h, "ba");
    l_inter = _mm512_mask_swizzle_epi32(l, 0xcccc, h, _MM_SWIZ_REG_BADC);
    h_inter = _mm512_mask_swizzle_epi32(h, 0x3333, l, _MM_SWIZ_REG_BADC);
    // show_bs(l_inter, "bb");
    // show_bs(h_inter, "bb");
    // show_lh(pll, phl, "ta");
    // show_lh(plh, phh, "ta");
    pll_inter = _mm512_mask_swizzle_epi64(pll, 0xcc, plh, _MM_SWIZ_REG_BADC);
    phl_inter = _mm512_mask_swizzle_epi64(phl, 0xcc, phh, _MM_SWIZ_REG_BADC);
    plh_inter = _mm512_mask_swizzle_epi64(plh, 0x33, pll, _MM_SWIZ_REG_BADC);
    phh_inter = _mm512_mask_swizzle_epi64(phh, 0x33, phl, _MM_SWIZ_REG_BADC);
    // show_lh(pll_inter, phl_inter, "tb");
    // show_lh(plh_inter, phh_inter, "tb");

    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 5 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    // key
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    // index
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    // show_bs(l, "ba");
    // show_bs(h, "ba");
    l_inter = _mm512_mask_swizzle_epi32(l, 0xaaaa, h, _MM_SWIZ_REG_CDAB);
    h_inter = _mm512_mask_swizzle_epi32(h, 0x5555, l, _MM_SWIZ_REG_CDAB);
    // show_bs(l_inter, "bb");
    // show_bs(h_inter, "bb");
    // show_lh(pll, phl, "ta");
    // show_lh(plh, phh, "ta");
    pll_inter = _mm512_mask_swizzle_epi64(pll, 0xaa, plh, _MM_SWIZ_REG_CDAB);
    phl_inter = _mm512_mask_swizzle_epi64(phl, 0xaa, phh, _MM_SWIZ_REG_CDAB);
    plh_inter = _mm512_mask_swizzle_epi64(plh, 0x55, pll, _MM_SWIZ_REG_CDAB);
    phh_inter = _mm512_mask_swizzle_epi64(phh, 0x55, phl, _MM_SWIZ_REG_CDAB);
    // show_lh(pll_inter, phl_inter, "tb");
    // show_lh(plh_inter, phh_inter, "tb");

    // l = _mm512_min_epi32(l_inter, h_inter);
    // h = _mm512_max_epi32(l_inter, h_inter); // level 6 comparison
    m0 = _mm512_cmp_epi32_mask(l_inter , h_inter, _MM_CMPINT_GT);
    // key
    l  = _mm512_mask_swizzle_epi32(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_epi32(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    // index
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    __m512i t1;
    __m512i plt1;
    __m512i pht1;
    // std::cout << "baseline_before:" << std::endl;
    // for(int i=0; i< 16; i++)
        // std::cout <<((int*)&l)[i] << " ";
    // std::cout << std::endl;
    l = _mm512_permute4f128_epi32(l, _MM_PERM_BDAC);
    // std::cout << "baseline_after:" << std::endl;
    // for(int i=0; i< 16; i++)
        // std::cout <<((int*)&l)[i] << " ";
    // std::cout << std::endl;
    h = _mm512_permute4f128_epi32(h, _MM_PERM_BDAC);

    // std::cout << "before:" << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&pll)[i] << " ";
    // std::cout << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&phl)[i] << " ";
    // std::cout << std::endl;
    __m512i tpll = _mm512_mask_permute4f128_epi32(phl, 0xff00, pll, _MM_PERM_BADC);
    __m512i tphl = _mm512_mask_permute4f128_epi32(pll, 0x00ff, phl, _MM_PERM_BADC);
    pll = tpll;
    phl = tphl;
    __m512i tplh = _mm512_mask_permute4f128_epi32(phh, 0xff00, plh, _MM_PERM_BADC);
    __m512i tphh = _mm512_mask_permute4f128_epi32(plh, 0x00ff, phh, _MM_PERM_BADC);
    plh = tplh;
    phh = tphh;

    // std::cout << "after:" << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&tpll)[i] << " ";
    // std::cout << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&tphl)[i] << " ";
    // std::cout << std::endl;

    t0  = _mm512_mask_swizzle_epi32(h, 0xcccc, l, _MM_SWIZ_REG_BADC); 
    t1  = _mm512_mask_swizzle_epi32(l, 0x3333, h, _MM_SWIZ_REG_BADC); 
    plt0  = _mm512_mask_swizzle_epi64(plh, 0xcc, pll, _MM_SWIZ_REG_BADC); 
    pht0  = _mm512_mask_swizzle_epi64(phh, 0xcc, phl, _MM_SWIZ_REG_BADC); 
    plt1  = _mm512_mask_swizzle_epi64(pll, 0x33, plh, _MM_SWIZ_REG_BADC); 
    pht1  = _mm512_mask_swizzle_epi64(phl, 0x33, phh, _MM_SWIZ_REG_BADC); 
    l = _mm512_mask_permute4f128_epi32(t1 , 0x0f0f, t0 , _MM_PERM_CDAB); 
    h = _mm512_mask_permute4f128_epi32(t0 , 0xf0f0, t1 , _MM_PERM_CDAB); 
    pll = _mm512_mask_permute4f128_epi32(plt1 , 0x00ff, plt0 , _MM_PERM_BADC); 
    phl = _mm512_mask_permute4f128_epi32(pht1 , 0x00ff, pht0 , _MM_PERM_BADC); 
    plh = _mm512_mask_permute4f128_epi32(plt0 , 0xff00, plt1 , _MM_PERM_BADC); 
    phh = _mm512_mask_permute4f128_epi32(pht0 , 0xff00, pht1 , _MM_PERM_BADC); 
    // std::cout << "baseline_before:" << std::endl;
    // for(int i=0; i< 16; i++)
        // std::cout <<((int*)&l)[i] << " ";
    // std::cout << std::endl;
    r1 = _mm512_shuffle_epi32(l, _MM_PERM_BDAC);
    // std::cout << "after_before:" << std::endl;
    // for(int i=0; i< 16; i++)
        // std::cout <<((int*)&r1)[i] << " ";
    // std::cout << std::endl;
    r2 = _mm512_shuffle_epi32(h, _MM_PERM_BDAC);

    // std::cout << "before:" << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&pll)[i] << " ";
    // std::cout << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&phl)[i] << " ";
    // std::cout << std::endl;
    __m512i xpr = _mm512_shuffle_epi32(pll, _MM_PERM_BADC);
    __m512i ypr = _mm512_permute4f128_epi32(pll, _MM_PERM_CDAB);
    plr1 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 
    xpr = _mm512_shuffle_epi32(phl, _MM_PERM_BADC);
    ypr = _mm512_permute4f128_epi32(phl, _MM_PERM_CDAB);
    phr1 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 

    xpr = _mm512_shuffle_epi32(plh, _MM_PERM_BADC);
    ypr = _mm512_permute4f128_epi32(plh, _MM_PERM_CDAB);
    plr2 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 
    xpr = _mm512_shuffle_epi32(phh, _MM_PERM_BADC);
    ypr = _mm512_permute4f128_epi32(phh, _MM_PERM_CDAB);
    phr2 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 

    // std::cout << "after:" << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&plr1)[i] << " ";
    // std::cout << std::endl;
    // for(int i=0; i< 8; i++)
        // std::cout <<((long*)&phr1)[i] << " ";
    // std::cout << std::endl;


    v0 = r1;
    v1 = r2;
    pl0 = plr1;
    ph0 = phr1;
    pl1 = plr2;
    ph1 = phr2;
}

template <typename T> // float key int index
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_merge_key(T& v0, T& v1, __m512i &p0, __m512i &p1)
{
    __m512 t0, l, h, l_inter, h_inter, l_shuffle, h_shuffle, r1, r2;
    __m512i pt0, pl, ph, pl_inter, ph_inter, pl_shuffle, ph_shuffle, pr1, pr2;
    __m512 reverse;
    __m512i preverse;
    __mmask16 m0 ;
    t0 = _mm512_permute4f128_ps(v1, _MM_PERM_ABCD);
    reverse = (__m512)_mm512_shuffle_epi32((__m512i)t0, _MM_PERM_ABCD);
    pt0 = _mm512_permute4f128_epi32(p1, _MM_PERM_ABCD);
    preverse = _mm512_shuffle_epi32(pt0, _MM_PERM_ABCD);

    // l = _mm512_min_ps(v0, reverse);
    // h = _mm512_max_ps(v0, reverse); // level 1 comparison
    m0 = _mm512_cmp_ps_mask(v0 , reverse, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(v0 , m0, reverse, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(reverse, m0, v0, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(p0 , m0, preverse, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(preverse, m0, p0, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_permute4f128_ps(l, 0xff00, h, _MM_PERM_BADC);
    h_inter = _mm512_mask_permute4f128_ps(h, 0x00ff, l, _MM_PERM_BADC);
    pl_inter = _mm512_mask_permute4f128_epi32(pl, 0xff00, ph, _MM_PERM_BADC);
    ph_inter = _mm512_mask_permute4f128_epi32(ph, 0x00ff, pl, _MM_PERM_BADC);

    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 2 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_permute4f128_ps(l, 0xf0f0, h, _MM_PERM_CDAB);
    h_inter = _mm512_mask_permute4f128_ps(h, 0x0f0f, l, _MM_PERM_CDAB);
    pl_inter = _mm512_mask_permute4f128_epi32(pl, 0xf0f0, ph, _MM_PERM_CDAB);
    ph_inter = _mm512_mask_permute4f128_epi32(ph, 0x0f0f, pl, _MM_PERM_CDAB);
    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 4 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_swizzle_ps(l, 0xcccc, h, _MM_SWIZ_REG_BADC);
    h_inter = _mm512_mask_swizzle_ps(h, 0x3333, l, _MM_SWIZ_REG_BADC);
    pl_inter = _mm512_mask_swizzle_epi32(pl, 0xcccc, ph, _MM_SWIZ_REG_BADC);
    ph_inter = _mm512_mask_swizzle_epi32(ph, 0x3333, pl, _MM_SWIZ_REG_BADC);
    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 5 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_swizzle_ps(l, 0xaaaa, h, _MM_SWIZ_REG_CDAB);
    h_inter = _mm512_mask_swizzle_ps(h, 0x5555, l, _MM_SWIZ_REG_CDAB);
    pl_inter = _mm512_mask_swizzle_epi32(pl, 0xaaaa, ph, _MM_SWIZ_REG_CDAB);
    ph_inter = _mm512_mask_swizzle_epi32(ph, 0x5555, pl, _MM_SWIZ_REG_CDAB);
    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 6 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pl  = _mm512_mask_swizzle_epi32(pl_inter, m0, ph_inter, _MM_SWIZ_REG_NONE);
    ph  = _mm512_mask_swizzle_epi32(ph_inter, m0, pl_inter, _MM_SWIZ_REG_NONE);

    __m512 t1;
    __m512i pt1;
    l = _mm512_permute4f128_ps(l, _MM_PERM_BDAC);
    h = _mm512_permute4f128_ps(h, _MM_PERM_BDAC);
    pl = _mm512_permute4f128_epi32(pl, _MM_PERM_BDAC);
    ph = _mm512_permute4f128_epi32(ph, _MM_PERM_BDAC);
    t0  = _mm512_mask_swizzle_ps(h, 0xcccc, l, _MM_SWIZ_REG_BADC); 
    t1  = _mm512_mask_swizzle_ps(l, 0x3333, h, _MM_SWIZ_REG_BADC); 
    pt0  = _mm512_mask_swizzle_epi32(ph, 0xcccc, pl, _MM_SWIZ_REG_BADC); 
    pt1  = _mm512_mask_swizzle_epi32(pl, 0x3333, ph, _MM_SWIZ_REG_BADC); 
    l = _mm512_mask_permute4f128_ps(t1 , 0x0f0f, t0 , _MM_PERM_CDAB); 
    h = _mm512_mask_permute4f128_ps(t0 , 0xf0f0, t1 , _MM_PERM_CDAB); 
    pl = _mm512_mask_permute4f128_epi32(pt1 , 0x0f0f, pt0 , _MM_PERM_CDAB); 
    ph = _mm512_mask_permute4f128_epi32(pt0 , 0xf0f0, pt1 , _MM_PERM_CDAB); 
    r1 = (__m512)_mm512_shuffle_epi32((__m512i)l, _MM_PERM_BDAC);
    r2 = (__m512)_mm512_shuffle_epi32((__m512i)h, _MM_PERM_BDAC);
    pr1 = _mm512_shuffle_epi32(pl, _MM_PERM_BDAC);
    pr2 = _mm512_shuffle_epi32(ph, _MM_PERM_BDAC);

    v0 = r1;
    v1 = r2;
    p0 = pr1;
    p1 = pr2;
}

template <typename T> // float key long index
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_merge_key(T& v0, T& v1, __m512i &pl0, __m512i &pl1, __m512i &ph0, __m512i &ph1)
{
    __m512 t0, l, h, l_inter, h_inter, l_shuffle, h_shuffle, r1, r2;
    __m512i plt0, pll, plh, pll_inter, plh_inter, pll_shuffle, plh_shuffle, plr1, plr2;
    __m512i pht0, phl, phh, phl_inter, phh_inter, phl_shuffle, phh_shuffle, phr1, phr2;
    __m512 reverse;
    __m512i plreverse;
    __m512i phreverse;
    __mmask16 m0 ;
    // key
    t0 = _mm512_permute4f128_ps(v1, _MM_PERM_ABCD);
    reverse = (__m512)_mm512_shuffle_epi32((__m512i)t0, _MM_PERM_ABCD);

    // index
    pht0 = _mm512_permute4f128_epi32(ph1, _MM_PERM_ABCD);
    plreverse = _mm512_shuffle_epi32(pht0, _MM_PERM_BADC);
    plt0 = _mm512_permute4f128_epi32(pl1, _MM_PERM_ABCD);
    phreverse = _mm512_shuffle_epi32(plt0, _MM_PERM_BADC);

    // l = _mm512_min_ps(v0, reverse);
    // h = _mm512_max_ps(v0, reverse); // level 1 comparison
    m0 = _mm512_cmp_ps_mask(v0 , reverse, _MM_CMPINT_GT);
    // key
    l  = _mm512_mask_swizzle_ps(v0 , m0, reverse, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(reverse, m0, v0, _MM_SWIZ_REG_NONE);
    // index
    pll  = _mm512_mask_swizzle_epi64(pl0 , m0   , plreverse, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(ph0 , m0>>8, phreverse, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plreverse, m0   , pl0, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phreverse, m0>>8, ph0, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_permute4f128_ps(l, 0xff00, h, _MM_PERM_BADC);
    h_inter = _mm512_mask_permute4f128_ps(h, 0x00ff, l, _MM_PERM_BADC);
    pll_inter = pll;
    phl_inter = plh;
    plh_inter = phl;
    phh_inter = phh;

    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 2 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_permute4f128_ps(l, 0xf0f0, h, _MM_PERM_CDAB);
    h_inter = _mm512_mask_permute4f128_ps(h, 0x0f0f, l, _MM_PERM_CDAB);
    pll_inter = _mm512_mask_permute4f128_epi32(pll, 0xff00, plh, _MM_PERM_BADC);
    phl_inter = _mm512_mask_permute4f128_epi32(phl, 0xff00, phh, _MM_PERM_BADC);
    plh_inter = _mm512_mask_permute4f128_epi32(plh, 0x00ff, pll, _MM_PERM_BADC);
    phh_inter = _mm512_mask_permute4f128_epi32(phh, 0x00ff, phl, _MM_PERM_BADC);
    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 4 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_swizzle_ps(l, 0xcccc, h, _MM_SWIZ_REG_BADC);
    h_inter = _mm512_mask_swizzle_ps(h, 0x3333, l, _MM_SWIZ_REG_BADC);
    pll_inter = _mm512_mask_swizzle_epi64(pll, 0xcc, plh, _MM_SWIZ_REG_BADC);
    phl_inter = _mm512_mask_swizzle_epi64(phl, 0xcc, phh, _MM_SWIZ_REG_BADC);
    plh_inter = _mm512_mask_swizzle_epi64(plh, 0x33, pll, _MM_SWIZ_REG_BADC);
    phh_inter = _mm512_mask_swizzle_epi64(phh, 0x33, phl, _MM_SWIZ_REG_BADC);
    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 5 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    l_inter = _mm512_mask_swizzle_ps(l, 0xaaaa, h, _MM_SWIZ_REG_CDAB);
    h_inter = _mm512_mask_swizzle_ps(h, 0x5555, l, _MM_SWIZ_REG_CDAB);
    pll_inter = _mm512_mask_swizzle_epi64(pll, 0xaa, plh, _MM_SWIZ_REG_CDAB);
    phl_inter = _mm512_mask_swizzle_epi64(phl, 0xaa, phh, _MM_SWIZ_REG_CDAB);
    plh_inter = _mm512_mask_swizzle_epi64(plh, 0x55, pll, _MM_SWIZ_REG_CDAB);
    phh_inter = _mm512_mask_swizzle_epi64(phh, 0x55, phl, _MM_SWIZ_REG_CDAB);
    // l = _mm512_min_ps(l_inter, h_inter);
    // h = _mm512_max_ps(l_inter, h_inter); // level 6 comparison
    m0 = _mm512_cmp_ps_mask(l_inter , h_inter, _MM_CMPINT_GT);
    l  = _mm512_mask_swizzle_ps(l_inter, m0, h_inter, _MM_SWIZ_REG_NONE);
    h  = _mm512_mask_swizzle_ps(h_inter, m0, l_inter, _MM_SWIZ_REG_NONE);
    pll  = _mm512_mask_swizzle_epi64(pll_inter, m0   , plh_inter, _MM_SWIZ_REG_NONE);
    phl  = _mm512_mask_swizzle_epi64(phl_inter, m0>>8, phh_inter, _MM_SWIZ_REG_NONE);
    plh  = _mm512_mask_swizzle_epi64(plh_inter, m0   , pll_inter, _MM_SWIZ_REG_NONE);
    phh  = _mm512_mask_swizzle_epi64(phh_inter, m0>>8, phl_inter, _MM_SWIZ_REG_NONE);

    __m512 t1;
    __m512i plt1;
    __m512i pht1;
    l = _mm512_permute4f128_ps(l, _MM_PERM_BDAC);
    h = _mm512_permute4f128_ps(h, _MM_PERM_BDAC);

    __m512i tpll = _mm512_mask_permute4f128_epi32(phl, 0xff00, pll, _MM_PERM_BADC);
    __m512i tphl = _mm512_mask_permute4f128_epi32(pll, 0x00ff, phl, _MM_PERM_BADC);
    pll = tpll;
    phl = tphl;
    __m512i tplh = _mm512_mask_permute4f128_epi32(phh, 0xff00, plh, _MM_PERM_BADC);
    __m512i tphh = _mm512_mask_permute4f128_epi32(plh, 0x00ff, phh, _MM_PERM_BADC);
    plh = tplh;
    phh = tphh;

    t0  = _mm512_mask_swizzle_ps(h, 0xcccc, l, _MM_SWIZ_REG_BADC); 
    t1  = _mm512_mask_swizzle_ps(l, 0x3333, h, _MM_SWIZ_REG_BADC); 
    plt0  = _mm512_mask_swizzle_epi64(plh, 0xcc, pll, _MM_SWIZ_REG_BADC); 
    pht0  = _mm512_mask_swizzle_epi64(phh, 0xcc, phl, _MM_SWIZ_REG_BADC); 
    plt1  = _mm512_mask_swizzle_epi64(pll, 0x33, plh, _MM_SWIZ_REG_BADC); 
    pht1  = _mm512_mask_swizzle_epi64(phl, 0x33, phh, _MM_SWIZ_REG_BADC); 
    l = _mm512_mask_permute4f128_ps(t1 , 0x0f0f, t0 , _MM_PERM_CDAB); 
    h = _mm512_mask_permute4f128_ps(t0 , 0xf0f0, t1 , _MM_PERM_CDAB); 
    pll = _mm512_mask_permute4f128_epi32(plt1 , 0x00ff, plt0 , _MM_PERM_BADC); 
    phl = _mm512_mask_permute4f128_epi32(pht1 , 0x00ff, pht0 , _MM_PERM_BADC); 
    plh = _mm512_mask_permute4f128_epi32(plt0 , 0xff00, plt1 , _MM_PERM_BADC); 
    phh = _mm512_mask_permute4f128_epi32(pht0 , 0xff00, pht1 , _MM_PERM_BADC); 
    r1 = (__m512)_mm512_shuffle_epi32((__m512i)l, _MM_PERM_BDAC);
    r2 = (__m512)_mm512_shuffle_epi32((__m512i)h, _MM_PERM_BDAC);

    __m512i xpr = _mm512_shuffle_epi32(pll, _MM_PERM_BADC);
    __m512i ypr = _mm512_permute4f128_epi32(pll, _MM_PERM_CDAB);
    plr1 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 
    xpr = _mm512_shuffle_epi32(phl, _MM_PERM_BADC);
    ypr = _mm512_permute4f128_epi32(phl, _MM_PERM_CDAB);
    phr1 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 

    xpr = _mm512_shuffle_epi32(plh, _MM_PERM_BADC);
    ypr = _mm512_permute4f128_epi32(plh, _MM_PERM_CDAB);
    plr2 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 
    xpr = _mm512_shuffle_epi32(phh, _MM_PERM_BADC);
    ypr = _mm512_permute4f128_epi32(phh, _MM_PERM_CDAB);
    phr2 = _mm512_mask_swizzle_epi64(xpr, 0x99, ypr, _MM_SWIZ_REG_NONE); 

    v0 = r1;
    v1 = r2;
    pl0 = plr1;
    ph0 = phr1;
    pl1 = plr2;
    ph1 = phr2;
}

template <typename T> // double key int index
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_merge_key(T& v0, T& v1, __m512i &pv0, __m512i &pv1)
{
    __m512d tmp;
    __m512d t0;
    __m512d t1;

    __m512i ptmp;
    __m512i pt0;
    __m512i pt1;
    __m512i xv0;
    __m512i xv1;
    __m512i yv0;
    __m512i yv1;
    __mmask16 mask; 
    __mmask8 m0; 

    tmp   = (__m512d)_mm512_shuffle_epi32((__m512i)v1, _MM_PERM_BADC);
    v1 = (__m512d)_mm512_permute4f128_epi32((__m512i)tmp, _MM_PERM_ABCD);
    ptmp   = _mm512_shuffle_epi32(pv1, _MM_PERM_ABCD);
    pv1 = _mm512_permute4f128_epi32(ptmp, _MM_PERM_CDAB);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 1 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi32(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi32(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);


    mask = 0xff00;
    v0 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t0, mask, (__m512i)t1, _MM_PERM_BADC);
    pv0 = _mm512_mask_permute4f128_epi32(pt0, 0xf0, pt1, _MM_PERM_CDAB);
    mask = 0x00ff;
    v1 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t1, mask, (__m512i)t0, _MM_PERM_BADC);
    pv1 = _mm512_mask_permute4f128_epi32(pt1, 0x0f, pt0, _MM_PERM_CDAB);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 2 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi32(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi32(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);

    mask = 0xcc;
    v0 = _mm512_mask_swizzle_pd(t0, mask, t1, _MM_SWIZ_REG_BADC);
    pv0 = _mm512_mask_swizzle_epi32(pt0, mask, pt1, _MM_SWIZ_REG_BADC);
    mask = 0x33;
    v1 = _mm512_mask_swizzle_pd(t1, mask, t0, _MM_SWIZ_REG_BADC);
    pv1 = _mm512_mask_swizzle_epi32(pt1, mask, pt0, _MM_SWIZ_REG_BADC);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 3 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi32(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi32(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);

    mask = 0xaa;
    v0 = _mm512_mask_swizzle_pd(t0, mask, t1, _MM_SWIZ_REG_CDAB);
    pv0 = _mm512_mask_swizzle_epi32(pt0, mask, pt1, _MM_SWIZ_REG_CDAB);
    mask = 0x55;
    v1 = _mm512_mask_swizzle_pd(t1, mask, t0, _MM_SWIZ_REG_CDAB);
    pv1 = _mm512_mask_swizzle_epi32(pt1, mask, pt0, _MM_SWIZ_REG_CDAB);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 4 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi32(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi32(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);

    v0 = (__m512d)_mm512_permute4f128_epi32((__m512i)t0, _MM_PERM_BDAC);
    v1 = (__m512d)_mm512_permute4f128_epi32((__m512i)t1, _MM_PERM_BDAC);
    // pv0 = pt0;
    // pv1 = pt1;

    xv0 = _mm512_permute4f128_epi32(pt0, _MM_PERM_CDAB);
    // std::cout << "xv0" << std::endl;
    // for(int i=0;i<16; i++)
        // std::cout << ((int*)&xv0)[i] << " ";
    // std::cout << std::endl;
    xv1 = _mm512_permute4f128_epi32(pt1, _MM_PERM_CDAB);
    yv0 = _mm512_shuffle_epi32(pt0, _MM_PERM_BADC);
    // std::cout << "yv0" << std::endl;
    // for(int i=0;i<16; i++)
        // std::cout << ((int*)&yv0)[i] << " ";
    // std::cout << std::endl;
    yv1 = _mm512_shuffle_epi32(pt1, _MM_PERM_BADC);
    pv0 = _mm512_mask_swizzle_epi32(xv0, 0x3c, yv0, _MM_SWIZ_REG_NONE); // might have problem of mask
    pv1 = _mm512_mask_swizzle_epi32(xv1, 0x3c, yv1, _MM_SWIZ_REG_NONE);

    mask = 0x55;
    t0  = _mm512_mask_swizzle_pd(v0, mask, v1, _MM_SWIZ_REG_CDAB); 
    pt0  = _mm512_mask_swizzle_epi32(pv0, mask, pv1, _MM_SWIZ_REG_CDAB); 
    mask = 0xaa;
    t1  = _mm512_mask_swizzle_pd(v1, mask, v0, _MM_SWIZ_REG_CDAB); 
    pt1  = _mm512_mask_swizzle_epi32(pv1, mask, pv0, _MM_SWIZ_REG_CDAB); 

    mask = 0x0f0f;
    v0 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t0, mask, (__m512i)t1, _MM_PERM_CDAB); 
    pv0 = _mm512_mask_shuffle_epi32(pt0, 0x33, pt1, _MM_PERM_BADC); 
    mask = 0xf0f0;
    v1 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t1, mask, (__m512i)t0, _MM_PERM_CDAB); 
    pv1 = _mm512_mask_shuffle_epi32(pt1, 0xcc, pt0, _MM_PERM_BADC); 

    v0 = _mm512_swizzle_pd(v0, _MM_SWIZ_REG_CDAB); 
    v1 = _mm512_swizzle_pd(v1, _MM_SWIZ_REG_CDAB); 
    pv0 = _mm512_swizzle_epi32(pv0, _MM_SWIZ_REG_CDAB); 
    pv1 = _mm512_swizzle_epi32(pv1, _MM_SWIZ_REG_CDAB); 

}

template <typename T> // double key long index
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_merge_key_l(T& v0, T& v1, __m512i &pv0, __m512i &pv1)
{
    __m512d tmp;
    __m512d t0;
    __m512d t1;

    __m512i ptmp;
    __m512i pt0;
    __m512i pt1;

    __mmask16 mask; 
    __mmask8 m0; 

    tmp   = (__m512d)_mm512_shuffle_epi32((__m512i)v1, _MM_PERM_BADC);
    v1 = (__m512d)_mm512_permute4f128_epi32((__m512i)tmp, _MM_PERM_ABCD);
    ptmp   = _mm512_shuffle_epi32(pv1, _MM_PERM_BADC);
    pv1 = _mm512_permute4f128_epi32(ptmp, _MM_PERM_ABCD);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 1 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi64(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi64(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);


    mask = 0xff00;
    v0 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t0, mask, (__m512i)t1, _MM_PERM_BADC);
    pv0 = _mm512_mask_permute4f128_epi32(pt0, mask, pt1, _MM_PERM_BADC);
    mask = 0x00ff;
    v1 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t1, mask, (__m512i)t0, _MM_PERM_BADC);
    pv1 = _mm512_mask_permute4f128_epi32(pt1, mask, pt0, _MM_PERM_BADC);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 2 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi64(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi64(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);

    mask = 0xcc;
    v0 = _mm512_mask_swizzle_pd(t0, mask, t1, _MM_SWIZ_REG_BADC);
    pv0 = _mm512_mask_swizzle_epi64(pt0, mask, pt1, _MM_SWIZ_REG_BADC);
    mask = 0x33;
    v1 = _mm512_mask_swizzle_pd(t1, mask, t0, _MM_SWIZ_REG_BADC);
    pv1 = _mm512_mask_swizzle_epi64(pt1, mask, pt0, _MM_SWIZ_REG_BADC);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 3 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi64(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi64(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);

    mask = 0xaa;
    v0 = _mm512_mask_swizzle_pd(t0, mask, t1, _MM_SWIZ_REG_CDAB);
    pv0 = _mm512_mask_swizzle_epi64(pt0, mask, pt1, _MM_SWIZ_REG_CDAB);
    mask = 0x55;
    v1 = _mm512_mask_swizzle_pd(t1, mask, t0, _MM_SWIZ_REG_CDAB);
    pv1 = _mm512_mask_swizzle_epi64(pt1, mask, pt0, _MM_SWIZ_REG_CDAB);

    // t0 = _mm512_gmin_pd(v0, v1);
    // t1 = _mm512_gmax_pd(v0, v1); // level 4 comparison
    m0 = _mm512_cmp_pd_mask(v0, v1, _MM_CMPINT_GT);
    t0  = _mm512_mask_swizzle_pd(v0 , m0, v1 , _MM_SWIZ_REG_NONE);
    t1  = _mm512_mask_swizzle_pd(v1 , m0, v0 , _MM_SWIZ_REG_NONE);
    pt0  = _mm512_mask_swizzle_epi64(pv0 , m0, pv1 , _MM_SWIZ_REG_NONE);
    pt1  = _mm512_mask_swizzle_epi64(pv1 , m0, pv0 , _MM_SWIZ_REG_NONE);

    v0 = (__m512d)_mm512_permute4f128_epi32((__m512i)t0, _MM_PERM_BDAC);
    v1 = (__m512d)_mm512_permute4f128_epi32((__m512i)t1, _MM_PERM_BDAC);
    pv0 = _mm512_permute4f128_epi32(pt0, _MM_PERM_BDAC);
    pv1 = _mm512_permute4f128_epi32(pt1, _MM_PERM_BDAC);

    mask = 0x55;
    t0  = _mm512_mask_swizzle_pd(v0, mask, v1, _MM_SWIZ_REG_CDAB); 
    pt0  = _mm512_mask_swizzle_epi64(pv0, mask, pv1, _MM_SWIZ_REG_CDAB); 
    mask = 0xaa;
    t1  = _mm512_mask_swizzle_pd(v1, mask, v0, _MM_SWIZ_REG_CDAB); 
    pt1  = _mm512_mask_swizzle_epi64(pv1, mask, pv0, _MM_SWIZ_REG_CDAB); 

    mask = 0x0f0f;
    v0 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t0, mask, (__m512i)t1, _MM_PERM_CDAB); 
    pv0 = _mm512_mask_permute4f128_epi32(pt0, mask, pt1, _MM_PERM_CDAB); 
    mask = 0xf0f0;
    v1 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t1, mask, (__m512i)t0, _MM_PERM_CDAB); 
    pv1 = _mm512_mask_permute4f128_epi32(pt1, mask, pt0, _MM_PERM_CDAB); 

    v0 = _mm512_swizzle_pd(v0, _MM_SWIZ_REG_CDAB); 
    v1 = _mm512_swizzle_pd(v1, _MM_SWIZ_REG_CDAB); 
    pv0 = _mm512_swizzle_epi64(pv0, _MM_SWIZ_REG_CDAB); 
    pv1 = _mm512_swizzle_epi64(pv1, _MM_SWIZ_REG_CDAB); 

}
/**
 * Float vector (__m512) version:
 * This method performs the in-register merge of two sorted vectors.
 *
 * @param v0 v1 sorted vector registers
 * @return sorted data stored horizontally in the two registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512>::value>::type
in_register_merge(T& v0, T& v1)
{
    __m512i t0, t1, r1, r2;
    __m512 reverse, l, h, l_inter, h_inter;
    t0 = _mm512_permute4f128_epi32((__m512i)v1, _MM_PERM_ABCD);
    reverse = (__m512)_mm512_shuffle_epi32(t0, _MM_PERM_ABCD);
    l = _mm512_min_ps(v0, reverse);
    h = _mm512_max_ps(v0, reverse); // level 1 comparison

    l_inter = (__m512)_mm512_mask_permute4f128_epi32((__m512i)l, 0xff00, (__m512i)h, _MM_PERM_BADC);
    h_inter = (__m512)_mm512_mask_permute4f128_epi32((__m512i)h, 0x00ff, (__m512i)l, _MM_PERM_BADC);
    l = _mm512_min_ps(l_inter, h_inter);
    h = _mm512_max_ps(l_inter, h_inter); // level 2 comparison

    l_inter = (__m512)_mm512_mask_permute4f128_epi32((__m512i)l, 0xf0f0, (__m512i)h, _MM_PERM_CDAB);
    h_inter = (__m512)_mm512_mask_permute4f128_epi32((__m512i)h, 0x0f0f, (__m512i)l, _MM_PERM_CDAB);
    l = _mm512_min_ps(l_inter, h_inter);
    h = _mm512_max_ps(l_inter, h_inter); // level 4 comparison

    l_inter = (__m512)_mm512_mask_swizzle_epi32((__m512i)l, 0xcccc, (__m512i)h, _MM_SWIZ_REG_BADC);
    h_inter = (__m512)_mm512_mask_swizzle_epi32((__m512i)h, 0x3333, (__m512i)l, _MM_SWIZ_REG_BADC);
    l = _mm512_min_ps(l_inter, h_inter);
    h = _mm512_max_ps(l_inter, h_inter); // level 5 comparison

    l_inter = (__m512)_mm512_mask_swizzle_epi32((__m512i)l, 0xaaaa, (__m512i)h, _MM_SWIZ_REG_CDAB);
    h_inter = (__m512)_mm512_mask_swizzle_epi32((__m512i)h, 0x5555, (__m512i)l, _MM_SWIZ_REG_CDAB);
    l = _mm512_min_ps(l_inter, h_inter);
    h = _mm512_max_ps(l_inter, h_inter); // level 6 comparison

    l = (__m512)_mm512_permute4f128_epi32((__m512i)l, _MM_PERM_BDAC);
    h = (__m512)_mm512_permute4f128_epi32((__m512i)h, _MM_PERM_BDAC);
    t0  = _mm512_mask_swizzle_epi32((__m512i)h, 0xcccc, (__m512i)l, _MM_SWIZ_REG_BADC); 
    t1  = _mm512_mask_swizzle_epi32((__m512i)l, 0x3333, (__m512i)h, _MM_SWIZ_REG_BADC); 
    l = (__m512)_mm512_mask_permute4f128_epi32(t1 , 0x0f0f, t0 , _MM_PERM_CDAB); 
    h = (__m512)_mm512_mask_permute4f128_epi32(t0 , 0xf0f0, t1 , _MM_PERM_CDAB); 
    r1 = _mm512_shuffle_epi32((__m512i)l, _MM_PERM_BDAC);
    r2 = _mm512_shuffle_epi32((__m512i)h, _MM_PERM_BDAC);

    v0 = (__m512)r1;
    v1 = (__m512)r2;
}
    

/**
 * Double vector (__m512d) version:
 * This method performs the in-register merge of two sorted vectors.
 *
 * @param v0 v1 sorted vector registers
 * @return sorted data stored horizontally in the two registers
 *
 */
template <typename T>
typename std::enable_if<std::is_same<T, __m512d>::value>::type
in_register_merge(T& v0, T& v1)
{
    __m512d tmp;
    __m512d t0;
    __m512d t1;
    __mmask16 mask; 

    tmp   = (__m512d)_mm512_shuffle_epi32((__m512i)v1, _MM_PERM_BADC);
    v1 = (__m512d)_mm512_permute4f128_epi32((__m512i)tmp, _MM_PERM_ABCD);
    t0 = _mm512_gmin_pd(v0, v1);
    t1 = _mm512_gmax_pd(v0, v1); // level 1 comparison

    mask = 0xff00;
    v0 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t0, mask, (__m512i)t1, _MM_PERM_BADC);
    mask = 0x00ff;
    v1 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t1, mask, (__m512i)t0, _MM_PERM_BADC);
    t0 = _mm512_gmin_pd(v0, v1);
    t1 = _mm512_gmax_pd(v0, v1); // level 2 comparison

    mask = 0xcc;
    v0 = _mm512_mask_swizzle_pd(t0, mask, t1, _MM_SWIZ_REG_BADC);
    mask = 0x33;
    v1 = _mm512_mask_swizzle_pd(t1, mask, t0, _MM_SWIZ_REG_BADC);
    t0 = _mm512_gmin_pd(v0, v1);
    t1 = _mm512_gmax_pd(v0, v1); // level 3 comparison

    mask = 0xaa;
    v0 = _mm512_mask_swizzle_pd(t0, mask, t1, _MM_SWIZ_REG_CDAB);
    mask = 0x55;
    v1 = _mm512_mask_swizzle_pd(t1, mask, t0, _MM_SWIZ_REG_CDAB);
    t0 = _mm512_gmin_pd(v0, v1);
    t1 = _mm512_gmax_pd(v0, v1); // level 4 comparison


    v0 = (__m512d)_mm512_permute4f128_epi32((__m512i)t0, _MM_PERM_BDAC);
    v1 = (__m512d)_mm512_permute4f128_epi32((__m512i)t1, _MM_PERM_BDAC);
    mask = 0x55;
    t0  = _mm512_mask_swizzle_pd(v0, mask, v1, _MM_SWIZ_REG_CDAB); 
    mask = 0xaa;
    t1  = _mm512_mask_swizzle_pd(v1, mask, v0, _MM_SWIZ_REG_CDAB); 

    mask = 0x0f0f;
    v0 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t0, mask, (__m512i)t1, _MM_PERM_CDAB); 
    mask = 0xf0f0;
    v1 = (__m512d)_mm512_mask_permute4f128_epi32((__m512i)t1, mask, (__m512i)t0, _MM_PERM_CDAB); 

    v0 = _mm512_swizzle_pd(v0, _MM_SWIZ_REG_CDAB); 
    v1 = _mm512_swizzle_pd(v1, _MM_SWIZ_REG_CDAB); 


}

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output)
{
    __m512i vec0 = _mm512_set1_epi32(1);
    __m512i vec1 = _mm512_set1_epi32(1);
    uint8_t stride = (uint8_t)simd_width::AVX512_INT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    int buffer[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_epi32(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_epi32(vec1 , inputB + stride);

        in_register_merge(vec0, vec1);

        _mm512_packstorelo_epi32(output + iout         , vec0 );
        _mm512_packstorehi_epi32(output + iout + stride, vec0 );
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputB + i1 + stride);
                i1 += stride;
            }
            in_register_merge(vec0, vec1);
            _mm512_packstorelo_epi32(output + iout         , vec0 );
            _mm512_packstorehi_epi32(output + iout + stride, vec0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + i0 + stride);
                i0 += stride;
                in_register_merge(vec0, vec1);
                _mm512_packstorelo_epi32(output + iout         , vec0 );
                _mm512_packstorehi_epi32(output + iout + stride, vec0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
            vec0  = _mm512_loadunpacklo_epi32(vec0 , inputB + i1         );
            vec0  = _mm512_loadunpackhi_epi32(vec0 , inputB + i1 + stride);
            i1 += stride;
            in_register_merge(vec0, vec1);
            _mm512_packstorelo_epi32(output + iout         , vec0 );
            _mm512_packstorehi_epi32(output + iout + stride, vec0 );
            iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_epi32(buffer         , vec1 );
        _mm512_packstorehi_epi32(buffer + stride, vec1 );

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

    }else
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

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr)
{
    __m512i vec0 = _mm512_set1_epi32(1);
    __m512i vec1 = _mm512_set1_epi32(1);

    __m512i p0 = _mm512_set1_epi32(1);
    __m512i p1 = _mm512_set1_epi32(1);
    uint8_t stride = (uint8_t)simd_width::AVX512_INT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    int buffer[stride];
    int buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_epi32(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_epi32(vec1 , inputB + stride);

        p0  = _mm512_loadunpacklo_epi32(p0 , ptrA         );
        p0  = _mm512_loadunpackhi_epi32(p0 , ptrA + stride);
        p1  = _mm512_loadunpacklo_epi32(p1 , ptrB         );
        p1  = _mm512_loadunpackhi_epi32(p1 , ptrB + stride);

        in_register_merge_key(vec0, vec1, p0, p1);

        _mm512_packstorelo_epi32(output + iout         , vec0 );
        _mm512_packstorehi_epi32(output + iout + stride, vec0 );
        _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
        _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + i0 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrA + i0         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputB + i1 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrB + i1         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrB + i1 + stride);
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, p0, p1);
            _mm512_packstorelo_epi32(output + iout         , vec0 );
            _mm512_packstorehi_epi32(output + iout + stride, vec0 );
            _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
            _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + i0 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrA + i0         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrA + i0 + stride);
                i0 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm512_packstorelo_epi32(output + iout         , vec0 );
                _mm512_packstorehi_epi32(output + iout + stride, vec0 );
                _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
                _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputB + i1 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrB + i1         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrB + i1 + stride);
                i1 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm512_packstorelo_epi32(output + iout         , vec0 );
                _mm512_packstorehi_epi32(output + iout + stride, vec0 );
                _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
                _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
                iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_epi32(buffer         , vec1 );
        _mm512_packstorehi_epi32(buffer + stride, vec1 );
        _mm512_packstorelo_epi32(buffer_ptr         , p1 );
        _mm512_packstorehi_epi32(buffer_ptr + stride, p1 );

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

    }else
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
typename std::enable_if<std::is_same<T, int>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr)
{
    __m512i vec0 = _mm512_set1_epi32(1);
    __m512i vec1 = _mm512_set1_epi32(1);

    __m512i pl0 = _mm512_set1_epi64(1);
    __m512i pl1 = _mm512_set1_epi64(1);
    __m512i ph0 = _mm512_set1_epi64(1);
    __m512i ph1 = _mm512_set1_epi64(1);
    uint8_t stride = (uint8_t)simd_width::AVX512_INT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    int buffer[stride];
    long buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_epi32(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_epi32(vec1 , inputB + stride);

        pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrA         );
        pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrA + 8     );
        ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrA + 8     );
        ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrA + stride);
        pl1  = _mm512_loadunpacklo_epi64(pl1 , ptrB         );
        pl1  = _mm512_loadunpackhi_epi64(pl1 , ptrB + 8     );
        ph1  = _mm512_loadunpacklo_epi64(ph1 , ptrB + 8     );
        ph1  = _mm512_loadunpackhi_epi64(ph1 , ptrB + stride);

        in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);

        _mm512_packstorelo_epi32(output + iout         , vec0 );
        _mm512_packstorehi_epi32(output + iout + stride, vec0 );
        _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
        _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
        _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
        _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + i0 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrA + i0         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputB + i1 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrB + i1         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrB + i1 + stride);
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);
            _mm512_packstorelo_epi32(output + iout         , vec0 );
            _mm512_packstorehi_epi32(output + iout + stride, vec0 );
            _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
            _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
            _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
            _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputA + i0 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrA + i0         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrA + i0 + stride);
                i0 += stride;
                in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);
                _mm512_packstorelo_epi32(output + iout         , vec0 );
                _mm512_packstorehi_epi32(output + iout + stride, vec0 );
                _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
                _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
                _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
                _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm512_loadunpacklo_epi32(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_epi32(vec0 , inputB + i1 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrB + i1         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrB + i1 + stride);
                i1 += stride;
                in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);
                _mm512_packstorelo_epi32(output + iout         , vec0 );
                _mm512_packstorehi_epi32(output + iout + stride, vec0 );
                _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
                _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
                _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
                _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
                iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_epi32(buffer         , vec1 );
        _mm512_packstorehi_epi32(buffer + stride, vec1 );
        _mm512_packstorelo_epi64(buffer_ptr         , pl1 );
        _mm512_packstorehi_epi64(buffer_ptr + 8     , pl1 );
        _mm512_packstorelo_epi64(buffer_ptr + 8     , ph1 );
        _mm512_packstorehi_epi64(buffer_ptr + stride, ph1 );

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

    }else
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
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr)
{
    __m512 vec0 = _mm512_set1_ps(1);
    __m512 vec1 = _mm512_set1_ps(1);

    __m512i p0 = _mm512_set1_epi32(1);
    __m512i p1 = _mm512_set1_epi32(1);
    uint8_t stride = (uint8_t)simd_width::AVX512_FLOAT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    float buffer[stride];
    int buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_ps(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_ps(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_ps(vec1 , inputB + stride);

        p0  = _mm512_loadunpacklo_epi32(p0 , ptrA         );
        p0  = _mm512_loadunpackhi_epi32(p0 , ptrA + stride);
        p1  = _mm512_loadunpacklo_epi32(p1 , ptrB         );
        p1  = _mm512_loadunpackhi_epi32(p1 , ptrB + stride);

        in_register_merge_key(vec0, vec1, p0, p1);

        _mm512_packstorelo_ps(output + iout         , vec0 );
        _mm512_packstorehi_ps(output + iout + stride, vec0 );
        _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
        _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + i0 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrA + i0         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputB + i1 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrB + i1         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrB + i1 + stride);
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, p0, p1);
            _mm512_packstorelo_ps(output + iout         , vec0 );
            _mm512_packstorehi_ps(output + iout + stride, vec0 );
            _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
            _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + i0 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrA + i0         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrA + i0 + stride);
                i0 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm512_packstorelo_ps(output + iout         , vec0 );
                _mm512_packstorehi_ps(output + iout + stride, vec0 );
                _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
                _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputB + i1 + stride);
                p0  = _mm512_loadunpacklo_epi32(p0 , ptrB + i1         );
                p0  = _mm512_loadunpackhi_epi32(p0 , ptrB + i1 + stride);
                i1 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm512_packstorelo_ps(output + iout         , vec0 );
                _mm512_packstorehi_ps(output + iout + stride, vec0 );
                _mm512_packstorelo_epi32(output_ptr + iout         , p0 );
                _mm512_packstorehi_epi32(output_ptr + iout + stride, p0 );
                iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_ps(buffer         , vec1 );
        _mm512_packstorehi_ps(buffer + stride, vec1 );
        _mm512_packstorelo_epi32(buffer_ptr         , p1 );
        _mm512_packstorehi_epi32(buffer_ptr + stride, p1 );

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

    }else
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
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr)
{
    __m512 vec0 = _mm512_set1_ps(1);
    __m512 vec1 = _mm512_set1_ps(1);

    __m512i pl0 = _mm512_set1_epi64(1);
    __m512i pl1 = _mm512_set1_epi64(1);
    __m512i ph0 = _mm512_set1_epi64(1);
    __m512i ph1 = _mm512_set1_epi64(1);
    uint8_t stride = (uint8_t)simd_width::AVX512_FLOAT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    float buffer[stride];
    long buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_ps(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_ps(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_ps(vec1 , inputB + stride);

        pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrA         );
        pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrA + 8     );
        ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrA + 8     );
        ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrA + stride);
        pl1  = _mm512_loadunpacklo_epi64(pl1 , ptrB         );
        pl1  = _mm512_loadunpackhi_epi64(pl1 , ptrB + 8     );
        ph1  = _mm512_loadunpacklo_epi64(ph1 , ptrB + 8     );
        ph1  = _mm512_loadunpackhi_epi64(ph1 , ptrB + stride);

        in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);

        _mm512_packstorelo_ps(output + iout         , vec0 );
        _mm512_packstorehi_ps(output + iout + stride, vec0 );
        _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
        _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
        _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
        _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + i0 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrA + i0         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputB + i1 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrB + i1         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrB + i1 + stride);
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);
            _mm512_packstorelo_ps(output + iout         , vec0 );
            _mm512_packstorehi_ps(output + iout + stride, vec0 );
            _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
            _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
            _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
            _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + i0 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrA + i0         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrA + i0 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrA + i0 + stride);
                i0 += stride;
                in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);
                _mm512_packstorelo_ps(output + iout         , vec0 );
                _mm512_packstorehi_ps(output + iout + stride, vec0 );
                _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
                _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
                _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
                _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputB + i1 + stride);
                pl0  = _mm512_loadunpacklo_epi64(pl0 , ptrB + i1         );
                pl0  = _mm512_loadunpackhi_epi64(pl0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpacklo_epi64(ph0 , ptrB + i1 + 8     );
                ph0  = _mm512_loadunpackhi_epi64(ph0 , ptrB + i1 + stride);
                i1 += stride;
                in_register_merge_key(vec0, vec1, pl0, pl1, ph0, ph1);
                _mm512_packstorelo_ps(output + iout         , vec0 );
                _mm512_packstorehi_ps(output + iout + stride, vec0 );
                _mm512_packstorelo_epi64(output_ptr + iout         , pl0 );
                _mm512_packstorehi_epi64(output_ptr + iout + 8     , pl0 );
                _mm512_packstorelo_epi64(output_ptr + iout + 8     , ph0 );
                _mm512_packstorehi_epi64(output_ptr + iout + stride, ph0 );
                iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_ps(buffer         , vec1 );
        _mm512_packstorehi_ps(buffer + stride, vec1 );
        _mm512_packstorelo_epi64(buffer_ptr         , pl1 );
        _mm512_packstorehi_epi64(buffer_ptr + 8     , pl1 );
        _mm512_packstorelo_epi64(buffer_ptr + 8     , ph1 );
        _mm512_packstorehi_epi64(buffer_ptr + stride, ph1 );

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

    }else
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
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr)
{
    __m512d vec0 = _mm512_set1_pd(1);
    __m512d vec1 = _mm512_set1_pd(1);

    __m512i p0 = _mm512_set1_epi32(1);
    __m512i p1 = _mm512_set1_epi32(1);
    __mmask16 mp = 0x00ff;
    uint8_t stride = (uint8_t)simd_width::AVX512_DOUBLE;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    double buffer[stride];
    int buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_pd(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_pd(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_pd(vec1 , inputB + stride);

        p0  = _mm512_mask_loadunpacklo_epi32(p0 , mp, ptrA           );
        p0  = _mm512_mask_loadunpackhi_epi32(p0 , mp, ptrA + stride*2);
        p1  = _mm512_mask_loadunpacklo_epi32(p1 , mp, ptrB           );
        p1  = _mm512_mask_loadunpackhi_epi32(p1 , mp, ptrB + stride*2);

        // std::cout << "previous:" << std::endl;
        // std::cout << "p0:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p0)[i] << " ";
        // std::cout << std::endl;
        // std::cout << "p1:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p1)[i] << " ";
        // std::cout << std::endl;

        in_register_merge_key(vec0, vec1, p0, p1);

        // std::cout << "after:" << std::endl;
        // std::cout << "p0:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p0)[i] << " ";
        // std::cout << std::endl;
        // std::cout << "p1:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p1)[i] << " ";
        // std::cout << std::endl;

        _mm512_packstorelo_pd(output + iout         , vec0 );
        _mm512_packstorehi_pd(output + iout + stride, vec0 );
        _mm512_mask_packstorelo_epi32(output_ptr + iout           , mp, p0);
        _mm512_mask_packstorehi_epi32(output_ptr + iout + stride*2, mp, p0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + i0 + stride);
                p0  = _mm512_mask_loadunpacklo_epi32(p0 , mp, ptrA + i0         );
                p0  = _mm512_mask_loadunpackhi_epi32(p0 , mp, ptrA + i0 + stride*2);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputB + i1 + stride);
                p0  = _mm512_mask_loadunpacklo_epi32(p0 , mp, ptrB + i1         );
                p0  = _mm512_mask_loadunpackhi_epi32(p0 , mp, ptrB + i1 + stride*2);
                i1 += stride;
            }
            in_register_merge_key(vec0, vec1, p0, p1);
            _mm512_packstorelo_pd(output + iout         , vec0 );
            _mm512_packstorehi_pd(output + iout + stride, vec0 );
            _mm512_mask_packstorelo_epi32(output_ptr + iout           , mp, p0 );
            _mm512_mask_packstorehi_epi32(output_ptr + iout + stride*2, mp, p0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + i0 + stride);
                p0  = _mm512_mask_loadunpacklo_epi32(p0 , mp, ptrA + i0           );
                p0  = _mm512_mask_loadunpackhi_epi32(p0 , mp, ptrA + i0 + stride*2);
                i0 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm512_packstorelo_pd(output + iout         , vec0 );
                _mm512_packstorehi_pd(output + iout + stride, vec0 );
                _mm512_mask_packstorelo_epi32(output_ptr + iout           , mp, p0 );
                _mm512_mask_packstorehi_epi32(output_ptr + iout + stride*2, mp, p0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputB + i1 + stride);
                p0  = _mm512_mask_loadunpacklo_epi32(p0 , mp, ptrB + i1           );
                p0  = _mm512_mask_loadunpackhi_epi32(p0 , mp, ptrB + i1 + stride*2);
                i1 += stride;
                in_register_merge_key(vec0, vec1, p0, p1);
                _mm512_packstorelo_pd(output + iout         , vec0 );
                _mm512_packstorehi_pd(output + iout + stride, vec0 );
                _mm512_mask_packstorelo_epi32(output_ptr + iout           , mp, p0 );
                _mm512_mask_packstorehi_epi32(output_ptr + iout + stride*2, mp, p0 );
                iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_pd(buffer         , vec1 );
        _mm512_packstorehi_pd(buffer + stride, vec1 );
        _mm512_mask_packstorelo_epi32(buffer_ptr           , mp, p1 );
        _mm512_mask_packstorehi_epi32(buffer_ptr + stride*2, mp, p1 );

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

    }else
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
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr)
{
    __m512d vec0 = _mm512_set1_pd(1);
    __m512d vec1 = _mm512_set1_pd(1);

    __m512i p0 = _mm512_set1_epi64(1);
    __m512i p1 = _mm512_set1_epi64(1);
    uint8_t stride = (uint8_t)simd_width::AVX512_DOUBLE;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    double buffer[stride];
    long buffer_ptr[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_pd(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_pd(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_pd(vec1 , inputB + stride);

        p0  = _mm512_loadunpacklo_epi64(p0 , ptrA           );
        p0  = _mm512_loadunpackhi_epi64(p0 , ptrA + stride);
        p1  = _mm512_loadunpacklo_epi64(p1 , ptrB           );
        p1  = _mm512_loadunpackhi_epi64(p1 , ptrB + stride);

        // std::cout << "previous:" << std::endl;
        // std::cout << "p0:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p0)[i] << " ";
        // std::cout << std::endl;
        // std::cout << "p1:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p1)[i] << " ";
        // std::cout << std::endl;

        in_register_merge_key_l(vec0, vec1, p0, p1);

        // std::cout << "after:" << std::endl;
        // std::cout << "p0:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p0)[i] << " ";
        // std::cout << std::endl;
        // std::cout << "p1:" << std::endl;
        // for(int i = 0; i < 16; i++)
            // std::cout << ((int*)&p1)[i] << " ";
        // std::cout << std::endl;

        _mm512_packstorelo_pd(output + iout         , vec0 );
        _mm512_packstorehi_pd(output + iout + stride, vec0 );
        _mm512_packstorelo_epi64(output_ptr + iout         , p0);
        _mm512_packstorehi_epi64(output_ptr + iout + stride, p0);
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + i0 + stride);
                p0  = _mm512_loadunpacklo_epi64(p0 , ptrA + i0         );
                p0  = _mm512_loadunpackhi_epi64(p0 , ptrA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputB + i1 + stride);
                p0  = _mm512_loadunpacklo_epi64(p0 , ptrB + i1         );
                p0  = _mm512_loadunpackhi_epi64(p0 , ptrB + i1 + stride);
                i1 += stride;
            }
            in_register_merge_key_l(vec0, vec1, p0, p1);
            _mm512_packstorelo_pd(output + iout         , vec0 );
            _mm512_packstorehi_pd(output + iout + stride, vec0 );
            _mm512_packstorelo_epi64(output_ptr + iout         , p0 );
            _mm512_packstorehi_epi64(output_ptr + iout + stride, p0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + i0 + stride);
                p0  = _mm512_loadunpacklo_epi64(p0 , ptrA + i0         );
                p0  = _mm512_loadunpackhi_epi64(p0 , ptrA + i0 + stride);
                i0 += stride;
                in_register_merge_key_l(vec0, vec1, p0, p1);
                _mm512_packstorelo_pd(output + iout         , vec0 );
                _mm512_packstorehi_pd(output + iout + stride, vec0 );
                _mm512_packstorelo_epi64(output_ptr + iout         , p0 );
                _mm512_packstorehi_epi64(output_ptr + iout + stride, p0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputB + i1 + stride);
                p0  = _mm512_loadunpacklo_epi64(p0 , ptrB + i1         );
                p0  = _mm512_loadunpackhi_epi64(p0 , ptrB + i1 + stride);
                i1 += stride;
                in_register_merge_key_l(vec0, vec1, p0, p1);
                _mm512_packstorelo_pd(output + iout         , vec0 );
                _mm512_packstorehi_pd(output + iout + stride, vec0 );
                _mm512_packstorelo_epi64(output_ptr + iout         , p0 );
                _mm512_packstorehi_epi64(output_ptr + iout + stride, p0 );
                iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_pd(buffer         , vec1 );
        _mm512_packstorehi_pd(buffer + stride, vec1 );
        _mm512_packstorelo_epi64(buffer_ptr         , p1 );
        _mm512_packstorehi_epi64(buffer_ptr + stride, p1 );

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

    }else
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
    __m512 vec0 = _mm512_set1_ps(1.f);
    __m512 vec1 = _mm512_set1_ps(1.f);
    uint8_t stride = (uint8_t)simd_width::AVX512_FLOAT;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    float buffer[stride];
    uint32_t i3=0;

    if(sizeA >= stride && sizeB >= stride)
    {
        vec0  = _mm512_loadunpacklo_ps(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_ps(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_ps(vec1 , inputB + stride);

        in_register_merge(vec0, vec1);

        _mm512_packstorelo_ps(output + iout         , vec0 );
        _mm512_packstorehi_ps(output + iout + stride, vec0 );
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride <= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputB + i1 + stride);
                i1 += stride;
            }
            in_register_merge(vec0, vec1);
            _mm512_packstorelo_ps(output + iout         , vec0 );
            _mm512_packstorehi_ps(output + iout + stride, vec0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_ps(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_ps(vec0 , inputA + i0 + stride);
                i0 += stride;
                in_register_merge(vec0, vec1);
                _mm512_packstorelo_ps(output + iout         , vec0 );
                _mm512_packstorehi_ps(output + iout + stride, vec0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
            vec0  = _mm512_loadunpacklo_ps(vec0 , inputB + i1         );
            vec0  = _mm512_loadunpackhi_ps(vec0 , inputB + i1 + stride);
            i1 += stride;
            in_register_merge(vec0, vec1);
            _mm512_packstorelo_ps(output + iout         , vec0 );
            _mm512_packstorehi_ps(output + iout + stride, vec0 );
            iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_ps(buffer         , vec1 );
        _mm512_packstorehi_ps(buffer + stride, vec1 );

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

    }else
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

template <typename T>
typename std::enable_if<std::is_same<T, double>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output)
{
    __m512d vec0 = _mm512_set1_pd(1.0);
    __m512d vec1 = _mm512_set1_pd(1.0);

    uint8_t stride = (uint8_t)simd_width::AVX512_DOUBLE;
    uint32_t i0=0;
    uint32_t i1=0;
    uint32_t iout=0;
    double buffer[stride];
    uint32_t i3=0;

    if(sizeA>=stride && sizeB>=stride)
    {
        vec0  = _mm512_loadunpacklo_pd(vec0 , inputA         );
        vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + stride);
        vec1  = _mm512_loadunpacklo_pd(vec1 , inputB         );
        vec1  = _mm512_loadunpackhi_pd(vec1 , inputB + stride);

        in_register_merge(vec0, vec1);

        _mm512_packstorelo_pd(output + iout         , vec0 );
        _mm512_packstorehi_pd(output + iout + stride, vec0 );
        i0 += stride;
        i1 += stride;
        iout += stride;

        while(i0+stride <= sizeA && i1+stride<= sizeB)
        {
            if(inputA[i0]<=inputB[i1])
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + i0 + stride);
                i0 += stride;
            }else
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputB + i1 + stride);
                i1 += stride;
            }
            in_register_merge(vec0, vec1);
            _mm512_packstorelo_pd(output + iout         , vec0 );
            _mm512_packstorehi_pd(output + iout + stride, vec0 );
            iout += stride;
        }
        while(i0+stride <= sizeA)
        {
            if(i1<sizeB && inputA[i0]<=inputB[i1] || i1==sizeB)
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputA + i0         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputA + i0 + stride);
                i0 += stride;
                in_register_merge(vec0, vec1);
                _mm512_packstorelo_pd(output + iout         , vec0 );
                _mm512_packstorehi_pd(output + iout + stride, vec0 );
                iout += stride;
            }else
                break;
        }
        while(i1+stride<= sizeB)
        {
            if(i0<sizeA && inputB[i1]<=inputA[i0] || i0==sizeA)
            {
                vec0  = _mm512_loadunpacklo_pd(vec0 , inputB + i1         );
                vec0  = _mm512_loadunpackhi_pd(vec0 , inputB + i1 + stride);
                i1 += stride;
                in_register_merge(vec0, vec1);
                _mm512_packstorelo_pd(output + iout         , vec0 );
                _mm512_packstorehi_pd(output + iout + stride, vec0 );
                iout += stride;
            }else
                break;
        }
        _mm512_packstorelo_pd(buffer         , vec1 );
        _mm512_packstorehi_pd(buffer + stride, vec1 );

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

    }else
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

} // end namespace aspas


