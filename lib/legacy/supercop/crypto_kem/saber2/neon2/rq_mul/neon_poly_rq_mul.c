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

#include "../SABER_params.h"
#include "../poly.h"
#include "neon_batch_multiplication.c"
#include "neon_matrix_transpose.c"

#define SB0 (SABER_N) // 256
#define SB1 (SB0 / 4) // 64
#define SB2 (SB1 / 2) // 32
#define SB3 (SB2 / 2) // 16

#define SB0_RES (2 * SB0) // 512
#define SB1_RES (2 * SB1) // 128
#define SB2_RES (2 * SB2) // 64
#define SB3_RES (2 * SB3) // 32

#define inv3 43691
#define inv15 61167

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

// c = a ^ b
#define vxor(c, a, b)                         \
    c.val[0] = veorq_u16(a.val[0], b.val[0]); \
    c.val[1] = veorq_u16(a.val[1], b.val[1]); \
    c.val[2] = veorq_u16(a.val[2], b.val[2]); \
    c.val[3] = veorq_u16(a.val[3], b.val[3]);

// c = a & b
#define vand(c, a, b)                  \
    c.val[0] = vandq_u16(a.val[0], b); \
    c.val[1] = vandq_u16(a.val[1], b); \
    c.val[2] = vandq_u16(a.val[2], b); \
    c.val[3] = vandq_u16(a.val[3], b);

// load c <= a
#define vload_x2(c, a) c = vld1q_u16_x2(a);

// store c <= a
#define vstore_x2(c, a) vst1q_u16_x2(c, a);

// c = a + b
#define vadd_x2(c, a, b)                      \
    c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
    c.val[1] = vaddq_u16(a.val[1], b.val[1]);

// c = a - b
#define vsub_x2(c, a, b)                      \
    c.val[0] = vsubq_u16(a.val[0], b.val[0]); \
    c.val[1] = vsubq_u16(a.val[1], b.val[1]);

// Evaluate and Copy, this go to TMP
void karat_neon_evaluate_combine(uint16_t *restrict w, uint16_t *restrict poly)
{
    uint16_t *c0 = poly,
             *c1 = &poly[SB3],
             *c2 = &poly[2 * SB3],
             *c3 = &poly[3 * SB3],
             *w0_mem = &w[0 * SB3],
             *w1_mem = &w[1 * SB3],
             *w2_mem = &w[2 * SB3],
             *w3_mem = &w[3 * SB3],
             *w4_mem = &w[4 * SB3],
             *w5_mem = &w[5 * SB3],
             *w6_mem = &w[6 * SB3],
             *w7_mem = &w[7 * SB3],
             *w8_mem = &w[8 * SB3];

    uint16x8x2_t r0, r1, r2, r3, tmp1, tmp2, tmp3;
    for (uint16_t addr = 0; addr < SB3; addr += 16)
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
        r8, sum0, sum1, sum2,                    // 4x2 = 8
        w20, w21, tmp;                           // 3x2 = 6
    uint16_t *w0_mem = &w[0 * SB3_RES],
             *w1_mem = &w[1 * SB3_RES],
             *w2_mem = &w[2 * SB3_RES],
             *w3_mem = &w[3 * SB3_RES],
             *w4_mem = &w[4 * SB3_RES],
             *w5_mem = &w[5 * SB3_RES],
             *w6_mem = &w[6 * SB3_RES],
             *w7_mem = &w[7 * SB3_RES],
             *w8_mem = &w[8 * SB3_RES];
    for (uint16_t addr = 0; addr < SB3_RES; addr += 16)
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

        vload_x2(tmp, &poly[addr + 0 * SB3]);
        vadd_x2(r0, tmp, r0);
        vstore_x2(&poly[addr + 0 * SB3], r0);

        vload_x2(tmp, &poly[addr + 1 * SB3]);
        vadd_x2(sum0, tmp, sum0);
        vstore_x2(&poly[addr + 1 * SB3], sum0);

        vload_x2(tmp, &poly[addr + 2 * SB3]);
        vadd_x2(w20, tmp, w20);
        vstore_x2(&poly[addr + 2 * SB3], w20);

        vload_x2(tmp, &poly[addr + 3 * SB3]);
        vadd_x2(sum1, tmp, sum1);
        vstore_x2(&poly[addr + 3 * SB3], sum1);

        vload_x2(tmp, &poly[addr + 4 * SB3]);
        vadd_x2(w21, tmp, w21);
        vstore_x2(&poly[addr + 4 * SB3], w21);

        vload_x2(tmp, &poly[addr + 5 * SB3]);
        vadd_x2(sum2, tmp, sum2);
        vstore_x2(&poly[addr + 5 * SB3], sum2);

        vload_x2(tmp, &poly[addr + 6 * SB3]);
        vadd_x2(r8, tmp, r8);
        vstore_x2(&poly[addr + 6 * SB3], r8);
    }
}

