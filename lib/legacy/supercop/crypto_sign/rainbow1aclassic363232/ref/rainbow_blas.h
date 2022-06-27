/// @file rainbow_blas.h
/// @brief Defining the functions used in rainbow.c acconding to the definitions in rainbow_config.h
///
///  Defining the functions used in rainbow.c acconding to the definitions in rainbow_config.h

#ifndef _RAINBOW_BLAS_H_
#define _RAINBOW_BLAS_H_

#include "blas.h"

#include "parallel_matrix_op.h"

#include "rainbow_config.h"



#ifdef _USE_GF16

#define gfv_get_ele     gf16v_get_ele
#define gfv_mul_scalar  gf16v_mul_scalar
#define gfv_madd        gf16v_madd

#define gfmat_prod      gf16mat_prod

#define batch_trimat_madd    batch_trimat_madd_gf16
#define batch_trimatTr_madd  batch_trimatTr_madd_gf16
#define batch_2trimat_madd   batch_2trimat_madd_gf16
#define batch_matTr_madd     batch_matTr_madd_gf16
#define batch_bmatTr_madd    batch_bmatTr_madd_gf16
#define batch_mat_madd       batch_mat_madd_gf16

#define batch_quad_trimat_eval batch_quad_trimat_eval_gf16
#define batch_quad_recmat_eval batch_quad_recmat_eval_gf16

#else

#define gfv_get_ele     gf256v_get_ele
#define gfv_mul_scalar  gf256v_mul_scalar
#define gfv_madd        gf256v_madd

#define gfmat_prod      gf256mat_prod

#define batch_trimat_madd    batch_trimat_madd_gf256
#define batch_trimatTr_madd  batch_trimatTr_madd_gf256
#define batch_2trimat_madd   batch_2trimat_madd_gf256
#define batch_matTr_madd     batch_matTr_madd_gf256
#define batch_bmatTr_madd    batch_bmatTr_madd_gf256
#define batch_mat_madd       batch_mat_madd_gf256

#define batch_quad_trimat_eval batch_quad_trimat_eval_gf256
#define batch_quad_recmat_eval batch_quad_recmat_eval_gf256

#endif




#if defined( _RAINBOW16_36_32_32 )

#define gfmat_inv       gf16mat_inv_32x32
#define gfmat_solve_linear_eq       gf16mat_solve_linear_eq_32x32

#elif defined( _RAINBOW256_68_32_48 )

#define gfmat_inv       gf256mat_inv_32x32
#define gfmat_solve_linear_eq       gf256mat_solve_linear_eq_48x48

#elif defined( _RAINBOW256_96_36_64 )

#define gfmat_inv       gf256mat_inv_36x36
#define gfmat_solve_linear_eq       gf256mat_solve_linear_eq_64x64

#else
error here.
#endif


#endif  // _RAINBOW_BLAS_H_

