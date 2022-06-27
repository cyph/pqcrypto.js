/* Based on sntrup4591761/cavx/r3_recip.c. */

#include "crypto_core.h"
#include <immintrin.h>

#define C 700

typedef signed char small;

typedef __m256i vec256;

static inline void vec256_frombits(vec256 *v,const small *b)
{
  int i;

  for (i = 0;i < 3;++i) {
    vec256 b0 = _mm256_loadu_si256((vec256 *) b); b += 32; /* 0,1,...,31 */
    vec256 b1 = _mm256_loadu_si256((vec256 *) b); b += 32; /* 32,33,... */
    vec256 b2 = _mm256_loadu_si256((vec256 *) b); b += 32;
    vec256 b3 = _mm256_loadu_si256((vec256 *) b); b += 32;
    vec256 b4 = _mm256_loadu_si256((vec256 *) b); b += 32;
    vec256 b5 = _mm256_loadu_si256((vec256 *) b); b += 32;
    vec256 b6 = _mm256_loadu_si256((vec256 *) b); b += 32;
    vec256 b7 = _mm256_loadu_si256((vec256 *) b); b += 32;

    vec256 c0 = _mm256_unpacklo_epi32(b0,b1); /* 0 1 2 3 32 33 34 35 4 5 6 7 36 37 38 39 ... 55 */
    vec256 c1 = _mm256_unpackhi_epi32(b0,b1); /* 8 9 10 11 40 41 42 43 ... 63 */
    vec256 c2 = _mm256_unpacklo_epi32(b2,b3);
    vec256 c3 = _mm256_unpackhi_epi32(b2,b3);
    vec256 c4 = _mm256_unpacklo_epi32(b4,b5);
    vec256 c5 = _mm256_unpackhi_epi32(b4,b5);
    vec256 c6 = _mm256_unpacklo_epi32(b6,b7);
    vec256 c7 = _mm256_unpackhi_epi32(b6,b7);

    vec256 d0 = c0 | _mm256_slli_epi32(c1,2); /* 0 8, 1 9, 2 10, 3 11, 32 40, 33 41, ..., 55 63 */
    vec256 d2 = c2 | _mm256_slli_epi32(c3,2);
    vec256 d4 = c4 | _mm256_slli_epi32(c5,2);
    vec256 d6 = c6 | _mm256_slli_epi32(c7,2);

    vec256 e0 = _mm256_unpacklo_epi64(d0,d2);
    vec256 e2 = _mm256_unpackhi_epi64(d0,d2);
    vec256 e4 = _mm256_unpacklo_epi64(d4,d6);
    vec256 e6 = _mm256_unpackhi_epi64(d4,d6);

    vec256 f0 = e0 | _mm256_slli_epi32(e2,1);
    vec256 f4 = e4 | _mm256_slli_epi32(e6,1);

    vec256 g0 = _mm256_permute2x128_si256(f0,f4,0x20);
    vec256 g4 = _mm256_permute2x128_si256(f0,f4,0x31);

    vec256 h = g0 | _mm256_slli_epi32(g4,4);

#define TRANSPOSE _mm256_set_epi8( 31,27,23,19, 30,26,22,18, 29,25,21,17, 28,24,20,16, 15,11,7,3, 14,10,6,2, 13,9,5,1, 12,8,4,0 )
    h = _mm256_shuffle_epi8(h,TRANSPOSE);
    h = _mm256_permute4x64_epi64(h,0xd8);
    h = _mm256_shuffle_epi32(h,0xd8);

    *v++ = h;
  }
}

