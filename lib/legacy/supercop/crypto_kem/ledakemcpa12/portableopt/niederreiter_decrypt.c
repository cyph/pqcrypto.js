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

#include "niederreiter_decrypt.h"

#include "qc_ldpc_parameters.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include "H_Q_matrices_generation.h"

#include "bf_decoding.h"
#include "rng.h"
#include "sha3.h"
#include <string.h>


/*----------------------------------------------------------------------------*/

int decrypt_niederreiter_indcpa(unsigned char *const ss,                   /* output */
                                const unsigned char *const ct,             /* input */
                                const unsigned char *const tag,            /* input */
                                const privateKeyNiederreiter_t * const sk) /* input */
{   // return 1 in case of success, 0 otherwise

    unsigned char decoded_error_vector[1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B] = { 0x00 };
    DIGIT privateSyndrome[NUM_DIGITS_GF2X_ELEMENT];
    gf2x_mod_mul_dense_to_sparse(privateSyndrome,
                                 (const DIGIT * const) ct,
                                 sk->HtrPosOnes[N0-1],
                                 V);

    int decode_ok = bf_decoding_CT(((DIGIT *)(&decoded_error_vector[1])),
                                (const POSITION_T (*)[V]) sk->HtrPosOnes,
                                (const POSITION_T (*)[V]) sk->HPosOnes,
                                privateSyndrome);
    // decode_ok == 1 means that no decoding failure occurred

/****         HERE starts the reconstruction of the seed employed in       ****/
/****         encryption and the re-generation of the error vector         ****/
/****         in a test_err_vect[...] array.                               ****/

   unsigned char reconstructed_err_vect_seed[TRNG_BYTE_LENGTH];
   unsigned char tagMask[HASH_BYTE_LENGTH];
   decoded_error_vector[0] = 0x01;
   HASH_FUNCTION((const unsigned char *) decoded_error_vector, // input
                 (1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B),  // input Length
                 tagMask);
   decoded_error_vector[0] = 0x00;
   for (int i = 0; i < TRNG_BYTE_LENGTH; ++i)
      reconstructed_err_vect_seed[i] = tag[i] ^ tagMask[i];

   xof_shake_t test_err_vect_seed_expander;
   shake_seedexpander_init(&test_err_vect_seed_expander,
                           reconstructed_err_vect_seed);
   POSITION_T test_errorPos[NUM_ERRORS_T];
   rand_error_pos_shake(test_errorPos,
                        &test_err_vect_seed_expander);


   unsigned char test_err_vect[1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B];
   test_err_vect[0]=0x00;
   expand_error(((DIGIT *)(&test_err_vect[1])),
                test_errorPos);

   int error_value_ok = memcmp(decoded_error_vector,
                               test_err_vect,
                               1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   // error_value_ok == 0 means that values
   // in decoded_error_vect[...] equal the ones in test_err_vect[...]

/****                                                                      ****/
/**** END CHECK BETWEEN decoded_error_vector[...] and test_error_vect[...] ****/
/****                                                                      ****/

   unsigned char tmp[1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B] = { 0x00 };
   // tmp stores the concatenation
   // of the secret LongTermSeed with size TRNG_BYTE_LENGTH
   // and the ciphertext ct with size NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B
   // However, for constant time implementation reasons, it needs to be longer
   // and precisely as long as the error vector
   // that has size  1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B,
   // while the first byte must be equal to 0x00
   memcpy((unsigned char* const)(&tmp[1]),
          (const unsigned char *const) sk->LongTermSeed,
          TRNG_BYTE_LENGTH);
   memcpy((unsigned char*const)(&tmp[1+TRNG_BYTE_LENGTH]),
          (const unsigned char *const) ct,
          NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);

   if (decode_ok == 1   &&
       error_value_ok == 0 ) { // this check also includes HW(e) == t

       // decoded_error_vector[0] = 0x00; this byte is already set to zero
       HASH_FUNCTION((unsigned char * const) decoded_error_vector,
                 (1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B),
                 ss);
   }
   else {
       // tmp[0] = 0x00; this byte is already set to zero
       HASH_FUNCTION((unsigned char * const) tmp,
                        (1+N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B),
                        ss);
   }
   return 1;
} // end decrypt_niederreiter_indcpa
