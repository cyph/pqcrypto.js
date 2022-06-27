#ifndef _DETERMINANTNV_GF2_H
#define _DETERMINANTNV_GF2_H

#include "prefix_name.h"
#include "gf2.h"
#include "matrix_gf2.h"
#include "arch.h"


gf2 PREFIX_NAME(determinantnv_nocst_gf2)(cst_Mnv_gf2 S, Mnv_gf2 S_cp);
#define determinantnv_nocst_gf2 PREFIX_NAME(determinantnv_nocst_gf2)

gf2 PREFIX_NAME(determinantnv_cst_gf2)(cst_Mnv_gf2 S, Mnv_gf2 S_cp);
#define determinantnv_cst_gf2 PREFIX_NAME(determinantnv_cst_gf2)


#if CONSTANT_TIME
    #define determinantnv_gf2 determinantnv_cst_gf2
#else
    #define determinantnv_gf2 determinantnv_nocst_gf2
#endif


#endif

