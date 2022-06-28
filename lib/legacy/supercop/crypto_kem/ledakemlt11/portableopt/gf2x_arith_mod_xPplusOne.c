/**
 *
 * <gf2x_arith_mod_xPplusOne.c>
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


#include "gf2x_arith_mod_xPplusOne.h"
#include "rng.h"
#include <string.h>  // memcpy(...), memset(...)
#include "architecture_detect.h"
#include <stdalign.h>
#include "djbsort.h"
#include <assert.h>
/*----------------------------------------------------------------------------*/

/* specialized for nin == 2 * NUM_DIGITS_GF2X_ELEMENT, as it is only used
 * by gf2x_mul */
static inline
void gf2x_mod(DIGIT out[],
              const int nin, const DIGIT in[])
{
  DIGIT aux[NUM_DIGITS_GF2X_ELEMENT+1];
  memcpy(aux, in, (NUM_DIGITS_GF2X_ELEMENT+1)*DIGIT_SIZE_B);
#if MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS != 0
  right_bit_shift_n(NUM_DIGITS_GF2X_ELEMENT+1, aux,
                    MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS);
#endif
  gf2x_add(NUM_DIGITS_GF2X_ELEMENT,out,
           NUM_DIGITS_GF2X_ELEMENT,aux+1,
           NUM_DIGITS_GF2X_ELEMENT,in+NUM_DIGITS_GF2X_ELEMENT);
#if MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS != 0
  out[0] &=  ((DIGIT)1 << MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS) - 1 ;
#endif

} // end gf2x_mod



/*----------------------------------------------------------------------------*/

#if (defined(DIGIT_IS_UINT8) || defined(DIGIT_IS_UINT16))
static
uint8_t byte_reverse_with_less32bitDIGIT(uint8_t b)
{
   uint8_t r = b;
   int s = (sizeof(b) << 3) - 1;
   for (b >>= 1; b; b >>= 1) {
      r <<= 1;
      r |= b & 1;
      s--;
   }
   r <<= s;
   return r;
} // end byte_reverse_less32bitDIGIT
#endif

#if defined(DIGIT_IS_UINT32)
static
uint8_t byte_reverse_with_32bitDIGIT(uint8_t b)
{
   b = ( (b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)
       ) * 0x10101LU >> 16;
   return b;
} // end byte_reverse_32bitDIGIT
#endif

#if defined(DIGIT_IS_UINT64)
static
uint8_t byte_reverse_with_64bitDIGIT(uint8_t b)
{
   b = (b * 0x0202020202ULL & 0x010884422010ULL) % 1023;
   return b;
} // end byte_reverse_64bitDIGIT
#endif

/*----------------------------------------------------------------------------*/

static
DIGIT reverse_digit(const DIGIT b)
{
   int i;
   union toReverse_t {
      uint8_t inByte[DIGIT_SIZE_B];
      DIGIT digitValue;
   } toReverse;

   toReverse.digitValue = b;
#if defined(DIGIT_IS_UINT64)
   for (i = 0; i < DIGIT_SIZE_B; i++)
      toReverse.inByte[i] = byte_reverse_with_64bitDIGIT(toReverse.inByte[i]);
   return __builtin_bswap64(toReverse.digitValue);
#elif defined(DIGIT_IS_UINT32)
   for (i = 0; i < DIGIT_SIZE_B; i++)
      toReverse.inByte[i] = byte_reverse_with_32bitDIGIT(toReverse.inByte[i]);
   return __builtin_bswap32(toReverse.digitValue);
#elif defined(DIGIT_IS_UINT16)
   for (i = 0; i < DIGIT_SIZE_B; i++)
      toReverse.inByte[i] = byte_reverse_with_less32bitDIGIT(toReverse.inByte[i]);
   reversed = __builtin_bswap16(toReverse.digitValue);
#elif defined(DIGIT_IS_UINT8)
   return byte_reverse_with_less32bitDIGIT(toReverse.inByte[0]);
#else
#error "Missing implementation for reverse_digit(...) \
with this CPU word bitsize !!! "
#endif
   return toReverse.digitValue;
} // end reverse_digit


/*----------------------------------------------------------------------------*/

void gf2x_transpose_in_place(DIGIT A[])
{
   /* it keeps the lsb in the same position and
    * inverts the sequence of the remaining bits
    */

   DIGIT mask = (DIGIT)0x1;
   DIGIT rev1, rev2, a00;
   int i, slack_bits_amount = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b - P;

   if (NUM_DIGITS_GF2X_ELEMENT == 1) {
      a00 = A[0] & mask;
      right_bit_shift(1, A);
      rev1 = reverse_digit(A[0]);
#if (NUM_DIGITS_GF2X_MOD_P_ELEMENT*DIGIT_SIZE_b - P)
      rev1 >>= (DIGIT_SIZE_b-(P%DIGIT_SIZE_b));
#endif
      A[0] = (rev1 & (~mask)) | a00;
      return;
   }

   a00 = A[NUM_DIGITS_GF2X_ELEMENT-1] & mask;
   right_bit_shift(NUM_DIGITS_GF2X_ELEMENT, A);

   for (i = NUM_DIGITS_GF2X_ELEMENT-1; i >= (NUM_DIGITS_GF2X_ELEMENT+1)/2; i--) {
      rev1 = reverse_digit(A[i]);
      rev2 = reverse_digit(A[NUM_DIGITS_GF2X_ELEMENT-1-i]);
      A[i] = rev2;
      A[NUM_DIGITS_GF2X_ELEMENT-1-i] = rev1;
   }
   if (NUM_DIGITS_GF2X_ELEMENT % 2 == 1)
      A[NUM_DIGITS_GF2X_ELEMENT/2] = reverse_digit(A[NUM_DIGITS_GF2X_ELEMENT/2]);

   if (slack_bits_amount)
      right_bit_shift_n(NUM_DIGITS_GF2X_ELEMENT, A,slack_bits_amount);
   A[NUM_DIGITS_GF2X_ELEMENT-1] = (A[NUM_DIGITS_GF2X_ELEMENT-1] & (~mask)) | a00;
} // end transpose_in_place

/*----------------------------------------------------------------------------*/
/* computes poly times digit multiplication as a support for KTT inverse */
/* PRE : nr = na + 1 */

