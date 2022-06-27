#include <immintrin.h>

#include "jumpdivsteps.h"
#include "polymul_8x8.h"
#include "polymul_NxN.h"

#include "params.h"
#include "rq.h"



#include "polymul_ntt.h"

#include "avx.h"


#define v4591_16 _mm256_set1_epi16(4591)
#define v15631_16 _mm256_set1_epi16(15631)
#define v15631_32 _mm256_set1_epi32(15631)
#define v1046748_32 _mm256_set1_epi32(1046748)
#define v01 _mm256_set_epi16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1)


#define ALIGNED __attribute__((aligned(32)))


/* return -1 if x!=0; else return 0 */
static int int16_nonzero_mask(int16 x)
{
  uint16 u = x; /* 0, else 1...65535 */
  uint32 v = u; /* 0, else 1...65535 */
  v = -v; /* 0, else 2^32-65535...2^32-1 */
  v >>= 31; /* 0, else 1 */
  return -v; /* 0, else -1 */
}


static inline void ymm_store( __m256i * c , __m256i * a , int n_ymm ) {
  for(int i=0;i<n_ymm;i++) _mm256_store_si256( c+i , *(a+i) );
}

static inline __m256i* _u_from_j256mat( __m256i* trans_mat ) { return trans_mat; }

static inline __m256i* _v_from_j256mat( __m256i* trans_mat ) { return trans_mat + 16; }

static inline __m256i* _q_from_j256mat( __m256i* trans_mat ) { return trans_mat + 32; }

static inline __m256i* _r_from_j256mat( __m256i* trans_mat ) { return trans_mat + 48; }

static inline __m256i* _f_from_j256mat( __m256i* trans_mat ) { return trans_mat + 64; }

static inline __m256i* _g_from_j256mat( __m256i* trans_mat ) { return trans_mat + 80; }



static void update256_fg_ntt512(modq *f, modq *g, int len, __m256i* M, __m256i *ntt_u , __m256i *ntt_v , __m256i *ntt_q , __m256i *ntt_r , __m256i u_hc , __m256i v_hc ) {
  int i;

  __m256i tmp0[32], tmp1[32], tmp2[32], tmp3[32];
  __m256i * nfi0 = &tmp0[0];
  __m256i * nfi1 = &tmp1[0];
  __m256i * ngi0 = &tmp2[0];
  __m256i * ngi1 = &tmp3[0];

  __m256i * cf = _f_from_j256mat(M);
  __m256i * cg = _g_from_j256mat(M);

  __m256i ntt_fi[3*32];
  __m256i ntt_gi[3*32];

  __m256i ntt_temp0[3*32];
  __m256i ntt_temp1[3*32];

  __m256i uv_hc = u_hc | v_hc;
  for (i=256; i<len; i+=256){
    __m256i * fi = (__m256i *) (f+i);
    __m256i * gi = (__m256i *) (g+i);
    polymul_ntt512_256_prepare( ntt_fi , fi );
    polymul_ntt512_256_prepare( ntt_gi , gi );

    polymul_ntt512_mul( ntt_temp0 , ntt_fi , ntt_u );
    polymul_ntt512_mul( ntt_temp1 , ntt_gi , ntt_v );
    polymul_ntt512_add( ntt_temp0 , ntt_temp0 , ntt_temp1 );
    polymul_ntt512_finale( nfi0 , ntt_temp0 );
    for(int j=0;j<16;j++) nfi0[j] ^= uv_hc&_mm256_blendv_epi8(gi[j],fi[j],u_hc);

    polymul_ntt512_mul( ntt_temp0 , ntt_fi , ntt_q );
    polymul_ntt512_mul( ntt_temp1 , ntt_gi , ntt_r );
    polymul_ntt512_add( ntt_temp0 , ntt_temp0 , ntt_temp1 );
    polymul_ntt512_finale( ngi0 , ntt_temp0 );

    for(int j=0;j<16;j++) nfi0[j] = center_adjust( _mm256_add_epi16( nfi0[j] , cf[j] ));
    for(int j=0;j<16;j++) ngi0[j] = center_adjust( _mm256_add_epi16( ngi0[j] , cg[j] ));

    cf = &nfi0[16];
    cg = &ngi0[16];

    ymm_store( (__m256i*)(f+i-256) , nfi0 , 16 );
    ymm_store( (__m256i*)(g+i-256) , ngi0 , 16 );

    __m256i *tmp;
    tmp = nfi0; nfi0 = nfi1; nfi1 = tmp;
    tmp = ngi0; ngi0 = ngi1; ngi1 = tmp;
  }
  ymm_store( (__m256i*)(f+i-256) , cf , 16 );
  ymm_store( (__m256i*)(g+i-256) , cg , 16 );
}



