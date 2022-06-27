/**
 *
 * Optimized ISO-C11 Implementation of LEDAcrypt using GCC built-ins.
 *
 * @version 3.0 (May 2020)
 *
 * In alphabetical order:
 *
 * @author Marco Baldi <m.baldi@univpm.it>
 * @author Alessandro Barenghi <alessandro.barenghi@polimi.it>
 * @author Franco Chiaraluce <f.chiaraluce@univpm.it>
 * @author Gerardo Pelosi <gerardo.pelosi@polimi.it>
 * @author Paolo Santini <p.santini@pm.univpm.it>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

#include "gf2x_arith.h"
#include <string.h>  // memset(...)
#include "architecture_detect.h"

/*----------------------------------------------------------------------------*/
#if (defined HIGH_PERFORMANCE_X86_64) || (defined HIGH_COMPATIBILITY_X86_64)

#define LOAD_m128_FROM_u64(a,b) ((a) = _mm_loadu_si128((__m128i *)(b)))
#define STORE_m128_INTO_u64(a,b) _mm_storeu_si128((__m128i *)(a), (b))

static inline void xor_128_v_with_r(uint64_t* i,__m128i v){
    __m128i v1 = _mm_loadu_si128((__m128i *)i);
    v1 = (__m128i) _mm_shuffle_pd( (__m128d) v1, (__m128d) v1, 1);
    v1 = _mm_xor_si128(v1, _mm_loadu_si128(&v));
    v1 = (__m128i) _mm_shuffle_pd( (__m128d) v1, (__m128d) v1, 1);
    _mm_storeu_si128((__m128i *)i, v1);
}

// 1-word multiplication
static inline void gf2x_mul_1_avx(uint64_t *a, const uint64_t *b, const uint64_t *c){
     __m128i res;
     res = _mm_clmulepi64_si128(_mm_set_epi64x(0, b[0]),_mm_set_epi64x(0, c[0]), 0);
     res = (__m128i) _mm_shuffle_pd( (__m128d) res, (__m128d) res, 1);
     STORE_m128_INTO_u64(a, res);
}

//2-word multiplication
static inline void gf2x_mul_2_avx(uint64_t *a, const uint64_t *b, const uint64_t *c) {
   register __m128i v1, v2;
   __m128i t1, t2, t3, t4;
   LOAD_m128_FROM_u64(v1, b);
   LOAD_m128_FROM_u64(v2, c);
   t1 = _mm_clmulepi64_si128(v1, v2, 0x11);
   t2 = _mm_clmulepi64_si128(v1, v2, 0x0);
   t3 = _mm_xor_si128(v1, _mm_srli_si128(v1, 8));
   t4 = _mm_xor_si128(v2, _mm_srli_si128(v2, 8));
   v1 = _mm_clmulepi64_si128(t3, t4, 0);
   v2 = _mm_xor_si128(_mm_xor_si128(v1, t2), t1);
   t1 = _mm_xor_si128(t1, _mm_slli_si128(v2, 8));
   t2 = _mm_xor_si128(t2, _mm_srli_si128(v2, 8));
   v1 = (__m128i) _mm_shuffle_pd( (__m128d) t1, (__m128d) t1,1);
   v2 = (__m128i) _mm_shuffle_pd( (__m128d) t2, (__m128d) t2,1);
   STORE_m128_INTO_u64(a, v2);
   STORE_m128_INTO_u64(a + 2, v1);
}

// 3-word multiplication
static inline void gf2x_mul_3_avx(uint64_t *a, const uint64_t *b, const uint64_t *c) {
  __m128i v1, v2;
  __m128i t1, t2, t3;
  __m128i t4, t5, t6, t7, t8;
  t3 = _mm_set_epi64x(c[0], b[0]);
  t7 = _mm_clmulepi64_si128(t3, t3, 1);

  t4 = _mm_loadu_si128((__m128i *)(b+1));
  t5 = _mm_loadu_si128((__m128i *)(c+1));
  t8 = _mm_clmulepi64_si128(t4, t5, 0x11);

  t6 = _mm_clmulepi64_si128(t4, t5, 0);
  v1 = _mm_unpackhi_epi64(t4, t5);
  v2 = _mm_unpacklo_epi64(t4, t5);
  t4 = _mm_xor_si128(v1, v2);
  t1 = _mm_clmulepi64_si128(t4, t4, 1);
  t5 = _mm_xor_si128(t3, v2);
  t2 = _mm_clmulepi64_si128(t5, t5, 1);
  v1 = _mm_xor_si128(v1, t3);
  t3 = _mm_clmulepi64_si128(v1, v1, 1);
  v1 = _mm_xor_si128(t6, t8);
  t1 = _mm_xor_si128(t1, v1);
  t2 = _mm_xor_si128(t2, _mm_xor_si128(t6, t7));
  t3 = _mm_xor_si128(t3, _mm_xor_si128(v1, t7));
  t8 = _mm_xor_si128(t8, _mm_slli_si128(t1, 8));
  t7 = _mm_xor_si128(t7, _mm_srli_si128(t2, 8));
  t3 = _mm_xor_si128(t3, _mm_alignr_epi8(t2, t1, 8));

  v1 = (__m128i) _mm_shuffle_pd( (__m128d) t7, (__m128d) t7,1);
  v2 = (__m128i) _mm_shuffle_pd( (__m128d) t3, (__m128d) t3,1);
  t1 = (__m128i) _mm_shuffle_pd( (__m128d) t8, (__m128d) t8,1);
  STORE_m128_INTO_u64(a, v1);
  STORE_m128_INTO_u64(a+2, v2);
  STORE_m128_INTO_u64(a+4, t1);
}

