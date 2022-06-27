/// @file blas_u32.h
/// @brief Inlined functions for implementing basic linear algebra functions for uint32 arch.
///

#ifndef _BLAS_U32_H_
#define _BLAS_U32_H_

#include "gf16.h"

#include <string.h>
#include <stdint.h>

static inline void _gf256v_add_u32(uint8_t *accu_b, const uint8_t *a, unsigned _num_byte) {
    unsigned n_u32 = _num_byte >> 2;
    for (unsigned i = 0; i < n_u32; i++) {
      uint32_t bx;
      uint32_t ax;
      memcpy(&bx,accu_b+4*i,4);
      memcpy(&ax,a+4*i,4);
      bx ^= ax;
      memcpy(accu_b+4*i,&bx,4);
    }

    unsigned rem = _num_byte & 3;
    if( !rem ) return;
    a += (n_u32 << 2);
    accu_b += (n_u32 << 2);
    for (unsigned i = 0; i < rem; i++) accu_b[i] ^= a[i];
}

static inline void _gf256v_conditional_add_u32(uint8_t *accu_b, uint8_t condition, const uint8_t *a, unsigned _num_byte) {
    uint32_t pr_u32 = ((uint32_t) 0) - ((uint32_t) condition);
    uint8_t pr_u8 = pr_u32 & 0xff;

    unsigned n_u32 = _num_byte >> 2;
    for (unsigned i = 0; i < n_u32; i++) {
      uint32_t bx;
      uint32_t ax;
      memcpy(&bx,accu_b+4*i,4);
      memcpy(&ax,a+4*i,4);
      bx ^= ax&pr_u32;
      memcpy(accu_b+4*i,&bx,4);
    }

    unsigned rem = _num_byte & 3;
    if( !rem ) return;
    a += (n_u32 << 2);
    accu_b += (n_u32 << 2);
    for (unsigned i = 0; i < rem; i++) accu_b[i] ^= (a[i] & pr_u8);
}

///////////////////////////////////////////////////

static inline void _gf16v_mul_scalar_u32(uint8_t *a, uint8_t gf16_b, unsigned _num_byte) {
    unsigned n_u32 = _num_byte >> 2;
    for (unsigned i = 0; i < n_u32; i++) {
      uint32_t ax;
      memcpy(&ax,a+4*i,4);
      ax = gf16v_mul_u32(ax, gf16_b);
      memcpy(a+4*i,&ax,4);
    }

    unsigned rem = _num_byte & 3;
    if( !rem ) return;
    union tmp_32 {
        uint8_t u8[4];
        uint32_t u32;
    } t;
    a += (n_u32 << 2);
    for (unsigned i = 0; i < rem; i++) t.u8[i] = a[i];
    t.u32 = gf16v_mul_u32(t.u32, gf16_b);
    for (unsigned i = 0; i < rem; i++) a[i] = t.u8[i];
}

static inline void _gf256v_mul_scalar_u32(uint8_t *a, uint8_t b, unsigned _num_byte) {
    unsigned n_u32 = _num_byte >> 2;
    for (unsigned i = 0; i < n_u32; i++) {
      uint32_t ax;
      memcpy(&ax,a+4*i,4);
      ax = gf256v_mul_u32(ax, b);
      memcpy(a+4*i,&ax,4);
    }

    unsigned rem = _num_byte & 3;
    if( !rem ) return;
    union tmp_32 {
        uint8_t u8[4];
        uint32_t u32;
    } t;
    a += (n_u32 << 2);
    for (unsigned i = 0; i < rem; i++) t.u8[i] = a[i];
    t.u32 = gf256v_mul_u32(t.u32, b);
    for (unsigned i = 0; i < rem; i++) a[i] = t.u8[i];
}

/////////////////////////////////////

static inline void _gf16v_madd_u32(uint8_t *accu_c, const uint8_t *a, uint8_t gf16_b, unsigned _num_byte) {
    unsigned n_u32 = _num_byte >> 2;
    for (unsigned i = 0; i < n_u32; i++) {
      uint32_t ax;
      uint32_t cx;
      memcpy(&ax,a+4*i,4);
      memcpy(&cx,accu_c+4*i,4);
      cx ^= gf16v_mul_u32(ax, gf16_b);
      memcpy(accu_c+4*i,&cx,4);
    }

    unsigned rem = _num_byte & 3;
    if( !rem ) return;
    union tmp_32 {
        uint8_t u8[4];
        uint32_t u32;
    } t;
    accu_c += (n_u32 << 2);
    a += (n_u32 << 2);
    for (unsigned i = 0; i < rem; i++) t.u8[i] = a[i];
    t.u32 = gf16v_mul_u32(t.u32, gf16_b);
    for (unsigned i = 0; i < rem; i++) accu_c[i] ^= t.u8[i];
}

static inline void _gf256v_madd_u32(uint8_t *accu_c, const uint8_t *a, uint8_t gf256_b, unsigned _num_byte) {
    unsigned n_u32 = _num_byte >> 2;
    for (unsigned i = 0; i < n_u32; i++) {
      uint32_t ax;
      uint32_t cx;
      memcpy(&ax,a+4*i,4);
      memcpy(&cx,accu_c+4*i,4);
      cx ^= gf256v_mul_u32(ax, gf256_b);
      memcpy(accu_c+4*i,&cx,4);
    }

    unsigned rem = _num_byte & 3;
    if( !rem ) return;
    union tmp_32 {
        uint8_t u8[4];
        uint32_t u32;
    } t;
    accu_c += (n_u32 << 2);
    a += (n_u32 << 2);
    for (unsigned i = 0; i < rem; i++) t.u8[i] = a[i];
    t.u32 = gf256v_mul_u32(t.u32, gf256_b);
    for (unsigned i = 0; i < rem; i++) accu_c[i] ^= t.u8[i];
}


#endif // _BLAS_U32_H_