static void update256_fg_ntt768(modq *f, modq *g, int len, __m256i* M, __m256i *ntt_u , __m256i *ntt_v , __m256i *ntt_q , __m256i *ntt_r , __m256i u_hc , __m256i v_hc ) {
  int i;

  __m256i tmp0[48], tmp1[48], tmp2[48], tmp3[48];
  __m256i * nfi0 = &tmp0[0];
  __m256i * nfi1 = &tmp1[0];
  __m256i * ngi0 = &tmp2[0];
  __m256i * ngi1 = &tmp3[0];

  __m256i * cf = _f_from_j256mat(M);
  __m256i * cg = _g_from_j256mat(M);

  __m256i ntt_fi[3*48];
  __m256i ntt_gi[3*48];

  __m256i ntt_temp0[3*48];
  __m256i ntt_temp1[3*48];

  __m256i uv_hc = u_hc | v_hc;
  for (i=256; i<len; i+=512){
    __m256i * fi = (__m256i *) (f+i);
    __m256i * gi = (__m256i *) (g+i);
    polymul_ntt768_512_prepare( ntt_fi , fi );
    polymul_ntt768_512_prepare( ntt_gi , gi );

    polymul_ntt768_mul( ntt_temp0 , ntt_fi , ntt_u );
    polymul_ntt768_mul( ntt_temp1 , ntt_gi , ntt_v );
    polymul_ntt768_add( ntt_temp0 , ntt_temp0 , ntt_temp1 );
    polymul_ntt768_finale( nfi0 , ntt_temp0 );
    for(int j=0;j<32;j++) nfi0[j] ^= uv_hc&_mm256_blendv_epi8(gi[j],fi[j],u_hc);

    polymul_ntt768_mul( ntt_temp0 , ntt_fi , ntt_q );
    polymul_ntt768_mul( ntt_temp1 , ntt_gi , ntt_r );
    polymul_ntt768_add( ntt_temp0 , ntt_temp0 , ntt_temp1 );
    polymul_ntt768_finale( ngi0 , ntt_temp0 );

    for(int j=0;j<16;j++) nfi0[j] = center_adjust( _mm256_add_epi16( nfi0[j] , cf[j] ));
    for(int j=0;j<16;j++) ngi0[j] = center_adjust( _mm256_add_epi16( ngi0[j] , cg[j] ));

    cf = &nfi0[32];
    cg = &ngi0[32];

    ymm_store( (__m256i*)(f+i-256) , nfi0 , 32 );
    ymm_store( (__m256i*)(g+i-256) , ngi0 , 32 );

    __m256i *tmp;
    tmp = nfi0; nfi0 = nfi1; nfi1 = tmp;
    tmp = ngi0; ngi0 = ngi1; ngi1 = tmp;
  }
  ymm_store( (__m256i*)(f+i-256) , cf , 16 );
  ymm_store( (__m256i*)(g+i-256) , cg , 16 );
}






