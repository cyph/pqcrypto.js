//// @file blas_matrix.c
/// @brief The standard implementations for blas_matrix.h
///

#include "blas_comm.h"
#include "blas.h"

#include "blas_matrix_ref.h"

#include <stdint.h>
#include <string.h>

/// This implementation depends on these vector functions :
///   0.  gf16v_mul_scalar
///       gf16v_madd
///       gf256v_add
///       gf256v_mul_scalar
///       gf256v_madd
///
///   1.  gf256v_conditional_add     for _gf(16/256)mat_gauss_elim()
///   3.  gf(16/256)mat_gauss_elim  for _gf(16/256)mat_solve_linear_eq()
///  these functions have to be defined in blas.h




///////////  matrix-vector  multiplications  ////////////////////////////////

void gf16mat_prod_ref(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b) {
    gf256v_set_zero(c, n_A_vec_byte);
    for (unsigned i = 0; i < n_A_width; i++) {
        uint8_t bb = gf16v_get_ele(b, i);
        gf16v_madd(c, matA, bb, n_A_vec_byte);
        matA += n_A_vec_byte;
    }
}

void gf256mat_prod_ref(uint8_t *c, const uint8_t *matA, unsigned n_A_vec_byte, unsigned n_A_width, const uint8_t *b) {
    gf256v_set_zero(c, n_A_vec_byte);
    for (unsigned i = 0; i < n_A_width; i++) {
        gf256v_madd(c, matA, b[i], n_A_vec_byte);
        matA += n_A_vec_byte;
    }
}







/////////////////   algorithms:  gaussian elim  //////////////////
////////////  private functions  /////////////////////////////



static
unsigned gf16mat_gauss_elim_32x64_ref( uint8_t *mat ) {
    const unsigned h = 32;
    const unsigned w = 64;
    const unsigned w_byte = w/2;

    unsigned r8 = 1;
    for (unsigned i = 0; i < h; i++) {
        unsigned i_start = ((i>>1)&(~(_BLAS_UNIT_LEN_-1)));
        uint8_t *ai = mat + i*w_byte;
        for (unsigned j = i + 1; j < h; j++) {
            uint8_t *aj = mat + j*w_byte;
            gf256v_conditional_add(ai + i_start, !gf16_is_nonzero(gf16v_get_ele(ai, i)), aj + i_start, w_byte - i_start );
        }
        uint8_t pivot = gf16v_get_ele(ai, i);
        r8 &= gf16_is_nonzero(pivot);
        pivot = gf16_inv(pivot);
        gf16v_mul_scalar(ai + i_start, pivot, w_byte - i_start );
        for (unsigned j = 0; j < h; j++) {
            if (i == j) continue;
            uint8_t *aj = mat + j*w_byte;
            gf16v_madd(aj + i_start, ai + i_start, gf16v_get_ele(aj, i), w_byte-i_start);
        }
    }
    return r8;
}


static
unsigned gf16mat_gauss_elim_ref(uint8_t *mat, unsigned h, unsigned w) {
    const unsigned w_byte = (w+1)>>1;

    unsigned r8 = 1;
    for (unsigned i = 0; i < h; i++) {
        unsigned i_start = ((i>>1)&(~(_BLAS_UNIT_LEN_-1)));
        uint8_t *ai = mat + i*w_byte;
        for (unsigned j = i + 1; j < h; j++) {
            uint8_t *aj = mat + j*w_byte;
            gf256v_conditional_add(ai + i_start, !gf16_is_nonzero(gf16v_get_ele(ai, i)), aj + i_start, w_byte - i_start );
        }
        uint8_t pivot = gf16v_get_ele(ai, i);
        r8 &= gf16_is_nonzero(pivot);
        pivot = gf16_inv(pivot);
        gf16v_mul_scalar(ai + i_start, pivot, w_byte - i_start );
        for (unsigned j = 0; j < h; j++) {
            if (i == j) continue;
            uint8_t *aj = mat + j*w_byte;
            gf16v_madd(aj + i_start, ai + i_start, gf16v_get_ele(aj, i), w_byte-i_start);
        }
    }
    return r8;
}


/////////////////////////////////////////////////


static
unsigned gf256mat_gauss_elim_ref( uint8_t * mat , unsigned h , unsigned w )
{
    unsigned r8 = 1;

    for(unsigned i=0;i<h;i++) {
        uint8_t * ai = mat + w*i;
        unsigned i_start = i-(i&(_BLAS_UNIT_LEN_-1));

        for(unsigned j=i+1;j<h;j++) {
            uint8_t * aj = mat + w*j;
            gf256v_conditional_add( ai + i_start , !gf256_is_nonzero(ai[i]) , aj + i_start , w - i_start );
        }
        r8 &= gf256_is_nonzero(ai[i]);
        uint8_t pivot = ai[i];
        pivot = gf256_inv( pivot );
        gf256v_mul_scalar( ai + i_start  , pivot , w - i_start );
        for(unsigned j=0;j<h;j++) {
            if(i==j) continue;
            uint8_t * aj = mat + w*j;
            gf256v_madd( aj + i_start , ai+ i_start , aj[i] , w - i_start );
        }
    }

    return r8;
}



////////////  private functions  /////////////////////////////




