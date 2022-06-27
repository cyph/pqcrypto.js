
#define NDEBUG
#include <assert.h>
#include <inttypes.h>
#include "avx.h"

#define ALIGNED __attribute((aligned(32)))

#define q 4591
#define qinv 15631 /* reciprocal of q mod 2^16 */
#define q15 7 /* round(2^15/q) */

static inline int16 add(int16 x,int16 y)
{
  return x+y;
}

static inline int16 sub(int16 x,int16 y)
{
  return x-y;
}

static inline int16 mullo(int16 x,int16 y)
{
  return x*y;
}

static inline int16 mulhi(int16 x,int16 y)
{
  return (x*(int32)y)>>16;
}

static inline int16 mulhrs(int16 x,int16 y)
{
  return (x*(int32)y+16384)>>15;
}

#define sub_x8 _mm_sub_epi16
#define const_x8 _mm_set1_epi16
#define mullo_x8 _mm_mullo_epi16
#define mulhrs_x8 _mm_mulhrs_epi16
#define load_x8(p) _mm_loadu_si128((int16x8 *) (p))
#define store_x8(p,v) _mm_storeu_si128((int16x8 *) (p),(v))

#define const_x16 _mm256_set1_epi16
#define add_x16 _mm256_add_epi16
#define sub_x16 _mm256_sub_epi16
#define mullo_x16 _mm256_mullo_epi16
#define mulhi_x16 _mm256_mulhi_epi16
#define mulhrs_x16 _mm256_mulhrs_epi16
#define load_x16(p) _mm256_loadu_si256((int16x16 *) (p))
#define store_x16(p,v) _mm256_storeu_si256((int16x16 *) (p),(v))

static inline int16x16 neg_x16(int16x16 f)
{
  return sub_x16(const_x16(0),f);
}

/* input range: -2^15 <= x < 2^15 */
/* output range: -4000 < out < 4000 */
static inline int16x8 squeeze13_x8(int16x8 f)
{
  /* XXX: for q=5167, need to do mulhi+mulhrs+mullo to achieve this range */
  return sub_x8(f,mullo_x8(mulhrs_x8(f,const_x8(q15)),const_x8(q)));
}

static inline int16x8 negsqueeze13_x8(int16x8 f)
{
  return sub_x8(mullo_x8(mulhrs_x8(f,const_x8(q15)),const_x8(q)),f);
}

static inline int16x16 squeeze13_x16(int16x16 f)
{
  return sub_x16(f,mullo_x16(mulhrs_x16(f,const_x16(q15)),const_x16(q)));
}

static inline int16x16 negsqueeze13_x16(int16x16 f)
{
  return sub_x16(mullo_x16(mulhrs_x16(f,const_x16(q15)),const_x16(q)),f);
}

/* input range: -2^15 <= x < 2^15 */
/* output range: -8000 < out < 8000 */
static inline int16x16 squeeze14_x16(int16x16 f)
{
  f = sub_x16(f,mullo_x16(mulhrs_x16(f,const_x16(q15)),const_x16(q)));
#ifndef NDEBUG
  f = add_x16(f,const_x16(q));
#endif
  return f;
}

static inline void assertrange_x16(int16x16 f,int limit)
{
  int p;
  for (p = 0;p < 16;++p) {
    assert(p[(int16 *) &f] >= -limit);
    assert(p[(int16 *) &f] <= limit);
  }
}

/* h = fg/65536 in (k^16)[x] */
/* where f,g are 1-coeff polys */
/* input range: +-16000 */
/* output range: +-8000 */
inline void mult1_over65536_x16(int16x16 h[1],const int16x16 f[1],const int16x16 g[1])
{
  /* 4 mul + 1 add */

  int16x16 gqinv,b,d,e;

  assertrange_x16(f[0],16000);
  assertrange_x16(g[0],16000);

  gqinv = mullo_x16(g[0],const_x16(qinv));
  b = mulhi_x16(f[0],g[0]);
  d = mullo_x16(f[0],gqinv);
  e = mulhi_x16(d,const_x16(q));
  h[0] = sub_x16(b,e);

  assertrange_x16(h[0],8000);
}

/* h = fg/65536 in (k^16)[x] */
/* where f,g are 2-coeff polys */
/* input range: +-8000 */
/* output range: +-8000 */
inline void mult2_over65536_x16(int16x16 h[3],const int16x16 f[2],const int16x16 g[2])
{
  /* strategy: refined Karatsuba */
  /* 13 mul + 9 add */

  int16x16 f0,f1,g0,g1,fmid,gmid,g0qinv,g1qinv,gmidqinv,b,d,h0,h1,h2;

  int i;
  for (i = 0;i < 2;++i) {
    assertrange_x16(f[i],8000);
    assertrange_x16(g[i],8000);
  }

  g0 = g[0];
  g1 = g[1];
  f0 = f[0];
  f1 = f[1];

  gmid = add_x16(g0,g1);
  fmid = add_x16(f0,f1);

  g0qinv = mullo_x16(g0,const_x16(qinv));
  b = mulhi_x16(f0,g0);
  d = mullo_x16(f0,g0qinv);
  h0 = mulhi_x16(d,const_x16(q));
  h0 = sub_x16(b,h0);

  g1qinv = mullo_x16(g1,const_x16(qinv));
  b = mulhi_x16(f1,g1);
  d = mullo_x16(f1,g1qinv);
  h2 = mulhi_x16(d,const_x16(q));
  h2 = sub_x16(b,h2);

  gmidqinv = add_x16(g0qinv,g1qinv); /* XXX: imitate this at higher levels */
  b = mulhi_x16(fmid,gmid);
  d = mullo_x16(fmid,gmidqinv);
  h1 = mulhi_x16(d,const_x16(q));
  h1 = sub_x16(b,h1);

  h[1] = squeeze14_x16(sub_x16(h1,add_x16(h0,h2)));
  h[0] = h0;
  h[2] = h2;

  for (i = 0;i < 3;++i)
    assertrange_x16(h[i],8000);
}

/* h = fg/65536 in (k^16)[x] */
/* where f,g are 4-coeff polys */
/* input range: +-4000 */
/* output range: +-8000 */
inline void mult4_over65536_x16(int16x16 h[7],const int16x16 f[4],const int16x16 g[4])
{
  /* strategy: refined Karatsuba */
  /* 48 mul + 38 add */

  int16x16 fmid[2];
  int16x16 gmid[2];
  int16x16 hmid[3];
  int16x16 c;

  int i;
  for (i = 0;i < 4;++i) {
    assertrange_x16(f[i],4000);
    assertrange_x16(g[i],4000);
  }

  fmid[0] = add_x16(f[0],f[2]);
  fmid[1] = add_x16(f[1],f[3]);
  gmid[0] = add_x16(g[0],g[2]);
  gmid[1] = add_x16(g[1],g[3]);

  mult2_over65536_x16(h,f,g);
  mult2_over65536_x16(h+4,f+2,g+2);
  mult2_over65536_x16(hmid,fmid,gmid);

  c = sub_x16(h[2],h[4]);
  h[2] = squeeze14_x16(add_x16(sub_x16(hmid[0],h[0]),c));
  h[3] = squeeze14_x16(sub_x16(hmid[1],add_x16(h[1],h[5])));
  h[4] = squeeze14_x16(sub_x16(sub_x16(hmid[2],h[6]),c));

  for (i = 0;i < 7;++i)
    assertrange_x16(h[i],8000);
}

/* h = fg/65536 in (k^16)[x]/(x^8+1) */
/* input range: +-4000 */
/* output range: +-4000 */
/* h can overlap inputs */
void mult8_nega_over65536_x16(int16x16 h[8],const int16x16 f[8],const int16x16 g[8])
{
  /* strategy: reduced refined Karatsuba */
  /* 176 mul + 159 add */

  int16x16 fmid[4],gmid[4],hbot[7],hmid[7],htop[7],c0,c1,c2,c3,d0,d1,d2,d3;

  int i;
  for (i = 0;i < 8;++i) {
    assertrange_x16(f[i],4000);
    assertrange_x16(g[i],4000);
  }

  fmid[0] = squeeze13_x16(add_x16(f[0],f[4]));
  fmid[1] = squeeze13_x16(add_x16(f[1],f[5]));
  fmid[2] = squeeze13_x16(add_x16(f[2],f[6]));
  fmid[3] = squeeze13_x16(add_x16(f[3],f[7]));
  gmid[0] = squeeze13_x16(add_x16(g[0],g[4]));
  gmid[1] = squeeze13_x16(add_x16(g[1],g[5]));
  gmid[2] = squeeze13_x16(add_x16(g[2],g[6]));
  gmid[3] = squeeze13_x16(add_x16(g[3],g[7]));

  mult4_over65536_x16(hbot,f,g);
  mult4_over65536_x16(htop,f+4,g+4);
  mult4_over65536_x16(hmid,fmid,gmid);

  c3 = sub_x16(hbot[3],htop[3]);
  d3 = add_x16(hbot[3],htop[3]);
  h[3] = squeeze13_x16(c3);
  h[7] = squeeze13_x16(sub_x16(hmid[3],d3));

  c0 = sub_x16(hbot[4],htop[0]);
  d0 = add_x16(hbot[0],htop[4]);
  h[0] = squeeze13_x16(add_x16(sub_x16(c0,hmid[4]),d0));
  h[4] = squeeze13_x16(sub_x16(add_x16(c0,hmid[0]),d0));

  c1 = sub_x16(hbot[5],htop[1]);
  d1 = add_x16(hbot[1],htop[5]);
  h[1] = squeeze13_x16(add_x16(sub_x16(c1,hmid[5]),d1));
  h[5] = squeeze13_x16(sub_x16(add_x16(c1,hmid[1]),d1));

  c2 = sub_x16(hbot[6],htop[2]);
  d2 = add_x16(hbot[2],htop[6]);
  h[2] = squeeze13_x16(add_x16(sub_x16(c2,hmid[6]),d2));
  h[6] = squeeze13_x16(sub_x16(add_x16(c2,hmid[2]),d2));

  for (i = 0;i < 8;++i)
    assertrange_x16(h[i],4000);
}

/* multiply f by x in (k^16)[x]/(x^8+1) */
static void twist8_1(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = neg_x16(f7);
  f[1] = f0;
  f[2] = f1;
  f[3] = f2;
  f[4] = f3;
  f[5] = f4;
  f[6] = f5;
  f[7] = f6;
}

/* multiply f by x^2 in (k^16)[x]/(x^8+1) */
static void twist8_2(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = neg_x16(f6);
  f[1] = neg_x16(f7);
  f[2] = f0;
  f[3] = f1;
  f[4] = f2;
  f[5] = f3;
  f[6] = f4;
  f[7] = f5;
}

/* multiply f by x^3 in (k^16)[x]/(x^8+1) */
static void twist8_3(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = neg_x16(f5);
  f[1] = neg_x16(f6);
  f[2] = neg_x16(f7);
  f[3] = f0;
  f[4] = f1;
  f[5] = f2;
  f[6] = f3;
  f[7] = f4;
}