static inline void vec256_tobits(const vec256 *v,small *b)
{
  int i;

  for (i = 0;i < 3;++i) {
    vec256 h = *v++;

    h = _mm256_shuffle_epi32(h,0xd8);
    h = _mm256_permute4x64_epi64(h,0xd8);
    h = _mm256_shuffle_epi8(h,TRANSPOSE);

    vec256 g0 = h & _mm256_set1_epi8(15);
    vec256 g4 = _mm256_srli_epi32(h,4) & _mm256_set1_epi8(15);

    vec256 f0 = _mm256_permute2x128_si256(g0,g4,0x20);
    vec256 f4 = _mm256_permute2x128_si256(g0,g4,0x31);

    vec256 e0 = f0 & _mm256_set1_epi8(5);
    vec256 e2 = _mm256_srli_epi32(f0,1) & _mm256_set1_epi8(5);
    vec256 e4 = f4 & _mm256_set1_epi8(5);
    vec256 e6 = _mm256_srli_epi32(f4,1) & _mm256_set1_epi8(5);

    vec256 d0 = _mm256_unpacklo_epi32(e0,e2);
    vec256 d2 = _mm256_unpackhi_epi32(e0,e2);
    vec256 d4 = _mm256_unpacklo_epi32(e4,e6);
    vec256 d6 = _mm256_unpackhi_epi32(e4,e6);

    vec256 c0 = d0 & _mm256_set1_epi8(1);
    vec256 c1 = _mm256_srli_epi32(d0,2) & _mm256_set1_epi8(1);
    vec256 c2 = d2 & _mm256_set1_epi8(1);
    vec256 c3 = _mm256_srli_epi32(d2,2) & _mm256_set1_epi8(1);
    vec256 c4 = d4 & _mm256_set1_epi8(1);
    vec256 c5 = _mm256_srli_epi32(d4,2) & _mm256_set1_epi8(1);
    vec256 c6 = d6 & _mm256_set1_epi8(1);
    vec256 c7 = _mm256_srli_epi32(d6,2) & _mm256_set1_epi8(1);

    vec256 b0 = _mm256_unpacklo_epi64(c0,c1);
    vec256 b1 = _mm256_unpackhi_epi64(c0,c1);
    vec256 b2 = _mm256_unpacklo_epi64(c2,c3);
    vec256 b3 = _mm256_unpackhi_epi64(c2,c3);
    vec256 b4 = _mm256_unpacklo_epi64(c4,c5);
    vec256 b5 = _mm256_unpackhi_epi64(c4,c5);
    vec256 b6 = _mm256_unpacklo_epi64(c6,c7);
    vec256 b7 = _mm256_unpackhi_epi64(c6,c7);

    _mm256_storeu_si256((vec256 *) b,b0); b += 32;
    _mm256_storeu_si256((vec256 *) b,b1); b += 32;
    _mm256_storeu_si256((vec256 *) b,b2); b += 32;
    _mm256_storeu_si256((vec256 *) b,b3); b += 32;
    _mm256_storeu_si256((vec256 *) b,b4); b += 32;
    _mm256_storeu_si256((vec256 *) b,b5); b += 32;
    _mm256_storeu_si256((vec256 *) b,b6); b += 32;
    _mm256_storeu_si256((vec256 *) b,b7); b += 32;
  }
}

static inline void reverse(small *srev,const small *s)
{
  int i;
  for (i = 0;i < 768;++i)
    srev[i] = s[767-i];
}

static void vec256_init(vec256 *G0,vec256 *G1,const small *s)
{
  int i;
  small srev[768+(768-C)];
  small si;
  small g0[768];
  small g1[768];

  reverse(srev,s);
  for (i = C;i < 768;++i) srev[i+768-C] = 0;

  for (i = 0;i < 768;++i) {
    si = srev[i+768-C];
    g0[i] = si & 1;
    g1[i] = (si >> 1) & 1;
  }

  vec256_frombits(G0,g0);
  vec256_frombits(G1,g1);
}

static void vec256_final(small *out,const vec256 *V0,const vec256 *V1)
{
  int i;
  small v0[768];
  small v1[768];
  small v[768];
  small vrev[768+(768-C)];

  vec256_tobits(V0,v0);
  vec256_tobits(V1,v1);

  for (i = 0;i < 768;++i)
    v[i] = v0[i] + v1[i];

  reverse(vrev,v);
  for (i = 768;i < 768+(768-C);++i) vrev[i] = 0;

  for (i = 0;i < 768;++i) out[i] = vrev[i+768-C];
}

static inline int negative_mask(int x)
{
  return x >> 31;
}

static inline void vec256_swap(vec256 *f,vec256 *g,int len,vec256 mask)
{
  vec256 flip;
  int i;

  for (i = 0;i < len;++i) {
    flip = mask & (f[i] ^ g[i]);
    f[i] ^= flip;
    g[i] ^= flip;
  }
}

static inline void vec256_scale(vec256 *f0,vec256 *f1,const vec256 c0,const vec256 c1)
{
  int i;

  for (i = 0;i < 3;++i) {
    vec256 f0i = f0[i];
    vec256 f1i = f1[i];

    f0i &= c0;
    f1i ^= c1;
    f1i &= f0i;

    f0[i] = f0i;
    f1[i] = f1i;
  }
}

