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
 * @file merger.tcc
 * Definition of merging functions to merge the sorted segments 
 * This file uses AVX/AVX512 instruction sets.
 *
 */

#include <immintrin.h> 
#include <algorithm> 
#include <type_traits> 

#include "aspas.h" 
#include "util/tools.h" 

namespace aspas
{

namespace internal
{

/*
template <typename T>
void merger(T*& orig, uint32_t size)
{
    uint8_t stride;
#ifdef __AVX__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX_DOUBLE;
    }
#endif
#ifdef __MIC__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX512_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX512_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX512_DOUBLE;
    }
#endif
    T *buf_array = new T[size]; 
    bool flip_flag = true;
    uint32_t i, j;

    for(i = stride; i < size; i = 2 * i) 
    {
        if(flip_flag) 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge(orig+j, std::min(j+i,size)-j, 
                      orig+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      buf_array+j);
            }
            flip_flag = false;
        } else 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge(buf_array+j, std::min(j+i,size)-j, 
                      buf_array+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      orig+j);
            }
            flip_flag = true;
        }
    }

    if(!flip_flag) util::copy_array(orig, size, buf_array, size);
    delete[] buf_array;

}
// */

template <typename T>
void merger(T*& orig, uint32_t size)
{
    uint8_t stride;
    uint32_t way; 
#ifdef __AVX__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX_INT;
        way = 16384;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX_FLOAT;
        way = 16384;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX_DOUBLE;
        way = 8192;
    }
#endif
#ifdef __MIC__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX512_INT;
        way = 256;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX512_FLOAT;
        way = 256;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX512_DOUBLE;
        way = 128;
    }
#endif

    T *buf_array = new T[size]; 
    bool flip_flag = true;
    uint32_t i, j;

    uint32_t k;
    uint32_t block_size = stride * way;

    // double tstart, tstop, ttime; 
    // tstart = dtime();
    for(k = 0; k < size; k += block_size)
    {
        flip_flag = true;
        for(i = stride; i < block_size; i = 2 * i) 
        {
            if(flip_flag) 
            {
                for(j = k; j < std::min(k+block_size,size); j = j + 2 * i) 
                {
                    merge(orig+j, std::min(j+i,std::min(k+block_size,size))-j, 
                          orig+std::min(j+i,std::min(k+block_size,size)), std::min(j+2*i,std::min(k+block_size,size))-std::min(j+i,std::min(k+block_size,size)), 
                          buf_array+j);
                }
                flip_flag = false;
            } else 
            {
                for(j = k; j < std::min(k+block_size,size); j = j + 2 * i) 
                {
                    merge(buf_array+j, std::min(j+i,std::min(k+block_size,size))-j, 
                          buf_array+std::min(j+i,std::min(k+block_size,size)), std::min(j+2*i,std::min(k+block_size,size))-std::min(j+i,std::min(k+block_size,size)), 
                          orig+j);
                }
                flip_flag = true;
            }
        }
    }
    // tstop = dtime();
    // ttime = tstop - tstart;
    // cout << "multiway\t" << way << "\ttime\t" << ttime << "\t" << endl;


    int num = way;
    int count = 0;
    while (num!= 0) 
    {
        if (num & 1 == 1) 
        {
            break;
        } else 
        {
            count++;
            num = num >> 1;
        }
    }
    if(count&1==1)
        flip_flag = false;
    else
        flip_flag = true;
    for(i = block_size; i < size; i = 2 * i) 
    {
        if(flip_flag) 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge(orig+j, std::min(j+i,size)-j, 
                      orig+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      buf_array+j);
            }
            flip_flag = false;
        } else 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge(buf_array+j, std::min(j+i,size)-j, 
                      buf_array+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      orig+j);
            }
            flip_flag = true;
        }
    }

    if(!flip_flag) std::copy(buf_array, buf_array+size, orig);// util::copy_array(orig, size, buf_array, size);
    delete[] buf_array;
}