/* multiply f by x^10 in (k^16)[x]/(x^8+1) */
static void twist8_10(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = f6;
  f[1] = f7;
  f[2] = neg_x16(f0);
  f[3] = neg_x16(f1);
  f[4] = neg_x16(f2);
  f[5] = neg_x16(f3);
  f[6] = neg_x16(f4);
  f[7] = neg_x16(f5);
}

/* multiply f by x^12 in (k^16)[x]/(x^8+1) */
static void twist8_12(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = f4;
  f[1] = f5;
  f[2] = f6;
  f[3] = f7;
  f[4] = neg_x16(f0);
  f[5] = neg_x16(f1);
  f[6] = neg_x16(f2);
  f[7] = neg_x16(f3);
}

/* multiply f by x^13 in (k^16)[x]/(x^8+1) */
static void twist8_13(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = f3;
  f[1] = f4;
  f[2] = f5;
  f[3] = f6;
  f[4] = f7;
  f[5] = neg_x16(f0);
  f[6] = neg_x16(f1);
  f[7] = neg_x16(f2);
}

/* multiply f by x^14 in (k^16)[x]/(x^8+1) */
static void twist8_14(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = f2;
  f[1] = f3;
  f[2] = f4;
  f[3] = f5;
  f[4] = f6;
  f[5] = f7;
  f[6] = neg_x16(f0);
  f[7] = neg_x16(f1);
}

/* multiply f by x^15 in (k^16)[x]/(x^8+1) */
static void twist8_15(int16x16 f[8])
{
  int16x16 f0 = f[0];
  int16x16 f1 = f[1];
  int16x16 f2 = f[2];
  int16x16 f3 = f[3];
  int16x16 f4 = f[4];
  int16x16 f5 = f[5];
  int16x16 f6 = f[6];
  int16x16 f7 = f[7];
  f[0] = f1;
  f[1] = f2;
  f[2] = f3;
  f[3] = f4;
  f[4] = f5;
  f[5] = f6;
  f[6] = f7;
  f[7] = neg_x16(f0);
}

/* input range: +-4000 */
/* output range: +-4000 */
void fft64(int16x16 fpad[16][8],const int16x16 f[64])
{
  /* 256 mul + 512 add + some negations */

  int i,j;

  int16x16 a00 = f[0];
  int16x16 a04 = f[4];
  int16x16 a40 = f[32];
  int16x16 a44 = f[36];
  fpad[0][0] = add_x16(a00,a04);
  fpad[4][0] = sub_x16(a00,a04);
  fpad[0][4] = add_x16(a40,a44);
  fpad[4][4] = sub_x16(a40,a44);
  fpad[8][0] = sub_x16(a00,a44);
  fpad[12][0] = add_x16(a00,a44);
  fpad[8][4] = add_x16(a40,a04);
  fpad[12][4] = sub_x16(a40,a04);

  int16x16 a01 = f[1];
  int16x16 a05 = f[5];
  int16x16 a41 = f[33];
  int16x16 a45 = f[37];
  fpad[1][0] = add_x16(a01,a05);
  fpad[5][2] = sub_x16(a01,a05);
  fpad[1][4] = add_x16(a41,a45);
  fpad[5][6] = sub_x16(a41,a45);
  fpad[13][7] = neg_x16(add_x16(a01,a45));
  fpad[9][1] = sub_x16(a01,a45);
  fpad[13][3] = sub_x16(a41,a05);
  fpad[9][5] = add_x16(a41,a05);

  int16x16 a02 = f[2];
  int16x16 a06 = f[6];
  int16x16 a42 = f[34];
  int16x16 a46 = f[38];
  fpad[2][0] = add_x16(a02,a06);
  fpad[6][4] = sub_x16(a02,a06);
  fpad[2][4] = add_x16(a42,a46);
  fpad[6][0] = sub_x16(a46,a42);
  fpad[14][6] = neg_x16(add_x16(a02,a46));
  fpad[10][2] = sub_x16(a02,a46);
  fpad[14][2] = sub_x16(a42,a06);
  fpad[10][6] = add_x16(a42,a06);

  int16x16 a03 = f[3];
  int16x16 a07 = f[7];
  int16x16 a43 = f[35];
  int16x16 a47 = f[39];
  fpad[3][0] = add_x16(a03,a07);
  fpad[7][6] = sub_x16(a03,a07);
  fpad[3][4] = add_x16(a43,a47);
  fpad[7][2] = sub_x16(a47,a43);
  fpad[15][5] = neg_x16(add_x16(a03,a47));
  fpad[11][3] = sub_x16(a03,a47);
  fpad[15][1] = sub_x16(a43,a07);
  fpad[11][7] = add_x16(a43,a07);


  int16x16 a20 = f[16];
  int16x16 a24 = f[20];
  int16x16 a60 = f[48];
  int16x16 a64 = f[52];
  fpad[0][2] = add_x16(a20,a24);
  fpad[4][2] = sub_x16(a20,a24);
  fpad[0][6] = add_x16(a60,a64);
  fpad[4][6] = sub_x16(a60,a64);
  fpad[8][2] = sub_x16(a20,a64);
  fpad[12][2] = add_x16(a20,a64);
  fpad[8][6] = add_x16(a60,a24);
  fpad[12][6] = sub_x16(a60,a24);

  int16x16 a21 = f[17];
  int16x16 a25 = f[21];
  int16x16 a61 = f[49];
  int16x16 a65 = f[53];
  fpad[1][2] = add_x16(a21,a25);
  fpad[5][4] = sub_x16(a21,a25);
  fpad[1][6] = add_x16(a61,a65);
  fpad[5][0] = sub_x16(a65,a61);
  fpad[9][3] = sub_x16(a21,a65);
  fpad[13][1] = add_x16(a21,a65);
  fpad[9][7] = add_x16(a61,a25);
  fpad[13][5] = sub_x16(a61,a25);

  int16x16 a22 = f[18];
  int16x16 a26 = f[22];
  int16x16 a62 = f[50];
  int16x16 a66 = f[54];
  fpad[2][2] = add_x16(a22,a26);
  fpad[6][6] = sub_x16(a22,a26);
  fpad[2][6] = add_x16(a62,a66);
  fpad[6][2] = sub_x16(a66,a62);
  fpad[10][4] = sub_x16(a22,a66);
  fpad[14][0] = add_x16(a22,a66);
  fpad[10][0] = neg_x16(add_x16(a62,a26));
  fpad[14][4] = sub_x16(a62,a26);

  int16x16 a23 = f[19];
  int16x16 a27 = f[23];
  int16x16 a63 = f[51];
  int16x16 a67 = f[55];
  fpad[3][2] = add_x16(a23,a27);
  fpad[7][0] = sub_x16(a27,a23);
  fpad[3][6] = add_x16(a63,a67);
  fpad[7][4] = sub_x16(a67,a63);
  fpad[11][5] = sub_x16(a23,a67);
  fpad[15][7] = neg_x16(add_x16(a23,a67));
  fpad[11][1] = neg_x16(add_x16(a63,a27));
  fpad[15][3] = sub_x16(a63,a27);


  int16x16 a10 = f[8];
  int16x16 a14 = f[12];
  int16x16 a50 = f[40];
  int16x16 a54 = f[44];
  fpad[0][1] = add_x16(a10,a14);
  fpad[4][1] = sub_x16(a10,a14);
  fpad[0][5] = add_x16(a50,a54);
  fpad[4][5] = sub_x16(a50,a54);
  fpad[8][1] = sub_x16(a10,a54);
  fpad[12][1] = add_x16(a10,a54);
  fpad[8][5] = add_x16(a50,a14);
  fpad[12][5] = sub_x16(a50,a14);

  int16x16 a11 = f[9];
  int16x16 a15 = f[13];
  int16x16 a51 = f[41];
  int16x16 a55 = f[45];
  fpad[1][1] = add_x16(a11,a15);
  fpad[5][3] = sub_x16(a11,a15);
  fpad[1][5] = add_x16(a51,a55);
  fpad[5][7] = sub_x16(a51,a55);
  fpad[9][2] = sub_x16(a11,a55);
  fpad[13][0] = add_x16(a11,a55);
  fpad[9][6] = add_x16(a51,a15);
  fpad[13][4] = sub_x16(a51,a15);

  int16x16 a12 = f[10];
  int16x16 a16 = f[14];
  int16x16 a52 = f[42];
  int16x16 a56 = f[46];
  fpad[2][1] = add_x16(a12,a16);
  fpad[6][5] = sub_x16(a12,a16);
  fpad[2][5] = add_x16(a52,a56);
  fpad[6][1] = sub_x16(a56,a52);
  fpad[10][3] = sub_x16(a12,a56);
  fpad[14][7] = neg_x16(add_x16(a12,a56));
  fpad[10][7] = add_x16(a52,a16);
  fpad[14][3] = sub_x16(a52,a16);

  int16x16 a13 = f[11];
  int16x16 a17 = f[15];
  int16x16 a53 = f[43];
  int16x16 a57 = f[47];
  fpad[3][1] = add_x16(a13,a17);
  fpad[7][7] = sub_x16(a13,a17);
  fpad[3][5] = add_x16(a53,a57);
  fpad[7][3] = sub_x16(a57,a53);
  fpad[11][4] = sub_x16(a13,a57);
  fpad[15][6] = neg_x16(add_x16(a13,a57));
  fpad[11][0] = neg_x16(add_x16(a53,a17));
  fpad[15][2] = sub_x16(a53,a17);


  int16x16 a30 = f[24];
  int16x16 a34 = f[28];
  int16x16 a70 = f[56];
  int16x16 a74 = f[60];
  fpad[0][3] = add_x16(a30,a34);
  fpad[0][7] = add_x16(a70,a74);
  fpad[4][3] = sub_x16(a30,a34);
  fpad[4][7] = sub_x16(a70,a74);
  fpad[8][3] = sub_x16(a30,a74);
  fpad[8][7] = add_x16(a70,a34);
  fpad[12][3] = add_x16(a30,a74);
  fpad[12][7] = sub_x16(a70,a34);

  int16x16 a31 = f[25];
  int16x16 a35 = f[29];
  int16x16 a71 = f[57];
  int16x16 a75 = f[61];
  fpad[1][3] = add_x16(a31,a35);
  fpad[1][7] = add_x16(a71,a75);
  fpad[5][5] = sub_x16(a31,a35);
  fpad[5][1] = sub_x16(a75,a71);
  fpad[9][4] = sub_x16(a31,a75);
  fpad[9][0] = neg_x16(add_x16(a71,a35));
  fpad[13][2] = add_x16(a31,a75);
  fpad[13][6] = sub_x16(a71,a35);

  int16x16 a32 = f[26];
  int16x16 a36 = f[30];
  int16x16 a72 = f[58];
  int16x16 a76 = f[62];
  fpad[2][3] = add_x16(a32,a36);
  fpad[2][7] = add_x16(a72,a76);
  fpad[6][7] = sub_x16(a32,a36);
  fpad[6][3] = sub_x16(a76,a72);
  fpad[10][5] = sub_x16(a32,a76);
  fpad[10][1] = neg_x16(add_x16(a72,a36));
  fpad[14][1] = add_x16(a32,a76);
  fpad[14][5] = sub_x16(a72,a36);

  int16x16 a33 = f[27];
  int16x16 a37 = f[31];
  int16x16 a73 = f[59];
  int16x16 a77 = f[63];
  fpad[3][3] = add_x16(a33,a37);
  fpad[3][7] = add_x16(a73,a77);
  fpad[7][1] = sub_x16(a37,a33);
  fpad[7][5] = sub_x16(a77,a73);
  fpad[11][6] = sub_x16(a33,a77);
  fpad[11][2] = neg_x16(add_x16(a73,a37));
  fpad[15][0] = add_x16(a33,a77);
  fpad[15][4] = sub_x16(a73,a37);

  for (i = 0;i < 16;i += 4) {
    for (j = 0;j < 4;++j) {
      int16x16 a = fpad[i][j];
      int16x16 c = fpad[i+2][j];
      int16x16 A = fpad[i][j+4];
      int16x16 C = fpad[i+2][j+4];
      int16x16 r = add_x16(a,c);
      int16x16 s = add_x16(A,C);
      int16x16 b = fpad[i+1][j];
      int16x16 d = fpad[i+3][j];
      int16x16 B = fpad[i+1][j+4];
      int16x16 D = fpad[i+3][j+4];
      int16x16 R = add_x16(b,d);
      int16x16 S = add_x16(B,D);
      int16x16 t = sub_x16(a,c);
      int16x16 u = sub_x16(A,C);
      int16x16 T = sub_x16(b,d);
      int16x16 U = sub_x16(B,D);
      fpad[i][j] = squeeze13_x16(add_x16(r,R));
      fpad[i+1][j] = squeeze13_x16(sub_x16(r,R));
      fpad[i][j+4] = squeeze13_x16(add_x16(s,S));
      fpad[i+1][j+4] = squeeze13_x16(sub_x16(s,S));
      fpad[i+2][j] = squeeze13_x16(sub_x16(t,U));
      fpad[i+2][j+4] = squeeze13_x16(add_x16(u,T));
      fpad[i+3][j] = squeeze13_x16(add_x16(t,U));
      fpad[i+3][j+4] = squeeze13_x16(sub_x16(u,T));
    }
  }
}