// Ultilize all 32 SIMD registers
void tc4_evaluate_neon_SB1(uint16_t *restrict w[7], uint16_t const poly[SABER_N])
{
    uint16_t *c0 = poly,
             *c1 = &poly[1 * SB1],
             *c2 = &poly[2 * SB1],
             *c3 = &poly[3 * SB1],
             *w0_mem = w[0],
             *w6_mem = w[6],
             *w1_mem = w[1],
             *w2_mem = w[2],
             *w3_mem = w[3],
             *w4_mem = w[4],
             *w5_mem = w[5];
    uint16x8x4_t r0, r1, r2, r3, tmp0, tmp1, tmp2, tmp3;
    for (uint16_t addr = 0; addr < SB1; addr += 32)
    {
        vload(r0, &c0[addr]);
        vload(r1, &c1[addr]);
        vload(r2, &c2[addr]);
        vload(r3, &c3[addr]);
        vstore(&w0_mem[addr], r0);
        vstore(&w6_mem[addr], r3);

        vadd(tmp0, r0, r2);
        vadd(tmp1, r1, r3);

        vadd(tmp2, tmp0, tmp1);
        vsub(tmp3, tmp0, tmp1);
        vstore(&w1_mem[addr], tmp2);
        vstore(&w2_mem[addr], tmp3);

        vsl(tmp2, r0, 2);
        vadd(tmp2, tmp2, r2);
        vsl(tmp2, tmp2, 1);
        vsl(tmp3, r1, 2);
        vadd(tmp3, tmp3, r3);

        vadd(tmp0, tmp2, tmp3);
        vsub(tmp1, tmp2, tmp3);
        vstore(&w3_mem[addr], tmp0);
        vstore(&w4_mem[addr], tmp1);

        vsl(tmp3, r3, 3);
        vsl(tmp2, r2, 2);
        vsl(tmp1, r1, 1);
        vadd(tmp0, tmp3, tmp2);
        vadd(tmp0, tmp0, tmp1);
        vadd(tmp0, tmp0, r0);
        vstore(&w5_mem[addr], tmp0);
    }
}

