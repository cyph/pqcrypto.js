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

#pragma once

#include "gf2x_limbs.h"
#include "architecture_detect.h"

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
#include "qc_ldpc_parameters.h"


#if ((P==40787) || (P==19709))
#define MIN_KAR_DIGITS 28
#else
#define MIN_KAR_DIGITS 10
#endif

#define MIN_TOOM_DIGITS ((P+DIGIT_SIZE_b-1)/DIGIT_SIZE_b)
#if (defined HIGH_PERFORMANCE_X86_64)
#define GF2X_MUL gf2x_mul_TC3
#else
#define GF2X_MUL gf2x_mul_Kar
#endif
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
/* PRE: max allowed shift: DIGIT_SIZE_b - used by TC3 mul */
static inline
void right_bit_shift_n(const int length, DIGIT in[], const int amount) {
#if (defined HIGH_PERFORMANCE_X86_64)
   int j;
   __m256i a,b;

   for(j = length-4; j >= 1; j-=4){
     a = _mm256_lddqu_si256( (__m256i *) &in[j-1]);  //load from in[j] to in[j+3]
     b = _mm256_lddqu_si256( (__m256i *) &in[j]);  //load from in[j+1] to in[j+4]

     a = _mm256_slli_epi64(a, (DIGIT_SIZE_b-amount));
     b = _mm256_srli_epi64(b, amount);

     _mm256_storeu_si256( (__m256i *) &in[j] , _mm256_or_si256(a,b));
   }

   j+=3;
   /* deal with case where j is neg, reincrement, escrement */
   for(; j > 0; j--) {
     in[j] = (in[j] >> amount) | (in[j-1] << (DIGIT_SIZE_b-amount));
   }
   in[0] >>= amount; //last element shift
#else
   int j;

   for (j = length-1 ; j >0 ; j--) {
     in[j] = (in[j] >> amount) | (in[j-1] << (DIGIT_SIZE_b-amount));
   }
   in[0] >>= amount; //last element shift
#endif
} // end right_bit_shift_n