// 4-word multiplication (non-recursive)
static inline void gf2x_mul_4_avx(uint64_t *a, const uint64_t *b, const uint64_t *c) {
   register __m128i v1, v2, t1, t2;
   __m128i m[9], bc0, bc1, bc2, bc3;
   v1 = _mm_set_epi64x(b[2], b[3]);
   v2 = _mm_set_epi64x(c[2], c[3]);
   t1 = _mm_set_epi64x(b[0], b[1]);
   t2 = _mm_set_epi64x(c[0], c[1]);

   bc0 = _mm_unpacklo_epi64(v1, v2);
   bc1 = _mm_unpackhi_epi64(v1, v2);
   bc2 = _mm_unpacklo_epi64(t1, t2);
   bc3 = _mm_unpackhi_epi64(t1, t2);

   m[0] = _mm_clmulepi64_si128(bc0, bc0, 1);
   m[1] = _mm_clmulepi64_si128(bc1, bc1, 1);
   m[2] = _mm_clmulepi64_si128(bc2, bc2, 1);
   m[3] = _mm_clmulepi64_si128(bc3, bc3, 1);
   t1   = _mm_xor_si128(bc0, bc1);
   t2   = _mm_xor_si128(bc2, bc3);
   m[4] = _mm_clmulepi64_si128(t1, t1, 1);
   m[5] = _mm_clmulepi64_si128(t2, t2, 1);
   t1   = _mm_xor_si128(bc0, bc2);
   t2   = _mm_xor_si128(bc1, bc3);
   m[6] = _mm_clmulepi64_si128(t1, t1, 1);
   m[7] = _mm_clmulepi64_si128(t2, t2, 1);
   v1   = _mm_xor_si128(t1, t2);
   m[8] = _mm_clmulepi64_si128(v1, v1, 1);
   t1   = _mm_xor_si128(m[0], m[1]);
   t2   = _mm_xor_si128(m[2], m[6]);
   v1   = _mm_xor_si128(t1, m[4]);
   bc0  = _mm_xor_si128(m[0], _mm_slli_si128(v1, 8));
   bc1  = _mm_xor_si128(t1, t2);
   t1   = _mm_xor_si128(m[2], m[3]);
   t2   = _mm_xor_si128(m[1], m[7]);
   v2   = _mm_xor_si128(t1, m[5]);
   bc2  = _mm_xor_si128(t1, t2);
   bc3  = _mm_xor_si128(m[3], _mm_srli_si128(v2, 8));
   t1   = _mm_xor_si128(m[6], m[7]);
   t2   = _mm_xor_si128(_mm_xor_si128(v1, v2), _mm_xor_si128(t1, m[8]));
   bc1  = _mm_xor_si128(bc1, _mm_alignr_epi8(t2, v1, 8));
   bc2  = _mm_xor_si128(bc2, _mm_alignr_epi8(v2, t2, 8));

   bc0 = (__m128i) _mm_shuffle_pd( (__m128d) bc0, (__m128d) bc0, 1);
   bc1 = (__m128i) _mm_shuffle_pd( (__m128d) bc1, (__m128d) bc1, 1);
   bc2 = (__m128i) _mm_shuffle_pd( (__m128d) bc2, (__m128d) bc2, 1);
   bc3 = (__m128i) _mm_shuffle_pd( (__m128d) bc3, (__m128d) bc3, 1);

   STORE_m128_INTO_u64(a+6, bc0);
   STORE_m128_INTO_u64(a+4, bc1);
   STORE_m128_INTO_u64(a+2, bc2);
   STORE_m128_INTO_u64(a+0, bc3);
}

// 5-word multiplication (strategy 5-1)
static inline void gf2x_mul_5_avx(uint64_t *a, const uint64_t *b, const uint64_t *c) {
register __m128i v1, v2, t1, t2,tmp;
   __m128i m[14], bc[5];
   bc[0] = _mm_set_epi64x(c[4], b[4]);
   bc[1] = _mm_set_epi64x(c[3], b[3]);
   bc[2] = _mm_set_epi64x(c[2], b[2]);
   bc[3] = _mm_set_epi64x(c[1], b[1]);
   bc[4] = _mm_set_epi64x(c[0], b[0]);
   LOAD_m128_FROM_u64(v1, b+3);
   LOAD_m128_FROM_u64(v2, c+3);
   m[1] = _mm_clmulepi64_si128(v1, v2, 0x11);
   m[2] = _mm_clmulepi64_si128(v1, v2, 0);
   v1 = _mm_set_epi64x(b[1], b[2]);
   v2 = _mm_set_epi64x(c[1], c[2]);
   m[3] = _mm_clmulepi64_si128(v1, v2, 0);
   m[4] = _mm_clmulepi64_si128(v1, v2, 0x11);
   m[5] = _mm_clmulepi64_si128(bc[4], bc[4], 1);
   v1 = _mm_xor_si128(bc[0], bc[1]);
   m[6] = _mm_clmulepi64_si128(v1, v1, 1);
   v2 = _mm_xor_si128(bc[0], bc[2]);
   m[7]= _mm_clmulepi64_si128(v2, v2, 1);
   t1 = _mm_xor_si128(bc[4], bc[2]);
   m[8] = _mm_clmulepi64_si128(t1, t1, 1);
   t2 = _mm_xor_si128(bc[3], bc[4]);
   m[9] = _mm_clmulepi64_si128(t2, t2, 1);
   v2 = _mm_xor_si128(v2, bc[3]);
   m[10]= _mm_clmulepi64_si128(v2, v2, 1);
   t1 = _mm_xor_si128(t1, bc[1]);
   m[11] = _mm_clmulepi64_si128(t1, t1, 1);
   v1 = _mm_xor_si128(v1, t2);
   m[12] = _mm_clmulepi64_si128(v1, v1, 1);
   v1 = _mm_xor_si128(v1, bc[2]);
   m[13] = _mm_clmulepi64_si128(v1, v1, 1);

   tmp = m[1];
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+8), tmp);

   tmp = m[5];
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+0), tmp);

   m[0] = _mm_xor_si128(m[1], m[2]);

   tmp = _mm_xor_si128(m[0], _mm_xor_si128(m[7], m[3]));
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   STORE_m128_INTO_u64(a+6,tmp);

   v1 = _mm_xor_si128(m[0], m[6]);
   m[0] = _mm_xor_si128(m[4], m[5]);

   tmp = _mm_xor_si128(m[0], _mm_xor_si128(m[8], m[3]));
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   STORE_m128_INTO_u64(a+2, tmp);

   v2 = _mm_xor_si128(m[0], m[9]);

   register __m128i t;
   t = _mm_xor_si128(m[13], v1);
   t = _mm_xor_si128(t, v2);
   t = _mm_xor_si128(t, m[10]);
   t = _mm_xor_si128(t, m[11]);

   tmp = t;
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+4), tmp);

   t = _mm_xor_si128(m[13], m[12]);

   tmp = _mm_load_si128((__m128i *)(a+6));
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   m[0] = _mm_xor_si128(tmp, t);

   tmp = _mm_load_si128((__m128i *)(a+2));
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   t = _mm_xor_si128(tmp, t);
   m[0] = _mm_xor_si128(m[0], m[5]);
   m[0] = _mm_xor_si128(m[0], m[11]);
   t = _mm_xor_si128(t, m[1]);
   t = _mm_xor_si128(t, m[10]);

   xor_128_v_with_r(a+7, v1);
   xor_128_v_with_r(a+5, t);
   xor_128_v_with_r(a+3, m[0]);
   xor_128_v_with_r(a+1, v2);
}

