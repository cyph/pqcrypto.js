//// @file blas_matrix.c
/// @brief The standard implementations for blas_matrix.h
///

#include "blas_comm.h"
#include "blas_matrix.h"
#include "blas.h"



// choosing the implementations depends on the macros _BLAS_AVX2_ and _BLAS_SSE_


#if defined( _BLAS_AVX2_ )

#include "blas_matrix_avx2.h"
#define gf16mat_prod_impl             gf16mat_prod_avx2
#define gf256mat_prod_impl            gf256mat_prod_avx2

#define gf16mat_solve_linear_eq_32x32_impl  gf16mat_solve_linear_eq_32x32_avx2
#define gf16mat_inv_32x32_impl              gf16mat_inv_32x32_avx2

#define gf256mat_solve_linear_eq_48x48_impl gf256mat_solve_linear_eq_48x48_avx2
#define gf256mat_inv_32x32_impl             gf256mat_inv_32x32_avx2

#define gf256mat_solve_linear_eq_64x64_impl gf256mat_solve_linear_eq_64x64_avx2
#define gf256mat_inv_36x36_impl             gf256mat_inv_36x36_avx2

#elif defined( _BLAS_SSE_ )

#include "blas_matrix_sse.h"
#define gf16mat_prod_impl             gf16mat_prod_sse
#define gf256mat_prod_impl            gf256mat_prod_sse

#define gf16mat_solve_linear_eq_32x32_impl  gf16mat_solve_linear_eq_32x32_sse
#define gf16mat_inv_32x32_impl              gf16mat_inv_32x32_sse

#define gf256mat_solve_linear_eq_48x48_impl gf256mat_solve_linear_eq_48x48_sse
#define gf256mat_inv_32x32_impl             gf256mat_inv_32x32_sse

#define gf256mat_solve_linear_eq_64x64_impl gf256mat_solve_linear_eq_64x64_sse
#define gf256mat_inv_36x36_impl             gf256mat_inv_36x36_sse

#else

#include "blas_matrix_ref.h"

#define gf16mat_prod_impl             gf16mat_prod_ref
#define gf256mat_prod_impl            gf256mat_prod_ref

#define gf16mat_solve_linear_eq_32x32_impl  gf16mat_solve_linear_eq_32x32_ref
#define gf16mat_inv_32x32_impl              gf16mat_inv_32x32_ref

#define gf256mat_solve_linear_eq_48x48_impl gf256mat_solve_linear_eq_48x48_ref
#define gf256mat_inv_32x32_impl             gf256mat_inv_32x32_ref

#define gf256mat_solve_linear_eq_64x64_impl gf256mat_solve_linear_eq_64x64_ref
#define gf256mat_inv_36x36_impl             gf256mat_inv_36x36_ref

#endif



////////////////

void gf16mat_prod(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b)
{
    gf16mat_prod_impl( c, matA, n_A_vec_byte, n_A_width, b);
}


void gf256mat_prod(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b)
{
    gf256mat_prod_impl( c, matA, n_A_vec_byte, n_A_width, b);
}


////////////////


unsigned gf16mat_solve_linear_eq_32x32( uint8_t * sol , const uint8_t * inp_mat , const uint8_t * c_terms )
{
    return gf16mat_solve_linear_eq_32x32_impl( sol , inp_mat , c_terms );
}

unsigned gf16mat_inv_32x32( uint8_t * inv_a , const uint8_t * a )
{
    return gf16mat_inv_32x32_impl( inv_a , a );
}


////////////////


unsigned gf256mat_solve_linear_eq_48x48( uint8_t * sol , const uint8_t * inp_mat , const uint8_t * c_terms )
{
    return gf256mat_solve_linear_eq_48x48_impl( sol , inp_mat , c_terms );
}

unsigned gf256mat_inv_32x32( uint8_t * inv_a , const uint8_t * a )
{
    return gf256mat_inv_32x32_impl( inv_a , a );
}


////////////////


unsigned gf256mat_solve_linear_eq_64x64( uint8_t * sol , const uint8_t * inp_mat , const uint8_t * c_terms )
{
    return gf256mat_solve_linear_eq_64x64_impl( sol , inp_mat , c_terms );
}

unsigned gf256mat_inv_36x36( uint8_t * inv_a , const uint8_t * a )
{
    return gf256mat_inv_36x36_impl( inv_a , a );
}

