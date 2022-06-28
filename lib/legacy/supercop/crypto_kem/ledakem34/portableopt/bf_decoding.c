/**
 *
 * <bf_decoding.c>
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
#include "bf_decoding.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include <string.h>
#include <assert.h>

#define ROTBYTE(a)   ( (a << 8) | (a >> (DIGIT_SIZE_b - 8)) )
#define ROTUPC(a)   ( (a >> 8) | (a << (DIGIT_SIZE_b - 8)) )
#define ROUND_UP(amount, round_amt) ( ((amount+round_amt-1)/round_amt)*round_amt )

#ifdef HIGH_PERFORMANCE_X86_64
int bf_decoding(DIGIT out[], // N0 polynomials
                const POSITION_T HtrPosOnes[N0][DV],
                const POSITION_T QtrPosOnes[N0][M],
                DIGIT privateSyndrome[]  //  1 polynomial
               )
{
#if P < 64
#error The circulant block size should exceed 64
#endif

   /* The unsatisfied parity checks are kept as N0 vectors , each one having
    * a length rounded up to the closest multiple of the coalescing factor 
    * DIGIT_SIZE_B, plus one entire digit, and with the last digit padded with zeroes.
    * This strategy allows to replicate an entire digit at the end of the computed UPCs 
    * effectively removing altogether the (key-dependent) checks which would happen 
    * in the correlation computation process */
#define VECTYPE __m256i
#define VECTYPE_SIZE_b 256
#define VECTYPE_SIZE_B (256/8)
#define BIT_SIZE_UPC 8

#define SIZE_OF_UPC_VECTORIZED_READ 32

   uint8_t unsatParityChecks[N0][ROUND_UP(P,SIZE_OF_UPC_VECTORIZED_READ)+256] = {{0}};
   POSITION_T currQBitPos[M];
   /* syndrome is endowed with cyclic padding in the leading word to avoid 
    * boundary checks. In particular as many bits as the vectorized bit load
    * performed from the syndrome are replicated. The replicated portion
    * is in the leading words of the syndrome */
