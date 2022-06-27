#include "crypto_core.h"
#include "ntt.h"

/* auto-generated; do not edit */

#include <stdint.h>
#include <immintrin.h>

typedef int8_t int8;
typedef int16_t int16;

#define zeta(n,i) (i[(__m256i *) zeta_##n])
#define zeta_x4(n,i) (i[(__m256i *) zeta_x4_##n])
#define zeta_qinv(n,i) (i[(__m256i *) qinvzeta_##n])
#define zeta_x4_qinv(n,i) (i[(__m256i *) qinvzeta_x4_##n])
#define zetainv(n,i) _mm256_loadu_reverse16((__m256i *) ((int16 *) zeta_##n+n/2+1-16*(i+1)))
#define zetainv_x4(n,i) _mm256_loadu_reverse16((__m256i *) ((int16 *) zeta_x4_##n+2*n+4-16*(i+1)))
#define zetainv_qinv(n,i) _mm256_loadu_reverse16((__m256i *) ((int16 *) qinvzeta_##n+n/2+1-16*(i+1)))
#define zetainv_x4_qinv(n,i) _mm256_loadu_reverse16((__m256i *) ((int16 *) qinvzeta_x4_##n+2*n+4-16*(i+1)))

static const __attribute((aligned(32))) int16 qdata_7681[] = {

#define q_x16 (qdata[0])
  7681,7681,7681,7681,7681,7681,7681,7681,7681,7681,7681,7681,7681,7681,7681,7681,

#define qrecip_x16 (qdata[1])
  17474,17474,17474,17474,17474,17474,17474,17474,17474,17474,17474,17474,17474,17474,17474,17474,

#define qshift_x16 (qdata[2])
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,

#define zeta4_x16 (qdata[3])
  -3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,-3777,

#define zeta4_x16_qinv (qdata[4])
  -28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,-28865,

#define zeta8_x16 (qdata[5])
  -3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,-3625,

#define zeta8_x16_qinv (qdata[6])
  -16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,-16425,

#define zetainv8_x16 (qdata[7])
  -3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,-3182,

#define zetainv8_x16_qinv (qdata[8])
  -10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,-10350,

#define zeta_x4_16 (qdata+9)
  -3593,-3593,-3593,-3593,-2194,-2194,-2194,-2194,-3625,-3625,-3625,-3625,1100,1100,1100,1100,
  -3777,-3777,-3777,-3777,-2456,-2456,-2456,-2456,3182,3182,3182,3182,3696,3696,3696,3696,
  3593,3593,3593,3593,0,0,0,0,0,0,0,0,0,0,0,0,

#define qinvzeta_x4_16 (qdata+12)
  -9,-9,-9,-9,4974,4974,4974,4974,-16425,-16425,-16425,-16425,7244,7244,7244,7244,
  -28865,-28865,-28865,-28865,-14744,-14744,-14744,-14744,10350,10350,10350,10350,-4496,-4496,-4496,-4496,
  9,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,

#define zeta_x4_32 (qdata+15)
  -3593,-3593,-3593,-3593,1414,1414,1414,1414,-2194,-2194,-2194,-2194,-2495,-2495,-2495,-2495,
  -3625,-3625,-3625,-3625,2876,2876,2876,2876,1100,1100,1100,1100,-2250,-2250,-2250,-2250,
  -3777,-3777,-3777,-3777,-1701,-1701,-1701,-1701,-2456,-2456,-2456,-2456,834,834,834,834,
  3182,3182,3182,3182,-2319,-2319,-2319,-2319,3696,3696,3696,3696,121,121,121,121,
  3593,3593,3593,3593,0,0,0,0,0,0,0,0,0,0,0,0,

#define qinvzeta_x4_32 (qdata+20)
  -9,-9,-9,-9,20870,20870,20870,20870,4974,4974,4974,4974,22593,22593,22593,22593,
  -16425,-16425,-16425,-16425,828,828,828,828,7244,7244,7244,7244,-23754,-23754,-23754,-23754,
  -28865,-28865,-28865,-28865,20315,20315,20315,20315,-14744,-14744,-14744,-14744,18242,18242,18242,18242,
  10350,10350,10350,10350,-18191,-18191,-18191,-18191,-4496,-4496,-4496,-4496,-11655,-11655,-11655,-11655,
  9,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,

#define zeta_64 (qdata+25)
  -3593,-617,1414,3706,-2194,-1296,-2495,-2237,-3625,2830,2876,-1599,1100,1525,-2250,2816,
  -3777,1921,-1701,2006,-2456,1483,834,-1986,3182,3364,-2319,-1993,3696,-2557,121,2088,
  3593,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

#define qinvzeta_64 (qdata+28)
  -9,19351,20870,-15750,4974,-9488,22593,7491,-16425,26382,828,23489,7244,20469,-23754,2816,
  -28865,-5759,20315,-3114,-14744,15307,18242,-19394,10350,-10972,-18191,-31177,-4496,-25597,-11655,22568,
  9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

#define zeta_128 (qdata+31)
  -3593,-2804,-617,-396,1414,-549,3706,810,-2194,-1321,-1296,438,-2495,-2535,-2237,-3689,
  -3625,2043,2830,-1881,2876,3153,-1599,7,1100,-514,1525,-1760,-2250,-2440,2816,3600,
  -3777,103,1921,-3174,-1701,1535,2006,-1887,-2456,1399,1483,-679,834,3772,-1986,1738,
  3182,-1431,3364,-3555,-2319,-2310,-1993,638,3696,-2956,-2557,-1305,121,2555,2088,-3266,
  3593,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

#define qinvzeta_128 (qdata+36)
  -9,-29428,19351,26228,20870,21467,-15750,5930,4974,-14121,-9488,-21066,22593,2073,7491,16279,
  -16425,-25093,26382,26279,828,-29103,23489,11783,7244,14846,20469,14624,-23754,-6536,2816,11792,
  -28865,-4505,-5759,-6246,20315,9215,-3114,6817,-14744,4983,15307,-28839,18242,1724,-19394,23242,
  10350,-21399,-10972,-29667,-18191,-21766,-31177,15998,-4496,23668,-25597,-5913,-11655,-24581,22568,-20674,
  9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

#define zeta_256 (qdata+41)
  -3593,2665,-2804,-2572,-617,727,-396,3417,1414,2579,-549,373,3706,3750,810,-1054,
  -2194,-2133,-1321,1681,-1296,-1386,438,-2732,-2495,1919,-2535,-2391,-2237,2835,-3689,2,
  -3625,-783,2043,3145,2830,1533,-1881,2789,2876,2649,3153,3692,-1599,-1390,7,-1166,
  1100,3310,-514,2224,1525,-2743,-1760,2385,-2250,-486,-2440,-1756,2816,-3816,3600,-3831,
  -3777,-1799,103,1497,1921,1521,-3174,-194,-1701,-859,1535,2175,2006,-2762,-1887,-1698,
  -2456,-3480,1399,2883,1483,-3428,-679,-2113,834,1532,3772,-660,-1986,-2764,1738,-915,
  3182,1056,-1431,1350,3364,1464,-3555,2919,-2319,-2160,-2310,730,-1993,-1598,638,3456,
  3696,-1168,-2956,-3588,-2557,-921,-1305,3405,121,-404,2555,-3135,2088,2233,-3266,-2426,
  3593,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

#define qinvzeta_256 (qdata+50)
  -9,-17303,-29428,24052,19351,-12073,26228,-24743,20870,-12269,21467,19317,-15750,-25946,5930,32738,
  4974,-4693,-14121,2193,-9488,26262,-21066,7508,22593,9599,2073,10409,7491,-12013,16279,-15358,
  -16425,-16655,-25093,32329,26382,24573,26279,13541,828,-25511,-29103,26220,23489,-8558,11783,-24718,
  7244,10478,14846,26800,20469,26441,14624,-29871,-23754,-3558,-6536,-16092,2816,8472,11792,-7415,
  -28865,-13575,-4505,-26663,-5759,-14351,-6246,-17602,20315,-22875,9215,9855,-3114,-24266,6817,-2722,
  -14744,-15768,4983,12611,15307,-21860,-28839,-27201,18242,32252,1724,21868,-19394,-8908,23242,13933,
  10350,17440,-21399,-11962,-10972,30136,-29667,-1689,-18191,6032,-21766,30426,-31177,15810,15998,3456,
  -4496,-9360,23668,27132,-25597,-5529,-5913,1869,-11655,22124,-24581,21953,22568,23225,-20674,17030,
  9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

#define zeta_512 (qdata+59)
  -3593,2005,2665,2891,-2804,2345,-2572,1121,-617,-188,727,2786,-396,-3208,3417,-17,
  1414,-3752,2579,2815,-549,1837,373,151,3706,-1012,3750,-1509,810,-3214,-1054,3177,
  -2194,-1403,-2133,-3314,-1321,83,1681,-658,-1296,2070,-1386,-3547,438,3781,-2732,2230,
  -2495,-1669,1919,2589,-2535,-3312,-2391,-3542,-2237,-1441,2835,-3568,-3689,-402,2,-1070,
  -3625,3763,-783,-3550,2043,-2303,3145,-436,2830,-893,1533,1712,-1881,124,2789,-2001,
  2876,-2460,2649,3770,3153,2965,3692,-1203,-1599,2874,-1390,-1407,7,-3745,-1166,1649,
  1100,2937,3310,3461,-514,-1526,2224,715,1525,-1689,-2743,434,-1760,-3163,2385,-929,
  -2250,-2167,-486,-1144,-2440,-370,-1756,2378,2816,-1084,-3816,-1586,3600,1931,-3831,-1242,
  -3777,592,-1799,2340,103,-1338,1497,-2071,1921,1519,1521,451,-3174,589,-194,-3744,
  -1701,3677,-859,-1295,1535,642,2175,-3794,2006,2130,-2762,2918,-1887,3334,-1698,2072,
  -2456,509,-3480,2998,1399,-3408,2883,1476,1483,-2262,-3428,-1779,-679,2258,-2113,1348,
  834,-692,1532,2247,3772,2083,-660,-226,-1986,2532,-2764,-3693,1738,-429,-915,-2059,
  3182,2812,1056,3434,-1431,-2515,1350,-236,3364,-2386,1464,222,-3555,-2963,2919,-2422,
  -2319,-3657,-2160,3450,-2310,-791,730,1181,-1993,-1404,-1598,2339,638,-3366,3456,2161,
  3696,-3343,-1168,2719,-2956,-826,-3588,-670,-2557,777,-921,1151,-1305,-796,3405,-1278,
  121,-3287,-404,1072,2555,293,-3135,2767,2088,-3335,2233,3581,-3266,3723,-2426,-179,
  3593,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

#define qinvzeta_512 (qdata+76)
  -9,4565,-17303,16715,-29428,15145,24052,-22943,19351,1860,-12073,-28958,26228,-7304,-24743,-529,
  20870,-24232,-12269,10495,21467,-16083,19317,20119,-15750,-27636,-25946,-12261,5930,-26766,32738,-16791,
  4974,25733,-4693,20238,-14121,18003,2193,6510,-9488,29718,26262,-25563,-21066,-1851,7508,-19274,
  22593,-28805,9599,-23523,2073,4880,10409,1578,7491,-10145,-12013,4624,16279,6766,-15358,24530,
  -16425,5299,-16655,-2526,-25093,-9983,32329,5708,26382,-23933,24573,26288,26279,30844,13541,30255,
  828,15972,-25511,17082,-29103,-27243,26220,-2739,23489,16186,-8558,-9087,11783,-12449,-24718,-14223,
  7244,-8839,10478,30597,14846,-12790,26800,14539,20469,-6297,26441,9650,14624,-25179,-29871,-9633,
  -23754,-5751,-3558,2952,-6536,23182,-16092,23882,2816,964,8472,-10802,11792,-17013,-7415,-30938,
  -28865,-23984,-13575,-11996,-4505,-14650,-26663,-22039,-5759,1007,-14351,10179,-6246,-947,-17602,-20128,
  20315,10333,-22875,-17167,9215,-14718,9855,-29394,-3114,27730,-24266,5990,6817,22790,-2722,14360,
  -14744,23549,-15768,-18506,4983,21168,12611,3524,15307,2858,-21860,29453,-28839,27858,-27201,3396,
  18242,5452,32252,-18745,1724,-4573,21868,31518,-19394,20964,-8908,-18541,23242,17491,13933,16885,
  10350,-32004,17440,-24214,-21399,-20435,-11962,-22764,-10972,-27986,30136,-802,-29667,11885,-1689,-13686,
  -18191,32695,6032,-16006,-21766,-20759,30426,-24931,-31177,-32124,15810,-4317,15998,26330,3456,-13711,
  -4496,-19215,-9360,26783,23668,-14138,27132,-32414,-25597,-2807,-5529,8831,-5913,17636,1869,-16638,
  -11655,9513,22124,25648,-24581,-21723,21953,-14129,22568,-15111,23225,26621,-20674,-15221,17030,-1715,
  9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

static inline __m256i sub_x16(__m256i a,__m256i b)
{
  __asm__("vpsubw %1,%0,%0" : "+x"(a),"+x"(b));
  return a;
}

static inline __m256i add_x16(__m256i a,__m256i b)
{
  return _mm256_add_epi16(a,b);
}

static inline __m256i reduce_x16(const __m256i *qdata,__m256i x)
{
  __m256i y = _mm256_mulhi_epi16(x,qrecip_x16);
  y = _mm256_mulhrs_epi16(y,qshift_x16);
  y = _mm256_mullo_epi16(y,q_x16);
  return sub_x16(x,y);
}

static inline __m256i mulmod_x16_scaled(const __m256i *qdata,__m256i x,__m256i y,__m256i yqinv)
{
  __m256i b = _mm256_mulhi_epi16(x,y);
  __m256i d = _mm256_mullo_epi16(x,yqinv);
  __m256i e = _mm256_mulhi_epi16(d,q_x16);
  return sub_x16(b,e);
}

static __attribute((aligned(32))) int8 shuffle_buf[32] = {
  14,15,12,13,10,11,8,9,6,7,4,5,2,3,0,1,
  14,15,12,13,10,11,8,9,6,7,4,5,2,3,0,1,
};
#define shuffle (*(__m256i *) shuffle_buf)

static inline __m256i _mm256_loadu_reverse16(const __m256i *p)
{
  __m256i x = _mm256_loadu_si256(p);
  x = _mm256_permute2x128_si256(x,x,1);
  x = _mm256_shuffle_epi8(x,shuffle);
  return x;
}

static void ntt128(int16 *f,int reps,const __m256i *qdata)
{
  __m256i f0,f1,f2,f3,g0,g1,g2,g3,h0,h1,h2,h3;
  int16 *origf = f;
  int rep;
  __m256i zetainv_128_0 = zetainv(128,0);
  __m256i zetainv_qinv_128_0 = zetainv_qinv(128,0);
  __m256i zetainv_x4_32_0 = zetainv_x4(32,0);
  __m256i zetainv_x4_qinv_32_0 = zetainv_x4_qinv(32,0);
  __m256i zetainv_128_1 = zetainv(128,1);
  __m256i zetainv_qinv_128_1 = zetainv_qinv(128,1);
  __m256i zetainv_x4_32_1 = zetainv_x4(32,1);
  __m256i zetainv_x4_qinv_32_1 = zetainv_x4_qinv(32,1);
  for (rep = 0;rep < reps;++rep) {
    f1 = _mm256_loadu_si256((__m256i *) (f + 32));
    f3 = _mm256_loadu_si256((__m256i *) (f + 96));
    g3 = sub_x16(f1,f3);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f1,f3);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 0));
    f2 = _mm256_loadu_si256((__m256i *) (f + 64));
    g2 = sub_x16(f0,f2);
    g0 = add_x16(f0,f2);
    
    f3 = sub_x16(g3,g2);
    f2 = add_x16(g2,g3);
    f3 = mulmod_x16_scaled(qdata,f3,zetainv_128_0,zetainv_qinv_128_0);
    f2 = mulmod_x16_scaled(qdata,f2,zeta(128,0),zeta_qinv(128,0));
    
    g2 = _mm256_unpacklo_epi16(f2,f3);
    g3 = _mm256_unpackhi_epi16(f2,f3);
    
    f1 = sub_x16(g0,g1);
    f0 = add_x16(g0,g1);
    f1 = mulmod_x16_scaled(qdata,f1,zeta(64,0),zeta_qinv(64,0));
    f0 = reduce_x16(qdata,f0);
    
    g0 = _mm256_unpacklo_epi16(f0,f1);
    h0 = _mm256_unpacklo_epi32(g0,g2);
    h1 = _mm256_unpackhi_epi32(g0,g2);
    g1 = _mm256_unpackhi_epi16(f0,f1);
    h2 = _mm256_unpacklo_epi32(g1,g3);
    h3 = _mm256_unpackhi_epi32(g1,g3);
    f0 = _mm256_permute2x128_si256(h0,h1,0x20);
    f2 = _mm256_permute2x128_si256(h0,h1,0x31);
    f1 = _mm256_permute2x128_si256(h2,h3,0x20);
    f3 = _mm256_permute2x128_si256(h2,h3,0x31);
    
    _mm256_storeu_si256((__m256i *) (f + 0),f0);
    _mm256_storeu_si256((__m256i *) (f + 64),f2);
    _mm256_storeu_si256((__m256i *) (f + 32),f1);
    _mm256_storeu_si256((__m256i *) (f + 96),f3);
    
    f1 = _mm256_loadu_si256((__m256i *) (f + 48));
    f3 = _mm256_loadu_si256((__m256i *) (f + 112));
    g3 = sub_x16(f1,f3);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f1,f3);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 16));
    f2 = _mm256_loadu_si256((__m256i *) (f + 80));
    g2 = sub_x16(f0,f2);
    g0 = add_x16(f0,f2);
    
    f3 = sub_x16(g3,g2);
    f2 = add_x16(g2,g3);
    f3 = mulmod_x16_scaled(qdata,f3,zetainv_128_1,zetainv_qinv_128_1);
    f2 = mulmod_x16_scaled(qdata,f2,zeta(128,1),zeta_qinv(128,1));
    
    g2 = _mm256_unpacklo_epi16(f2,f3);
    g3 = _mm256_unpackhi_epi16(f2,f3);
    
    f1 = sub_x16(g0,g1);
    f0 = add_x16(g0,g1);
    f1 = mulmod_x16_scaled(qdata,f1,zeta(64,1),zeta_qinv(64,1));
    f0 = reduce_x16(qdata,f0);
    
    g0 = _mm256_unpacklo_epi16(f0,f1);
    h0 = _mm256_unpacklo_epi32(g0,g2);
    h1 = _mm256_unpackhi_epi32(g0,g2);
    g1 = _mm256_unpackhi_epi16(f0,f1);
    h2 = _mm256_unpacklo_epi32(g1,g3);
    h3 = _mm256_unpackhi_epi32(g1,g3);
    f0 = _mm256_permute2x128_si256(h0,h1,0x20);
    f2 = _mm256_permute2x128_si256(h0,h1,0x31);
    f1 = _mm256_permute2x128_si256(h2,h3,0x20);
    f3 = _mm256_permute2x128_si256(h2,h3,0x31);
    
    _mm256_storeu_si256((__m256i *) (f + 16),f0);
    _mm256_storeu_si256((__m256i *) (f + 80),f2);
    _mm256_storeu_si256((__m256i *) (f + 48),f1);
    _mm256_storeu_si256((__m256i *) (f + 112),f3);
    
    f += 128;
  }
  f = origf;
  for (rep = 0;rep < reps;++rep) {
    f1 = _mm256_loadu_si256((__m256i *) (f + 64));
    f3 = _mm256_loadu_si256((__m256i *) (f + 80));
    g3 = sub_x16(f1,f3);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f1,f3);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 0));
    f2 = _mm256_loadu_si256((__m256i *) (f + 16));
    g2 = sub_x16(f0,f2);
    g0 = add_x16(f0,f2);
    
    f3 = sub_x16(g3,g2);
    f2 = add_x16(g2,g3);
    f3 = mulmod_x16_scaled(qdata,f3,zetainv_x4_32_0,zetainv_x4_qinv_32_0);
    f2 = mulmod_x16_scaled(qdata,f2,zeta_x4(32,0),zeta_x4_qinv(32,0));
    
    g2 = _mm256_unpacklo_epi64(f2,f3);
    g3 = _mm256_unpackhi_epi64(f2,f3);
    
    f1 = sub_x16(g0,g1);
    f0 = add_x16(g0,g1);
    f1 = mulmod_x16_scaled(qdata,f1,zeta_x4(16,0),zeta_x4_qinv(16,0));
    f0 = reduce_x16(qdata,f0);
    
    g1 = _mm256_unpackhi_epi64(f0,f1);
    g0 = _mm256_unpacklo_epi64(f0,f1);
    f1 = _mm256_permute2x128_si256(g1,g3,0x20);
    f3 = _mm256_permute2x128_si256(g1,g3,0x31);
    f0 = _mm256_permute2x128_si256(g0,g2,0x20);
    f2 = _mm256_permute2x128_si256(g0,g2,0x31);
    
    _mm256_storeu_si256((__m256i *) (f + 64),f1);
    _mm256_storeu_si256((__m256i *) (f + 80),f3);
    _mm256_storeu_si256((__m256i *) (f + 0),f0);
    _mm256_storeu_si256((__m256i *) (f + 16),f2);
    
    f1 = _mm256_loadu_si256((__m256i *) (f + 96));
    f3 = _mm256_loadu_si256((__m256i *) (f + 112));
    g3 = sub_x16(f1,f3);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f1,f3);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 32));
    f2 = _mm256_loadu_si256((__m256i *) (f + 48));
    g2 = sub_x16(f0,f2);
    g0 = add_x16(f0,f2);
    
    f3 = sub_x16(g3,g2);
    f2 = add_x16(g2,g3);
    f3 = mulmod_x16_scaled(qdata,f3,zetainv_x4_32_1,zetainv_x4_qinv_32_1);
    f2 = mulmod_x16_scaled(qdata,f2,zeta_x4(32,1),zeta_x4_qinv(32,1));
    
    g2 = _mm256_unpacklo_epi64(f2,f3);
    g3 = _mm256_unpackhi_epi64(f2,f3);
    
    f1 = sub_x16(g0,g1);
    f0 = add_x16(g0,g1);
    f1 = mulmod_x16_scaled(qdata,f1,zeta_x4(16,1),zeta_x4_qinv(16,1));
    f0 = reduce_x16(qdata,f0);
    
    g1 = _mm256_unpackhi_epi64(f0,f1);
    g0 = _mm256_unpacklo_epi64(f0,f1);
    f1 = _mm256_permute2x128_si256(g1,g3,0x20);
    f3 = _mm256_permute2x128_si256(g1,g3,0x31);
    f0 = _mm256_permute2x128_si256(g0,g2,0x20);
    f2 = _mm256_permute2x128_si256(g0,g2,0x31);
    
    _mm256_storeu_si256((__m256i *) (f + 96),f1);
    _mm256_storeu_si256((__m256i *) (f + 112),f3);
    _mm256_storeu_si256((__m256i *) (f + 32),f0);
    _mm256_storeu_si256((__m256i *) (f + 48),f2);
    
    f += 128;
  }
  f = origf;
  for (rep = 0;rep < reps;++rep) {
    
    f1 = _mm256_loadu_si256((__m256i *) (f + 16));
    f3 = _mm256_loadu_si256((__m256i *) (f + 48));
    g3 = sub_x16(f1,f3);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f1,f3);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 0));
    f2 = _mm256_loadu_si256((__m256i *) (f + 32));
    g2 = sub_x16(f0,f2);
    g0 = add_x16(f0,f2);
    
    f2 = add_x16(g2,g3);
    f3 = sub_x16(g2,g3);
    f2 = reduce_x16(qdata,f2);
    f3 = reduce_x16(qdata,f3);
    
    f1 = sub_x16(g0,g1);
    f0 = add_x16(g0,g1);
    f0 = reduce_x16(qdata,f0);
    
    h0 = f0;
    h1 = f1;
    h2 = f2;
    h3 = f3;
    
    f1 = _mm256_loadu_si256((__m256i *) (f + 80));
    f3 = _mm256_loadu_si256((__m256i *) (f + 112));
    g3 = sub_x16(f1,f3);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f1,f3);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 64));
    f2 = _mm256_loadu_si256((__m256i *) (f + 96));
    g2 = sub_x16(f0,f2);
    g0 = add_x16(f0,f2);
    
    f3 = sub_x16(g3,g2);
    f2 = add_x16(g2,g3);
    f3 = mulmod_x16_scaled(qdata,f3,zetainv8_x16,zetainv8_x16_qinv);
    f2 = mulmod_x16_scaled(qdata,f2,zeta8_x16,zeta8_x16_qinv);
    
    f1 = sub_x16(g0,g1);
    f0 = add_x16(g0,g1);
    f1 = mulmod_x16_scaled(qdata,f1,zeta4_x16,zeta4_x16_qinv);
    f0 = reduce_x16(qdata,f0);
    
    g0 = add_x16(h0,f0);
    g1 = add_x16(h1,f1);
    g2 = add_x16(h2,f2);
    g3 = add_x16(h3,f3);
    _mm256_storeu_si256((__m256i *) (f + 0),g0);
    _mm256_storeu_si256((__m256i *) (f + 16),g1);
    _mm256_storeu_si256((__m256i *) (f + 32),g2);
    _mm256_storeu_si256((__m256i *) (f + 48),g3);
    g0 = sub_x16(h0,f0);
    g1 = sub_x16(h1,f1);
    g2 = sub_x16(h2,f2);
    g3 = sub_x16(h3,f3);
    _mm256_storeu_si256((__m256i *) (f + 64),g0);
    _mm256_storeu_si256((__m256i *) (f + 80),g1);
    _mm256_storeu_si256((__m256i *) (f + 96),g2);
    _mm256_storeu_si256((__m256i *) (f + 112),g3);
    f += 128;
  }
}

