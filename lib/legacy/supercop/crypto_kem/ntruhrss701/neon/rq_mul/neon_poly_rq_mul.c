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

#define NTRU_N 701
#define NTRU_N_PAD 704
#define SB0 351          // 351
#define SB0_PAD 352
#define SB1 (SB0 / 3)    // 117
#define SB1_PAD 120
#define SB2 (SB1 / 3)    // 39
#define SB2_PAD 40
#define SB3 (SB2 / 3)    // 13
#define SB3_PAD 16

#define SB3_RES (2 * SB3) // 26
#define SB3_RES_PAD 32
#define SB2_RES (2 * SB2) // 78
#define SB2_RES_PAD 80
#define SB1_RES (2 * SB1) // 234
#define SB1_RES_PAD 240
#define SB0_RES (2 * SB0) // 702
#define SB0_RES_PAD 704

#define MASK (NTRU_Q - 1)

#define inv3 43691

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

// c = a + b
#define vadd(c, a, b)                         \
    c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
    c.val[1] = vaddq_u16(a.val[1], b.val[1]); \
    c.val[2] = vaddq_u16(a.val[2], b.val[2]); \
    c.val[3] = vaddq_u16(a.val[3], b.val[3]);

// c = a & b
#define vand(c, a, b)                  \
    c.val[0] = vandq_u16(a.val[0], b); \
    c.val[1] = vandq_u16(a.val[1], b); \
    c.val[2] = vandq_u16(a.val[2], b); \
    c.val[3] = vandq_u16(a.val[3], b);

// c = a - b
#define vsub(c, a, b)                         \
    c.val[0] = vsubq_u16(a.val[0], b.val[0]); \
    c.val[1] = vsubq_u16(a.val[1], b.val[1]); \
    c.val[2] = vsubq_u16(a.val[2], b.val[2]); \
    c.val[3] = vsubq_u16(a.val[3], b.val[3]);

// c = value
#define vzero(c, value)            \
    c.val[0] = vmovq_n_u16(value); \
    c.val[1] = vmovq_n_u16(value); \
    c.val[2] = vmovq_n_u16(value); \
    c.val[3] = vmovq_n_u16(value);

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
#define vmuln_x2(c, a, value)                 \
    c.val[0] = vmulq_n_u16(a.val[0], value); \
    c.val[1] = vmulq_n_u16(a.val[1], value);

// load c <= a
#define vload_x1(c, a) c = vld1q_u16(a);

// store c <= a
#define vstore_x1(c, a) vst1q_u16(c, a);

// c = a << value
#define vsl_x1(c, a, value) c = vshlq_n_u16(a, value);

// c = a + b
#define vadd_x1(c, a, b) c = vaddq_u16(a, b);

// c = a - b
#define vsub_x1(c, a, b) c = vsubq_u16(a, b);


// Evaluate and copy
void tc3_evaluate_neon_SB1(uint16_t *restrict w[5], uint16_t *restrict poly)
{
    uint16_t *w0_mem = w[0],
             *w1_mem = w[1],
             *w2_mem = w[2],
             *w3_mem = w[3],
             *w4_mem = w[4],
             *c0 = &poly[0*SB1],
             *c1 = &poly[1*SB1],
             *c2 = &poly[2*SB1];
    uint16x8_t r0, r1, r2, p0, p1, p_1, tmp;
    for (uint16_t addr = 0; addr < SB1_PAD; addr+= 8)
    {
        vload_x1(r0, &c0[addr]);
        vload_x1(r1, &c1[addr]);
        vload_x1(r2, &c2[addr]);

        // p0 = r0 + r2
        vadd_x1(p0, r0, r2);
        // p1 = r0 + r2 + r1
        vadd_x1(p1, p0, r1);
        // p(-1) = r0 + r2 - r1
        vsub_x1(p_1, p0, r1);
        // r0
        vstore_x1(&w0_mem[addr], r0);
        // r0 + r2 + r1
        vstore_x1(&w1_mem[addr], p1);
        // r0 + r2 - r1
        vstore_x1(&w2_mem[addr], p_1);

        vadd_x1(tmp, p_1, r2); // r0 + 2r2 -r1
        vsl_x1(tmp, tmp, 1); // (r0 + 2r2 -r1) << 1
        vsub_x1(tmp, tmp, r0); // 4r2 - 2r1 + r0
        vstore_x1(&w3_mem[addr], tmp);

        // r2
        vstore_x1(&w4_mem[addr], r2);

    }
}