/*
template <typename T>
void merger_key(T*& orig, int *&ptr, uint32_t size)
{
    uint8_t stride;
#ifdef __AVX__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX_DOUBLE;
    }
#endif
#ifdef __MIC__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX512_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX512_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX512_DOUBLE;
    }
#endif
    T *buf_array = new T[size]; 
    int *buf_ptr = new int[size]; 
    bool flip_flag = true;
    uint32_t i, j;

    for(i = stride; i < size; i = 2 * i) 
    {
        if(flip_flag) 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(orig+j, std::min(j+i,size)-j, 
                      orig+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      buf_array+j, ptr+j, ptr+std::min(j+i,size), buf_ptr+j);
            }
            flip_flag = false;
        } else 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(buf_array+j, std::min(j+i,size)-j, 
                      buf_array+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      orig+j, buf_ptr+j, buf_ptr+std::min(j+i,size), ptr+j);
            }
            flip_flag = true;
        }
    }

    if(!flip_flag) 
    {
        util::copy_array(orig, size, buf_array, size);
        util::copy_array(ptr, size, buf_ptr, size);
    }
    delete[] buf_array;
    delete[] buf_ptr;

}
// */

//*
template <typename T>
void merger_key(T*& orig, int *&ptr, uint32_t size)
{
    uint8_t stride;
    uint32_t way; 
#ifdef __AVX__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX_INT;
        way = 8192;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX_FLOAT;
        way = 8192;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX_DOUBLE;
        way = 4096;
    }
#endif
#ifdef __MIC__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX512_INT;
        way = 128;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX512_FLOAT;
        way = 128;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX512_DOUBLE;
        way = 64;
    }
#endif
    T *buf_array = new T[size]; 
    int *buf_ptr = new int[size]; 
    bool flip_flag = true;
    uint32_t i, j;

    uint32_t k;
    uint32_t block_size = stride * way;

    for(k = 0; k < size; k += block_size)
    {
        flip_flag = true;
        for(i = stride; i < block_size; i = 2 * i) 
        {
            if(flip_flag) 
            {
                for(j = k; j < std::min(k+block_size,size); j = j + 2 * i) 
                {
                    merge_key(orig+j, std::min(j+i,std::min(k+block_size,size))-j, 
                          orig+std::min(j+i,std::min(k+block_size,size)), std::min(j+2*i,std::min(k+block_size,size))-
                          std::min(j+i,std::min(k+block_size,size)), 
                          buf_array+j, ptr+j, ptr+std::min(j+i,size), buf_ptr+j);
                }
                flip_flag = false;
            } else 
            {
                for(j = k; j < std::min(k+block_size,size); j = j + 2 * i) 
                {
                    merge_key(buf_array+j, std::min(j+i,std::min(k+block_size,size))-j, 
                          buf_array+std::min(j+i,std::min(k+block_size,size)), std::min(j+2*i,std::min(k+block_size,size))-std::min(j+i,std::min(k+block_size,size)), 
                          orig+j, buf_ptr+j, buf_ptr+std::min(j+i,size), ptr+j );
                }
                flip_flag = true;
            }
        }
    }

    int num = way;
    int count = 0;
    while (num!= 0) 
    {
        if (num & 1 == 1) 
        {
            break;
        } else 
        {
            count++;
            num = num >> 1;
        }
    }
    if(count&1==1)
        flip_flag = false;
    else
        flip_flag = true;

    for(i = block_size; i < size; i = 2 * i) 
    {
        if(flip_flag) 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(orig+j, std::min(j+i,size)-j, 
                      orig+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      buf_array+j, ptr+j, ptr+std::min(j+i,size), buf_ptr+j);
            }
            flip_flag = false;
        } else 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(buf_array+j, std::min(j+i,size)-j, 
                      buf_array+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      orig+j, buf_ptr+j, buf_ptr+std::min(j+i,size), ptr+j);
            }
            flip_flag = true;
        }
    }

    if(!flip_flag) 
    {
        std::copy(buf_array, buf_array+size, orig);
        std::copy(buf_ptr, buf_ptr+size, ptr);
        // util::copy_array(orig, size, buf_array, size);
        // util::copy_array(ptr, size, buf_ptr, size);
    }
    delete[] buf_array;
    delete[] buf_ptr;

}
// */

