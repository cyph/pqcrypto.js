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

#include "mceliece_keygen.h"
#include "mceliece_cca2_encrypt.h"
#include "mceliece_cca2_decrypt.h"
#include "rng.h"
#include <string.h>
#include <stdio.h>
#include "crypto_encrypt.h"

int crypto_encrypt_keypair( unsigned char *pk,
                            unsigned char *sk )
{

   key_gen_mceliece((publicKeyMcEliece_t *) pk,
                    (privateKeyMcEliece_t *) sk);
   return 0;
}

int crypto_encrypt( unsigned char *c, unsigned long long *clen,
                    const unsigned char *m, unsigned long long mlen,
                    const unsigned char *pk )
{
   /* NIST API provides a byte aligned message: all bytes are assumed full.
    * Therefore, if mlen exceeds
    * floor( (k-8*(KOBARA_IMAI_CONSTANT_LENGTH_B+sizeof(KI_LENGTH_FIELD_TYPE)))/8 )
    * defined as MAX_BYTES_IN_IWORD the message will not fit , together with
    * the constant and its length, in the information word
    *
    * The minimum ciphertext overhead is
    * NUM_DIGITS_GF2X_ELEMENT +
    * KOBARA_IMAI_CONSTANT_LENGTH_B +
    * sizeof(KI_LENGTH_FIELD_TYPE)  */

    if (mlen<=MAX_BYTES_IN_IWORD){
      *clen= N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B;
    } else {
      int leftover_len = mlen- MAX_BYTES_IN_IWORD;
      *clen= N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B + leftover_len;
    }
   if (encrypt_Kobara_Imai(c,
                           (publicKeyMcEliece_t *)pk,
                           mlen,
                           m) == 1) {
      return 0;
   }
   return -1;
}

int crypto_encrypt_open( unsigned char *m, unsigned long long *mlen,
                         const unsigned char *c, unsigned long long clen,
                         const unsigned char *sk )
{
   if ( decrypt_Kobara_Imai(m,
                            mlen,
                            (privateKeyMcEliece_t *)sk,
                            clen,
                            c) == 1 ) {
      return 0;
   }

   return -1;
}
