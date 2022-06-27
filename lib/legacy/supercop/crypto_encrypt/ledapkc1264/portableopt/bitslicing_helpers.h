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

#if (defined HIGH_PERFORMANCE_X86_64)

#define BITSLICED_OPERAND_WIDTH (BITS_TO_REPRESENT(V)+1)
#define SLICE_TYPE __m256i
#define NUM_BITS_IN_BITSLICED_OP (256)
#define NUM_SLICES_GF2X_ELEMENT ( (NUM_DIGITS_GF2X_ELEMENT+3)/ \
                                  (NUM_BITS_IN_BITSLICED_OP/DIGIT_SIZE_b) )

/*bitsliced operand*/
typedef struct {
    SLICE_TYPE slice[BITSLICED_OPERAND_WIDTH];
} bs_operand_t;


static inline
void bitslice_half_adder(SLICE_TYPE  addend_a,
                         SLICE_TYPE  addend_b,
                         SLICE_TYPE* result,
                         SLICE_TYPE* carry_out){
   _mm256_storeu_si256 (result, _mm256_xor_si256(addend_a , addend_b));
   _mm256_storeu_si256 (carry_out, _mm256_and_si256(addend_a , addend_b));
   return;
}

static inline
bs_operand_t bitslice_inc(bs_operand_t a, SLICE_TYPE b){
   bs_operand_t result;
   SLICE_TYPE carry;
   bitslice_half_adder(a.slice[0],b,&(result.slice[0]),&carry);
   for(int i = 1; i<BITSLICED_OPERAND_WIDTH; i++){
       bitslice_half_adder(a.slice[i],
                           carry,
                           &(result.slice[i]),
                           &carry);
   }
   return result;
}

static inline
bs_operand_t slice_constant(uint16_t constant){
   bs_operand_t result;
   __m256i one = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF,
                                    0xFFFFFFFFFFFFFFFF,
                                    0xFFFFFFFFFFFFFFFF,
                                    0xFFFFFFFFFFFFFFFF);
   __m256i zero = _mm256_set_epi64x(0,0,0,0);
   for(int bit = 0; bit < BITSLICED_OPERAND_WIDTH; bit++){
      if ((constant & 1) == 1) {
         result.slice[bit] = one;
      } else {
         result.slice[bit] = zero;
      }
      constant >>=1;
   }
   return result;
}

#else

#define BITSLICED_OPERAND_WIDTH (BITS_TO_REPRESENT(V)+1)
#define NUM_BITS_IN_BITSLICED_OP (DIGIT_SIZE_b)
#define SLICE_TYPE DIGIT
#define NUM_SLICES_GF2X_ELEMENT ( NUM_DIGITS_GF2X_ELEMENT/  \
                                 (NUM_BITS_IN_BITSLICED_OP/DIGIT_SIZE_b) )

/*bitsliced operand, slice zero is the LSB, slice BITSLICED_OPERAND_WIDTH is
 the MSB */
typedef struct {
    SLICE_TYPE slice[BITSLICED_OPERAND_WIDTH];
} bs_operand_t;

static inline
void bitslice_half_adder(SLICE_TYPE  addend_a,
                         SLICE_TYPE  addend_b,
                         SLICE_TYPE* result,
                         SLICE_TYPE* carry_out){
   *result    = addend_a ^ addend_b;
   *carry_out = addend_a & addend_b;
   return;
}

static inline
bs_operand_t bitslice_inc(bs_operand_t a, SLICE_TYPE b){
   bs_operand_t result;
   SLICE_TYPE carry;
   bitslice_half_adder(a.slice[0],b,&(result.slice[0]),&carry);
   for(int i = 1; i<BITSLICED_OPERAND_WIDTH; i++){
       bitslice_half_adder(a.slice[i],
                           carry,
                           &(result.slice[i]),
                           &carry);
   }
   return result;
}

static inline
bs_operand_t slice_constant(int16_t constant){
   bs_operand_t result;
   for(int bit = 0; bit < BITSLICED_OPERAND_WIDTH; bit++){
      result.slice[bit] = (SLICE_TYPE)0 - (constant & 1);
      constant >>=1;
   }
   return result;
}

#endif
