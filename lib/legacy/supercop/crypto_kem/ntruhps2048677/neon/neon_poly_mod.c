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
#include "poly.h"

// score c <= a
#define poly_vstore_x1(c, a) vst1q_u16(c, a);

// c = a >> value
#define poly_vsr_x1(c, a, value) c = vshrq_n_u16(a, value);

// c = a << value
#define poly_vsl_x1(c, a, value) c = vshlq_n_u16(a, value);

// c = value
#define poly_vdup_x1(c, value) c = vdupq_n_u16(value);

// c = a & b
#define poly_vand_x1(c, a, b) c = vandq_u16(a, b);

// c = a + b
#define poly_vadd_x1(c, a, b) c = vaddq_u16(a, b);

// c = a - b
#define poly_vsub_x1(c, a, b) c = vsubq_u16(a, b);

// c = a ^ b
#define poly_vxor_x1(c, a, b) c = veorq_u16(a, b);

#if defined(__clang__)

// load c <= a
#define poly_vload(c, a) c = vld1q_u16_x4(a);

#elif defined(__GNUC__)

// load c <= a
#define poly_vload(c, a)          \
    c.val[0] = vld1q_u16(a);      \
    c.val[1] = vld1q_u16(a + 8);  \
    c.val[2] = vld1q_u16(a + 16); \
    c.val[3] = vld1q_u16(a + 24);

#else
#error "Unsupported compiler"
#endif

// store c <= a
#define poly_vstore(c, a)                    \
    vst1q_u16(c + 0, (uint16x8_t)a.val[0]);  \
    vst1q_u16(c + 8, (uint16x8_t)a.val[1]);  \
    vst1q_u16(c + 16, (uint16x8_t)a.val[2]); \
    vst1q_u16(c + 24, (uint16x8_t)a.val[3]);

// store c <= a
#define poly_vstore_const(c, a) \
    vst1q_u16(c + 0, a);        \
    vst1q_u16(c + 8, a);        \
    vst1q_u16(c + 16, a);       \
    vst1q_u16(c + 24, a);

// c = a >> value
#define poly_vsr(c, a, value)                \
    c.val[0] = vshrq_n_u16(a.val[0], value); \
    c.val[1] = vshrq_n_u16(a.val[1], value); \
    c.val[2] = vshrq_n_u16(a.val[2], value); \
    c.val[3] = vshrq_n_u16(a.val[3], value);

// c = a >> value
#define poly_vsr_sign(c, a, value)           \
    c.val[0] = vshrq_n_s16(a.val[0], value); \
    c.val[1] = vshrq_n_s16(a.val[1], value); \
    c.val[2] = vshrq_n_s16(a.val[2], value); \
    c.val[3] = vshrq_n_s16(a.val[3], value);

// c = a << value
#define poly_vsl(c, a, value)                \
    c.val[0] = vshlq_n_u16(a.val[0], value); \
    c.val[1] = vshlq_n_u16(a.val[1], value); \
    c.val[2] = vshlq_n_u16(a.val[2], value); \
    c.val[3] = vshlq_n_u16(a.val[3], value);

// c = a & const
#define poly_vand_const(c, a, b)       \
    c.val[0] = vandq_u16(a.val[0], b); \
    c.val[1] = vandq_u16(a.val[1], b); \
    c.val[2] = vandq_u16(a.val[2], b); \
    c.val[3] = vandq_u16(a.val[3], b);

// c = a & b
#define poly_vand_sign(c, a, b)                          \
    c.val[0] = vandq_s16(a.val[0], (int16x8_t)b.val[0]); \
    c.val[1] = vandq_s16(a.val[1], (int16x8_t)b.val[1]); \
    c.val[2] = vandq_s16(a.val[2], (int16x8_t)b.val[2]); \
    c.val[3] = vandq_s16(a.val[3], (int16x8_t)b.val[3]);

// c = a + b
#define poly_vadd(c, a, b)                    \
    c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
    c.val[1] = vaddq_u16(a.val[1], b.val[1]); \
    c.val[2] = vaddq_u16(a.val[2], b.val[2]); \
    c.val[3] = vaddq_u16(a.val[3], b.val[3]);

// c = a + const
#define poly_vadd_const(c, a, b)       \
    c.val[0] = vaddq_u16(a.val[0], b); \
    c.val[1] = vaddq_u16(a.val[1], b); \
    c.val[2] = vaddq_u16(a.val[2], b); \
    c.val[3] = vaddq_u16(a.val[3], b);