/* inverse of fft64 except for a multiplication by 16 */
/* input range: +-8000 */
/* output range: +-4000 */
void unfft64_scale16(int16x16 f[64],int16x16 fpad[16][8])
{
  int i,j;

  for (i = 0;i < 16;++i)
    for (j = 0;j < 8;++j)
      assertrange_x16(fpad[i][j],8000);

  for (i = 0;i < 16;i += 4) {
    for (j = 0;j < 4;++j) {
      int16x16 a = fpad[i][j];
      int16x16 A = fpad[i+1][j];
      int16x16 b = fpad[i][j+4];
      int16x16 B = fpad[i+1][j+4];
      int16x16 c = fpad[i+2][j];
      int16x16 d = fpad[i+2][j+4];
      int16x16 C = fpad[i+3][j];
      int16x16 D = fpad[i+3][j+4];
      int16x16 r = add_x16(a,A);
      int16x16 s = sub_x16(a,A);
      int16x16 t = add_x16(b,B);
      int16x16 u = sub_x16(b,B);
      int16x16 R = add_x16(C,c);
      int16x16 S = sub_x16(d,D);
      int16x16 T = add_x16(d,D);
      int16x16 U = sub_x16(C,c);
      fpad[i][j] = add_x16(r,R);
      fpad[i+1][j] = add_x16(s,S);
      fpad[i][j+4] = add_x16(t,T);
      fpad[i+1][j+4] = add_x16(u,U);
      fpad[i+2][j] = sub_x16(r,R);
      fpad[i+3][j] = sub_x16(s,S);
      fpad[i+2][j+4] = sub_x16(t,T);
      fpad[i+3][j+4] = sub_x16(u,U);
    }
  }

  twist8_14(fpad[5]);
  twist8_12(fpad[6]);
  twist8_10(fpad[7]);
  twist8_15(fpad[9]);
  twist8_14(fpad[10]);
  twist8_13(fpad[11]);
  twist8_1(fpad[13]);
  twist8_2(fpad[14]);
  twist8_3(fpad[15]);

  for (i = 0;i < 3;++i) {
    for (j = 0;j < 4;++j) {
      int16x16 a = squeeze13_x16(fpad[i][j]);
      int16x16 b = squeeze13_x16(fpad[i][j+4]);
      int16x16 A = squeeze13_x16(fpad[i+4][j]);
      int16x16 B = squeeze13_x16(fpad[i+4][j+4]);
      int16x16 c = squeeze13_x16(fpad[i+8][j]);
      int16x16 d = squeeze13_x16(fpad[i+8][j+4]);
      int16x16 C = squeeze13_x16(fpad[i+12][j]);
      int16x16 D = squeeze13_x16(fpad[i+12][j+4]);

      int16x16 r = add_x16(a,A);
      int16x16 s = add_x16(b,B);
      int16x16 t = sub_x16(a,A);
      int16x16 u = sub_x16(b,B);
      int16x16 R = add_x16(C,c);
      int16x16 S = add_x16(d,D);
      int16x16 T = sub_x16(d,D);
      int16x16 U = sub_x16(C,c);

      fpad[i][j] = add_x16(r,R);
      fpad[i][j+4] = add_x16(s,S);
      fpad[i+4][j] = add_x16(t,T);
      fpad[i+4][j+4] = add_x16(u,U);
      fpad[i+8][j] = sub_x16(r,R);
      fpad[i+8][j+4] = sub_x16(s,S);
      fpad[i+12][j] = sub_x16(t,T);
      fpad[i+12][j+4] = sub_x16(u,U);
    }
  }
  for (i = 3;i < 4;++i) {
    for (j = 0;j < 4;++j) {
      int16x16 a = squeeze13_x16(fpad[i][j]);
      int16x16 b = squeeze13_x16(fpad[i][j+4]);
      int16x16 A = squeeze13_x16(fpad[i+4][j]);
      int16x16 B = squeeze13_x16(fpad[i+4][j+4]);
      int16x16 c = squeeze13_x16(fpad[i+8][j]);
      int16x16 d = squeeze13_x16(fpad[i+8][j+4]);
      int16x16 C = squeeze13_x16(fpad[i+12][j]);
      int16x16 D = squeeze13_x16(fpad[i+12][j+4]);

      int16x16 r = add_x16(a,A);
      int16x16 s = add_x16(b,B);
      int16x16 t = sub_x16(a,A);
      int16x16 u = sub_x16(b,B);
      int16x16 R = add_x16(C,c);
      int16x16 S = add_x16(d,D);

      fpad[i][j] = add_x16(r,R);
      fpad[i][j+4] = add_x16(s,S);
      fpad[i+4][j] = add_x16(t,t);
      fpad[i+4][j+4] = add_x16(u,u);
      fpad[i+8][j] = sub_x16(r,R);
      fpad[i+8][j+4] = sub_x16(s,S);
      /* not used below:
         fpad[i+12][j] = const_x16(0);
         fpad[i+12][j+4] = const_x16(0);
      */
    }
  }

  /* map to (k[x]/(x^8+1))[y]/(y^8-x) */

  for (i = 0;i < 7;++i) {
    f[i] = squeeze13_x16(sub_x16(fpad[i][0],fpad[i+8][7]));
    for (j = 1;j < 8;++j)
      f[i+8*j] = squeeze13_x16(add_x16(fpad[i][j],fpad[i+8][j-1]));
  }
  for (i = 7;i < 8;++i) {
    /* y^15 does not appear; i.e., fpad[i+8] is 0 */
    for (j = 0;j < 8;++j)
      f[i+8*j] = squeeze13_x16(fpad[i][j]);
  }

  for (i = 0;i < 64;++i)
    assertrange_x16(f[i],4000);
}

/* h = fg/4096 in (k^16)[y]/(y^64+1) */
/* input range: +-4000 */
/* output range: +-4000 */
void mult64_nega_over4096_x16(int16x16 h[64],const int16x16 f[64],const int16x16 g[64])
{
  /* strategy: Nussbaumer's trick */
  /* map k[y]/(y^64+1) to (k[x]/(x^8+1))[y]/(y^8-x) */
  /* lift to (k[x]/(x^8+1))[y] */
  /* map to (k[x]/(x^8+1))[y]/(y^16-1) */
  /* then use size-16 FFT, and 16 mults in k[x]/(x^8+1) */

  int16x16 fpad[16][8];
  int16x16 gpad[16][8];
#define hpad fpad
  int i;

  for (i = 0;i < 64;++i) {
    assertrange_x16(f[i],4000);
    assertrange_x16(g[i],4000);
  }

  fft64(fpad,f);
  fft64(gpad,g);

  for (i = 0;i < 16;++i)
    mult8_nega_over65536_x16(hpad[i],fpad[i],gpad[i]);

  unfft64_scale16(h,hpad);

  for (i = 0;i < 64;++i)
    assertrange_x16(h[i],4000);
}

static inline void assertrange8_64(const int16 f[8][64],int limit)
{
  int i,j;
  for (i = 0;i < 8;++i)
    for (j = 0;j < 64;++j) {
      assert(f[i][j] >= -limit);
      assert(f[i][j] <= limit);
    }
}