// 6-word multiplication (1 round Karatsuba w/ gf2x_mul_3_avx)
static inline void gf2x_mul_6_avx(uint64_t *a, const uint64_t *b, const uint64_t *c) {
    /* compute onto destination b0c0 */
    gf2x_mul_3_avx(a,b,c);
    /* compute onto destination b0c0 */
    gf2x_mul_3_avx(a+6,b+3,c+3);

    alignas(32) uint64_t sum1[3], sum2[3], prodsum[6];
    __m128i tmp1,tmp2;
    LOAD_m128_FROM_u64(tmp1, c);
    LOAD_m128_FROM_u64(tmp2, c+3);
    tmp1 = _mm_xor_si128(tmp1, tmp2);
    STORE_m128_INTO_u64(sum1, tmp1);
    sum1[2] = c[2]^c[5];

    LOAD_m128_FROM_u64(tmp1, b);
    LOAD_m128_FROM_u64(tmp2, b+3);
    tmp1 = _mm_xor_si128(tmp1,tmp2);
    STORE_m128_INTO_u64(sum2, tmp1);
    sum2[2] = b[2]^b[5];

    /* (a1+a0)*(b1+b0) +  */
    gf2x_mul_3_avx(prodsum,sum1,sum2);

    LOAD_m128_FROM_u64(tmp1, prodsum);
    LOAD_m128_FROM_u64(tmp2, a);
    tmp1 = _mm_xor_si128(tmp1,tmp2);
    LOAD_m128_FROM_u64(tmp2, a+6);
    tmp1 = _mm_xor_si128(tmp1,tmp2);

    __m128i tmp3;
    LOAD_m128_FROM_u64(tmp3, prodsum+2);
    LOAD_m128_FROM_u64(tmp2, a+2);
    tmp3 = _mm_xor_si128(tmp3,tmp2);
    LOAD_m128_FROM_u64(tmp2, a+8);
    tmp3 = _mm_xor_si128(tmp3,tmp2);

    __m128i tmp4;
    LOAD_m128_FROM_u64(tmp4, prodsum+4);
    LOAD_m128_FROM_u64(tmp2, a+4);
    tmp4 = _mm_xor_si128(tmp4,tmp2);
    LOAD_m128_FROM_u64(tmp2, a+10);
    tmp4 = _mm_xor_si128(tmp4,tmp2);


    LOAD_m128_FROM_u64(tmp2, a+3);
    tmp1 = _mm_xor_si128(tmp1,tmp2);
    STORE_m128_INTO_u64(a + 3, tmp1);

    LOAD_m128_FROM_u64(tmp2, a+5);
    tmp3 = _mm_xor_si128(tmp3,tmp2);
    STORE_m128_INTO_u64(a + 5, tmp3);

    LOAD_m128_FROM_u64(tmp2, a+7);
    tmp4 = _mm_xor_si128(tmp4,tmp2);
    STORE_m128_INTO_u64(a + 7, tmp4);
}

