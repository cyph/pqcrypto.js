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

This file provides bit and byte lengths for input and output parameters.
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "parameters.h"

#define TWEAKEY_LENGTH_BITS       (TWEAK_LENGTH_BITS+KEY_LENGTH_BITS)
#define LANE_BITS                 64
#define ROUND_TWEAKEY_LENGTH_BITS 64
#define BLOCK_LENGTH_BITS         128
#define NONCE_LENGTH_BITS         120
#define TAG_LENGTH_BITS           128

#define TWEAK_BYTES         (TWEAK_LENGTH_BITS/8)
#define KEY_BYTES           (KEY_LENGTH_BITS/8)
#define TWEAKEY_BYTES       (TWEAKEY_LENGTH_BITS/8)
#define LANE_BYTES          (LANE_BITS/8)
#define ROUND_TWEAKEY_BYTES (ROUND_TWEAKEY_LENGTH_BITS/8)
#define BLOCK_BYTES         (BLOCK_LENGTH_BITS/8)
#define NONCE_BYTES         (NONCE_LENGTH_BITS/8)
#define TAG_BYTES           (TAG_LENGTH_BITS/8)

#endif /* CONSTANTS_H */
