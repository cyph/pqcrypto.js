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

#pragma once

#include "qc_ldpc_parameters.h"
#include "gf2x_limbs.h"
#include "gf2x_arith_mod_xPplusOne.h"
#include "mceliece.h"

#define CRYPTO_ALGNAME "LEDAPKC"

/* size in bytes of the secret key */
#define  CRYPTO_SECRETKEYBYTES (sizeof(privateKeyMcEliece_t))

/* size in bytes of the public key */
#define  CRYPTO_PUBLICKEYBYTES (sizeof(publicKeyMcEliece_t))

/* size in bytes of maximum overhead on the encrypted message */
#define  CRYPTO_BYTES (N0*NUM_DIGITS_GF2X_ELEMENT*DIGIT_SIZE_B)

/*----------------------------------------------------------------------------*/
/*                                  PKC API                                   */
/*----------------------------------------------------------------------------*/

/* Generates a keypair - pk is the public key and
 *                       sk is the secret key.
 */
int crypto_encrypt_keypair( unsigned char *pk,
                            unsigned char *sk );

/* Encrypt - c is the ciphertext,
 *           m is the plaintext,
 *           pk is the public key
 */
int crypto_encrypt( unsigned char *c, unsigned long long *clen,
                    const unsigned char *m, unsigned long long mlen,
                    const unsigned char *pk );

/* Decrypt - c is the ciphertext,
 *           m is the plaintext,
 *           sk is the secret key
 */
int crypto_encrypt_open( unsigned char *m, unsigned long long *mlen,
                         const unsigned char *c, unsigned long long clen,
                         const unsigned char *sk );