static void ntt512(int16 *f,int reps,const __m256i *qdata)
{
  __m256i f0,f1,f2,f3,g0,g1,g2,g3,h0,h1,h2,h3;
  int16 *origf = f;
  int rep;
  __m256i zetainv_512[8];
  __m256i zetainv_qinv_512[8];
  int i;
  for (i = 0;i < 8;++i) zetainv_512[i] = zetainv(512,i);
  for (i = 0;i < 8;++i) zetainv_qinv_512[i] = zetainv_qinv(512,i);
  for (rep = 0;rep < reps;++rep) {
    for (i = 0;i < 8;++i) {
      f1 = _mm256_loadu_si256((__m256i *) (f + 16*i + 128));
      f3 = _mm256_loadu_si256((__m256i *) (f + 16*i + 384));
      g3 = sub_x16(f1,f3);
      g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
      g1 = add_x16(f1,f3);
      
      f0 = _mm256_loadu_si256((__m256i *) (f + 16*i));
      f2 = _mm256_loadu_si256((__m256i *) (f + 16*i + 256));
      g2 = sub_x16(f0,f2);
      g0 = add_x16(f0,f2);
      
      f3 = sub_x16(g3,g2);
      f2 = add_x16(g2,g3);
      f3 = mulmod_x16_scaled(qdata,f3,zetainv_512[i],zetainv_qinv_512[i]);
      f2 = mulmod_x16_scaled(qdata,f2,zeta(512,i),zeta_qinv(512,i));
      
      f1 = sub_x16(g0,g1);
      f0 = add_x16(g0,g1);
      f1 = mulmod_x16_scaled(qdata,f1,zeta(256,i),zeta_qinv(256,i));
      f0 = reduce_x16(qdata,f0);
      
      _mm256_storeu_si256((__m256i *) (f + 16*i + 384),f3);
      _mm256_storeu_si256((__m256i *) (f + 16*i + 256),f2);
      _mm256_storeu_si256((__m256i *) (f + 16*i + 128),f1);
      _mm256_storeu_si256((__m256i *) (f + 16*i),f0);
      
    }
    f += 512;
  }
  f = origf;
  ntt128(f,reps*4,qdata);
}