void tc3_evaluate_neon_combine(uint16_t *restrict w, uint16_t *restrict poly)
{
    uint16_t *c0 = &poly[0*SB3],
             *c1 = &poly[1*SB3],
             *c2 = &poly[2*SB3],
             *c3 = &poly[3*SB3],
             *c4 = &poly[4*SB3],
             *c5 = &poly[5*SB3],
             *c6 = &poly[6*SB3],
             *c7 = &poly[7*SB3],
             *c8 = &poly[8*SB3],
             *w00 = &w[ 0*SB3_PAD],
             *w01 = &w[ 1*SB3_PAD],
             *w02 = &w[ 2*SB3_PAD],
             *w03 = &w[ 3*SB3_PAD],
             *w04 = &w[ 4*SB3_PAD],
             *w05 = &w[ 5*SB3_PAD],
             *w06 = &w[ 6*SB3_PAD],
             *w07 = &w[ 7*SB3_PAD],
             *w08 = &w[ 8*SB3_PAD],
             *w09 = &w[ 9*SB3_PAD],
             *w10 = &w[10*SB3_PAD],
             *w11 = &w[11*SB3_PAD],
             *w12 = &w[12*SB3_PAD],
             *w13 = &w[13*SB3_PAD],
             *w14 = &w[14*SB3_PAD],
             *w15 = &w[15*SB3_PAD],
             *w16 = &w[16*SB3_PAD],
             *w17 = &w[17*SB3_PAD],
             *w18 = &w[18*SB3_PAD],
             *w19 = &w[19*SB3_PAD],
             *w20 = &w[20*SB3_PAD],
             *w21 = &w[21*SB3_PAD],
             *w22 = &w[22*SB3_PAD],
             *w23 = &w[23*SB3_PAD],
             *w24 = &w[24*SB3_PAD];
    // Utilize 22 SIMD registers
    uint16x8_t a0, a1, a2, a3, a4, a5, a6, a7, a8, //9
               tmp0, tmp1, tmp2, tmp3, // 4
               s0, s1, s2, // 3
               e0, e1, e2, // 3
               t0, t1, t2; // 3
    for (uint16_t addr = 0; addr < SB3; addr += 8)
    {
        vload_x1(a0, &c0[addr]);
        vload_x1(a1, &c1[addr]);
        vload_x1(a2, &c2[addr]);
        vload_x1(a3, &c3[addr]);
        vload_x1(a4, &c4[addr]);
        vload_x1(a5, &c5[addr]);
        vload_x1(a6, &c6[addr]);
        vload_x1(a7, &c7[addr]);
        vload_x1(a8, &c8[addr]);

        vadd_x1(tmp0, a2, a0);
        vadd_x1(tmp1, tmp0, a1);
        vsub_x1(tmp2, tmp0, a1);
        vadd_x1(tmp3, tmp2, a2);
        vsl_x1(tmp3, tmp3, 1);
        vsub_x1(tmp3, tmp3, a0);

        vstore_x1(&w00[addr], a0);
        vstore_x1(&w01[addr], tmp1);
        vstore_x1(&w02[addr], tmp2);
        vstore_x1(&w03[addr], tmp3);
        vstore_x1(&w04[addr], a2);

        vadd_x1(tmp0, a8, a6);
        vadd_x1(tmp1, tmp0, a7);
        vsub_x1(tmp2, tmp0, a7);
        vadd_x1(tmp3, tmp2, a8);
        vsl_x1(tmp3, tmp3, 1);
        vsub_x1(tmp3, tmp3, a6);

        vstore_x1(&w20[addr], a6);
        vstore_x1(&w21[addr], tmp1);
        vstore_x1(&w22[addr], tmp2);
        vstore_x1(&w23[addr], tmp3);
        vstore_x1(&w24[addr], a8);

        vadd_x1(s0, a0, a6);
        vadd_x1(s1, a1, a7);
        vadd_x1(s2, a2, a8);

        vadd_x1(e0, s0, a3);
        vadd_x1(e1, s1, a4);
        vadd_x1(e2, s2, a5);

        vadd_x1(tmp0, e2, e0);
        vadd_x1(tmp1, tmp0, e1);
        vsub_x1(tmp2, tmp0, e1);
        vadd_x1(tmp3, tmp2, e2);
        vsl_x1(tmp3, tmp3, 1);
        vsub_x1(tmp3, tmp3, e0);

        vstore_x1(&w05[addr], e0);
        vstore_x1(&w06[addr], tmp1);
        vstore_x1(&w07[addr], tmp2);
        vstore_x1(&w08[addr], tmp3);
        vstore_x1(&w09[addr], e2);

        vsub_x1(e0, s0, a3);
        vsub_x1(e1, s1, a4);
        vsub_x1(e2, s2, a5);

        vadd_x1(tmp0, e2, e0);
        vadd_x1(tmp1, tmp0, e1);
        vsub_x1(tmp2, tmp0, e1);
        vadd_x1(tmp3, tmp2, e2);
        vsl_x1(tmp3, tmp3, 1);
        vsub_x1(tmp3, tmp3, e0);

        vstore_x1(&w10[addr], e0);
        vstore_x1(&w11[addr], tmp1);
        vstore_x1(&w12[addr], tmp2);
        vstore_x1(&w13[addr], tmp3);
        vstore_x1(&w14[addr], e2);

        vsl_x1(t0, a6, 1);
        vsl_x1(t1, a7, 1);
        vsl_x1(t2, a8, 1);
        vsub_x1(t0, t0, a3);
        vsub_x1(t1, t1, a4);
        vsub_x1(t2, t2, a5);
        vsl_x1(t0, t0, 1);
        vsl_x1(t1, t1, 1);
        vsl_x1(t2, t2, 1);
        vadd_x1(t0, t0, a0);
        vadd_x1(t1, t1, a1);
        vadd_x1(t2, t2, a2);

        vadd_x1(tmp0, t2, t0);
        vadd_x1(tmp1, tmp0, t1);
        vsub_x1(tmp2, tmp0, t1);
        vadd_x1(tmp3, tmp2, t2);
        vsl_x1(tmp3, tmp3, 1);
        vsub_x1(tmp3, tmp3, t0);

        vstore_x1(&w15[addr], t0);
        vstore_x1(&w16[addr], tmp1);
        vstore_x1(&w17[addr], tmp2);
        vstore_x1(&w18[addr], tmp3);
        vstore_x1(&w19[addr], t2);
    }
}

