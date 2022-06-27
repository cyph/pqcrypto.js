/*
Threshold Implementation of the Lilliput-AE tweakable block cipher.

Authors, hereby denoted as "the implementer":
    Alexandre Adomnicai,
    Kévin Le Gouguec,
    Léo Reynaud,
    2019.

For more information, feedback or questions, refer to our website:
https://paclido.fr/lilliput-ae

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/

---

This file provides a first-order threshold implementation of the Lilliput-AE
tweakable block cipher. The input block is split into 3 shares while the key
is split into 2 shares for the tweakey schedule. The S-box relies on look-up
tables and saves some memory usage at the cost of additional operations as
described in the specification. This implementation operates on 3 shares
throughout the entire round function in order to avoid extra randomness
generation to switch from 2 shares to 3 shares and vice versa.
*/

#include <stdint.h>
#include <string.h>

#include "cipher.h"
#include "constants.h"
#include "random.h"
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

static const uint8_t F[16][16] = {
    {0x0, 0x2, 0x0, 0x2, 0x2, 0x0, 0x2, 0x0, 0x0, 0x2, 0x0, 0x2, 0x2, 0x0, 0x2, 0x0},
    {0x0, 0x2, 0x9, 0xb, 0x3, 0x1, 0xa, 0x8, 0xd, 0xf, 0x4, 0x6, 0xe, 0xc, 0x7, 0x5},
    {0x0, 0xb, 0x0, 0xb, 0xb, 0x0, 0xb, 0x0, 0x1, 0xa, 0x1, 0xa, 0xa, 0x1, 0xa, 0x1},
    {0x9, 0x2, 0x0, 0xb, 0x3, 0x8, 0xa, 0x1, 0x5, 0xe, 0xc, 0x7, 0xf, 0x4, 0x6, 0xd},
    {0x1, 0x2, 0x8, 0xb, 0x3, 0x0, 0xa, 0x9, 0x9, 0xa, 0x0, 0x3, 0xb, 0x8, 0x2, 0x1},
    {0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x3, 0x0, 0x5, 0x6, 0x5, 0x6, 0x6, 0x5, 0x6, 0x5},
    {0x8, 0x2, 0x1, 0xb, 0x3, 0x9, 0xa, 0x0, 0x1, 0xb, 0x8, 0x2, 0xa, 0x0, 0x3, 0x9},
    {0x0, 0xa, 0x0, 0xa, 0xa, 0x0, 0xa, 0x0, 0x4, 0xe, 0x4, 0xe, 0xe, 0x4, 0xe, 0x4},
    {0x1, 0xe, 0x0, 0xf, 0xb, 0x4, 0xa, 0x5, 0x1, 0xe, 0x0, 0xf, 0xb, 0x4, 0xa, 0x5},
    {0xc, 0x3, 0x4, 0xb, 0x7, 0x8, 0xf, 0x0, 0x1, 0xe, 0x9, 0x6, 0xa, 0x5, 0x2, 0xd},
    {0x0, 0x6, 0x1, 0x7, 0x3, 0x5, 0x2, 0x4, 0x1, 0x7, 0x0, 0x6, 0x2, 0x4, 0x3, 0x5},
    {0x4, 0x2, 0xc, 0xa, 0x6, 0x0, 0xe, 0x8, 0x8, 0xe, 0x0, 0x6, 0xa, 0xc, 0x2, 0x4},
    {0x8, 0x6, 0x0, 0xe, 0x2, 0xc, 0xa, 0x4, 0x0, 0xe, 0x8, 0x6, 0xa, 0x4, 0x2, 0xc},
    {0x4, 0xa, 0x5, 0xb, 0xf, 0x1, 0xe, 0x0, 0x1, 0xf, 0x0, 0xe, 0xa, 0x4, 0xb, 0x5},
    {0x0, 0x7, 0x8, 0xf, 0x3, 0x4, 0xb, 0xc, 0x9, 0xe, 0x1, 0x6, 0xa, 0xd, 0x2, 0x5},
    {0x5, 0x2, 0x4, 0x3, 0x7, 0x0, 0x6, 0x1, 0x1, 0x6, 0x0, 0x7, 0x3, 0x4, 0x2, 0x5}
};