static inline void vec256_eliminate(vec256 *f0,vec256 *f1,vec256 *g0,vec256 *g1,int len,const vec256 c0,const vec256 c1)
{
  int i;

  for (i = 0;i < len;++i) {
    vec256 f0i = f0[i];
    vec256 f1i = f1[i];
    vec256 g0i = g0[i];
    vec256 g1i = g1[i];
    vec256 t;

    f0i &= c0;
    f1i ^= c1;
    f1i &= f0i;

    t = g0i ^ f0i;
    g0[i] = t | (g1i ^ f1i);
    g1[i] = (g1i ^ f0i) & (f1i ^ t);
  }
}

static inline int vec256_bit0mask(vec256 *f)
{
  return -(_mm_cvtsi128_si32(_mm256_castsi256_si128(f[0])) & 1);
}

static inline void vec256_divx_1(vec256 *f)
{
  unsigned long long f0 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[0]));

  f0 = f0 >> 1;

  f[0] = _mm256_blend_epi32(f[0],_mm256_set_epi64x(0,0,0,f0),0x3);

  f[0] = _mm256_permute4x64_epi64(f[0],0x39);
}

static inline void vec256_divx_2(vec256 *f)
{
  unsigned long long f0 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[0]));
  unsigned long long f1 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[1]));

  f0 = (f0 >> 1) | (f1 << 63);
  f1 = f1 >> 1;

  f[0] = _mm256_blend_epi32(f[0],_mm256_set_epi64x(0,0,0,f0),0x3);
  f[1] = _mm256_blend_epi32(f[1],_mm256_set_epi64x(0,0,0,f1),0x3);

  f[0] = _mm256_permute4x64_epi64(f[0],0x39);
  f[1] = _mm256_permute4x64_epi64(f[1],0x39);
}

static inline void vec256_divx_3(vec256 *f)
{
  unsigned long long f0,f1,f2;

  f0 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[0]));
  f1 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[1]));
  f2 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[2]));

  f0 = (f0 >> 1) | (f1 << 63);
  f1 = (f1 >> 1) | (f2 << 63);
  f2 = f2 >> 1;

  f[0] = _mm256_blend_epi32(f[0],_mm256_set_epi64x(0,0,0,f0),0x3);
  f[1] = _mm256_blend_epi32(f[1],_mm256_set_epi64x(0,0,0,f1),0x3);
  f[2] = _mm256_blend_epi32(f[2],_mm256_set_epi64x(0,0,0,f2),0x3);

  f[0] = _mm256_permute4x64_epi64(f[0],0x39);
  f[1] = _mm256_permute4x64_epi64(f[1],0x39);
  f[2] = _mm256_permute4x64_epi64(f[2],0x39);
}

static inline void vec256_timesx_1(vec256 *f)
{
  unsigned long long f0;

  f[0] = _mm256_permute4x64_epi64(f[0],0x93);

  f0 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[0]));

  f0 = f0 << 1;

  f[0] = _mm256_blend_epi32(f[0],_mm256_set_epi64x(0,0,0,f0),0x3);
}

static inline void vec256_timesx_2(vec256 *f)
{
  unsigned long long f0,f1;

  f[0] = _mm256_permute4x64_epi64(f[0],0x93);
  f[1] = _mm256_permute4x64_epi64(f[1],0x93);

  f0 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[0]));
  f1 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[1]));

  f1 = (f1 << 1) | (f0 >> 63);
  f0 = f0 << 1;

  f[0] = _mm256_blend_epi32(f[0],_mm256_set_epi64x(0,0,0,f0),0x3);
  f[1] = _mm256_blend_epi32(f[1],_mm256_set_epi64x(0,0,0,f1),0x3);
}

static inline void vec256_timesx_3(vec256 *f)
{
  unsigned long long f0,f1,f2;

  f[0] = _mm256_permute4x64_epi64(f[0],0x93);
  f[1] = _mm256_permute4x64_epi64(f[1],0x93);
  f[2] = _mm256_permute4x64_epi64(f[2],0x93);

  f0 = *(unsigned long long *) &f[0];
  f1 = *(unsigned long long *) &f[1];
  /*
  f2 = *(unsigned long long *) &f[2];
  f0 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[0]));
  f1 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[1]));
  */
  f2 = _mm_cvtsi128_si64(_mm256_castsi256_si128(f[2]));

  f2 = (f2 << 1) | (f1 >> 63);
  f1 = (f1 << 1) | (f0 >> 63);
  f0 = f0 << 1;

  *(unsigned long long *) &f[0] = f0;
  *(unsigned long long *) &f[1] = f1;
  /*
  *(unsigned long long *) &f[2] = f2;
  f[0] = _mm256_blend_epi32(f[0],_mm256_set_epi64x(0,0,0,f0),0x3);
  f[1] = _mm256_blend_epi32(f[1],_mm256_set_epi64x(0,0,0,f1),0x3);
  */
  f[2] = _mm256_blend_epi32(f[2],_mm256_set_epi64x(0,0,0,f2),0x3);
}