void tc3_interpolate_neon_SB3(uint16_t *restrict poly, uint16_t *restrict w)
{
    uint16_t *w0_mem = &w[0*SB3_RES_PAD],
             *w1_mem = &w[1*SB3_RES_PAD],
             *w2_mem = &w[2*SB3_RES_PAD],
             *w3_mem = &w[3*SB3_RES_PAD],
             *w4_mem = &w[4*SB3_RES_PAD];
    // 28 SIMD registers
    uint16x8x2_t r0, r1, r2, r3, r4, // 5x2 = 10
                 v1, v2, v3,  // 3x2 = 6
                 c1, c2, c3, // 3x2 = 6
                 tmp1, tmp2, tmp3; // 3x2 = 6
    for (uint16_t addr = 0; addr < SB3_RES_PAD; addr += 16)
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
        vsr_x2(tmp1, tmp1, 1);
        vsl_x2(tmp2, r4, 1);
        vadd_x2(c3, tmp1, tmp2);

        // c2 = v2 + v1 - r4
        vadd_x2(c2, v2, v1);
        vsub_x2(c2, c2, r4);

        // c1 = v1 - c3
        vsub_x2(c1, v1, c3);

        vload_x2(tmp1, &poly[addr + 0*SB3]);
        vadd_x2(r0, tmp1, r0);
        vstore_x2(&poly[addr + 0*SB3], r0);

        vload_x2(tmp2, &poly[addr + 1*SB3]);
        vadd_x2(c1, tmp2, c1);
        vstore_x2(&poly[addr + 1*SB3], c1);

        vload_x2(tmp3, &poly[addr + 2*SB3]);
        vadd_x2(c2, tmp3, c2);
        vstore_x2(&poly[addr + 2*SB3], c2);

        vload_x2(tmp1, &poly[addr + 3*SB3]);
        vadd_x2(c3, tmp1, c3);
        vstore_x2(&poly[addr + 3*SB3], c3);

        vload_x2(tmp2, &poly[addr + 4*SB3]);
        vadd_x2(r4, tmp2, r4);
        vstore_x2(&poly[addr + 4*SB3], r4);
    }
}

