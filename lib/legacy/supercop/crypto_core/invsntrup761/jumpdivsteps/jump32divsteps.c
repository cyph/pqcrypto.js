#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "modq.h"

#include "polymul_8x8.h"

#include "jumpdivsteps.h"


int jump32divsteps (int delta, __m256i *fi, __m256i *gi, __m256i *uvqr){
  modq f0, g0;
  __m256i f0vec, g0vec, maskvec;
  __m256i  fl, fh, gl, gh, ul, uh, vl, vh, ql, qh, rl, rh;
  int i;
  __m256i minusdelta_v, mask, temp, gx, ux, uy, vx, vy, qx, rx;

  minusdelta_v = _mm256_set1_epi32(-delta);
  __m256i delta_v = _mm256_set1_epi16((int16_t)delta);
  fl = fi[0]; fh = fi[1];
  gl = gi[0]; gh = gi[1];

  gl = barrett_fake(gl); gh = barrett_fake(gh);
  fl = barrett_fake(fl); fh = barrett_fake(fh);

  ul = rl = v01;
  uh = vl = vh = ql = qh = rh = v0;

  g0vec = gl;
  gl = _mm256_permute2x128_si256(gl,gh,0x20);
  gh = _mm256_permute2x128_si256(g0vec,gh,0x31);
  f0vec = fl;
  fl = _mm256_permute2x128_si256(fl,fh,0x20);
  fh = _mm256_permute2x128_si256(f0vec,fh,0x31);

  for (i=32; i>0; i--) {
    gl = center_adjust( gl );

    temp = _mm256_andnot_si256(_mm256_cmpeq_epi16(gl,v0),_mm256_srai_epi16(minusdelta_v,15));
    maskvec = _mm256_broadcastw_epi16(_mm256_castsi256_si128(temp));
    temp = _mm256_sign_epi16(minusdelta_v,maskvec)^_mm256_andnot_si256(maskvec,minusdelta_v);
    minusdelta_v = _mm256_sub_epi16(temp,v01);  

    __m256i gx = _mm256_blendv_epi8(gl,fl,maskvec);
    fl = _mm256_blendv_epi8(fl,gl,maskvec);
    g0vec = _mm256_broadcastw_epi16(_mm256_castsi256_si128(gx));
    f0vec = _mm256_broadcastw_epi16(_mm256_castsi256_si128(fl));
    gl = _mm256_sub_epi16(montproduct(gx,f0vec),montproduct(fl,g0vec));
    gx = _mm256_blendv_epi8(gh,fh,maskvec);
    fh = _mm256_blendv_epi8(fh,gh,maskvec);
    gh = _mm256_sub_epi16(montproduct(gx,f0vec),montproduct(fh,g0vec));
    
    gx = _mm256_permute2x128_si256(gl,gl,0x81);
    gl = _mm256_alignr_epi8(gh,gl,2);
    gh = _mm256_alignr_epi8(gx,gh,2);
    
    rx  = _mm256_blendv_epi8(rl,vl,maskvec);
    vl  = _mm256_blendv_epi8(vl,rl,maskvec);
    rl  = _mm256_sub_epi16(montproduct(rx,f0vec),montproduct(vl,g0vec));
    rx  = _mm256_blendv_epi8(rh,vh,maskvec);
    vh  = _mm256_blendv_epi8(vh,rh,maskvec);
    rh  = _mm256_sub_epi16(montproduct(rx,f0vec),montproduct(vh,g0vec));
    
    vx = _mm256_permute2x128_si256(vh,vh,0x01);
    vh = _mm256_alignr_epi8(vh,vl,14);
    vl = _mm256_alignr_epi8(vl,vx,14);

    qx  = _mm256_blendv_epi8(ql,ul,maskvec);
    ul  = _mm256_blendv_epi8(ul,ql,maskvec);
    ql  = _mm256_sub_epi16(montproduct(qx,f0vec),montproduct(ul,g0vec));
    qx  = _mm256_blendv_epi8(qh,uh,maskvec);
    uh  = _mm256_blendv_epi8(uh,qh,maskvec);
    qh  = _mm256_sub_epi16(montproduct(qx,f0vec),montproduct(uh,g0vec));
    
    ux = _mm256_permute2x128_si256(uh,uh,0x01);
    uh = _mm256_alignr_epi8(uh,ul,14);
    ul = _mm256_alignr_epi8(ul,ux,14);
    
  }

  g0vec = gl;
  gl = _mm256_permute2x128_si256(gl,gh,0x20);
  gh = _mm256_permute2x128_si256(g0vec,gh,0x31);
  f0vec = fl;
  fl = _mm256_permute2x128_si256(fl,fh,0x20);
  fh = _mm256_permute2x128_si256(f0vec,fh,0x31);
  g0vec = ul;
  ul = _mm256_permute2x128_si256(ul,uh,0x20);
  uh = _mm256_permute2x128_si256(g0vec,uh,0x31);
  f0vec = vl;
  vl = _mm256_permute2x128_si256(vl,vh,0x20);
  vh = _mm256_permute2x128_si256(f0vec,vh,0x31);
  g0vec = ql;
  ql = _mm256_permute2x128_si256(ql,qh,0x20);
  qh = _mm256_permute2x128_si256(g0vec,qh,0x31);
  f0vec = rl;
  rl = _mm256_permute2x128_si256(rl,rh,0x20);
  rh = _mm256_permute2x128_si256(f0vec,rh,0x31);

  uvqr[0] = ul; uvqr[1] = uh; uvqr[2] = vl; uvqr[3] = vh;
  uvqr[4] = ql; uvqr[5] = qh; uvqr[6] = rl; uvqr[7] = rh;
  uvqr[8] = fl; uvqr[9] = fh; uvqr[10] = gl; uvqr[11] = gh;

  //for(i=0;i<12;i++) uvqr[i] = center_adjust(center_adjust(uvqr[i]));
  //return(- (modq) _mm_cvtsi128_si64(_mm256_castsi256_si128(minusdelta_v)));
  return(- (modq) _mm_extract_epi16(_mm256_castsi256_si128(minusdelta_v),0));
}