#define DIGIT_SIZE_OF_SYNDROME_VECTORIZED_READ (VECTYPE_SIZE_b/DIGIT_SIZE_b)
   DIGIT currSyndrome[NUM_DIGITS_GF2X_ELEMENT+DIGIT_SIZE_OF_SYNDROME_VECTORIZED_READ];
   int check;
   int imax = ITERATIONS_MAX;
   unsigned int synd_corrt_vec[][2]= {SYNDROME_TRESH_LOOKUP_TABLE};

   do {
      gf2x_copy(currSyndrome+DIGIT_SIZE_OF_SYNDROME_VECTORIZED_READ, privateSyndrome);

/*position of the first set bit in the word, counting 63 ... 0*/
#if (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT == (DIGIT_SIZE_b-1))
    for (int i =0 ;i < DIGIT_SIZE_OF_SYNDROME_VECTORIZED_READ ; i++){
       currSyndrome[i] = currSyndrome[NUM_DIGITS_GF2X_ELEMENT+i];
    }
#else
    for (int i =0 ;i < DIGIT_SIZE_OF_SYNDROME_VECTORIZED_READ ; i++){
        currSyndrome[1+i] |= ( currSyndrome[NUM_DIGITS_GF2X_ELEMENT+i] << 
                            (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT+1) );
        currSyndrome[0+i] = currSyndrome[NUM_DIGITS_GF2X_ELEMENT+i] >> ( DIGIT_SIZE_b- (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT+1) );
    }
#endif


#define VEC_COMB_MASK _mm256_set_epi64x (0x0101010101010101ULL,0x0101010101010101ULL,0x0101010101010101ULL,0x0101010101010101ULL)

    VECTYPE upcMat[BIT_SIZE_UPC];
    VECTYPE packedSynBits;
    for (int i = 0; i < N0; i++) {
       /* the following loop is vectorized. VECTYPE_SIZE_b bits from the syndrome
        * are read at once */
       for (int valueIdx = 0; valueIdx < P; valueIdx = valueIdx + DIGIT_SIZE_b) { 
          memset(upcMat, 0 , VECTYPE_SIZE_B*(BIT_SIZE_UPC));
          /* this fetches DIGIT_SIZE_b bits from each Htrpos, packed, and adds them to the 64 upc counters in upcmat */
          for(int HtrOneIdx = 0; HtrOneIdx < DV; HtrOneIdx++) {
                POSITION_T basePos = (HtrPosOnes[i][HtrOneIdx]+valueIdx);
                /* constant time computation of mod P*/
                SIGNED_POSITION_T mask = ((((SIGNED_POSITION_T) basePos) - P) >> 31);
                basePos = (basePos & mask) | ((basePos-P) & ~mask);
                /* lsb here is the one in base pos, others are subseq*/
                packedSynBits = gf2x_get_DIGIT_SIZE_coeff_vector_boundless(currSyndrome,basePos); 
                for(int upcMatRow = 0; upcMatRow < BIT_SIZE_UPC; upcMatRow++){
                  upcMat[upcMatRow] = _mm256_add_epi8 (upcMat[upcMatRow],
                                                       _mm256_and_si256 (VEC_COMB_MASK, 
                                                                         packedSynBits));
                  packedSynBits = _mm256_srli_epi16 (packedSynBits,1);
                }
          }/* end of for computing 256 upcs*/

         /* commit computed UPCs in the upc vector, in the proper order. 
          * upcMat essentially needs transposition and linearization by row,
          * starting from the last row */

#define imm8(n0, n1, n2, n3) (n3 + (n2<<2) + (n1<<4) + (n0<<6))
#define SHUFMASK ( _mm256_set_epi8(15,11,7,3,14,10,6,2,13,9,5,1,12,8,4,0,15,11,7,3,14,10,6,2,13,9,5,1,12,8,4,0))
     __m256i x[8];

    x[0] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[0],(__m256) upcMat[1],imm8(1,0,1,0));
    x[1] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[2],(__m256) upcMat[3],imm8(1,0,1,0));
    x[2] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[4],(__m256) upcMat[5],imm8(1,0,1,0));
    x[3] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[6],(__m256) upcMat[7],imm8(1,0,1,0));
    x[4] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[0],(__m256) upcMat[1],imm8(3,2,3,2));
    x[5] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[2],(__m256) upcMat[3],imm8(3,2,3,2));
    x[6] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[4],(__m256) upcMat[5],imm8(3,2,3,2));
    x[7] = (__m256i) _mm256_shuffle_ps( (__m256) upcMat[6],(__m256) upcMat[7],imm8(3,2,3,2));

    upcMat[0] = x[0];
    upcMat[1] = x[1];
    upcMat[2] = x[2];
    upcMat[3] = x[3];
    upcMat[4] = x[4];
    upcMat[5] = x[5];
    upcMat[6] = x[6];
    upcMat[7] = x[7];

    x[0] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[0],(__m256) upcMat[1],imm8(2,0,2,0));
    x[1] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[2],(__m256) upcMat[3],imm8(2,0,2,0));
    x[2] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[0],(__m256) upcMat[1],imm8(3,1,3,1));
    x[3] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[2],(__m256) upcMat[3],imm8(3,1,3,1));
    x[4] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[4],(__m256) upcMat[5],imm8(2,0,2,0));
    x[5] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[6],(__m256) upcMat[7],imm8(2,0,2,0));
    x[6] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[4],(__m256) upcMat[5],imm8(3,1,3,1));
    x[7] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[6],(__m256) upcMat[7],imm8(3,1,3,1));

    upcMat[0] = x[0];
    upcMat[1] = x[1];
    upcMat[2] = x[2];
    upcMat[3] = x[3];
    upcMat[4] = x[4];
    upcMat[5] = x[5];
    upcMat[6] = x[6];
    upcMat[7] = x[7];

    upcMat[0] = _mm256_shuffle_epi8 (upcMat[0],SHUFMASK);
    upcMat[1] = _mm256_shuffle_epi8 (upcMat[1],SHUFMASK);
    upcMat[2] = _mm256_shuffle_epi8 (upcMat[2],SHUFMASK);
    upcMat[3] = _mm256_shuffle_epi8 (upcMat[3],SHUFMASK);
    upcMat[4] = _mm256_shuffle_epi8 (upcMat[4],SHUFMASK);
    upcMat[5] = _mm256_shuffle_epi8 (upcMat[5],SHUFMASK);
    upcMat[6] = _mm256_shuffle_epi8 (upcMat[6],SHUFMASK);
    upcMat[7] = _mm256_shuffle_epi8 (upcMat[7],SHUFMASK);

    x[0] = _mm256_unpacklo_epi64 (upcMat[0], upcMat[1]);
    x[1] = _mm256_unpackhi_epi64 (upcMat[0], upcMat[1]);
    x[2] = _mm256_unpacklo_epi64 (upcMat[2], upcMat[3]);
    x[3] = _mm256_unpackhi_epi64 (upcMat[2], upcMat[3]);
    x[4] = _mm256_unpacklo_epi64 (upcMat[4], upcMat[5]);
    x[5] = _mm256_unpackhi_epi64 (upcMat[4], upcMat[5]);
    x[6] = _mm256_unpacklo_epi64 (upcMat[6], upcMat[7]);
    x[7] = _mm256_unpackhi_epi64 (upcMat[6], upcMat[7]);

    upcMat[0] = x[0];
    upcMat[1] = x[1];
    upcMat[2] = x[2];
    upcMat[3] = x[3];
    upcMat[4] = x[4];
    upcMat[5] = x[5];
    upcMat[6] = x[6];
    upcMat[7] = x[7];

    upcMat[0] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[0],(__m256) upcMat[0],imm8(3,1,2,0));
    upcMat[1] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[1],(__m256) upcMat[1],imm8(3,1,2,0));
    upcMat[2] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[2],(__m256) upcMat[2],imm8(3,1,2,0));
    upcMat[3] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[3],(__m256) upcMat[3],imm8(3,1,2,0));
    upcMat[4] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[4],(__m256) upcMat[4],imm8(3,1,2,0));
    upcMat[5] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[5],(__m256) upcMat[5],imm8(3,1,2,0));
    upcMat[6] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[6],(__m256) upcMat[6],imm8(3,1,2,0));
    upcMat[7] = (__m256i) _mm256_shuffle_ps((__m256) upcMat[7],(__m256) upcMat[7],imm8(3,1,2,0));

    x[0] = _mm256_permute2f128_si256(upcMat[0],upcMat[0],1);
    x[1] = _mm256_permute2f128_si256(upcMat[1],upcMat[1],1);
    x[2] = _mm256_permute2f128_si256(upcMat[2],upcMat[2],1);
    x[3] = _mm256_permute2f128_si256(upcMat[3],upcMat[3],1);
    x[4] = _mm256_permute2f128_si256(upcMat[4],upcMat[4],1);
    x[5] = _mm256_permute2f128_si256(upcMat[5],upcMat[5],1);
    x[6] = _mm256_permute2f128_si256(upcMat[6],upcMat[6],1);
    x[7] = _mm256_permute2f128_si256(upcMat[7],upcMat[7],1);

    VECTYPE* vp;

    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*0]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (upcMat[0],x[1], 0xF0));
    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*1]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (upcMat[2],x[3], 0xF0));
    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*2]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (upcMat[4],x[5], 0xF0));
    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*3]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (upcMat[6],x[7], 0xF0));
    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*4]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (x[0],upcMat[1], 0xF0));
    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*5]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (x[2],upcMat[3], 0xF0));
    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*6]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (x[4],upcMat[5], 0xF0));
    vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+32*7]);
    _mm256_storeu_si256(vp, _mm256_blend_epi32 (x[6],upcMat[7], 0xF0));
         }
    }

      /* circular padding of unsatisfiedParityChecks so that the vector 
       * correlation computation does not need to wraparound loads*/
      /* no need to fill the pad, vectorized upc computation fills it up 
       * widely */

      // computation of syndrome weight and threshold determination