void tc3_interpolate_neon_SB2(uint16_t *restrict poly, uint16_t *restrict w)
{
    uint16_t *w0_mem = &w[0*SB2_RES_PAD],
             *w1_mem = &w[1*SB2_RES_PAD],
             *w2_mem = &w[2*SB2_RES_PAD],
             *w3_mem = &w[3*SB2_RES_PAD],
             *w4_mem = &w[4*SB2_RES_PAD];
    // 28 SIMD registers
    uint16x8x2_t r0, r1, r2, r3, r4, // 5x2 = 10
                 v1, v2, v3,  // 3x2 = 6
                 c1, c2, c3, // 3x2 = 6
                 tmp1, tmp2, tmp3; // 3x2 = 6
    for (uint16_t addr = 0; addr < SB2_RES_PAD; addr += 16)
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
        vsr_x2(tmp1, tmp1, 1);
        vsl_x2(tmp2, r4, 1);
        vadd_x2(c3, tmp1, tmp2);

        // c2 = v2 + v1 - r4
        vadd_x2(c2, v2, v1);
        vsub_x2(c2, c2, r4);

        // c1 = v1 - c3
        vsub_x2(c1, v1, c3);

        vload_x2(tmp1, &poly[addr + 0*SB2]);
        vadd_x2(r0, tmp1, r0);
        vstore_x2(&poly[addr + 0*SB2], r0);

        vload_x2(tmp2, &poly[addr + 1*SB2]);
        vadd_x2(c1, tmp2, c1);
        vstore_x2(&poly[addr + 1*SB2], c1);

        vload_x2(tmp3, &poly[addr + 2*SB2]);
        vadd_x2(c2, tmp3, c2);
        vstore_x2(&poly[addr + 2*SB2], c2);

        vload_x2(tmp1, &poly[addr + 3*SB2]);
        vadd_x2(c3, tmp1, c3);
        vstore_x2(&poly[addr + 3*SB2], c3);

        vload_x2(tmp2, &poly[addr + 4*SB2]);
        vadd_x2(r4, tmp2, r4);
        vstore_x2(&poly[addr + 4*SB2], r4);
    }
}

void tc3_interpolate_neon_SB1(uint16_t *restrict poly, uint16_t *restrict w)
{
    uint16_t *w0_mem = &w[0*SB1_RES_PAD],
             *w1_mem = &w[1*SB1_RES_PAD],
             *w2_mem = &w[2*SB1_RES_PAD],
             *w3_mem = &w[3*SB1_RES_PAD],
             *w4_mem = &w[4*SB1_RES_PAD];
    // 28 SIMD registers
    uint16x8x2_t r0, r1, r2, r3, r4, // 5x2 = 10
                 v1, v2, v3,  // 3x2 = 6
                 c1, c2, c3, // 3x2 = 6
                 tmp1, tmp2, tmp3; // 3x2 = 6
    for (uint16_t addr = 0; addr < SB1_RES_PAD; addr+= 16)
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
        vsr_x2(tmp1, tmp1, 1);
        vsl_x2(tmp2, r4, 1);
        vadd_x2(c3, tmp1, tmp2);

        // c2 = v2 + v1 - r4
        vadd_x2(c2, v2, v1);
        vsub_x2(c2, c2, r4);

        // c1 = v1 - c3
        vsub_x2(c1, v1, c3);

        vload_x2(tmp1, &poly[addr + 0*SB1]);
        vadd_x2(r0, tmp1, r0);
        vstore_x2(&poly[addr + 0*SB1], r0);

        vload_x2(tmp2, &poly[addr + 1*SB1]);
        vadd_x2(c1, tmp2, c1);
        vstore_x2(&poly[addr + 1*SB1], c1);

        vload_x2(tmp3, &poly[addr + 2*SB1]);
        vadd_x2(c2, tmp3, c2);
        vstore_x2(&poly[addr + 2*SB1], c2);

        vload_x2(tmp1, &poly[addr + 3*SB1]);
        vadd_x2(c3, tmp1, c3);
        vstore_x2(&poly[addr + 3*SB1], c3);

        vload_x2(tmp2, &poly[addr + 4*SB1]);
        vadd_x2(r4, tmp2, r4);
        vstore_x2(&poly[addr + 4*SB1], r4);
    }
}