/* input in (k[x]/(x^64+1))[y]/(y^8-1) */
/* f represents poly: sum f[i][j] y^i x^j */
/* output (in place): 8 elements of k[x]/(x^64+1) */
/* input range: +-4000 */
/* output range: +-4000 */
void fft8_64(int16 f[8][64])
{
  assertrange8_64(f,4000);

  int16x16 a00 = load_x16(&f[0][0]);
  int16x16 a40 = load_x16(&f[4][0]);
  int16x16 a20 = load_x16(&f[2][0]);
  int16x16 a60 = load_x16(&f[6][0]);
  int16x16 a10 = load_x16(&f[1][0]);
  int16x16 a50 = load_x16(&f[5][0]);
  int16x16 a30 = load_x16(&f[3][0]);
  int16x16 a70 = load_x16(&f[7][0]);

  int16x16 b00 = add_x16(a00,a40);
  int16x16 b40 = sub_x16(a00,a40);

  int16x16 b20 = add_x16(a20,a60);
  int16x16 b60 = sub_x16(a20,a60);

  int16x16 b10 = add_x16(a10,a50);
  int16x16 b50 = sub_x16(a10,a50);

  int16x16 b30 = add_x16(a30,a70);
  int16x16 b70 = sub_x16(a30,a70);

  int16x16 c00 = add_x16(b00,b20);
  int16x16 c20 = sub_x16(b00,b20);

  int16x16 c10 = add_x16(b10,b30);
  int16x16 c30 = sub_x16(b10,b30);

  int16x16 d00 = add_x16(c00,c10);
  d00 = squeeze13_x16(d00);
  store_x16(&f[0][0],d00);
  int16x16 d10 = sub_x16(c00,c10);
  d10 = squeeze13_x16(d10);
  store_x16(&f[1][0],d10);


  int16x16 a12 = load_x16(&f[1][32]);
  int16x16 a52 = load_x16(&f[5][32]);
  int16x16 a32 = load_x16(&f[3][32]);
  int16x16 a72 = load_x16(&f[7][32]);

  int16x16 b12 = add_x16(a12,a52);
  int16x16 b52 = sub_x16(a12,a52);

  int16x16 b32 = add_x16(a32,a72);
  int16x16 b72 = sub_x16(a32,a72);

  int16x16 c32 = sub_x16(b12,b32);
  int16x16 c12 = add_x16(b12,b32);

  int16x16 d20 = sub_x16(c20,c32);
  d20 = squeeze13_x16(d20);
  store_x16(&f[2][0],d20);
  int16x16 d30 = add_x16(c20,c32);
  d30 = squeeze13_x16(d30);
  store_x16(&f[3][0],d30);


  int16x16 a02 = load_x16(&f[0][32]);
  int16x16 a42 = load_x16(&f[4][32]);
  int16x16 a22 = load_x16(&f[2][32]);
  int16x16 a62 = load_x16(&f[6][32]);

  int16x16 b02 = add_x16(a02,a42);
  int16x16 b42 = sub_x16(a02,a42);

  int16x16 b22 = add_x16(a22,a62);
  int16x16 b62 = sub_x16(a22,a62);

  int16x16 c22 = sub_x16(b02,b22);
  int16x16 c02 = add_x16(b02,b22);

  int16x16 d22 = add_x16(c22,c30);
  d22 = squeeze13_x16(d22);
  store_x16(&f[2][32],d22);
  int16x16 d32 = sub_x16(c22,c30);
  d32 = squeeze13_x16(d32);
  store_x16(&f[3][32],d32);

  int16x16 d02 = add_x16(c02,c12);
  d02 = squeeze13_x16(d02);
  store_x16(&f[0][32],d02);
  int16x16 d12 = sub_x16(c02,c12);
  d12 = squeeze13_x16(d12);
  store_x16(&f[1][32],d12);



  int16x16 a01 = load_x16(&f[0][16]);
  int16x16 a41 = load_x16(&f[4][16]);
  int16x16 a21 = load_x16(&f[2][16]);
  int16x16 a61 = load_x16(&f[6][16]);

  int16x16 b01 = add_x16(a01,a41);
  int16x16 b41 = sub_x16(a01,a41);

  int16x16 b21 = add_x16(a21,a61);
  int16x16 b61 = sub_x16(a21,a61);

  int16x16 c01 = add_x16(b01,b21);
  int16x16 c21 = sub_x16(b01,b21);

  int16x16 a11 = load_x16(&f[1][16]);
  int16x16 a51 = load_x16(&f[5][16]);

  int16x16 b11 = add_x16(a11,a51);
  int16x16 b51 = sub_x16(a11,a51);

  int16x16 a31 = load_x16(&f[3][16]);
  int16x16 a71 = load_x16(&f[7][16]);

  int16x16 b31 = add_x16(a31,a71);
  int16x16 b71 = sub_x16(a31,a71);

  int16x16 c11 = add_x16(b11,b31);
  int16x16 c31 = sub_x16(b11,b31);

  int16x16 d01 = add_x16(c01,c11);
  d01 = squeeze13_x16(d01);
  store_x16(&f[0][16],d01);
  int16x16 d11 = sub_x16(c01,c11);
  d11 = squeeze13_x16(d11);
  store_x16(&f[1][16],d11);


  int16x16 a13 = load_x16(&f[1][48]);
  int16x16 a53 = load_x16(&f[5][48]);

  int16x16 a33 = load_x16(&f[3][48]);
  int16x16 a73 = load_x16(&f[7][48]);

  int16x16 b13 = add_x16(a13,a53);
  int16x16 b53 = sub_x16(a13,a53);

  int16x16 b33 = add_x16(a33,a73);
  int16x16 b73 = sub_x16(a33,a73);

  int16x16 c33 = sub_x16(b13,b33);
  int16x16 c13 = add_x16(b13,b33);

  int16x16 d21 = sub_x16(c21,c33);
  d21 = squeeze13_x16(d21);
  store_x16(&f[2][16],d21);
  int16x16 d31 = add_x16(c21,c33);
  d31 = squeeze13_x16(d31);
  store_x16(&f[3][16],d31);


  int16x16 c51 = sub_x16(b51,b73);
  int16x16 c71 = add_x16(b51,b73);

  int16x16 c42 = add_x16(b42,b60);
  int16x16 c62 = sub_x16(b42,b60);

  int16x16 d42 = add_x16(c42,c51);
  d42 = squeeze13_x16(d42);
  store_x16(&f[4][32],d42);
  int16x16 d52 = sub_x16(c42,c51);
  d52 = squeeze13_x16(d52);
  store_x16(&f[5][32],d52);


  int16x16 c73 = sub_x16(b53,b71);
  int16x16 c53 = add_x16(b53,b71);

  int16x16 d62 = sub_x16(c62,c73);
  d62 = squeeze13_x16(d62);
  store_x16(&f[6][32],d62);
  int16x16 d72 = add_x16(c62,c73);
  d72 = squeeze13_x16(d72);
  store_x16(&f[7][32],d72);


  int16x16 c40 = sub_x16(b40,b62);
  int16x16 c60 = add_x16(b40,b62);

  int16x16 d40 = sub_x16(c40,c53);
  d40 = squeeze13_x16(d40);
  store_x16(&f[4][0],d40);
  int16x16 d50 = add_x16(c40,c53);
  d50 = squeeze13_x16(d50);
  store_x16(&f[5][0],d50);

  int16x16 d60 = sub_x16(c60,c71);
  d60 = squeeze13_x16(d60);
  store_x16(&f[6][0],d60);
  int16x16 d70 = add_x16(c60,c71);
  d70 = squeeze13_x16(d70);
  store_x16(&f[7][0],d70);


  int16x16 a23 = load_x16(&f[2][48]);
  int16x16 a63 = load_x16(&f[6][48]);

  int16x16 b63 = sub_x16(a23,a63);
  int16x16 b23 = add_x16(a23,a63);

  int16x16 c41 = sub_x16(b41,b63);
  int16x16 c61 = add_x16(b41,b63);

  int16x16 c50 = sub_x16(b50,b72);
  int16x16 c70 = add_x16(b50,b72);

  int16x16 d41 = add_x16(c41,c50);
  d41 = squeeze13_x16(d41);
  store_x16(&f[4][16],d41);
  int16x16 d51 = sub_x16(c41,c50);
  d51 = squeeze13_x16(d51);
  store_x16(&f[5][16],d51);


  int16x16 a03 = load_x16(&f[0][48]);
  int16x16 a43 = load_x16(&f[4][48]);

  int16x16 b03 = add_x16(a03,a43);
  int16x16 b43 = sub_x16(a03,a43);

  int16x16 c43 = add_x16(b43,b61);
  int16x16 c63 = sub_x16(b43,b61);

  int16x16 c52 = add_x16(b52,b70);
  int16x16 c72 = sub_x16(b52,b70);

  int16x16 d43 = add_x16(c43,c52);
  d43 = squeeze13_x16(d43);
  store_x16(&f[4][48],d43);
  int16x16 d53 = sub_x16(c43,c52);
  d53 = squeeze13_x16(d53);
  store_x16(&f[5][48],d53);

  int16x16 d61 = sub_x16(c61,c72);
  d61 = squeeze13_x16(d61);
  store_x16(&f[6][16],d61);
  int16x16 d71 = add_x16(c61,c72);
  d71 = squeeze13_x16(d71);
  store_x16(&f[7][16],d71);

  int16x16 d63 = add_x16(c63,c70);
  d63 = squeeze13_x16(d63);
  store_x16(&f[6][48],d63);
  int16x16 d73 = sub_x16(c63,c70);
  d73 = squeeze13_x16(d73);
  store_x16(&f[7][48],d73);

  int16x16 c03 = add_x16(b03,b23);
  int16x16 c23 = sub_x16(b03,b23);

  int16x16 d03 = add_x16(c03,c13);
  d03 = squeeze13_x16(d03);
  store_x16(&f[0][48],d03);
  int16x16 d13 = sub_x16(c03,c13);
  d13 = squeeze13_x16(d13);
  store_x16(&f[1][48],d13);

  int16x16 d23 = add_x16(c23,c31);
  d23 = squeeze13_x16(d23);
  store_x16(&f[2][48],d23);
  int16x16 d33 = sub_x16(c23,c31);
  d33 = squeeze13_x16(d33);
  store_x16(&f[3][48],d33);

  assertrange8_64(f,4000);
}

/* input range: +-4000 */
/* output range: +-32000 */
void unfft8_64_scale8(int16 f[8][64])
{
  int i,j;

  assertrange8_64(f,4000);

  /* undo stage 3: y-1,y+1,y-x^32,y+x^32,y-x^16,y+x^16,y-x^48,y+x^48 */
  for (i = 0;i < 1;++i)
    for (j = 0;j < 64;++j) {
      int16 a = f[i][j];
      int16 A = f[i+1][j];
      f[i][j] = add(a,A);
      f[i+1][j] = sub(a,A);
    }
  for (i = 2;i < 3;++i)
    for (j = 0;j < 32;++j) {
      int16 a = f[i][j];
      int16 b = f[i][j+32];
      int16 A = f[i+1][j];
      int16 B = f[i+1][j+32];
      f[i][j] = add(a,A);
      f[i][j+32] = add(b,B);
      f[i+1][j] = sub(b,B);
      f[i+1][j+32] = sub(A,a);
    }
  for (i = 4;i < 5;++i)
    for (j = 0;j < 16;++j) {
      int16 a = f[i][j];
      int16 b = f[i][j+16];
      int16 c = f[i][j+32];
      int16 d = f[i][j+48];
      int16 A = f[i+1][j];
      int16 B = f[i+1][j+16];
      int16 C = f[i+1][j+32];
      int16 D = f[i+1][j+48];
      f[i][j] = add(a,A);
      f[i][j+16] = add(b,B);
      f[i][j+32] = add(c,C);
      f[i][j+48] = add(d,D);
      f[i+1][j] = sub(b,B);
      f[i+1][j+16] = sub(c,C);
      f[i+1][j+32] = sub(d,D);
      f[i+1][j+48] = sub(A,a);
    }
  for (i = 6;i < 7;++i)
    for (j = 0;j < 16;++j) {
      int16 a = f[i][j];
      int16 b = f[i][j+16];
      int16 c = f[i][j+32];
      int16 d = f[i][j+48];
      int16 A = f[i+1][j];
      int16 B = f[i+1][j+16];
      int16 C = f[i+1][j+32];
      int16 D = f[i+1][j+48];
      f[i][j] = add(a,A);
      f[i][j+16] = add(b,B);
      f[i][j+32] = add(c,C);
      f[i][j+48] = add(d,D);
      f[i+1][j] = sub(d,D);
      f[i+1][j+16] = sub(A,a);
      f[i+1][j+32] = sub(B,b);
      f[i+1][j+48] = sub(C,c);
    }

  assertrange8_64(f,8000);

  for (i = 0;i < 2;++i) {
    for (j = 0;j < 32;++j) {
      int16 a = f[i][j];
      int16 b = f[i][j+32];
      int16 A = f[i+2][j];
      int16 B = f[i+2][j+32];
      int16 c = f[i+4][j];
      int16 d = f[i+4][j+32];
      int16 C = f[i+6][j];
      int16 D = f[i+6][j+32];
      int16 r = add(a,A);
      int16 s = add(b,B);
      int16 t = sub(a,A);
      int16 u = sub(b,B);
      int16 R = add(c,C);
      int16 S = add(d,D);
      int16 T = sub(d,D);
      int16 U = sub(C,c);
      f[i][j] = add(r,R);
      f[i+4][j] = sub(r,R);
      f[i][j+32] = add(s,S);
      f[i+4][j+32] = sub(s,S);
      f[i+2][j] = add(t,T);
      f[i+6][j] = sub(t,T);
      f[i+2][j+32] = add(u,U);
      f[i+6][j+32] = sub(u,U);
    }
  }

  assertrange8_64(f,32000);
}

