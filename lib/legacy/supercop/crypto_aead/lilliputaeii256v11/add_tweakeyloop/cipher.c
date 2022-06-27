/*
Implementation of the Lilliput-AE tweakable block cipher.

Authors, hereby denoted as "the implementer":
    Kévin Le Gouguec,
    2019.

For more information, feedback or questions, refer to our website:
https://paclido.fr/lilliput-ae

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/

---

This file provides the implementation for Lilliput-TBC.
*/

#include <stdint.h>
#include <string.h>

#include "cipher.h"
#include "constants.h"
#include "tweakey.h"


enum permutation
{
    PERMUTATION_ENCRYPTION = 0, /* PI(i) */
    PERMUTATION_DECRYPTION = 1, /* PI^-1(i) */
    PERMUTATION_NONE
};

typedef enum permutation permutation;

static const uint8_t PERMUTATIONS[2][BLOCK_BYTES] = {
    [PERMUTATION_ENCRYPTION] = { 13,  9, 14,  8, 10, 11, 12, 15,  4,  5,  3,  1,  2,  6,  0,  7 },
    [PERMUTATION_DECRYPTION] = { 14, 11, 12, 10,  8,  9, 13, 15,  3,  1,  4,  5,  6,  0,  2,  7 }
};

static const uint8_t S[256] = {
    0x20, 0x00, 0xB2, 0x85, 0x3B, 0x35, 0xA6, 0xA4, 0x30, 0xE4, 0x6A, 0x2C, 0xFF, 0x59, 0xE2, 0x0E,
    0xF8, 0x1E, 0x7A, 0x80, 0x15, 0xBD, 0x3E, 0xB1, 0xE8, 0xF3, 0xA2, 0xC2, 0xDA, 0x51, 0x2A, 0x10,
    0x21, 0x01, 0x23, 0x78, 0x5C, 0x24, 0x27, 0xB5, 0x37, 0xC7, 0x2B, 0x1F, 0xAE, 0x0A, 0x77, 0x5F,
    0x6F, 0x09, 0x9D, 0x81, 0x04, 0x5A, 0x29, 0xDC, 0x39, 0x9C, 0x05, 0x57, 0x97, 0x74, 0x79, 0x17,
    0x44, 0xC6, 0xE6, 0xE9, 0xDD, 0x41, 0xF2, 0x8A, 0x54, 0xCA, 0x6E, 0x4A, 0xE1, 0xAD, 0xB6, 0x88,
    0x1C, 0x98, 0x7E, 0xCE, 0x63, 0x49, 0x3A, 0x5D, 0x0C, 0xEF, 0xF6, 0x34, 0x56, 0x25, 0x2E, 0xD6,
    0x67, 0x75, 0x55, 0x76, 0xB8, 0xD2, 0x61, 0xD9, 0x71, 0x8B, 0xCD, 0x0B, 0x72, 0x6C, 0x31, 0x4B,
    0x69, 0xFD, 0x7B, 0x6D, 0x60, 0x3C, 0x2F, 0x62, 0x3F, 0x22, 0x73, 0x13, 0xC9, 0x82, 0x7F, 0x53,
    0x32, 0x12, 0xA0, 0x7C, 0x02, 0x87, 0x84, 0x86, 0x93, 0x4E, 0x68, 0x46, 0x8D, 0xC3, 0xDB, 0xEC,
    0x9B, 0xB7, 0x89, 0x92, 0xA7, 0xBE, 0x3D, 0xD8, 0xEA, 0x50, 0x91, 0xF1, 0x33, 0x38, 0xE0, 0xA9,
    0xA3, 0x83, 0xA1, 0x1B, 0xCF, 0x06, 0x95, 0x07, 0x9E, 0xED, 0xB9, 0xF5, 0x4C, 0xC0, 0xF4, 0x2D,
    0x16, 0xFA, 0xB4, 0x03, 0x26, 0xB3, 0x90, 0x4F, 0xAB, 0x65, 0xFC, 0xFE, 0x14, 0xF7, 0xE3, 0x94,
    0xEE, 0xAC, 0x8C, 0x1A, 0xDE, 0xCB, 0x28, 0x40, 0x7D, 0xC8, 0xC4, 0x48, 0x6B, 0xDF, 0xA5, 0x52,
    0xE5, 0xFB, 0xD7, 0x64, 0xF9, 0xF0, 0xD3, 0x5E, 0x66, 0x96, 0x8F, 0x1D, 0x45, 0x36, 0xCC, 0xC5,
    0x4D, 0x9F, 0xBF, 0x0F, 0xD1, 0x08, 0xEB, 0x43, 0x42, 0x19, 0xE7, 0x99, 0xA8, 0x8E, 0x58, 0xC1,
    0x9A, 0xD4, 0x18, 0x47, 0xAA, 0xAF, 0xBC, 0x5B, 0xD5, 0x11, 0xD0, 0xB0, 0x70, 0xBB, 0x0D, 0xBA
};


