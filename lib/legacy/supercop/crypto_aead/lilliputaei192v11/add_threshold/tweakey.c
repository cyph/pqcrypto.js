/*
Implementation of the Lilliput-AE tweakable block cipher.

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

This file provides a first-order threshold implementation of Lilliput-TBC's
tweakey schedule, where the tweak and the key are split into two shares.
*/

#include <stdint.h>
#include <string.h>

#include "constants.h"
#include "multiplications.h"
#include "random.h"
#include "tweakey.h"


#define LANES_NB       (TWEAKEY_BYTES/LANE_BYTES)
#define TWEAK_LANES_NB (TWEAK_BYTES/LANE_BYTES)
#define KEY_LANES_NB   (KEY_BYTES/LANE_BYTES)


void tweakey_state_init(
    uint8_t TK_X[TWEAKEY_BYTES],
    uint8_t TK_Y[KEY_BYTES],
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES]
)
{
    uint8_t SHARES_0[KEY_BYTES];
    randombytes(sizeof(SHARES_0), SHARES_0);

    memcpy(TK_Y, SHARES_0, KEY_BYTES);
    memcpy(TK_X, tweak, TWEAK_BYTES);

    for (size_t i=0; i<KEY_BYTES; i++){
        TK_X[i+TWEAK_BYTES] = key[i] ^ SHARES_0[i];
    }
}


void tweakey_state_extract(
    const uint8_t TK_X[TWEAKEY_BYTES],
    const uint8_t TK_Y[KEY_BYTES],
    uint8_t round_constant,
    uint8_t round_tweakey_X[ROUND_TWEAKEY_BYTES],
    uint8_t round_tweakey_Y[ROUND_TWEAKEY_BYTES]
)
{
    memset(round_tweakey_X, 0, ROUND_TWEAKEY_BYTES);
    memset(round_tweakey_Y, 0, ROUND_TWEAKEY_BYTES);

    for (size_t j=0; j<LANES_NB; j++)
    {
        const uint8_t *TKj_X = TK_X + j*LANE_BYTES;

        for (size_t k=0; k<LANE_BYTES; k++)
        {
            round_tweakey_X[k] ^= TKj_X[k];
        }
    }

    for (size_t j=0; j<KEY_LANES_NB; j++)
    {
        const uint8_t *TKj_Y = TK_Y + j*LANE_BYTES;

        for (size_t k=0; k<LANE_BYTES; k++)
        {
            round_tweakey_Y[k] ^= TKj_Y[k];
        }
    }

    round_tweakey_X[0] ^= round_constant;
}


typedef void (*matrix_multiplication)(const uint8_t x[LANE_BYTES], uint8_t y[LANE_BYTES]);

static const matrix_multiplication ALPHAS[7] = {
    _multiply_M,
    _multiply_M2,
    _multiply_M3,
    _multiply_M4,
    _multiply_MR,
    _multiply_MR2,
    _multiply_MR3
};


void tweakey_state_update(uint8_t TK_X[TWEAKEY_BYTES], uint8_t TK_Y[KEY_BYTES])
{
    for (size_t j=0; j<TWEAK_LANES_NB; j++)
    {
        uint8_t *TKj_X = TK_X + j*LANE_BYTES;

        uint8_t TKj_old_X[LANE_BYTES];
        memcpy(TKj_old_X, TKj_X, LANE_BYTES);

        ALPHAS[j](TKj_old_X, TKj_X);
    }

    for (size_t j=0; j<KEY_LANES_NB; j++)
    {
        uint8_t *TKj_X = TK_X + (j + TWEAK_LANES_NB)*LANE_BYTES;
        uint8_t *TKj_Y = TK_Y + j*LANE_BYTES;

        uint8_t TKj_X_old[LANE_BYTES];
        uint8_t TKj_Y_old[LANE_BYTES];
        memcpy(TKj_X_old, TKj_X, LANE_BYTES);
        memcpy(TKj_Y_old, TKj_Y, LANE_BYTES);

        ALPHAS[j + TWEAK_LANES_NB](TKj_X_old, TKj_X);
        ALPHAS[j + TWEAK_LANES_NB](TKj_Y_old, TKj_Y);
    }
}