// 7-word multiplication (strategy 7-1)
static inline void gf2x_mul_7_avx(uint64_t *a, const uint64_t *b, const uint64_t *c) {
   __m128i m[22], bc[7];
   __m128i v1, v2;
   __m128i t1, t2, t3, t4, t5, t6, t7, t8,tmp;
   bc[0] = _mm_set_epi64x(c[6], b[6]);
   bc[1] = _mm_set_epi64x(c[5], b[5]);
   bc[2] = _mm_set_epi64x(c[4], b[4]);
   bc[3] = _mm_set_epi64x(c[3], b[3]);
   bc[4] = _mm_set_epi64x(c[2], b[2]);
   bc[5] = _mm_set_epi64x(c[1], b[1]);
   bc[6] = _mm_set_epi64x(c[0], b[0]);

   v1 = _mm_set_epi64x(b[5],b[6]);
   v2 = _mm_set_epi64x(c[5],c[6]);
   m[1] = _mm_clmulepi64_si128(v1, v2, 0); //p0
   m[2] = _mm_clmulepi64_si128(v1, v2, 0x11); //p1
   t1 = _mm_xor_si128(v1, _mm_xor_si128(_mm_slli_si128(v1, 8), _mm_cvtsi64_si128(b[4])));
   t2 = _mm_xor_si128(v2, _mm_xor_si128(_mm_slli_si128(v2, 8), _mm_cvtsi64_si128(c[4])));
   m[4] = _mm_clmulepi64_si128(t1, t2, 0); //p02
   m[3] = _mm_clmulepi64_si128(t1, t2, 0x11); //p01
   v1 = _mm_set_epi64x(b[1],b[2]);
   v2 = _mm_set_epi64x(c[1],c[2]);

   m[5] = _mm_clmulepi64_si128(v1, v2, 0);    //p4
   m[6] = _mm_clmulepi64_si128(v1, v2, 0x11); //p5
   t3 = _mm_xor_si128(v1, _mm_set1_epi64x(b[0]));
   t4 = _mm_xor_si128(v2, _mm_set1_epi64x(c[0]));
   m[7] = _mm_clmulepi64_si128(t3, t4, 0);    //p46
   m[8] = _mm_clmulepi64_si128(t3, t4, 0x11); //p56
   v1 = _mm_xor_si128(t1, t3);
   v2 = _mm_xor_si128(t2, t4);
   m[9] = _mm_clmulepi64_si128(v1, v2, 0);  //p0246
   t5 = _mm_xor_si128(_mm_xor_si128(bc[1], bc[3]), bc[5]);
   m[16] = _mm_clmulepi64_si128(t5, t5, 1); //p135
   t6 = _mm_xor_si128(t5, bc[4]);
   m[17] = _mm_clmulepi64_si128(t6, t6, 1);  //p1345
   m[18] = _mm_clmulepi64_si128(_mm_xor_si128(v1, t5), _mm_xor_si128(v2, _mm_srli_si128(t5, 8)), 0); //p0123456
   v1 = _mm_xor_si128(_mm_srli_si128(t1, 8), t3);
   v2 = _mm_xor_si128(_mm_srli_si128(t2, 8), t4);
   t7 = _mm_xor_si128(_mm_insert_epi64(v1, 0, 1), _mm_slli_si128(v2, 8));
   m[15] = _mm_clmulepi64_si128(t7, t7, 1);  //p0146
   t8 = _mm_xor_si128(t7, bc[3]);
   m[10] = _mm_clmulepi64_si128(t8, t8, 1); //p01346
   t7 = _mm_xor_si128(bc[3], bc[0]);
   t7 = _mm_xor_si128(t7, _mm_srli_si128(t3, 8));
   t7 = _mm_xor_si128(t7, _mm_insert_epi64(t4, 0, 0));
   m[11] = _mm_clmulepi64_si128(t7, t7, 1);  //p0356
   t7 = _mm_xor_si128(t7, bc[2]);
   m[12] = _mm_clmulepi64_si128(t7, t7, 1);  //p02356
   m[13] = _mm_clmulepi64_si128(bc[2], bc[2], 1); //p2

   m[14] = _mm_clmulepi64_si128(bc[6], bc[6], 1); //p6
   t5 = _mm_xor_si128(_mm_xor_si128(bc[2], bc[4]), bc[5]);
   t6 = _mm_xor_si128(t5, bc[1]);
   m[19] = _mm_clmulepi64_si128(t6, t6, 1); //p1245
   t7 = _mm_xor_si128(t5, bc[6]);
   m[20] = _mm_clmulepi64_si128(t7, t7, 1); //p2456
   t5 = _mm_xor_si128(bc[2], bc[3]);
   t6 = _mm_xor_si128(t5, _mm_xor_si128(bc[0], bc[4]));
   m[21] = _mm_clmulepi64_si128(t6, t6, 1); //p0234
   t7 = _mm_xor_si128(t5, _mm_xor_si128(bc[1], bc[6]));
   m[0] = _mm_clmulepi64_si128(t7, t7, 1); //p1236

   tmp = m[1];
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+12), tmp);

   tmp = m[14];
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+0), tmp);

   t1 = _mm_xor_si128(m[1], m[2]);
   t2 = _mm_xor_si128(m[4], m[13]);
   t8 = _mm_xor_si128(t1, t2);

   tmp = t8;
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+10), tmp);
   _mm_store_si128((__m128i *)(a+8), tmp);
   t8 = _mm_xor_si128(t1, m[3]);

   xor_128_v_with_r(a+11, t8);
   t7 = _mm_xor_si128(t2, m[3]);

   tmp = t7;
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+6), tmp);
   _mm_store_si128((__m128i *)(a+4), tmp);

   t5 = _mm_xor_si128(m[6], m[14]);
   t6 = _mm_xor_si128(m[5], m[7]);
   t4 = _mm_xor_si128(t5, t6);

   tmp = t4;
   tmp = (__m128i) _mm_shuffle_pd( (__m128d) tmp, (__m128d) tmp, 1);
   _mm_store_si128((__m128i *)(a+2), tmp);

   xor_128_v_with_r(a+1, _mm_xor_si128(t5, m[8]));
   t3 = _mm_xor_si128(m[8], m[14]);
   t1 = _mm_xor_si128(m[15], m[17]);
   t5 = _mm_xor_si128(m[16], m[9]);
   v1 = _mm_xor_si128(m[8], m[6]);

   xor_128_v_with_r(a+5, _mm_xor_si128(_mm_xor_si128(t7, t5), _mm_xor_si128(_mm_xor_si128(t6, t3),
   _mm_xor_si128(m[20], t1))));
   t1 = _mm_xor_si128(m[12], t1);
   v2 = _mm_xor_si128(m[0], _mm_xor_si128(m[19], m[21]));
   xor_128_v_with_r(a+7, _mm_xor_si128(_mm_xor_si128(t1, v2), _mm_xor_si128(_mm_xor_si128(t6, t3),
   _mm_xor_si128(m[2], m[3]))));
   xor_128_v_with_r(a+6, _mm_xor_si128(_mm_xor_si128(v1, v2), _mm_xor_si128(m[10], m[20])));
   t3 = _mm_xor_si128(m[10], _mm_xor_si128(m[18], m[21]));
   xor_128_v_with_r(a+4, _mm_xor_si128(_mm_xor_si128(_mm_xor_si128(m[1], m[6]), t6),
   _mm_xor_si128(m[0], _mm_xor_si128(t1, t3))));
   xor_128_v_with_r(a+8, _mm_xor_si128(_mm_xor_si128(t1, t5), _mm_xor_si128(_mm_xor_si128(m[18], m[20]),
   _mm_xor_si128(v1, m[19]))));

   xor_128_v_with_r(a+3, _mm_xor_si128(_mm_xor_si128(_mm_xor_si128(m[11], m[17]), _mm_xor_si128(m[2], m[19])),
   _mm_xor_si128(_mm_xor_si128(t4, t2), _mm_xor_si128(t3, t5))));
   v1 = _mm_xor_si128(_mm_xor_si128(m[11], m[17]), m[20]);
   v2 = _mm_xor_si128(_mm_xor_si128(m[0], m[10]), m[12]);
   xor_128_v_with_r(a+9, _mm_xor_si128(_mm_xor_si128(t6, m[8]), _mm_xor_si128(_mm_xor_si128(t5, t8),
   _mm_xor_si128(v1, v2))));
}

