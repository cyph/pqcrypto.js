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
 * @file aspas.h
 * @author Kaixi Hou 
 *
 * Declaration of utility functions of the auto-generated sort, 
 * merge, and parallel_sort.
 *
 */

#ifndef ASPAS_H_
#define ASPAS_H_

#include <utility>
#include <cstdint>
#include <unistd.h>

/**
 * This namespace contains parallel sorting functions that operate on various
 * primitive data types (i.e. int, float, double) on both of AVX and AVX512 ISA.
 */
//! Auto-generated SIMD parallel sorting tools interfaces and implementations. 
namespace aspas
{


/// thread_num is set to be the value of the logical cores of the current platform.
uint32_t thread_num = sysconf(_SC_NPROCESSORS_ONLN);

/**
 * Represents the number of elements of various types one vector can hold
 * on different platforms.
 */
enum class simd_width : std::int8_t
{
    /// SIMD width of integer types in AVX ISA (CPU)
    AVX_INT       = 8 ,
    /// SIMD width of float types in AVX ISA (CPU)
    AVX_FLOAT     = 8 ,
    /// SIMD width of double types in AVX ISA (CPU)
    AVX_DOUBLE    = 4 ,
    /// SIMD width of integer types in AVX512 ISA (MIC)
    AVX512_INT    = 16,
    /// SIMD width of float types in AVX512 ISA (MIC)
    AVX512_FLOAT  = 16,
    /// SIMD width of double types in AVX512 ISA (MIC)
    AVX512_DOUBLE = 8 
};

/**
 * This method changes the number of the parallel threads. This value is stored in the
 * variable named thread_num, which is set to the number of the logical cores by 
 * default.
 * 
 * @param num number of parallel threads 
 * @return 
 *
 */
//! This method changes the number of the parallel threads.
void thread_num_init(uint32_t num);

/**
 * This method sorts the given input array. Currently the input array can be of the type
 * of int, float, and double.
 *
 * @param array the pointer to the first element of the input array
 * @param size the size of the input array
 * @return the sorted elements are stored in the pointer of array
 *
 */
//! This method sorts the given input array.
template <class T>
void sort(T* array, uint32_t size);

/**
 * This method sorts the given input key array with associative indices. 
 * The key array can be of data type of int, float, and double.
 *
 * @param array the pointer to the first element of the key array
 * @param id the pointer referring to the corresponding element of the key 
 * @param size the size of the input key array
 * @return the sorted elements are stored in the pointer of array
 *
 */
//! This method sorts the given input key array.
template <class T>
void sort_key(T* array, int *id, uint32_t size);

template <class T>
void sort_key(T* array, long *id, uint32_t size);

/**
 * Integer version <br>
 * This method merges two sorted input arrays pointed by inputA and inputB respectively.
 *
 * @param inputA the first sorted array
 * @param sizeA the size of the first array
 * @param inputB the second sorted array
 * @param sizeB the size of the second array
 * @param output the saving target of the merged array
 * @return 
 *
 */
//! This method merges two sorted input arrays into one.
template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output);

/**
 * Float version <br>
 * This method merges two sorted input arrays pointed by inputA and inputB respectively.
 *
 * @param inputA the first sorted array
 * @param sizeA the size of the first array
 * @param inputB the second sorted array
 * @param sizeB the size of the second array
 * @param output the saving target of the merged array
 * @return 
 *
 */
//! This method merges two sorted input arrays into one.
template <typename T>
typename std::enable_if<std::is_same<T, float>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output);

/**
 * Double version <br>
 * This method merges two sorted input arrays pointed by inputA and inputB respectively.
 *
 * @param inputA the first sorted array
 * @param sizeA the size of the first array
 * @param inputB the second sorted array
 * @param sizeB the size of the second array
 * @param output the saving target of the merged array
 * @return 
 *
 */
//! This method merges two sorted input arrays into one.
template <typename T>
typename std::enable_if<std::is_same<T, double>::value>::type
merge(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output);

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr);

template <typename T>
typename std::enable_if<std::is_same<T, float>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr);

template <typename T>
typename std::enable_if<std::is_same<T, double>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, int *ptrA, int *ptrB, int *output_ptr);

template <typename T>
typename std::enable_if<std::is_same<T, int>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr);

template <typename T>
typename std::enable_if<std::is_same<T, float>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr);

template <typename T>
typename std::enable_if<std::is_same<T, double>::value>::type
merge_key(T* inputA, uint32_t sizeA, T* inputB, uint32_t sizeB, T* output, long *ptrA, long *ptrB, long *output_ptr);


/**
 * This method sorts the input array using (thread_num) threads in parallel. By default, 
 * the threads used are equal to the number of the logical cores.
 *
 * @param array the input array
 * @param size the size of the input array
 * @return the sorted elements are stored in the pointer of array
 *
 */
//! This method sorts the input array using multiple threads.
template <class T>
void parallel_sort(T*& array, uint32_t size);

template <class T>
void parallel_sort_key(T*& array, int *id, uint32_t size);

template <class T>
void parallel_sort_key(T*& array, long *id, uint32_t size);

}

#include "aspas.tcc"

#ifdef __AVX2__
#include "aspas_merge_avx2.tcc" 
#else
#ifdef __AVX__
#include "aspas_merge_avx.tcc"
#endif
#endif

#ifdef __MIC__
#include "aspas_merge_mic.tcc" 
#endif


#endif
