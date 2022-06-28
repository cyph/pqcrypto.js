/**
 *
 * <gf2x_arith.h>
 *
 * @version 2.0 (March 2019)
 *
 * Reference ISO-C11 Implementation of LEDAcrypt using GCC built-ins.
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

#pragma once

#include "gf2x_limbs.h"
#include "architecture_detect.h"
#include <assert.h>

/*----------------------------------------------------------------------------*/
/*
 * Elements of GF(2)[x] are stored in compact dense binary form.
 *
 * Each bit in a byte is assumed to be the coefficient of a binary
 * polynomial f(x), in Big-Endian format (i.e., reading everything from
 * left to right, the most significant element is met first):
 *
 * byte:(0000 0000) == 0x00 ... f(x) == 0
 * byte:(0000 0001) == 0x01 ... f(x) == 1
 * byte:(0000 0010) == 0x02 ... f(x) == x
 * byte:(0000 0011) == 0x03 ... f(x) == x+1
 * ...                      ... ...
 * byte:(0000 1111) == 0x0F ... f(x) == x^{3}+x^{2}+x+1
 * ...                      ... ...
 * byte:(1111 1111) == 0xFF ... f(x) == x^{7}+x^{6}+x^{5}+x^{4}+x^{3}+x^{2}+x+1
 *
 *
 * A "machine word" (A_i) is considered as a DIGIT.
 * Bytes in a DIGIT are assumed in Big-Endian format:
 * E.g., if sizeof(DIGIT) == 4:
 * A_i: A_{i,3} A_{i,2} A_{i,1} A_{i,0}.
 * A_{i,3} denotes the most significant byte, A_{i,0} the least significant one.
 * f(x) ==   x^{31} + ... + x^{24} +
 *         + x^{23} + ... + x^{16} +
 *         + x^{15} + ... + x^{8}  +
 *         + x^{7}  + ... + x^{0}
 *
 *
 * Multi-precision elements (i.e., with multiple DIGITs) are stored in
 * Big-endian format:
 *           A = A_{n-1} A_{n-2} ... A_1 A_0
 *
 *           position[A_{n-1}] == 0
 *           position[A_{n-2}] == 1
 *           ...
 *           position[A_{1}]  ==  n-2
 *           position[A_{0}]  ==  n-1
 */
/*----------------------------------------------------------------------------*/

#define MIN_KAR_DIGITS 9
#define MIN_TOOM_DIGITS 50
#define GF2X_MUL gf2x_mul_TC3

/*----------------------------------------------------------------------------*/

static inline void gf2x_add(const int nr, DIGIT Res[],
                            const int na, const DIGIT A[],
                            const int nb, const DIGIT B[]) {
#if (defined HIGH_PERFORMANCE_X86_64)
 __m256i a, b;
 unsigned i;
 for(i = 0; i < nr/4; i++){
     a = _mm256_lddqu_si256( (__m256i *)A + i );
     b = _mm256_lddqu_si256( (__m256i *)B + i );
     _mm256_storeu_si256(((__m256i *)Res + i), _mm256_xor_si256(a, b));
 }
 i = i*2;
 if(nr %4 >= 2){
 __m128i c, d;
     c = _mm_lddqu_si128( (__m128i *)A + i );
     d = _mm_lddqu_si128( (__m128i *)B + i );
     _mm_storeu_si128(((__m128i *)Res + i), _mm_xor_si128(c, d));
     i++;
 }

 if( (nr & 1) == 1){
      Res[nr-1] = A[nr-1] ^ B[nr-1];
 }

#elif (defined HIGH_COMPATIBILITY_X86_64)
 __m128i a, b;
 for (unsigned i = 0; i < nr/2; i++){
     a = _mm_lddqu_si128( (__m128i *)A + i );
     b = _mm_lddqu_si128( (__m128i *)B + i );
     _mm_storeu_si128(((__m128i *)Res + i), _mm_xor_si128(a, b));
 }
 if( (nr & 1) != 0){
      Res[nr-1] = A[nr-1] ^ B[nr-1];
 }
#else
   for (unsigned i = 0; i < nr; i++)
      Res[i] = A[i] ^ B[i];
#endif
} // end gf2x_add

/*----------------------------------------------------------------------------*/

void GF2X_MUL(const int nr, DIGIT Res[],
              const int na, const DIGIT A[],
              const int nb, const DIGIT B[]
             );

/*----------------------------------------------------------------------------*/