/*
out = (3s)^(-1) mod m, returning 0, if s is invertible mod m
or returning -1 if s is not invertible mod m
out,s are polys of degree <p
m is x^p-x-1
*/
int rq_recip3(modq *out,const small *s)
{
  int loop;
  int i;
  int delta = 1;

  ALIGNED modq f[1024];
  ALIGNED modq g[1536];

  for (i = 0;i < 769;++i) f[i] = 0;
  f[0] = 1;
  f[p-1] = -1;
  f[p] = -1;
  /* generalization: initialize f to reversal of any deg-p polynomial m */
  for (i = 0;i < p;++i) g[i] = 3 * s[p-1-i];
  for (i = p;i < 769;++i) g[i] = 0;

  __m256i v21[32],r21[32];
  __m256i M21_v_hc;

{
  __m256i M1[96],M2[96];
  // p = 761
  //__m256i M1[96];
  // loop = 0 -)256
  delta = jump256divsteps(delta, (__m256i*)f, (__m256i*)g, M1 );
  __m256i M1_u_hc = uv_sep( _u_from_j256mat(M1) );
  __m256i M1_v_hc = uv_sep( _v_from_j256mat(M1) );
  __m256i M1_u[32*3]; polymul_ntt512_256_prepare( M1_u, _u_from_j256mat(M1) );
  __m256i M1_v[32*3]; polymul_ntt512_256_prepare( M1_v, _v_from_j256mat(M1) );
  __m256i M1_q[32*3]; polymul_ntt512_256_prepare( M1_q, _q_from_j256mat(M1) );
  __m256i M1_r[32*3]; polymul_ntt512_256_prepare( M1_r, _r_from_j256mat(M1) );
  update256_fg_ntt512(f,g,p+1, M1, M1_u, M1_v, M1_q, M1_r , M1_u_hc , M1_v_hc );

  //__m256i M2[96];
  // loop = 256 -)512
  delta = jump256divsteps(delta, (__m256i*)f, (__m256i*)g, M2 );
  __m256i M2_u_hc = uv_sep( _u_from_j256mat(M2) );
  __m256i M2_v_hc = uv_sep( _v_from_j256mat(M2) );
  __m256i M2_u[32*3]; polymul_ntt512_256_prepare( M2_u, _u_from_j256mat(M2) );
  __m256i M2_v[32*3]; polymul_ntt512_256_prepare( M2_v, _v_from_j256mat(M2) );
  __m256i M2_q[32*3]; polymul_ntt512_256_prepare( M2_q, _q_from_j256mat(M2) );
  __m256i M2_r[32*3]; polymul_ntt512_256_prepare( M2_r, _r_from_j256mat(M2) );
  update256_fg_ntt512(f,g,p+1, M2, M2_u, M2_v, M2_q, M2_r , M2_u_hc , M2_v_hc );

/////////
// performing (M6*M5*M4)*(M3*M2*M1) for the upper right element with degree < 768.
/////////

// the right column of the M2*M1
  polymul_ntt512_mul( M1_u , M2_u , M1_v );
  polymul_ntt512_mul( M1_q , M2_v , M1_r );
  polymul_ntt512_add( M1_u , M1_u , M1_q );

  polymul_ntt512_mul( M2_q , M2_q , M1_v );
  polymul_ntt512_mul( M2_r , M2_r , M1_r );
  polymul_ntt512_add( M1_q , M2_q , M2_r );

  // mod 4591
  polymul_ntt512_finale( v21 , M1_u );
  polymul_ntt512_finale( r21 , M1_q );

  // if: M1_v_hc = 1
  for(int i=0;i<16;i++) r21[16+i] = center_adjust( _mm256_add_epi16( r21[16+i] , M1_v_hc& _q_from_j256mat(M2)[i] ) );

  M21_v_hc = M2_u_hc & M1_v_hc;
  __m256i M2_uv_hc = M2_u_hc | M2_v_hc;
  // if: M2_u_hc = 1 or M2_v_hc = 1   --> M2_u = 0 and M2_v = 0
  for(int i=0;i<16;i++) v21[i+16] ^= (M2_uv_hc & _mm256_blendv_epi8( _v_from_j256mat(M1)[i] , _r_from_j256mat(M1)[i] , M2_v_hc ) );

}
//////////////////////////////////

  __m256i v321[48],r321[48];
{
  __m256i M3[96];

  // loop = 512-)768 = 761 + 7
  //__m256i M3[96];
  delta = jump256divsteps(delta, (__m256i*)f, (__m256i*)g, M3 );
  __m256i M3_u_hc = uv_sep( _u_from_j256mat(M3) );
  __m256i M3_v_hc = uv_sep( _v_from_j256mat(M3) );
  __m256i M3_u[48*3]; polymul_ntt768_256_prepare( M3_u, _u_from_j256mat(M3) );
  __m256i M3_v[48*3]; polymul_ntt768_256_prepare( M3_v, _v_from_j256mat(M3) );
  __m256i M3_q[48*3]; polymul_ntt768_256_prepare( M3_q, _q_from_j256mat(M3) );
  __m256i M3_r[48*3]; polymul_ntt768_256_prepare( M3_r, _r_from_j256mat(M3) );
  update256_fg_ntt768(f,g,p+1, M3, M3_u, M3_v, M3_q, M3_r , M3_u_hc , M3_v_hc );

//////////////
// M3*(M2*M1)
//////////////

  __m256i v1[48*3], r1[48*3];
  polymul_ntt768_512_prepare( v1 , v21 );
  polymul_ntt768_512_prepare( r1 , r21 );

  polymul_ntt768_mul( M3_u , M3_u , v1 );
  polymul_ntt768_mul( M3_v , M3_v , r1 );
  polymul_ntt768_add( M3_u , M3_u , M3_v );

  polymul_ntt768_mul( M3_q , M3_q , v1 );
  polymul_ntt768_mul( M3_r , M3_r , r1 );
  polymul_ntt768_add( M3_v , M3_q , M3_r );

  polymul_ntt768_finale( v321 , M3_u );
  polymul_ntt768_finale( r321 , M3_v );

  // if: M21_v_hc = 1
  for(int i=0;i<16;i++) r321[32+i] = center_adjust( _mm256_add_epi16( r321[32+i] , M21_v_hc& _u_from_j256mat(M3)[i] ) );
  //__m256i M321_v_hc = M3_u_hc & M21_v_hc; // useless term.
  // if: M3_u_hc = 1 or M3_v_hc = 1  --> M3_u = 0 and M3_v = 0
  __m256i M3_uv_hc = M3_u_hc | M3_v_hc;
  for(int i=0;i<32;i++) v321[i+16] ^= ( M3_uv_hc & _mm256_blendv_epi8( v21[i] , r21[i] , M3_v_hc ) );
}

///////////////////////////////////


  __m256i u654[48],v654[48];
{
  __m256i M4[96];

  loop = p-1-7;  // 753
  //__m256i M4[96];
  delta = jump256divsteps(delta, (__m256i*)f, (__m256i*)g, M4 );
  __m256i M4_u_hc = uv_sep( _u_from_j256mat(M4) );
  __m256i M4_v_hc = uv_sep( _v_from_j256mat(M4) );
  __m256i M4_u[48*3]; polymul_ntt768_256_prepare( M4_u, _u_from_j256mat(M4) );
  __m256i M4_v[48*3]; polymul_ntt768_256_prepare( M4_v, _v_from_j256mat(M4) );
  __m256i M4_q[48*3]; polymul_ntt768_256_prepare( M4_q, _q_from_j256mat(M4) );
  __m256i M4_r[48*3]; polymul_ntt768_256_prepare( M4_r, _r_from_j256mat(M4) );
  update256_fg_ntt768(f,g,loop+1, M4, M4_u, M4_v, M4_q, M4_r , M4_u_hc , M4_v_hc );

////////////////////////////////////

  __m256i *_u3 = v21;
  __m256i *_v3 = r21;
  {
  __m256i M5[96], M6[96];

  loop -= 256;  // 497
  //__m256i M5[96];
  delta = jump256divsteps(delta, (__m256i*)f, (__m256i*)g, M5 );
  __m256i M5_u_hc = uv_sep( _u_from_j256mat(M5) );
  __m256i M5_v_hc = uv_sep( _v_from_j256mat(M5) );
  __m256i M5_u[32*3]; polymul_ntt512_256_prepare( M5_u, _u_from_j256mat(M5) );
  __m256i M5_v[32*3]; polymul_ntt512_256_prepare( M5_v, _v_from_j256mat(M5) );
  __m256i M5_q[32*3]; polymul_ntt512_256_prepare( M5_q, _q_from_j256mat(M5) );
  __m256i M5_r[32*3]; polymul_ntt512_256_prepare( M5_r, _r_from_j256mat(M5) );
  update256_fg_ntt512(f,g,loop+1, M5, M4_u, M4_v, M4_q, M4_r , M5_u_hc , M5_v_hc);

  loop -= 256;  // 241
  //__m256i M6[96];
  delta = jump256xdivsteps(delta, (__m256i*)f, (__m256i*)g, M6, loop );
  __m256i M6_u[32*3]; polymul_ntt512_256_prepare( M6_u, _u_from_j256mat(M6) );
  __m256i M6_v[32*3]; polymul_ntt512_256_prepare( M6_v, _v_from_j256mat(M6) );
  ymm_store( (__m256i*)f, &M6[64] , 1 );

///////////////
// M6*M5
///////////////

  polymul_ntt512_mul( M5_u , M6_u , M5_u );
  polymul_ntt512_mul( M5_q , M6_v , M5_q );
  polymul_ntt512_add( M5_u , M5_u , M5_q );

  polymul_ntt512_mul( M5_v , M6_u , M5_v );
  polymul_ntt512_mul( M5_r , M6_v , M5_r );
  polymul_ntt512_add( M5_q , M5_v , M5_r );

  // mod4591
  polymul_ntt512_finale( _u3 , M5_u );
  polymul_ntt512_finale( _v3 , M5_q );

  // if: M5_u_hc = 1
  for(int i=0;i<16;i++) _u3[i+16] = center_adjust( _mm256_add_epi16(_u3[i+16],M5_u_hc&_u_from_j256mat(M6)[i]) );
  // if: M5_v_hc = 1
  for(int i=0;i<16;i++) _v3[i+16] = center_adjust( _mm256_add_epi16(_v3[i+16],M5_v_hc&_u_from_j256mat(M6)[i]) );

  }

///////////////////
// (M6*M5)*M4
////////////////////

  __m256i u3[48*3], v3[48*3];
  polymul_ntt768_512_prepare( u3 , _u3 );
  polymul_ntt768_512_prepare( v3 , _v3 );

  polymul_ntt768_mul( M4_u , u3 , M4_u );
  polymul_ntt768_mul( M4_q , v3 , M4_q );
  polymul_ntt768_add( M4_u , M4_u , M4_q );

  polymul_ntt768_mul( M4_v , u3 , M4_v );
  polymul_ntt768_mul( M4_r , v3 , M4_r );
  polymul_ntt768_add( M4_q , M4_v , M4_r );

  polymul_ntt768_finale( u654 , M4_u );
  polymul_ntt768_finale( v654 , M4_q );

  // if: M4_u_hc = 1
  for(int i=0;i<32;i++) u654[i+16] = center_adjust( _mm256_add_epi16(u654[i+16],M4_u_hc&_u3[i]) );
  // if: M4_v_hc = 1
  for(int i=0;i<32;i++) v654[i+16] = center_adjust( _mm256_add_epi16(v654[i+16],M4_v_hc&_v3[i]) );

}
////////////////////////////////////////////////////


//
// performing (M6*M5*M4) *  (M3*M2*M1) for the upper right element with degree < 768.
//

  modq *v = g;

  // (M6*M5*M4)*(M3*M2*M1)
  mult768_over64_2( v, (const int16 *)u654, (const int16 *)v321, (const int16 *)v654, (const int16 *)r321);

  __m256i *z = (__m256i*)v;
  // z[i] = aX[i]//64 mod 4591
  for(int i=0;i<48;i++) _mm256_store_si256( z+i , montproduct( _mm256_load_si256(z+i) , _mm256_set1_epi16(2721) ) );


//
// output
//

  modq c;
  c = modq_reciprocal(modq_freeze(f[0]));
  for (i = 0;i < p;++i) out[i] = modq_product(modq_freeze(v[p-i]),c);
  //for (i = p;i < 768;++i) out[i] = 0;
  return int16_nonzero_mask(delta);
}