static const uint8_t G[4][16] = {
    {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
    {0x0, 0x1, 0x2, 0x3, 0x5, 0x4, 0x7, 0x6, 0x8, 0x9, 0xa, 0xb, 0xd, 0xc, 0xf, 0xe},
    {0x0, 0x1, 0x3, 0x2, 0x4, 0x5, 0x7, 0x6, 0x8, 0x9, 0xb, 0xa, 0xc, 0xd, 0xf, 0xe},
    {0x1, 0x0, 0x2, 0x3, 0x4, 0x5, 0x7, 0x6, 0x9, 0x8, 0xa, 0xb, 0xc, 0xd, 0xf, 0xe}
};

static const uint8_t Q[8][16] = {
    {0x0, 0x4, 0x2, 0x6, 0x8, 0xc, 0xa, 0xe, 0x1, 0x5, 0x3, 0x7, 0x9, 0xd, 0xb, 0xf},
    {0x0, 0x4, 0xa, 0xe, 0x8, 0xc, 0x2, 0x6, 0x3, 0x7, 0x9, 0xd, 0xb, 0xf, 0x1, 0x5},
    {0x0, 0xc, 0x2, 0xe, 0x8, 0x4, 0xa, 0x6, 0x1, 0xd, 0x3, 0xf, 0x9, 0x5, 0xb, 0x7},
    {0x8, 0x4, 0x2, 0xe, 0x0, 0xc, 0xa, 0x6, 0xb, 0x7, 0x1, 0xd, 0x3, 0xf, 0x9, 0x5},
    {0x0, 0x6, 0x2, 0x4, 0x8, 0xe, 0xa, 0xc, 0x1, 0x7, 0x3, 0x5, 0x9, 0xf, 0xb, 0xd},
    {0x2, 0x4, 0x8, 0xe, 0xa, 0xc, 0x0, 0x6, 0x1, 0x7, 0xb, 0xd, 0x9, 0xf, 0x3, 0x5},
    {0x0, 0xe, 0x2, 0xc, 0x8, 0x6, 0xa, 0x4, 0x1, 0xf, 0x3, 0xd, 0x9, 0x7, 0xb, 0x5},
    {0xa, 0x4, 0x0, 0xe, 0x2, 0xc, 0x8, 0x6, 0x9, 0x7, 0x3, 0xd, 0x1, 0xf, 0xb, 0x5}
};

static const uint8_t P[16] = {
    0x0, 0x2, 0x8, 0xa, 0x4, 0X6, 0xc, 0xe, 0x1, 0x3, 0x9, 0xb, 0x5, 0x7, 0xd, 0xf
};

static void _state_init(
    uint8_t X[BLOCK_BYTES],
    uint8_t Y[BLOCK_BYTES],
    uint8_t Z[BLOCK_BYTES],
    const uint8_t message[BLOCK_BYTES]
)
{
    uint8_t SHARES_0[BLOCK_BYTES];
    uint8_t SHARES_1[BLOCK_BYTES];
    randombytes(sizeof(SHARES_0), SHARES_0);
    randombytes(sizeof(SHARES_1), SHARES_1);

    memcpy(X, SHARES_0, BLOCK_BYTES);
    memcpy(Y, SHARES_1, BLOCK_BYTES);
    for (size_t i=0; i<BLOCK_BYTES; i++)
    {
        Z[i] = message[i] ^ SHARES_0[i] ^ SHARES_1[i];
    }
}


static void _compute_round_tweakeys(
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES],
    uint8_t RTK_X[ROUNDS][ROUND_TWEAKEY_BYTES],
    uint8_t RTK_Y[ROUNDS][ROUND_TWEAKEY_BYTES]
)
{
    uint8_t TK_X[TWEAKEY_BYTES];
    uint8_t TK_Y[TWEAKEY_BYTES];
    tweakey_state_init(TK_X, TK_Y, key, tweak);
    tweakey_state_extract(TK_X, TK_Y, 0, RTK_X[0], RTK_Y[0]);

    for (size_t i=1; i<ROUNDS; i++)
    {
        tweakey_state_update(TK_X, TK_Y);
        tweakey_state_extract(TK_X, TK_Y, i, RTK_X[i], RTK_Y[i]);
    }
}