#if (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT == (DIGIT_SIZE_b-1))
/* The digits of the syndrome are all full, no slack to be cleared before
 * popcounting */
#else
 currSyndrome[4] = currSyndrome[4] & 
                   ( ( ((DIGIT) 1) << (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT+1) ) -  1);
#endif
      int syndrome_wt = population_count(currSyndrome+4);

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

      //Computation of correlation  with a full Q matrix

      for (int i = 0; i < N0; i++) {
         uint8_t  qBlockIdxes[M];
         uint16_t qBlockOffsets[M];
         uint8_t  qBlockIdxesCursor = 0, linearized_cursor = 0;
         int endQblockIdx = 0;
         for (int blockIdx = 0; blockIdx < N0; blockIdx++) {
           endQblockIdx += qBlockWeights[blockIdx][i];
           for (; linearized_cursor < endQblockIdx; linearized_cursor++) {
             qBlockIdxes[linearized_cursor] = qBlockIdxesCursor;
             qBlockOffsets[linearized_cursor] =  (uint16_t) qBlockIdxesCursor;
           }
           qBlockIdxesCursor++;
         }
         /* the following loop is vectorized and computes a vector of 
          * correlations as wide as the number of bytes which fit in a vector 
          * register */
         for (int j = 0; j <= P; j+=SIZE_OF_UPC_VECTORIZED_READ) {
            int currQoneIdx; // position in the column of QtrPosOnes[][...]
            /* vector of correlations computed in a single sweep of AVX2 vectorized
             * computation.  Stored with type punning as a 4 item uint64_t
             * vectorized corrs. */
            uint64_t correlation = 0; 
            uint64_t correlation_avx[4] = {0}; 
            __m256i vecCorr = _mm256_setzero_si256();
            __m256i vecUpcToAdd;

            /* a single correlation value is the sum of m "block circulant 
             * positioned" values of UPC. For each position, load 
             * SIZE_OF_UPC_VECTORIZED_READ UPCs*/

            for (currQoneIdx = 0; currQoneIdx < M; currQoneIdx++) {

                int tmp = QtrPosOnes[i][currQoneIdx]+j;
                SIGNED_POSITION_T mask = ((((SIGNED_POSITION_T) tmp) - P) >> 31);
                tmp = (tmp & mask) | ((tmp-P) & ~mask);
                currQBitPos[currQoneIdx] = tmp;  
                /* only a single base index must be kept per vector load*/
                
                vecUpcToAdd = _mm256_lddqu_si256((__m256i *) (&unsatParityChecks[qBlockOffsets[currQoneIdx]][tmp]));
                vecCorr = _mm256_add_epi8(vecUpcToAdd,vecCorr);
            }
            _mm256_storeu_si256( (__m256i *) correlation_avx,vecCorr);

            
            for(int vecCorrDeltaJ = 0 ; vecCorrDeltaJ < SIZE_OF_UPC_VECTORIZED_READ/DIGIT_SIZE_B; vecCorrDeltaJ++) { 
            /* Correlation based flipping */
            correlation = correlation_avx[vecCorrDeltaJ];
               for (int delta_j = 0 ; 
                    delta_j < DIGIT_SIZE_B && (j+vecCorrDeltaJ*DIGIT_SIZE_B+delta_j < P) ; 
                    delta_j++){
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                  uint64_t single_correl = ( correlation >> (8*delta_j) ) & 0xFF; 
#else          
                  uint64_t single_correl = ( correlation >> (8*(DIGIT_SIZE_B-1-delta_j)) ) & 0xFF; 
#endif         
                  if ((single_correl >= corrt_syndrome_based)) {
                      /* correlation will always be null for garbage bits */
                     gf2x_toggle_coeff(out+NUM_DIGITS_GF2X_ELEMENT*i, (j+vecCorrDeltaJ*DIGIT_SIZE_B+delta_j)%P); /*this can be vectorized if a vector of correlations are available*/
                     for (int v = 0; v < M; v++) { /* the same goes here */
                        unsigned syndromePosToFlip;
                        for (int HtrOneIdx = 0; HtrOneIdx < DV; HtrOneIdx++) {
                           syndromePosToFlip = (HtrPosOnes[qBlockIdxes[v]][HtrOneIdx] + 
                                                (currQBitPos[v]+vecCorrDeltaJ*DIGIT_SIZE_B+delta_j)%P );
                           syndromePosToFlip = syndromePosToFlip >= P ? syndromePosToFlip - P : syndromePosToFlip;
                           gf2x_toggle_coeff(privateSyndrome, syndromePosToFlip);
                        }
                     } // end for v
                  } // end if
               } // end for flipping correlations exceeding threshold
            } // end of for selecting part of correlation vector to analyze
         } // end for j
      } // end for i

      imax = imax - 1;
      check = 0;
      while (check < NUM_DIGITS_GF2X_ELEMENT && privateSyndrome[check] == 0) {
          check++;
      }

   } while (imax != 0 && check < NUM_DIGITS_GF2X_ELEMENT);

   return (check == NUM_DIGITS_GF2X_ELEMENT);
}  // end QdecodeSyndromeThresh_bitFlip_sparse

