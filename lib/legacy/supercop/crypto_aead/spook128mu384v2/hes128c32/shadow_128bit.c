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

#ifdef BENCH_IACA
#include "iacaMarks.h"
#else
#define IACA_START
#define IACA_END
#endif

#include <xmmintrin.h>

#include "primitives.h"

#ifdef SHADOW_TYPE_128_BIT

#define SHADOW_NS 6                   // Number of steps
#define SHADOW_NR 2 * SHADOW_NS       // Number of rounds

#define LS_ROWS 4      // Rows in the LS design
#define LS_ROW_BYTES 4 // number of bytes per row in the LS design
#define MLS_BUNDLES                                                            \
  (SHADOW_NBYTES / (LS_ROWS* LS_ROW_BYTES)) // Bundles in the mLS design

typedef uint32_t row_set __attribute__ ((vector_size (16)));
typedef int32_t srow_set __attribute__ ((vector_size (16)));

typedef struct __attribute__((aligned(64))) shadow_simd {
    row_set rows[4];
} shadow_simd;

static void sbox_layer_simd(shadow_simd* simd);
static void lbox_simd(row_set* x, row_set* y);
static void lbox_layer_simd(shadow_simd* simd);
static void dbox_mls_layer_simd(shadow_simd *simd);

static void sbox_layer_simd(shadow_simd* simd) {
  row_set y1 = (simd->rows[0] & simd->rows[1]) ^ simd->rows[2];
  row_set y0 = (simd->rows[3] & simd->rows[0]) ^ simd->rows[1];
  row_set y3 = (y1 & simd->rows[3]) ^ simd->rows[0];
  row_set y2 = (y0 & y1) ^ simd->rows[3];
  simd->rows[0] = y0;
  simd->rows[1] = y1;
  simd->rows[2] = y2;
  simd->rows[3] = y3;
}

