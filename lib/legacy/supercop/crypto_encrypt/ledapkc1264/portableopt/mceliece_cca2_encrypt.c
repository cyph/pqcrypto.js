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


#include "mceliece_cca2_encrypt.h"
#include "gf2x_limbs.h"
#include "rng.h"
#include "sha3.h"
#include "gf2x_arith_mod_xPplusOne.h"

#include "constant_weight_codec.h"

#include <randombytes.h>
#include <string.h>  // memset(...), memcpy(...)
#include <assert.h>


/*----------------------------------------------------------------------------*/

static
void encrypt_McEliece(DIGIT codeword[],           // N0   polynomials
                      const publicKeyMcEliece_t *const pk,
                      const DIGIT ptx[],          // N0-1 polynomials
                      const DIGIT err[])          // N0   polynomials
{
   memcpy(codeword, ptx, (N0-1)*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   memset(codeword+(N0-1)*NUM_DIGITS_GF2X_ELEMENT,
          0x00,
          NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

   DIGIT saux[NUM_DIGITS_GF2X_ELEMENT];

   for (int i = 0; i < N0-1; i++) {
      memset(saux,0x00,NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
      gf2x_mod_mul(saux,
                   pk->Mtr+i*NUM_DIGITS_GF2X_ELEMENT,
                   ptx+i*NUM_DIGITS_GF2X_ELEMENT);
      gf2x_mod_add(codeword+(N0-1)*NUM_DIGITS_GF2X_ELEMENT,
                   codeword+(N0-1)*NUM_DIGITS_GF2X_ELEMENT,
                   saux);
   }
   for (int i = 0; i < N0; i++) {
      gf2x_mod_add(codeword+i*NUM_DIGITS_GF2X_ELEMENT,
                   codeword+i*NUM_DIGITS_GF2X_ELEMENT,
                   err+i*NUM_DIGITS_GF2X_ELEMENT
                  );
   }
} // end encrypt_McEliece

/*----------------------------------------------------------------------------*/

void char_right_bit_shift_n(const int length, uint8_t in[], const int amount) {
   assert(amount < 8);
   if ( amount == 0 ) return;
   int j;
   uint8_t mask;
   mask = ((uint8_t)0x01 << amount) - 1;
   for (j = length-1; j > 0 ; j--) {
      in[j] >>= amount;
      in[j] |=  (in[j-1] & mask) << (8 - amount);
   }
   in[j] >>= amount;
}

/*----------------------------------------------------------------------------*/
/*  shifts the input stream so that the bytewise pad is on the left before
 * conversion */
static
int bytestream_into_poly_seq(DIGIT polySeq[], int numPoly,
                             unsigned char * S,
                             const unsigned long byteLenS)
{
   int padsize = (K%8) ? 8-(K%8) : 0;
   char_right_bit_shift_n(byteLenS, S, padsize);
   if ( numPoly <= 0 || byteLenS <= 0 || byteLenS <  ( (numPoly*P +7) /8) )
      return 0;

   unsigned int slack_bits = byteLenS*8 - numPoly*P;
   unsigned int bitCursor = slack_bits;
   uint64_t buffer=0;
   for (unsigned polyIdx = 0; polyIdx < numPoly; polyIdx++) {
      for (unsigned exponent = 0; exponent < P; exponent++) {
         buffer = bitstream_read(S, 1, &bitCursor);
         gf2x_set_coeff(&polySeq[NUM_DIGITS_GF2X_ELEMENT*polyIdx],
                        exponent,
                        buffer
                       );
      }
   }

   return 1;
} // end bytestream_into_poly_seq

/*----------------------------------------------------------------------------*/

int encrypt_Kobara_Imai(unsigned char *const output,
                        const publicKeyMcEliece_t *const pk,
                        const uint32_t bytePtxLen,
                        const unsigned char *const ptx
                       )
{
   /* Generate PRNG pad */
   unsigned char secretSeed[TRNG_BYTE_LENGTH];
   uint64_t paddedSequenceLen;
   int isPaddedSequenceOnlyKBits=0;
   if(bytePtxLen<=MAX_BYTES_IN_IWORD) {
     /*warning, in this case the padded sequence is exactly K bits*/
     paddedSequenceLen =  (K+7)/8;
     isPaddedSequenceOnlyKBits=1;
   } else {
     paddedSequenceLen = KOBARA_IMAI_CONSTANT_LENGTH_B+KI_LENGTH_FIELD_SIZE+bytePtxLen;
   }

   unsigned char prngSequence[paddedSequenceLen];

   randombytes(secretSeed, TRNG_BYTE_LENGTH);
   deterministic_random_byte_generator(prngSequence,sizeof(prngSequence),
                                       secretSeed,TRNG_BYTE_LENGTH);

   /*to avoid the use of additional memory, exploit the memory allocated for
    * the ciphertext to host the prng-padded ptx+const+len. */
   memset(output,0,N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);


   KI_LENGTH_FIELD_TYPE correctlySizedBytePtxLen = bytePtxLen;
   memcpy(output+KOBARA_IMAI_CONSTANT_LENGTH_B, &(correctlySizedBytePtxLen),
          KI_LENGTH_FIELD_SIZE);
   memcpy(output+KOBARA_IMAI_CONSTANT_LENGTH_B+KI_LENGTH_FIELD_SIZE,
          ptx,bytePtxLen);
    for (int i = 0;i < paddedSequenceLen;i++){
        output[i] ^= prngSequence[i];
    }
   if (isPaddedSequenceOnlyKBits==1){
       output[paddedSequenceLen-1] &= ~(((uint8_t)0xFF) >> (K%8));
   }

   /* prepare buffer which will be translated in the information word */
   assert( (K+7)/8 == KOBARA_IMAI_CONSTANT_LENGTH_B+KI_LENGTH_FIELD_SIZE+MAX_BYTES_IN_IWORD+1);
   unsigned char iwordBuffer[(K+7)/8];

   memcpy(iwordBuffer, output, sizeof(iwordBuffer));

   /* transform into an information word poly sequence */
   DIGIT informationWord[(N0-1)*NUM_DIGITS_GF2X_ELEMENT] ={0};
   bytestream_into_poly_seq(informationWord,N0-1,iwordBuffer,(K+7)/8);

   /* prepare hash of padded sequence, before leftover is moved to its final place */
   unsigned char hashDigest[HASH_BYTE_LENGTH]={0};
   HASH_FUNCTION(output,            // sequence to digest
                 paddedSequenceLen, // input Length
                 hashDigest);         // output

   /* move leftover padded string (if present) onto its final position*/
   if (bytePtxLen>MAX_BYTES_IN_IWORD){
   memmove(output+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B,
           output+sizeof(iwordBuffer)-1,
           bytePtxLen-MAX_BYTES_IN_IWORD);
#if ((K%8)!=0)
   /*clear partial leakage from leftover string, only happens if K%8 !=0 */
   uint8_t initialLeftoverMask = ((uint8_t)0xFF) >> (K%8);
   output[N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B] &= initialLeftoverMask ;
#endif
   }

   /*prepare CWEnc input as zero extended seed ^ hash of */
   unsigned char cwEncInputBuffer[HASH_BYTE_LENGTH+CWENC_EXTRA_RANDOMNESS_LEN];
   memcpy(cwEncInputBuffer,hashDigest,HASH_BYTE_LENGTH);
   for (unsigned i = 0; i < TRNG_BYTE_LENGTH; i++)
         cwEncInputBuffer[i] ^= secretSeed[i];

   DIGIT  cwEncodedError[N0*NUM_DIGITS_GF2X_ELEMENT];
   /* continue drawing fresh randomness in case the constant weight encoding
    * fails */
   int binaryToConstantWeightOk = 0;
   do {
      /* blank cwenc destination buffer */
      memset(cwEncodedError, 0x00, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
      /* draw filler randomness for cwenc input from an independent random*/
      randombytes(secretSeed, TRNG_BYTE_LENGTH);
      deterministic_random_byte_generator(cwEncInputBuffer+HASH_BYTE_LENGTH,
                                          CWENC_EXTRA_RANDOMNESS_LEN,
                                          secretSeed,TRNG_BYTE_LENGTH);

      binaryToConstantWeightOk = binary_to_constant_weight_approximate(cwEncodedError,
                                 cwEncInputBuffer,
                                 HASH_BYTE_LENGTH+CWENC_EXTRA_RANDOMNESS_LEN);
   } while (binaryToConstantWeightOk == 0);

   DIGIT codeword[N0*NUM_DIGITS_GF2X_ELEMENT] = {0};

   encrypt_McEliece(codeword, pk, informationWord, cwEncodedError);

   /* output composition looks like codeword || left bytepad leftover
    * and is thus long as ROUND_UP(leftover_bits,8)+
    * N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B */
   // the output byte stream is made of N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B bytes
   memcpy(output, codeword, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

   return 1;
} // end encrypt_Kobara_Imai
/*----------------------------------------------------------------------------*/
