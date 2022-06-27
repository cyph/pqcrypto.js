// Yarara-128 implementation
// Target: Generic C99
// Copyright (c) 2019, Miguel Montes <mmontes@iua.edu.ar>
#include <assert.h>
#include <stdint.h>
#include "api.h"
#include "crypto_aead.h"

#define SBOX_SIZE 4
#define STATE_SIZE 32
#define RATE 8
#define PADDING_BYTE 0x80
#define INITIAL_ROUNDS 10
#define ROUNDS 6
#define FINAL_ROUNDS ROUNDS
#define ROTR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))
#define MIX(x, a, b) x ^= ROTR(x, a) ^ ROTR(x, b)

#define initial_permutation(state) permutation(INITIAL_ROUNDS, 0xbeull, state)
#define ad_permutation(state) permutation(ROUNDS, 0xadull, state)
#define m_permutation(state) permutation(ROUNDS, 0xaeull, state)
#define final_permutation(state) permutation(FINAL_ROUNDS, 0xebull, state)

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

void permutation(int rounds, uint64_t rconst, unsigned char state[STATE_SIZE]) {
    uint64_t x[SBOX_SIZE];
    load64(x, state + 0);
    load64(x + 1, state + 8);
    load64(x + 2, state + 16);
    load64(x + 3, state + 24);
    for (int i = 0, j = 0; i < rounds; i++, j+=15) {
        // addition of round constant
        x[1] ^= (rconst ^ j);
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

void initialize(
    unsigned char state[STATE_SIZE],
    const unsigned char *k,
    const unsigned char *npub) {
    assert(CRYPTO_KEYBYTES + CRYPTO_NPUBBYTES <= STATE_SIZE);
    for (int i = 0; i < CRYPTO_KEYBYTES; i++) {
        state[i] = k[i];
    }
    for (int i = 0; i < CRYPTO_NPUBBYTES; i++) {
        state[CRYPTO_KEYBYTES + i] = npub[i];
    }
    initial_permutation(state);
}

void process_ad(
    unsigned char state[STATE_SIZE],
    const unsigned char *ad,
    unsigned long long adlen) {
    if (adlen) {
        int remaining = adlen % RATE;
        unsigned long long full_blocks = adlen - remaining;
        for (unsigned long long i = 0; i < full_blocks; i += RATE) {
            for (int j = 0; j < RATE; j++) {
                state[j] ^= ad[i + j];
            }
            ad_permutation(state);
        }
        for (int i = 0; i < remaining; i++) {
            state[i] ^= ad[full_blocks + i];
        }
        state[remaining] ^= PADDING_BYTE;
        ad_permutation(state);
    }
    state[STATE_SIZE-1] ^= 1;
}

void encrypt(
    unsigned char state[STATE_SIZE],
    unsigned char *c,
    const unsigned char *m,
    unsigned long long mlen) {
    int remaining = mlen % RATE;
    unsigned long long full_blocks = mlen - remaining;
    for (unsigned long long i = 0; i < full_blocks; i += RATE) {
        for (int j = 0; j < RATE; j++) {
            state[j] ^= m[i + j];
            c[i + j] = state[j];
        }
        m_permutation(state);
    }
    for (int j = 0; j < remaining; j++) {
        state[j] ^= m[full_blocks + j];
        c[full_blocks + j] = state[j];
    }
    state[remaining] ^= PADDING_BYTE;
    m_permutation(state);
}

void decrypt(
    unsigned char state[STATE_SIZE],
    unsigned char *m,
    const unsigned char *c,
    unsigned long long mlen) {
    int remaining = mlen % RATE;
    unsigned long long full_blocks = mlen - remaining;
    for (unsigned long long i = 0; i < full_blocks; i += RATE) {
        for (int j = 0; j < RATE; j++) {
            m[i + j] = c[i + j] ^ state[j];
            state[j] = c[i + j];
        }
        m_permutation(state);
    }
    for (int j = 0; j < remaining; j++) {
        m[full_blocks + j] = c[full_blocks + j] ^ state[j];
        state[j] = c[full_blocks + j];
    }
    state[remaining] ^= PADDING_BYTE;
    m_permutation(state);
}

void finalize(
    unsigned char* restrict state,
    unsigned char* restrict tag) {
    int l = CRYPTO_ABYTES;
    int i = 0;
    while (l >= RATE) {
        final_permutation(state);
        for (int j = 0; j < RATE; j++) {
            tag[i++] = state[j];
        }
        l -= RATE;
    }
    if (l > 0) {
        final_permutation(state);
        for (int j = 0; j < l; j++) {
            tag[i++] = state[j];
        }
    }
}

int crypto_aead_encrypt(
    unsigned char *c, unsigned long long *clen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k) {
    (void)nsec;
    unsigned char state[STATE_SIZE];

    initialize(state, k, npub);
    process_ad(state, ad, adlen);
    encrypt(state, c, m, mlen);
    finalize(state, c + mlen);
    *clen = mlen + CRYPTO_ABYTES;
    return 0;
}

int crypto_aead_decrypt(
    unsigned char *m, unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k) {
    (void)nsec;
    unsigned char tag[CRYPTO_ABYTES];
    *mlen = 0;
    if (clen < CRYPTO_ABYTES)
        return -1;
    unsigned char state[STATE_SIZE];

    initialize(state, k, npub);
    process_ad(state, ad, adlen);
    decrypt(state, m, c, clen - CRYPTO_ABYTES);
    finalize(state, tag);
    *mlen = clen - CRYPTO_ABYTES;
    int result = 0;
    for (int j = 0; j < CRYPTO_ABYTES; j++) {
        result |= tag[j] ^ c[(*mlen) + j];
    }
    return result ? -1 : 0;
}