int jump32xdivsteps (int delta, __m256i *fi, __m256i *gi, __m256i *uvqr, int x){
  __m256i f0vec, g0vec, maskvec;
  __m256i  fl, fh, gl, gh, ul, uh, vl, vh, ql, qh, rl, rh;
  int i;
  __m256i minusdelta_v, mask, temp, gx, ux, uy, vx, vy, qx, rx;

  minusdelta_v = _mm256_set1_epi32(-delta);
  fl = fi[0]; fh = fi[1];
  gl = gi[0]; gh = gi[1];

  gl = barrett_fake(gl); gh = barrett_fake(gh);
  fl = barrett_fake(fl); fh = barrett_fake(fh);

  ul = rl = v01;
  uh = vl = vh = ql = qh = rh = v0;

  g0vec = gl;
  gl = _mm256_permute2x128_si256(gl,gh,0x20);
  gh = _mm256_permute2x128_si256(g0vec,gh,0x31);
  f0vec = fl;
  fl = _mm256_permute2x128_si256(fl,fh,0x20);
  fh = _mm256_permute2x128_si256(f0vec,fh,0x31);


  for (i=x; i>0; i--) {
    gl = center_adjust( gl );

    temp = _mm256_andnot_si256(_mm256_cmpeq_epi16(gl,v0),_mm256_srai_epi16(minusdelta_v,15));
    maskvec = _mm256_broadcastw_epi16(_mm256_castsi256_si128(temp));
    temp = _mm256_sign_epi16(minusdelta_v,maskvec)^_mm256_andnot_si256(maskvec,minusdelta_v);
    minusdelta_v = _mm256_sub_epi16(temp,v01);
      
    __m256i gx = _mm256_blendv_epi8(gl,fl,maskvec);
    fl = _mm256_blendv_epi8(fl,gl,maskvec);
    g0vec = _mm256_broadcastw_epi16(_mm256_castsi256_si128(gx));
    f0vec = _mm256_broadcastw_epi16(_mm256_castsi256_si128(fl));
    gl = _mm256_sub_epi16(montproduct(gx,f0vec),montproduct(fl,g0vec));
    gx = _mm256_blendv_epi8(gh,fh,maskvec);
    fh = _mm256_blendv_epi8(fh,gh,maskvec);
    gh = _mm256_sub_epi16(montproduct(gx,f0vec),montproduct(fh,g0vec));
    
    gx = _mm256_permute2x128_si256(gl,gl,0x81);
    gl = _mm256_alignr_epi8(gh,gl,2);
    gh = _mm256_alignr_epi8(gx,gh,2);
    
    rx  = _mm256_blendv_epi8(rl,vl,maskvec);
    vl  = _mm256_blendv_epi8(vl,rl,maskvec);
    rl  = _mm256_sub_epi16(montproduct(rx,f0vec),montproduct(vl,g0vec));
    rx  = _mm256_blendv_epi8(rh,vh,maskvec);
    vh  = _mm256_blendv_epi8(vh,rh,maskvec);
    rh  = _mm256_sub_epi16(montproduct(rx,f0vec),montproduct(vh,g0vec));
    
    vx = _mm256_permute2x128_si256(vh,vh,0x01);
    vh = _mm256_alignr_epi8(vh,vl,14);
    vl = _mm256_alignr_epi8(vl,vx,14);

    qx  = _mm256_blendv_epi8(ql,ul,maskvec);
    ul  = _mm256_blendv_epi8(ul,ql,maskvec);
    ql  = _mm256_sub_epi16(montproduct(qx,f0vec),montproduct(ul,g0vec));
    qx  = _mm256_blendv_epi8(qh,uh,maskvec);
    uh  = _mm256_blendv_epi8(uh,qh,maskvec);
    qh  = _mm256_sub_epi16(montproduct(qx,f0vec),montproduct(uh,g0vec));
    
    ux = _mm256_permute2x128_si256(uh,uh,0x01);
    uh = _mm256_alignr_epi8(uh,ul,14);
    ul = _mm256_alignr_epi8(ul,ux,14);
    
  }

  g0vec = gl;
  gl = _mm256_permute2x128_si256(gl,gh,0x20);
  gh = _mm256_permute2x128_si256(g0vec,gh,0x31);
  f0vec = fl;
  fl = _mm256_permute2x128_si256(fl,fh,0x20);
  fh = _mm256_permute2x128_si256(f0vec,fh,0x31);
  g0vec = ul;
  ul = _mm256_permute2x128_si256(ul,uh,0x20);
  uh = _mm256_permute2x128_si256(g0vec,uh,0x31);
  f0vec = vl;
  vl = _mm256_permute2x128_si256(vl,vh,0x20);
  vh = _mm256_permute2x128_si256(f0vec,vh,0x31);
  g0vec = ql;
  ql = _mm256_permute2x128_si256(ql,qh,0x20);
  qh = _mm256_permute2x128_si256(g0vec,qh,0x31);
  f0vec = rl;
  rl = _mm256_permute2x128_si256(rl,rh,0x20);
  rh = _mm256_permute2x128_si256(f0vec,rh,0x31);

  uvqr[0] = ul; uvqr[1] = uh; uvqr[2] = vl; uvqr[3] = vh;
  uvqr[4] = ql; uvqr[5] = qh; uvqr[6] = rl; uvqr[7] = rh;
  uvqr[8] = fl; uvqr[9] = fh; uvqr[10] = gl; uvqr[11] = gh;

//  for(i=0;i<12;i++) uvqr[i] = center_adjust(center_adjust(uvqr[i]));
//  return(- (modq) _mm_cvtsi128_si64(_mm256_castsi256_si128(minusdelta_v)));
  return(- (modq) _mm_extract_epi16(_mm256_castsi256_si128(minusdelta_v),0));
}