// Ultilize all 32 SIMD registers
void tc4_interpolate_neon_SB1(uint16_t *restrict poly, uint16_t *restrict w[7])
{
    uint16x8x4_t r0, r1, r2, r3, r4, r5, r6, tmp;
    uint16_t *w0_mem = w[0],
             *w1_mem = w[1],
             *w2_mem = w[2],
             *w3_mem = w[3],
             *w4_mem = w[4],
             *w5_mem = w[5],
             *w6_mem = w[6];
    for (uint16_t addr = 0; addr < SB1_RES; addr += 32)
    {
        vload(r0, &w0_mem[addr]);
        vload(r1, &w1_mem[addr]);
        vload(r2, &w2_mem[addr]);
        vload(r3, &w3_mem[addr]);
        vload(r4, &w4_mem[addr]);
        vload(r5, &w5_mem[addr]);
        vload(r6, &w6_mem[addr]);

        vadd(r5, r5, r3);
        vadd(r4, r4, r3);
        vsr(r4, r4, 1);
        vadd(r2, r2, r1);
        vsr(r2, r2, 1);
        vsub(r3, r3, r4);
        vsub(r1, r1, r2);
        vsl(tmp, r2, 6);
        vadd(tmp, tmp, r2);
        vsub(r5, r5, tmp);
        vsub(r2, r2, r6);
        vsub(r2, r2, r0);
        vmuln(tmp, r2, 45);
        vadd(r5, tmp, r5);
        vsub(r4, r4, r6);
        vsr(r4, r4, 2);
        vsr(r3, r3, 1);
        vsl(tmp, r3, 2);
        vsub(r5, r5, tmp);
        vsub(r3, r3, r1);
        vmuln(r3, r3, inv3);
        vsl(tmp, r0, 4);
        vsub(r4, r4, tmp);
        vsl(tmp, r2, 2);
        vsub(r4, r4, tmp);
        vmuln(r4, r4, inv3);

        vadd(r2, r2, r4);
        vsr(r5, r5, 1);
        vmuln(r5, r5, inv15);
        vsub(r1, r1, r5);
        vsub(r1, r1, r3);
        vmuln(r1, r1, inv3);

        vsl(tmp, r1, 2);
        vadd(tmp, tmp, r1);
        vadd(r3, r3, tmp);
        vsub(r5, r5, r1);

        vload(tmp, &poly[addr]);
        vadd(r0, tmp, r0);
        vstore(&poly[addr], r0);

        vload(tmp, &poly[SB1 + addr]);
        vsub(r1, tmp, r1);
        vstore(&poly[SB1 + addr], r1);

        vload(tmp, &poly[2 * SB1 + addr]);
        vadd(r2, tmp, r2);
        vstore(&poly[2 * SB1 + addr], r2);

        vload(tmp, &poly[3 * SB1 + addr]);
        vadd(r3, tmp, r3);
        vstore(&poly[3 * SB1 + addr], r3);

        vload(tmp, &poly[4 * SB1 + addr]);
        vsub(r4, tmp, r4);
        vstore(&poly[4 * SB1 + addr], r4);

        vload(tmp, &poly[5 * SB1 + addr]);
        vadd(r5, tmp, r5);
        vstore(&poly[5 * SB1 + addr], r5);

        vload(tmp, &poly[6 * SB1 + addr]);
        vadd(r6, tmp, r6);
        vstore(&poly[6 * SB1 + addr], r6);
    }
}

static inline void neon_poly_neon_reduction(uint16_t *restrict poly, uint16_t *restrict tmp, const uint16_t MASK)
{
    uint16x8_t mask;
    uint16x8x4_t res, tmp1, tmp2;
    mask = vdupq_n_u16(MASK - 1);
    for (uint16_t addr = 0; addr < SABER_N; addr += 32)
    {
        vload(tmp1, &tmp[addr]);
        vload(tmp2, &tmp[addr + SABER_N]);
        vsub(res, tmp1, tmp2);
        vand(res, res, mask);
        vstore(&poly[addr], res);
    }
}

void neon_toom_cook_422_evaluate(uint16_t tmp_out[SB3 * 64], uint16_t const poly[SABER_N])
{
    uint16_t *w[7];
    uint16_t tmp[7 * SB1];

    w[0] = &tmp[0 * SB1];
    w[1] = &tmp[1 * SB1];
    w[2] = &tmp[2 * SB1];
    w[3] = &tmp[3 * SB1];
    w[4] = &tmp[4 * SB1];
    w[5] = &tmp[5 * SB1];
    w[6] = &tmp[6 * SB1];

    tc4_evaluate_neon_SB1(w, poly);

    karat_neon_evaluate_combine(&tmp_out[0 * 9 * SB3], w[0]);
    karat_neon_evaluate_combine(&tmp_out[1 * 9 * SB3], w[1]);
    karat_neon_evaluate_combine(&tmp_out[2 * 9 * SB3], w[2]);
    karat_neon_evaluate_combine(&tmp_out[3 * 9 * SB3], w[3]);
    karat_neon_evaluate_combine(&tmp_out[4 * 9 * SB3], w[4]);
    karat_neon_evaluate_combine(&tmp_out[5 * 9 * SB3], w[5]);
    karat_neon_evaluate_combine(&tmp_out[6 * 9 * SB3], w[6]);
    transpose_8x16(tmp_out);
}