/*C fallback if there are no AVX2*/
#else

int bf_decoding(DIGIT out[], // N0 polynomials
                const POSITION_T HtrPosOnes[N0][DV],
                const POSITION_T QtrPosOnes[N0][M],
                DIGIT privateSyndrome[]  //  1 polynomial
               )
{
#if P < 64
#error The circulant block size should exceed 64
#endif

   /* The unsatisfied parity checks are kept as N0 vectors , each one having
    * a length rounded up to the closest multiple of the coalescing factor 
    * DIGIT_SIZE_B, plus one entire digit, and with the last digit padded with zeroes.
    * This strategy allows to replicate an entire digit at the end of the computed UPCs 
    * effectively removing altogether the (key-dependent) checks which would happen 
    * in the correlation computation process */
#define SIZE_OF_UPC_VECTORIZED_READ 32
   uint8_t unsatParityChecks[N0][ ROUND_UP(P,SIZE_OF_UPC_VECTORIZED_READ)+SIZE_OF_UPC_VECTORIZED_READ] = {{0}};
   POSITION_T currQBitPos[M];
   /* syndrome is endowed with cyclic padding in the leading word to avoid 
    * boundary checks. The pad should be at least as long as the bit-len of one 
    * vector of bits which is read during UPC computation */
   DIGIT currSyndrome[NUM_DIGITS_GF2X_ELEMENT+1];
   int check;
   int imax = ITERATIONS_MAX;
   unsigned int synd_corrt_vec[][2]= {SYNDROME_TRESH_LOOKUP_TABLE};

   do {
      gf2x_copy(currSyndrome+1, privateSyndrome);

/*position of the first set bit in the word, counting 63 ... 0*/
#if (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT == (DIGIT_SIZE_b-1))
      currSyndrome[0] = currSyndrome[NUM_DIGITS_GF2X_ELEMENT];
#else
       currSyndrome[1] |= (currSyndrome[NUM_DIGITS_GF2X_ELEMENT] << (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT+1) );
       currSyndrome[0] = currSyndrome[NUM_DIGITS_GF2X_ELEMENT] >> (DIGIT_SIZE_b- (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT+1));
#endif

#define VECTYPE DIGIT
#define VECTYPE_SIZE_b DIGIT_SIZE_b
#define VECTYPE_SIZE_B DIGIT_SIZE_B
#define BIT_SIZE_UPC 8
#define VEC_COMB_MASK 0x0101010101010101ULL

    VECTYPE upcMat[VECTYPE_SIZE_b/BIT_SIZE_UPC];
    VECTYPE packedSynBits;
    for (int i = 0; i < N0; i++) {
       for (int valueIdx = 0; valueIdx < P; valueIdx = valueIdx + DIGIT_SIZE_b) { /* this index will be vectorized*/
          memset(upcMat, 0 , VECTYPE_SIZE_B*(VECTYPE_SIZE_b/BIT_SIZE_UPC));
          /* this fetches DIGIT_SIZE_b bits from each Htrpos, packed, and adds them to the 64 upc counters in upcmat */
          for(int HtrOneIdx = 0; HtrOneIdx < DV; HtrOneIdx++) {
                POSITION_T basePos = (HtrPosOnes[i][HtrOneIdx]+valueIdx);
                basePos = basePos %P ;
                /* lsb here is the one in base pos, others are subseq*/
                packedSynBits = gf2x_get_DIGIT_SIZE_coeff_vector_boundless(currSyndrome,basePos); 
                for(int upcMatRow = 0; upcMatRow < VECTYPE_SIZE_b/BIT_SIZE_UPC; upcMatRow++){
                    upcMat[upcMatRow] += packedSynBits & VEC_COMB_MASK;
                    packedSynBits = packedSynBits >> 1;
                }

              }/* end of for computing 64 upcs*/
         /* commit computed UPCs in the upc vector, in the proper order. 
          * UPCmat essentially needs transposition and linearization by row,
          * starting from the last row */
          for(int upcMatCol = 0; upcMatCol < VECTYPE_SIZE_b/BIT_SIZE_UPC; upcMatCol++){
              VECTYPE upcBuf = 0;
              for(int upcMatRow = 0; upcMatRow < VECTYPE_SIZE_b/BIT_SIZE_UPC; upcMatRow++){
               uint8_t matByte = upcMat[upcMatRow];
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
               upcBuf |=  ((VECTYPE)(matByte)) << (8*upcMatRow);
#else
               upcBuf |=  (upcBuf << 8) + ((VECTYPE)(matByte));
#endif
                  upcMat[upcMatRow] = upcMat[upcMatRow] >> 8;
              }
              VECTYPE* vp = (VECTYPE *)(&unsatParityChecks[i][valueIdx+8*upcMatCol]);
              *(vp) = upcBuf;
          } 
         }
     }

      /* padding unsatisfiedParityChecks */
      for (int i = 0; i < N0; i++) {
          for(int j = 0; j < DIGIT_SIZE_B; j++)
           unsatParityChecks[i][P+j] = unsatParityChecks[i][j];
      }
#if (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT == (DIGIT_SIZE_b-1))
/* The digits of the syndrome are all full, no slack to be cleared before
 * popcounting */
#else
 currSyndrome[1] = currSyndrome[1] & 
                   ( ( ((DIGIT) 1) << (MSb_POSITION_IN_MSB_DIGIT_OF_ELEMENT+1) ) -  1);
#endif
      // computation of syndrome weight and threshold determination
      int syndrome_wt = population_count(currSyndrome+1);
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

      //Computation of correlation  with a full Q matrix
      for (int i = 0; i < N0; i++) {
            uint8_t qBlockIdxes[M];
            uint16_t qBlockOffsets[M];
            uint8_t qBlockIdxesCursor = 0, linearized_cursor = 0;
            int endQblockIdx = 0;
            for (int blockIdx = 0; blockIdx < N0; blockIdx++) {
              endQblockIdx += qBlockWeights[blockIdx][i];
              for (; linearized_cursor < endQblockIdx; linearized_cursor++) {
                qBlockIdxes[linearized_cursor] = qBlockIdxesCursor;
                qBlockOffsets[linearized_cursor] =  (uint16_t) qBlockIdxesCursor;
              }
              qBlockIdxesCursor++;
            }
         for (int j = 0; j <= P; j+=DIGIT_SIZE_B) {
            int currQoneIdx; // position in the column of QtrPosOnes[][...]
            /* a vector of (single byte wide) correlation values is computed
             * at each iteration of the outer loop. 8x vectorization is possible
             * with uint64_t. The most significant byte in the uint64_t vector 
             * of uint8_t is the one matching the outer loop index position*/
            uint64_t correlation = 0; 

            for (currQoneIdx = 0; currQoneIdx < M; currQoneIdx++) {
                int tmp = QtrPosOnes[i][currQoneIdx]+j; /* this must turn into a vector load of 8x UPCs with consecutive j indices */
                tmp = tmp % P ;  /* this will need to be checked before and have a split vector load */
                currQBitPos[currQoneIdx] = tmp;  /* only a single base index must be kept per vector load*/
                uint64_t * vp = (uint64_t*)(&unsatParityChecks[qBlockOffsets[currQoneIdx]][tmp]);
                correlation += *vp;
            }

            /* Correlation based flipping */
            for (int delta_j = 0 ; delta_j < DIGIT_SIZE_B && (j+delta_j < P) ; delta_j++){
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
               uint64_t single_correl = ( correlation >> (8*delta_j) ) & 0xFF; 
#else
               uint64_t single_correl = ( correlation >> (8*(DIGIT_SIZE_B-1-delta_j)) ) & 0xFF; 
#endif
               if ((single_correl > corrt_syndrome_based)) { /* correlation will always be null for garbage bits */
                  gf2x_toggle_coeff(out+NUM_DIGITS_GF2X_ELEMENT*i, (j+delta_j)%P); /*this can be vectorized if a vector of correlations are available*/
                  for (int v = 0; v < M; v++) { /* the same goes here */
                     unsigned syndromePosToFlip;
                     for (int HtrOneIdx = 0; HtrOneIdx < DV; HtrOneIdx++) {
                        syndromePosToFlip = (HtrPosOnes[qBlockIdxes[v]][HtrOneIdx] + (currQBitPos[v]+delta_j)%P );
                        syndromePosToFlip = syndromePosToFlip >= P ? syndromePosToFlip - P : syndromePosToFlip;
                        gf2x_toggle_coeff(privateSyndrome, syndromePosToFlip);
                     }
                  } // end for v
               } // end if
            } // end for flipping correlations exceeding threshold
         } // end for j
      } // end for i

      imax = imax - 1;
      check = 0;
      while (check < NUM_DIGITS_GF2X_ELEMENT && privateSyndrome[check] == 0) {
          check++;
      }
   } while (imax != 0 && check < NUM_DIGITS_GF2X_ELEMENT);
   return (check == NUM_DIGITS_GF2X_ELEMENT);
}  // end QdecodeSyndromeThresh_bitFlip_sparse
#endif
