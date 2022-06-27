// Coral 256 implementation
// Target: generic C99
// Copyright (c)
#include <stdint.h>
#include "api.h"
#include "crypto_hash.h"

#define SBOX_SIZE 4
#define STATE_SIZE 32
#define RATE 4
#define PADDING_BYTE 0x80
#define ROUNDS 10
#define ROTR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))
#define MIX(x, a, b) x ^= ROTR(x, a) ^ ROTR(x, b)

void load64(uint64_t *x, unsigned char *S) {
    *x = (uint64_t)S[0];
    for (int i = 1; i < 8; ++i) {
        *x <<= 8;
        *x |= (uint64_t)S[i];
    }
}

void store64(unsigned char *S, uint64_t x) {
    S[7] = (unsigned char)x;
    for (int i = 6; i >= 0; --i) {
        x >>= 8;
        S[i] = (unsigned char)x;
    }
}

void sbox(uint64_t state[SBOX_SIZE]) {
    uint64_t x0 = state[0];
    uint64_t x1 = state[1];
    uint64_t x2 = state[2];
    uint64_t x3 = state[3];
    state[0] = x0 ^ (x1 & x2) ^ (x1 & x3) ^ (x2 & x3) ^ (x0 & x2 & x3) ^ (x1 & x2 & x3);
    state[1] = ~x1 ^ x2 ^ (x0 & x2) ^ (x0 & x3) ^ (x2 & x3) ^ (x0 & x1 & x2) ^ (x0 & x2 & x3);
    state[2] = x1 ^ x3 ^ (x0 & x2) ^ (x2 & x3) ^ (x0 & x1 & x3) ^ (x0 & x2 & x3);
    state[3] = x0 ^ x2 ^ x3 ^ (x1 & x3) ^ (x0 & x1 & x2) ^ (x0 & x1 & x3);
}

void diffusion(uint64_t state[SBOX_SIZE]) {
    MIX(state[0], 19, 28);
    MIX(state[1], 61, 39);
    MIX(state[2], 1, 6);
    MIX(state[3], 10, 17);
    uint64_t temp = state[0] ^ state[1] ^ state[2] ^ state[3];
    state[0] ^= temp;
    state[1] ^= temp;
    state[2] ^= temp;
    state[3] ^= temp;
    MIX(state[0], 19, 28);
    MIX(state[1], 61, 39);
    MIX(state[2], 1, 6);
    MIX(state[3], 10, 17);
}

void permutation(unsigned char state[STATE_SIZE]) {
    uint64_t x[SBOX_SIZE];
    load64(x, state + 0);
    load64(x + 1, state + 8);
    load64(x + 2, state + 16);
    load64(x + 3, state + 24);
    for (int i = 0, j = 0; i < ROUNDS; i++, j+= 15) {
        // addition of round constant
        x[1] ^= (0xbe ^ j);
        // substitution layer
        sbox(x);
        // linear diffusion layer
        diffusion(x);
    }
    store64(state + 0, x[0]);
    store64(state + 8, x[1]);
    store64(state + 16, x[2]);
    store64(state + 24, x[3]);
}

int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen) {
    unsigned char state[STATE_SIZE] = {0};
    int remaining = inlen % RATE;
    unsigned long long full_blocks = inlen - remaining;
    for (unsigned long long i = 0; i < full_blocks; i += RATE) {
        for (int j = 0; j < RATE; j++) {
            state[j] ^= in[i + j];
        }
        permutation(state);
    }
    for (int i = 0; i < remaining; i++) {
        state[i] ^= in[full_blocks + i];
    }
    state[remaining] ^= PADDING_BYTE;
    unsigned int output_bytes;
    for (output_bytes = CRYPTO_BYTES; output_bytes >= RATE; output_bytes -= RATE) {
        permutation(state);
        for (int i = 0; i < RATE; i++) {
            out[i] = state[i];
        }
        out += RATE;
    }

    return 0;
}