#define squeeze13store_x8(p,f) store_x8((p),squeeze13_x8(f))
#define squeeze13store_x16(p,f) store_x16((p),squeeze13_x16(f))
#define negsqueeze13store_x8(p,f) store_x8((p),negsqueeze13_x8(f))
#define negsqueeze13store_x16(p,f) store_x16((p),negsqueeze13_x16(f))

static inline void rotate2(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+14);
  int16x16 f2 = load_x16(f+30);
  int16x16 f3 = load_x16(f+46);
  int16x8 ftop = load_x8(f+56);
  ftop = _mm_shuffle_epi32(ftop,0x3);
  negsqueeze13store_x8(f+0,ftop);
  squeeze13store_x16(f+2,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x16(f+32,f2);
  squeeze13store_x16(f+48,f3);
}

static inline void unrotate2(int16 f[64])
{
  int16x16 f0 = load_x16(f+2);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  int16x8 ftop = load_x8(f);
  ftop = _mm_shuffle_epi32(ftop,0x0);
  negsqueeze13store_x8(f+56,ftop);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+14,f1);
  squeeze13store_x16(f+30,f2);
  squeeze13store_x16(f+46,f3);
}

static inline void rotate4(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+12);
  int16x16 f2 = load_x16(f+28);
  int16x16 f3 = load_x16(f+44);
  int16x8 ftop = load_x8(f+56);
  ftop = _mm_shuffle_epi32(ftop,0xe);
  negsqueeze13store_x8(f+0,ftop);
  squeeze13store_x16(f+4,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x16(f+32,f2);
  squeeze13store_x16(f+48,f3);
}

static inline void unrotate4(int16 f[64])
{
  int16x16 f0 = load_x16(f+4);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  int16x8 ftop = load_x8(f+0);
  ftop = _mm_shuffle_epi32(ftop,0x40);
  negsqueeze13store_x8(f+56,ftop);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+12,f1);
  squeeze13store_x16(f+28,f2);
  squeeze13store_x16(f+44,f3);
}

static inline void rotate6(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+10);
  int16x16 f2 = load_x16(f+26);
  int16x16 f3 = load_x16(f+42);
  int16x8 ftop = load_x8(f+56);
  ftop = _mm_shuffle_epi32(ftop,0x39);
  negsqueeze13store_x8(f+0,ftop);
  squeeze13store_x16(f+6,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x16(f+32,f2);
  squeeze13store_x16(f+48,f3);
}

static inline void unrotate6(int16 f[64])
{
  int16x8 ftop = load_x8(f);
  int16x16 f0 = load_x16(f+6);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  ftop = _mm_shuffle_epi32(ftop,0x90);
  negsqueeze13store_x8(f+56,ftop);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+10,f1);
  squeeze13store_x16(f+26,f2);
  squeeze13store_x16(f+42,f3);
}

static inline void rotate8(int16 f[64])
{
  int16x8 f0 = load_x8(f);
  int16x16 f1 = load_x16(f+8);
  int16x16 f2 = load_x16(f+24);
  int16x16 f3 = load_x16(f+40);
  int16x8 ftop = load_x8(f+56);
  negsqueeze13store_x8(f+0,ftop);
  squeeze13store_x8(f+8,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x16(f+32,f2);
  squeeze13store_x16(f+48,f3);
}

static inline void unrotate8(int16 f[64])
{
  int16x8 f0 = load_x8(f+8);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  int16x8 ftop = load_x8(f+0);
  negsqueeze13store_x8(f+56,ftop);
  squeeze13store_x8(f+0,f0);
  squeeze13store_x16(f+8,f1);
  squeeze13store_x16(f+24,f2);
  squeeze13store_x16(f+40,f3);
}

static inline void rotate10(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+6);
  int16x16 f2 = load_x16(f+22);
  int16x16 f3 = load_x16(f+38);
  int16x8 ftop0 = load_x8(f+54);
  int16x8 ftop1 = load_x8(f+56);
  negsqueeze13store_x8(f+0,ftop0);
  negsqueeze13store_x8(f+2,ftop1);
  squeeze13store_x16(f+10,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x16(f+32,f2);
  squeeze13store_x16(f+48,f3);
}

static inline void unrotate10(int16 f[64])
{
  int16x16 f0 = load_x16(f+10);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  int16x8 ftop0 = load_x8(f+0);
  int16x8 ftop1 = load_x8(f+2);
  negsqueeze13store_x8(f+54,ftop0);
  negsqueeze13store_x8(f+56,ftop1);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+6,f1);
  squeeze13store_x16(f+22,f2);
  squeeze13store_x16(f+38,f3);
}

static inline void rotate12(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+4);
  int16x16 f2 = load_x16(f+20);
  int16x16 f3 = load_x16(f+36);
  int16x8 ftop0 = load_x8(f+52);
  int16x8 ftop1 = load_x8(f+56);
  negsqueeze13store_x8(f+0,ftop0);
  negsqueeze13store_x8(f+4,ftop1);
  squeeze13store_x16(f+12,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x16(f+32,f2);
  squeeze13store_x16(f+48,f3);
}

static inline void unrotate12(int16 f[64])
{
  int16x16 f0 = load_x16(f+12);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  int16x8 ftop0 = load_x8(f+0);
  int16x8 ftop1 = load_x8(f+4);
  negsqueeze13store_x8(f+52,ftop0);
  negsqueeze13store_x8(f+56,ftop1);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+4,f1);
  squeeze13store_x16(f+20,f2);
  squeeze13store_x16(f+36,f3);
}

static inline void rotate14(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+2);
  int16x16 f2 = load_x16(f+18);
  int16x16 f3 = load_x16(f+34);
  int16x8 ftop0 = load_x8(f+50);
  int16x8 ftop1 = load_x8(f+56);
  negsqueeze13store_x8(f+0,ftop0);
  negsqueeze13store_x8(f+6,ftop1);
  squeeze13store_x16(f+14,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x16(f+32,f2);
  squeeze13store_x16(f+48,f3);
}

static inline void unrotate14(int16 f[64])
{
  int16x16 f0 = load_x16(f+14);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  int16x8 ftop0 = load_x8(f+0);
  int16x8 ftop1 = load_x8(f+6);
  negsqueeze13store_x8(f+50,ftop0);
  negsqueeze13store_x8(f+56,ftop1);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+2,f1);
  squeeze13store_x16(f+18,f2);
  squeeze13store_x16(f+34,f3);
}

static inline void rotate16(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  negsqueeze13store_x16(f+0,f3);
  squeeze13store_x16(f+16,f0);
  squeeze13store_x16(f+32,f1);
  squeeze13store_x16(f+48,f2);
}

static inline void unrotate16(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  squeeze13store_x16(f+0,f1);
  squeeze13store_x16(f+16,f2);
  squeeze13store_x16(f+32,f3);
  negsqueeze13store_x16(f+48,f0);
}

static inline void rotate18(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+14);
  int16x16 f2 = load_x16(f+30);
  int16x16 ftop0 = load_x16(f+46);
  int16x8 ftop1 = load_x8(f+56);
  negsqueeze13store_x16(f+0,ftop0);
  negsqueeze13store_x8(f+10,ftop1);
  squeeze13store_x16(f+18,f0);
  squeeze13store_x16(f+32,f1);
  squeeze13store_x16(f+48,f2);
}

static inline void unrotate18(int16 f[64])
{
  int16x16 f0 = load_x16(f+18);
  int16x16 f1 = load_x16(f+32);
  int16x16 f2 = load_x16(f+48);
  int16x16 ftop0 = load_x16(f+0);
  int16x8 ftop1 = load_x8(f+10);
  negsqueeze13store_x16(f+46,ftop0);
  negsqueeze13store_x8(f+56,ftop1);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+14,f1);
  squeeze13store_x16(f+30,f2);
}

static inline void rotate24(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x8 f2 = load_x8(f+32);
  int16x16 ftop0 = load_x16(f+40);
  int16x8 ftop1 = load_x8(f+56);
  negsqueeze13store_x16(f+0,ftop0);
  negsqueeze13store_x8(f+16,ftop1);
  squeeze13store_x16(f+24,f0);
  squeeze13store_x16(f+40,f1);
  squeeze13store_x8(f+56,f2);
}

static inline void unrotate24(int16 f[64])
{
  int16x16 f0 = load_x16(f+24);
  int16x16 f1 = load_x16(f+40);
  int16x8 f2 = load_x8(f+56);
  int16x16 ftop0 = load_x16(f+0);
  int16x8 ftop1 = load_x8(f+16);
  negsqueeze13store_x16(f+40,ftop0);
  negsqueeze13store_x8(f+56,ftop1);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x8(f+32,f2);
}

static inline void rotate30(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x8 f2 = load_x8(f+26);
  int16x16 ftop0 = load_x16(f+34);
  int16x16 ftop1 = load_x16(f+48);
  negsqueeze13store_x16(f+0,ftop0);
  negsqueeze13store_x16(f+14,ftop1);
  squeeze13store_x16(f+30,f0);
  squeeze13store_x16(f+46,f1);
  squeeze13store_x8(f+56,f2);
}

static inline void unrotate30(int16 f[64])
{
  int16x16 f0 = load_x16(f+30);
  int16x16 f1 = load_x16(f+46);
  int16x8 f2 = load_x8(f+56);
  int16x16 ftop0 = load_x16(f+0);
  int16x16 ftop1 = load_x16(f+14);
  negsqueeze13store_x16(f+34,ftop0);
  negsqueeze13store_x16(f+48,ftop1);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+16,f1);
  squeeze13store_x8(f+26,f2);
}

static inline void rotate32(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  negsqueeze13store_x16(f+0,f2);
  negsqueeze13store_x16(f+16,f3);
  squeeze13store_x16(f+32,f0);
  squeeze13store_x16(f+48,f1);
}

static inline void unrotate32(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  squeeze13store_x16(f+0,f2);
  squeeze13store_x16(f+16,f3);
  negsqueeze13store_x16(f+32,f0);
  negsqueeze13store_x16(f+48,f1);
}

static inline void rotate36(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+12);
  int16x8 ftop0 = load_x8(f+28);
  int16x16 ftop1 = load_x16(f+32);
  int16x16 ftop2 = load_x16(f+48);
  negsqueeze13store_x8(f+0,ftop0);
  negsqueeze13store_x16(f+4,ftop1);
  negsqueeze13store_x16(f+20,ftop2);
  squeeze13store_x16(f+36,f0);
  squeeze13store_x16(f+48,f1);
}