// 8-word multiplication (1 round Karatsuba w/ gf2x_mul_4_avx)
static inline void gf2x_mul_8_avx(uint64_t *a, const uint64_t *b, const uint64_t *c) {
    /* compute onto destination b0c0 */
    gf2x_mul_4_avx(a,b,c);
    /* compute onto destination b0c0 */
    gf2x_mul_4_avx(a+8,b+4,c+4);

    alignas(32) uint64_t sum1[4], sum2[4], prodsum[8];
    __m128i tmp1,tmp2;

    LOAD_m128_FROM_u64(tmp1, c);
    LOAD_m128_FROM_u64(tmp2, c+4);
    tmp1 = _mm_xor_si128(tmp1, tmp2);
    STORE_m128_INTO_u64(sum1, tmp1);
    LOAD_m128_FROM_u64(tmp1, c+2);
    LOAD_m128_FROM_u64(tmp2, c+6);
    tmp1 = _mm_xor_si128(tmp1, tmp2);
    STORE_m128_INTO_u64(sum1+2, tmp1);

    LOAD_m128_FROM_u64(tmp1, b);
    LOAD_m128_FROM_u64(tmp2, b+4);
    tmp1 = _mm_xor_si128(tmp1, tmp2);
    STORE_m128_INTO_u64(sum2, tmp1);
    LOAD_m128_FROM_u64(tmp1, b+2);
    LOAD_m128_FROM_u64(tmp2, b+6);
    tmp1 = _mm_xor_si128(tmp1, tmp2);
    STORE_m128_INTO_u64(sum2+2, tmp1);

    /* (a1+a0)*(b1+b0) +  */
    gf2x_mul_4_avx(prodsum,sum1,sum2);

    LOAD_m128_FROM_u64(tmp1, prodsum);
    LOAD_m128_FROM_u64(tmp2, a);
    tmp1 = _mm_xor_si128(tmp1,tmp2);
    LOAD_m128_FROM_u64(tmp2, a+8);
    tmp1 = _mm_xor_si128(tmp1,tmp2);

    __m128i tmp3;
    LOAD_m128_FROM_u64(tmp3, prodsum+2);
    LOAD_m128_FROM_u64(tmp2, a+2);
    tmp3 = _mm_xor_si128(tmp3,tmp2);
    LOAD_m128_FROM_u64(tmp2, a+10);
    tmp3 = _mm_xor_si128(tmp3,tmp2);

    __m128i tmp4;
    LOAD_m128_FROM_u64(tmp4, prodsum+4);
    LOAD_m128_FROM_u64(tmp2, a+4);
    tmp4 = _mm_xor_si128(tmp4,tmp2);
    LOAD_m128_FROM_u64(tmp2, a+12);
    tmp4 = _mm_xor_si128(tmp4,tmp2);

    __m128i tmp5;
    LOAD_m128_FROM_u64(tmp5, prodsum+6);
    LOAD_m128_FROM_u64(tmp2, a+6);
    tmp5 = _mm_xor_si128(tmp5,tmp2);
    LOAD_m128_FROM_u64(tmp2, a+14);
    tmp5 = _mm_xor_si128(tmp5,tmp2);

    LOAD_m128_FROM_u64(tmp2, a+4);
    tmp1 = _mm_xor_si128(tmp1,tmp2);
    STORE_m128_INTO_u64(a + 4, tmp1);

    LOAD_m128_FROM_u64(tmp2, a+6);
    tmp3 = _mm_xor_si128(tmp3,tmp2);
    STORE_m128_INTO_u64(a + 6, tmp3);

    LOAD_m128_FROM_u64(tmp2, a+8);
    tmp4 = _mm_xor_si128(tmp4,tmp2);
    STORE_m128_INTO_u64(a + 8, tmp4);

    LOAD_m128_FROM_u64(tmp2, a+10);
    tmp5 = _mm_xor_si128(tmp5,tmp2);
    STORE_m128_INTO_u64(a + 10, tmp5);
}

// 9-word multiplication (strategy 9-1)
static inline void gf2x_mul_9_avx(uint64_t *c, const uint64_t *a, const uint64_t *b) {
   alignas(32) uint64_t t1[5], t2[5], t3[10];

   gf2x_mul_5_avx(c+8, a+4, b+4);
   gf2x_mul_4_avx(c, a, b);

   _mm256_store_pd((double *)(t1+1),
                   _mm256_xor_pd(_mm256_loadu_pd((double *)a),
                                 _mm256_loadu_pd((double *)(a+5)))
                  );
   _mm256_storeu_pd((double *)(t2+1),
                    _mm256_xor_pd(_mm256_loadu_pd((double *)b),
                                  _mm256_loadu_pd((double *)(b+5)))
                   );
   t1[0] = a[4];
   t2[0] = b[4];

   gf2x_mul_5_avx(t3, t1, t2);

   __m256d d1 = _mm256_loadu_pd((double *)(t3+6));
   d1 = _mm256_xor_pd(d1, _mm256_loadu_pd((double *)(c+14)));
   d1 = _mm256_xor_pd(d1, _mm256_loadu_pd((double *)(c+4)));

   __m256d d2 = _mm256_loadu_pd((double *)(t3+2));
   d2 = _mm256_xor_pd(d2, _mm256_loadu_pd((double *)(c+10)));
   d2 = _mm256_xor_pd(d2, _mm256_loadu_pd((double *)(c   )));

   _mm_storeu_si128((__m128i *)(c+3),
                    _mm_xor_si128(_mm_loadu_si128((__m128i *)(c+3)),
                                  _mm_xor_si128(_mm_loadu_si128((__m128i *)(t3)),
                                                _mm_loadu_si128((__m128i *)(c+8)))
                                 )
                   );
   _mm256_storeu_pd((double *)(c+9),
                    _mm256_xor_pd(_mm256_loadu_pd((double *)(c+9)), d1));
   _mm256_storeu_pd((double *)(c+5),
                    _mm256_xor_pd(_mm256_loadu_pd((double *)(c+5)), d2));
}

#define CAT(a, b, c) PRIMITIVE_CAT(a, b, c)
#define PRIMITIVE_CAT(a, b, c) a ## b ## c
#define CHUNK_SIZE 9

/* Arbitrary width multiplication exploiting 1x1 to CHUNK_SIZE x CHUNK_SIZE
 * atomic multiplications.
 * The strategy exploits the largest possible multiplications to compute the
 * partial products starting from the least significant digits
 * at the end of the computation of each set of CHUNK_SIZE intermediates,
 * the remaining remC = (lenght mod CHUNK_SIZE) digits are computed exploiting
 * a remC x remC multiplication and 1x1 multiplications for the remaining part
 * Once only remC intermediate results remain to be computed, a sequence of
 * remC x remC multiplications are employed to compute the length/remC*remC
 * digits.
 * Finally a single (length%remC)x(length%remC) multiplication and a sequence
 * of 1x1 ones are employed to complete the computation of the remaining digits
 */