void ntt512_7681(int16 *f,int reps)
{
  ntt512(f,reps,(const __m256i *) qdata_7681);
}

static void invntt128(int16 *f,int reps,const __m256i *qdata)
{
  __m256i f0,f1,f2,f3,g0,g1,g2,g3,h0,h1,h2,h3;
  int16 *origf = f;
  int rep;
  __m256i zetainv_x4_16_0 = zetainv_x4(16,0);
  __m256i zetainv_x4_qinv_16_0 = zetainv_x4_qinv(16,0);
  __m256i zetainv_x4_32_0 = zetainv_x4(32,0);
  __m256i zetainv_x4_qinv_32_0 = zetainv_x4_qinv(32,0);
  __m256i zetainv_64_0 = zetainv(64,0);
  __m256i zetainv_qinv_64_0 = zetainv_qinv(64,0);
  __m256i zetainv_128_0 = zetainv(128,0);
  __m256i zetainv_qinv_128_0 = zetainv_qinv(128,0);
  __m256i zetainv_x4_16_1 = zetainv_x4(16,1);
  __m256i zetainv_x4_qinv_16_1 = zetainv_x4_qinv(16,1);
  __m256i zetainv_x4_32_1 = zetainv_x4(32,1);
  __m256i zetainv_x4_qinv_32_1 = zetainv_x4_qinv(32,1);
  __m256i zetainv_64_1 = zetainv(64,1);
  __m256i zetainv_qinv_64_1 = zetainv_qinv(64,1);
  __m256i zetainv_128_1 = zetainv(128,1);
  __m256i zetainv_qinv_128_1 = zetainv_qinv(128,1);
  for (rep = 0;rep < reps;++rep) {
    f0 = _mm256_loadu_si256((__m256i *) (f +   0));
    f1 = _mm256_loadu_si256((__m256i *) (f +  64));
    f2 = _mm256_loadu_si256((__m256i *) (f +  16));
    f3 = _mm256_loadu_si256((__m256i *) (f +  80));
    g0 = _mm256_loadu_si256((__m256i *) (f +  32));
    g1 = _mm256_loadu_si256((__m256i *) (f +  96));
    g2 = _mm256_loadu_si256((__m256i *) (f +  48));
    g3 = _mm256_loadu_si256((__m256i *) (f + 112));
    
    h1 = sub_x16(f0,f1);
    h1 = reduce_x16(qdata,h1);
    h0 = add_x16(f0,f1);
    h3 = sub_x16(f2,f3);
    h3 = mulmod_x16_scaled(qdata,h3,zeta4_x16,zeta4_x16_qinv);
    h2 = add_x16(f2,f3);
    f1 = sub_x16(g0,g1);
    f1 = mulmod_x16_scaled(qdata,f1,zetainv8_x16,zetainv8_x16_qinv);
    f0 = add_x16(g0,g1);
    f3 = sub_x16(g2,g3);
    f3 = mulmod_x16_scaled(qdata,f3,zeta8_x16,zeta8_x16_qinv);
    f2 = add_x16(g2,g3);
    
    g0 = add_x16(h0,h2);
    g0 = reduce_x16(qdata,g0);
    g2 = sub_x16(h0,h2);
    g2 = reduce_x16(qdata,g2);
    g1 = sub_x16(h1,h3);
    g3 = add_x16(h1,h3);
    h2 = sub_x16(f0,f2);
    h2 = mulmod_x16_scaled(qdata,h2,zeta4_x16,zeta4_x16_qinv);
    h0 = add_x16(f0,f2);
    h3 = add_x16(f1,f3);
    h3 = mulmod_x16_scaled(qdata,h3,zeta4_x16,zeta4_x16_qinv);
    h1 = sub_x16(f1,f3);
    
    f0 = add_x16(g0,h0);
    g0 = sub_x16(g0,h0);
    f1 = add_x16(g1,h1);
    g1 = sub_x16(g1,h1);
    f2 = sub_x16(g2,h2);
    g2 = add_x16(g2,h2);
    f3 = sub_x16(g3,h3);
    g3 = add_x16(g3,h3);
    
    _mm256_storeu_si256((__m256i *) (f +   0),f0);
    _mm256_storeu_si256((__m256i *) (f +  32),g0);
    _mm256_storeu_si256((__m256i *) (f +  64),f1);
    _mm256_storeu_si256((__m256i *) (f +  96),g1);
    _mm256_storeu_si256((__m256i *) (f +  16),f2);
    _mm256_storeu_si256((__m256i *) (f +  48),g2);
    _mm256_storeu_si256((__m256i *) (f +  80),f3);
    _mm256_storeu_si256((__m256i *) (f + 112),g3);
    
    f += 128;
  }
  f = origf;
  for (rep = 0;rep < reps;++rep) {
    f0 = _mm256_loadu_si256((__m256i *) (f + 0));
    f1 = _mm256_loadu_si256((__m256i *) (f + 64));
    f2 = _mm256_loadu_si256((__m256i *) (f + 16));
    f3 = _mm256_loadu_si256((__m256i *) (f + 80));
    
    g0 = _mm256_unpacklo_epi64(f0,f1);
    g1 = _mm256_unpacklo_epi64(f2,f3);
    g2 = _mm256_unpackhi_epi64(f0,f1);
    g3 = _mm256_unpackhi_epi64(f2,f3);
    f2 = _mm256_permute2x128_si256(g0,g1,0x31);
    f3 = _mm256_permute2x128_si256(g2,g3,0x31);
    f0 = _mm256_permute2x128_si256(g0,g1,0x20);
    f1 = _mm256_permute2x128_si256(g2,g3,0x20);
    
    f2 = mulmod_x16_scaled(qdata,f2,zetainv_x4_32_0,zetainv_x4_qinv_32_0);
    f3 = mulmod_x16_scaled(qdata,f3,zeta_x4(32,0),zeta_x4_qinv(32,0));
    
    g3 = add_x16(f3,f2);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g2 = sub_x16(f3,f2);
    
    f0 = reduce_x16(qdata,f0);
    f1 = mulmod_x16_scaled(qdata,f1,zetainv_x4_16_0,zetainv_x4_qinv_16_0);
    
    g1 = add_x16(f0,f1);
    g0 = sub_x16(f0,f1);
    
    f1 = add_x16(g1,g3);
    f3 = sub_x16(g1,g3);
    f0 = add_x16(g0,g2);
    f2 = sub_x16(g0,g2);
    
    _mm256_storeu_si256((__m256i *) (f + 64),f1);
    _mm256_storeu_si256((__m256i *) (f + 80),f3);
    _mm256_storeu_si256((__m256i *) (f + 0),f0);
    _mm256_storeu_si256((__m256i *) (f + 16),f2);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 32));
    f1 = _mm256_loadu_si256((__m256i *) (f + 96));
    f2 = _mm256_loadu_si256((__m256i *) (f + 48));
    f3 = _mm256_loadu_si256((__m256i *) (f + 112));
    
    g0 = _mm256_unpacklo_epi64(f0,f1);
    g1 = _mm256_unpacklo_epi64(f2,f3);
    g2 = _mm256_unpackhi_epi64(f0,f1);
    g3 = _mm256_unpackhi_epi64(f2,f3);
    f2 = _mm256_permute2x128_si256(g0,g1,0x31);
    f3 = _mm256_permute2x128_si256(g2,g3,0x31);
    f0 = _mm256_permute2x128_si256(g0,g1,0x20);
    f1 = _mm256_permute2x128_si256(g2,g3,0x20);
    
    f2 = mulmod_x16_scaled(qdata,f2,zetainv_x4_32_1,zetainv_x4_qinv_32_1);
    f3 = mulmod_x16_scaled(qdata,f3,zeta_x4(32,1),zeta_x4_qinv(32,1));
    
    g3 = add_x16(f3,f2);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g2 = sub_x16(f3,f2);
    
    f0 = reduce_x16(qdata,f0);
    f1 = mulmod_x16_scaled(qdata,f1,zetainv_x4_16_1,zetainv_x4_qinv_16_1);
    
    g1 = add_x16(f0,f1);
    g0 = sub_x16(f0,f1);
    
    f1 = add_x16(g1,g3);
    f3 = sub_x16(g1,g3);
    f0 = add_x16(g0,g2);
    f2 = sub_x16(g0,g2);
    
    _mm256_storeu_si256((__m256i *) (f + 96),f1);
    _mm256_storeu_si256((__m256i *) (f + 112),f3);
    _mm256_storeu_si256((__m256i *) (f + 32),f0);
    _mm256_storeu_si256((__m256i *) (f + 48),f2);
    
    f += 128;
  }
  f = origf;
  for (rep = 0;rep < reps;++rep) {
    f0 = _mm256_loadu_si256((__m256i *) (f + 0));
    f2 = _mm256_loadu_si256((__m256i *) (f + 64));
    f1 = _mm256_loadu_si256((__m256i *) (f + 32));
    f3 = _mm256_loadu_si256((__m256i *) (f + 96));
    
    g0 = _mm256_permute2x128_si256(f0,f2,0x20);
    g2 = _mm256_permute2x128_si256(f0,f2,0x31);
    f0 = _mm256_unpacklo_epi16(g0,g2);
    f2 = _mm256_unpackhi_epi16(g0,g2);
    g1 = _mm256_permute2x128_si256(f1,f3,0x20);
    g3 = _mm256_permute2x128_si256(f1,f3,0x31);
    f1 = _mm256_unpacklo_epi16(g1,g3);
    f3 = _mm256_unpackhi_epi16(g1,g3);
    g1 = _mm256_unpackhi_epi16(f0,f2);
    g0 = _mm256_unpacklo_epi16(f0,f2);
    g3 = _mm256_unpackhi_epi16(f1,f3);
    g2 = _mm256_unpacklo_epi16(f1,f3);
    f2 = _mm256_unpacklo_epi64(g1,g3);
    f3 = _mm256_unpackhi_epi64(g1,g3);
    f0 = _mm256_unpacklo_epi64(g0,g2);
    f1 = _mm256_unpackhi_epi64(g0,g2);
    
    f2 = mulmod_x16_scaled(qdata,f2,zetainv_128_0,zetainv_qinv_128_0);
    f3 = mulmod_x16_scaled(qdata,f3,zeta(128,0),zeta_qinv(128,0));
    f0 = reduce_x16(qdata,f0);
    f1 = mulmod_x16_scaled(qdata,f1,zetainv_64_0,zetainv_qinv_64_0);
    
    g3 = add_x16(f3,f2);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f0,f1);
    g2 = sub_x16(f3,f2);
    g0 = sub_x16(f0,f1);
    
    f1 = add_x16(g1,g3);
    f3 = sub_x16(g1,g3);
    f0 = add_x16(g0,g2);
    f2 = sub_x16(g0,g2);
    
    _mm256_storeu_si256((__m256i *) (f + 32),f1);
    _mm256_storeu_si256((__m256i *) (f + 96),f3);
    _mm256_storeu_si256((__m256i *) (f + 0),f0);
    _mm256_storeu_si256((__m256i *) (f + 64),f2);
    
    f0 = _mm256_loadu_si256((__m256i *) (f + 16));
    f2 = _mm256_loadu_si256((__m256i *) (f + 80));
    f1 = _mm256_loadu_si256((__m256i *) (f + 48));
    f3 = _mm256_loadu_si256((__m256i *) (f + 112));
    
    g0 = _mm256_permute2x128_si256(f0,f2,0x20);
    g2 = _mm256_permute2x128_si256(f0,f2,0x31);
    f0 = _mm256_unpacklo_epi16(g0,g2);
    f2 = _mm256_unpackhi_epi16(g0,g2);
    g1 = _mm256_permute2x128_si256(f1,f3,0x20);
    g3 = _mm256_permute2x128_si256(f1,f3,0x31);
    f1 = _mm256_unpacklo_epi16(g1,g3);
    f3 = _mm256_unpackhi_epi16(g1,g3);
    g1 = _mm256_unpackhi_epi16(f0,f2);
    g0 = _mm256_unpacklo_epi16(f0,f2);
    g3 = _mm256_unpackhi_epi16(f1,f3);
    g2 = _mm256_unpacklo_epi16(f1,f3);
    f2 = _mm256_unpacklo_epi64(g1,g3);
    f3 = _mm256_unpackhi_epi64(g1,g3);
    f0 = _mm256_unpacklo_epi64(g0,g2);
    f1 = _mm256_unpackhi_epi64(g0,g2);
    
    f2 = mulmod_x16_scaled(qdata,f2,zetainv_128_1,zetainv_qinv_128_1);
    f3 = mulmod_x16_scaled(qdata,f3,zeta(128,1),zeta_qinv(128,1));
    f0 = reduce_x16(qdata,f0);
    f1 = mulmod_x16_scaled(qdata,f1,zetainv_64_1,zetainv_qinv_64_1);
    
    g3 = add_x16(f3,f2);
    g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
    g1 = add_x16(f0,f1);
    g2 = sub_x16(f3,f2);
    g0 = sub_x16(f0,f1);
    
    f1 = add_x16(g1,g3);
    f3 = sub_x16(g1,g3);
    f0 = add_x16(g0,g2);
    f2 = sub_x16(g0,g2);
    
    _mm256_storeu_si256((__m256i *) (f + 48),f1);
    _mm256_storeu_si256((__m256i *) (f + 112),f3);
    _mm256_storeu_si256((__m256i *) (f + 16),f0);
    _mm256_storeu_si256((__m256i *) (f + 80),f2);
    
    f += 128;
  }
}

