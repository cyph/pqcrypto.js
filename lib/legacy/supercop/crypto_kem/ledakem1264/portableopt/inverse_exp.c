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

#include "gf2x_limbs.h"
#include "gf2x_arith.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include "architecture_detect.h"
#include <stdalign.h>

#if (defined HIGH_PERFORMANCE_X86_64)
static inline
void expand_clmul(uint64_t* input, uint64_t* destination ) {
  /* note, this load swaps lo and hi words */
  __m128i b = _mm_load_si128((__m128i*)input);
  __m128i expandedhi = _mm_clmulepi64_si128(b,b,0);
  __m128i expandedlo = _mm_clmulepi64_si128(b,b,0xFF);
  /*slightly faster, despite using a shufpd under the hood*/
  _mm_storer_pd ((double*) destination, (__m128d)expandedhi);
  _mm_storer_pd ((double*) (destination+2), (__m128d)expandedlo);
}
#endif
static inline
uint64_t expand(uint32_t x)  {
     uint64_t res = x;
    res = (res | (res << 16)) & 0x0000FFFF0000FFFF;
    res = (res | (res << 8)) & 0x00FF00FF00FF00FF;
    res = (res | (res << 4)) & 0x0F0F0F0F0F0F0F0F;
    res = (res | (res << 2)) & 0x3333333333333333;
    res = (res | (res << 1)) & 0x5555555555555555;
    return res;
}

/*
compute poly^2 in F2[X]/(x^P -1)
poly,poly_sq --> dense definition.
save the resalt in poly_sq.
*/
#if (DIGIT_MAX == UINT64_MAX)
#define DIGIT_SIZE_b_BITS 6
#define HALF_DIGIT uint32_t
#elif (DIGIT_MAX == UINT32_MAX)
#define DIGIT_SIZE_b_BITS 5
#define HALF_DIGIT uint16_t
#endif

/* splitting point of the operand, dividing bits to be expanded from bits
to be expanded and folded back. The value is to be subtracted from  */
#define LOW_HALFWORD_MASK ( ((DIGIT)1 << (DIGIT_SIZE_b/2))-1)

/* AVX2 based bit interleaving CLMUL */
void pow_2_A(DIGIT * poly_sq, DIGIT * poly) {

  alignas(32) DIGIT tmp[2*NUM_DIGITS_GF2X_ELEMENT] = {0};

  int outidx = 2*NUM_DIGITS_GF2X_ELEMENT-4;
  int i = NUM_DIGITS_GF2X_ELEMENT-2;
#if (NUM_DIGITS_GF2X_ELEMENT%2==1) /* there may be an extra word to expand*/
  DIGIT word;
  HALF_DIGIT lohalf,hihalf;
  word = poly[NUM_DIGITS_GF2X_ELEMENT-1];
  hihalf = (word >> DIGIT_SIZE_b/2) & LOW_HALFWORD_MASK;
  lohalf = word & LOW_HALFWORD_MASK;
  tmp[2*NUM_DIGITS_GF2X_ELEMENT-1]  = expand(lohalf);
  tmp[2*NUM_DIGITS_GF2X_ELEMENT-2] = expand(hihalf);
  i--;
  outidx -=2;
#endif
  for(; i >= 0; i = i-2){
#if defined(HIGH_PERFORMANCE_X86_64)
      expand_clmul(poly+i,tmp+outidx);
#else
      DIGIT word;
      HALF_DIGIT lohalf,hihalf;
      word = poly[i+1];
      hihalf = (word >> DIGIT_SIZE_b/2) & LOW_HALFWORD_MASK;
      lohalf = word & LOW_HALFWORD_MASK;
      tmp[outidx+3]  = expand(lohalf);
      tmp[outidx+2] = expand(hihalf);

      word = poly[i];
      hihalf = (word >> DIGIT_SIZE_b/2) & LOW_HALFWORD_MASK;
      lohalf = word & LOW_HALFWORD_MASK;
      tmp[outidx+1]  = expand(lohalf);
      tmp[outidx] = expand(hihalf);
#endif
      outidx=outidx-4;
  }
  gf2x_mod(poly_sq,2*NUM_DIGITS_GF2X_ELEMENT, tmp);
  return;
}

#define SQUARE_IMPLEMENTATION pow_2_A
void raise_2_i_clmul(DIGIT* a, int i){
    int64_t actual_exp = 1 << i;
    for (int j = 0; j < actual_exp; j++){
      SQUARE_IMPLEMENTATION(a, a);
    }
}