void gf2x_mul_avx(const int nr, uint64_t Res[],
                  const int na, const uint64_t A[],
                  const int nb, const uint64_t B[]) {
   int i, j;
   int remC, quoC; /* remainder and quotient of operand length mod 9*/
   quoC = na / CHUNK_SIZE;
   remC = na % CHUNK_SIZE;
   int quorem = 0, remrem = 0; /*remainder and quotient of operand length mod (remainder of length mod CHUNK_SIZE) */
   if(remC !=0) {
      quorem = na / remC;
      remrem = na % remC;
   }
   alignas(32) uint64_t buffer[CHUNK_SIZE*2];

   memset(Res, 0x00, nr*sizeof(uint64_t));

   /* compute all the intermediate values allowing */
   for( j = 0; j < quoC; j++){ /* B macro-word idx */
     for( i = 0; i < quoC; i++){ /* A macro-word idx */
         /* CHUNK_SIZE x CHUNK_SIZE full block multiplication */
         CAT(gf2x_mul_,CHUNK_SIZE,_avx)(buffer,
                     A+(na - CHUNK_SIZE*(i+1)  ),
                     B+(nb - CHUNK_SIZE*(j+1)  ));
         gf2x_add(2*CHUNK_SIZE,Res+(nr - CHUNK_SIZE*(i+j+2)),
                  2*CHUNK_SIZE,buffer,
                  2*CHUNK_SIZE,Res+(nr - CHUNK_SIZE*(i+j+2)));
     }

     if (remC != 0) {
        /* remC x remC full block mul */
        switch(remC) {
            case 8: gf2x_mul_8_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 7: gf2x_mul_7_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 6: gf2x_mul_6_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 5: gf2x_mul_5_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 4: gf2x_mul_4_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 3: gf2x_mul_3_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 2: gf2x_mul_2_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 1: gf2x_mul_1_avx(buffer, A, B+(nb - CHUNK_SIZE*j - remC)); break;
            case 0: ; /* do nothing */
        }
        int offset = CHUNK_SIZE*(quoC+j);
        gf2x_add(2*remC, Res+(nr - offset - 2*remC),
                 2*remC, buffer,
                 2*remC, Res+(nr - offset - 2*remC));
        /* complete the remaining remC x remrem muls */
        offset += remC;
        for (int k = 0; k < CHUNK_SIZE - remC ; k++){ /* remaining b uint64_ts */
          for (int l = 0; l < remC ; l++){ /* remaining a uint64_ts */
            gf2x_mul_1_avx(buffer, A+(remC-1)-l , B+((nb - CHUNK_SIZE*j - remC-1) - k));
            gf2x_add(2, Res+(nr - offset - 2 - (l+k)),
                     2, buffer,
                     2, Res+(nr - offset - 2 - (l+k)));
          }
        }
     }
   }

   if (remC != 0) {
      /* complete the multiplication computing the product of the most signif. remC uint64_ts of B
       * by the entire A */
      /* Start with na / remC = quorem, remC x remC muls */
      for ( i = 0; i < quorem ; i++ ){
         switch(remC) {
            case 8: gf2x_mul_8_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 7: gf2x_mul_7_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 6: gf2x_mul_6_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 5: gf2x_mul_5_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 4: gf2x_mul_4_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 3: gf2x_mul_3_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 2: gf2x_mul_2_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 1: gf2x_mul_1_avx(buffer, A+(na - remC*(i+1)  ), B); break;
            case 0: ; /* do nothing */
         }
        gf2x_add(2*remC,Res+(nr - CHUNK_SIZE*(quoC)- remC*i - 2*remC),
                 2*remC,buffer,
                 2*remC,Res+(nr - CHUNK_SIZE*(quoC)- remC*i - 2*remC));
      }
      /* compute the remrem x remrem block */
      int offset = remC*quorem + CHUNK_SIZE*(quoC);
      switch(remrem) {
            case 8: gf2x_mul_8_avx( buffer, A, B+remC-remrem ); break;
            case 7: gf2x_mul_7_avx( buffer, A, B+remC-remrem ); break;
            case 6: gf2x_mul_6_avx( buffer, A, B+remC-remrem ); break;
            case 5: gf2x_mul_5_avx( buffer, A, B+remC-remrem ); break;
            case 4: gf2x_mul_4_avx( buffer, A, B+remC-remrem ); break;
            case 3: gf2x_mul_3_avx( buffer, A, B+remC-remrem ); break;
            case 2: gf2x_mul_2_avx( buffer, A, B+remC-remrem ); break;
            case 1: gf2x_mul_1_avx( buffer, A, B+remC-remrem ); break;
            case 0: ;/* do nothing */
      }
      gf2x_add(2*remrem, Res+(nr - offset - 2*remrem),
               2*remrem, buffer,
               2*remrem, Res+(nr - offset - 2*remrem));
      offset = offset + remrem;
      /* the remaining remrem uint64_ts of A are multiplied by the remC - remrem leading of  B*/
      for (int k = 0; k < remC - remrem; k++){ /* uint64_t index in B */
          for (int l = 0; l < remrem ; l++){
              gf2x_mul_1_avx(buffer, A+(remrem-1)-l , B+(remC - remrem -1)-k);
              gf2x_add(2, Res+(nr - offset - 2 - (l+k)),
                       2, buffer,
                       2, Res+(nr - offset - 2 - (l+k)));
          }
      }
   }
}
/*----------------------------------------------------------------------------*/
#else

void gf2x_mul_comb(const int nr, DIGIT Res[],
                   const int na, const DIGIT A[],
                   const int nb, const DIGIT B[]) {
   int i, j, k;
   DIGIT u, h;

   memset(Res, 0x00, nr*sizeof(DIGIT));

   for (k = DIGIT_SIZE_b-1; k > 0; k--) {
      for (i = na-1; i >= 0; i--)
         if ( A[i] & (((DIGIT)0x1) << k) )
            for (j = nb-1; j >= 0; j--) Res[i+j+1] ^= B[j];


      u = Res[na+nb-1];
      Res[na+nb-1] = u << 0x1;
      for (j = 1; j < na+nb; ++j) {
         h = u >> (DIGIT_SIZE_b-1);
         u = Res[na+nb-1-j];
         Res[na+nb-1-j] = h^(u << 0x1);
      }
   }
   for (i = na-1; i >= 0; i--)
      if ( A[i] & ((DIGIT)0x1) )
         for (j = nb-1; j >= 0; j--) Res[i+j+1] ^= B[j];
}
#endif