static void _nonlinear_layer(
    uint8_t X[BLOCK_BYTES],
    uint8_t Y[BLOCK_BYTES],
    uint8_t Z[BLOCK_BYTES],
    const uint8_t RTK_X[ROUND_TWEAKEY_BYTES],
    const uint8_t RTK_Y[ROUND_TWEAKEY_BYTES]
)
{
    uint8_t x_hi, y_hi, z_hi;   // High nibbles for the Feistel network
    uint8_t x_lo, y_lo, z_lo;   // Low nibbles for the Feistel network
    uint8_t tmp0, tmp1, tmp2;
    uint8_t TMP_X[ROUND_TWEAKEY_BYTES];
    uint8_t TMP_Y[ROUND_TWEAKEY_BYTES];
    uint8_t TMP_Z[ROUND_TWEAKEY_BYTES];

    // Apply the RTK to two shares
    for (size_t j=0; j<ROUND_TWEAKEY_BYTES; j++)
    {
        TMP_X[j] = X[j] ^ RTK_X[j];
        TMP_Y[j] = Y[j] ^ RTK_Y[j];
    }

    // Threshold Implementation of the 8-bit S-box
    for (size_t j=0; j<ROUND_TWEAKEY_BYTES; j++)
    {
        // Decomposition into nibbles
        x_hi = TMP_X[j] >> 4;
        x_lo = TMP_X[j] & 0xf;
        y_hi = TMP_Y[j] >> 4;
        y_lo = TMP_Y[j] & 0xf;
        z_hi = Z[j] >> 4;
        z_lo = Z[j] & 0xf;
        // First 4-bit S-box
        tmp0 = G[(y_lo&7)>>1][z_lo];
        tmp1 = G[(z_lo&7)>>1][x_lo];
        tmp2 = G[(x_lo&7)>>1][y_lo];
        x_hi ^= F[tmp1][tmp2];
        y_hi ^= F[tmp2][tmp0];
        z_hi ^= F[tmp0][tmp1];
        // Second 4-bit S-box
        tmp0 = P[Q[y_hi&3 ^ (y_hi&8)>>1][z_hi]];
        tmp1 = P[Q[z_hi&3 ^ (z_hi&8)>>1][x_hi]];
        tmp2 = P[Q[x_hi&3 ^ (x_hi&8)>>1][y_hi]];
        x_lo ^= Q[tmp1&3 ^ (tmp1&8)>>1][tmp2];
        y_lo ^= Q[tmp2&3 ^ (tmp2&8)>>1][tmp0];
        z_lo ^= Q[tmp0&3 ^ (tmp0&8)>>1][tmp1];
        // Third 4-bit S-box
        tmp0 = G[(y_lo&7)>>1][z_lo] ^ 1;
        tmp1 = G[(z_lo&7)>>1][x_lo];
        tmp2 = G[(x_lo&7)>>1][y_lo];
        x_hi ^= F[tmp1][tmp2];
        y_hi ^= F[tmp2][tmp0];
        z_hi ^= F[tmp0][tmp1];
        // Build bytes from nibbles
        TMP_X[j] = (x_hi << 4 | x_lo);
        TMP_Y[j] = (y_hi << 4 | y_lo);
        TMP_Z[j] = (z_hi << 4 | z_lo);
    }

    for (size_t j=0; j<8; j++)
    {
        size_t dest_j = 15-j;
        X[dest_j] ^= TMP_X[j];
        Y[dest_j] ^= TMP_Y[j];
        Z[dest_j] ^= TMP_Z[j];
    }
}

