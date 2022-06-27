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

#include <string.h>
#include <stdint.h>

#include "primitives.h"

#ifdef SHADOW_TYPE_32_BIT

#define CLYDE_128_NS 6                // Number of steps
#define CLYDE_128_NR 2 * CLYDE_128_NS // Number of rounds
#define SHADOW_NS 6                   // Number of steps
#define SHADOW_NR 2 * SHADOW_NS       // Number of rounds

// Apply a S-box layer to a Clyde-128 state.
static void sbox_layer(uint32_t* state) {
  uint32_t y1 = (state[0] & state[1]) ^ state[2];
  uint32_t y0 = (state[3] & state[0]) ^ state[1];
  uint32_t y3 = (y1 & state[3]) ^ state[0];
  uint32_t y2 = (y0 & y1) ^ state[3];
  state[0] = y0;
  state[1] = y1;
  state[2] = y2;
  state[3] = y3;
}

// Apply a L-box to a pair of Clyde-128 rows.
#define ROT32(x,n) ((uint32_t)(((x)>>(n))|((x)<<(32-(n)))))
static void lbox(uint32_t* x, uint32_t* y) {
  uint32_t a, b, c, d;
  a = *x ^ ROT32(*x, 12);
  b = *y ^ ROT32(*y, 12);
  a = a ^ ROT32(a, 3);
  b = b ^ ROT32(b, 3);
  a = a ^ ROT32(*x, 17);
  b = b ^ ROT32(*y, 17);
  c = a ^ ROT32(a, 31);
  d = b ^ ROT32(b, 31);
  a = a ^ ROT32(d, 26);
  b = b ^ ROT32(c, 25);
  a = a ^ ROT32(c, 15);
  b = b ^ ROT32(d, 15);
  *x = a;
  *y = b;
}

static const uint32_t CST_LFSR_POLY_MASK = 0xc5;
// Initial value of the constant generation polynomial
// This is the result of applying the LFSR function 1024 times
// the value 0x1.
static const uint32_t CST_LFSR_INIT_VALUE = 0xf8737400;
// Row on which to XOR the constant in Shadow Round A
static const uint32_t SHADOW_RA_CST_ROW = 1;
// Bundle on which to XOR the constant in Shadow Round B
static const uint32_t SHADOW_RB_CST_BUNDLE = 0;

// Update (by 1 step) the constant generation LFSR
static uint32_t update_lfsr(uint32_t lfsr)
{
  // Arithmetic shift left, equivalent to
  // uint32_t b_out_ext = (lfsr & 0x80000000) ? 0xffffffff : 0x0;
  // but constant-time.
  uint32_t b_out_ext = (uint32_t)(((int32_t)lfsr) >> 31);
  return (lfsr << 1) ^ (b_out_ext & CST_LFSR_POLY_MASK);
}

// Multiplication by polynomial x modulo x^32+x^8+1
static uint32_t xtime(uint32_t x)
{
  uint32_t b = x >> 31;
  return (x << 1) ^ b ^ (b << 8);
}

// Apply a D-box layer to a Shadow state.
static void dbox_mls_layer(shadow_state state, unsigned int row) {
#if SMALL_PERM
    uint32_t x0 = state[0][row];
    uint32_t x1 = state[1][row];
    uint32_t x2 = state[2][row];
    uint32_t a = x0 ^ x1;
    uint32_t b = x0 ^ x2;
    uint32_t c = x1 ^ b;
    uint32_t d = a ^ xtime(b);
    state[0][row] = b ^ d;
    state[1][row] = c;
    state[2][row] = d;
#else
    state[0][row] ^= state[1][row];
    state[2][row] ^= state[3][row];
    state[1][row] ^= state[2][row];
    state[3][row] ^= xtime(state[0][row]);
    state[1][row] = xtime(state[1][row]);
    state[0][row] ^= state[1][row];
    state[2][row] ^= xtime(state[3][row]);
    state[1][row] ^= state[2][row];
    state[3][row] ^= state[0][row];
#endif // SMALL_PERM
}

// Shadow permutation. Updates state.
void shadow(shadow_state state) {
  uint32_t lfsr = CST_LFSR_INIT_VALUE;
  for (unsigned int s = 0; s < SHADOW_NS; s++) {
    for (unsigned int b = 0; b < MLS_BUNDLES; b++) {
      sbox_layer(state[b]);
      lbox(&state[b][0], &state[b][1]);
      lbox(&state[b][2], &state[b][3]);
      state[b][SHADOW_RA_CST_ROW] ^= lfsr;
      lfsr = update_lfsr(lfsr);
      sbox_layer(state[b]);
    }
    for (unsigned int row = 0; row < LS_ROWS; row++) {
      dbox_mls_layer(state, row);
      state[SHADOW_RB_CST_BUNDLE][row] ^= lfsr;
      lfsr = update_lfsr(lfsr);
    }
  }
}

#endif // SHADOW_TYPE_32_BIT
