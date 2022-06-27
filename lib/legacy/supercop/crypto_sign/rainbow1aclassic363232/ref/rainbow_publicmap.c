
#include "rainbow_keypair.h"
#include "rainbow_blas.h"


#define _MAX_N 256

#if _PUB_N > _MAX_N
error. _PUB_N > _MAX_N
#endif



#if 16 == _GFSIZE
#if _PUB_M_BYTE > 32
error. _PUB_M_BYTE > 32
#endif
#define TMPVEC_LEN 32
#else
#if _PUB_M_BYTE > 128
error. _PUB_M_BYTE > 128
#endif
#define TMPVEC_LEN 128
#endif



static
void accu_eval_quad( unsigned char * accu_res , const unsigned char * trimat , const unsigned char * x_in_byte , unsigned num_gfele_x , unsigned vec_len )
{
   const unsigned char *_x = x_in_byte;
   unsigned char _xixj[_MAX_N];
   unsigned n = num_gfele_x;

   for(unsigned i=0;i<n;i++) {
#if defined( _BLAS_AVX2_ )
      unsigned i_start = i-(i&31);
#elif defined( _BLAS_SSE_ )
      unsigned i_start = i-(i&15);
#elif defined( _BLAS_UINT64_ )
      unsigned i_start = i-(i&7);
#else
      unsigned i_start = i-(i&3);
#endif
      for(unsigned j=i;j<n;j++) _xixj[j]=_x[j];
      gfv_mul_scalar( _xixj+i_start , _x[i] , n-i_start );
      for(unsigned j=i;j<n;j++) {
         unsigned idx = _xixj[j];
         if(idx) gf256v_add( accu_res + TMPVEC_LEN*idx , trimat , vec_len );
         trimat += vec_len;
      }
   }
}



static
void accu_eval_quad_rect( unsigned char * accu_res , const unsigned char * y , unsigned num_y , const unsigned char * mat , const unsigned char * x , unsigned num_x , unsigned vec_len )
{
   unsigned char _xy[_MAX_N];

   for(unsigned i=0;i<num_y;i++) {
      for(unsigned j=0;j<num_x;j++) _xy[j]=x[j];
      gfv_mul_scalar( _xy , y[i] , num_x );
      for(unsigned j=0;j<num_x;j++) {
         unsigned idx = _xy[j];
         if(idx) gf256v_add( accu_res + TMPVEC_LEN*idx , mat , vec_len );
         mat += vec_len;
      }
   }
}



static
void madd_reduce( unsigned char * y , const unsigned char * tmp_res , unsigned vec_len )
{
    unsigned char tmp[TMPVEC_LEN];
    int accu_bit = 1;

    gf256v_set_zero( y , vec_len );
    // x1
    for(int i=1;i<_GFSIZE;i+=2) gf256v_add( y , tmp_res+TMPVEC_LEN*i , vec_len );
    // x2
    accu_bit = 1<<1; // 2
    gf256v_set_zero( tmp , vec_len );
    for(int i=accu_bit;i<_GFSIZE;i+=accu_bit*2) {
      for(int j=0;j<accu_bit;j++) gf256v_add( tmp , tmp_res+TMPVEC_LEN*(i+j) , vec_len );
    }
    gf16v_madd( y , tmp , accu_bit ,  vec_len );

    // x4
    accu_bit = 1<<2; // 4
    gf256v_set_zero( tmp , vec_len );
    for(int i=accu_bit;i<_GFSIZE;i+=accu_bit*2) {
      for(int j=0;j<accu_bit;j++) gf256v_add( tmp , tmp_res+TMPVEC_LEN*(i+j) , vec_len );
    }
    gf16v_madd( y , tmp , accu_bit ,  vec_len );

    // x8
    accu_bit = 1<<3; // 8
    gf256v_set_zero( tmp , vec_len );
    for(int i=accu_bit;i<_GFSIZE;i+=accu_bit*2) {
      for(int j=0;j<accu_bit;j++) gf256v_add( tmp , tmp_res+TMPVEC_LEN*(i+j) , vec_len );
    }
    gf16v_madd( y , tmp , accu_bit ,  vec_len );

#if 256 == _GFSIZE

    accu_bit = 1<<4; // 16
    gf256v_set_zero( tmp , vec_len );
    for(int i=accu_bit;i<_GFSIZE;i+=accu_bit*2) {
      for(int j=0;j<accu_bit;j++) gf256v_add( tmp , tmp_res+TMPVEC_LEN*(i+j) , vec_len );
    }
    gf256v_madd( y , tmp , accu_bit ,  vec_len );

    accu_bit = 1<<5; // 32
    gf256v_set_zero( tmp , vec_len );
    for(int i=accu_bit;i<_GFSIZE;i+=accu_bit*2) {
      for(int j=0;j<accu_bit;j++) gf256v_add( tmp , tmp_res+TMPVEC_LEN*(i+j) , vec_len );
    }
    gf256v_madd( y , tmp , accu_bit ,  vec_len );

    accu_bit = 1<<6; // 64
    gf256v_set_zero( tmp , vec_len );
    for(int i=accu_bit;i<_GFSIZE;i+=accu_bit*2) {
      for(int j=0;j<accu_bit;j++) gf256v_add( tmp , tmp_res+TMPVEC_LEN*(i+j) , vec_len );
    }
    gf256v_madd( y , tmp , accu_bit ,  vec_len );

    accu_bit = 1<<7; // 128
    gf256v_set_zero( tmp , vec_len );
    for(int i=accu_bit;i<_GFSIZE;i+=accu_bit*2) {
      for(int j=0;j<accu_bit;j++) gf256v_add( tmp , tmp_res+TMPVEC_LEN*(i+j) , vec_len );
    }
    gf256v_madd( y , tmp , accu_bit ,  vec_len );

#endif
}






