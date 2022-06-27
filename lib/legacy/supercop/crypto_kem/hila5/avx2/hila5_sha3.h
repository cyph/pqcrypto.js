// hila5_sha3.h
// 2011-11-19  Markku-Juhani O. Saarinen <mjos@iki.fi>

#ifndef _HILA5_SHA3_H_
#define _HILA5_SHA3_H_

#include <stddef.h>
#include <stdint.h>

// state context
typedef struct {
    union {                                 // state:
        uint8_t b[200];                     // 8-bit bytes
        uint64_t q[25];                     // 64-bit words
    } st;
    int pt, rsiz, mdlen;                    // these don't overflow
} hila5_sha3_ctx_t;

// OpenSSL - like interface
int hila5_sha3_init(hila5_sha3_ctx_t *c, int mdlen); // mdlen = hash len, bytes
int hila5_sha3_update(hila5_sha3_ctx_t *c, const void *data, size_t len);
int hila5_sha3_final(void *md, hila5_sha3_ctx_t *c); // digest goes to md

// compute a sha3 hash (md) of given byte length from "in"
void *hila5_sha3(const void *in, size_t inlen, void *md, int mdlen);

// SHAKE128 and SHAKE256 extensible-output functions
#define hila5_shake128_init(c) hila5_sha3_init(c, 16)
#define hila5_shake256_init(c) hila5_sha3_init(c, 32)
#define hila5_shake_update hila5_sha3_update

void hila5_shake_xof(hila5_sha3_ctx_t *c);
void hila5_shake_out(hila5_sha3_ctx_t *c, void *out, size_t len);

#endif

