/**
 *
 * <niederreiter_decrypt.c>
 *
 * @version 2.0 (March 2019)
 *
 * Reference ISO-C11 Implementation of the LEDAcrypt KEM cipher using GCC built-ins.
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

#include "niederreiter_decrypt.h"

#include "qc_ldpc_parameters.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include "H_Q_matrices_generation.h"

#include "bf_decoding.h"
#include "dfr_test.h"
#include <string.h>

extern int thresholds[2];
/*----------------------------------------------------------------------------*/

int decrypt_niederreiter(DIGIT err[],            // N0 circ poly
                         const privateKeyNiederreiter_t *const sk,
                         const DIGIT syndrome[]  // 1 circ poly
                        )
{
   AES_XOF_struct niederreiter_decrypt_expander;
   seedexpander_from_trng(&niederreiter_decrypt_expander,
                          sk->prng_seed);
   /* rebuild secret key values */
   POSITION_T HPosOnes[N0][DV];
   POSITION_T QPosOnes[N0][M];
   int rejections =  sk->rejections;
   thresholds[1] = sk->secondIterThreshold;
   POSITION_T LPosOnes[N0][DV*M];
   do {
     generateHPosOnes(HPosOnes, &niederreiter_decrypt_expander);
     generateQPosOnes(QPosOnes, &niederreiter_decrypt_expander);
   
     for (int i = 0; i < N0; i++) {
        for (int j = 0; j< DV*M; j++) {
           LPosOnes[i][j]=INVALID_POS_VALUE;
        }
     }

     POSITION_T auxPosOnes[DV*M];
     unsigned char processedQOnes[N0] = {0};
     for (int colQ = 0; colQ < N0; colQ++) {
        for (int i = 0; i < N0; i++) {
           gf2x_mod_mul_sparse(DV*M, auxPosOnes,
                               DV, HPosOnes[i],
                               qBlockWeights[i][colQ], QPosOnes[i]+processedQOnes[i]);
           gf2x_mod_add_sparse(DV*M, LPosOnes[colQ],
                               DV*M, LPosOnes[colQ],
                               DV*M, auxPosOnes);
           processedQOnes[i] += qBlockWeights[i][colQ];
        }
     }
     rejections--;
   } while(rejections>=0);

   POSITION_T HtrPosOnes[N0][DV];
   POSITION_T QtrPosOnes[N0][M];
   transposeHPosOnes(HtrPosOnes, HPosOnes);
   transposeQPosOnes(QtrPosOnes, QPosOnes);

   POSITION_T auxSparse[DV*M];
   POSITION_T Ln0trSparse[DV*M];
   for(int i = 0; i< DV*M; i++) {
      Ln0trSparse[i] = INVALID_POS_VALUE;
      auxSparse[i]= INVALID_POS_VALUE;
   }

   for (int i = 0; i < N0; i++) {
      gf2x_mod_mul_sparse(DV*M, auxSparse,
                          DV,   HPosOnes[i],
                          qBlockWeights[i][N0-1], &QPosOnes[i][ M-qBlockWeights[i][N0-1] ]
                         );
      gf2x_mod_add_sparse(DV*M, Ln0trSparse,
                          DV*M, Ln0trSparse,
                          DV*M, auxSparse
                         );
   } // end for i
   gf2x_transpose_in_place_sparse(DV*M, Ln0trSparse);

   DIGIT privateSyndrome[NUM_DIGITS_GF2X_ELEMENT];
   gf2x_mod_mul_dense_to_sparse(privateSyndrome,
                                syndrome,
                                Ln0trSparse,
                                DV*M);


   
   int decryptOk = 0;
   DIGIT err_computed[N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B] = {0};
   decryptOk = bf_decoding(err_computed, 
                           (const POSITION_T (*)[DV]) HtrPosOnes,
                           (const POSITION_T (*)[M])  QtrPosOnes, 
                           privateSyndrome
                          );

   int err_weight = 0;
   for (int i =0 ;i < N0; i++){
       err_weight += population_count(err_computed+(NUM_DIGITS_GF2X_ELEMENT*i));
   }
   decryptOk = decryptOk && (err_weight == NUM_ERRORS_T);
 
   /* prepare mockup error vector in case a decoding failure occurs */  
   DIGIT err_mockup[N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B];
   memcpy(err_mockup, syndrome, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   memcpy(err_mockup+NUM_DIGITS_GF2X_ELEMENT, sk->decryption_failure_secret, TRNG_BYTE_LENGTH);
   memset( ((unsigned char*) err_mockup)+(NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B)+TRNG_BYTE_LENGTH, 
           0x00, 
           (N0-1)*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B-TRNG_BYTE_LENGTH
         ); 

   if (!decryptOk){ 
       memcpy(err, err_mockup, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   } else {
       memcpy(err, err_computed, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   }
   
   return decryptOk;
} // end decrypt_niederreiter

/*----------------------------------------------------------------------------*/


