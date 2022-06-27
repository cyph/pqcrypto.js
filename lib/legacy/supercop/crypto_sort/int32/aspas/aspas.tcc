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
 * @file aspas.tcc
 * @author Kaixi Hou
 *
 * Definition of utility functions of the auto-generated sort, 
 * parallel_sort.
 *
 */

#include <cstdint>
#include <cmath>
#include <pthread.h>

#include "aspas.h" 
#include "sorter.h" 
#include "merger.h" 
#include "util/tools.h" 

namespace aspas
{

void thread_num_init(uint32_t num)
{
    thread_num = num;
    return;
}

template <class T>
void sort(T* array, uint32_t size)
{
    internal::sorter(array, size);
    internal::merger(array, size);
    return;
}

template <class T>
void sort_key(T* array, int *id, uint32_t size)
{
    // std::cout << "before:" << std::endl;
    // for(int i = 0; i < size; i++)
    // {
        // std::cout << id[i] << " ";
        // if((i + 1) % 4 == 0)
            // std::cout << std::endl;
    // }
    // std::cout << std::endl;
    internal::sorter_key(array, id, size);
    // std::cout << "\nafter:" << std::endl;
    // for(int i = 0; i < size; i++)
    // {
        // std::cout << id[i] << " ";
        // if((i + 1) % 4 == 0)
            // std::cout << std::endl;
    // }
    // std::cout << std::endl;
    internal::merger_key(array, id, size);
    // std::cout << "finally:" << std::endl;
    // for(int i = 0; i < size; i++)
    // {
        // std::cout << id[i] << " ";
        // if((i + 1) % 4 == 0)
            // std::cout << std::endl;
    // }
    // std::cout << std::endl;
    return;
}

template <class T>
void sort_key(T* array, long *id, uint32_t size)
{
    // std::cout << "before:" << std::endl;
    // for(int i = 0; i < size; i++)
    // {
        // std::cout << id[i] << " ";
        // if((i + 1) % 8 == 0)
            // std::cout << std::endl;
    // }
    // std::cout << std::endl;
    internal::sorter_key(array, id, size);
    // std::cout << "\nafter:" << std::endl;
    // for(int i = 0; i < size; i++)
    // {
        // std::cout << id[i] << " ";
        // if((i + 1) % 8 == 0)
            // std::cout << std::endl;
    // }
    // std::cout << std::endl;
    internal::merger_key(array, id, size);
    // std::cout << "finally:" << std::endl;
    // for(int i = 0; i < size; i++)
    // {
        // std::cout << id[i] << " ";
        // if((i + 1) % 8 == 0)
            // std::cout << std::endl;
    // }
    // std::cout << std::endl;
    return;
}

template<class T>
struct args_t
{
	uint32_t start;
	uint32_t end;
    T *input;
};

template<class T>
struct args_split
{
	uint32_t start;
	uint32_t mid;
	uint32_t end;
    uint32_t tid;
    uint32_t factor;
    T *input;
    T *inputa;
    int *startA;
    int *startB;
}; 

template<class T>
struct args_t_key
{
	uint32_t start;
	uint32_t end;
    T *input;
    int *id;
};

template<class T>
struct args_split_key
{
	uint32_t start;
	uint32_t mid;
	uint32_t end;
    uint32_t tid;
    uint32_t factor;
    T *input;
    T *inputa;
    int *id;
    int *ida;
    int *startA;
    int *startB;
}; 

template<class T>
struct args_t_key_l
{
	uint32_t start;
	uint32_t end;
    T *input;
    long *id;
};

template<class T>
struct args_split_key_l
{
	uint32_t start;
	uint32_t mid;
	uint32_t end;
    uint32_t tid;
    uint32_t factor;
    T *input;
    T *inputa;
    long *id;
    long *ida;
    int *startA;
    int *startB;
}; 


template<class T>
void* thread_sort_kernel(void* arguments)
{
	args_t<T>* args = (args_t<T>*)arguments;
    uint32_t start = args->start;
    uint32_t end   = args->end;

    sort<T>((args->input)+start, end-start);

	pthread_exit(NULL);
}

template<class T>
void* thread_sort_kernel_key(void* arguments)
{
	args_t_key<T>* args = (args_t_key<T>*)arguments;
    uint32_t start = args->start;
    uint32_t end   = args->end;

    sort_key<T>((args->input)+start, (args->id)+start, end-start);

	pthread_exit(NULL);
}

template<class T>
void* thread_sort_kernel_key_l(void* arguments)
{
	args_t_key_l<T>* args = (args_t_key_l<T>*)arguments;
    uint32_t start = args->start;
    uint32_t end   = args->end;

    sort_key<T>((args->input)+start, (args->id)+start, end-start);

	pthread_exit(NULL);
}

template<class T>
void* thread_merge_kernel(void* arguments)
{
	args_split<T>* args = (args_split<T>*)arguments;
    uint32_t i, j, k;
    i = args->start;
    j = args->mid;
    k = args->end;
    int SIZEA = j - i;
    int SIZEB = k - j;
    int SIZEC = k - i;
    int tid = args->tid;
    int factor = args->factor;
    int *startA = args->startA;
    int *startB = args->startB;

    long target = (long)SIZEC*tid/factor;
    int offset = (int) target;
    // std::merge(args->input+startA[tid], args->input+startA[tid+1], args->input+SIZEA+startB[tid], args->input+SIZEA+startB[tid+1], args->inputa+offset);
    merge<T>(args->input+startA[tid], startA[tid+1]-startA[tid], args->input+SIZEA+startB[tid], startB[tid+1]-startB[tid], args->inputa+offset/*SIZEC/factor*tid*/);
	pthread_exit(NULL);
}

template<class T>
void* thread_merge_kernel_key(void* arguments)
{
	args_split_key<T>* args = (args_split_key<T>*)arguments;
    uint32_t i, j, k;
    i = args->start;
    j = args->mid;
    k = args->end;
    int SIZEA = j - i;
    int SIZEB = k - j;
    int SIZEC = k - i;
    int tid = args->tid;
    int factor = args->factor;
    int *startA = args->startA;
    int *startB = args->startB;

    long target = (long)SIZEC*tid/factor;
    int offset = (int) target;

    merge_key<T>(args->input+startA[tid], startA[tid+1]-startA[tid], args->input+SIZEA+startB[tid], startB[tid+1]-startB[tid], args->inputa+offset, args->id+startA[tid], args->id+SIZEA+startB[tid], args->ida+offset);
	pthread_exit(NULL);
}

template<class T>
void* thread_merge_kernel_key_l(void* arguments)
{
	args_split_key_l<T>* args = (args_split_key_l<T>*)arguments;
    uint32_t i, j, k;
    i = args->start;
    j = args->mid;
    k = args->end;
    int SIZEA = j - i;
    int SIZEB = k - j;
    int SIZEC = k - i;
    int tid = args->tid;
    int factor = args->factor;
    int *startA = args->startA;
    int *startB = args->startB;

    long target = (long)SIZEC*tid/factor;
    int offset = (int) target;

    merge_key<T>(args->input+startA[tid], startA[tid+1]-startA[tid], args->input+SIZEA+startB[tid], startB[tid+1]-startB[tid], args->inputa+offset, args->id+startA[tid], args->id+SIZEA+startB[tid], args->ida+offset);
	pthread_exit(NULL);
}

template<class T>
T find_kth(T oA[], T A[], int m, T oB[], T B[], int n, int k, int &indA, int &indB)  
{
    // always assume that m is equal or smaller than n
    if(m > n)  
        return find_kth(oB, B, n, oA, A, m, k, indB, indA);
    if(m == 0)  
    {
        indA = A-1-oA;
        indB = B+k-1-oB;
        return B[k - 1];
    }
    if(k == 1)  
    {
        if(A[0]<=B[0])
        {
            indA = A-oA;
            indB = B-1-oB;
            return A[0];
        }
        else
        {
            indA = A-1-oA;
            indB = B-oB;
            return B[0];
        }
        // return min(A[0], B[0]);
    }
    //divide  k  into  two  parts
    int ia = std::min(k / 2, m), ib = k - ia;
    if(A[ia - 1] < B[ib - 1])
    {
        return find_kth(oA, A + ia, m - ia, oB, B, n, k - ia, indA, indB);
    }
    else if (A[ia - 1] > B[ib - 1])
    {
        return find_kth(oA, A, m, oB, B + ib, n - ib, k - ib, indA, indB);
    }
    else
    {
        indA = A+ia-1-oA;
        indB = B+ib-1-oB;
        return A[ia - 1];
    }
}

template<typename iT>
void find_kth2(iT* key_left_start,
              iT* key_left_end,
              iT* key_right_start,
              iT* key_right_end,
              int Kth,        
              int &key_left_idx_i,
              int &key_right_idx_i)
{
    int k = Kth;
    int key_right_top = k < (key_right_end - key_right_start) ? k : (key_right_end - key_right_start);
    int key_right_bottom = (key_left_end - key_left_start) < k ? (k - (key_left_end - key_left_start)) : 0;
    int key_left_top = k > (key_right_end - key_right_start) ? (k - (key_right_end - key_right_start)) : 0;
    int antidiag_middle;

    if(k == (key_right_end - key_right_start + key_left_end - key_left_start))
    {
        key_right_idx_i = key_right_end - key_right_start -1;
        key_left_idx_i = key_left_end - key_left_start -1;
        return ;
    }
    //if(key_right_top == key_right_bottom) std::cout<<"error0:"<<k<<", top = "<<key_right_top<<", bottom = "<<key_right_bottom<<", left_length = "<<key_left_end - key_left_start<<", right_length = "<<key_right_end - key_right_start<<std::endl;

    if((key_left_end - key_left_start) ==0 || (key_right_end - key_right_start) == 0)
    {
        if((key_left_end - key_left_start) ==0)
        {
            key_left_idx_i = -1;
            key_right_idx_i = k - 1;
            return;
        }
        else if((key_right_end - key_right_start) == 0)
        {
            key_right_idx_i = -1;
            key_left_idx_i = k -1;
            return;
        }
        else
        {
            std::cerr<<"ERROR!: look for "<<k<<"th element, total input length is 0!"<<std::endl;
        }
    }
    while (true)
    {
        // handle different corner cases
        if(key_right_top == key_right_bottom + 1)
        {
            if(*(key_right_start + key_right_bottom) > *(key_left_start + key_left_top))
            {
                key_right_idx_i = key_right_bottom;
                key_left_idx_i = key_left_top + 1;
                break;
            }
            else
            {
                key_right_idx_i = key_right_top;
                key_left_idx_i = key_left_top;
                break;
            }
        }

        antidiag_middle = (int)ceil((double)(key_right_top - key_right_bottom) / 2.0);
        //if(antidiag_middle == 0) std::cout<<"error1: top = "<<key_right_top<<", bottom = "<<key_right_bottom<<std::endl;
        //if(key_right_bottom == (key_right_top - antidiag_middle)) std::cout<<"error2, top = "<<key_right_top<<", bottom = "<<key_right_bottom<<std::endl;
       if (*(key_right_start + key_right_top - antidiag_middle) > *(key_left_start + key_left_top + antidiag_middle - 1))
        {
            // check the lower p
            if (*(key_right_start + key_right_top - antidiag_middle - 1) <= *(key_left_start + key_left_top + antidiag_middle))
            {
                // find intersection point
                key_left_idx_i = key_left_top + antidiag_middle;
                key_right_idx_i = key_right_top - antidiag_middle;
                break;
            }
            else
            {
                // move top point
                key_left_top = key_left_top + antidiag_middle;
                key_right_top = key_right_top - antidiag_middle;
            }
        }
        else
        {
            key_right_bottom = key_right_top - antidiag_middle;
        }
    }
    key_left_idx_i--;
    key_right_idx_i--;
}

template<class T>
void* thread_findkth_kernel(void* arguments)
{
	args_split<T>* args = (args_split<T>*)arguments;
    uint32_t i, j, k;
    i = args->start;
    j = args->mid;
    k = args->end;
    int SIZEA = j - i;
    int SIZEB = k - j;
    int SIZEC = k - i;
    int tid = args->tid;
    int factor = args->factor;
    int *startA = args->startA;
    int *startB = args->startB;

    int indA, indB;

    long target = (long)SIZEC*(tid+1)/factor; // avoid int overflow
    int kth = (int) target;

    // find_kth(args->input, args->input, SIZEA, args->input+SIZEA, args->input+SIZEA, SIZEB, kth/*SIZEC/factor*(tid+1)*/, indA, indB);
    find_kth2(args->input, args->input+SIZEA, args->input+SIZEA, args->input+SIZEA+SIZEB, kth/*SIZEC/factor*(tid+1)*/, indA, indB);
    args->startA[tid+1] = indA+1;
    args->startB[tid+1] = indB+1;

	pthread_exit(NULL);
}

template<class T>
void* thread_findkth_kernel_key(void* arguments)
{
	args_split_key<T>* args = (args_split_key<T>*)arguments;
    uint32_t i, j, k;
    i = args->start;
    j = args->mid;
    k = args->end;
    int SIZEA = j - i;
    int SIZEB = k - j;
    int SIZEC = k - i;
    int tid = args->tid;
    int factor = args->factor;
    int *startA = args->startA;
    int *startB = args->startB;

    int indA, indB;

    long target = (long)SIZEC*(tid+1)/factor; // avoid int overflow
    int kth = (int) target;

    // find_kth(args->input, args->input, SIZEA, args->input+SIZEA, args->input+SIZEA, SIZEB, kth/*SIZEC/factor*(tid+1)*/, indA, indB);
    find_kth2(args->input, args->input+SIZEA, args->input+SIZEA, args->input+SIZEA+SIZEB, kth/*SIZEC/factor*(tid+1)*/, indA, indB);
    args->startA[tid+1] = indA+1;
    args->startB[tid+1] = indB+1;

	pthread_exit(NULL);
}

template<class T>
void* thread_findkth_kernel_key_l(void* arguments)
{
	args_split_key_l<T>* args = (args_split_key_l<T>*)arguments;
    uint32_t i, j, k;
    i = args->start;
    j = args->mid;
    k = args->end;
    int SIZEA = j - i;
    int SIZEB = k - j;
    int SIZEC = k - i;
    int tid = args->tid;
    int factor = args->factor;
    int *startA = args->startA;
    int *startB = args->startB;

    int indA, indB;

    long target = (long)SIZEC*(tid+1)/factor; // avoid int overflow
    int kth = (int) target;

    // find_kth(args->input, args->input, SIZEA, args->input+SIZEA, args->input+SIZEA, SIZEB, kth/*SIZEC/factor*(tid+1)*/, indA, indB);
    find_kth2(args->input, args->input+SIZEA, args->input+SIZEA, args->input+SIZEA+SIZEB, kth/*SIZEC/factor*(tid+1)*/, indA, indB);
    args->startA[tid+1] = indA+1;
    args->startB[tid+1] = indB+1;

	pthread_exit(NULL);
}

template <class T>
void parallel_sort(T*& array, uint32_t size)
{
	pthread_t* threads = new pthread_t[thread_num];
	args_t<T>* thread_args = new args_t<T>[thread_num]; 

    uint32_t b = size / thread_num;
    uint32_t m = size % thread_num;
    for(uint32_t i = 0; i < thread_num; i++)
    {
        if(i < m)
        {
            thread_args[i].start=i*(b+1);
            thread_args[i].end=thread_args[i].start+b+1;
        }else
        {
            thread_args[i].start=i*b+m;
            thread_args[i].end=thread_args[i].start+b;
        }
        thread_args[i].input = array;
		pthread_create(&threads[i], NULL, thread_sort_kernel<T>, &thread_args[i]);
    }

	for(uint32_t i = 0; i < thread_num; i++)
	{
    	pthread_join(threads[i], NULL);
	}
    
    // merge sort buffer 
    T* inputa = new T[size]; 
    // works on inputa if true 
    bool flaga = false; 
    uint32_t segments = thread_num;
    uint32_t tnum;
    args_split<T>* as_old;
    args_split<T>* as_new;

    as_old = new args_split<T>[thread_num];
    for(uint32_t i = 0; i < thread_num; i++)
    {
        as_old[i].start = thread_args[i].start;
        as_old[i].end   = thread_args[i].end;
    }

    as_new = new args_split<T>[thread_num];
    int factor = 2; 
    int v = thread_num; // v is the closest power of 2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    int *startA = (int *)malloc(sizeof(int)*(((thread_num+(factor-1))/factor)*(factor+1)));
    int *startB = (int *)malloc(sizeof(int)*(((thread_num+(factor-1))/factor)*(factor+1)));
    while(factor <= v)
    {
        
        for(uint32_t i = 0; i < thread_num; i++)
        {
            as_new[i].start = as_old[(i/factor)*factor].start;

            if((i/factor)*factor+factor/2<thread_num)
                as_new[i].mid = as_old[(i/factor)*factor+factor/2].start;
            else
                as_new[i].mid = as_old[thread_num-1].end;

            if((i/factor)*factor+factor<thread_num)
                as_new[i].end = as_old[(i/factor)*factor+factor].start;
            else
                as_new[i].end = as_old[thread_num-1].end;

            int tfactor = std::min(thread_num, (i/factor+1)*factor)-(i/factor)*factor;
            as_new[i].tid = i % tfactor;
            as_new[i].factor = tfactor;
            as_new[i].startA = startA+(i/factor)*(factor+1);
            as_new[i].startB = startB+(i/factor)*(factor+1);
            if(as_new[i].tid == 0)
            {
                as_new[i].startA[0] = 0;
                as_new[i].startB[0] = 0;
            }
            if(flaga)
            {
                as_new[i].input = inputa + as_new[i].start;
                as_new[i].inputa = array + as_new[i].start;
            } else
            {
                as_new[i].input = array + as_new[i].start;
                as_new[i].inputa = inputa + as_new[i].start;
            }

            pthread_create(&threads[i], NULL, thread_findkth_kernel<T>, &as_new[i]);
        }

        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_create(&threads[i], NULL, thread_merge_kernel<T>, &as_new[i]);
        }
        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        if(flaga)
        {
            flaga=false;
        } else
        {
            flaga=true;
        }
        factor *= 2;
    }
    free(startA);
    free(startB);