#ifdef HIGH_PERFORMANCE_X86_64
#define GF2X_DIGIT_TIMES_POLY_MUL gf2x_digit_times_poly_mul_avx
static
void gf2x_digit_times_poly_mul_avx(const int nr, 
                                     DIGIT Res[NUM_DIGITS_GF2X_ELEMENT+1],
                               const int na, const DIGIT A[],
                               const DIGIT B){

    __m128i prodRes0,prodRes1,
            accumRes,loopCarriedWord,lowToHighWord,
            wideB,wideA;

    int i;
    wideB=_mm_set_epi64x(0, B);
    loopCarriedWord = _mm_set_epi64x(0,0);

    for (i = na-1; i >= 1 ; i=i-2){
      /*wideA contains [ A[i] A[i-1] ] */
      wideA = _mm_lddqu_si128((__m128i *)&A[i-1]);

      prodRes0 = _mm_clmulepi64_si128(wideA, wideB, 1);
      prodRes1 = _mm_clmulepi64_si128(wideA, wideB, 0);

      accumRes = _mm_xor_si128(loopCarriedWord,prodRes0);
      lowToHighWord = _mm_slli_si128(prodRes1,8);
      accumRes = _mm_xor_si128(accumRes,lowToHighWord);

      accumRes = (__m128i) _mm_shuffle_pd( (__m128d) accumRes, 
                                          (__m128d) accumRes, 1);
      _mm_storeu_si128((__m128i *)(&Res[i]), accumRes);

      loopCarriedWord = _mm_srli_si128(prodRes1,8);
    }
    if (i == 0){ /*skipped last iteration i=0, compensate*/
      prodRes0 = _mm_clmulepi64_si128(_mm_set_epi64x(0, A[0]), wideB, 0);

      accumRes = loopCarriedWord;
      accumRes = _mm_xor_si128(accumRes,prodRes0);
      accumRes = (__m128i) _mm_shuffle_pd( (__m128d) accumRes,
                                           (__m128d) accumRes, 1);
      _mm_storeu_si128((__m128i *)(&Res[0]), accumRes);
    } else { /*i == 1*/
        /*regular exit condition, do nothing*/
    }

}

#else
#define GF2X_DIGIT_TIMES_POLY_MUL gf2x_digit_times_poly_mul

void gf2x_digit_times_poly_mul(const int nr, DIGIT Res[NUM_DIGITS_GF2X_ELEMENT+1],
                               const int na, const DIGIT A[],
                               const DIGIT B){

    DIGIT pres[2];
    Res[nr-1]=0;
    for (int i = (nr-1)-1; i >= 0 ; i--){
       GF2X_MUL(2, pres, 1, &A[i], 1, &B);
       Res[i+1] = Res[i+1] ^ pres[1];
       Res[i] =  pres[0];
    }
}
#endif

/*----------------------------------------------------------------------------
*
* Based on: K. Kobayashi, N. Takagi and K. Takagi, "Fast inversion algorithm in 
* GF(2m) suitable for implementation with a polynomial multiply instruction on 
* GF(2)," in IET Computers & Digital Techniques, vol. 6, no. 3, pp. 180-185, 
* May 2012. doi: 10.1049/iet-cdt.2010.0006
*/