static int r3_recip(small *out,const small *s)
{
  vec256 F0[3];
  vec256 F1[3];
  vec256 G0[3];
  vec256 G1[3];
  vec256 V0[3];
  vec256 V1[3];
  vec256 R0[3];
  vec256 R1[3];
  vec256 c0vec,c1vec;
  int loop;
  int c0,c1;
  int minusdelta = -1;
  int swapmask;
  vec256 swapvec;

  vec256_init(G0,G1,s);

  F0[0] = F0[1] = _mm256_set_epi32(-1,-1,-1,-1,-1,-1,-1,-1);
  F0[2] = _mm256_set_epi32(32767,-1,32767,-1,32767,-1,65535,-1);
  F1[0] = F1[1] = F1[2] = _mm256_set1_epi32(0);

  V0[2] = V0[1] = V0[0] = _mm256_set1_epi32(0);
  V1[2] = V1[1] = V1[0] = _mm256_set1_epi32(0);

  R0[0] = _mm256_set_epi32(0,0,0,0,0,0,0,1);
  R0[2] = R0[1] = _mm256_set1_epi32(0);
  R1[2] = R1[1] = R1[0] = _mm256_set1_epi32(0);

  for (loop = 0;loop < 256;++loop) {
    vec256_timesx_1(V0);
    vec256_timesx_1(V1);
    swapmask = negative_mask(minusdelta) & vec256_bit0mask(G0);

    c0 = vec256_bit0mask(F0) & vec256_bit0mask(G0);
    c1 = vec256_bit0mask(F1) ^ vec256_bit0mask(G1);
    c1 &= c0;

    minusdelta ^= swapmask & (minusdelta ^ -minusdelta);
    minusdelta -= 1;

    swapvec = _mm256_set1_epi32(swapmask);
    vec256_swap(F0,G0,3,swapvec);
    vec256_swap(F1,G1,3,swapvec);

    c0vec = _mm256_set1_epi32(c0);
    c1vec = _mm256_set1_epi32(c1);

    vec256_eliminate(F0,F1,G0,G1,3,c0vec,c1vec);
    vec256_divx_3(G0);
    vec256_divx_3(G1);

    vec256_swap(V0,R0,1,swapvec);
    vec256_swap(V1,R1,1,swapvec);
    vec256_eliminate(V0,V1,R0,R1,1,c0vec,c1vec);
  }

  for (loop = 256;loop < 512;++loop) {
    vec256_timesx_2(V0);
    vec256_timesx_2(V1);
    swapmask = negative_mask(minusdelta) & vec256_bit0mask(G0);

    c0 = vec256_bit0mask(F0) & vec256_bit0mask(G0);
    c1 = vec256_bit0mask(F1) ^ vec256_bit0mask(G1);
    c1 &= c0;

    minusdelta ^= swapmask & (minusdelta ^ -minusdelta);
    minusdelta -= 1;

    swapvec = _mm256_set1_epi32(swapmask);
    vec256_swap(F0,G0,3,swapvec);
    vec256_swap(F1,G1,3,swapvec);

    c0vec = _mm256_set1_epi32(c0);
    c1vec = _mm256_set1_epi32(c1);

    vec256_eliminate(F0,F1,G0,G1,3,c0vec,c1vec);
    vec256_divx_3(G0);
    vec256_divx_3(G1);

    vec256_swap(V0,R0,2,swapvec);
    vec256_swap(V1,R1,2,swapvec);
    vec256_eliminate(V0,V1,R0,R1,2,c0vec,c1vec);
  }

  for (loop = 512;loop < C + (C-1-512);++loop) {
    vec256_timesx_3(V0);
    vec256_timesx_3(V1);
    swapmask = negative_mask(minusdelta) & vec256_bit0mask(G0);

    c0 = vec256_bit0mask(F0) & vec256_bit0mask(G0);
    c1 = vec256_bit0mask(F1) ^ vec256_bit0mask(G1);
    c1 &= c0;

    minusdelta ^= swapmask & (minusdelta ^ -minusdelta);
    minusdelta -= 1;

    swapvec = _mm256_set1_epi32(swapmask);
    vec256_swap(F0,G0,3,swapvec);
    vec256_swap(F1,G1,3,swapvec);

    c0vec = _mm256_set1_epi32(c0);
    c1vec = _mm256_set1_epi32(c1);

    vec256_eliminate(F0,F1,G0,G1,3,c0vec,c1vec);
    vec256_divx_3(G0);
    vec256_divx_3(G1);

    vec256_swap(V0,R0,3,swapvec);
    vec256_swap(V1,R1,3,swapvec);
    vec256_eliminate(V0,V1,R0,R1,3,c0vec,c1vec);
  }

  for (loop = 512;loop > 256;--loop) {
    vec256_timesx_3(V0);
    vec256_timesx_3(V1);
    swapmask = negative_mask(minusdelta) & vec256_bit0mask(G0);

    c0 = vec256_bit0mask(F0) & vec256_bit0mask(G0);
    c1 = vec256_bit0mask(F1) ^ vec256_bit0mask(G1);
    c1 &= c0;

    minusdelta ^= swapmask & (minusdelta ^ -minusdelta);
    minusdelta -= 1;

    swapvec = _mm256_set1_epi32(swapmask);
    vec256_swap(F0,G0,2,swapvec);
    vec256_swap(F1,G1,2,swapvec);

    c0vec = _mm256_set1_epi32(c0);
    c1vec = _mm256_set1_epi32(c1);

    vec256_eliminate(F0,F1,G0,G1,2,c0vec,c1vec);
    vec256_divx_2(G0);
    vec256_divx_2(G1);

    vec256_swap(V0,R0,3,swapvec);
    vec256_swap(V1,R1,3,swapvec);
    vec256_eliminate(V0,V1,R0,R1,3,c0vec,c1vec);
  }

  for (loop = 256;loop > 0;--loop) {
    vec256_timesx_3(V0);
    vec256_timesx_3(V1);
    swapmask = negative_mask(minusdelta) & vec256_bit0mask(G0);

    c0 = vec256_bit0mask(F0) & vec256_bit0mask(G0);
    c1 = vec256_bit0mask(F1) ^ vec256_bit0mask(G1);
    c1 &= c0;

    minusdelta ^= swapmask & (minusdelta ^ -minusdelta);
    minusdelta -= 1;

    swapvec = _mm256_set1_epi32(swapmask);
    vec256_swap(F0,G0,1,swapvec);
    vec256_swap(F1,G1,1,swapvec);

    c0vec = _mm256_set1_epi32(c0);
    c1vec = _mm256_set1_epi32(c1);

    vec256_eliminate(F0,F1,G0,G1,1,c0vec,c1vec);
    vec256_divx_1(G0);
    vec256_divx_1(G1);

    vec256_swap(V0,R0,3,swapvec);
    vec256_swap(V1,R1,3,swapvec);
    vec256_eliminate(V0,V1,R0,R1,3,c0vec,c1vec);
  }

  c0vec = _mm256_set1_epi32(vec256_bit0mask(F0));
  c1vec = _mm256_set1_epi32(vec256_bit0mask(F1));
  vec256_scale(V0,V1,c0vec,c1vec);

  vec256_final(out,V0,V1);
  return negative_mask(minusdelta);
}

int crypto_core(unsigned char *out,const unsigned char *in,const unsigned char *kunused,const unsigned char *cunused)
{
  small intop = in[2*700]&3;
  small input[768];
  small output[768];
  int i;

  /* XXX: obviously input/output format should be packed into bytes */

  intop = 3 - intop; /* 0 1 2 3 */
  intop &= (intop-3)>>5; /* 0 1 2 */
  intop += 1; /* 0 1 2 3, offset by 1 */

  for (i = 0;i < 700;++i) {
    small x = in[2*i]&3; /* 0 1 2 3 */
    x += intop; /* 0 1 2 3 4 5 6, offset by 1 */
    x = (x&3)+(x>>2); /* 0 1 2 3, offset by 1 */
    x &= (x-3)>>5; /* 0 1 2, offset by 1 */
    input[i] = x - 1;
  }
  /* XXX: merge with vec256_init */

  r3_recip(output,input);

  for (i = 0;i < 704;++i) {
    out[2*i] = output[i];
    out[2*i+1] = 0;
  }
  return 0;
}
