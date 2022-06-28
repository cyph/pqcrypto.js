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
#include "rng.h"
#include "sha3.h"
#include <string.h>


/*----------------------------------------------------------------------------*/

int decrypt_niederreiter_indcpa(unsigned char *const ss, 
                                 const unsigned char *const ct, 
                                 const privateKeyNiederreiter_t * const sk)
{   // return 1 in case of success, 0 otherwise 

    DIGIT decoded_error_vector[N0*NUM_DIGITS_GF2X_ELEMENT];

    DIGIT privateSyndrome[NUM_DIGITS_GF2X_ELEMENT];
    gf2x_mod_mul_dense_to_sparse(privateSyndrome,
                                 (const DIGIT * const) ct,
                                 sk->Ln0trSparse,
                                 DV*M);
   int decode_ok = 0;
   memset(decoded_error_vector, 0x00, N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B);
   decode_ok = bf_decoding(decoded_error_vector, 
                           (const POSITION_T (*)[DV]) sk->HtrPosOnes,
                           (const POSITION_T (*)[M])  sk->QtrPosOnes, 
                            privateSyndrome);
   HASH_FUNCTION((const unsigned char *) decoded_error_vector,
                 (N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B),
                 ss);

   return decode_ok;
} // end decrypt_niederreiter_indcpa 