int gf2x_mod_inverse_KTT(DIGIT out[], const DIGIT in[]){  /* in^{-1} mod x^P-1 */

#if NUM_DIGITS_GF2X_MODULUS == NUM_DIGITS_GF2X_ELEMENT
 DIGIT s[NUM_DIGITS_GF2X_ELEMENT+1] = {0},
       r[NUM_DIGITS_GF2X_ELEMENT+1];
 r[0]=0;
 memcpy(r+1,in, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

 /* S starts set to the modulus */
 s[NUM_DIGITS_GF2X_ELEMENT+1-1] = 1;
 s[0+1] |= ((DIGIT)1) << MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS;

 DIGIT v[2*NUM_DIGITS_GF2X_ELEMENT] = {0}, 
       u[2*NUM_DIGITS_GF2X_ELEMENT] = {0};

 u[2*NUM_DIGITS_GF2X_ELEMENT-1] = (DIGIT) 2; /* x */

 int deg_r = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b -1;
 int deg_s = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b -1;

 DIGIT c,d;
 DIGIT h00,h01,h10,h11;

 DIGIT hibitmask = ( (DIGIT) 1) << (DIGIT_SIZE_b-1);

 DIGIT r_h00[NUM_DIGITS_GF2X_ELEMENT+2];
 DIGIT s_h01[NUM_DIGITS_GF2X_ELEMENT+2];
 DIGIT r_h10[NUM_DIGITS_GF2X_ELEMENT+2];
 DIGIT s_h11[NUM_DIGITS_GF2X_ELEMENT+2];
 DIGIT u_h00[2*NUM_DIGITS_GF2X_ELEMENT+1];
 DIGIT v_h01[2*NUM_DIGITS_GF2X_ELEMENT+1];
 DIGIT u_h10[2*NUM_DIGITS_GF2X_ELEMENT+1];
 DIGIT v_h11[2*NUM_DIGITS_GF2X_ELEMENT+1];

 while(deg_r > 0){
     c=r[1];
     d=s[1];
     if(c == 0){
        left_DIGIT_shift_n(NUM_DIGITS_GF2X_ELEMENT+1,r,1);
        left_DIGIT_shift_n(2*NUM_DIGITS_GF2X_ELEMENT,u,1);
         deg_r = deg_r - DIGIT_SIZE_b;
     } else {
        /* H = I */
        h00 = 1; h01 = 0;
        h10 = 0; h11 = 1;
        for(int j = 1 ; (j < DIGIT_SIZE_b) && (deg_r > 0) ;j++) {
           if ( (c & hibitmask) == 0){ /* */
               c = c << 1;

               h00 = h00 << 1; 
               h01 = h01 << 1;
               deg_r--;
           } else { /* hibit r[0] set */
               if (deg_r == deg_s){
                 deg_r--;
                 if ( (d & hibitmask) == hibitmask){ /* hibit r[0],s[0] set, deg_r == deg_s */
                    DIGIT temp = c;
                    c = (c^d) << 1; /* (c-d)*x */
                    d = temp;
                    /*mult H*/
                    DIGIT r00;
                    r00 = (h00 << 1) ^ (h10 << 1);
                    DIGIT r01;
                    r01 = (h01 << 1) ^ (h11 << 1);
                    h10 = h00;
                    h11 = h01;
                    h00 = r00;
                    h01 = r01;
                 } else { /* hibit r[0] set, s[0] unset, deg_r == deg_s */
                    DIGIT temp;
                    temp = c;
                    c = d << 1;
                    d = temp;
                    /*mult H*/
                    DIGIT r00;
                    r00 = (h10 << 1);
                    DIGIT r01;
                    r01 = (h11 << 1);
                    h10 = h00; 
                    h11 = h01;
                    h00 = r00;
                    h01 = r01;
                 }
               } else { /* if (deg_r != deg_s) */
                  deg_s--;
                  if ( (d & hibitmask) == hibitmask){ /* hibit r[0],s[0] set, deg_r != deg_s */
                     d = (c^d) << 1; /* (c-d) * x*/
                     /* mult H */
                     h10 = (h00 << 1) ^ (h10 << 1);
                     h11 = (h01 << 1) ^ (h11 << 1);
                  } else { /* hibit r[0] set, s[0] unset, deg_r != deg_s */
                     d = d << 1;
                     /*mul H*/

                     h10 = h10 << 1; 
                     h11 = h11 << 1;
                  }
               } /*(deg_r == deg_s)*/
           } /* if ( (c & ((DIGIT 1) << (DIGIT_SIZE_b-1))) == 0) */
        } /* while */
        /*update r , s */

        GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, r_h00,
                                  NUM_DIGITS_GF2X_ELEMENT+1, r,
                                  h00);
        GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, s_h01,
                                  NUM_DIGITS_GF2X_ELEMENT+1, s,
                                  h01);
        GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, r_h10,
                                  NUM_DIGITS_GF2X_ELEMENT+1, r,
                                  h10);
        GF2X_DIGIT_TIMES_POLY_MUL(NUM_DIGITS_GF2X_ELEMENT+2, s_h11,
                                  NUM_DIGITS_GF2X_ELEMENT+1, s,
                                  h11);

        gf2x_add(NUM_DIGITS_GF2X_ELEMENT+1, r,
                 NUM_DIGITS_GF2X_ELEMENT+1, r_h00+1,
                 NUM_DIGITS_GF2X_ELEMENT+1, s_h01+1);

        gf2x_add(NUM_DIGITS_GF2X_ELEMENT+1, s,
                 NUM_DIGITS_GF2X_ELEMENT+1, r_h10+1,
                 NUM_DIGITS_GF2X_ELEMENT+1, s_h11+1);

        /* *********************** update u, v *************************/
        GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, u_h00,
                                  2*NUM_DIGITS_GF2X_ELEMENT, u,
                                  h00);
        GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, v_h01,
                                  2*NUM_DIGITS_GF2X_ELEMENT, v,
                                  h01);
        GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, u_h10,
                                  2*NUM_DIGITS_GF2X_ELEMENT, u,
                                  h10);
        GF2X_DIGIT_TIMES_POLY_MUL(2*NUM_DIGITS_GF2X_ELEMENT+1, v_h11,
                                  2*NUM_DIGITS_GF2X_ELEMENT, v,
                                  h11);

        gf2x_add(2*NUM_DIGITS_GF2X_ELEMENT, u,
                 2*NUM_DIGITS_GF2X_ELEMENT, u_h00+1,
                 2*NUM_DIGITS_GF2X_ELEMENT, v_h01+1);
        gf2x_add(2*NUM_DIGITS_GF2X_ELEMENT, v,
                 2*NUM_DIGITS_GF2X_ELEMENT, u_h10+1,
                 2*NUM_DIGITS_GF2X_ELEMENT, v_h11+1);
     }
 }
 if (deg_r == 0) {
  memcpy(out,u,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
 }
 else {
  memcpy(out,v,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
 }
#else
#error IMPLEMENT MEMCPY INTO A LARGER OPERAND
#endif

 return 0;
}



/*----------------------------------------------------------------------------*/

void gf2x_scalarprod(int nr, DIGIT Res[],
                     int na, DIGIT a0[], DIGIT a1[],
                     int nb, DIGIT b0[], DIGIT b1[]
                     )
{
    if(na == nb){
       DIGIT tmp[nr];
       GF2X_MUL(nr,Res, na,a0, nb,b0);
       GF2X_MUL(nr,tmp, na,a1, nb,b1);
       gf2x_add(nr, Res, nr, tmp, nr, Res);
    } else if (na > nb){
        DIGIT   tmp[na*2];

        DIGIT  bufb[na];
        memset(bufb,0x00,(na-nb)*DIGIT_SIZE_B);
        memcpy(bufb+(na-nb),b0,nb*DIGIT_SIZE_B);
        GF2X_MUL(na*2,tmp, na,a0, na,bufb);

        DIGIT  tmp2[na*2];

        memset(bufb,0x00,(na-nb)*DIGIT_SIZE_B);
        memcpy(bufb+(na-nb),b1,nb*DIGIT_SIZE_B);

        GF2X_MUL(na*2,tmp2, na,a1, na,bufb);
        gf2x_add(na*2, tmp2, na*2, tmp, na*2, tmp2);

        memcpy(Res,tmp2+(na-nb),nr*DIGIT_SIZE_B);
    } else /*nb > na*/ {
        DIGIT   tmp[nb*2];

        DIGIT  bufa[nb];
        memset(bufa,0x00,(nb-na)*DIGIT_SIZE_B);
        memcpy(bufa+(nb-na),a0,na*DIGIT_SIZE_B);
        GF2X_MUL(nb*2,tmp, nb, bufa, nb,b0);

        DIGIT  tmp2[nb*2];

        memset(bufa,0x00,(nb-na)*DIGIT_SIZE_B);
        memcpy(bufa+(nb-na),a1,na*DIGIT_SIZE_B);

        GF2X_MUL(nb*2,tmp2, nb,bufa, nb,b1);

        gf2x_add(nb*2, tmp2, nb*2, tmp, nb*2, tmp2);
        memcpy(Res,tmp2+(nb*2-(nb+na)),(na+nb)*DIGIT_SIZE_B);
    }
}


#define CTIME_IF(mask,then,else)  ((mask&(then)) | (~mask&(else) ))

#define SIGNED_DIGIT int64_t
static inline
int divstepsx(int n, int t, 
              int delta, 
              DIGIT f64, DIGIT g64,
              DIGIT * p00, DIGIT * p01, 
              DIGIT * p10, DIGIT * p11) {

    DIGIT u, v, q, r;
    SIGNED_DIGIT g0, f0;

    //int a = 2*P-1;
    u = ((DIGIT) 1) << n;
    v = 0;
    q = 0;
    r = ((DIGIT) 1) << n;
    DIGIT tmp,tmp2;
    
    while (n > 0) {
      SIGNED_DIGIT swap_mask = ((delta > 0) & ((g64 & 1) != 0));
      swap_mask = (swap_mask << (DIGIT_SIZE_b-1)) >> (DIGIT_SIZE_b-1);
      delta = CTIME_IF(swap_mask,-delta,delta);
      tmp  = CTIME_IF(swap_mask,g64,f64);
      tmp2 = CTIME_IF(swap_mask,f64,g64);
      f64  = tmp;
      g64  = tmp2;
      
      tmp  = CTIME_IF(swap_mask,q,u);
      tmp2 = CTIME_IF(swap_mask,u,q);
      u  = tmp;
      q  = tmp2;
      
      tmp  = CTIME_IF(swap_mask,r,v);
      tmp2 = CTIME_IF(swap_mask,v,r);
      v  = tmp;
      r  = tmp2;

      delta++;
      g0 = (((SIGNED_DIGIT) (g64 & (DIGIT) 0x1)) << (DIGIT_SIZE_b - 1)) >>
          (DIGIT_SIZE_b - 1);
      f0 = (((SIGNED_DIGIT) (f64 & (DIGIT) 0x1)) << (DIGIT_SIZE_b - 1)) >>
          (DIGIT_SIZE_b - 1);
      q =   (f0 & q) ^ (g0 & u);
      r =   (f0 & r) ^ (g0 & v);
      g64 = (f0 & g64) ^ (g0 & f64);
      
      g64 >>= 1;
      q >>= 1;
      r >>= 1;
      n--;
      t--;
   } //end while
    *p00 = u;
    *p01 = v;
    *p10 = q;
    *p11 = r;

    return delta;
}

/*  truncates polynomial inout to degree, zeroing other coefficients, 
 *  returns pointer to truncated polynomial region */
DIGIT* gf2x_trunc(int inDigitLen, DIGIT inout[], int degree){

    int straightIdx = (inDigitLen*DIGIT_SIZE_b -1) - degree;
    int digitIdx = straightIdx / DIGIT_SIZE_b;
    unsigned int inDigitIdx = straightIdx % DIGIT_SIZE_b;
    /*poly does not fill the MS digit, clear slack*/
    if(inDigitIdx != 0){
        DIGIT mask = (( (DIGIT) 1) << (DIGIT_SIZE_b-1-inDigitIdx+1))- ((DIGIT)1);
        inout[digitIdx] &= mask;
    }
    return inout+digitIdx;
}

int jumpdivstep(int n, int t, int delta,
                int nf, DIGIT   f[], DIGIT g[],
                DIGIT t00[], DIGIT t01[],
                DIGIT t10[], DIGIT t11[])
{

    if (n <= DIGIT_SIZE_b-1) {
     delta = divstepsx(n, t, delta, f[0],
            g[0],
            t00, t01,
            t10, t11);
    } else {

   /* round the cutting point to a digit limit */
   int j = n / 2;
   j = (j+DIGIT_SIZE_b-2)/(DIGIT_SIZE_b-1);
   j = j * (DIGIT_SIZE_b-1);

   int num_digits_j       = j/DIGIT_SIZE_b+1; /* (j+DIGIT_SIZE_b-1)/DIGIT_SIZE_b */;
   DIGIT p_00[num_digits_j],p_01[num_digits_j],
         p_10[num_digits_j],p_11[num_digits_j];

   /* note: these local_f and local_g will be used by the downward call 
    * they must be dup'ed and trimmed to the proper digit */
   
   DIGIT local_f[num_digits_j];
   DIGIT local_g[num_digits_j];
   
   memcpy(local_f, f+(nf-num_digits_j), num_digits_j * DIGIT_SIZE_B);
   memcpy(local_g, g+(nf-num_digits_j), num_digits_j * DIGIT_SIZE_B);

   DIGIT *trunc_f, *trunc_g;
   trunc_f = gf2x_trunc(num_digits_j, local_f, j);
   trunc_g = gf2x_trunc(num_digits_j, local_g, j);

   delta = jumpdivstep(j, j, delta, num_digits_j,
                       trunc_f, trunc_g, 
                       p_00, p_01, p_10, p_11);

   /* note: entire f and g must be matrixmultiplied! use the ones from above */
   DIGIT f_sum[num_digits_j+nf];
   DIGIT g_sum[num_digits_j+nf];

   gf2x_scalarprod(num_digits_j+nf, f_sum,
                   num_digits_j,    p_00, p_01,
                   nf,              f, g);

   gf2x_scalarprod(num_digits_j+nf, g_sum,
                   num_digits_j,    p_10, p_11,
                   nf,                    f, g);


   right_bit_shift_wide_n(num_digits_j+nf, f_sum, j);
   right_bit_shift_wide_n(num_digits_j+nf, g_sum, j);

   /* truncate to n-j degree, i.e. to n-j bits from the bottom */
   int num_digits_nminusj = (n-j)/DIGIT_SIZE_b+1;

   DIGIT  q_00[num_digits_nminusj],
          q_01[num_digits_nminusj],
          q_10[num_digits_nminusj], 
          q_11[num_digits_nminusj];

   delta = jumpdivstep(n - j, n - j, delta, 
                       num_digits_nminusj,
                       gf2x_trunc(num_digits_j+nf, f_sum, n-j), 
                       gf2x_trunc(num_digits_j+nf, g_sum, n-j), 
                       q_00, q_01, q_10, q_11);

   DIGIT large_tmp[num_digits_j+num_digits_nminusj];

   gf2x_scalarprod(num_digits_j+num_digits_nminusj, large_tmp,
                   num_digits_j,                    p_00, p_10,
                   num_digits_nminusj,                    q_00, q_01);
   memcpy(t00,
          large_tmp+(num_digits_j+num_digits_nminusj-nf),
          (nf)*DIGIT_SIZE_B);

   gf2x_scalarprod(num_digits_j+num_digits_nminusj, large_tmp,
                   num_digits_j,                    p_01, p_11,
                   num_digits_nminusj,                    q_00, q_01);
   memcpy(t01,
          large_tmp+(num_digits_j+num_digits_nminusj-nf),
          (nf)*DIGIT_SIZE_B);

   gf2x_scalarprod(num_digits_j+num_digits_nminusj, large_tmp,
                   num_digits_j,                    p_00, p_10,
                   num_digits_nminusj,                    q_10, q_11);
   memcpy(t10,
          large_tmp+(num_digits_j+num_digits_nminusj-nf),
          (nf)*DIGIT_SIZE_B);

   gf2x_scalarprod(num_digits_j+num_digits_nminusj, large_tmp,
                   num_digits_j,                    p_01, p_11,
                   num_digits_nminusj,                    q_10, q_11);
   memcpy(t11,
          large_tmp+(num_digits_j+num_digits_nminusj-nf),
          (nf)*DIGIT_SIZE_B);
   }
   return delta;
}

void gf2x_reflect_in_place(DIGIT A[])
{
   DIGIT rev1,rev2;
   for (int i = NUM_DIGITS_GF2X_ELEMENT-1; i >= (NUM_DIGITS_GF2X_ELEMENT+1)/2; i--) {
      rev1 = reverse_digit(A[i]);
      rev2 = reverse_digit(A[NUM_DIGITS_GF2X_ELEMENT-1-i]);
      A[i] = rev2;
      A[NUM_DIGITS_GF2X_ELEMENT-1-i] = rev1;
   }
   if (NUM_DIGITS_GF2X_ELEMENT % 2 == 1)
      A[NUM_DIGITS_GF2X_ELEMENT/2] = reverse_digit(A[NUM_DIGITS_GF2X_ELEMENT/2]);
} // end transpose_in_place


#define MATRIX_ELEM_DIGITS ((2 * P - 1)/DIGIT_SIZE_b+1)
int gf2x_mod_inverse_DJB(DIGIT out[], const DIGIT in[])
{
#if NUM_DIGITS_GF2X_MODULUS == NUM_DIGITS_GF2X_ELEMENT
    DIGIT f[NUM_DIGITS_GF2X_ELEMENT] = { 0 },   // S(x)
    g[NUM_DIGITS_GF2X_ELEMENT] = { 0 };   // R(x)

    DIGIT u[MATRIX_ELEM_DIGITS] = { 0 },
          v[MATRIX_ELEM_DIGITS] = { 0 },
          q[MATRIX_ELEM_DIGITS] = { 0 }, 
          r[MATRIX_ELEM_DIGITS] = { 0 };

    int delta = 1;

    memcpy(g, in, NUM_DIGITS_GF2X_ELEMENT * DIGIT_SIZE_B);

    f[0] |= (((DIGIT) 0x1) << MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS);
    f[NUM_DIGITS_GF2X_ELEMENT - 1] = 0x1;

    int slack_bits_amount = (DIGIT_SIZE_b * NUM_DIGITS_GF2X_ELEMENT) - P;

    gf2x_reflect_in_place(f);
    right_bit_shift_n(NUM_DIGITS_GF2X_ELEMENT, f, slack_bits_amount - 1);
    gf2x_reflect_in_place(g);
    right_bit_shift_n(NUM_DIGITS_GF2X_ELEMENT, g, slack_bits_amount);
 
    DIGIT largef[MATRIX_ELEM_DIGITS], largeg[MATRIX_ELEM_DIGITS];
    memset(largef,0x00,MATRIX_ELEM_DIGITS*DIGIT_SIZE_B);
    memcpy(largef+(MATRIX_ELEM_DIGITS-NUM_DIGITS_GF2X_ELEMENT),
           f,
           NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
    memset(largeg,0x00,MATRIX_ELEM_DIGITS*DIGIT_SIZE_B);
    memcpy(largeg+(MATRIX_ELEM_DIGITS-NUM_DIGITS_GF2X_ELEMENT),
           g,
           NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

    delta = jumpdivstep(2 * P - 1, 2 * P - 1, 
                        delta, MATRIX_ELEM_DIGITS, 
                        largef, largeg, u, v, q, r);


    /* Since I should reverse d-1 I can just reverse on d after dividing by x
       return kx( x^(2*d-2)*P[0][1] /f[0]).reverse(d-1) */
    right_bit_shift_n(MATRIX_ELEM_DIGITS, v, 1);

    gf2x_reflect_in_place(v + (MATRIX_ELEM_DIGITS - NUM_DIGITS_GF2X_ELEMENT));

    /* reflection is full-word-wise, shift away the slack bits */
    right_bit_shift_n(NUM_DIGITS_GF2X_ELEMENT, v + (MATRIX_ELEM_DIGITS - NUM_DIGITS_GF2X_ELEMENT), slack_bits_amount);

    memcpy(out, v + (MATRIX_ELEM_DIGITS - NUM_DIGITS_GF2X_ELEMENT),
      NUM_DIGITS_GF2X_ELEMENT * DIGIT_SIZE_B);

    DIGIT clear_slack_mask = ( ((DIGIT) 1) << 
                               ((DIGIT_SIZE_b) - slack_bits_amount) ) - 1;
    out[0] = out[0] & clear_slack_mask;

#else
#error IMPLEMENT MEMCPY INTO A LARGER OPERAND
#endif
    return 0;
}

// /*----------------------------------------------------------------------------*/
// 
// void gf2x_digit_times_poly_mul_avx_b(const int nr, DIGIT Res[NUM_DIGITS_GF2X_ELEMENT+1],
//                                const int na, const DIGIT A[],
//                                const DIGIT B){
// 
//     __m128i prodRes0,prodRes1,
//             accumRes,loopCarriedWord,lowToHighWord,
//             wideB,wideA;
// 
//     int i;
//     wideB=_mm_set_epi64x(0, B);
//     loopCarriedWord = _mm_set_epi64x(0,0);
// 
//     for (i = na-1; i >= 1 ; i=i-2){
//       /*wideA contains [ A[i] A[i-1] ] */
//       wideA = _mm_lddqu_si128((__m128i *)&A[i-1]);
// 
//       prodRes0 = _mm_clmulepi64_si128(wideA, wideB, 1);
//       prodRes1 = _mm_clmulepi64_si128(wideA, wideB, 0);
// 
//       accumRes = _mm_xor_si128(loopCarriedWord,prodRes0);
//       lowToHighWord = _mm_slli_si128(prodRes1,8);
//       accumRes = _mm_xor_si128(accumRes,lowToHighWord);
// 
//       accumRes = (__m128i) _mm_shuffle_pd( (__m128d) accumRes,
//                                           (__m128d) accumRes, 1);
//       STORE_m128_INTO_u64(&Res[i], accumRes);
// 
//       loopCarriedWord = _mm_srli_si128(prodRes1,8);
//     }
//     if (i == 0){
//         /*skipped last iteration sith i=0, compensate*/
//       prodRes0 = _mm_clmulepi64_si128(_mm_set_epi64x(0, A[0]), wideB, 0);
// 
//       accumRes = loopCarriedWord;
//       accumRes = _mm_xor_si128(accumRes,prodRes0);
//       accumRes = (__m128i) _mm_shuffle_pd( (__m128d) accumRes,
//                                            (__m128d) accumRes, 1);
//       STORE_m128_INTO_u64(&Res[0], accumRes);
//     } else { /*i == 1*/
//         /*regular exit condition, do nothing*/
//     }
// 
// }
// 
// 
// /*----------------------------------------------------------------------------*/
// 
// int gf2x_mod_inverse_KTT(DIGIT out[], const DIGIT in[]){
// 
// #if NUM_DIGITS_GF2X_MODULUS == NUM_DIGITS_GF2X_ELEMENT
//  DIGIT s[NUM_DIGITS_GF2X_ELEMENT+1] = {0},
//        r[NUM_DIGITS_GF2X_ELEMENT+1];
//  r[0]=0;
//  memcpy(r+1,in, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
// 
//  /* S starts set to the modulus */
//  s[NUM_DIGITS_GF2X_ELEMENT+1-1] = 1;
//  s[0+1] |= ((DIGIT)1) << MSb_POSITION_IN_MSB_DIGIT_OF_MODULUS;
// 
//  DIGIT v[2*NUM_DIGITS_GF2X_ELEMENT] = {0},
//        u[2*NUM_DIGITS_GF2X_ELEMENT] = {0};
// 
//  u[2*NUM_DIGITS_GF2X_ELEMENT-1] = (DIGIT) 2; /* x */
// 
//  int deg_r = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b -1;
//  int deg_s = NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_b -1;
// 
//  DIGIT c,d;
//  DIGIT h00,h01,h10,h11;
// 
//  DIGIT hibitmask = ( (DIGIT) 1) << (DIGIT_SIZE_b-1);
// 
//  DIGIT r_h00[NUM_DIGITS_GF2X_ELEMENT+2];
//  DIGIT s_h01[NUM_DIGITS_GF2X_ELEMENT+2];
//  DIGIT r_h10[NUM_DIGITS_GF2X_ELEMENT+2];
//  DIGIT s_h11[NUM_DIGITS_GF2X_ELEMENT+2];
//  DIGIT u_h00[2*NUM_DIGITS_GF2X_ELEMENT+1];
//  DIGIT v_h01[2*NUM_DIGITS_GF2X_ELEMENT+1];
//  DIGIT u_h10[2*NUM_DIGITS_GF2X_ELEMENT+1];
//  DIGIT v_h11[2*NUM_DIGITS_GF2X_ELEMENT+1];
// 
//  while(deg_r > 0){
//      c=r[1];
//      d=s[1];
//      /* H = I */
//      h00 = 1; h01 = 0;
//      h10 = 0; h11 = 1;
//      for(int j = 1 ; (j < DIGIT_SIZE_b) && (deg_r > 0) ;j++) {
//         DIGIT is_g0_set = ((c & hibitmask) == hibitmask);
//         DIGIT is_f0_set = ((d & hibitmask) == hibitmask);
//         if ( !is_g0_set ){ /* g(0) == 0 */
//             c = c << 1;
//             h00 = h00 << 1;
//             h01 = h01 << 1;
//             deg_r--;
//         } else { /*  g(0) != 0  */
//             d = (c * is_f0_set ^ d * is_g0_set) << 1; /* (c-d) * x*/
//             h10 = (h00 << 1) * is_f0_set ^ (h10 << 1) * is_g0_set;
//             h11 = (h01 << 1) * is_f0_set ^ (h11 << 1) * is_g0_set;
//             if (deg_r == deg_s){ /* delta == 0 */
//                 deg_r--;
//                 /* swap c-d */
//                 DIGIT temp = c;
//                 c=d;
//                 d=temp;
//                 /* swap H00 - H10*/
//                 temp = h10;
//                 h10 = h00;
//                 h00 = temp;
//                 /* swap H00 - H10*/
//                 temp = h11;
//                 h11 = h01;
//                 h01 = temp;
//             } else { /* delta != 0 */
//                 deg_s--;
//             }
//         } /* if ( (c & ((DIGIT 1) << (DIGIT_SIZE_b-1))) == 0) */
//      } /* while */
//      /*update r , s */
// 
//      gf2x_digit_times_poly_mul_avx_b(NUM_DIGITS_GF2X_ELEMENT+2, r_h00,
//                                NUM_DIGITS_GF2X_ELEMENT+1, r,
//                                h00);
//      gf2x_digit_times_poly_mul_avx_b(NUM_DIGITS_GF2X_ELEMENT+2, s_h01,
//                                NUM_DIGITS_GF2X_ELEMENT+1, s,
//                                h01);
//      gf2x_digit_times_poly_mul_avx_b(NUM_DIGITS_GF2X_ELEMENT+2, r_h10,
//                                NUM_DIGITS_GF2X_ELEMENT+1, r,
//                                h10);
//      gf2x_digit_times_poly_mul_avx_b(NUM_DIGITS_GF2X_ELEMENT+2, s_h11,
//                                NUM_DIGITS_GF2X_ELEMENT+1, s,
//                                h11);
// 
//      gf2x_add(NUM_DIGITS_GF2X_ELEMENT+1, r,
//               NUM_DIGITS_GF2X_ELEMENT+1, r_h00+1,
//               NUM_DIGITS_GF2X_ELEMENT+1, s_h01+1);
// 
//      gf2x_add(NUM_DIGITS_GF2X_ELEMENT+1, s,
//               NUM_DIGITS_GF2X_ELEMENT+1, r_h10+1,
//               NUM_DIGITS_GF2X_ELEMENT+1, s_h11+1);
// 
//      /* *********************** update u, v *************************/
//      gf2x_digit_times_poly_mul_avx_b(2*NUM_DIGITS_GF2X_ELEMENT+1, u_h00,
//                                2*NUM_DIGITS_GF2X_ELEMENT, u,
//                                h00);
//      gf2x_digit_times_poly_mul_avx_b(2*NUM_DIGITS_GF2X_ELEMENT+1, v_h01,
//                                2*NUM_DIGITS_GF2X_ELEMENT, v,
//                                h01);
//      gf2x_digit_times_poly_mul_avx_b(2*NUM_DIGITS_GF2X_ELEMENT+1, u_h10,
//                                2*NUM_DIGITS_GF2X_ELEMENT, u,
//                                h10);
//      gf2x_digit_times_poly_mul_avx_b(2*NUM_DIGITS_GF2X_ELEMENT+1, v_h11,
//                                2*NUM_DIGITS_GF2X_ELEMENT, v,
//                                h11);
// 
//      gf2x_add(2*NUM_DIGITS_GF2X_ELEMENT, u,
//               2*NUM_DIGITS_GF2X_ELEMENT, u_h00+1,
//               2*NUM_DIGITS_GF2X_ELEMENT, v_h01+1);
//      gf2x_add(2*NUM_DIGITS_GF2X_ELEMENT, v,
//               2*NUM_DIGITS_GF2X_ELEMENT, u_h10+1,
//               2*NUM_DIGITS_GF2X_ELEMENT, v_h11+1);
//  }
//  if (deg_r == 0) {
//   memcpy(out,u,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
//  }
//  else {
//   memcpy(out,v,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
//  }
// #else
// #error IMPLEMENT MEMCPY INTO A LARGER OPERAND
// #endif
// 
//  return 0;
// }
/*----------------------------------------------------------------------------*/


void gf2x_mod_mul(DIGIT Res[], const DIGIT A[], const DIGIT B[])
{

   DIGIT aux[2*NUM_DIGITS_GF2X_ELEMENT];
   GF2X_MUL(2*NUM_DIGITS_GF2X_ELEMENT, aux,
                 NUM_DIGITS_GF2X_ELEMENT, A,
                 NUM_DIGITS_GF2X_ELEMENT, B);
   gf2x_mod(Res, 2*NUM_DIGITS_GF2X_ELEMENT, aux);

} // end gf2x_mod_mul

/*----------------------------------------------------------------------------*/
/* computes operand*x^shiftAmt + Res. assumes res is  
 * wide and operand is NUM_DIGITS_GF2X_ELEMENT with blank slack bits */
static inline
void gf2x_fmac(DIGIT Res[], 
               const DIGIT operand[],
               const unsigned int shiftAmt){
    
#if defined(HIGH_PERFORMANCE_X86_64)
   DIGIT shiftedOp[NUM_DIGITS_GF2X_ELEMENT*2] = {0};
   memcpy(shiftedOp+NUM_DIGITS_GF2X_ELEMENT,
          operand, 
          NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   left_bit_shift_wide_n(NUM_DIGITS_GF2X_ELEMENT*2, 
                         shiftedOp, shiftAmt);
   gf2x_add(NUM_DIGITS_GF2X_ELEMENT*2, Res,
            NUM_DIGITS_GF2X_ELEMENT*2, Res,
            NUM_DIGITS_GF2X_ELEMENT*2, shiftedOp);
#else
  unsigned int digitShift = shiftAmt / DIGIT_SIZE_b;
  unsigned int inDigitShift= shiftAmt % DIGIT_SIZE_b;
  DIGIT tmp,prevLo=0;
  int i;
  SIGNED_DIGIT inDigitShiftMask = ((SIGNED_DIGIT) (inDigitShift>0)  << (DIGIT_SIZE_b-1)) >> (DIGIT_SIZE_b-1);
  for(i = NUM_DIGITS_GF2X_ELEMENT-1; i>=0 ; i--){
     tmp = operand[i];
     Res[NUM_DIGITS_GF2X_ELEMENT+i-digitShift] ^= prevLo | (tmp << inDigitShift);
     prevLo = (tmp >> (DIGIT_SIZE_b - inDigitShift)) & inDigitShiftMask;
  }
  Res[NUM_DIGITS_GF2X_ELEMENT+i-digitShift] ^= prevLo;
#endif
}

/*----------------------------------------------------------------------------*/

/*PRE: the representation of the sparse coefficients is sorted in increasing
 order of the coefficients themselves */
void gf2x_mod_mul_dense_to_sparse(DIGIT Res[],
                                  const DIGIT dense[],
                                  const POSITION_T sparse[],
                                  unsigned int nPos)
{
   DIGIT resDouble[2*NUM_DIGITS_GF2X_ELEMENT] = {0};

   for (unsigned int i = 0; i < nPos; i++) { 
       if (sparse[i] != INVALID_POS_VALUE) {
           gf2x_fmac(resDouble, dense ,sparse[i]);
       }
   }
   
   gf2x_mod(Res, 2*NUM_DIGITS_GF2X_ELEMENT, resDouble);

} // end gf2x_mod_mul

/*----------------------------------------------------------------------------*/


void gf2x_transpose_in_place_sparse(int sizeA, POSITION_T A[])
{

   POSITION_T t;
   int i = 0, j;

   if (A[i] == 0) {
      i = 1;
   }
   j = i;

   for (; i < sizeA && A[i] != INVALID_POS_VALUE; i++) {
      A[i] = P-A[i];
   }

   for (i -= 1; j < i; j++, i--) {
      t = A[j];
      A[j] = A[i];
      A[i] = t;
   }

} // end gf2x_transpose_in_place_sparse

/*----------------------------------------------------------------------------*/

void gf2x_mod_mul_sparse(int sizeR, /*number of ones in the result, 
                                     * max sizeA*sizeB */
                         POSITION_T Res[],
                         int sizeA, /*number of ones in A*/
                         const POSITION_T A[],
                         int sizeB, /*number of ones in B*/
                         const POSITION_T B[])
{
   /* compute all the coefficients, filling invalid positions with P*/
   unsigned lastFilledPos=0;
   for(int i = 0 ; i < sizeA ; i++){
     for(int j = 0 ; j < sizeB ; j++){
          uint32_t prod = ((uint32_t) A[i]) + ((uint32_t) B[j]);
          prod = ( (prod >= P) ? prod - P : prod);
       if ((A[i] != INVALID_POS_VALUE) &&
           (B[j] != INVALID_POS_VALUE)){
            Res[lastFilledPos] = prod;
        } else{
            Res[lastFilledPos] = INVALID_POS_VALUE;
        }
        lastFilledPos++;
     }
   }
   while(lastFilledPos < sizeR){
        Res[lastFilledPos] = INVALID_POS_VALUE;
        lastFilledPos++;
   }
   int32_sort((int32_t *)Res, sizeR);
   /* eliminate duplicates */
   POSITION_T lastReadPos = Res[0];
   int duplicateCount;
   int write_idx = 0;
   int read_idx = 0;
   while(read_idx < sizeR  && Res[read_idx] != INVALID_POS_VALUE){
      lastReadPos = Res[read_idx];
      read_idx++;
      duplicateCount=1;
      while( (Res[read_idx] == lastReadPos) && (Res[read_idx] != INVALID_POS_VALUE)){
        read_idx++;
        duplicateCount++;
      }
      if (duplicateCount % 2) {
        Res[write_idx] = lastReadPos;
        write_idx++;
      }
   }
   /* fill remaining cells with INVALID_POS_VALUE */
   for(; write_idx < sizeR; write_idx++) {
      Res[write_idx] = INVALID_POS_VALUE;
   }
} // end gf2x_mod_mul_sparse


/*----------------------------------------------------------------------------*/
/* the implementation is safe even in case A or B alias with the result */
/* PRE: A and B should be sorted and have INVALID_POS_VALUE at the end */
void gf2x_mod_add_sparse(int sizeR,
                         POSITION_T Res[],
                         int sizeA,
                         POSITION_T A[],
                         int sizeB,
                         POSITION_T B[])
{

   POSITION_T tmpRes[sizeR];
   int idxA = 0, idxB = 0, idxR = 0;
   while ( idxA < sizeA  &&
           idxB < sizeB  &&
           A[idxA] != INVALID_POS_VALUE &&
           B[idxB] != INVALID_POS_VALUE ) {

      if (A[idxA] == B[idxB]) {
         idxA++;
         idxB++;
      } else {
         if (A[idxA] < B[idxB]) {
            tmpRes[idxR] = A[idxA];
            idxA++;
         } else {
            tmpRes[idxR] = B[idxB];
            idxB++;
         }
         idxR++;
      }
   }

   while (idxA < sizeA && A[idxA] != INVALID_POS_VALUE) {
      tmpRes[idxR] = A[idxA];
      idxA++;
      idxR++;
   }

   while (idxB < sizeB && B[idxB] != INVALID_POS_VALUE) {
      tmpRes[idxR] = B[idxB];
      idxB++;
      idxR++;
   }

   while (idxR < sizeR) {
      tmpRes[idxR] = INVALID_POS_VALUE;
      idxR++;
   }
   memcpy(Res,tmpRes,sizeof(POSITION_T)*sizeR);

} // end gf2x_mod_add_sparse

/*----------------------------------------------------------------------------*/

/* Return a uniform random value in the range 0..n-1 inclusive,
 * applying a rejection sampling strategy and exploiting as a random source
 * the NIST seedexpander seeded with the proper key.
 * Assumes that the maximum value for the range n is 2^32-1
 */
static
int rand_range(const int n, const int logn, AES_XOF_struct *seed_expander_ctx)
{

   unsigned long required_rnd_bytes = (logn+7)/8;
   unsigned char rnd_char_buffer[4];
   uint32_t rnd_value;
   uint32_t mask = ( (uint32_t)1 << logn) - 1;

   do {
      seedexpander(seed_expander_ctx, rnd_char_buffer, required_rnd_bytes);
      /* obtain an endianness independent representation of the generated random
       bytes into an unsigned integer */
      rnd_value =  ((uint32_t)rnd_char_buffer[3] << 24) +
                   ((uint32_t)rnd_char_buffer[2] << 16) +
                   ((uint32_t)rnd_char_buffer[1] <<  8) +
                   ((uint32_t)rnd_char_buffer[0] <<  0) ;
      rnd_value = mask & rnd_value;
   } while (rnd_value >= n);

   return rnd_value;
} // end rand_range

/*----------------------------------------------------------------------------*/
/* Obtains fresh randomness and seed-expands it until all the required positions
 * for the '1's in the circulant block are obtained */

void rand_circulant_sparse_block(POSITION_T *pos_ones,
                                 const int countOnes,
                                 AES_XOF_struct *seed_expander_ctx)
{

   int duplicated, placedOnes = 0;

   while (placedOnes < countOnes) {
      int p = rand_range(NUM_BITS_GF2X_ELEMENT,
                         BITS_TO_REPRESENT(P),
                         seed_expander_ctx);
      duplicated = 0;
      for (int j = 0; j < placedOnes; j++) if (pos_ones[j] == p) duplicated = 1;
      if (duplicated == 0) {
         pos_ones[placedOnes] = p;
         placedOnes++;
      }
   }
} // rand_circulant_sparse_block

/*----------------------------------------------------------------------------*/

void rand_circulant_blocks_sequence(DIGIT sequence[N0*NUM_DIGITS_GF2X_ELEMENT],
                                    const int countOnes,
                                    AES_XOF_struct *seed_expander_ctx)
{

   int rndPos[countOnes],  duplicated, counter = 0;
   memset(sequence, 0x00, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);


   while (counter < countOnes) {
      int p = rand_range(N0*NUM_BITS_GF2X_ELEMENT,BITS_TO_REPRESENT(P),
                         seed_expander_ctx);
      duplicated = 0;
      for (int j = 0; j < counter; j++) if (rndPos[j] == p) duplicated = 1;
      if (duplicated == 0) {
         rndPos[counter] = p;
         counter++;
      }
   }
   for (int j = 0; j < counter; j++) {
      int polyIndex = rndPos[j] / P;
      int exponent = rndPos[j] % P;
      gf2x_set_coeff( sequence + NUM_DIGITS_GF2X_ELEMENT*polyIndex, exponent,
                      ( (DIGIT) 1));
   }
} // end rand_circulant_blocks_sequence
/*----------------------------------------------------------------------------*/

void rand_error_pos(POSITION_T errorPos[NUM_ERRORS_T],
                    AES_XOF_struct *seed_expander_ctx)
{

   int duplicated, counter = 0;

   while (counter < NUM_ERRORS_T) {
      int p = rand_range(N0*NUM_BITS_GF2X_ELEMENT,BITS_TO_REPRESENT(P),
                         seed_expander_ctx);
      duplicated = 0;
      for (int j = 0; j < counter; j++) if (errorPos[j] == p) duplicated = 1;
      if (duplicated == 0) {
         errorPos[counter] = p;
         counter++;
      }
   }
} // end rand_error_pos

/*----------------------------------------------------------------------------*/

void expand_error(DIGIT sequence[N0*NUM_DIGITS_GF2X_ELEMENT],
                  POSITION_T errorPos[NUM_ERRORS_T])
{
   memset(sequence, 0x00, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

   for (int j = 0; j < NUM_ERRORS_T; j++) {
      int polyIndex = errorPos[j] / P;
      int exponent = errorPos[j] % P;
      gf2x_set_coeff( sequence + NUM_DIGITS_GF2X_ELEMENT*polyIndex, exponent,
                      ( (DIGIT) 1));
   }
} // end rand_circulant_blocks_sequence

/*----------------------------------------------------------------------------*/