static inline void unrotate36(int16 f[64])
{
  int16x16 f0 = load_x16(f+36);
  int16x16 f1 = load_x16(f+48);
  int16x8 ftop0 = load_x8(f+0);
  int16x16 ftop1 = load_x16(f+4);
  int16x16 ftop2 = load_x16(f+20);
  negsqueeze13store_x8(f+28,ftop0);
  negsqueeze13store_x16(f+32,ftop1);
  negsqueeze13store_x16(f+48,ftop2);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x16(f+12,f1);
}

static inline void rotate40(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x8 f1 = load_x8(f+16);
  int16x8 ftop0 = load_x8(f+24);
  int16x16 ftop1 = load_x16(f+32);
  int16x16 ftop2 = load_x16(f+48);
  negsqueeze13store_x8(f+0,ftop0);
  negsqueeze13store_x16(f+8,ftop1);
  negsqueeze13store_x16(f+24,ftop2);
  squeeze13store_x16(f+40,f0);
  squeeze13store_x8(f+56,f1);
}

static inline void unrotate40(int16 f[64])
{
  int16x16 f0 = load_x16(f+40);
  int16x8 f1 = load_x8(f+56);
  int16x8 ftop0 = load_x8(f+0);
  int16x16 ftop1 = load_x16(f+8);
  int16x16 ftop2 = load_x16(f+24);
  negsqueeze13store_x8(f+24,ftop0);
  negsqueeze13store_x16(f+32,ftop1);
  negsqueeze13store_x16(f+48,ftop2);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x8(f+16,f1);
}

static inline void rotate42(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x8 f1 = load_x8(f+14);
  int16x16 ftop0 = load_x16(f+22);
  int16x16 ftop1 = load_x16(f+32);
  int16x16 ftop2 = load_x16(f+48);
  negsqueeze13store_x16(f+0,ftop0);
  negsqueeze13store_x16(f+10,ftop1);
  negsqueeze13store_x16(f+26,ftop2);
  squeeze13store_x16(f+42,f0);
  squeeze13store_x8(f+56,f1);
}

static inline void unrotate42(int16 f[64])
{
  int16x16 f0 = load_x16(f+42);
  int16x8 f1 = load_x8(f+56);
  int16x16 ftop0 = load_x16(f+0);
  int16x16 ftop1 = load_x16(f+10);
  int16x16 ftop2 = load_x16(f+26);
  negsqueeze13store_x16(f+22,ftop0);
  negsqueeze13store_x16(f+32,ftop1);
  negsqueeze13store_x16(f+48,ftop2);
  squeeze13store_x16(f+0,f0);
  squeeze13store_x8(f+14,f1);
}

static inline void rotate48(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  negsqueeze13store_x16(f+0,f1);
  negsqueeze13store_x16(f+16,f2);
  negsqueeze13store_x16(f+32,f3);
  squeeze13store_x16(f+48,f0);
}

static inline void unrotate48(int16 f[64])
{
  int16x16 f0 = load_x16(f);
  int16x16 f1 = load_x16(f+16);
  int16x16 f2 = load_x16(f+32);
  int16x16 f3 = load_x16(f+48);
  squeeze13store_x16(f+0,f3);
  negsqueeze13store_x16(f+16,f0);
  negsqueeze13store_x16(f+32,f1);
  negsqueeze13store_x16(f+48,f2);
}

static inline void rotate56(int16 f[64])
{
  int16x8 f0 = load_x8(f+0);
  int16x8 ftop0 = load_x8(f+8);
  int16x16 ftop1 = load_x16(f+16);
  int16x16 ftop2 = load_x16(f+32);
  int16x16 ftop3 = load_x16(f+48);
  negsqueeze13store_x8(f+0,ftop0);
  negsqueeze13store_x16(f+8,ftop1);
  negsqueeze13store_x16(f+24,ftop2);
  negsqueeze13store_x16(f+40,ftop3);
  squeeze13store_x8(f+56,f0);
}

static inline void unrotate56(int16 f[64])
{
  int16x8 f0 = load_x8(f+56);
  int16x8 ftop0 = load_x8(f+0);
  int16x16 ftop1 = load_x16(f+8);
  int16x16 ftop2 = load_x16(f+24);
  int16x16 ftop3 = load_x16(f+40);
  negsqueeze13store_x8(f+8,ftop0);
  negsqueeze13store_x16(f+16,ftop1);
  negsqueeze13store_x16(f+32,ftop2);
  negsqueeze13store_x16(f+48,ftop3);
  squeeze13store_x8(f+0,f0);
}

static inline void twist64_2(int16 f[8][64])
{
  rotate2(f[1]);
  rotate4(f[2]);
  rotate6(f[3]);
  rotate8(f[4]);
  rotate10(f[5]);
  rotate12(f[6]);
  rotate14(f[7]);
}

static inline void twist64_6(int16 f[8][64])
{
  rotate6(f[1]);
  rotate12(f[2]);
  rotate18(f[3]);
  rotate24(f[4]);
  rotate30(f[5]);
  rotate36(f[6]);
  rotate42(f[7]);
}

static inline void twist64_8(int16 f[8][64])
{
  rotate8(f[1]);
  rotate16(f[2]);
  rotate24(f[3]);
  rotate32(f[4]);
  rotate40(f[5]);
  rotate48(f[6]);
  rotate56(f[7]);
}

static inline void untwist64_2(int16 f[8][64])
{
  unrotate2(f[1]);
  unrotate4(f[2]);
  unrotate6(f[3]);
  unrotate8(f[4]);
  unrotate10(f[5]);
  unrotate12(f[6]);
  unrotate14(f[7]);
}

static inline void untwist64_6(int16 f[8][64])
{
  unrotate6(f[1]);
  unrotate12(f[2]);
  unrotate18(f[3]);
  unrotate24(f[4]);
  unrotate30(f[5]);
  unrotate36(f[6]);
  unrotate42(f[7]);
}

static inline void untwist64_8(int16 f[8][64])
{
  unrotate8(f[1]);
  unrotate16(f[2]);
  unrotate24(f[3]);
  unrotate32(f[4]);
  unrotate40(f[5]);
  unrotate48(f[6]);
  unrotate56(f[7]);
}

static inline void assertrange48_64(const int16 f[48][64],int limit)
{
  int i,j;
  for (i = 0;i < 48;++i)
    for (j = 0;j < 64;++j) {
      assert(f[i][j] >= -limit);
      assert(f[i][j] <= limit);
    }
}

/* size-48 truncated FFT over k[x]/(x^64+1) */
/* size-768 input -> (k[x]/(x^64+1))[y]/(y^48-y^32+y^16-1) */
/* output: 48 elements of k[x]/(x^64+1) */
/* input range: +-8000 */
/* output range: +-4000 */
void fft48_64(int16 f[48][64],const int16 orig[768])
{
  int i,j;

  for (i = 0;i < 8;++i) {
    int16x16 r = load_x16(&orig[i*32]);
    int16x16 s = load_x16(&orig[(i+8)*32]);
    int16x16 t = load_x16(&orig[(i+16)*32]);
    int16x16 rt = add_x16(r,t);

    int16x16 R = load_x16(&orig[i*32+16]);
    int16x16 S = load_x16(&orig[(i+8)*32+16]);
    int16x16 T = load_x16(&orig[(i+16)*32+16]);
    int16x16 RT = add_x16(R,T);

    store_x16(&f[i][0],r);
    store_x16(&f[i][16],add_x16(R,s));
    store_x16(&f[i][32],add_x16(t,S));
    store_x16(&f[i][48],T);

    store_x16(&f[i+8][0],r);
    store_x16(&f[i+8][16],sub_x16(R,s));
    store_x16(&f[i+8][32],sub_x16(t,S));
    store_x16(&f[i+8][48],T);

    store_x16(&f[i+16][0],sub_x16(r,S));
    store_x16(&f[i+16][16],R);
    store_x16(&f[i+16][32],neg_x16(t));
    store_x16(&f[i+16][48],sub_x16(s,T));

    store_x16(&f[i+24][0],add_x16(r,S));
    store_x16(&f[i+24][16],R);
    store_x16(&f[i+24][32],neg_x16(t));
    store_x16(&f[i+24][48],neg_x16(add_x16(s,T)));

    store_x16(&f[i+32][0],squeeze13_x16(add_x16(rt,s)));
    store_x16(&f[i+32][16],squeeze13_x16(add_x16(RT,S)));
    store_x16(&f[i+32][32],const_x16(0));
    store_x16(&f[i+32][48],const_x16(0));

    store_x16(&f[i+40][0],sub_x16(rt,s));
    store_x16(&f[i+40][16],sub_x16(RT,S));
    store_x16(&f[i+40][32],const_x16(0));
    store_x16(&f[i+40][48],const_x16(0));
  }

  assertrange48_64(f,32000);

  for (i = 0;i < 32;i += 8)
    for (j = 0;j < 64;j += 16)
      *(int16x16 *) &f[i][j] = squeeze13_x16(*(int16x16 *) &f[i][j]);

  for (i = 40;i <= 40;++i)
    for (j = 0;j < 64;j += 16)
      *(int16x16 *) &f[i][j] = squeeze13_x16(*(int16x16 *) &f[i][j]);

  twist64_2(f);
  fft8_64(f);

  untwist64_6(f+8);
  fft8_64(f+8);

  twist64_6(f+16);
  fft8_64(f+16);

  untwist64_2(f+24);
  fft8_64(f+24);

  twist64_8(f+40);
  fft8_64(f+40);

  fft8_64(f+32);

  assertrange48_64(f,4000);
}

