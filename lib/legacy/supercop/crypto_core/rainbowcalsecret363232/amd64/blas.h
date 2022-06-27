/// @file blas.h
/// @brief Defining the implementations for linear algebra functions depending on the machine architecture.
///

#ifndef _BLAS_H_
#define _BLAS_H_


#include <stdint.h>

#include <stdio.h>




#include "blas_config.h"



#ifdef _BLAS_UINT64_

#define _BLAS_UNIT_LEN_ 8


#include "blas_u64.h"


#define gf16v_mul_scalar  _gf16v_mul_scalar_u64
#define gf16v_madd        _gf16v_madd_u64

#define gf256v_add        _gf256v_add_u64
#define gf256v_mul_scalar  _gf256v_mul_scalar_u64
#define gf256v_madd        _gf256v_madd_u64

#define gf256v_conditional_add      _gf256v_conditional_add_u64

#else
error here.
#endif


#include "blas_comm.h"
#include "blas_matrix.h"


#endif // _BLAS_H_