/*---------------------------------------------------------------------------*/
/* allows the second operand to be shorter than the first */
/* the result should be as large as the first operand*/
static inline void gf2x_add_asymm(const int nr, DIGIT Res[],
              const int na, const DIGIT A[],
              const int nb, const DIGIT B[]) {
   int delta = na-nb;
   memcpy(Res,A,delta*DIGIT_SIZE_B);
   gf2x_add(nb, Res+delta,
            nb, A+delta,
            nb, B);
} // end gf2x_mul_comb

/*---------------------------------------------------------------------------*/

void gf2x_mul_Kar(const int nr, DIGIT Res[],
                   const int na, const DIGIT A[],
                   const int nb, const DIGIT B[]){

    if(na < MIN_KAR_DIGITS || nb < MIN_KAR_DIGITS){
        /* fall back to schoolbook */
#if (defined HIGH_PERFORMANCE_X86_64) || (defined HIGH_COMPATIBILITY_X86_64)
        gf2x_mul_avx(nr, Res, na, A, nb, B);
#else
        gf2x_mul_comb(nr, Res, na, A, nb, B);
#endif
        return;
    }

    if(na %2 == 0){
      unsigned bih = na/2;
      DIGIT middle[2*bih], sumA[bih], sumB[bih];
      gf2x_add(bih,sumA,
               bih,A,
               bih,A+bih);
      gf2x_add(bih,sumB,
               bih,B,
               bih,B+bih);
      gf2x_mul_Kar(2*bih, middle,
                    bih, sumA,
                    bih, sumB);
      gf2x_mul_Kar(2*bih, Res+2*bih,
                    bih, A+bih,
                    bih, B+bih);
      gf2x_add(2*bih, middle,
               2*bih, middle,
               2*bih, Res+2*bih);
      gf2x_mul_Kar(2*bih, Res,
                    bih, A,
                    bih, B);
      gf2x_add(2*bih, middle,
               2*bih, middle,
               2*bih, Res);
      gf2x_add(2*bih, Res+bih,
               2*bih, Res+bih,
               2*bih, middle);
    } else {
      unsigned bih = na/2 + 1;
      DIGIT middle[2*bih], sumA[bih], sumB[bih];
      gf2x_add_asymm(bih,  sumA,
                     bih,  A+bih-1,
                     bih-1,A);
      gf2x_add_asymm(bih,  sumB,
                     bih,  B+bih-1,
                     bih-1,B);
      gf2x_mul_Kar(2*bih, middle,
                    bih, sumA,
                    bih, sumB);
      gf2x_mul_Kar(2*bih, Res+2*(bih-1),
                    bih, A+bih-1,
                    bih, B+bih-1);
      gf2x_add(2*bih, middle,
               2*bih, middle,
               2*bih, Res+2*(bih-1));
      gf2x_mul_Kar(2*(bih-1), Res,
                    (bih-1), A,
                    (bih-1), B);
      gf2x_add_asymm(2*bih, middle,
                     2*bih, middle,
                     2*(bih-1), Res);
      gf2x_add(2*bih, Res+bih-2,
               2*bih, Res+bih-2,
               2*bih, middle);
    }
}

/*----------------------------------------------------------------------------*/
#if (defined HIGH_PERFORMANCE_X86_64) || (defined HIGH_COMPATIBILITY_X86_64)
static inline void gf2x_exact_div_x_plus_one(const int na, DIGIT A[]) {
    DIGIT t = 0;
    int i;
    __m256i vec_t = _mm256_set_epi64x(0,0,0,0), tmp;
    for (i = na - 4; i >= 0; i=i-4) {
    /* translation of  t ^= A[i]; */
        tmp = _mm256_lddqu_si256((__m256i*) (A+i));
        vec_t = _mm256_xor_si256(tmp,vec_t);
        for (int j = 1; j <= DIGIT_SIZE_b / 2; j = j * 2) {
    /* translation of t ^= t << (unsigned) j; */
        tmp = _mm256_slli_epi64(vec_t, j);
            vec_t = _mm256_xor_si256(tmp,vec_t);
        }

        DIGIT hicarry = 0, locarry =0;
        t = _mm256_extract_epi64 (vec_t, 3);
        t >>= DIGIT_SIZE_b - 1;
        locarry -= t;
        t = _mm256_extract_epi64 (vec_t, 1);
        t >>= DIGIT_SIZE_b - 1;
        hicarry -= t;
        tmp = _mm256_set_epi64x(0,locarry,0,hicarry);
        vec_t = _mm256_xor_si256(tmp,vec_t);

        t = _mm256_extract_epi64 (vec_t, 2);
        t >>= DIGIT_SIZE_b - 1;
        hicarry = (DIGIT)0 - t;
        tmp = _mm256_set_epi64x(0,0,hicarry,hicarry);
        vec_t = _mm256_xor_si256(tmp,vec_t);
    /* translation of =   A[i] = t; */
        _mm256_storeu_si256( (__m256i *) (A+i) ,vec_t);
        t = _mm256_extract_epi64 (vec_t, 0);
        t >>= DIGIT_SIZE_b - 1;
        vec_t = _mm256_set_epi64x(t,0,0,0);
    }
    i = i >= -3 ? i+3 : i;
    for (; i >= 0; i--) {

        t ^= A[i];

        for (int j = 1; j <= DIGIT_SIZE_b / 2; j = j * 2) {
            t ^= t << (unsigned) j;
        }
        A[i] = t;
        t >>= DIGIT_SIZE_b - 1;
    }
} // end gf2x_exact_div_x_plus_one
#else
static inline void gf2x_exact_div_x_plus_one(const int na, DIGIT A[]) {
    DIGIT t = 0;
    for (int i = na - 1; i >= 0; i--) {

        t ^= A[i];

        for (int j = 1; j <= DIGIT_SIZE_b / 2; j = j * 2) {
            t ^= t << (unsigned) j;
        }

        A[i] = t;
        t >>= DIGIT_SIZE_b - 1;
    }
} // end gf2x_exact_div_x_plus_one
#endif

/*---------------------------------------------------------------------------*/
/* Toom-Cook 3 algorithm as reported in
 * Marco Bodrato: "Towards Optimal Toom-Cook Multiplication for Univariate and
 * Multivariate Polynomials in Characteristic 2 and 0". WAIFI 2007: 116-133   */