void rainbow_publicmap( unsigned char * y, const unsigned char * trimat, const unsigned char * x )
{
    unsigned char tmp[TMPVEC_LEN*_GFSIZE] = {0};
    unsigned char _x[_MAX_N];
    for(unsigned i=0;i<_PUB_N;i++) _x[i] = gfv_get_ele( x , i );

    accu_eval_quad( tmp , trimat , _x , _PUB_N , _PUB_M_BYTE );
    madd_reduce( y , tmp , _PUB_M_BYTE );
}




#include "utils_prng.h"



void rainbow_publicmap_cpk( unsigned char * z, const cpk_t * pk, const unsigned char *w )
{
    prng_t prng0;
    prng_set( &prng0 , pk->pk_seed , LEN_PKSEED );

    // assuming:
    // 1) _O2_BYTE*(_V1*_O2) is the largest size among l1_O1, l1_Q2, ..... l2_Q1, .... l2_Q9.
    // 2) 128 >= _O1_BYTE + _O2_BYTE

#define _BUF_SIZE_1 (((_V1+1)>_O2*2)? _O2_BYTE*(N_TRIANGLE_TERMS(_V1)): _O2_BYTE*_V1*_O2)
#if ( _O2<_O1)||(128<_O1_BYTE+_O2_BYTE)
error: buffer size.
#endif
    unsigned char buffer[_BUF_SIZE_1 ];
    sk_t * _sk;

    unsigned char tmp[TMPVEC_LEN*_GFSIZE] = {0};
    unsigned char _x[_MAX_N];
    unsigned char *_v1 = _x;
    unsigned char *_o1 = _v1 + _V1;
    unsigned char *_o2 = _o1 + _O1;
    for(unsigned i=0;i<_PUB_N;i++) _x[i] = gfv_get_ele( w , i );

    prng_gen( &prng0 , buffer , sizeof(_sk->l1_F1) ); // l1_F1
    accu_eval_quad( tmp , buffer , _v1 , _V1 , _O1_BYTE );

    prng_gen( &prng0 , buffer ,  sizeof(_sk->l1_F2) );  // l1_F2
    accu_eval_quad_rect( tmp , _v1 , _V1 , buffer , _o1 , _O1 , _O1_BYTE );

    accu_eval_quad_rect( tmp , _v1 , _V1 , pk->l1_Q3 , _o2 , _O2 , _O1_BYTE );

    accu_eval_quad( tmp , pk->l1_Q5 , _o1 , _O1 , _O1_BYTE );

    accu_eval_quad_rect( tmp , _o1 , _O1 , pk->l1_Q6 , _o2 , _O2 , _O1_BYTE );

    accu_eval_quad( tmp , pk->l1_Q9 , _o2 , _O2 , _O1_BYTE );

    // l2
    unsigned char tmp2[TMPVEC_LEN*_GFSIZE] = {0};

    prng_gen( &prng0 , buffer ,  sizeof(_sk->l2_F1) ); // l2_F1
    accu_eval_quad( tmp2 , buffer , _v1 , _V1 , _O2_BYTE );

    prng_gen( &prng0 , buffer ,  sizeof(_sk->l2_F2) ); // l2_F2
    accu_eval_quad_rect( tmp2 , _v1 , _V1 , buffer , _o1 , _O1 , _O2_BYTE );

    prng_gen( &prng0 , buffer ,  sizeof(_sk->l2_F3) ); // l2_F3
    accu_eval_quad_rect( tmp2 , _v1 , _V1 , buffer , _o2 , _O2 , _O2_BYTE );

    prng_gen( &prng0 , buffer ,  sizeof(_sk->l2_F5) ); // l2_F5
    accu_eval_quad( tmp2 , buffer , _o1 , _O1 , _O2_BYTE );

    prng_gen( &prng0 , buffer ,  sizeof(_sk->l2_F6) ); // l2_F6
    accu_eval_quad_rect( tmp2 , _o1 , _O1 , buffer , _o2 , _O2 , _O2_BYTE );

    accu_eval_quad( tmp2 , pk->l2_Q9 , _o2 , _O2 , _O2_BYTE );

    for(int i=0;i<_GFSIZE;i++) gf256v_add( tmp+i*TMPVEC_LEN+_O1_BYTE , tmp2+i*TMPVEC_LEN , _O2_BYTE );
    madd_reduce( z , tmp , _PUB_M_BYTE );
}