    if(flaga)
        util::copy_array(array, size, inputa, size);

    delete[] inputa;
    return;

}

template <class T>
void parallel_sort_key(T*& array, int *id, uint32_t size)
{
	pthread_t* threads = new pthread_t[thread_num];
	args_t_key<T>* thread_args = new args_t_key<T>[thread_num]; 

    uint32_t b = size / thread_num;
    uint32_t m = size % thread_num;
    for(uint32_t i = 0; i < thread_num; i++)
    {
        if(i < m)
        {
            thread_args[i].start=i*(b+1);
            thread_args[i].end=thread_args[i].start+b+1;
        }else
        {
            thread_args[i].start=i*b+m;
            thread_args[i].end=thread_args[i].start+b;
        }
        thread_args[i].input = array;
        thread_args[i].id = id;
		pthread_create(&threads[i], NULL, thread_sort_kernel_key<T>, &thread_args[i]);
    }

	for(uint32_t i = 0; i < thread_num; i++)
	{
    	pthread_join(threads[i], NULL);
	}
    
    // merge sort buffer 
    T* inputa = new T[size]; 
    int* ida = new int[size]; 
    // works on inputa if true 
    bool flaga = false; 
    uint32_t segments = thread_num;
    uint32_t tnum;
    args_split_key<T>* as_old;
    args_split_key<T>* as_new;

    as_old = new args_split_key<T>[thread_num];
    for(uint32_t i = 0; i < thread_num; i++)
    {
        as_old[i].start = thread_args[i].start;
        as_old[i].end   = thread_args[i].end;
    }

    as_new = new args_split_key<T>[thread_num];
    int factor = 2; 
    int v = thread_num; // v is the closest power of 2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    int *startA = (int *)malloc(sizeof(int)*(((thread_num+(factor-1))/factor)*(factor+1)));
    int *startB = (int *)malloc(sizeof(int)*(((thread_num+(factor-1))/factor)*(factor+1)));
    while(factor <= v)
    {
        
        for(uint32_t i = 0; i < thread_num; i++)
        {
            as_new[i].start = as_old[(i/factor)*factor].start;

            if((i/factor)*factor+factor/2<thread_num)
                as_new[i].mid = as_old[(i/factor)*factor+factor/2].start;
            else
                as_new[i].mid = as_old[thread_num-1].end;

            if((i/factor)*factor+factor<thread_num)
                as_new[i].end = as_old[(i/factor)*factor+factor].start;
            else
                as_new[i].end = as_old[thread_num-1].end;

            int tfactor = std::min(thread_num, (i/factor+1)*factor)-(i/factor)*factor;
            as_new[i].tid = i % tfactor;
            as_new[i].factor = tfactor;
            as_new[i].startA = startA+(i/factor)*(factor+1);
            as_new[i].startB = startB+(i/factor)*(factor+1);
            if(as_new[i].tid == 0)
            {
                as_new[i].startA[0] = 0;
                as_new[i].startB[0] = 0;
            }
            if(flaga)
            {
                as_new[i].input = inputa + as_new[i].start;
                as_new[i].inputa = array + as_new[i].start;
                as_new[i].id = ida + as_new[i].start;
                as_new[i].ida = id + as_new[i].start;
            } else
            {
                as_new[i].input = array + as_new[i].start;
                as_new[i].inputa = inputa + as_new[i].start;
                as_new[i].id = id + as_new[i].start;
                as_new[i].ida = ida + as_new[i].start;
            }

            pthread_create(&threads[i], NULL, thread_findkth_kernel_key<T>, &as_new[i]);
        }

        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_create(&threads[i], NULL, thread_merge_kernel_key<T>, &as_new[i]);
        }
        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        if(flaga)
        {
            flaga=false;
        } else
        {
            flaga=true;
        }
        factor *= 2;
    }
    free(startA);
    free(startB);

    if(flaga)
    {
        util::copy_array(array, size, inputa, size);
        util::copy_array(id, size, ida, size);
    }

    delete[] inputa;
    delete[] ida;
    return;

}

