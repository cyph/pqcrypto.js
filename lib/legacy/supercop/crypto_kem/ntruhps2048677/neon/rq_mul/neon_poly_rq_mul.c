/*=============================================================================
 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/
#include <arm_neon.h>

#include "../params.h"
#include "neon_batch_multiplication.c"
#include "neon_matrix_transpose.c"

#define NTRU_N 677
#define NTRU_N_PAD 720
#define SB0 (NTRU_N_PAD / 3) // 240
#define SB1 (SB0 / 4)        // 60
#define SB1_PAD 64           // 64
#define SB2 (SB1 / 2)        // 30 -- Not use
#define SB3 (SB2 / 2)        // 15
#define SB3_PAD 16           // 16

#define SB3_RES (2 * SB3) // 30
#define SB3_RES_PAD 32    // 32
#define SB2_RES (2 * SB2) // 60
#define SB2_RES_PAD 64 // 64
#define SB1_RES (2 * SB1) // 120
#define SB0_RES (2 * SB0) // 480

#define MASK (NTRU_Q - 1)

#if defined(__clang__)

// load c <= a
#define vload(c, a) c = vld1q_u16_x4(a);

// store c <= a
#define vstore(c, a) vst1q_u16_x4(c, a);

#elif defined(__GNUC__)

#define vload(c, a)               \
    c.val[0] = vld1q_u16(a);      \
    c.val[1] = vld1q_u16(a + 8);  \
    c.val[2] = vld1q_u16(a + 16); \
    c.val[3] = vld1q_u16(a + 24);

#define vstore(c, a)             \
    vst1q_u16(c, a.val[0]);      \
    vst1q_u16(c + 8, a.val[1]);  \
    vst1q_u16(c + 16, a.val[2]); \
    vst1q_u16(c + 24, a.val[3]);

#else
#error "Unsupported compiler"
#endif

#define inv3 43691
#define inv15 61167

// c = a << value
#define vsl(c, a, value)                     \
    c.val[0] = vshlq_n_u16(a.val[0], value); \
    c.val[1] = vshlq_n_u16(a.val[1], value); \
    c.val[2] = vshlq_n_u16(a.val[2], value); \
    c.val[3] = vshlq_n_u16(a.val[3], value);

// c = a >> value
#define vsr(c, a, value)                     \
    c.val[0] = vshrq_n_u16(a.val[0], value); \
    c.val[1] = vshrq_n_u16(a.val[1], value); \
    c.val[2] = vshrq_n_u16(a.val[2], value); \
    c.val[3] = vshrq_n_u16(a.val[3], value);

// c = a + b
#define vadd(c, a, b)                         \
    c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
    c.val[1] = vaddq_u16(a.val[1], b.val[1]); \
    c.val[2] = vaddq_u16(a.val[2], b.val[2]); \
    c.val[3] = vaddq_u16(a.val[3], b.val[3]);

// c = a - b
#define vsub(c, a, b)                         \
    c.val[0] = vsubq_u16(a.val[0], b.val[0]); \
    c.val[1] = vsubq_u16(a.val[1], b.val[1]); \
    c.val[2] = vsubq_u16(a.val[2], b.val[2]); \
    c.val[3] = vsubq_u16(a.val[3], b.val[3]);

// c = a * value
#define vmuln(c, a, value)                   \
    c.val[0] = vmulq_n_u16(a.val[0], value); \
    c.val[1] = vmulq_n_u16(a.val[1], value); \
    c.val[2] = vmulq_n_u16(a.val[2], value); \
    c.val[3] = vmulq_n_u16(a.val[3], value);

// c = value
#define vzero(c, value)            \
    c.val[0] = vmovq_n_u16(value); \
    c.val[1] = vmovq_n_u16(value); \
    c.val[2] = vmovq_n_u16(value); \
    c.val[3] = vmovq_n_u16(value);

// c = ~a
#define vnot(c, a)                  \
    c.val[0] = vmvnq_u16(a.val[0]); \
    c.val[1] = vmvnq_u16(a.val[1]); \
    c.val[2] = vmvnq_u16(a.val[2]); \
    c.val[3] = vmvnq_u16(a.val[3]);

// c = a
#define vdup(c, a) c = vdupq_n_u16(a);

// c = c + a_const
#define vadd_const(c, a, b_const)            \
    c.val[0] = vaddq_u16(a.val[0], b_const); \
    c.val[1] = vaddq_u16(a.val[1], b_const); \
    c.val[2] = vaddq_u16(a.val[2], b_const); \
    c.val[3] = vaddq_u16(a.val[3], b_const);

// load c <= a
#define vload_x2(c, a) c = vld1q_u16_x2(a);

// store c <= a
#define vstore_x2(c, a) vst1q_u16_x2(c, a);

// c = a << value
#define vsl_x2(c, a, value)                  \
    c.val[0] = vshlq_n_u16(a.val[0], value); \
    c.val[1] = vshlq_n_u16(a.val[1], value);

// c = a >> value
#define vsr_x2(c, a, value)                  \
    c.val[0] = vshrq_n_u16(a.val[0], value); \
    c.val[1] = vshrq_n_u16(a.val[1], value);

// c = a + b
#define vadd_x2(c, a, b)                      \
    c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
    c.val[1] = vaddq_u16(a.val[1], b.val[1]);

// c = a - b
#define vsub_x2(c, a, b)                      \
    c.val[0] = vsubq_u16(a.val[0], b.val[0]); \
    c.val[1] = vsubq_u16(a.val[1], b.val[1]);

// c = a * value
#define vmuln_x2(c, a, value)                \
    c.val[0] = vmulq_n_u16(a.val[0], value); \
    c.val[1] = vmulq_n_u16(a.val[1], value);

// load c <= a
#define vload_x1(c, a) c = vld1q_u16(a);

// store c <= a
#define vstore_x1(c, a) vst1q_u16(c, a);

// c = a << value
#define vsl_x1(c, a, value) c = vshlq_n_u16(a, value);

// c = a >> value
#define vsr_x1(c, a, value) c = vshrq_n_u16(a, value);

// c = a + b
#define vadd_x1(c, a, b) c = vaddq_u16(a, b);

// c = a - b
#define vsub_x1(c, a, b) c = vsubq_u16(a, b);

// c = a * value
#define vmuln_x1(c, a, value) c = vmulq_n_u16(a, value);

// load c <= a
#define vload_x3(c, a) c = vld1q_u16_x3(a);

// store c <= a
#define vstore_x3(c, a) vst1q_u16_x3(c, a);

// c = a + b
#define vadd_x3(c, a, b)                      \
    c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
    c.val[1] = vaddq_u16(a.val[1], b.val[1]); \
    c.val[2] = vaddq_u16(a.val[2], b.val[2]);

#define vand_x3(c, a, b)               \
    c.val[0] = vandq_u16(a.val[0], b); \
    c.val[1] = vandq_u16(a.val[1], b); \
    c.val[2] = vandq_u16(a.val[2], b);

// Evaluate and Copy, this go to TMP
void karat_neon_evaluate_combine(uint16_t *restrict w, uint16_t *restrict  poly)
{
    uint16_t *c0 = &poly[0*SB3_PAD],
             *c1 = &poly[1*SB3_PAD],
             *c2 = &poly[2*SB3_PAD],
             *c3 = &poly[3*SB3_PAD],
             *w0_mem = &w[0*SB3_PAD],
             *w1_mem = &w[1*SB3_PAD],
             *w2_mem = &w[2*SB3_PAD],
             *w3_mem = &w[3*SB3_PAD],
             *w4_mem = &w[4*SB3_PAD],
             *w5_mem = &w[5*SB3_PAD],
             *w6_mem = &w[6*SB3_PAD],
             *w7_mem = &w[7*SB3_PAD],
             *w8_mem = &w[8*SB3_PAD];

    uint16x8x2_t r0, r1, r2, r3, tmp1, tmp2, tmp3;
    for (uint16_t addr = 0; addr < SB3_PAD; addr += 16)
    {
        vload_x2(r0, &c0[addr]);
        vload_x2(r1, &c1[addr]);
        vload_x2(r2, &c2[addr]);
        vload_x2(r3, &c3[addr]);

        vstore_x2(&w0_mem[addr], r0);
        vstore_x2(&w2_mem[addr], r1);
        vstore_x2(&w6_mem[addr], r2);
        vstore_x2(&w8_mem[addr], r3);

        vadd_x2(tmp1, r0, r1);
        vstore_x2(&w1_mem[addr], tmp1);

        vadd_x2(tmp2, r2, r3);
        vstore_x2(&w7_mem[addr], tmp2);

        vadd_x2(tmp1, r0, r2);
        vstore_x2(&w3_mem[addr], tmp1);

        vadd_x2(tmp2, r1, r3);
        vstore_x2(&w5_mem[addr], tmp2);

        vadd_x2(tmp3, tmp1, tmp2);
        vstore_x2(&w4_mem[addr], tmp3);
    }
}

// Combine Karatsuba Interpolation
void karat_neon_interpolate_combine(uint16_t *restrict poly, uint16_t *restrict w)
{
    uint16x8x2_t r0, r1, r2, r3, r4, r5, r6, r7, // 8x2 = 16
                 r8, sum0, sum1, sum2,   // 4x2 = 8
                 w20, w21, tmp; // 3x2 = 6
    uint16_t *w0_mem = &w[0*SB3_RES_PAD],
             *w1_mem = &w[1*SB3_RES_PAD],
             *w2_mem = &w[2*SB3_RES_PAD],
             *w3_mem = &w[3*SB3_RES_PAD],
             *w4_mem = &w[4*SB3_RES_PAD],
             *w5_mem = &w[5*SB3_RES_PAD],
             *w6_mem = &w[6*SB3_RES_PAD],
             *w7_mem = &w[7*SB3_RES_PAD],
             *w8_mem = &w[8*SB3_RES_PAD];
    for (uint16_t addr = 0; addr < SB3_RES_PAD; addr += 16)
    {
        vload_x2(r0, &w0_mem[addr]); // a
        vload_x2(r1, &w1_mem[addr]); // b
        vload_x2(r2, &w2_mem[addr]); // c
        vload_x2(r3, &w3_mem[addr]); // d
        vload_x2(r4, &w4_mem[addr]); // e
        vload_x2(r5, &w5_mem[addr]); // f
        vload_x2(r6, &w6_mem[addr]); // g
        vload_x2(r7, &w7_mem[addr]); // h
        vload_x2(r8, &w8_mem[addr]); // i

        // r0 + r2
        vadd_x2(sum0, r0, r2);
        // C1 = r1 - r0 - r2
        vsub_x2(sum0, r1, sum0);

        // r3 + r5
        vadd_x2(sum1, r3, r5);
        // B1 = r4 - r3 - r5
        vsub_x2(sum1, r4, sum1);

        // r6 + r8
        vadd_x2(sum2, r6, r8);
        // A1 = r7 - r6 - r8
        vsub_x2(sum2, r7, sum2);

        // r0 + r6
        vadd_x2(w20, r0, r6);
        // r3 + r2
        vadd_x2(tmp, r3, r2);
        // r3 + r2 - r0 - r6
        vsub_x2(w20, tmp, w20);

        // C1 + A1
        vadd_x2(tmp, sum0, sum2);
        // B1 = B1 - C1 - A1
        vsub_x2(sum1, sum1, tmp);

        // r2 + r8
        vadd_x2(tmp, r2, r8);
        // r5 + r6
        vadd_x2(w21, r5, r6);
        // w21 = r5 + r6 - r2 - r8
        vsub_x2(w21, w21, tmp);

        vload_x2(tmp, &poly[addr + 0*SB3]);
        vadd_x2(r0, tmp, r0);
        vstore_x2(&poly[addr + 0*SB3], r0);

        vload_x2(tmp, &poly[addr + 1*SB3]);
        vadd_x2(sum0, tmp, sum0);
        vstore_x2(&poly[addr + 1*SB3], sum0);

        vload_x2(tmp, &poly[addr + 2*SB3]);
        vadd_x2(w20, tmp, w20);
        vstore_x2(&poly[addr + 2*SB3], w20);

        vload_x2(tmp, &poly[addr + 3*SB3]);
        vadd_x2(sum1, tmp, sum1);
        vstore_x2(&poly[addr + 3*SB3], sum1);

        vload_x2(tmp, &poly[addr + 4*SB3]);
        vadd_x2(w21, tmp, w21);
        vstore_x2(&poly[addr + 4*SB3], w21);

        vload_x2(tmp, &poly[addr + 5*SB3]);
        vadd_x2(sum2, tmp, sum2);
        vstore_x2(&poly[addr + 5*SB3], sum2);

        vload_x2(tmp, &poly[addr + 6*SB3]);
        vadd_x2(r8, tmp, r8);
        vstore_x2(&poly[addr + 6*SB3], r8);
    }
}

// Ultilize all 32 SIMD registers, Copy
void tc4_evaluate_neon_SB1(uint16_t *restrict w[7], uint16_t *restrict poly)
{
    uint16_t *c0 = poly,
             *c1 = &poly[1*SB1],
             *c2 = &poly[2*SB1],
             *c3 = &poly[3*SB1],
             *w0_mem = w[0],
             *w6_mem = w[6],
             *w1_mem = w[1],
             *w2_mem = w[2],
             *w3_mem = w[3],
             *w4_mem = w[4],
             *w5_mem = w[5];
    uint16x8x2_t r0, r1, r2, r3, tmp0, tmp1, tmp2, tmp3;
    for (uint16_t addr = 0, addr15 = 0; addr15 < SB1; addr += 16, addr15 += 15)
    {
        vload_x2(r0, &c0[addr15]);
        vload_x2(r1, &c1[addr15]);
        vload_x2(r2, &c2[addr15]);
        vload_x2(r3, &c3[addr15]);
        vstore_x2(&w0_mem[addr], r0);
        vstore_x2(&w6_mem[addr], r3);

        vadd_x2(tmp0, r0, r2);
        vadd_x2(tmp1, r1, r3);

        vadd_x2(tmp2, tmp0, tmp1);
        vsub_x2(tmp3, tmp0, tmp1);
        vstore_x2(&w1_mem[addr], tmp2);
        vstore_x2(&w2_mem[addr], tmp3);

        vsl_x2(tmp2, r0, 2);
        vadd_x2(tmp2, tmp2, r2);
        vsl_x2(tmp2, tmp2, 1);
        vsl_x2(tmp3, r1, 2);
        vadd_x2(tmp3, tmp3, r3);

        vadd_x2(tmp0, tmp2, tmp3);
        vsub_x2(tmp1, tmp2, tmp3);
        vstore_x2(&w3_mem[addr], tmp0);
        vstore_x2(&w4_mem[addr], tmp1);

        vsl_x2(tmp3, r3, 3);
        vsl_x2(tmp2, r2, 2);
        vsl_x2(tmp1, r1, 1);
        vadd_x2(tmp0, tmp3, tmp2);
        vadd_x2(tmp0, tmp0, tmp1);
        vadd_x2(tmp0, tmp0, r0);
        vstore_x2(&w5_mem[addr], tmp0);
    }
}

// void tc_interpolate_neon_sb1(uint16_t poly[512], uint16_t w[7][128])
void tc4_interpolate_neon_SB1(uint16_t *restrict poly, uint16_t *restrict w[7])
{
    uint16x8_t r0, r1, r2, r3, r4, r5, r6, tmp;
    uint16_t *w0_mem = w[0],
             *w1_mem = w[1],
             *w2_mem = w[2],
             *w3_mem = w[3],
             *w4_mem = w[4],
             *w5_mem = w[5],
             *w6_mem = w[6];
    for (uint16_t addr = 0; addr < SB1_RES; addr += 8)
    {
        vload_x1(r0, &w0_mem[addr]);
        vload_x1(r1, &w1_mem[addr]);
        vload_x1(r2, &w2_mem[addr]);
        vload_x1(r3, &w3_mem[addr]);
        vload_x1(r4, &w4_mem[addr]);
        vload_x1(r5, &w5_mem[addr]);
        vload_x1(r6, &w6_mem[addr]);

        vadd_x1(r5, r5, r3);
        vadd_x1(r4, r4, r3);
        vsr_x1(r4, r4, 1);
        vadd_x1(r2, r2, r1);
        vsr_x1(r2, r2, 1);
        vsub_x1(r3, r3, r4);
        vsub_x1(r1, r1, r2);
        vsl_x1(tmp, r2, 6);
        vadd_x1(tmp, tmp, r2);
        vsub_x1(r5, r5, tmp);
        vsub_x1(r2, r2, r6);
        vsub_x1(r2, r2, r0);
        vmuln_x1(tmp, r2, 45);
        vadd_x1(r5, tmp, r5);
        vsub_x1(r4, r4, r6);
        vsr_x1(r4, r4, 2);
        vsr_x1(r3, r3, 1);
        vsl_x1(tmp, r3, 2);
        vsub_x1(r5, r5, tmp);
        vsub_x1(r3, r3, r1);
        vmuln_x1(r3, r3, inv3);
        vsl_x1(tmp, r0, 4);
        vsub_x1(r4, r4, tmp);
        vsl_x1(tmp, r2, 2);
        vsub_x1(r4, r4, tmp);
        vmuln_x1(r4, r4, inv3);

        vadd_x1(r2, r2, r4);
        vsr_x1(r5, r5, 1);
        vmuln_x1(r5, r5, inv15);
        vsub_x1(r1, r1, r5);
        vsub_x1(r1, r1, r3);
        vmuln_x1(r1, r1, inv3);

        vsl_x1(tmp, r1, 2);
        vadd_x1(tmp, tmp, r1);
        vadd_x1(r3, r3, tmp);
        vsub_x1(r5, r5, r1);

        vload_x1(tmp, &poly[addr]);
        vadd_x1(r0, tmp, r0);
        vstore_x1(&poly[addr], r0);

        vload_x1(tmp, &poly[SB1 + addr]);
        vsub_x1(r1, tmp, r1);
        vstore_x1(&poly[SB1 + addr], r1);

        vload_x1(tmp, &poly[2*SB1 + addr]);
        vadd_x1(r2, tmp, r2);
        vstore_x1(&poly[2*SB1 + addr], r2);

        vload_x1(tmp, &poly[3*SB1 + addr]);
        vadd_x1(r3, tmp, r3);
        vstore_x1(&poly[3*SB1 + addr], r3);

        vload_x1(tmp, &poly[4*SB1 + addr]);
        vsub_x1(r4, tmp, r4);
        vstore_x1(&poly[4*SB1 + addr], r4);

        vload_x1(tmp, &poly[5*SB1 + addr]);
        vadd_x1(r5, tmp, r5);
        vstore_x1(&poly[5*SB1 + addr], r5);

        vload_x1(tmp, &poly[6*SB1 + addr]);
        vadd_x1(r6, tmp, r6);
        vstore_x1(&poly[6*SB1 + addr], r6);
    }
}

// Evaluate and copy
void tc3_evaluate_neon_SB0(uint16_t *restrict w[5], uint16_t *restrict poly)
{
    uint16_t *w0_mem = w[0],
             *w1_mem = w[1],
             *w2_mem = w[2],
             *w3_mem = w[3],
             *w4_mem = w[4],
             *c0 = &poly[0*SB0],
             *c1 = &poly[1*SB0],
             *c2 = &poly[2*SB0];
    uint16x8x2_t r0, r1, r2, p0, p1, p_1, tmp;
    for (uint16_t addr = 0; addr < SB0; addr+= 16)
    {
        vload_x2(r0, &c0[addr]);
        vload_x2(r1, &c1[addr]);
        vload_x2(r2, &c2[addr]);

        // p0 = r0 + r2
        vadd_x2(p0, r0, r2);
        // p1 = r0 + r2 + r1
        vadd_x2(p1, p0, r1);
        // p(-1) = r0 + r2 - r1
        vsub_x2(p_1, p0, r1);
        // r0
        vstore_x2(&w0_mem[addr], r0);
        // r0 + r2 + r1
        vstore_x2(&w1_mem[addr], p1);
        // r0 + r2 - r1
        vstore_x2(&w2_mem[addr], p_1);
        // r2
        vstore_x2(&w4_mem[addr], r2);

        vadd_x2(tmp, p_1, r2); // r0 + 2r2 -r1
        vsl_x2(tmp, tmp, 1); // (r0 + 2r2 -r1) << 1
        vsub_x2(tmp, tmp, r0); // 4r2 - 2r1 + r0

        vstore_x2(&w3_mem[addr], tmp);
    }
}

// void tc3_interpolate_neon_SB0(uint16_t *restrict poly, uint16_t *w[5])
void tc3_interpolate_neon_SB0(uint16_t *restrict poly, uint16_t *w[5])
{
    uint16_t *w0_mem = w[0],
             *w1_mem = w[1],
             *w2_mem = w[2],
             *w3_mem = w[3],
             *w4_mem = w[4];
    // 28 SIMD registers
    uint16x8x2_t r0, r1, r2, r3, r4, // 5x2 = 10
                 v1, v2, v3,  // 3x2 = 6
                 c1, c2, c3, // 3x2 = 6
                 tmp1, tmp2, tmp3; // 3x2 = 6
    for (uint16_t addr = 0; addr < SB0_RES; addr+= 16)
    {
        vload_x2(r0, &w0_mem[addr]); // 0
        vload_x2(r1, &w1_mem[addr]); // 1
        vload_x2(r2, &w2_mem[addr]); // -1
        vload_x2(r3, &w3_mem[addr]); // -2
        vload_x2(r4, &w4_mem[addr]); // inf

        // v3 = (r3 - r1)*inv3
        vsub_x2(v3, r3, r1);
        vmuln_x2(v3, v3, inv3);

        // v1 = (r1 - r2) >> 1
        vsub_x2(v1, r1, r2);
        vsr_x2(v1, v1, 1);

        // v2 = (r2 - r0)
        vsub_x2(v2, r2, r0);

        // c3 = (v2 - v3)>>1  + (r4 << 1)
        vsub_x2(tmp1, v2, v3);
        vsl_x2(tmp2, r4, 1);
        vsr_x2(tmp1, tmp1, 1);
        vadd_x2(c3, tmp1, tmp2);

        // c2 = v2 + v1 - r4
        vadd_x2(c2, v2, v1);
        vsub_x2(c2, c2, r4);

        // c1 = v1 - c3
        vsub_x2(c1, v1, c3);

        vload_x2(tmp1, &poly[addr + 0*SB0]);
        vadd_x2(r0, tmp1, r0);
        vstore_x2(&poly[addr + 0*SB0], r0);

        vload_x2(tmp2, &poly[addr + 1*SB0]);
        vadd_x2(c1, tmp2, c1);
        vstore_x2(&poly[addr + 1*SB0], c1);

        vload_x2(tmp3, &poly[addr + 2*SB0]);
        vadd_x2(c2, tmp3, c2);
        vstore_x2(&poly[addr + 2*SB0], c2);

        vload_x2(tmp1, &poly[addr + 3*SB0]);
        vadd_x2(c3, tmp1, c3);
        vstore_x2(&poly[addr + 3*SB0], c3);

        vload_x2(tmp2, &poly[addr + 4*SB0]);
        vadd_x2(r4, tmp2, r4);
        vstore_x2(&poly[addr + 4*SB0], r4);
    }
}

void neon_toom_cook_422_combine(uint16_t *restrict polyC, uint16_t *restrict polyA, uint16_t *restrict polyB)
{
    // TC4
    uint16_t *aw[7], *bw[7], *cw[7];
    // TC4-2-2 Combine
    // Total Memory: 16*128 + 32*64 = 4096 16-bit coefficients
    uint16_t tmp_aabb[SB3_PAD*128], tmp_cc[SB3_RES_PAD*64];
    uint16_t *tmp_aa = &tmp_aabb[SB3_PAD*0],
             *tmp_bb = &tmp_aabb[SB3_PAD*64];
    // Done
    uint16x8x4_t zero;

    // TC4
    aw[0] = &tmp_cc[0*SB1_PAD];
    aw[1] = &tmp_cc[1*SB1_PAD];
    aw[2] = &tmp_cc[2*SB1_PAD];
    aw[3] = &tmp_cc[3*SB1_PAD];
    aw[4] = &tmp_cc[4*SB1_PAD];
    aw[5] = &tmp_cc[5*SB1_PAD];
    aw[6] = &tmp_cc[6*SB1_PAD];

    bw[0] = &tmp_cc[7*SB1_PAD];
    bw[1] = &tmp_cc[8*SB1_PAD];
    bw[2] = &tmp_cc[9*SB1_PAD];
    bw[3] = &tmp_cc[10*SB1_PAD];
    bw[4] = &tmp_cc[11*SB1_PAD];
    bw[5] = &tmp_cc[12*SB1_PAD];
    bw[6] = &tmp_cc[13*SB1_PAD];

    cw[0] = &tmp_aabb[0*SB1_RES];
    cw[1] = &tmp_aabb[1*SB1_RES];
    cw[2] = &tmp_aabb[2*SB1_RES];
    cw[3] = &tmp_aabb[3*SB1_RES];
    cw[4] = &tmp_aabb[4*SB1_RES];
    cw[5] = &tmp_aabb[5*SB1_RES];
    cw[6] = &tmp_aabb[6*SB1_RES];
    // DONE TC4

    
    // Evaluate A, No Copy
    // Size: 256 to 64x7
    tc4_evaluate_neon_SB1(aw, polyA);

    // Evaluate B, No Copy
    // Size: 256 to 64x7
    tc4_evaluate_neon_SB1(bw, polyB);

    karat_neon_evaluate_combine(&tmp_aa[0*9*SB3_PAD], aw[0]);
    karat_neon_evaluate_combine(&tmp_aa[1*9*SB3_PAD], aw[1]);
    karat_neon_evaluate_combine(&tmp_aa[2*9*SB3_PAD], aw[2]);
    karat_neon_evaluate_combine(&tmp_aa[3*9*SB3_PAD], aw[3]);
    karat_neon_evaluate_combine(&tmp_aa[4*9*SB3_PAD], aw[4]);
    karat_neon_evaluate_combine(&tmp_aa[5*9*SB3_PAD], aw[5]);
    karat_neon_evaluate_combine(&tmp_aa[6*9*SB3_PAD], aw[6]);

    karat_neon_evaluate_combine(&tmp_bb[0*9*SB3_PAD], bw[0]);
    karat_neon_evaluate_combine(&tmp_bb[1*9*SB3_PAD], bw[1]);
    karat_neon_evaluate_combine(&tmp_bb[2*9*SB3_PAD], bw[2]);
    karat_neon_evaluate_combine(&tmp_bb[3*9*SB3_PAD], bw[3]);
    karat_neon_evaluate_combine(&tmp_bb[4*9*SB3_PAD], bw[4]);
    karat_neon_evaluate_combine(&tmp_bb[5*9*SB3_PAD], bw[5]);
    karat_neon_evaluate_combine(&tmp_bb[6*9*SB3_PAD], bw[6]);

    // Transpose 8x8x16
    half_transpose_8x16(tmp_aa);
    half_transpose_8x16(tmp_bb);
    // Batch multiplication
    schoolbook_half_8x_neon(tmp_cc, tmp_aa, tmp_bb);
    // Transpose 8x8x32
    half_transpose_8x32(tmp_cc);

    vzero(zero, 0);
    for (uint16_t addr = 0; addr < SB1_RES*7; addr+=32)
    {
        vstore(&tmp_aabb[addr], zero);
    }

    karat_neon_interpolate_combine(cw[0], &tmp_cc[0*9*SB3_RES_PAD]);
    karat_neon_interpolate_combine(cw[1], &tmp_cc[1*9*SB3_RES_PAD]);
    karat_neon_interpolate_combine(cw[2], &tmp_cc[2*9*SB3_RES_PAD]);
    karat_neon_interpolate_combine(cw[3], &tmp_cc[3*9*SB3_RES_PAD]);
    karat_neon_interpolate_combine(cw[4], &tmp_cc[4*9*SB3_RES_PAD]);
    karat_neon_interpolate_combine(cw[5], &tmp_cc[5*9*SB3_RES_PAD]);
    karat_neon_interpolate_combine(cw[6], &tmp_cc[6*9*SB3_RES_PAD]);

    // Interpolate C = A*B = CC
    // Size: 128*7 to 128*4 = 512
    tc4_interpolate_neon_SB1(polyC, cw);
}

void poly_neon_reduction(uint16_t *poly, uint16_t *tmp)
{
    uint16x8_t mask; 
    uint16x8x3_t res, tmp1, tmp2;
    mask = vdupq_n_u16(MASK);
    for (uint16_t addr = 0; addr < NTRU_N_PAD; addr += 24)
    {
        vload_x3(tmp2, &tmp[addr]);
        vload_x3(tmp1, &tmp[addr + NTRU_N]);
        vadd_x3(res, tmp1, tmp2);
        vand_x3(res, res, mask);
        vstore_x3(&poly[addr], res);
    }
}

void poly_mul_neon(uint16_t *restrict polyC, uint16_t *restrict polyA, uint16_t *restrict polyB)
{
    uint16x8x4_t zero;
    uint16_t *kaw[5], *kbw[5], *kcw[5];
    uint16_t tmp_ab[SB0 * 5 * 2];
    uint16_t tmp_c[SB0_RES * 5];

    // Better for caching
    kaw[0] = &tmp_ab[0 * SB0];
    kbw[0] = &tmp_ab[1 * SB0];

    kaw[1] = &tmp_ab[2 * SB0];
    kbw[1] = &tmp_ab[3 * SB0];

    kaw[2] = &tmp_ab[4 * SB0];
    kbw[2] = &tmp_ab[5 * SB0];

    kaw[3] = &tmp_ab[6 * SB0];
    kbw[3] = &tmp_ab[7 * SB0];

    kaw[4] = &tmp_ab[8 * SB0];
    kbw[4] = &tmp_ab[9 * SB0];

    kcw[0] = &tmp_c[0 * SB0_RES];
    kcw[1] = &tmp_c[1 * SB0_RES];
    kcw[2] = &tmp_c[2 * SB0_RES];
    kcw[3] = &tmp_c[3 * SB0_RES];
    kcw[4] = &tmp_c[4 * SB0_RES];

    vzero(zero, 0);
    for (uint16_t addr = 0; addr < SB0_RES * 5; addr += 32)
    {
        vstore(&tmp_c[addr], zero);
    }

    // Toom-Cook-3 Evaluate A
    tc3_evaluate_neon_SB0(kaw, polyA);
    // Toom-Cook-3 Evaluate B
    tc3_evaluate_neon_SB0(kbw, polyB);

    // Toom Cook 4-way combine
    neon_toom_cook_422_combine(kcw[0], kaw[0], kbw[0]);

    // Toom Cook 4-way combine
    neon_toom_cook_422_combine(kcw[1], kaw[1], kbw[1]);

    // Toom Cook 4-way combine
    neon_toom_cook_422_combine(kcw[2], kaw[2], kbw[2]);

    // Toom Cook 4-way combine
    neon_toom_cook_422_combine(kcw[3], kaw[3], kbw[3]);

    // Toom Cook 4-way combine
    neon_toom_cook_422_combine(kcw[4], kaw[4], kbw[4]);

    // Karatsuba Interpolate
    // * Re-use tmp_ab
    vzero(zero, 0);
    for (uint16_t addr = 0; addr < SB0_RES * 3; addr += 32)
    {
        vstore(&tmp_ab[addr], zero);
    }
    tc3_interpolate_neon_SB0(tmp_ab, kcw);

    // Ring reduction
    // Reduce from 1440 -> 720
    poly_neon_reduction(polyC, tmp_ab);
}