static void _state_init(uint8_t X[BLOCK_BYTES], const uint8_t message[BLOCK_BYTES])
{
    memcpy(X, message, BLOCK_BYTES);
}


static void _compute_round_tweakeys(
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES],
    uint8_t RTK[ROUNDS][ROUND_TWEAKEY_BYTES]
)
{
    uint8_t TK[TWEAKEY_BYTES];
    tweakey_state_init(TK, key, tweak);
    tweakey_state_extract(TK, 0, RTK[0]);

    for (size_t i=1; i<ROUNDS; i++)
    {
        tweakey_state_update(TK);
        tweakey_state_extract(TK, i, RTK[i]);
    }
}


static uint8_t _Fj(uint8_t Xj, uint8_t RTKj)
{
    return S[Xj ^ RTKj];
}

static void _nonlinear_layer(uint8_t X[BLOCK_BYTES], const uint8_t RTK[ROUND_TWEAKEY_BYTES])
{
    for (size_t j=0; j<8; j++)
    {
        X[15-j] ^= _Fj(X[j], RTK[j]);
    }
}

static void _linear_layer(uint8_t X[BLOCK_BYTES])
{
    for (size_t j=1; j<8; j++)
    {
        X[15] ^= X[j];
    }

    for (size_t j=14; j>8; j--)
    {
        X[j] ^= X[7];
    }
}

static void _permutation_layer(uint8_t X[BLOCK_BYTES], permutation p)
{
    if (p == PERMUTATION_NONE)
    {
        return;
    }

    uint8_t X_old[BLOCK_BYTES];
    memcpy(X_old, X, BLOCK_BYTES);

    const uint8_t *pi = PERMUTATIONS[p];

    for (size_t j=0; j<BLOCK_BYTES; j++)
    {
        X[pi[j]] = X_old[j];
    }
}

static void _one_round_egfn(uint8_t X[BLOCK_BYTES], const uint8_t RTK[ROUND_TWEAKEY_BYTES], permutation p)
{
    _nonlinear_layer(X, RTK);
    _linear_layer(X);
    _permutation_layer(X, p);
}


void lilliput_tbc_encrypt(
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES],
    const uint8_t message[BLOCK_BYTES],
    uint8_t ciphertext[BLOCK_BYTES]
)
{
    uint8_t X[BLOCK_BYTES];
    _state_init(X, message);

    uint8_t RTK[ROUNDS][ROUND_TWEAKEY_BYTES];
    _compute_round_tweakeys(key, tweak, RTK);

    for (size_t i=0; i<ROUNDS-1; i++)
    {
        _one_round_egfn(X, RTK[i], PERMUTATION_ENCRYPTION);
    }

    _one_round_egfn(X, RTK[ROUNDS-1], PERMUTATION_NONE);

    memcpy(ciphertext, X, BLOCK_BYTES);
}

void lilliput_tbc_decrypt(
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES],
    const uint8_t ciphertext[BLOCK_BYTES],
    uint8_t message[BLOCK_BYTES]
)
{
    uint8_t X[BLOCK_BYTES];
    _state_init(X, ciphertext);

    uint8_t RTK[ROUNDS][ROUND_TWEAKEY_BYTES];
    _compute_round_tweakeys(key, tweak, RTK);

    for (size_t i=0; i<ROUNDS-1; i++)
    {
        _one_round_egfn(X, RTK[ROUNDS-1-i], PERMUTATION_DECRYPTION);
    }

    _one_round_egfn(X, RTK[0], PERMUTATION_NONE);

    memcpy(message, X, BLOCK_BYTES);
}