/*----------------------------------------------------------------------------*/
/* PRE: max allowed shift: DIGIT_SIZE_b - used by TC3 mul */
static inline
void left_bit_shift_n(const int length, DIGIT in[], const int amount) {
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
/*  Dedicated Toom-Cook helper functions */

/* PRE: max allowed shift: DIGIT_SIZE_b */
static inline
void right_bit_shift_and_accumulate(const int length,
                                    DIGIT in[],
                                    DIGIT to_accumulate[]) {
#if (defined HIGH_PERFORMANCE_X86_64)
   int j;
   __m256i a,b,to_accu,to_store;

   for(j = length-4; j >= 1; j-=4){
     a = _mm256_lddqu_si256( (__m256i *) &in[j-1]);  //load from in[j] to in[j+3]
     b = _mm256_lddqu_si256( (__m256i *) &in[j]);  //load from in[j+1] to in[j+4]

     a = _mm256_slli_epi64(a, (DIGIT_SIZE_b-1));
     b = _mm256_srli_epi64(b, 1);

     to_accu = _mm256_lddqu_si256( (__m256i *) &to_accumulate[j]);
     to_store = _mm256_xor_si256(to_accu,_mm256_or_si256(a,b));
     _mm256_storeu_si256( (__m256i *) &in[j] , to_store);
   }
   j+=3;
   for(; j > 0; j--) {
     in[j] = ((in[j] >> 1) | (in[j-1] << (DIGIT_SIZE_b-1))) ^ to_accumulate[j];
   }
   in[0] = (in[0] >> 1) ^ to_accumulate[0]; //last element shift
#else
   int j;

   for (j = length-1 ; j >0 ; j--) {
     in[j] = ((in[j] >> 1) | (in[j-1] << (DIGIT_SIZE_b-1))) ^ to_accumulate[j];
   }
   in[0] = (in[0] >> 1) ^ to_accumulate[j]; //last element shift
#endif
} // end right_bit_shift_n

/******************************************************************************/
static inline
void add_first_by_x_second_by_xsquared(const int lengthout,
                                       DIGIT out[],
                                       const int lengthin1,
                                       DIGIT in1[],
                                       const int lengthin2,
                                       DIGIT in2[]) {
/* Settle first word trivially */
    out[0] = ( (in1[0] >> (DIGIT_SIZE_b-1) ) & 1) ^
             ( (in2[0] >> (DIGIT_SIZE_b-2) ) & 3);
#if (defined HIGH_PERFORMANCE_X86_64)
   int j;
   __m256i a1,b1, a2,b2;

   for(j = 0; j < (lengthin1-1)/4; j++){

     a1 = _mm256_lddqu_si256( (__m256i *) &in1[0] + j);  //load from in[j] to in[j+3]
     b1 = _mm256_lddqu_si256( (__m256i *) &in1[1] + j);  //load from in[j+1] to in[j+4]
     a1 = _mm256_slli_epi64(a1, 1);
     b1 = _mm256_srli_epi64(b1, (DIGIT_SIZE_b-1));

     a2 = _mm256_lddqu_si256( (__m256i *) &in2[0] + j);  //load from in[j] to in[j+3]
     b2 = _mm256_lddqu_si256( (__m256i *) &in2[1] + j);  //load from in[j+1] to in[j+4]
     a2 = _mm256_slli_epi64(a2, 2);
     b2 = _mm256_srli_epi64(b2, (DIGIT_SIZE_b-2));
     __m256i to_store = _mm256_xor_si256(_mm256_or_si256(a1,b1),_mm256_or_si256(a2,b2));

     _mm256_storeu_si256( (__m256i *) &out[1] + j , to_store);
   }

   for(j = j*4; j < lengthin1-1; j++) {
       out[1+j] = ( (in1[j] << 1) | (in1[j+1] >> (DIGIT_SIZE_b-1)) ) ^
                  ( (in2[j] << 2) | (in2[j+1] >> (DIGIT_SIZE_b-2)) );
   }

   out[lengthout-1] = (in1[lengthin1-1] << 1) ^ (in2[lengthin2-1] << 2); //last element shift
#else
   int j;
   DIGIT maskx, maskxsquared;
   maskx = ~(((DIGIT)0x01 << (DIGIT_SIZE_b - 1)) - 1);
   maskxsquared = ~(((DIGIT)0x01 << (DIGIT_SIZE_b - 2)) - 1);

   for (j = 0 ; j < lengthin1-1 ; j++) {
       DIGIT tmp1, tmp2;
       tmp1= (in1[j]<< 1) | ((in1[j+1] & maskx) >> (DIGIT_SIZE_b - 1));
       tmp2= (in2[j]<< 2) | ((in1[j+1] & maskxsquared) >> (DIGIT_SIZE_b - 2));
       out[1+j]= tmp1 ^ tmp2;
   }
#endif

} // end add_first_by_x_second_by_xsquared

static inline
void accu_first_plus_second_by_xcube_plus_one(const int lengthout,
                                              DIGIT out[],
                                              const int lengthin1,
                                              DIGIT in1[]) {
    /* The first word is untouched */
    /* Settle the second word trivially */
    out[1] ^= ( (in1[0] >> (DIGIT_SIZE_b-3)) & 7);
#if (defined HIGH_PERFORMANCE_X86_64)
   int j;
   __m256i a1_orig,a1,b1, in_by_x3p1, acc ;

   for(j = 0; j < (lengthin1-1)/4; j++){

    a1_orig = _mm256_lddqu_si256( (__m256i *) &in1[0] + j);  //load from in[j] to in[j+3]
    b1 = _mm256_lddqu_si256( (__m256i *) &in1[1] + j);  //load from in[j+1] to in[j+4]
    a1 = _mm256_slli_epi64(a1_orig, 3);
    b1 = _mm256_srli_epi64(b1, (DIGIT_SIZE_b-3));

    in_by_x3p1 = _mm256_xor_si256(_mm256_or_si256(a1,b1),a1_orig);

    acc = _mm256_lddqu_si256( (__m256i *) &out[2] + j);  //load from in[j] to in[j+3]
    __m256i to_store = _mm256_xor_si256(acc,in_by_x3p1);
    _mm256_storeu_si256( (__m256i *) &out[2] + j , to_store);
   }

   for(j = j*4; j < lengthin1-1; j++) {
       out[2+j] ^= ((in1[j] << 3) | (in1[j+1] >> (DIGIT_SIZE_b-3))) ^ in1[j];
   }

   out[lengthout-1] ^= (in1[lengthin1-1] << 3) ^ (in1[lengthin1-1]); //last element shift
#else
   int j;
   DIGIT mask;
   mask = ~(((DIGIT)0x01 << (DIGIT_SIZE_b - 3)) - 1);
   for (j = 0 ; j < lengthin1-1 ; j++) {
      out[2+j] ^= ((in1[j] << 3)  | (in1[j+1] & mask) >> (DIGIT_SIZE_b - 3)) ^ in1[j];
   }
   out[lengthout-1] ^= (in1[lengthin1-1] << 3) ^ (in1[lengthin1-1]); //last element shift
#endif
} // end accu_first_plus_second_by_xcube_plus_one
