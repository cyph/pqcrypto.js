#ifndef _DETERMINANTNV_GF2_H
#define _DETERMINANTNV_GF2_H

#include "prefix_name.h"
#include "matrix_gf2.h"


int PREFIX_NAME(determinantnv_nocst_gf2)(cst_Mnv_gf2 S);
#define determinantnv_nocst_gf2 PREFIX_NAME(determinantnv_nocst_gf2)

int PREFIX_NAME(determinantnv_gf2)(cst_Mnv_gf2 S);
#define determinantnv_gf2 PREFIX_NAME(determinantnv_gf2)


#endif