void gf2x_mul_TC3(const int nr, DIGIT Res[],
                   const int na, const DIGIT A[],
                   const int nb, const DIGIT B[]) {

    if(na < MIN_TOOM_DIGITS || nb < MIN_TOOM_DIGITS){
        /* fall back to Karatsuba */
        gf2x_mul_Kar(nr, Res, na, A, nb, B);
        return;
    }

    unsigned bih; //number of limbs for each part.
    if (na % 3 == 0) {
        bih = na / 3;
    } else {
        bih = na / 3 + 1;
    }

    DIGIT u2[bih], *u1, *u0;

    int leading_slack = (3 - (na) % 3) %3;

    int i;
    for(i = 0; i < leading_slack ; i++){
        u2[i] = 0;
    }
    for (; i < bih; ++i) {
        u2[i] = A[i-leading_slack];
    }
    u1= (DIGIT *) (A+bih-leading_slack);
    u0= (DIGIT *) (A+2*bih-leading_slack);

    DIGIT v2[bih], *v1, *v0; /* partitioned inputs */
    for(i = 0; i < leading_slack ; i++){
        v2[i] = 0;
    }
    for (; i < bih; ++i) {
        v2[i] = B[i-leading_slack];
    }
    v1=(DIGIT *) (B+bih-leading_slack);
    v0=(DIGIT *) (B+2*bih-leading_slack);

    DIGIT sum_u[bih]; /*bih digit wide*/
    gf2x_add(bih, sum_u,
             bih, u0,
             bih, u1);
    gf2x_add(bih, sum_u,
             bih, sum_u,
             bih, u2);

    DIGIT sum_v[bih]; /*bih digit wide*/
    gf2x_add(bih, sum_v,
             bih, v0,
             bih, v1);
    gf2x_add(bih, sum_v,
             bih, sum_v,
             bih, v2);

    DIGIT w1[2*bih];
    gf2x_mul_TC3(2*bih, w1,
                 bih, sum_u,
                 bih, sum_v);

    DIGIT u1_x1_u2_x2[bih + 1];
    /* This adds the second polynomial, multiplied by x to the second,
     * multiplied by x^2.
     * Pre, the operands are of the same size, the results has an extra word */
    add_first_by_x_second_by_xsquared(bih+1, u1_x1_u2_x2,
                                      bih, u1,
                                      bih, u2);

    DIGIT temp_u_components[bih + 1];
    gf2x_add_asymm(bih+1, temp_u_components,
                   bih+1, u1_x1_u2_x2,
                   bih, sum_u);

    DIGIT v1_x1_v2_x2[bih + 1];
    add_first_by_x_second_by_xsquared(bih+1, v1_x1_v2_x2,
                                      bih, v1,
                                      bih, v2);
    DIGIT temp_v_components[bih + 1];
    gf2x_add_asymm(bih+1, temp_v_components,
                   bih+1, v1_x1_v2_x2,
                   bih, sum_v);

    DIGIT w3[2*bih+2];
    gf2x_mul_TC3(2*bih+2, w3,
                 bih+1, temp_u_components,
                 bih+1, temp_v_components);

    gf2x_add_asymm(bih+1, u1_x1_u2_x2,
                   bih+1, u1_x1_u2_x2,
                   bih, u0);
    gf2x_add_asymm(bih+1, v1_x1_v2_x2,
                   bih+1, v1_x1_v2_x2,
                   bih, v0);

    DIGIT w2[2*bih+2];
    gf2x_mul_TC3(2*bih+2, w2,
                 bih+1, u1_x1_u2_x2,
                 bih+1, v1_x1_v2_x2);

    DIGIT w4[2*bih];
    gf2x_mul_TC3(2*bih, w4,
                   bih, u2,
                   bih, v2);

    DIGIT * w0 = Res+(nr-2*bih);

    gf2x_mul_TC3(2*bih, w0,
                   bih, u0,
                   bih, v0);

    // Interpolation starts
    gf2x_add(2*bih+2, w3,
             2*bih+2, w2,
             2*bih+2, w3);
    gf2x_add_asymm(2*bih+2, w2,
                   2*bih+2, w2,
                   2*bih, w0);

    /* takes first operand, shifts right it by 1 bit and accumulates the
     * addition of the result with the second operand.
     * Pre: the second operand must have the same length as the first */
    right_bit_shift_and_accumulate(2*bih+2, w2,
                                    w3);

    // w2 + (w4 * x^3+1) = w2 + w4 + w4 << 3
    accu_first_plus_second_by_xcube_plus_one(2*bih+2, w2,
                                             2*bih, w4);

    gf2x_exact_div_x_plus_one(2*bih+2, w2);

    gf2x_add(2*bih, w1,
             2*bih, w1,
             2*bih, w0);
    gf2x_add_asymm(2*bih+2, w3,
                   2*bih+2, w3,
                   2*bih, w1);

    right_bit_shift_n(2*bih+2, w3, 1);
    gf2x_exact_div_x_plus_one(2*bih+2, w3);

    gf2x_add(2*bih, w1,
             2*bih, w1,
             2*bih, w4);

    memset(Res,0,(nr-2*bih )*DIGIT_SIZE_B);

    DIGIT * w1_final = Res+(nr-(3*bih+2));
    gf2x_add(2*bih+2, w1_final,
             2*bih+2, w2,
             2*bih+2, w1_final);
    gf2x_add_asymm(2*bih+2, w1_final,
                   2*bih+2, w1_final,
                   2*bih, w1);

    gf2x_add(2*bih+2, w2,
             2*bih+2, w2,
             2*bih+2, w3);

    // Result recombination starts here

    /* optimization: topmost slack digits should be computed, and not added,
     * zeroization can be avoided altogether with a proper merge of the
     * results */

    int leastSignifDigitIdx = nr - 1;
    /* w0 is already written in place */

    /* w4 does not overlap with w0 */
    leastSignifDigitIdx = nr - 1 - 4*bih;
    for (int i = 0; i< 2*bih && (leastSignifDigitIdx - i >= 0) ; i++){
        Res[leastSignifDigitIdx - i] = w4[2*bih   - 1 - i];
    }
    /* w1, can be computed directly with in-place accumulation */
    /* w2, w3 overlap with each others at least partially */
    leastSignifDigitIdx = nr - 1 - 2*bih;
    for (int i = 0; i< 2*bih+2; i++){
        Res[leastSignifDigitIdx - i] ^= w2[2*bih+2 - 1 - i];
    }
    leastSignifDigitIdx = nr - 1 - 3*bih;
    for (int i = 0; i< 2*bih+2 ; i++){
        Res[leastSignifDigitIdx - i] ^= w3[2*bih+2 - 1 - i];
    }
}