static void _linear_layer(uint8_t X[BLOCK_BYTES])
{
    X[15] ^= X[1];
    X[15] ^= X[2];
    X[15] ^= X[3];
    X[15] ^= X[4];
    X[15] ^= X[5];
    X[15] ^= X[6];
    X[15] ^= X[7];

    X[14] ^= X[7];
    X[13] ^= X[7];
    X[12] ^= X[7];
    X[11] ^= X[7];
    X[10] ^= X[7];
    X[9]  ^= X[7];
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

static void _one_round_egfn(
    uint8_t X[BLOCK_BYTES],
    uint8_t Y[BLOCK_BYTES],
    uint8_t Z[BLOCK_BYTES],
    const uint8_t RTK_X[ROUND_TWEAKEY_BYTES],
    const uint8_t RTK_Y[ROUND_TWEAKEY_BYTES],
    permutation p
)
{
    _nonlinear_layer(X, Y, Z, RTK_X, RTK_Y);
    _linear_layer(X);
    _linear_layer(Y);
    _linear_layer(Z);
    _permutation_layer(X, p);
    _permutation_layer(Y, p);
    _permutation_layer(Z, p);
}


void lilliput_tbc_encrypt(
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES],
    const uint8_t message[BLOCK_BYTES],
    uint8_t ciphertext[BLOCK_BYTES]
)
{
    uint8_t X[BLOCK_BYTES];
    uint8_t Y[BLOCK_BYTES];
    uint8_t Z[BLOCK_BYTES];
    _state_init(X, Y, Z, message);

    uint8_t RTK_X[ROUNDS][ROUND_TWEAKEY_BYTES];
    uint8_t RTK_Y[ROUNDS][ROUND_TWEAKEY_BYTES];
    _compute_round_tweakeys(key, tweak, RTK_X, RTK_Y);


    for (size_t i=0; i<ROUNDS-1; i++)
    {
        _one_round_egfn(X, Y, Z, RTK_X[i], RTK_Y[i], PERMUTATION_ENCRYPTION);
    }

    _one_round_egfn(X, Y, Z, RTK_X[ROUNDS-1], RTK_Y[ROUNDS-1], PERMUTATION_NONE);


    for (size_t i=0; i<BLOCK_BYTES; i++)
    {
        ciphertext[i] = X[i] ^ Y[i] ^ Z[i];
    }
}

void lilliput_tbc_decrypt(
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES],
    const uint8_t ciphertext[BLOCK_BYTES],
    uint8_t message[BLOCK_BYTES]
)
{
    uint8_t X[BLOCK_BYTES];
    uint8_t Y[BLOCK_BYTES];
    uint8_t Z[BLOCK_BYTES];
    _state_init(X, Y, Z, ciphertext);

    uint8_t RTK_X[ROUNDS][ROUND_TWEAKEY_BYTES];
    uint8_t RTK_Y[ROUNDS][ROUND_TWEAKEY_BYTES];
    _compute_round_tweakeys(key, tweak, RTK_X, RTK_Y);

    for (size_t i=0; i<ROUNDS-1; i++)
    {
        _one_round_egfn(X, Y, Z, RTK_X[ROUNDS-1-i], RTK_Y[ROUNDS-1-i], PERMUTATION_DECRYPTION);
    }

    _one_round_egfn(X, Y, Z, RTK_X[0], RTK_Y[0], PERMUTATION_NONE);

    for (size_t i=0; i<BLOCK_BYTES; i++)
    {
        message[i] = X[i] ^ Y[i] ^ Z[i];
    }
}