unsigned gf16mat_solve_linear_eq_32x32_ref(uint8_t *sol, const uint8_t *inp_mat, const uint8_t *c_terms ) {
    const unsigned vec_len = 16+_BLAS_UNIT_LEN_;
    uint8_t mat[32*vec_len];
    const unsigned n=32;
    const unsigned n_2 = n/2;
    for(unsigned i=0;i<n;i++) {
        uint8_t *mi = mat+i*vec_len;
        for(unsigned j=0;j<n;j++) gf16v_set_ele( mi , j , gf16v_get_ele( inp_mat+j*16 , i ) );
        mi[n_2] = gf16v_get_ele(c_terms,i);
    }
    uint8_t r8 = gf16mat_gauss_elim_ref(mat,n,vec_len*2);
    for(unsigned i=0;i<n;i++) gf16v_set_ele( sol , i , mat[i*vec_len+n_2] );
    return r8;
}



static inline
void gf16mat_submat(uint8_t *mat2, unsigned w2, unsigned st, const uint8_t *mat, unsigned w, unsigned h) {
    unsigned n_byte_w1 = (w + 1) / 2;
    unsigned n_byte_w2 = (w2 + 1) / 2;
    unsigned st_2 = st / 2;
    for (unsigned i = 0; i < h; i++) {
        for (unsigned j = 0; j < n_byte_w2; j++) mat2[i * n_byte_w2 + j] = mat[i * n_byte_w1 + st_2 + j];
    }
}



unsigned gf16mat_inv_32x32_ref(uint8_t *inv_a, const uint8_t *a ) {
    const unsigned H=32;
    uint8_t mat[32*32];
    for (unsigned i = 0; i < H; i++) {
        uint8_t *ai = mat + i * 32;
        gf256v_set_zero(ai, 32 );
        gf256v_add(ai, a + i * 16, 16);
        gf16v_set_ele(ai + 16, i, 1);
    }
    uint8_t r8 = gf16mat_gauss_elim_32x64_ref(mat);
    gf16mat_submat(inv_a, H, H, mat, 2 * H, H);
    return r8;
}


/////////////////////////////////////////////////


static inline
void gf256mat_submat( uint8_t * mat2 , unsigned w2 , unsigned st , const uint8_t * mat , unsigned w , unsigned h )
{
    for(unsigned i=0;i<h;i++) {
        for(unsigned j=0;j<w2;j++) mat2[i*w2+j] = mat[i*w+st+j];
    }
}


unsigned gf256mat_solve_linear_eq_48x48_ref( uint8_t * sol , const uint8_t * inp_mat , const uint8_t * c_terms )
{
    const unsigned n = 48;
    const unsigned vec_len = n + _BLAS_UNIT_LEN_;

    uint8_t mat[ n*vec_len ];  // no need to clean to zero
    for(unsigned i=0;i<n;i++) {
        uint8_t * mi = mat + i*vec_len;
        for(unsigned j=0;j<n;j++) mi[j] = inp_mat[j*n+i];
        mi[n] = c_terms[i];
    }
    unsigned r8 = gf256mat_gauss_elim_ref( mat , n , vec_len );
    for(unsigned i=0;i<n;i++) sol[i] = mat[i*vec_len+n];
    gf256v_set_zero(mat,n*vec_len); // clean
    return r8;
}




unsigned gf256mat_inv_32x32_ref( uint8_t * inv_a , const uint8_t * a )
{
    const unsigned H=32;
    uint8_t mat[H*H*2];
    for(unsigned i=0;i<H;i++) {
        uint8_t * ai = mat + i*2*H;
        gf256v_set_zero( ai , 2*H );
        gf256v_add( ai , a + i*H , H );
        ai[H+i] = 1;
    }
    unsigned char r8 = gf256mat_gauss_elim_ref( mat , H , 2*H );
    gf256mat_submat( inv_a , H , H , mat , 2*H , H );
    gf256v_set_zero(mat,H*2*H);
    return r8;
}


//////////////////////////////////////////////////////


unsigned gf256mat_solve_linear_eq_64x64_ref( uint8_t * sol , const uint8_t * inp_mat , const uint8_t * c_terms )
{
    const unsigned n = 64;
    const unsigned vec_len = n + _BLAS_UNIT_LEN_;

    uint8_t mat[ n*vec_len ];  // no need to clean to zero
    for(unsigned i=0;i<n;i++) {
        uint8_t * mi = mat + i*vec_len;
        for(unsigned j=0;j<n;j++) mi[j] = inp_mat[j*n+i];
        mi[n] = c_terms[i];
    }
    unsigned r8 = gf256mat_gauss_elim_ref( mat , n , vec_len );
    for(unsigned i=0;i<n;i++) sol[i] = mat[i*vec_len+n];
    gf256v_set_zero(mat,n*vec_len); // clean
    return r8;
}




unsigned gf256mat_inv_36x36_ref( uint8_t * inv_a , const uint8_t * a )
{
    const unsigned H=36;
    uint8_t mat[H*H*2];
    for(unsigned i=0;i<H;i++) {
        uint8_t * ai = mat + i*2*H;
        gf256v_set_zero( ai , 2*H );
        gf256v_add( ai , a + i*H , H );
        ai[H+i] = 1;
    }
    unsigned char r8 = gf256mat_gauss_elim_ref( mat , H , 2*H );
    gf256mat_submat( inv_a , H , H , mat , 2*H , H );
    gf256v_set_zero(mat,H*2*H);
    return r8;
}





