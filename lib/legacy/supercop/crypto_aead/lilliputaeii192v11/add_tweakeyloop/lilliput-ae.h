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

This file provides the interface for both Lilliput-I and Lilliput-II,
implemented by lilliput-i.c and lilliput-ii.c respectively.
*/

#ifndef LILLIPUT_AE_H
#define LILLIPUT_AE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "constants.h"


void lilliput_ae_encrypt(
    size_t        message_len,
    const uint8_t message[message_len],
    size_t        auth_data_len,
    const uint8_t auth_data[auth_data_len],
    const uint8_t key[KEY_BYTES],
    const uint8_t nonce[NONCE_BYTES],
    uint8_t       ciphertext[message_len],
    uint8_t       tag[TAG_BYTES]
);

bool lilliput_ae_decrypt(
    size_t        ciphertext_len,
    const uint8_t ciphertext[ciphertext_len],
    size_t        auth_data_len,
    const uint8_t auth_data[auth_data_len],
    const uint8_t key[KEY_BYTES],
    const uint8_t nonce[NONCE_BYTES],
    const uint8_t tag[TAG_BYTES],
    uint8_t       message[ciphertext_len]
);


#endif /* LILLIPUT_AE_H */