/* input range: +-4000 */
/* output range: +-16000 */
void unfft48_64_scale64(int16 f[48][64])
{
  int i,j;

  assertrange48_64(f,4000);

  unfft8_64_scale8(f);
  untwist64_2(f);

  unfft8_64_scale8(f+8);
  twist64_6(f+8);

  unfft8_64_scale8(f+16);
  untwist64_6(f+16);

  unfft8_64_scale8(f+24);
  twist64_2(f+24);

  unfft8_64_scale8(f+32);

  unfft8_64_scale8(f+40);
  untwist64_8(f+40);

  for (i = 0;i < 32;i += 8)
    for (j = 0;j < 64;j += 16)
      *(int16x16 *) &f[i][j] = squeeze13_x16(*(int16x16 *) &f[i][j]);
  for (i = 40;i < 48;i += 8)
    for (j = 0;j < 64;j += 16)
      *(int16x16 *) &f[i][j] = squeeze13_x16(*(int16x16 *) &f[i][j]);

  for (i = 0;i < 8;++i) {
    int16x16 a = load_x16(&f[i][0]);
    int16x16 A = load_x16(&f[i+8][0]);
    int16x16 r = add_x16(a,A);
    int16x16 y = sub_x16(A,a);
    int16x16 e = load_x16(&f[i+16][0]);
    int16x16 E = load_x16(&f[i+24][0]);
    int16x16 R = add_x16(e,E);
    int16x16 W = sub_x16(E,e);

    int16x16 g = load_x16(&f[i+16][0+32]);
    int16x16 G = load_x16(&f[i+24][0+32]);
    int16x16 T = add_x16(g,G);
    int16x16 Y = sub_x16(G,g);

    store_x16(&f[i][0],add_x16(r,R));
    store_x16(&f[i+16][0+32],sub_x16(R,r));

    int16x16 c = load_x16(&f[i][0+32]);
    int16x16 C = load_x16(&f[i+8][0+32]);
    int16x16 w = sub_x16(c,C);
    int16x16 t = add_x16(c,C);

    store_x16(&f[i][0+32],add_x16(t,T));
    store_x16(&f[i+16][0],sub_x16(t,T));

    int16x16 h = load_x16(&f[i+16][0+48]);
    int16x16 H = load_x16(&f[i+24][0+48]);
    int16x16 U = add_x16(h,H);
    int16x16 V = sub_x16(h,H);

    int16x16 b = load_x16(&f[i][0+16]);
    int16x16 B = load_x16(&f[i+8][0+16]);
    int16x16 s = add_x16(b,B);
    int16x16 v = sub_x16(b,B);

    store_x16(&f[i+8][0],add_x16(v,V));
    store_x16(&f[i+24][0+32],sub_x16(V,v));

    store_x16(&f[i+8][0+16],add_x16(w,W));
    store_x16(&f[i+24][0+48],sub_x16(W,w));

    int16x16 d = load_x16(&f[i][0+48]);
    int16x16 D = load_x16(&f[i+8][0+48]);
    int16x16 u = add_x16(d,D);
    int16x16 x = sub_x16(d,D);

    int16x16 z = load_x16(&f[i+16][0+16]);
    int16x16 F = load_x16(&f[i+24][0+16]);
    int16x16 S = add_x16(z,F);
    int16x16 X = sub_x16(F,z);

    store_x16(&f[i][0+48],add_x16(u,U));
    store_x16(&f[i+16][0+16],sub_x16(u,U));

    store_x16(&f[i+8][0+48],add_x16(y,Y));
    store_x16(&f[i+24][0+16],sub_x16(y,Y));

    store_x16(&f[i][0+16],add_x16(s,S));
    store_x16(&f[i+16][0+48],sub_x16(S,s));

    store_x16(&f[i+8][0+32],add_x16(x,X));
    store_x16(&f[i+24][0],sub_x16(x,X));
  }

  for (i = 0;i < 8;++i)
    for (j = 0;j < 64;j += 16) {
      int16x16 a = load_x16(&f[i][j]);
      int16x16 A = load_x16(&f[i+8][j]);
      int16x16 b = load_x16(&f[i+16][j]);
      int16x16 B = load_x16(&f[i+24][j]);
      int16x16 x = squeeze13_x16(load_x16(&f[i+32][j]));
      int16x16 X = load_x16(&f[i+40][j]);
      int16x16 c = add_x16(x,X);
      int16x16 C = sub_x16(x,X);
      c = squeeze13_x16(c);
      c = add_x16(c,c);
      b = squeeze13_x16(b);
      c = sub_x16(c,b);
      a = squeeze13_x16(a);
      store_x16(&f[i][j],add_x16(a,c));
      store_x16(&f[i+16][j],add_x16(b,b));
      store_x16(&f[i+32][j],sub_x16(c,a));
      C = squeeze13_x16(C);
      C = add_x16(C,C);
      B = squeeze13_x16(B);
      C = sub_x16(C,B);
      A = squeeze13_x16(A);
      store_x16(&f[i+8][j],add_x16(A,C));
      store_x16(&f[i+24][j],add_x16(B,B));
      store_x16(&f[i+40][j],sub_x16(C,A));
    }

  assertrange48_64(f,16000);
}

void transpose(int16x16 out[64],const int16 in[16][64])
{
  int loop;

  for (loop = 4;loop > 0;--loop) {
    int16x16 a0 = load_x16(&in[0][0]);
    int16x16 a1 = load_x16(&in[1][0]);
    int16x16 b0 = _mm256_unpacklo_epi16(a0,a1);
    int16x16 b1 = _mm256_unpackhi_epi16(a0,a1);
    int16x16 a2 = load_x16(&in[2][0]);
    int16x16 a3 = load_x16(&in[3][0]);
    int16x16 b2 = _mm256_unpacklo_epi16(a2,a3);
    int16x16 b3 = _mm256_unpackhi_epi16(a2,a3);
    int16x16 c0 = _mm256_unpacklo_epi32(b0,b2);
    int16x16 c2 = _mm256_unpackhi_epi32(b0,b2);
    int16x16 c1 = _mm256_unpacklo_epi32(b1,b3);
    int16x16 c3 = _mm256_unpackhi_epi32(b1,b3);
    int16x16 a4 = load_x16(&in[4][0]);
    int16x16 a5 = load_x16(&in[5][0]);
    int16x16 b4 = _mm256_unpacklo_epi16(a4,a5);
    int16x16 b5 = _mm256_unpackhi_epi16(a4,a5);
    int16x16 a6 = load_x16(&in[6][0]);
    int16x16 a7 = load_x16(&in[7][0]);
    int16x16 b6 = _mm256_unpacklo_epi16(a6,a7);
    int16x16 b7 = _mm256_unpackhi_epi16(a6,a7);
    int16x16 c4 = _mm256_unpacklo_epi32(b4,b6);
    int16x16 c6 = _mm256_unpackhi_epi32(b4,b6);
    int16x16 c5 = _mm256_unpacklo_epi32(b5,b7);
    int16x16 c7 = _mm256_unpackhi_epi32(b5,b7);
    int16x16 a8 = load_x16(&in[8][0]);
    int16x16 a9 = load_x16(&in[9][0]);
    int16x16 b8 = _mm256_unpacklo_epi16(a8,a9);
    int16x16 b9 = _mm256_unpackhi_epi16(a8,a9);
    int16x16 a10 = load_x16(&in[10][0]);
    int16x16 a11 = load_x16(&in[11][0]);
    int16x16 b10 = _mm256_unpacklo_epi16(a10,a11);
    int16x16 b11 = _mm256_unpackhi_epi16(a10,a11);
    int16x16 c8 = _mm256_unpacklo_epi32(b8,b10);
    int16x16 c10 = _mm256_unpackhi_epi32(b8,b10);
    int16x16 c9 = _mm256_unpacklo_epi32(b9,b11);
    int16x16 c11 = _mm256_unpackhi_epi32(b9,b11);
    int16x16 a12 = load_x16(&in[12][0]);
    int16x16 a13 = load_x16(&in[13][0]);
    int16x16 b12 = _mm256_unpacklo_epi16(a12,a13);
    int16x16 b13 = _mm256_unpackhi_epi16(a12,a13);
    int16x16 a14 = load_x16(&in[14][0]);
    int16x16 a15 = load_x16(&in[15][0]);
    int16x16 b14 = _mm256_unpacklo_epi16(a14,a15);
    int16x16 b15 = _mm256_unpackhi_epi16(a14,a15);
    int16x16 c12 = _mm256_unpacklo_epi32(b12,b14);
    int16x16 c14 = _mm256_unpackhi_epi32(b12,b14);
    int16x16 c13 = _mm256_unpacklo_epi32(b13,b15);
    int16x16 c15 = _mm256_unpackhi_epi32(b13,b15);

    in = (const int16 (*)[64]) &in[0][16];

    int16x16 d0 = _mm256_unpacklo_epi64(c0,c4);
    int16x16 d4 = _mm256_unpackhi_epi64(c0,c4);
    int16x16 d8 = _mm256_unpacklo_epi64(c8,c12);
    int16x16 d12 = _mm256_unpackhi_epi64(c8,c12);
    int16x16 e0 = _mm256_permute2x128_si256(d0,d8,0x20);
    int16x16 e8 = _mm256_permute2x128_si256(d0,d8,0x31);
    int16x16 e4 = _mm256_permute2x128_si256(d4,d12,0x20);
    int16x16 e12 = _mm256_permute2x128_si256(d4,d12,0x31);
    out[0] = e0;
    out[0+8] = e8;
    out[0+1] = e4;
    out[0+9] = e12;

    int16x16 d1 = _mm256_unpacklo_epi64(c1,c5);
    int16x16 d5 = _mm256_unpackhi_epi64(c1,c5);
    int16x16 d9 = _mm256_unpacklo_epi64(c9,c13);
    int16x16 d13 = _mm256_unpackhi_epi64(c9,c13);
    int16x16 e1 = _mm256_permute2x128_si256(d1,d9,0x20);
    int16x16 e9 = _mm256_permute2x128_si256(d1,d9,0x31);
    int16x16 e5 = _mm256_permute2x128_si256(d5,d13,0x20);
    int16x16 e13 = _mm256_permute2x128_si256(d5,d13,0x31);
    out[0+4] = e1;
    out[0+12] = e9;
    out[0+5] = e5;
    out[0+13] = e13;

    int16x16 d2 = _mm256_unpacklo_epi64(c2,c6);
    int16x16 d6 = _mm256_unpackhi_epi64(c2,c6);
    int16x16 d10 = _mm256_unpacklo_epi64(c10,c14);
    int16x16 d14 = _mm256_unpackhi_epi64(c10,c14);
    int16x16 e2 = _mm256_permute2x128_si256(d2,d10,0x20);
    int16x16 e10 = _mm256_permute2x128_si256(d2,d10,0x31);
    int16x16 e6 = _mm256_permute2x128_si256(d6,d14,0x20);
    int16x16 e14 = _mm256_permute2x128_si256(d6,d14,0x31);
    out[0+2] = e2;
    out[0+3] = e6;
    out[0+10] = e10;
    out[0+11] = e14;

    int16x16 d3 = _mm256_unpacklo_epi64(c3,c7);
    int16x16 d7 = _mm256_unpackhi_epi64(c3,c7);
    int16x16 d11 = _mm256_unpacklo_epi64(c11,c15);
    int16x16 d15 = _mm256_unpackhi_epi64(c11,c15);
    int16x16 e3 = _mm256_permute2x128_si256(d3,d11,0x20);
    int16x16 e11 = _mm256_permute2x128_si256(d3,d11,0x31);
    int16x16 e7 = _mm256_permute2x128_si256(d7,d15,0x20);
    int16x16 e15 = _mm256_permute2x128_si256(d7,d15,0x31);
    out[0+6] = e3;
    out[0+7] = e7;
    out[0+14] = e11;
    out[0+15] = e15;

    out += 16;
  }
}