/* Shifts by a single bit  - currently unused used by reflection in DJB inverse, gf2x_mod, TC3 mul*/
static
inline
void right_bit_shift(const int length, DIGIT in[])
{
#if (defined HIGH_PERFORMANCE_X86_64)
   int j;
   if(length>=5){
   __m256i v,u,x,v_tmp;
   __m128i one;
   one = _mm_setzero_si128();
   one = _mm_insert_epi64(one, 1,0);

   v = _mm256_lddqu_si256( (__m256i *)&in[(length-1)-3]);
   u = _mm256_setzero_si256();
   u = _mm256_insert_epi64(u, in[(length-1)-4], 0);
   x = v;
   x = _mm256_permute4x64_epi64(x,0x93);
   x = _mm256_blend_epi32 (x,u, 0x03);

   _mm256_storeu_si256( ((__m256i *)&in[(length-1)-3]),
                        _mm256_or_si256( _mm256_srl_epi64(v, one),
                                         _mm256_slli_epi64(x,DIGIT_SIZE_b-1)
                                       )
                      );
   u=_mm256_permute4x64_epi64(u,0x39);
   j=(length-1)-8;
   for(; j >= 0;j = j-4){
      v = _mm256_lddqu_si256( (__m256i *)&in[j]);
      /* shuffle V so that V = [8 7 6 5] -> V = [7 6 5 8] to be blended with
         u[X X X 0]*/
      v_tmp = _mm256_permute4x64_epi64(v,0x39);
      x = _mm256_blend_epi32 (v_tmp,u, 0xC0);
      _mm256_storeu_si256( ((__m256i *)&in[j+1]),
                           _mm256_or_si256( _mm256_srl_epi64(x, one),
                                            _mm256_slli_epi64(v,DIGIT_SIZE_b-1)
                                          )
                      );
      u=v_tmp;
      /* useless, I'll just not blend the non-relevant part
      u= _mm256_and_si256(u,u_hi_mask); */
   }
   j+=4;
   /*here the highest word of u contains the unshifted MSW before head*/
   if(j == 0) {
     u=_mm256_srli_epi64(u, 1);
     in[j] = _mm256_extract_epi64 (u, 3);
   } 
   if (j > 0){
       /* stops GCC loop optimizer from complaining from an UB due to signed 
        * integer underflow */
       unsigned x; 
       x= j;
       for(; x>0;x--){
       in[x] >>= 1;
       in[x] |= (in[x-1] & (DIGIT)0x01) << (DIGIT_SIZE_b-1);
     }
     in[x] >>= 1;
   }
} else {
  for(j=length-1; j > 0; j--){
       in[j] >>= 1;
       in[j] |= (in[j-1] & (DIGIT)0x01) << (DIGIT_SIZE_b-1);
     }
     in[j] >>= 1;
}
#elif (defined HIGH_COMPATIBILITY_X86_64)
#define UNR 3
   int j;
   __m128i a,b,c,d,e,f;

   for (j = length-1; j > UNR*2 ; j=j-(UNR*2)) {

      a = _mm_lddqu_si128( (__m128i *)&in[j-1]);  //load in[j-1] and in[j]
      b = _mm_lddqu_si128( (__m128i *)&in[j-2]);  //load in[j-2] and in[j-1]
      c = _mm_lddqu_si128( (__m128i *)&in[j-3]);  //load in[j-3] and in[j-2]
      d = _mm_lddqu_si128( (__m128i *)&in[j-4]);  //load in[j-4] and in[j-3]
      e = _mm_lddqu_si128( (__m128i *)&in[j-5]);  //load in[j-5] and in[j-4]
      f = _mm_lddqu_si128( (__m128i *)&in[j-6]);  //load in[j-5] and in[j-6]

      a = _mm_srli_epi64(a, 1);
      b = _mm_slli_epi64(b, (DIGIT_SIZE_b-1));
      c = _mm_srli_epi64(c, 1);
      d = _mm_slli_epi64(d, (DIGIT_SIZE_b-1));
      e = _mm_srli_epi64(e, 1);
      f = _mm_slli_epi64(f, (DIGIT_SIZE_b-1));

      _mm_storeu_si128(((__m128i *)&in[j-1]), _mm_or_si128(a, b));
      _mm_storeu_si128(((__m128i *)&in[j-3]), _mm_or_si128(c, d));
      _mm_storeu_si128(((__m128i *)&in[j-5]), _mm_or_si128(e, f));

   }

   for(; j > 0; j--){
     in[j] >>= 1;
     in[j] |= (in[j-1] & (DIGIT)0x01) << (DIGIT_SIZE_b-1);
   }
   in[j] >>= 1;
#else
   int j;
   for (j = length-1; j > 0 ; j--) {
      in[j] >>= 1;
      in[j] |=  (in[j-1] & (DIGIT)0x01) << (DIGIT_SIZE_b-1);
   }
   in[j] >>=1;
#endif
} // end right_bit_shift

