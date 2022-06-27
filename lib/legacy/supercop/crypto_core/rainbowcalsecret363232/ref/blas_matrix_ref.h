/// @file blas_matrix_ref.h
/// @brief linear algebra functions for matrix op.
///
#ifndef _BLAS_MATRIX_REF_H_
#define _BLAS_MATRIX_REF_H_

#include <stdint.h>



#ifdef  __cplusplus
extern  "C" {
#endif


///////////////// Section: multiplications  ////////////////////////////////

void gf16mat_prod_ref(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b);

void gf256mat_prod_ref(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b);


/////////////////////////////////////////////////////

unsigned gf16mat_solve_linear_eq_32x32_ref(uint8_t *sol, const uint8_t *inp_mat, const uint8_t *c_terms );

unsigned gf16mat_inv_32x32_ref(uint8_t *inv_a, const uint8_t *a );


///////////////////////////////////////////////////////


unsigned gf256mat_solve_linear_eq_48x48_ref(uint8_t *sol, const uint8_t *inp_mat, const uint8_t *c_terms );

unsigned gf256mat_inv_32x32_ref(uint8_t *inv_a, const uint8_t *a );


///////////////////////////////////////////////////////


unsigned gf256mat_solve_linear_eq_64x64_ref(uint8_t *sol, const uint8_t *inp_mat, const uint8_t *c_terms );

unsigned gf256mat_inv_36x36_ref(uint8_t *inv_a, const uint8_t *a );


#ifdef  __cplusplus
}
#endif

#endif  // _BLAS_MATRIX_REF_H_

