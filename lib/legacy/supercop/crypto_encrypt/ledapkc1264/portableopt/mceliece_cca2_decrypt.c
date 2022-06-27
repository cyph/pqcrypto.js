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

#include "mceliece_cca2_decrypt.h"
#include "H_Q_matrices_generation.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include "bf_decoding.h"
#include "sha3.h"
#include "rng.h"
#include "constant_weight_codec.h"

#include <string.h> // memset(...), memcpy(....)
#include <stdio.h>

/*----------------------------------------------------------------------------*/

static
int decrypt_McEliece (DIGIT decoded_err[],
                      DIGIT correct_codeword[],
                      privateKeyMcEliece_t * sk,
                      const unsigned char *const ctx)
{
   AES_XOF_struct mceliece_decrypt_expander;
   seedexpander_from_trng(&mceliece_decrypt_expander,
                          sk->prng_seed);
   /* rebuild secret key values */
   POSITION_T HPosOnes[N0][V];

   int rejections = sk->rejections;

   do {
     generateHPosOnes(HPosOnes, &mceliece_decrypt_expander);
     rejections--;
   } while(rejections>=0);

   POSITION_T HtrPosOnes[N0][V];
   transposeHPosOnes(HtrPosOnes, (const POSITION_T (*)[V])HPosOnes);
   /* end rebuild secret key values */

   DIGIT privateSyndrome[NUM_DIGITS_GF2X_ELEMENT]; // privateSyndrome := yVar* Htr
   memset(privateSyndrome, 0x00, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

   DIGIT aux[NUM_DIGITS_GF2X_ELEMENT];
   for(int i = 0; i < N0; i++) {
#if (defined HIGH_PERFORMANCE_X86_64)
   DIGIT Htrblock[NUM_DIGITS_GF2X_ELEMENT] = {0};
   gf2x_mod_densify_CT(Htrblock,HtrPosOnes[i],V);
   gf2x_mod_mul(aux,((const DIGIT *)ctx)+i*NUM_DIGITS_GF2X_ELEMENT,Htrblock);
#else
       gf2x_mod_mul_dense_to_sparse(aux,
                                   ((const DIGIT *)ctx)+i*NUM_DIGITS_GF2X_ELEMENT,
                                   HtrPosOnes[i],
                                   V);
#endif
      gf2x_mod_add(privateSyndrome, privateSyndrome, aux);
   }

   memset(decoded_err,0x00, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   /*perform syndrome decoding to obtain error vector */
   int ok;
   ok = bf_decoding_CT(decoded_err,
                    (const POSITION_T (*)[V])HtrPosOnes,
                    (const POSITION_T (*)[V])HPosOnes,
                    privateSyndrome);
   if ( ok == 0 ) return 0;
   int err_weight = 0;
   for (int i =0 ;i < N0; i++){
       err_weight += population_count(decoded_err+(NUM_DIGITS_GF2X_ELEMENT*i));
   }
   if (err_weight != NUM_ERRORS_T) return 0;

   /* correct input codeword */
   for (unsigned i = 0; i < N0; i++) {
      gf2x_mod_add(correct_codeword+i*NUM_DIGITS_GF2X_ELEMENT,
                   (DIGIT *) ctx+i*NUM_DIGITS_GF2X_ELEMENT,
                   decoded_err+i*NUM_DIGITS_GF2X_ELEMENT);
   }
   return 1;
}
/*----------------------------------------------------------------------------*/
static
void char_left_bit_shift_n(const int length, uint8_t in[], const int amount)
{
   if ( amount == 0 ) return;
   int j;
   uint8_t mask;
   mask = ~(((uint8_t)0x01 << (8 - amount)) - 1);
   for (j = 0 ; j < length-1 ; j++) {
      in[j] <<= amount;
      in[j] |=  (in[j+1] & mask) >> (8 - amount);
   }
   in[j] <<= amount;
} // end right_bit_shift_n



/*----------------------------------------------------------------------------*/
static
int poly_seq_into_bytestream(unsigned char output[],
                             const unsigned int byteOutputLength,
                             DIGIT zPoly[],
                             const unsigned int numPoly)
{
   DIGIT bitValue;
   unsigned int output_bit_cursor =  byteOutputLength*8-numPoly*P;

   if (NUM_BITS_GF2X_ELEMENT*numPoly > 8*byteOutputLength) return 0;

   for (int i =  0; i <numPoly; i++) {
      for (unsigned exponent = 0; exponent < NUM_BITS_GF2X_ELEMENT; exponent++) {
         bitValue = gf2x_get_coeff(zPoly+i*NUM_DIGITS_GF2X_ELEMENT, exponent);
         bitstream_write_construction(output, 1, &output_bit_cursor, bitValue );
      } // end for exponent
   } // end for i
   int padsize = (K%8) ? 8-(K%8) : 0;
   char_left_bit_shift_n(byteOutputLength, output, padsize);
   return 1;
} // end poly_seq_into_bytestream

/*----------------------------------------------------------------------------*/

extern int thresholds[2];

int decrypt_Kobara_Imai(unsigned char *const output,  //
                        unsigned long long * byteOutputLength,
                        privateKeyMcEliece_t * sk,
                        const unsigned long long clen,
                        const unsigned char *const ctx // constituted by codeword || leftover
                       )
{
   DIGIT err[N0*NUM_DIGITS_GF2X_ELEMENT];
   DIGIT correctedCodeword[N0*NUM_DIGITS_GF2X_ELEMENT];

   /* first N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B bytes are the actual McE
    * ciphertext. Note: storage endiannes in BE hardware should flip bytes */
   memcpy(correctedCodeword,ctx,N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   thresholds[1] = sk->secondIterThreshold;
   if (decrypt_McEliece (err, correctedCodeword, sk, ctx) == 0 ) {
      fprintf(stderr,"Decode FAIL\n");
      return 0;
   }

   /* correctedCodeword now contains the correct codeword, iword is the first
    * portion, followed by syndrome turn back iword into a bytesequence */
   uint64_t paddedSequenceLen;
   if(clen <= N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B) {
     paddedSequenceLen =  (K+7)/8;
   } else {
     paddedSequenceLen = clen - N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B  -1 /*leftover dup!*/
                         + (K+7)/8;
   }
   unsigned char paddedOutput[paddedSequenceLen];
   memset(paddedOutput,0,paddedSequenceLen);
   poly_seq_into_bytestream(paddedOutput,(K+7)/8,
                            correctedCodeword,(N0-1));

   /* move back leftover padded string (if present) onto its position*/
   if (clen>N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B){
#if ((K%8)!=0)
   /* meld back byte split across iword and leftover. Recall that leftover is
    * built with leading zeroes, and output from iword has trailing zeroes
    * so no masking away is needed */
   paddedOutput[((K+7)/8)-1] |= ctx[N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B];
#endif

   int remainingToCopy= paddedSequenceLen - ((K+7)/8);
   memmove(paddedOutput+( (K+7)/8),
           ctx+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B+1/*first leftover already managed*/,
           remainingToCopy);
   }


   unsigned char outputHash[HASH_BYTE_LENGTH] = {0};
   HASH_FUNCTION(paddedOutput,// param.s: input, inputLen, output
                 paddedSequenceLen,
                 outputHash);

   /* rebuild message hash ^ seed from error vector */
   unsigned char cwEncOutputBuffer[HASH_BYTE_LENGTH+CWENC_EXTRA_RANDOMNESS_LEN] = {0};
   constant_weight_to_binary_approximate(cwEncOutputBuffer, err);

   /* obtain back the PRNG seed */
   unsigned char secretSeed[TRNG_BYTE_LENGTH];
   for (int i = 0; i < TRNG_BYTE_LENGTH; i++) {
      secretSeed[i] = cwEncOutputBuffer[i] ^ outputHash[i];
   }

   /* test that the padding bytes of the seed are actually zero */
   for (int i = TRNG_BYTE_LENGTH; i < HASH_BYTE_LENGTH; i++) {
      if((cwEncOutputBuffer[i] ^ outputHash[i]) != 0){
         fprintf(stderr,"Nonzero TRNG pad \n");
         return 0;
      }
   }

   unsigned char prngSequence[paddedSequenceLen];
   memset (prngSequence,0x00,paddedSequenceLen);
   deterministic_random_byte_generator(prngSequence,sizeof(prngSequence),
                                       secretSeed,TRNG_BYTE_LENGTH);
   /* remove PRNG Pad from entire message */
   for (int i = 0;i < paddedSequenceLen;i++){
       paddedOutput[i] ^= prngSequence[i];
   }

   /*test if Kobara Imai constant, default to zero, matches */
   for (int i = 0; i < KOBARA_IMAI_CONSTANT_LENGTH_B; i++) {
      if(paddedOutput[i] != 0){
          fprintf(stderr,"KI const mismatch \n");
         return 0;
      }
   }
   /* retrieve message len, and set it */
   KI_LENGTH_FIELD_TYPE correctlySizedBytePtxLen ;

   memcpy(&(correctlySizedBytePtxLen),
          paddedOutput+KOBARA_IMAI_CONSTANT_LENGTH_B,
          KI_LENGTH_FIELD_SIZE);

   *byteOutputLength=correctlySizedBytePtxLen;
   /* copy message in output buffer */
   memcpy(output,
          paddedOutput+KOBARA_IMAI_CONSTANT_LENGTH_B+KI_LENGTH_FIELD_SIZE,
          correctlySizedBytePtxLen);
   return 1;
} // end decrypt_Kobara_Imai