void neon_toom_cook_333_combine(uint16_t *restrict polyC, uint16_t *restrict polyA, uint16_t *restrict polyB)
{
    // TC3-3-3 Combine
    uint16_t *aw[5], *bw[5];

    // Total memory: 16*256 + 32*128 = 8192 16-bit coefficients
    uint16_t tmp_aabb[SB3_PAD*256], tmp_cc[SB3_RES_PAD*128];
    uint16_t *tmp_aa = &tmp_aabb[SB3_PAD*0],
             *tmp_bb = &tmp_aabb[SB3_PAD*128];
    // Done
    uint16x8x4_t zero;


    // TC3
    aw[0] = &tmp_cc[0*SB1_PAD];
    aw[1] = &tmp_cc[1*SB1_PAD];
    aw[2] = &tmp_cc[2*SB1_PAD];
    aw[3] = &tmp_cc[3*SB1_PAD];
    aw[4] = &tmp_cc[4*SB1_PAD];

    bw[0] = &tmp_cc[5*SB1_PAD];
    bw[1] = &tmp_cc[6*SB1_PAD];
    bw[2] = &tmp_cc[7*SB1_PAD];
    bw[3] = &tmp_cc[8*SB1_PAD];
    bw[4] = &tmp_cc[9*SB1_PAD];
    // DONE TC3


    // Evaluate A, Copy
    // Size: 432 to 144x5
    tc3_evaluate_neon_SB1(aw, polyA);

    // Evaluate B, Copy
    // Size: 432 to 144x5
    tc3_evaluate_neon_SB1(bw, polyB);

    tc3_evaluate_neon_combine(&tmp_aa[0*25*SB3_PAD], aw[0]);
    tc3_evaluate_neon_combine(&tmp_aa[1*25*SB3_PAD], aw[1]);
    tc3_evaluate_neon_combine(&tmp_aa[2*25*SB3_PAD], aw[2]);
    tc3_evaluate_neon_combine(&tmp_aa[3*25*SB3_PAD], aw[3]);
    tc3_evaluate_neon_combine(&tmp_aa[4*25*SB3_PAD], aw[4]);

    tc3_evaluate_neon_combine(&tmp_bb[0*25*SB3_PAD], bw[0]);
    tc3_evaluate_neon_combine(&tmp_bb[1*25*SB3_PAD], bw[1]);
    tc3_evaluate_neon_combine(&tmp_bb[2*25*SB3_PAD], bw[2]);
    tc3_evaluate_neon_combine(&tmp_bb[3*25*SB3_PAD], bw[3]);
    tc3_evaluate_neon_combine(&tmp_bb[4*25*SB3_PAD], bw[4]);

    // Transpose 8x8x16
    half_transpose_8x16(tmp_aa);
    half_transpose_8x16(tmp_bb);
    // Batch multiplication
    schoolbook_half_8x_neon(tmp_cc, tmp_aa, tmp_bb);
    // Transpose 8x8x32
    half_transpose_8x32(tmp_cc);

    vzero(zero, 0);
    for (uint16_t addr = 0; addr < SB2_RES_PAD*25; addr+=32)
    {
        vstore(&tmp_aabb[addr], zero);
    }

    tc3_interpolate_neon_SB3(&tmp_aabb[0*SB2_RES_PAD], &tmp_cc[0*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[1*SB2_RES_PAD], &tmp_cc[1*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[2*SB2_RES_PAD], &tmp_cc[2*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[3*SB2_RES_PAD], &tmp_cc[3*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[4*SB2_RES_PAD], &tmp_cc[4*5*SB3_RES_PAD]);

    tc3_interpolate_neon_SB3(&tmp_aabb[5*SB2_RES_PAD], &tmp_cc[5*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[6*SB2_RES_PAD], &tmp_cc[6*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[7*SB2_RES_PAD], &tmp_cc[7*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[8*SB2_RES_PAD], &tmp_cc[8*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[9*SB2_RES_PAD], &tmp_cc[9*5*SB3_RES_PAD]);

    tc3_interpolate_neon_SB3(&tmp_aabb[10*SB2_RES_PAD], &tmp_cc[10*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[11*SB2_RES_PAD], &tmp_cc[11*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[12*SB2_RES_PAD], &tmp_cc[12*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[13*SB2_RES_PAD], &tmp_cc[13*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[14*SB2_RES_PAD], &tmp_cc[14*5*SB3_RES_PAD]);

    tc3_interpolate_neon_SB3(&tmp_aabb[15*SB2_RES_PAD], &tmp_cc[15*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[16*SB2_RES_PAD], &tmp_cc[16*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[17*SB2_RES_PAD], &tmp_cc[17*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[18*SB2_RES_PAD], &tmp_cc[18*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[19*SB2_RES_PAD], &tmp_cc[19*5*SB3_RES_PAD]);

    tc3_interpolate_neon_SB3(&tmp_aabb[20*SB2_RES_PAD], &tmp_cc[20*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[21*SB2_RES_PAD], &tmp_cc[21*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[22*SB2_RES_PAD], &tmp_cc[22*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[23*SB2_RES_PAD], &tmp_cc[23*5*SB3_RES_PAD]);
    tc3_interpolate_neon_SB3(&tmp_aabb[24*SB2_RES_PAD], &tmp_cc[24*5*SB3_RES_PAD]);

    vzero(zero, 0);
    for (uint16_t addr = 0; addr < SB1_RES_PAD*5; addr+=32)
    {
        vstore(&tmp_cc[addr], zero);
    }

    tc3_interpolate_neon_SB2(&tmp_cc[0*SB1_RES_PAD], &tmp_aabb[0*5*SB2_RES_PAD]);
    tc3_interpolate_neon_SB2(&tmp_cc[1*SB1_RES_PAD], &tmp_aabb[1*5*SB2_RES_PAD]);
    tc3_interpolate_neon_SB2(&tmp_cc[2*SB1_RES_PAD], &tmp_aabb[2*5*SB2_RES_PAD]);
    tc3_interpolate_neon_SB2(&tmp_cc[3*SB1_RES_PAD], &tmp_aabb[3*5*SB2_RES_PAD]);
    tc3_interpolate_neon_SB2(&tmp_cc[4*SB1_RES_PAD], &tmp_aabb[4*5*SB2_RES_PAD]);

    // Interpolate C = A*B = CC
    // Size: 234*5 to 234*3 = 702
    tc3_interpolate_neon_SB1(polyC, tmp_cc);
}

void poly_neon_reduction(uint16_t *poly, uint16_t *tmp)
{
    uint16x8x4_t res, tmp1, tmp2;
    uint16x8_t mask;
    mask = vdupq_n_u16(MASK);
    for (uint16_t addr = 0; addr < NTRU_N_PAD; addr += 32)
    {
        vload(tmp2, &tmp[addr]);
        vload(tmp1, &tmp[addr + NTRU_N]);
        vadd(res, tmp1, tmp2);
        vand(res, res, mask);
        vstore(&poly[addr], res);
    }
}

void karat_neon_evaluate_SB0(uint16_t *restrict w[3], uint16_t *restrict  poly)
{
    uint16_t *c0 = poly,
             *c1 = &poly[SB0],
             *w0_mem = w[0],
             *w1_mem = w[1],
             *w2_mem = w[2];
    uint16x8x4_t r0, r1, r2;
    for (uint16_t addr = 0; addr < SB0_PAD; addr +=32)
    {
        vload(r0, &c0[addr]);
        vload(r1, &c1[addr]);

        vstore(&w0_mem[addr], r0);
        vstore(&w2_mem[addr], r1);

        vadd(r2, r0, r1);
        vstore(&w1_mem[addr], r2);

    }
}

// Interpolate
void karat_neon_interpolate_SB0(uint16_t *restrict poly, uint16_t *restrict w[3])
{
    uint16x8x4_t r0, r1, r2, tmp0, tmp1, tmp2, r0r2;
    uint16_t *w0_mem = w[0],
             *w1_mem = w[1],
             *w2_mem = w[2];
    for (uint16_t i = 0; i < SB0_RES; i+=32)
    {
        vload(r0, &w0_mem[i]);
        vload(r1, &w1_mem[i]);
        vload(r2, &w2_mem[i]);

        vload(tmp0, &poly[0*SB0 + i]);
        vload(tmp1, &poly[1*SB0 + i]);
        vload(tmp2, &poly[2*SB0 + i]);

        vadd(tmp1, tmp1, r1);
        vadd(r0r2, r0, r2);
        vsub(tmp1, tmp1, r0r2);

        vadd(tmp0, r0, tmp0);
        vadd(tmp2, r2, tmp2);

        vstore(&poly[0*SB0 + i], tmp0);
        vstore(&poly[1*SB0 + i], tmp1);
        vstore(&poly[2*SB0 + i], tmp2);

    }
}

void poly_mul_neon(uint16_t *restrict polyC, 
                   uint16_t *restrict polyA, 
                   uint16_t *restrict polyB)
{
    uint16x8x4_t zero;
    uint16_t *kaw[3], *kbw[3], *kcw[3];
    uint16_t tmp_ab[SB0_PAD * 3 * 2 + 8]; // Avoid reading out of bound by 2
    uint16_t tmp_c[SB0_RES_PAD * 3];

    // Better for caching
    kaw[0] = &tmp_ab[0 * SB0_PAD];
    kaw[1] = &tmp_ab[1 * SB0_PAD];
    kaw[2] = &tmp_ab[2 * SB0_PAD];

    kbw[0] = &tmp_ab[3 * SB0_PAD];
    kbw[1] = &tmp_ab[4 * SB0_PAD];
    kbw[2] = &tmp_ab[5 * SB0_PAD];

    kcw[0] = &tmp_c[0 * SB0_RES_PAD];
    kcw[1] = &tmp_c[1 * SB0_RES_PAD];
    kcw[2] = &tmp_c[2 * SB0_RES_PAD];

    vzero(zero, 0);
    for (uint16_t addr = 0; addr < SB0_RES_PAD * 3; addr += 32)
    {
        vstore(&tmp_c[addr], zero);
    }

    // Toom-Cook-3 Evaluate A
    karat_neon_evaluate_SB0(kaw, polyA);
    // Toom-Cook-3 Evaluate B
    karat_neon_evaluate_SB0(kbw, polyB);

    // Toom Cook 333-way combine
    neon_toom_cook_333_combine(kcw[0], kaw[0], kbw[0]);

    // Toom Cook 333-way combine
    neon_toom_cook_333_combine(kcw[1], kaw[1], kbw[1]);

    // Toom Cook 333-way combine
    neon_toom_cook_333_combine(kcw[2], kaw[2], kbw[2]);

    // Karatsuba Interpolate
    // * Re-use tmp_ab
    vzero(zero, 0);
    for (uint16_t addr = 0; addr < SB0_RES * 3; addr += 32)
    {
        vstore(&tmp_ab[addr], zero);
    }
    karat_neon_interpolate_SB0(tmp_ab, kcw);

    // Ring reduction
    // Reduce from 1728 -> 864
    poly_neon_reduction(polyC, tmp_ab);
}
