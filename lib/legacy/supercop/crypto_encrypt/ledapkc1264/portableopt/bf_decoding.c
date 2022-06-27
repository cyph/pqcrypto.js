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
#if !(defined CONSTANT_TIME)
#define CONSTANT_TIME 1
#endif

#include "bf_decoding.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include "bitslicing_helpers.h"

#define ROTBYTE(a)   ( (a << 8) | (a >> (DIGIT_SIZE_b - 8)) )
#define ROTUPC(a)   ( (a >> 8) | (a << (DIGIT_SIZE_b - 8)) )
#define ROUND_UP(amount, round_amt) ( ((amount+round_amt-1)/round_amt)*round_amt )

#if (defined CONSTANT_TIME)
   /* The second threshold is updated before decoding retrieving it from the
   * private key field secondIterThreshold*/
   int thresholds[2] = {B0 , (V)/2+1};
#endif

void lift_mul_dense_to_sparse_CT(bs_operand_t bs_res[],
                                 // NUM_DIGITS_GF2X_ELEMENT/(NUM_BITS_IN_BITSLICED_OP/DIGIT_SIZE_b) wide
                                  const DIGIT dense[],
                                  const POSITION_T sparse[],
                                  unsigned int nPos){
    SLICE_TYPE tmp[NUM_SLICES_GF2X_ELEMENT];
    for(int i =0; i< nPos; i++){
#if (defined HIGH_PERFORMANCE_X86_64)
        /* note : last words of tmp will be intentionally garbage, in case
         * NUM_DIGITS_GF2X_ELEMENT is not divisible by 4, for alignment reasons
         * Their content won't be used */
        gf2x_mod_mul_monom((DIGIT*)tmp,sparse[i],dense);
#else
        gf2x_mod_mul_monom(tmp,sparse[i],dense);
#endif
        for(int j = 0 ; j < NUM_SLICES_GF2X_ELEMENT; j++) {
            bs_res[j] = bitslice_inc(bs_res[j], tmp[j]);
        }
    }
}

#if (defined CONSTANT_TIME) && (defined HIGH_PERFORMANCE_X86_64)
/*In case a constant time implementation is desired, pick the fastest strategy
 when AVX2 are available, i.e., dense by dense multiplication */
#define DENSE_H
#endif

