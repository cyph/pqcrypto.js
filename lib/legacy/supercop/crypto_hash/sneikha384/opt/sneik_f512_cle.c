//  f512_cle.c
//  2019-02-23  Markku-Juhani O. Saarinen <mjos@pqshield.com>
//  Copyright (C) 2019, PQShield Ltd. Please see LICENSE.

//  SNEIK v1.1 f512 implementation for a 32-bit little endian C99 target.

#include <stdint.h>

#ifndef ROR32
#define ROR32(x, y) (((x) >> (y)) | ((x) << (32 - (y))))
#endif

#define MIX_F(pos, vec, t0, t1, t2, t3) {       \
    t0 += t3;                                   \
    t0 = t0 ^ ROR32(t0, 7) ^ ROR32(t0, 8);      \
    t0 ^= ROR32(t2, 31);                        \
    t2 = vec[(pos + 2) & 0xF];                  \
    t0 += t2;                                   \
    t0 = t0 ^ ROR32(t0, 15) ^ ROR32(t0, 23);    \
    t0 ^= t1;                                   \
    vec[pos] = t0;                              \
}

void sneik_f512(void *state, uint8_t dom, uint8_t rounds)
{
    const uint8_t rc[] = {
        0xEF, 0xE0, 0xD9, 0xD6, 0xBA, 0xB5, 0x8C, 0x83,
        0x10, 0x1F, 0x26, 0x29, 0x45, 0x4A, 0x73, 0x7C  // (only 8 used now)
    };

    int i;
    uint32_t *sv = (uint32_t *) state;      // little endian, alignment ok
    uint32_t w0, w1, w2, w3;

    w0 = sv[0];
    w1 = sv[1];
    w2 = sv[14];
    w3 = sv[15];

    for (i = 0; i < rounds; i++) {
        w0 ^= (uint32_t) rc[i];
        w1 ^= (uint32_t) dom;
        MIX_F(  0,  sv, w0, w1, w2, w3 );
        MIX_F(  1,  sv, w1, w2, w3, w0 );
        MIX_F(  2,  sv, w2, w3, w0, w1 );
        MIX_F(  3,  sv, w3, w0, w1, w2 );
        MIX_F(  4,  sv, w0, w1, w2, w3 );
        MIX_F(  5,  sv, w1, w2, w3, w0 );
        MIX_F(  6,  sv, w2, w3, w0, w1 );
        MIX_F(  7,  sv, w3, w0, w1, w2 );
        MIX_F(  8,  sv, w0, w1, w2, w3 );
        MIX_F(  9,  sv, w1, w2, w3, w0 );
        MIX_F(  10, sv, w2, w3, w0, w1 );
        MIX_F(  11, sv, w3, w0, w1, w2 );
        MIX_F(  12, sv, w0, w1, w2, w3 );
        MIX_F(  13, sv, w1, w2, w3, w0 );
        MIX_F(  14, sv, w2, w3, w0, w1 );
        MIX_F(  15, sv, w3, w0, w1, w2 );
    }
}