/*
template <typename T>
void merger_key(T*& orig, long *&ptr, uint32_t size)
{
    uint8_t stride;
#ifdef __AVX__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX_DOUBLE;
    }
#endif
#ifdef __MIC__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX512_INT;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX512_FLOAT;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX512_DOUBLE;
    }
#endif
    T *buf_array = new T[size]; 
    long *buf_ptr = new long [size]; 
    bool flip_flag = true;
    uint32_t i, j;

    for(i = stride; i < size; i = 2 * i) 
    {
        if(flip_flag) 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(orig+j, std::min(j+i,size)-j, 
                      orig+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      buf_array+j, ptr+j, ptr+std::min(j+i,size), buf_ptr+j);
            }
            flip_flag = false;
        } else 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(buf_array+j, std::min(j+i,size)-j, 
                      buf_array+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      orig+j, buf_ptr+j, buf_ptr+std::min(j+i,size), ptr+j);
            }
            flip_flag = true;
        }
    }

    if(!flip_flag) 
    {
        util::copy_array(orig, size, buf_array, size);
        util::copy_array(ptr, size, buf_ptr, size);
    }
    delete[] buf_array;
    delete[] buf_ptr;

}
// */

//*
template <typename T>
void merger_key(T*& orig, long *&ptr, uint32_t size)
{
    uint8_t stride;
    uint32_t way; 
#ifdef __AVX__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX_INT;
        way = 8192;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX_FLOAT;
        way = 8192;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX_DOUBLE;
        way = 4096;
    }
#endif
#ifdef __MIC__
    if(std::is_same<T,int>::value)
    {
        stride = (uint8_t)simd_width::AVX512_INT;
        way = 64;
    }
    if(std::is_same<T,float>::value)
    {
        stride = (uint8_t)simd_width::AVX512_FLOAT;
        way = 64;
    }
    if(std::is_same<T,double>::value)
    {
        stride = (uint8_t)simd_width::AVX512_DOUBLE;
        way = 32;
    }
#endif
    T *buf_array = new T[size]; 
    long *buf_ptr = new long [size]; 
    bool flip_flag = true;
    uint32_t i, j;

    uint32_t k;
    uint32_t block_size = stride * way;

    for(k = 0; k < size; k += block_size)
    {
        flip_flag = true;
        for(i = stride; i < block_size; i = 2 * i) 
        {
            if(flip_flag) 
            {
                for(j = k; j < std::min(k+block_size,size); j = j + 2 * i) 
                {
                    merge_key(orig+j, std::min(j+i,std::min(k+block_size,size))-j, 
                          orig+std::min(j+i,std::min(k+block_size,size)), std::min(j+2*i,std::min(k+block_size,size))-
                          std::min(j+i,std::min(k+block_size,size)), 
                          buf_array+j, ptr+j, ptr+std::min(j+i,size), buf_ptr+j);
                }
                flip_flag = false;
            } else 
            {
                for(j = k; j < std::min(k+block_size,size); j = j + 2 * i) 
                {
                    merge_key(buf_array+j, std::min(j+i,std::min(k+block_size,size))-j, 
                          buf_array+std::min(j+i,std::min(k+block_size,size)), std::min(j+2*i,std::min(k+block_size,size))-std::min(j+i,std::min(k+block_size,size)), 
                          orig+j, buf_ptr+j, buf_ptr+std::min(j+i,size), ptr+j );
                }
                flip_flag = true;
            }
        }
    }

    int num = way;
    int count = 0;
    while (num!= 0) 
    {
        if (num & 1 == 1) 
        {
            break;
        } else 
        {
            count++;
            num = num >> 1;
        }
    }
    if(count&1==1)
        flip_flag = false;
    else
        flip_flag = true;

    for(i = block_size; i < size; i = 2 * i) 
    {
        if(flip_flag) 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(orig+j, std::min(j+i,size)-j, 
                      orig+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      buf_array+j, ptr+j, ptr+std::min(j+i,size), buf_ptr+j);
            }
            flip_flag = false;
        } else 
        {
            for(j = 0; j < size; j = j + 2 * i) 
            {
                merge_key(buf_array+j, std::min(j+i,size)-j, 
                      buf_array+std::min(j+i,size), std::min(j+2*i,size)-std::min(j+i,size), 
                      orig+j, buf_ptr+j, buf_ptr+std::min(j+i,size), ptr+j);
            }
            flip_flag = true;
        }
    }

    if(!flip_flag) 
    {
        std::copy(buf_array, buf_array+size, orig);
        std::copy(buf_ptr, buf_ptr+size, ptr);
        // util::copy_array(orig, size, buf_array, size);
        // util::copy_array(ptr, size, buf_ptr, size);
    }
    delete[] buf_array;
    delete[] buf_ptr;

}
// */

}

}


