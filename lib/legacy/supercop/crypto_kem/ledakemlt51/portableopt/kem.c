/**
 *
 * <kem.c>
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

#include "niederreiter_keygen.h"
#include "niederreiter_encrypt.h"
#include "niederreiter_decrypt.h"
#include "rng.h"
#include "sha3.h"
#include <string.h>
#include "randombytes.h"
#include "crypto_kem.h"
/* Generates a keypair - pk is the public key and sk is the secret key. */
int crypto_kem_keypair( unsigned char *pk,
                        unsigned char *sk )
{

   key_gen_niederreiter((publicKeyNiederreiter_t *) pk,
                        (privateKeyNiederreiter_t *) sk);
   return 0;
}

/* Encrypt - pk is the public key, ct is a key encapsulation message
  (ciphertext), ss is the shared secret.*/
int crypto_kem_enc( unsigned char *ct,
                    unsigned char *ss,
                    const unsigned char *pk )
{

   AES_XOF_struct niederreiter_encap_key_expander;
   unsigned char encapsulated_key_seed[TRNG_BYTE_LENGTH];
   randombytes(encapsulated_key_seed,TRNG_BYTE_LENGTH);
   seedexpander_from_trng(&niederreiter_encap_key_expander,encapsulated_key_seed);

   POSITION_T errorPos[NUM_ERRORS_T];
   rand_error_pos(errorPos,&niederreiter_encap_key_expander);
   
   DIGIT error_vector[N0*NUM_DIGITS_GF2X_ELEMENT];
   expand_error(error_vector,errorPos);

   HASH_FUNCTION((const unsigned char *) error_vector,    // input
                 (N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B), // input Length
                 ss);

   encrypt_niederreiter((DIGIT *) ct,
                        (publicKeyNiederreiter_t *) pk,
                        errorPos,
                        error_vector);
   return 0;
}


/* Decrypt - ct is a key encapsulation message (ciphertext), sk is the private
   key, ss is the shared secret */

int crypto_kem_dec( unsigned char *ss,
                    const unsigned char *ct,
                    const unsigned char *sk )
{
   DIGIT decoded_error_vector[N0*NUM_DIGITS_GF2X_ELEMENT];

   int decode_ok = decrypt_niederreiter(decoded_error_vector,
                                        (privateKeyNiederreiter_t *)sk,
                                        (DIGIT *)ct);
   HASH_FUNCTION((const unsigned char *) decoded_error_vector,
                    (N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B),
                    ss);
   if (decode_ok == 1) {
      return 0;
   }
   return 1;
}