// c = a - b
#define poly_vsub_const_sign(c, a, b)                        \
    c.val[0] = vsubq_s16((int16x8_t)a.val[0], (int16x8_t)b); \
    c.val[1] = vsubq_s16((int16x8_t)a.val[1], (int16x8_t)b); \
    c.val[2] = vsubq_s16((int16x8_t)a.val[2], (int16x8_t)b); \
    c.val[3] = vsubq_s16((int16x8_t)a.val[3], (int16x8_t)b);

// c = a - const
#define poly_vsub_const(c, a, b)       \
    c.val[0] = vsubq_u16(a.val[0], b); \
    c.val[1] = vsubq_u16(a.val[1], b); \
    c.val[2] = vsubq_u16(a.val[2], b); \
    c.val[3] = vsubq_u16(a.val[3], b);

// c = a ^ b
#define poly_vxor_sign(c, a, b)               \
    c.val[0] = veorq_s16(a.val[0], b.val[0]); \
    c.val[1] = veorq_s16(a.val[1], b.val[1]); \
    c.val[2] = veorq_s16(a.val[2], b.val[2]); \
    c.val[3] = veorq_s16(a.val[3], b.val[3]);

// c = a ^ b
#define poly_vxor_const(c, a, b)       \
    c.val[0] = veorq_u16(a.val[0], b); \
    c.val[1] = veorq_u16(a.val[1], b); \
    c.val[2] = veorq_u16(a.val[2], b); \
    c.val[3] = veorq_u16(a.val[3], b);

// c = ~a
#define poly_vnot_sign(c, a)        \
    c.val[0] = vmvnq_s16(a.val[0]); \
    c.val[1] = vmvnq_s16(a.val[1]); \
    c.val[2] = vmvnq_s16(a.val[2]); \
    c.val[3] = vmvnq_s16(a.val[3]);

// void neon_poly_mod_3_Phi_n(poly *r) name for testing
void poly_mod_3_Phi_n(poly *r)
{
    // 4 SIMD registers
    uint16x8_t last, hex_0xff, hex_0x0f, hex_0x03;
    // last = r->coeffs[NTRU_N -1]
    poly_vdup_x1(last, r->coeffs[NTRU_N - 1]);
    // last = last << 1
    poly_vsl_x1(last, last, 1);

    poly_vdup_x1(hex_0xff, 0xff);
    poly_vdup_x1(hex_0x0f, 0x0f);
    poly_vdup_x1(hex_0x03, 0x03);

    // 8x4 = 32 SIMD registers
    uint16x8x4_t r0, r1, r2, r3;
    int16x8x4_t t, c, a, b;

    for (uint16_t addr = 0; addr < NTRU_N32; addr += 32)
    {
        poly_vload(r0, &r->coeffs[addr]);

        // r->coeffs[i] + 2*r->coeffs[NTRU_N-1]
        poly_vadd_const(r3, r0, last);

        // r3 = (r3 >> 8) + (r3 & 0xff)
        poly_vsr(r1, r3, 8);
        poly_vand_const(r2, r3, hex_0xff);
        poly_vadd(r3, r1, r2);

        // r3 = (r3 >> 4) + (r3 & 0xf)
        poly_vsr(r1, r3, 4);
        poly_vand_const(r2, r3, hex_0x0f);
        poly_vadd(r3, r1, r2);

        // r3 = (r3 >> 2) + (r3 & 0x3)
        poly_vsr(r1, r3, 2);
        poly_vand_const(r2, r3, hex_0x03);
        poly_vadd(r3, r1, r2);

        // r3 = (r3 >> 2) + (r3 & 0x3)
        poly_vsr(r1, r3, 2);
        poly_vand_const(r2, r3, hex_0x03);
        poly_vadd(r3, r1, r2);

        // t = r3 - 3
        poly_vsub_const_sign(t, r3, hex_0x03);
        // c = t >> 15
        poly_vsr_sign(c, t, 15);

        // a = c & r3
        poly_vand_sign(a, c, r3);
        // b = ~c & t
        poly_vnot_sign(b, c);
        poly_vand_sign(b, b, t);
        // c = a ^ b
        poly_vxor_sign(c, a, b);

        poly_vstore(&r->coeffs[addr], c);
    }
    // 677, 678, 679
    r->coeffs[NTRU_N] = 0;
    r->coeffs[NTRU_N + 1] = 0;
    r->coeffs[NTRU_N + 2] = 0;

    poly_vxor_x1(last, last, last);
    // 680 + 32 = 712
    poly_vstore_const(&r->coeffs[NTRU_N + 3], last);
    // 712 -> 720
    poly_vstore_x1(&r->coeffs[NTRU_N + 35], last);
}

