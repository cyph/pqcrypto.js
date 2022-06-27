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

This file provides the interface for the first-order threshold implementation
of Lilliput-TBC's tweakey schedule.
*/

#ifndef TWEAKEY_H
#define TWEAKEY_H

#include <stdint.h>

#include "constants.h"


void tweakey_state_init(
    uint8_t TK_X[TWEAKEY_BYTES],
    uint8_t TK_Y[TWEAKEY_BYTES],
    const uint8_t key[KEY_BYTES],
    const uint8_t tweak[TWEAK_BYTES]
);

void tweakey_state_extract(
    const uint8_t TK_X[TWEAKEY_BYTES],
    const uint8_t TK_Y[KEY_BYTES],
    uint8_t round_constant,
    uint8_t round_tweakey_X[ROUND_TWEAKEY_BYTES],
    uint8_t round_tweakey_Y[ROUND_TWEAKEY_BYTES]
);

void tweakey_state_update(uint8_t TK_X[TWEAKEY_BYTES], uint8_t TK_Y[KEY_BYTES]);


#endif /* TWEAKEY_H */
