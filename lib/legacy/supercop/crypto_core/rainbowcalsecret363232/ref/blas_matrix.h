/// @file blas_matrix.h
/// @brief linear algebra functions for matrix op.
///
#ifndef _BLAS_MATRIX_H_
#define _BLAS_MATRIX_H_

#include <stdint.h>



#ifdef  __cplusplus
extern  "C" {
#endif


///////////////// Section: multiplications  ////////////////////////////////


/// @brief matrix-vector multiplication:  c = matA * b , in GF(16)
///
/// @param[out]  c         - the output vector c
/// @param[in]   matA      - a column-major matrix A.
/// @param[in]   n_A_vec_byte  - the size of column vectors in bytes.
/// @param[in]   n_A_width   - the width of matrix A.
/// @param[in]   b          - the vector b.
///
void gf16mat_prod(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b);


/// @brief matrix-vector multiplication:  c = matA * b , in GF(256)
///
/// @param[out]  c         - the output vector c
/// @param[in]   matA      - a column-major matrix A.
/// @param[in]   n_A_vec_byte  - the size of column vectors in bytes.
/// @param[in]   n_A_width   - the width of matrix A.
/// @param[in]   b          - the vector b.
///
void gf256mat_prod(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b);






/////////////////////////////////////////////////////




/// @brief Solving linear equations, in GF(16)
///
/// @param[out]  sol       - the solutions.
/// @param[in]   inp_mat   - the matrix parts of input equations.
/// @param[in]   c_terms   - the constant terms of the input equations.
/// @return   1(true) if success. 0(false) if the matrix is singular.
///
unsigned gf16mat_solve_linear_eq_32x32(uint8_t *sol, const uint8_t *inp_mat, const uint8_t *c_terms );


/// @brief Solving linear equations, in GF(256)
///
/// @param[out]  sol       - the solutions.
/// @param[in]   inp_mat   - the matrix parts of input equations.
/// @param[in]   c_terms   - the constant terms of the input equations.
/// @return   1(true) if success. 0(false) if the matrix is singular.
///
unsigned gf256mat_solve_linear_eq_48x48(uint8_t *sol, const uint8_t *inp_mat, const uint8_t *c_terms );


/// @brief Solving linear equations, in GF(256)
///
/// @param[out]  sol       - the solutions.
/// @param[in]   inp_mat   - the matrix parts of input equations.
/// @param[in]   c_terms   - the constant terms of the input equations.
/// @return   1(true) if success. 0(false) if the matrix is singular.
///
unsigned gf256mat_solve_linear_eq_64x64(uint8_t *sol, const uint8_t *inp_mat, const uint8_t *c_terms );



///////////////////////////////////////////////////////


/// @brief Computing the inverse matrix, in GF(16)
///
/// @param[out]  inv_a     - the output of matrix a.
/// @param[in]   a         - a matrix a.
/// @return   1(true) if success. 0(false) if the matrix is singular.
///
unsigned gf16mat_inv_32x32(uint8_t *inv_a, const uint8_t *a );


/// @brief Computing the inverse matrix, in GF(256)
///
/// @param[out]  inv_a     - the output of matrix a.
/// @param[in]   a         - a matrix a.
/// @return   1(true) if success. 0(false) if the matrix is singular.
///
unsigned gf256mat_inv_32x32(uint8_t *inv_a, const uint8_t *a );


/// @brief Computing the inverse matrix, in GF(256)
///
/// @param[out]  inv_a     - the output of matrix a.
/// @param[in]   a         - a matrix a.
/// @return   1(true) if success. 0(false) if the matrix is singular.
///
unsigned gf256mat_inv_36x36(uint8_t *inv_a, const uint8_t *a );



#ifdef  __cplusplus
}
#endif

#endif  // _BLAS_MATRIX_H_

