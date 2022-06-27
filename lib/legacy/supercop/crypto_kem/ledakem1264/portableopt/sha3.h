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
#define SHA_3_LIBKECCAK

#pragma once
#if defined(SHA_3_LIBKECCAK)
#include <libkeccak.a.headers/KeccakHash.h>

static inline
void xof_shake_init(Keccak_HashInstance *state, int val) {

    if (val == 128)
       Keccak_HashInitialize_SHAKE128(state);
    else
       Keccak_HashInitialize_SHAKE256(state);
}

static inline
void xof_shake_update(Keccak_HashInstance *state,
                      const unsigned char *input,
                      unsigned int inputByteLen,
                      unsigned char *output){

   Keccak_HashUpdate(state,
                     (const BitSequence *) input,
                     (BitLength) inputByteLen
                    );

}

static inline
void xof_shake_final_and_extract(Keccak_HashInstance *state,
                                 unsigned char *output,
                                 unsigned int outputByteLen){
   Keccak_HashFinal(state,
                    (BitSequence *) output
                   );
   Keccak_HashSqueeze(state,
                      (BitSequence *) output,
                      (BitLength) outputByteLen
                     );

}

static inline
void xof_shake_extract(Keccak_HashInstance *state,
                       unsigned char *output,
                       unsigned int outputByteLen){
   Keccak_HashSqueeze(state,
                      (BitSequence *) output,
                      (BitLength) outputByteLen
                     );
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline
void sha3_256(const unsigned char *input,
              unsigned int inputByteLen,
              unsigned char *output)
{
   Keccak_HashInstance state;
   Keccak_HashInitialize(&state, 1088,  512, 256, 0x06);
   Keccak_HashUpdate(&state, input, inputByteLen*8);
  Keccak_HashFinal(&state, output);
}

/**
  *  Function to compute SHA3-384 on the input message.
  *  The output length is fixed to 48 bytes.
  */
static inline
void sha3_384(const unsigned char *input,
              unsigned int inputByteLen,
              unsigned char *output)
{
   Keccak_HashInstance state;
   Keccak_HashInitialize(&state, 832,  768, 384, 0x06);
   Keccak_HashUpdate(&state, input, inputByteLen*8);
   Keccak_HashFinal(&state, output);
}

/**
  *  Function to compute SHA3-512 on the input message.
  *  The output length is fixed to 64 bytes.
  */
static inline
void sha3_512(const unsigned char *input,
              unsigned int inputByteLen,
              unsigned char *output)
{
   Keccak_HashInstance state;
   Keccak_HashInitialize(&state, 576,  1024, 512, 0x06);
   Keccak_HashUpdate(&state, input, inputByteLen*8);
   Keccak_HashFinal(&state, output);
}

#else

void Keccak(      unsigned int rate,
                  unsigned int capacity,
                  const unsigned char *input,
                  unsigned long long int inputByteLen,
                  unsigned char delimitedSuffix,
                  unsigned char *output,
                  unsigned long long int outputByteLen);

/**
  *  Function to compute SHA3-256 on the input message.
  *  The output length is fixed to 32 bytes.
  */
static inline
void sha3_256(const unsigned char *input,
              unsigned int inputByteLen,
              unsigned char *output)
{
   Keccak(1088, 512, input, inputByteLen, 0x06, output, 32);
}

/**
  *  Function to compute SHA3-384 on the input message.
  *  The output length is fixed to 48 bytes.
  */
static inline
void sha3_384(const unsigned char *input,
              unsigned int inputByteLen,
              unsigned char *output)
{
   Keccak(832, 768, input, inputByteLen, 0x06, output, 48);
}

/**
  *  Function to compute SHA3-512 on the input message.
  *  The output length is fixed to 64 bytes.
  */
static inline
void sha3_512(const unsigned char *input,
              unsigned int inputByteLen,
              unsigned char *output)
{
   Keccak(576, 1024, input, inputByteLen, 0x06, output, 64);
}

#endif