#define ROT32(x,n) (((x)>>(n))|((x)<<(32-(n))))
static void lbox_simd(row_set* x, row_set* y) {
  row_set a, b, c, d;
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

static void lbox_layer_simd(shadow_simd* simd) {
  lbox_simd(&simd->rows[0], &simd->rows[1]);
  lbox_simd(&simd->rows[2], &simd->rows[3]);
}

static const row_set dbox_shuffle1 = { 0, 4, 1, 5 };
static const row_set dbox_shuffle2 = { 2, 6, 3, 7 };
static const row_set dbox_shuffle3 = { 0, 1, 4, 5 };
static const row_set dbox_shuffle4 = { 2, 3, 6, 7 };
static void transpose_state(shadow_simd *simd) {
    row_set t0 = __builtin_shuffle(simd->rows[0], simd->rows[1], dbox_shuffle1);
    row_set t1 = __builtin_shuffle(simd->rows[2], simd->rows[3], dbox_shuffle1);
    row_set t2 = __builtin_shuffle(simd->rows[0], simd->rows[1], dbox_shuffle2);
    row_set t3 = __builtin_shuffle(simd->rows[2], simd->rows[3], dbox_shuffle2);
    simd->rows[0] = __builtin_shuffle(t0, t1, dbox_shuffle3);
    simd->rows[1] = __builtin_shuffle(t0, t1, dbox_shuffle4);
    simd->rows[2] = __builtin_shuffle(t2, t3, dbox_shuffle3);
    simd->rows[3] = __builtin_shuffle(t2, t3, dbox_shuffle4);
    /*
    __m128i I0 = simd->rows[0];
    __m128i I1 = simd->rows[1];
    __m128i I2 = simd->rows[2];
    __m128i I3 = simd->rows[3];
    __m128i T0 = _mm_unpacklo_epi32(I0, I1);
    __m128i T1 = _mm_unpacklo_epi32(I2, I3);
    __m128i T2 = _mm_unpackhi_epi32(I0, I1);
    __m128i T3 = _mm_unpackhi_epi32(I2, I3);
    simd->rows[0] = _mm_unpacklo_epi64(T0, T1);
    simd->rows[1] = _mm_unpackhi_epi64(T0, T1);
    simd->rows[2] = _mm_unpacklo_epi64(T2, T3);
    simd->rows[3] = _mm_unpackhi_epi64(T2, T3);
    */
}
static row_set xtime(row_set x) {
    row_set b = (row_set) (((srow_set) x) >> 31);
    return (x << 1) ^ (b & 0x101);
}
static void dbox_mls_layer_simd(shadow_simd *simd) {
#if SMALL_PERM==0
    simd->rows[0] ^= simd->rows[1];
    simd->rows[2] ^= simd->rows[3];
    simd->rows[1] ^= simd->rows[2];
    simd->rows[3] ^= xtime(simd->rows[0]);
    simd->rows[1] = xtime(simd->rows[1]);
    simd->rows[0] ^= simd->rows[1];
    simd->rows[2] ^= xtime(simd->rows[3]);
    simd->rows[1] ^= simd->rows[2];
    simd->rows[3] ^= simd->rows[0];
#else
    row_set x0 = simd->rows[0];
    row_set x1 = simd->rows[1];
    row_set x2 = simd->rows[2];
    row_set a = x0 ^ x1;
    row_set b = x0 ^ x2;
    row_set c = x1 ^ b;
    row_set d = a ^ xtime(b);
    simd->rows[0] = b ^ d;
    simd->rows[1] = c;
    simd->rows[2] = d;
#endif
}

// Row on which to XOR the constant in Shadow Round A
static const uint32_t SHADOW_RA_CST_ROW = 1;
// Bundle on which to XOR the constant in Shadow Round B
static const uint32_t SHADOW_RB_CST_BUNDLE = 0;

#if SMALL_PERM==0
static const row_set SHADOW_CST_RA[SHADOW_NS] = {
    { 0xf8737400, 0xf0e6e8c5, 0xe1cdd14f, 0xc39ba25b },
    { 0x73744118, 0xe6e88230, 0xcdd104a5, 0x9ba2098f },
    { 0x74413cff, 0xe88279fe, 0xd104f339, 0xa209e6b7 },
    { 0x413cd9a4, 0x8279b348, 0x4f36655, 0x9e6ccaa },
    { 0x3cd99585, 0x79b32b0a, 0xf3665614, 0xe6ccaced },
    { 0xd99594cc, 0xb32b295d, 0x6656527f, 0xccaca4fe } 
};
static const row_set SHADOW_CST_RB[SHADOW_NS] = {
    { 0x87374473, 0xe6e8823, 0x1cdd1046, 0x39ba208c },
 { 0x374413db, 0x6e8827b6, 0xdd104f6c, 0xba209e1d },
 { 0x4413cdab, 0x88279b56, 0x104f3669, 0x209e6cd2 },
 { 0x13cd9954, 0x279b32a8, 0x4f366550, 0x9e6ccaa0 },
 { 0xcd99591f, 0x9b32b2fb, 0x36656533, 0x6ccaca66 },
 { 0x99594939, 0x32b292b7, 0x6565256e, 0xcaca4adc } 
};
#else
static const row_set SHADOW_CST_RA[SHADOW_NS] = {
    { 0xf8737400, 0xf0e6e8c5, 0xe1cdd14f, 0x0 },
 { 0x39ba208c, 0x73744118, 0xe6e88230, 0x0 },
 { 0xdd104f6c, 0xba209e1d, 0x74413cff, 0x0 },
 { 0x88279b56, 0x104f3669, 0x209e6cd2, 0x0 },
 { 0x13cd9954, 0x279b32a8, 0x4f366550, 0x0 },
 { 0xe6ccaced, 0xcd99591f, 0x9b32b2fb, 0x0 } 
};
static const row_set SHADOW_CST_RB[SHADOW_NS] = {
    { 0xc39ba25b, 0x87374473, 0xe6e8823, 0x1cdd1046 },
 { 0xcdd104a5, 0x9ba2098f, 0x374413db, 0x6e8827b6 },
 { 0xe88279fe, 0xd104f339, 0xa209e6b7, 0x4413cdab },
 { 0x413cd9a4, 0x8279b348, 0x4f36655, 0x9e6ccaa },
 { 0x9e6ccaa0, 0x3cd99585, 0x79b32b0a, 0xf3665614 },
 { 0x36656533, 0x6ccaca66, 0xd99594cc, 0xb32b295d } 
};
#endif // SMALL_PERM==0


void shadow(shadow_state state) {
#if SMALL_PERM==0
    shadow_simd simd = {
        {
            { state[0][0], state[1][0], state[2][0], state[3][0] },
            { state[0][1], state[1][1], state[2][1], state[3][1] },
            { state[0][2], state[1][2], state[2][2], state[3][2] },
            { state[0][3], state[1][3], state[2][3], state[3][3] }
        }
    };
#else
    shadow_simd simd = {
        {
            { state[0][0], state[1][0], state[2][0], 0 },
            { state[0][1], state[1][1], state[2][1], 0 },
            { state[0][2], state[1][2], state[2][2], 0 },
            { state[0][3], state[1][3], state[2][3], 0 }
        }
    };
#endif // SMALL_PERM==0
    for (unsigned int s = 0; s < SHADOW_NS; s++) {
        IACA_START
            sbox_layer_simd(&simd);
        lbox_layer_simd(&simd);
            simd.rows[SHADOW_RA_CST_ROW] ^= SHADOW_CST_RA[s];
        sbox_layer_simd(&simd);
        transpose_state(&simd);
        dbox_mls_layer_simd(&simd);
        simd.rows[SHADOW_RB_CST_BUNDLE] ^= SHADOW_CST_RB[s];
        transpose_state(&simd);
    }
    IACA_END
    row_set res0 = { simd.rows[0][0], simd.rows[1][0], simd.rows[2][0], simd.rows[3][0] };
    row_set res1 = { simd.rows[0][1], simd.rows[1][1], simd.rows[2][1], simd.rows[3][1] };
    row_set res2 = { simd.rows[0][2], simd.rows[1][2], simd.rows[2][2], simd.rows[3][2] };
    row_set res3 = { simd.rows[0][3], simd.rows[1][3], simd.rows[2][3], simd.rows[3][3] };
    memcpy(state[0], &res0, sizeof(row_set));
    memcpy(state[1], &res1, sizeof(row_set));
    memcpy(state[2], &res2, sizeof(row_set));
#if SMALL_PERM==0
    memcpy(state[3], &res3, sizeof(row_set));
#endif // SMALL_PERM==0
}

#endif // SHADOW_TYPE_128_BIT
