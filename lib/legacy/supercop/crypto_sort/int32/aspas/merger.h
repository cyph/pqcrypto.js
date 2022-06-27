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
 * @file merger.h
 * Declaration of merging functions to merge the sorted segments 
 *
 */

#ifndef ASPAS_MERGER_H_
#define ASPAS_MERGER_H_

#include <cstdint>

namespace aspas
{

namespace internal
{

/**
 * This method merges the sorted data in the segments.
 *
 * @param data targeting data
 * @param size data size
 * @return fully sorted data
 *
 */
template <typename T>
void merger(T*& data, uint32_t size);

/**
 * This method merges the sorted keys in the segments.
 *
 * @param data targeting key data
 * @param ptr targeting pointer data
 * @param size data size
 * @return fully sorted data
 *
 */
template <typename T>
void merger_key(T*& data, int *&ptr, uint32_t size);

template <typename T>
void merger_key(T*& data, long *&ptr, uint32_t size);

}

}

#include "merger.tcc"

#endif

