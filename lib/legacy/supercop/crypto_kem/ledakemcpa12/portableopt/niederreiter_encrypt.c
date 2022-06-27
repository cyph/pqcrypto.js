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

#include "niederreiter_encrypt.h"
#include "qc_ldpc_parameters.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include "rng.h"
#include "sha3.h"
#include <string.h> // memset(...)

static
void encrypt_niederreiter(DIGIT syndrome[],                // 1  polynomial
                          const publicKeyNiederreiter_t *const pk,
                          const POSITION_T errorPos[NUM_ERRORS_T], // positions of asserted bits
                          const DIGIT err[])
{
   int i;
   DIGIT saux[NUM_DIGITS_GF2X_ELEMENT];
   unsigned int filled;
   memset(syndrome, 0x00, NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   POSITION_T blkErrorPos[NUM_ERRORS_T];
   for (i = 0; i < N0-1; i++) {
       filled=0;
       for (int j = 0 ; j < NUM_ERRORS_T; j ++){
           if(errorPos[j] / P == i){
               blkErrorPos[filled] =  errorPos[j] % P;
               filled++;
           }
       }
       gf2x_mod_mul_dense_to_sparse(saux,
                                   pk->Mtr+i*NUM_DIGITS_GF2X_ELEMENT,
                                   blkErrorPos,
                                   filled);
       gf2x_mod_add(syndrome, syndrome, saux);
    }   // end for
    gf2x_mod_add(syndrome, syndrome, err+(N0-1)*NUM_DIGITS_GF2X_ELEMENT);
} // end encrypt_niederreiter

/*----------------------------------------------------------------------------*/

void encrypt_niederreiter_indcpa(unsigned char * const ct, /* ciphertext - output    */
                                 unsigned char * const tag,/* tag for message confirmation - output */
                                 unsigned char * const ss, /* shared secret - output */
                                 const publicKeyNiederreiter_t *const pk) {

   unsigned char err_vect_seed[TRNG_BYTE_LENGTH];
   randombytes(err_vect_seed, TRNG_BYTE_LENGTH);

   xof_shake_t err_vect_seed_expander;
   shake_seedexpander_init(&err_vect_seed_expander,
                           err_vect_seed);

   POSITION_T errorPos[NUM_ERRORS_T];
   rand_error_pos_shake(errorPos,
                        &err_vect_seed_expander);

   unsigned char error_vector[1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B];
   // domain separation error_vector[0] = 0x00; needed
   // to use the HASH_FUNCTION as a KDF
   // error_vector[0] = 0x01; will be needed to use the HASH_FUNCTION
   // as a proper cryptographic compressor
   error_vector[0] = 0x00;
   expand_error( ( (DIGIT *)(&error_vector[1]) ), errorPos);

   HASH_FUNCTION((unsigned char *) error_vector,        // input
                 (1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B), // input Length
                 ss);

   encrypt_niederreiter((DIGIT *) ct,
                        pk,
                        errorPos,
                        ((const DIGIT * const) (&error_vector[1])));
   unsigned char tagMask[HASH_BYTE_LENGTH];
   error_vector[0] = 0x01;
   HASH_FUNCTION((unsigned char *) error_vector,        // input
                 (1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B), // input Length
                 tagMask);

   for (int i = 0; i < TRNG_BYTE_LENGTH; ++i) {
      tag[i] = err_vect_seed[i] ^ tagMask[i];
   }

} // end encrypt_niederreiter_indcpa

/*----------------------------------------------------------------------------*/