void untranspose(int16 out[16][64],const int16x16 in[64])
{
  int loop;
  
  for (loop = 4;loop > 0;--loop) {
    int16x16 a0 = in[0];
    int16x16 a1 = in[1];
    int16x16 b0 = _mm256_unpacklo_epi16(a0,a1);
    int16x16 b1 = _mm256_unpackhi_epi16(a0,a1);
    int16x16 a2 = in[2];
    int16x16 a3 = in[3];
    int16x16 b2 = _mm256_unpacklo_epi16(a2,a3);
    int16x16 b3 = _mm256_unpackhi_epi16(a2,a3);
    int16x16 c0 = _mm256_unpacklo_epi32(b0,b2);
    int16x16 c2 = _mm256_unpackhi_epi32(b0,b2);
    int16x16 c1 = _mm256_unpacklo_epi32(b1,b3);
    int16x16 c3 = _mm256_unpackhi_epi32(b1,b3);
    int16x16 a4 = in[4];
    int16x16 a5 = in[5];
    int16x16 b4 = _mm256_unpacklo_epi16(a4,a5);
    int16x16 b5 = _mm256_unpackhi_epi16(a4,a5);
    int16x16 a6 = in[6];
    int16x16 a7 = in[7];
    int16x16 b6 = _mm256_unpacklo_epi16(a6,a7);
    int16x16 b7 = _mm256_unpackhi_epi16(a6,a7);
    int16x16 c4 = _mm256_unpacklo_epi32(b4,b6);
    int16x16 c6 = _mm256_unpackhi_epi32(b4,b6);
    int16x16 c5 = _mm256_unpacklo_epi32(b5,b7);
    int16x16 c7 = _mm256_unpackhi_epi32(b5,b7);
    int16x16 a8 = in[8];
    int16x16 a9 = in[9];
    int16x16 b8 = _mm256_unpacklo_epi16(a8,a9);
    int16x16 b9 = _mm256_unpackhi_epi16(a8,a9);
    int16x16 a10 = in[10];
    int16x16 a11 = in[11];
    int16x16 b10 = _mm256_unpacklo_epi16(a10,a11);
    int16x16 b11 = _mm256_unpackhi_epi16(a10,a11);
    int16x16 c8 = _mm256_unpacklo_epi32(b8,b10);
    int16x16 c10 = _mm256_unpackhi_epi32(b8,b10);
    int16x16 c9 = _mm256_unpacklo_epi32(b9,b11);
    int16x16 c11 = _mm256_unpackhi_epi32(b9,b11);
    int16x16 a12 = in[12];
    int16x16 a13 = in[13];
    int16x16 b12 = _mm256_unpacklo_epi16(a12,a13);
    int16x16 b13 = _mm256_unpackhi_epi16(a12,a13);
    int16x16 a14 = in[14];
    int16x16 a15 = in[15];
    int16x16 b14 = _mm256_unpacklo_epi16(a14,a15);
    int16x16 b15 = _mm256_unpackhi_epi16(a14,a15);
    int16x16 c12 = _mm256_unpacklo_epi32(b12,b14);
    int16x16 c14 = _mm256_unpackhi_epi32(b12,b14);
    int16x16 c13 = _mm256_unpacklo_epi32(b13,b15);
    int16x16 c15 = _mm256_unpackhi_epi32(b13,b15);

    in += 16;

    int16x16 d0 = _mm256_unpacklo_epi64(c0,c4);
    int16x16 d4 = _mm256_unpackhi_epi64(c0,c4);
    int16x16 d8 = _mm256_unpacklo_epi64(c8,c12);
    int16x16 d12 = _mm256_unpackhi_epi64(c8,c12);
    int16x16 e0 = _mm256_permute2x128_si256(d0,d8,0x20);
    int16x16 e4 = _mm256_permute2x128_si256(d4,d12,0x20);
    int16x16 e8 = _mm256_permute2x128_si256(d0,d8,0x31);
    int16x16 e12 = _mm256_permute2x128_si256(d4,d12,0x31);
    store_x16(&out[0][0],e0);
    store_x16(&out[1][0],e4);
    store_x16(&out[8][0],e8);
    store_x16(&out[9][0],e12);

    int16x16 d1 = _mm256_unpacklo_epi64(c1,c5);
    int16x16 d5 = _mm256_unpackhi_epi64(c1,c5);
    int16x16 d9 = _mm256_unpacklo_epi64(c9,c13);
    int16x16 d13 = _mm256_unpackhi_epi64(c9,c13);
    int16x16 e1 = _mm256_permute2x128_si256(d1,d9,0x20);
    int16x16 e5 = _mm256_permute2x128_si256(d5,d13,0x20);
    int16x16 e9 = _mm256_permute2x128_si256(d1,d9,0x31);
    int16x16 e13 = _mm256_permute2x128_si256(d5,d13,0x31);
    store_x16(&out[4][0],e1);
    store_x16(&out[5][0],e5);
    store_x16(&out[12][0],e9);
    store_x16(&out[13][0],e13);

    int16x16 d2 = _mm256_unpacklo_epi64(c2,c6);
    int16x16 d6 = _mm256_unpackhi_epi64(c2,c6);
    int16x16 d10 = _mm256_unpacklo_epi64(c10,c14);
    int16x16 d14 = _mm256_unpackhi_epi64(c10,c14);
    int16x16 e2 = _mm256_permute2x128_si256(d2,d10,0x20);
    int16x16 e6 = _mm256_permute2x128_si256(d6,d14,0x20);
    int16x16 e10 = _mm256_permute2x128_si256(d2,d10,0x31);
    int16x16 e14 = _mm256_permute2x128_si256(d6,d14,0x31);
    store_x16(&out[2][0],e2);
    store_x16(&out[3][0],e6);
    store_x16(&out[10][0],e10);
    store_x16(&out[11][0],e14);

    int16x16 d3 = _mm256_unpacklo_epi64(c3,c7);
    int16x16 d7 = _mm256_unpackhi_epi64(c3,c7);
    int16x16 d11 = _mm256_unpacklo_epi64(c11,c15);
    int16x16 d15 = _mm256_unpackhi_epi64(c11,c15);
    int16x16 e3 = _mm256_permute2x128_si256(d3,d11,0x20);
    int16x16 e7 = _mm256_permute2x128_si256(d7,d15,0x20);
    int16x16 e11 = _mm256_permute2x128_si256(d3,d11,0x31);
    int16x16 e15 = _mm256_permute2x128_si256(d7,d15,0x31);
    store_x16(&out[6][0],e3);
    store_x16(&out[7][0],e7);
    store_x16(&out[14][0],e11);
    store_x16(&out[15][0],e15);

    out = (int16 (*)[64]) &out[0][16];
  }
}

static inline void assertrange768(const int16 f[768],int limit)
{
  int i;
  for (i = 0;i < 768;++i) {
    assert(f[i] >= -limit);
    assert(f[i] <= limit);
  }
}

/* h = fg/64 in k[y] */
/* where f,g are 768-coeff polys */
/* input range: +-4000 */
/* output range: +-4000 */
void mult768_over64(int16 h[1536],const int16 f[768],const int16 g[768])
{
  /* strategy: truncated cyclic Schoenhage trick */
  /* map k[y]/(y^1536-y^1024+y^512-1) */
  /* to (k[z]/(z^48-z^32+z^16-1))[y]/(y^32-z) */
  /* lift to k[y][z]/(z^48-z^32+z^16-1) */
  /* map to (k[y]/(y^64+1))[z]/(z^48-z^32+z^16-1) */
  /* note that k[y]/(y^64+1) supports size-128 FFT */

  int i;
  ALIGNED int16 fpad[48][64]; /* sum fpad[i][j] z^i y^j */
  ALIGNED int16 gpad[48][64];
  int16x16 ftr[64];
  int16x16 gtr[64];
  int16x16 htr[64];

  assertrange768(f,4000);
  assertrange768(g,4000);

  fft48_64(fpad,f);
  fft48_64(gpad,g);

  assertrange48_64(fpad,4000);
  assertrange48_64(gpad,4000);

  for (i = 0;i < 48;i += 16) {
    transpose(ftr,fpad+i);
    transpose(gtr,gpad+i);
    mult64_nega_over4096_x16(htr,ftr,gtr);
    untranspose(hpad+i,htr);
  }

  assertrange48_64(hpad,4000);

  unfft48_64_scale64(hpad);

  assertrange48_64(hpad,16000);

  store_x16(&h[0],squeeze13_x16(load_x16(&hpad[0][0])));
  store_x16(&h[16],squeeze13_x16(load_x16(&hpad[0][16])));
  for (i = 0;i < 46;++i) {
    store_x16(&h[32*i+32],squeeze13_x16(add_x16(load_x16(&hpad[i][32]),load_x16(&hpad[i+1][0]))));
    store_x16(&h[32*i+48],squeeze13_x16(add_x16(load_x16(&hpad[i][48]),load_x16(&hpad[i+1][16]))));
  }
  store_x16(&h[32*46+32],squeeze13_x16(load_x16(&hpad[46][32])));
  store_x16(&h[32*46+48],squeeze13_x16(load_x16(&hpad[46][48])));

  for (i = 0;i < 1536;++i) {
    assert(h[i] >= -4000); assert(h[i] <= 4000);
  }
}




#define v4591_16 _mm256_set1_epi16(4591)
#define v2295_16 _mm256_set1_epi16(2295)

static inline __m256i center_adjust(__m256i x) {
  __m256i x_abs = _mm256_sign_epi16(x,x);
  __m256i p_pm = _mm256_sign_epi16(v4591_16,x);
  __m256i mask = _mm256_cmpgt_epi16(x_abs,v2295_16);
  __m256i diff = _mm256_and_si256(p_pm,mask);
  __m256i res = _mm256_sub_epi16(x,diff);
  return(res);
}


/* h = (fg + vr)/64 in k[y] */
/* where f,g are 768-coeff polys */
/* input range: +-4000 */
/* output range: +-4000 */
void mult768_over64_2(int16 h[1536],const int16 f[768],const int16 g[768],const int16 v[768],const int16 r[768])
{

  int i;
  ALIGNED int16 fpad[48][64]; /* sum fpad[i][j] z^i y^j */
  ALIGNED int16 gpad[48][64];
  int16x16 ftr[64];
  int16x16 gtr[64];
  ALIGNED int16 vpad[48][64]; /* sum fpad[i][j] z^i y^j */
  ALIGNED int16 rpad[48][64];
  int16x16 vtr[64];
  int16x16 rtr[64];

  int16x16 htr[64];
  int16x16 htr2[64];

  assertrange768(f,4000);
  assertrange768(g,4000);

  fft48_64(fpad,f);
  fft48_64(gpad,g);
  fft48_64(vpad,v);
  fft48_64(rpad,r);

  assertrange48_64(fpad,4000);
  assertrange48_64(gpad,4000);

  for (i = 0;i < 48;i += 16) {
    transpose(ftr,fpad+i);
    transpose(gtr,gpad+i);
    mult64_nega_over4096_x16(htr,ftr,gtr);

    transpose(vtr,vpad+i);
    transpose(rtr,rpad+i);
    mult64_nega_over4096_x16(htr2,vtr,rtr);

    for (int j = 0;j < 64;j ++ ) {
      htr[j] = center_adjust( add_x16(htr[j],htr2[j]) );
    }

    untranspose(hpad+i,htr);
  }

  assertrange48_64(hpad,4000);

  unfft48_64_scale64(hpad);

  assertrange48_64(hpad,16000);

  store_x16(&h[0],squeeze13_x16(load_x16(&hpad[0][0])));
  store_x16(&h[16],squeeze13_x16(load_x16(&hpad[0][16])));
  for (i = 0;i < 46;++i) {
    store_x16(&h[32*i+32],squeeze13_x16(add_x16(load_x16(&hpad[i][32]),load_x16(&hpad[i+1][0]))));
    store_x16(&h[32*i+48],squeeze13_x16(add_x16(load_x16(&hpad[i][48]),load_x16(&hpad[i+1][16]))));
  }
  store_x16(&h[32*46+32],squeeze13_x16(load_x16(&hpad[46][32])));
  store_x16(&h[32*46+48],squeeze13_x16(load_x16(&hpad[46][48])));

  for (i = 0;i < 1536;++i) {
    assert(h[i] >= -4000); assert(h[i] <= 4000);
  }
}