int bf_decoding_CT(DIGIT out[],
                   const POSITION_T HtrPosOnes[N0][V],
                   const POSITION_T HPosOnes[N0][V],
                   DIGIT privateSyndrome[]){
  bs_operand_t bs_unsatParityChecks[N0*NUM_SLICES_GF2X_ELEMENT];

#if (defined HIGH_PERFORMANCE_X86_64)
   DIGIT deltaerr[N0*4*NUM_SLICES_GF2X_ELEMENT];
#else
   DIGIT deltaerr[N0*NUM_DIGITS_GF2X_ELEMENT];
#endif
#if !(defined CONSTANT_TIME)
   unsigned int synd_corrt_vec[][2]= {SYNDROME_TRESH_LOOKUP_TABLE};
#endif

#if (defined DENSE_H)
   /* densify HTr*/
   DIGIT HTr[N0][NUM_DIGITS_GF2X_ELEMENT] = {{0}};
   for(int i=0;i<N0;i++){
       gf2x_mod_densify_CT(HTr[i],HtrPosOnes[i],V);
   }
#endif

#if (defined CONSTANT_TIME)
   for(int iteration = 0;iteration < ITERATIONS_MAX; iteration++){
#else
   int syndrome_wt=1;
   for(int iteration = 0; (iteration < ITERATIONS_MAX) && (syndrome_wt !=0); iteration++){
#endif

#if !(defined CONSTANT_TIME)
      /* computation of syndrome weight and threshold determination */
      syndrome_wt = population_count(privateSyndrome);
      int min_idx=0;
      int max_idx;
      max_idx = sizeof(synd_corrt_vec)/(2*sizeof(unsigned int)) - 1;
      int thresh_table_idx = (min_idx + max_idx)/2;
      while(min_idx< max_idx) {
         if (synd_corrt_vec[thresh_table_idx][0] <= syndrome_wt) {
            min_idx = thresh_table_idx +1;
         } else {
            max_idx = thresh_table_idx -1;
         }
         thresh_table_idx = (min_idx +max_idx)/2;
      }
      int corrt_syndrome_based=synd_corrt_vec[thresh_table_idx][1];
      bs_operand_t sliced_threshold;
      uint16_t cut_neg_threshold = (uint16_t) (-corrt_syndrome_based);
      sliced_threshold = slice_constant(cut_neg_threshold);
#else
      /* Fixed threshold per iteration */
      bs_operand_t sliced_threshold;
      uint16_t cut_neg_threshold = (uint16_t) (-thresholds[iteration]);
      sliced_threshold = slice_constant(cut_neg_threshold);
#endif
     /* preload UPCs to -threshold */
      for (int i=0;i < N0*NUM_SLICES_GF2X_ELEMENT; i++){
         bs_unsatParityChecks[i]=sliced_threshold;
      }

      for (int i = 0; i < N0; i++) {
        lift_mul_dense_to_sparse_CT(bs_unsatParityChecks+(i*NUM_SLICES_GF2X_ELEMENT),
                                    privateSyndrome,
                                    HPosOnes[i],
                                    V);
      }

#if (defined HIGH_PERFORMANCE_X86_64)
      /* compute and deltaerr: note, each poly is embedded in a larger
       * variable as long as N0*NUM_SLICES_GF2X_ELEMENT, each poly
       * starts at i*NUM_SLICES_GF2X_ELEMENT */
      for(int i = N0*NUM_SLICES_GF2X_ELEMENT-1 ; i >=0 ; i--) {
          *(__m256i* )(deltaerr+i*4) = ~bs_unsatParityChecks[i].slice[BITSLICED_OPERAND_WIDTH-1];
      }
      for(int i = 0; i < N0; i++) {
         for(int j = 0; j < NUM_DIGITS_GF2X_ELEMENT; j++) {
            out[(i*NUM_DIGITS_GF2X_ELEMENT)+j] ^= deltaerr[(i*NUM_SLICES_GF2X_ELEMENT*4)+j];
         }
      }
#else
      /* compute and add deltaerr */
      for(int i = N0*NUM_SLICES_GF2X_ELEMENT-1 ; i >=0 ; i--) {
         deltaerr[i] = ~bs_unsatParityChecks[i].slice[BITSLICED_OPERAND_WIDTH-1];
         out[i] ^= deltaerr[i];
      }
#endif

      /* Clean deltaerr now or syndrome contribution will include slack bits */
      for(int i = 0; i < N0; i++){
#if (defined HIGH_PERFORMANCE_X86_64)
          deltaerr[i*NUM_SLICES_GF2X_ELEMENT*4] &= SLACK_CLEAR_MASK;
#else
          deltaerr[i*NUM_DIGITS_GF2X_ELEMENT] &= SLACK_CLEAR_MASK;
#endif
      }
      /*clean slack of output error*/
      for(int i = 0; i < N0; i++){
          out[i*NUM_DIGITS_GF2X_ELEMENT] &= SLACK_CLEAR_MASK;
      }

      /* add deltaerr contribution to syndrome */
#if (defined DENSE_H)
      DIGIT deltasynd_dense[NUM_DIGITS_GF2X_ELEMENT];
      for(int i = 0; i < N0; i++){
#if (defined HIGH_PERFORMANCE_X86_64)
            gf2x_mod_mul(deltasynd_dense,
                         HTr[i],
                         deltaerr+i*(NUM_SLICES_GF2X_ELEMENT*4));
#else
            gf2x_mod_mul(deltasynd_dense,
                         HTr[i],
                         deltaerr+i*NUM_DIGITS_GF2X_ELEMENT);
#endif
            gf2x_mod_add(privateSyndrome,
                         deltasynd_dense,
                         privateSyndrome);
      }
#else
        for(int i = 0; i < N0; i++){
            for(int HtrOneIdx = 0; HtrOneIdx < V; HtrOneIdx++){
#if (defined HIGH_PERFORMANCE_X86_64)
            gf2x_mod_fmac(privateSyndrome,
                                HtrPosOnes[i][HtrOneIdx],
                                deltaerr+i*(NUM_SLICES_GF2X_ELEMENT*4));
#else
            gf2x_mod_fmac(privateSyndrome,
                                HtrPosOnes[i][HtrOneIdx],
                                deltaerr+i*NUM_DIGITS_GF2X_ELEMENT);
#endif
            }
        }
        privateSyndrome[0] &= SLACK_CLEAR_MASK;
#endif

   }
   int check= 0;
   while (check < NUM_DIGITS_GF2X_ELEMENT && privateSyndrome[check++] == 0);
   return (check == NUM_DIGITS_GF2X_ELEMENT);
}