/*----------------------------------------------------------------------------*/
/* PRE: max allowed shift: DIGIT_SIZE_b */
static inline
void right_bit_shift_n(const int length, DIGIT in[], const int amount) {
   assert(amount < DIGIT_SIZE_b);

#if (defined HIGH_PERFORMANCE_X86_64)
   int j;
   if(length>=5){
   __m256i v,u,x,v_tmp;

   v = _mm256_lddqu_si256( (__m256i *)&in[(length-1)-3]);
   u = _mm256_setzero_si256();
   u = _mm256_insert_epi64(u, in[(length-1)-4], 0);
   x = v;
   x = _mm256_permute4x64_epi64(x,0x93);
   x = _mm256_blend_epi32 (x,u, 0x03);

   _mm256_storeu_si256( ((__m256i *)&in[(length-1)-3]),
                        _mm256_or_si256( _mm256_srli_epi64(v, amount),
                                         _mm256_slli_epi64(x,DIGIT_SIZE_b-amount)
                                       )
                      );
   u=_mm256_permute4x64_epi64(u,0x39);
   j=(length-1)-8;
   for(; j >= 0;j = j-4){
      v = _mm256_lddqu_si256( (__m256i *)&in[j]);
      /* shuffle V so that V = [8 7 6 5] -> V = [7 6 5 8] to be blended with
         u[X X X 0]*/
      v_tmp = _mm256_permute4x64_epi64(v,0x39);
      x = _mm256_blend_epi32 (v_tmp,u, 0xC0);
      _mm256_storeu_si256( ((__m256i *)&in[j+1]),
                           _mm256_or_si256( _mm256_srli_epi64(x, amount),
                                            _mm256_slli_epi64(v,DIGIT_SIZE_b-amount)
                                          )
                      );
      u=v_tmp;
      /* useless, I'll just not blend the non-relevant part
      u= _mm256_and_si256(u,u_hi_mask); */
   }
   j+=4;
   /*here the highest word of u contains the unshifted MSW before head*/
   if(j == 0) {
     u=_mm256_srli_epi64(u, amount);
     in[j] = _mm256_extract_epi64 (u, 3);
   }
   if (j > 0){
       /* stops GCC loop optimizer from complaining from an UB due to signed 
        * integer underflow */
       unsigned x;
       x= j;
       DIGIT mask;
       mask = ((DIGIT)0x01 << amount) - 1;
       for(; x>0; x--){
       in[x] >>= amount;
       in[x] |= (in[x-1] & mask) << (DIGIT_SIZE_b-amount);
     }
     in[0] >>= amount;
   }
} else {
  DIGIT mask;
  mask = ((DIGIT)0x01 << amount) - 1;    
  for(j=length-1; j > 0; j--){
       in[j] >>= amount;
       in[j] |= (in[j-1] & mask) << (DIGIT_SIZE_b-amount);
     }
     in[j] >>= amount;
}
#elif (defined HIGH_COMPATIBILITY_X86_64)
#define UNR 3
   int j;
   __m128i a,b,c,d,e,f;

   for (j = length-1; j > UNR*2 ; j=j-(UNR*2)) {

      a = _mm_lddqu_si128( (__m128i *)&in[j-1]);  //load in[j-1] and in[j]
      b = _mm_lddqu_si128( (__m128i *)&in[j-2]);  //load in[j-2] and in[j-1]
      c = _mm_lddqu_si128( (__m128i *)&in[j-3]);  //load in[j-3] and in[j-2]
      d = _mm_lddqu_si128( (__m128i *)&in[j-4]);  //load in[j-4] and in[j-3]
      e = _mm_lddqu_si128( (__m128i *)&in[j-5]);  //load in[j-5] and in[j-4]
      f = _mm_lddqu_si128( (__m128i *)&in[j-6]);  //load in[j-5] and in[j-6]

      a = _mm_srli_epi64(a, 1);
      b = _mm_slli_epi64(b, (DIGIT_SIZE_b-1));
      c = _mm_srli_epi64(c, 1);
      d = _mm_slli_epi64(d, (DIGIT_SIZE_b-1));
      e = _mm_srli_epi64(e, 1);
      f = _mm_slli_epi64(f, (DIGIT_SIZE_b-1));

      _mm_storeu_si128(((__m128i *)&in[j-1]), _mm_or_si128(a, b));
      _mm_storeu_si128(((__m128i *)&in[j-3]), _mm_or_si128(c, d));
      _mm_storeu_si128(((__m128i *)&in[j-5]), _mm_or_si128(e, f));

   }
   DIGIT mask;
   mask = ((DIGIT)0x01 << amount) - 1;
   for(; j > 0; j--){
     in[j] >>= 1;
     in[j] |= (in[j-1] & mask) << (DIGIT_SIZE_b-1);
   }
   in[j] >>= 1;
#else
   int j;
   DIGIT mask;
   mask = ((DIGIT)0x01 << amount) - 1;
   for (j = length-1; j > 0 ; j--) {
      in[j] >>= amount;
      in[j] |=  (in[j-1] & mask) << (DIGIT_SIZE_b - amount);
   }
   in[j] >>= amount;
#endif
} // end right_bit_shift_n