static inline void neon_toom_cook_422_mul(uint16_t tmp_cc[SB3_RES * 64], uint16_t tmp_aa[SB3 * 64], uint16_t tmp_bb[SB3 * 64])
{
    schoolbook_neon(tmp_cc, tmp_aa, tmp_bb);
}

void neon_toom_cook_422_interpolate(uint16_t poly[2 * SABER_N], uint16_t tmp_cc[SB3_RES * 64])
{
    uint16x8x4_t zero;
    uint16_t tmp_aabb[SB1_RES * 7];
    uint16_t *cw[7];

    cw[0] = &tmp_aabb[0 * SB1_RES];
    cw[1] = &tmp_aabb[1 * SB1_RES];
    cw[2] = &tmp_aabb[2 * SB1_RES];
    cw[3] = &tmp_aabb[3 * SB1_RES];
    cw[4] = &tmp_aabb[4 * SB1_RES];
    cw[5] = &tmp_aabb[5 * SB1_RES];
    cw[6] = &tmp_aabb[6 * SB1_RES];

    vxor(zero, zero, zero);
    for (uint16_t addr = 0; addr < SB1_RES * 7; addr += 32)
    {
        vstore(&tmp_aabb[addr], zero);
    }

    // Transpose 8x8x32
    transpose_8x32(tmp_cc);

    karat_neon_interpolate_combine(cw[0], &tmp_cc[0 * 9 * SB3_RES]);
    karat_neon_interpolate_combine(cw[1], &tmp_cc[1 * 9 * SB3_RES]);
    karat_neon_interpolate_combine(cw[2], &tmp_cc[2 * 9 * SB3_RES]);
    karat_neon_interpolate_combine(cw[3], &tmp_cc[3 * 9 * SB3_RES]);
    karat_neon_interpolate_combine(cw[4], &tmp_cc[4 * 9 * SB3_RES]);
    karat_neon_interpolate_combine(cw[5], &tmp_cc[5 * 9 * SB3_RES]);
    karat_neon_interpolate_combine(cw[6], &tmp_cc[6 * 9 * SB3_RES]);

    // Interpolate C = A*B = CC
    // Size: 128*7 to 128*4 = 512
    tc4_interpolate_neon_SB1(poly, cw);
}

void neon_vector_vector_mul(uint16_t accumulate[SABER_N], const uint16_t modP,
                            const uint16_t polyvecA[SABER_K][SABER_N],
                            const uint16_t polyvecB[SABER_K][SABER_N])
{
    uint16_t tmp_cc[SB3_RES * 64],
        tmp_acc[SB3_RES * 64],
        polyC[2 * SABER_N];
    uint16x8x4_t zero, tmp, acc;
    uint16x8_t mod;
    vxor(zero, zero, zero);
    for (uint16_t addr = 0; addr < SB3_RES * 64; addr += 32)
    {
        vstore(&tmp_acc[addr], zero);
    }

    uint16_t tmp_aa[SB3 * 64], tmp_bb[SB3 * 64];

    for (uint16_t k = 0; k < SABER_K; k++)
    {
        neon_toom_cook_422_evaluate(tmp_aa, polyvecA[k]);
        neon_toom_cook_422_evaluate(tmp_bb, polyvecB[k]);
        neon_toom_cook_422_mul(tmp_cc, tmp_aa, tmp_bb);

        for (uint16_t addr = 0; addr < SB3_RES * 64; addr += 32)
        {
            vload(tmp, &tmp_cc[addr]);
            vload(acc, &tmp_acc[addr]);

            vadd(acc, acc, tmp);
            vstore(&tmp_acc[addr], acc);
        }
    }

    for (uint16_t addr = 0; addr < SABER_N * 2; addr += 32)
    {
        vstore(&polyC[addr], zero);
    }
    neon_toom_cook_422_interpolate(polyC, tmp_acc);

    neon_poly_neon_reduction(accumulate, polyC, SABER_P);
}

void printArray(uint16_t *M, char *string, uint16_t length)
{
    printf("%s\n", string);
    for (uint16_t i = 0; i < length; i++)
    {
        printf("%d, ", M[i]);
    }
    printf("\n");
}

