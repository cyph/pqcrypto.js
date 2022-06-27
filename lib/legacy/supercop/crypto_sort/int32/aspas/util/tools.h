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
 * @file tools.h
 * Defines the tool functions for the sorting.
 *
 */

#ifndef UTIL_TOOLS_H_
#define UTIL_TOOLS_H_

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <sys/time.h>

#include "aspas.h"

namespace util
{

/**
 * This method checks if the test_array is sorted.
 *
 * @param test_array sorted array candidate
 * @param size array size
 * @return true if the array has been sorted,
 * false if the array hasn't been sorted
 *
 */
template <typename T>
bool check_sorted(T* test_array, uint32_t size) 
{
    uint32_t i;
    for(i = 1; i < size; i++) 
    {
        if ( test_array[i] < test_array[i-1] ) 
        {
            // std::cout << std::endl;
            // std::cout << "i:" << i << std::endl;
            // std::cout << "a:" << test_array[i-1] << std::endl;
            // std::cout << "b:" << test_array[i] << std::endl;
            return false;
        }
    }
    return true;
}

template <typename T>
bool check_sorted_key(T* test_array, int *id, uint32_t size) 
{
    T *sorted = new T[size];

    for(int i = 0; i < size; i++)
    {
        sorted[i] = test_array[id[i]];
    }

    bool ret_flag = std::is_sorted(sorted, sorted+size);
    delete[] sorted;

    return ret_flag;
}

template <typename T>
bool check_sorted_key(T* test_array, long *id, uint32_t size) 
{
    T *sorted = new T[size];

    for(int i = 0; i < size; i++)
    {
        sorted[i] = test_array[id[i]];
    }

    bool ret_flag = std::is_sorted(sorted, sorted+size);
    delete[] sorted;

    return ret_flag;
}

/**
 * This method checks if the test_array is partially sorted.
 * The partially sorted array is divided by segments. The size 
 * of each segment is same with the vector register.
 *
 * @param test_array sorted array candidate
 * @param size array size
 * @return true if the array has been partially sorted,
 * false if the array hasn't been partially sorted
 *
 */
template <typename T>
bool check_partially_sorted(T* test_array, uint32_t size) 
{

    uint8_t stride;
#ifdef __AVX__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)aspas::simd_width::AVX_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)aspas::simd_width::AVX_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)aspas::simd_width::AVX_DOUBLE;
    }
#endif
#ifdef __MIC__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)aspas::simd_width::AVX512_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)aspas::simd_width::AVX512_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)aspas::simd_width::AVX512_DOUBLE;
    }
#endif
 
    uint32_t i, j;
    for(i = 0; i + stride - 1 < size; i += stride) 
    {
        for(j = i; j - stride + 1 < i; j++)
        {
            if ( test_array[j] > test_array[j+1] ) 
            {
                return false;
            }
        }
    }
    for(/* cont'd */; i < size-1; i++) 
    {
        if ( test_array[i] > test_array[i+1] ) 
        {
            return false;
        }
    }
    
    return true;
}

/**
 * This method lists all the elements in arr.
 *
 * @param arr targeting array 
 * @param size array size
 * @return print out the elements in arr 
 *
 */
template <typename T>
void list_array(T*& arr, uint32_t size, char* prompt) 
{
    if(prompt != NULL)
        std::cerr << prompt << std::endl;
    uint32_t i;
    int LINE_SIZE;
#ifdef __AVX__
    if(std::is_same<T, int>::value)
        LINE_SIZE = (int)aspas::simd_width::AVX_INT;
    if(std::is_same<T, float>::value)
        LINE_SIZE = (int)aspas::simd_width::AVX_FLOAT;
    if(std::is_same<T, double>::value)
        LINE_SIZE = (double)aspas::simd_width::AVX_DOUBLE;
#endif
#ifdef __MIC__
    if(std::is_same<T, int>::value)
        LINE_SIZE = (int)aspas::simd_width::AVX512_INT;
    if(std::is_same<T, float>::value)
        LINE_SIZE = (int)aspas::simd_width::AVX512_FLOAT;
    if(std::is_same<T, double>::value)
        LINE_SIZE = (double)aspas::simd_width::AVX512_DOUBLE;
#endif
    for (i = 0; i < size; i++) {
        std::cerr << arr[i] << "\t";
        if((i + 1) % LINE_SIZE == 0) 
            std::cerr << std::endl;
    }
    if((i) % LINE_SIZE != 0) 
        std::cerr << std::endl;
}

/**
 * This method copies elements from b to a.
 *
 * @param a destination array 
 * @param n_a destination array size 
 * @param b source array 
 * @param n_b source array size 
 * @return elements copied from b to a 
 *
 */
template <typename T>
void copy_array(T* a, uint32_t n_a, T* b, uint32_t n_b) 
{
    uint32_t i; 
    if(n_a!=n_b)
    {
        std::cerr << "error: copy size different" << std::endl;
        return ;
    }
    for(i = 0; i < n_a ; i++) 
    {
        a[i] = b[i];
    }
}

/**
 * This method returns the current timestamp.
 *
 * @return the current time in second(s)
 *
 */
double dtime() 
{
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime, (struct timezone*)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec*1.0e-6);
    return (tseconds);
}

}

#endif
