#ifndef _SORT_GF2N_H
#define _SORT_GF2N_H


#include "prefix_name.h"
#include "arch.h"
#include "matrix_gf2n.h"


void PREFIX_NAME(quickSort_nocst_gf2n)(vec_gf2n tab_start, vec_gf2n tab_last);
void PREFIX_NAME(selectionSort_nocst_gf2n)(vec_gf2n tab, int l);
void PREFIX_NAME(fast_sort_gf2n)(vec_gf2n tab, int l);
void PREFIX_NAME(selectionSort_gf2n)(vec_gf2n tab, int l);
#define quickSort_nocst_gf2n PREFIX_NAME(quickSort_nocst_gf2n)
#define selectionSort_nocst_gf2n PREFIX_NAME(selectionSort_nocst_gf2n)
#define fast_sort_gf2n PREFIX_NAME(fast_sort_gf2n)
#define selectionSort_gf2n PREFIX_NAME(selectionSort_gf2n)


#ifdef MQSOFT_REF
    /* The selection sort is the reference implementation. */
    #define sort_nocst_gf2n selectionSort_nocst_gf2n

    /* Constant-time */
    #if CONSTANT_TIME
        #define sort_gf2n selectionSort_gf2n
    #else
        #define sort_gf2n sort_nocst_gf2n
    #endif
#else
    /* The fastest function in variable-time */
    #define sort_nocst_gf2n(tab,l) \
                quickSort_nocst_gf2n(tab,(tab)+((l)-1)*NB_WORD_GFqn)

    /* Constant-time */
    #if CONSTANT_TIME
        #define sort_gf2n fast_sort_gf2n
    #else
        #define sort_gf2n sort_nocst_gf2n
    #endif
#endif


#endif