#if defined(HIGH_PERFORMANCE_X86_64)
#include "inverse_perm_tables.h"
void raise_2_i_inverse_perm_table_linsweep(DIGIT* a, int tableindex){
    DIGIT tmp[NUM_DIGITS_GF2X_ELEMENT];
    memcpy(tmp, a, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
    int d_inDigitIdx;
    /* leading digit has slack bits */
    DIGIT d_digit = 0;
    for( d_inDigitIdx = (DIGIT_SIZE_b-(P%DIGIT_SIZE_b));
         d_inDigitIdx < DIGIT_SIZE_b;
         d_inDigitIdx++){
           unsigned int s_straightIdx = inverse_index_permutation_table[tableindex][d_inDigitIdx];
           unsigned int s_digitIdx = s_straightIdx / DIGIT_SIZE_b;
           unsigned int s_inDigitIdx = s_straightIdx % DIGIT_SIZE_b;
           DIGIT to_move = (tmp[s_digitIdx] >> (DIGIT_SIZE_b-1-s_inDigitIdx)) & ((DIGIT) 1) ;
           d_digit = (d_digit <<1) | to_move;
    }
    a[0] = d_digit;

    /* deal with spare digits which cannot be vectorized */
    int d_digit_idx;
    for(d_digit_idx = 1; d_digit_idx < 1+(NUM_DIGITS_GF2X_ELEMENT%4 + 4-1)%4 ; d_digit_idx++ ){
        d_digit=0;
        for( d_inDigitIdx = 0; d_inDigitIdx < DIGIT_SIZE_b;d_inDigitIdx++){
           unsigned int s_straightIdx = inverse_index_permutation_table[tableindex][d_digit_idx*DIGIT_SIZE_b + d_inDigitIdx];
           unsigned int s_digitIdx = s_straightIdx / DIGIT_SIZE_b;
           unsigned int s_inDigitIdx = s_straightIdx % DIGIT_SIZE_b;
           DIGIT to_move = (tmp[s_digitIdx] >> (DIGIT_SIZE_b-1-s_inDigitIdx)) & ((DIGIT) 1) ;
           d_digit = (d_digit << 1) | to_move;
        }
        a[d_digit_idx] = d_digit;
    }
    /* vectorize the remaining ones */
    __m256i vec_d_digit =_mm256_set_epi64x(0,0,0,0);
    for (; d_digit_idx <= NUM_DIGITS_GF2X_ELEMENT-4; d_digit_idx = d_digit_idx+4 ){


        d_digit=0;
        for( d_inDigitIdx = 0; d_inDigitIdx < DIGIT_SIZE_b; d_inDigitIdx++){
           unsigned int tableElemIdx = d_digit_idx *DIGIT_SIZE_b +
                                       0 *DIGIT_SIZE_b +
                                       d_inDigitIdx;
           unsigned int s_straightIdx =
                  inverse_index_permutation_table[tableindex][tableElemIdx];
           unsigned int s_digitIdx = s_straightIdx / DIGIT_SIZE_b;
           unsigned int s_inDigitIdx = s_straightIdx % DIGIT_SIZE_b;
           DIGIT to_move = (tmp[s_digitIdx] >> (DIGIT_SIZE_b-1-s_inDigitIdx)) & ((DIGIT) 1) ;
           d_digit = (d_digit << 1) | to_move;
        }
        vec_d_digit = _mm256_insert_epi64(vec_d_digit, d_digit, 0);

        d_digit=0;
        for( d_inDigitIdx = 0; d_inDigitIdx < DIGIT_SIZE_b; d_inDigitIdx++){
           unsigned int tableElemIdx = d_digit_idx *DIGIT_SIZE_b +
                                       1 *DIGIT_SIZE_b +
                                       d_inDigitIdx;
           unsigned int s_straightIdx =
                  inverse_index_permutation_table[tableindex][tableElemIdx];
           unsigned int s_digitIdx = s_straightIdx / DIGIT_SIZE_b;
           unsigned int s_inDigitIdx = s_straightIdx % DIGIT_SIZE_b;
           DIGIT to_move = (tmp[s_digitIdx] >> (DIGIT_SIZE_b-1-s_inDigitIdx)) & ((DIGIT) 1) ;
           d_digit = (d_digit << 1) | to_move;
        }
        vec_d_digit = _mm256_insert_epi64(vec_d_digit, d_digit, 1);

        d_digit=0;
        for( d_inDigitIdx = 0; d_inDigitIdx < DIGIT_SIZE_b; d_inDigitIdx++){
           unsigned int tableElemIdx = d_digit_idx *DIGIT_SIZE_b +
                                       2 *DIGIT_SIZE_b +
                                       d_inDigitIdx;
           unsigned int s_straightIdx =
                  inverse_index_permutation_table[tableindex][tableElemIdx];
           unsigned int s_digitIdx = s_straightIdx / DIGIT_SIZE_b;
           unsigned int s_inDigitIdx = s_straightIdx % DIGIT_SIZE_b;
           DIGIT to_move = (tmp[s_digitIdx] >> (DIGIT_SIZE_b-1-s_inDigitIdx)) & ((DIGIT) 1) ;
           d_digit = (d_digit << 1) | to_move;
        }
        vec_d_digit = _mm256_insert_epi64(vec_d_digit, d_digit, 2);

        d_digit=0;
        for( d_inDigitIdx = 0; d_inDigitIdx < DIGIT_SIZE_b; d_inDigitIdx++){
           unsigned int tableElemIdx = d_digit_idx *DIGIT_SIZE_b +
                                       3 *DIGIT_SIZE_b +
                                       d_inDigitIdx;
           unsigned int s_straightIdx =
                  inverse_index_permutation_table[tableindex][tableElemIdx];
           unsigned int s_digitIdx = s_straightIdx / DIGIT_SIZE_b;
           unsigned int s_inDigitIdx = s_straightIdx % DIGIT_SIZE_b;
           DIGIT to_move = (tmp[s_digitIdx] >> (DIGIT_SIZE_b-1-s_inDigitIdx)) & ((DIGIT) 1) ;
           d_digit = (d_digit << 1) | to_move;
        }
        vec_d_digit = _mm256_insert_epi64(vec_d_digit, d_digit,3);
        _mm256_storeu_si256((__m256i*) (a+d_digit_idx), vec_d_digit);
    }
}
#else
#include "inverse_powers_mod_p.h"
void raise_2_i_factor_inverse_permute(DIGIT* a, int tableindex){
    DIGIT tmp[NUM_DIGITS_GF2X_ELEMENT];
    memcpy(tmp, a, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
    for(int dst_exp = 0; dst_exp < P; dst_exp++){
        int exp = ((int64_t)dst_exp *inverse_factors_table[tableindex])%P;
        DIGIT to_move = gf2x_get_coeff(tmp, exp);
        gf2x_set_coeff(a, dst_exp, to_move);
    }
}
#endif

#define RAISE_2_I_TRADEOFF_TH (5)
void raise_2_i_hybrid(DIGIT* a, int i){
    if (i <= RAISE_2_I_TRADEOFF_TH){
       raise_2_i_clmul(a,i);
    }
    else {
#if defined(HIGH_PERFORMANCE_X86_64)
       raise_2_i_inverse_perm_table_linsweep(a,i);
#else
       raise_2_i_factor_inverse_permute(a,i);
#endif
    }
}

void gf2x_mod_inverse_exp(DIGIT * polyInv, DIGIT * poly) {
  uint64_t p = P - 2;
  int i = 1;
  alignas(32) DIGIT b[NUM_DIGITS_GF2X_ELEMENT];
  alignas(32) DIGIT a[NUM_DIGITS_GF2X_ELEMENT];
  alignas(32) DIGIT temp[NUM_DIGITS_GF2X_ELEMENT];

  if ((p & 1) == 0){
    b[NUM_DIGITS_GF2X_ELEMENT - 1] = 0x1;
  }
  else {
    for (int j = 0; j < NUM_DIGITS_GF2X_ELEMENT; j++) {
      b[j] = poly[j];
    }
  }
  for (int j = 0; j < NUM_DIGITS_GF2X_ELEMENT; j++) {
    a[j] = poly[j];
  }
  p >>= 1;
  while (p > 0) {
    for (int j = 0; j < NUM_DIGITS_GF2X_ELEMENT; j++) {
      temp[j] = a[j];
    }
    //a^2^2^(i-1)
    raise_2_i_hybrid(a,i - 1);
    gf2x_mod_mul(a, a, temp);
    if (p & 1) {
      //b^2^2^i
      raise_2_i_hybrid(b,i);
      gf2x_mod_mul(b, b, a);
    }
    p >>= 1;
    i++;
  }
  SQUARE_IMPLEMENTATION(b, b);

  for (i = 0; i < NUM_DIGITS_GF2X_ELEMENT; i++){
    polyInv[i] = b[i];
  }
  return;
}
