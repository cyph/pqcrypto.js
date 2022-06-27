#ifndef _SORT_GF2N_H
#define _SORT_GF2N_H


#include "prefix_name.h"
#include "arch.h"
#include "matrix_gf2n.h"


void PREFIX_NAME(selectionSort_nocst_gf2n)(vec_gf2n tab, int l);
void PREFIX_NAME(selectionSort_gf2n)(vec_gf2n tab, int l);
#define selectionSort_nocst_gf2n PREFIX_NAME(selectionSort_nocst_gf2n)
#define selectionSort_gf2n PREFIX_NAME(selectionSort_gf2n)


/* The selection sort is the reference implementation. */
#define sort_nocst_gf2n selectionSort_nocst_gf2n

/* Constant-time */
#if CONSTANT_TIME
    #define sort_gf2n selectionSort_gf2n
#else
    #define sort_gf2n sort_nocst_gf2n
#endif


#endif