// void neon_poly_mod_q_Phi_n(poly *r) name for testing only
void poly_mod_q_Phi_n(poly *r)
{
    // 1 SIMD registers
    uint16x8_t last;
    // last = r->coeffs[NTRU_N -1]
    poly_vdup_x1(last, r->coeffs[NTRU_N - 1]);

    // 8 SIMD registers
    uint16x8x4_t r0, r3;
    for (uint16_t addr = 0; addr < NTRU_N32; addr += 32)
    {
        poly_vload(r0, &r->coeffs[addr]);

        poly_vsub_const(r3, r0, last);

        poly_vstore(&r->coeffs[addr], r3);
    }
    // 677, 678, 679
    r->coeffs[NTRU_N] = 0;
    r->coeffs[NTRU_N + 1] = 0;
    r->coeffs[NTRU_N + 2] = 0;

    poly_vxor_x1(last, last, last);
    // 680 + 32 = 712
    poly_vstore_const(&r->coeffs[NTRU_N + 3], last);
    // 712 -> 720
    poly_vstore_x1(&r->coeffs[NTRU_N + 35], last);
}

// void neon_poly_Rq_to_S3(poly *r, const poly *a) name for testing only
void poly_Rq_to_S3(poly *r, const poly *a)
{
    // 7 SIMD registers
    uint16x8_t last, modQ, tt, tmp, hex_0xff, hex_0x0f, hex_0x03;
    poly_vdup_x1(last, a->coeffs[NTRU_N - 1]);

    poly_vdup_x1(modQ, NTRU_Q - 1);

    poly_vdup_x1(hex_0x03, 0x03);
    poly_vdup_x1(hex_0xff, 0xff);
    poly_vdup_x1(hex_0x0f, 0x0f);

    // t = MODQ(a->coeffs[i]);
    poly_vand_x1(tt, last, modQ);
    //  tmp = ((t >> (NTRU_LOGQ-1)) ^ 3) << NTRU_LOGQ;
    poly_vsr_x1(tmp, tt, NTRU_LOGQ - 1);
    poly_vxor_x1(tmp, tmp, hex_0x03);
    poly_vsl_x1(tmp, tmp, NTRU_LOGQ);
    // last = tmp + t
    poly_vadd_x1(last, tmp, tt);
    // last = last << 1
    poly_vsl_x1(last, last, 1);

    // 8x4 = 32 SIMD registers
    uint16x8x4_t r0, r1, r2, r3;
    int16x8x4_t t, c, aa, bb;

    for (uint16_t addr = 0; addr < NTRU_N32; addr += 32)
    {
        poly_vload(r0, &a->coeffs[addr]);
        poly_vand_const(r0, r0, modQ);

        poly_vsr(r1, r0, NTRU_LOGQ - 1);
        poly_vxor_const(r1, r1, hex_0x03);
        poly_vsl(r1, r1, NTRU_LOGQ);

        poly_vadd(r2, r0, r1);

        poly_vadd_const(r3, r2, last);

        // r3 = (res >> 8) + (res & 0xff)
        poly_vsr(r1, r3, 8);
        poly_vand_const(r2, r3, hex_0xff);
        poly_vadd(r3, r1, r2);

        // r3 = (r3 >> 4) + (r3 & 0xf)
        poly_vsr(r1, r3, 4);
        poly_vand_const(r2, r3, hex_0x0f);
        poly_vadd(r3, r1, r2);

        // r3 = (r3 >> 2) + (r3 & 0x3)
        poly_vsr(r1, r3, 2);
        poly_vand_const(r2, r3, hex_0x03);
        poly_vadd(r3, r1, r2);

        // r3 = (r3 >> 2) + (r3 & 0x3)
        poly_vsr(r1, r3, 2);
        poly_vand_const(r2, r3, hex_0x03);
        poly_vadd(r3, r1, r2);

        // t = r3 - 3
        poly_vsub_const_sign(t, r3, hex_0x03);
        // c = t >> 15
        poly_vsr_sign(c, t, 15);

        // a = c & r3
        poly_vand_sign(aa, c, r3);
        // b = ~c & t
        poly_vnot_sign(bb, c);
        poly_vand_sign(bb, bb, t);
        // c = a ^ b
        poly_vxor_sign(c, aa, bb);

        poly_vstore(&r->coeffs[addr], c);
    }
    // 677, 678, 679
    r->coeffs[NTRU_N] = 0;
    r->coeffs[NTRU_N + 1] = 0;
    r->coeffs[NTRU_N + 2] = 0;

    poly_vxor_x1(last, last, last);
    // 680 + 32 = 712
    poly_vstore_const(&r->coeffs[NTRU_N + 3], last);
    // 712 -> 720
    poly_vstore_x1(&r->coeffs[NTRU_N + 35], last);
}