static void invntt512(int16 *f,int reps,const __m256i *qdata)
{
  __m256i f0,f1,f2,f3,g0,g1,g2,g3,h0,h1,h2,h3;
  int16 *origf = f;
  int rep;
  __m256i zetainv_512[8];
  __m256i zetainv_qinv_512[8];
  __m256i zetainv_256[8];
  __m256i zetainv_qinv_256[8];
  int i;
  for (i = 0;i < 8;++i) zetainv_512[i] = zetainv(512,i);
  for (i = 0;i < 8;++i) zetainv_qinv_512[i] = zetainv_qinv(512,i);
  for (i = 0;i < 8;++i) zetainv_256[i] = zetainv(256,i);
  for (i = 0;i < 8;++i) zetainv_qinv_256[i] = zetainv_qinv(256,i);
  invntt128(f,4*reps,qdata);
  for (rep = 0;rep < reps;++rep) {
    for(i = 0;i < 8;++i) {
      f2 = _mm256_loadu_si256((__m256i *) (f + 16*i + 256));
      f3 = _mm256_loadu_si256((__m256i *) (f + 16*i + 384));
      
      f2 = mulmod_x16_scaled(qdata,f2,zetainv_512[i],zetainv_qinv_512[i]);
      f3 = mulmod_x16_scaled(qdata,f3,zeta(512,i),zeta_qinv(512,i));
      g3 = add_x16(f3,f2);
      g3 = mulmod_x16_scaled(qdata,g3,zeta4_x16,zeta4_x16_qinv);
      g2 = sub_x16(f3,f2);
      
      f0 = _mm256_loadu_si256((__m256i *) (f + 16*i + 0));
      f1 = _mm256_loadu_si256((__m256i *) (f + 16*i + 128));
      
      f0 = reduce_x16(qdata,f0);
      f1 = mulmod_x16_scaled(qdata,f1,zetainv_256[i],zetainv_qinv_256[i]);
      g1 = add_x16(f0,f1);
      g0 = sub_x16(f0,f1);
      
      f1 = add_x16(g1,g3);
      f3 = sub_x16(g1,g3);
      f0 = add_x16(g0,g2);
      f2 = sub_x16(g0,g2);
      
      _mm256_storeu_si256((__m256i *) (f + 16*i + 128),f1);
      _mm256_storeu_si256((__m256i *) (f + 16*i + 384),f3);
      _mm256_storeu_si256((__m256i *) (f + 16*i + 0),f0);
      _mm256_storeu_si256((__m256i *) (f + 16*i + 256),f2);
    }
    f += 512;
  }
}

void invntt512_7681(int16 *f,int reps)
{
  invntt512(f,reps,(const __m256i *) qdata_7681);
}
