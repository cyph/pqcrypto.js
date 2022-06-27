/* MIT License
 *
 * Copyright (c) 2019 2020 Gaëtan Cassiers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "s1p.h"
#include "parameters.h"
#include "primitives.h"

#define CAPACITY_BYTES 32
#define RATE_BYTES (SHADOW_NBYTES - CAPACITY_BYTES)
#define RATE_BUNDLES (RATE_BYTES/(LS_ROWS*LS_ROW_BYTES))

// Working mode for block compression.
typedef enum {
  AD,
  PLAINTEXT,
  CIPHERTEXT
} compress_mode;

static void compress_block(unsigned char *state, unsigned char *out,
                           const unsigned char *d, compress_mode mode,
                           unsigned long long offset, unsigned long long n);

static unsigned long long compress_data(shadow_state state,
                                        unsigned char *out,
                                        const unsigned char *d,
                                        unsigned long long dlen,
                                        compress_mode mode);

static void init_sponge_state(shadow_state state,
                              const unsigned char *k, const unsigned char *p,
                              const unsigned char *n);
static void xor_bytes(unsigned char* dest, const unsigned char* src1,
               const unsigned char* src2, unsigned long long n);

void init_keys(const unsigned char **k, unsigned char p[P_NBYTES],
               const unsigned char *k_glob) {
  *k = k_glob;
#if MULTI_USER
  memcpy(p, k_glob + CLYDE128_NBYTES, P_NBYTES);
  p[P_NBYTES - 1] &= 0x7F; // set last p bit to 0
  p[P_NBYTES - 1] |= 0x40; // set next to last p bit to 0
#else
  memset(p, 0, P_NBYTES);
#endif // MULTI_USER
}

static void init_sponge_state(shadow_state state,
                              const unsigned char *k, const unsigned char *p,
                              const unsigned char *n) {
  // init state
  memset(state, 0, SHADOW_NBYTES);
  memcpy(state[1], p, P_NBYTES);
  memcpy(state[2], n, CRYPTO_NPUBBYTES);
  // TBC
  memcpy(state[0], n, CRYPTO_NPUBBYTES);
  clyde128_encrypt(state[0], state[1], k);

  // initial permutation
  shadow(state);
}

void s1p_encrypt(unsigned char *c, unsigned long long *clen,
                 const unsigned char *ad, unsigned long long adlen,
                 const unsigned char *m, unsigned long long mlen,
                 const unsigned char *k, const unsigned char *p,
                 const unsigned char *n) {
  // permutation state
  shadow_state state;
  init_sponge_state(state, k, p, n);

  // compress associated data
  compress_data(state, NULL, ad, adlen, AD);

  // compress message
  unsigned long long c_bytes = 0;
  if (mlen > 0) {
    state[RATE_BUNDLES][0] ^= 0x01;
    c_bytes = compress_data(state, c, m, mlen, PLAINTEXT);
  }

  // tag
  state[1][LS_ROWS- 1] |= 0x80000000;
  clyde128_encrypt(state[0], state[1], k);
  memcpy(c+c_bytes, state[0], CLYDE128_NBYTES);
  *clen = c_bytes + CLYDE128_NBYTES;
}

int s1p_decrypt(unsigned char *m, unsigned long long *mlen,
                const unsigned char *ad, unsigned long long adlen,
                const unsigned char *c, unsigned long long clen,
                const unsigned char *k, const unsigned char *p,
                const unsigned char *n) {
  // permutation state
  shadow_state state;
  init_sponge_state(state, k, p, n);

  // compress associated data
  compress_data(state, NULL, ad, adlen, AD);

  // compress message
  unsigned long long m_bytes = 0;
  if (clen > CLYDE128_NBYTES) {
    state[RATE_BUNDLES][0] ^= 0x01;
    m_bytes = compress_data(state, m, c, clen - CLYDE128_NBYTES, CIPHERTEXT);
  }

  // tag verification
  state[1][LS_ROWS- 1] |= 0x80000000;
  clyde128_encrypt(state[0], state[1], k);
  unsigned char *st0 = (unsigned char *) state[0];
  int tag_ok = 1;
  for (int i = 0; i < 4*LS_ROWS; i++) {
    tag_ok &= (st0[i] == c[m_bytes+i]);
  }
  if (tag_ok) {
    *mlen = m_bytes;
    return 0;
  } else {
    // Reset output buffer to avoid unintended unauthenticated plaintext
    // release.
    memset(m, 0, clen - CLYDE128_NBYTES);
    *mlen = 0;
    return -1;
  }
}

// Compress a block into the state. Length of the block is n and buffers are
// accessed starting at offset.  Input block is d, output is written into
// buffer out if mode is PLAINTEXT or CIPHERTEXT.
// Only the XOR operation is performed, not XORing of padding constants.
static void compress_block(unsigned char *state, unsigned char *out,
                           const unsigned char *d, compress_mode mode,
                           unsigned long long offset, unsigned long long n) {
  if (mode == CIPHERTEXT) {
    xor_bytes(out + offset, state, d + offset, n);
    memcpy(state, d + offset, n);
  } else {
    xor_bytes(state, state, d + offset, n);
    if (mode == PLAINTEXT) {
      memcpy(out + offset, state, n);
    }
  }
}

// Compress a block into the state (in duplex-sponge mode).
// Input data buffer is d with length dlen.
// Output is written into buffer out if mode is PLAINTEXT or CIPHERTEXT.
// Padding is handled if needed.
static unsigned long long compress_data(shadow_state state,
                                        unsigned char *out,
                                        const unsigned char *d,
                                        unsigned long long dlen,
                                        compress_mode mode) {
  unsigned long long i;
  for (i = 0; i < dlen / RATE_BYTES; i++) {
    compress_block((uint8_t *)state, out, d, mode, i * RATE_BYTES, RATE_BYTES);
    shadow(state);
  }
  int rem = dlen % RATE_BYTES;
  if (rem != 0) {
    compress_block((uint8_t *)state, out, d, mode, i * RATE_BYTES, rem);
    ((uint8_t *)state)[rem] ^= 0x01;
    ((uint8_t *)state)[RATE_BYTES] ^= 0x02;
    shadow(state);
  }
  return i * RATE_BYTES + rem;
}

// XOR buffers src1 and src2 into buffer dest (all buffers contain n bytes).
void xor_bytes(unsigned char* dest, const unsigned char* src1,
               const unsigned char* src2, unsigned long long n) {
  for (unsigned long long i = 0; i < n; i++) {
    dest[i] = src1[i] ^ src2[i];
  }
}