void neon_matrix_vector_mul(uint16_t vectorB[SABER_K][SABER_N], const uint16_t modQ,
                            const polyvec matrixA[SABER_K],
                            const uint16_t vectorS[SABER_K][SABER_N])
{
    uint16_t tmp_vector_eval[SB3 * 64],
        tmp_matrix_eval[SB3 * 64],
        tmp_accumulate[SB3_RES * 64],
        tmp_res[SABER_K][SB3_RES * 64];

    uint16x8x4_t neon_acc, neon_res;

    uint16x8x4_t zero;
    vxor(zero, zero, zero);
    for (uint16_t i = 0; i < SABER_K; i++)
    {
        for (uint16_t addr = 0; addr < SB3_RES * 64; addr += 32)
        {
            vstore(&tmp_res[i][addr], zero);
        }
    }

    for (uint16_t j = 0; j < SABER_K; j++)
    {
        neon_toom_cook_422_evaluate(tmp_vector_eval, vectorS[j]);
        for (uint16_t i = 0; i < SABER_K; i++)
        {
            neon_toom_cook_422_evaluate(tmp_matrix_eval, matrixA[i].vec[j].coeffs);
            neon_toom_cook_422_mul(tmp_accumulate, tmp_vector_eval, tmp_matrix_eval);

            for (uint16_t addr = 0; addr < SB3_RES * 64; addr += 32)
            {
                vload(neon_acc, &tmp_accumulate[addr]);
                vload(neon_res, &tmp_res[i][addr]);

                vadd(neon_res, neon_acc, neon_res);

                vstore(&tmp_res[i][addr], neon_res);
            }
        }
    }

    vxor(zero, zero, zero);
    for (uint16_t addr = 0; addr < SB3_RES * 16 * SABER_K; addr += 32)
    {
        vstore(&tmp_accumulate[addr], zero);
    }

    for (uint16_t i = 0; i < SABER_K; i++)
    {
        neon_toom_cook_422_interpolate(&tmp_accumulate[i << 9], tmp_res[i]);
        neon_poly_neon_reduction(vectorB[i], &tmp_accumulate[i << 9], SABER_Q);
    }
}

void neon_matrix_vector_mul_transpose(uint16_t vectorB[SABER_K][SABER_N], const uint16_t modQ,
                                      const polyvec matrixA[SABER_K],
                                      const uint16_t vectorS[SABER_K][SABER_N])
{
    uint16_t tmp_vector_eval[SB3 * 64],
        tmp_matrix_eval[SB3 * 64],
        tmp_accumulate[SB3_RES * 64],
        tmp_res[SABER_K][SB3_RES * 64];

    uint16x8x4_t neon_acc, neon_res;

    uint16x8x4_t zero;
    vxor(zero, zero, zero);
    for (uint16_t i = 0; i < SABER_K; i++)
    {
        for (uint16_t addr = 0; addr < SB3_RES * 64; addr += 32)
        {
            vstore(&tmp_res[i][addr], zero);
        }
    }

    for (uint16_t j = 0; j < SABER_K; j++)
    {
        neon_toom_cook_422_evaluate(tmp_vector_eval, vectorS[j]);
        for (uint16_t i = 0; i < SABER_K; i++)
        {
            neon_toom_cook_422_evaluate(tmp_matrix_eval, matrixA[j].vec[i].coeffs);
            neon_toom_cook_422_mul(tmp_accumulate, tmp_vector_eval, tmp_matrix_eval);

            for (uint16_t addr = 0; addr < SB3_RES * 64; addr += 32)
            {
                vload(neon_acc, &tmp_accumulate[addr]);
                vload(neon_res, &tmp_res[i][addr]);

                vadd(neon_res, neon_acc, neon_res);

                vstore(&tmp_res[i][addr], neon_res);
            }
        }
    }

    vxor(zero, zero, zero);
    for (uint16_t addr = 0; addr < SB3_RES * 16 * SABER_K; addr += 32)
    {
        vstore(&tmp_accumulate[addr], zero);
    }

    for (uint16_t i = 0; i < SABER_K; i++)
    {
        neon_toom_cook_422_interpolate(&tmp_accumulate[i << 9], tmp_res[i]);
        neon_poly_neon_reduction(vectorB[i], &tmp_accumulate[i << 9], SABER_Q);
    }
}