template <class T>
void parallel_sort_key(T*& array, long *id, uint32_t size)
{
	pthread_t* threads = new pthread_t[thread_num];
	args_t_key_l<T>* thread_args = new args_t_key_l<T>[thread_num]; 

    uint32_t b = size / thread_num;
    uint32_t m = size % thread_num;
    for(uint32_t i = 0; i < thread_num; i++)
    {
        if(i < m)
        {
            thread_args[i].start=i*(b+1);
            thread_args[i].end=thread_args[i].start+b+1;
        }else
        {
            thread_args[i].start=i*b+m;
            thread_args[i].end=thread_args[i].start+b;
        }
        thread_args[i].input = array;
        thread_args[i].id = id;
		pthread_create(&threads[i], NULL, thread_sort_kernel_key_l<T>, &thread_args[i]);
    }

	for(uint32_t i = 0; i < thread_num; i++)
	{
    	pthread_join(threads[i], NULL);
	}
    
    // merge sort buffer 
    T* inputa = new T[size]; 
    long* ida = new long[size]; 
    // works on inputa if true 
    bool flaga = false; 
    uint32_t segments = thread_num;
    uint32_t tnum;
    args_split_key_l<T>* as_old;
    args_split_key_l<T>* as_new;

    as_old = new args_split_key_l<T>[thread_num];
    for(uint32_t i = 0; i < thread_num; i++)
    {
        as_old[i].start = thread_args[i].start;
        as_old[i].end   = thread_args[i].end;
    }

    as_new = new args_split_key_l<T>[thread_num];
    int factor = 2; 
    int v = thread_num; // v is the closest power of 2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    int *startA = (int *)malloc(sizeof(int)*(((thread_num+(factor-1))/factor)*(factor+1)));
    int *startB = (int *)malloc(sizeof(int)*(((thread_num+(factor-1))/factor)*(factor+1)));
    while(factor <= v)
    {
        
        for(uint32_t i = 0; i < thread_num; i++)
        {
            as_new[i].start = as_old[(i/factor)*factor].start;

            if((i/factor)*factor+factor/2<thread_num)
                as_new[i].mid = as_old[(i/factor)*factor+factor/2].start;
            else
                as_new[i].mid = as_old[thread_num-1].end;

            if((i/factor)*factor+factor<thread_num)
                as_new[i].end = as_old[(i/factor)*factor+factor].start;
            else
                as_new[i].end = as_old[thread_num-1].end;

            int tfactor = std::min(thread_num, (i/factor+1)*factor)-(i/factor)*factor;
            as_new[i].tid = i % tfactor;
            as_new[i].factor = tfactor;
            as_new[i].startA = startA+(i/factor)*(factor+1);
            as_new[i].startB = startB+(i/factor)*(factor+1);
            if(as_new[i].tid == 0)
            {
                as_new[i].startA[0] = 0;
                as_new[i].startB[0] = 0;
            }
            if(flaga)
            {
                as_new[i].input = inputa + as_new[i].start;
                as_new[i].inputa = array + as_new[i].start;
                as_new[i].id = ida + as_new[i].start;
                as_new[i].ida = id + as_new[i].start;
            } else
            {
                as_new[i].input = array + as_new[i].start;
                as_new[i].inputa = inputa + as_new[i].start;
                as_new[i].id = id + as_new[i].start;
                as_new[i].ida = ida + as_new[i].start;
            }

            pthread_create(&threads[i], NULL, thread_findkth_kernel_key_l<T>, &as_new[i]);
        }

        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_create(&threads[i], NULL, thread_merge_kernel_key_l<T>, &as_new[i]);
        }
        for (uint32_t i = 0; i < thread_num; ++i)
        {
            pthread_join(threads[i], NULL);
        }

        if(flaga)
        {
            flaga=false;
        } else
        {
            flaga=true;
        }
        factor *= 2;
    }
    free(startA);
    free(startB);

    if(flaga)
    {
        util::copy_array(array, size, inputa, size);
        util::copy_array(id, size, ida, size);
    }

    delete[] inputa;
    delete[] ida;
    return;

}


}