/*----------------------------------------------------------------------------*/
/* Shifts by whole digits */
static 
inline
void right_DIGIT_shift_n(const int length, DIGIT in[], int amount)
{
#if defined(HIGH_PERFORMANCE_X86_64)
    __m256i a;
    int j;
   if(length-amount > 4) {
     for(j = length-4; j >= amount+4; j = j-4){
           /* load from j-amt store in j */
        a = _mm256_lddqu_si256(   (__m256i *) (&in[j - amount]));
            _mm256_storeu_si256(  (__m256i *) (&in[j]) , a);
     }
     j = j + 3;
   } else {
     j = length - 1;
   }
   for (; j >= amount; j--) {
      in[j] = in[j-amount];
   }
   for (; j >=0 ; j--) {
      in[j] = (DIGIT)0;
   }
#else
   int j;
   for (j = length-1; j >= amount; j--) {
      in[j] = in[j-amount];
   }
   for (; j >=0 ; j--) {
      in[j] = (DIGIT)0;
   }
#endif
} // end right_DIGIT_shift_n

/*----------------------------------------------------------------------------*/
/* Shift by an arbitrary amount -- used by gf2x_mod_inverse_DJB*/
static inline
void right_bit_shift_wide_n(const int length, DIGIT in[], int amount)
{
   right_DIGIT_shift_n(length, in, amount / DIGIT_SIZE_b);
   right_bit_shift_n(length, in, amount % DIGIT_SIZE_b);
}

/*----------------------------------------------------------------------------*/
/* PRE: max allowed shift: DIGIT_SIZE_b - used by TC3 mul */
static inline
void left_bit_shift_n(const int length, DIGIT in[], const int amount) {
   assert(amount < DIGIT_SIZE_b);
   if ( amount == 0 ) return;
#if (defined HIGH_PERFORMANCE_X86_64)
   int j;
   __m256i a,b;

   for(j = 0; j < (length-1)/4; j++){

     a = _mm256_lddqu_si256( (__m256i *) &in[0] + j);  //load from in[j] to in[j+3]
     b = _mm256_lddqu_si256( (__m256i *) &in[1] + j);  //load from in[j+1] to in[j+4]

     a = _mm256_slli_epi64(a, amount);
     b = _mm256_srli_epi64(b, (DIGIT_SIZE_b-amount));

     _mm256_storeu_si256( (__m256i *) &in[0] + j , _mm256_or_si256(a,b));
   }

   for(j = j*4; j < length-1; j++) {
     in[j] <<= amount;
     in[j] |= in[j+1] >> (DIGIT_SIZE_b-amount);
   }

   in[length-1] <<= amount; //last element shift
#else
   int j;
   DIGIT mask;
   mask = ~(((DIGIT)0x01 << (DIGIT_SIZE_b - amount)) - 1);
   for (j = 0 ; j < length-1 ; j++) {
      in[j] <<= amount;
      in[j] |=  (in[j+1] & mask) >> (DIGIT_SIZE_b - amount);
   }
   in[j] <<= amount;
#endif
} // end left_bit_shift_n
/*----------------------------------------------------------------------------*/
/* shifts by whole digits - used by gf2x_mod_inverse_KTT */
static inline
void left_DIGIT_shift_n(const int length, DIGIT in[], int amount)
{
    
#if defined(HIGH_PERFORMANCE_X86_64)
    __m256i a;
    int j;
   if(length-amount > 4) {
     for(j = 0; j < length-amount-4; j = j+4){
           /* load from j-amt store in j */
        a = _mm256_lddqu_si256(   (__m256i *) (&in[j + amount]));
            _mm256_storeu_si256(  (__m256i *) (&in[j]) , a);
     }
     j = j - 1;
   } else {
     j = 0;
   }
   for (; j + amount < length; j++) {
      in[j] = in[j+amount];
   }
   for (; j < length; j++) {
      in[j] = (DIGIT)0;
   }
#else
   int j;
   for (j = 0; (j + amount) < length; j++) {
      in[j] = in[j+amount];
   }
   for (; j < length; j++) {
      in[j] = (DIGIT)0;
   }
#endif
} // end left_bit_shift_n


/*----------------------------------------------------------------------------*/
/* may shift by an arbitrary amount - used by NONE */
static inline
void left_bit_shift_wide_n(const int length, DIGIT in[], int amount)
{
   left_DIGIT_shift_n(length, in, amount / DIGIT_SIZE_b);
   left_bit_shift_n(length, in, amount % DIGIT_SIZE_b);
} // end left_bit_shift_n

/*----------------------------------------------------------------------------*/
