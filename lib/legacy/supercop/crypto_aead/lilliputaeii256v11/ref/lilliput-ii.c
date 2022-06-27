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

This file implements Lilliput-AE's nonce-misuse-resistant mode based on SCT-2.
*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cipher.h"
#include "lilliput-ae.h"
#include "lilliput-ae-utils.h"


static void _init_msg_tweak(const uint8_t tag[TAG_BYTES], uint8_t tweak[TWEAK_BYTES])
{
    /* The t-bit tweak is filled as follows:
     *
     *   1    2                      t
     * [ 1 || tag[2,t] XOR block index  ]
     *
     * The s-bit block index is XORed to the tag as follows:
     *
     *   2       t-s    t-s+1                                  t
     * [ tag[2, t-s] || tag[t-s+1, t] XOR block index, MSB first ]
     *
     * This function sets bits 1 to t-s once and for all.
     */

    memcpy(tweak, tag, TAG_BYTES-sizeof(size_t));
    tweak[0] |= 0x80;
}

static void _fill_msg_tweak(const uint8_t tag[TAG_BYTES], size_t block_index, uint8_t tweak[TWEAK_BYTES])
{
    /* The t-bit tweak is filled as follows:
     *
     *   1    2                      t
     * [ 1 || tag[2,t] XOR block index  ]
     *
     * The s-bit block index is XORed to the tag as follows:
     *
     *   2       t-s    t-s+1                                  t
     * [ tag[2, t-s] || tag[t-s+1, t] XOR block index, MSB first ]
     *
     * This function assumes bits 1 to t-s have already been set, and
     * only sets bits t-s+1 to t.
     */

    copy_block_index(block_index, tweak);

    for (size_t i=TWEAK_BYTES-sizeof(size_t); i<TWEAK_BYTES; i++)
    {
        tweak[i] ^= tag[i];
    }
}

static void _fill_tag_tweak(const uint8_t N[NONCE_BYTES], uint8_t tweak[TWEAK_BYTES])
{
    /* The t-bit tweak is filled as follows:
     *
     *   1  4    5   8    t-|N|+1     t
     * [ 0001 ||  0^4  ||        nonce  ]
     */

    tweak[0] = 0x10;
    memcpy(&tweak[1], N, TWEAK_BYTES-1);
}

static void _generate_tag(
    const uint8_t key[KEY_BYTES],
    size_t        M_len,
    const uint8_t M[M_len],
    const uint8_t N[NONCE_BYTES],
    const uint8_t Auth[BLOCK_BYTES],
    uint8_t       tag[TAG_BYTES]
)
{
    uint8_t Ek_Mj[BLOCK_BYTES];
    uint8_t tag_tmp[TAG_BYTES];
    uint8_t tweak[TWEAK_BYTES];

    memset(tweak, 0, TWEAK_BYTES);
    memcpy(tag_tmp, Auth, TAG_BYTES);

    size_t l = M_len / BLOCK_BYTES;
    size_t rest = M_len % BLOCK_BYTES;

    for (size_t j=0; j<l; j++)
    {
        fill_index_tweak(0x0, j, tweak);
        encrypt(key, tweak, &M[j*BLOCK_BYTES], Ek_Mj);
        xor_into(tag_tmp, Ek_Mj);
    }

    if (rest != 0)
    {
        uint8_t M_rest[BLOCK_BYTES];
        pad10(rest, &M[l*BLOCK_BYTES], M_rest);
        fill_index_tweak(0x4, l, tweak);
        encrypt(key, tweak, M_rest, Ek_Mj);
        xor_into(tag_tmp, Ek_Mj);
    }

    _fill_tag_tweak(N, tweak);
    encrypt(key, tweak, tag_tmp, tag);
}

static void _encrypt_message(
    const uint8_t key[KEY_BYTES],
    size_t        M_len,
    const uint8_t M[M_len],
    const uint8_t N[NONCE_BYTES],
    const uint8_t tag[TAG_BYTES],
    uint8_t       C[M_len]
)
{
    uint8_t Ek_N[BLOCK_BYTES];

    uint8_t tweak[TWEAK_BYTES];
    _init_msg_tweak(tag, tweak);

    uint8_t padded_N[BLOCK_BYTES];
    padded_N[0] = 0;
    memcpy(&padded_N[1], N, NONCE_BYTES);

    size_t l = M_len / BLOCK_BYTES;
    size_t rest = M_len % BLOCK_BYTES;

    for (size_t j=0; j<l; j++)
    {
        _fill_msg_tweak(tag, j, tweak);
        encrypt(key, tweak, padded_N, Ek_N);
        xor_arrays(BLOCK_BYTES, &C[j*BLOCK_BYTES], &M[j*BLOCK_BYTES], Ek_N);
    }

    if (rest != 0)
    {
        _fill_msg_tweak(tag, l, tweak);
        encrypt(key, tweak, padded_N, Ek_N);
        xor_arrays(rest, &C[l*BLOCK_BYTES], &M[l*BLOCK_BYTES], Ek_N);
    }
}

void lilliput_ae_encrypt(
    size_t        message_len,
    const uint8_t message[message_len],
    size_t        auth_data_len,
    const uint8_t auth_data[auth_data_len],
    const uint8_t key[KEY_BYTES],
    const uint8_t nonce[NONCE_BYTES],
    uint8_t       ciphertext[message_len],
    uint8_t       tag[TAG_BYTES]
)
{
    uint8_t auth[BLOCK_BYTES];
    process_associated_data(key, auth_data_len, auth_data, auth);

    _generate_tag(key, message_len, message, nonce, auth, tag);

    _encrypt_message(key, message_len, message, nonce, tag, ciphertext);
}

bool lilliput_ae_decrypt(
    size_t        ciphertext_len,
    const uint8_t ciphertext[ciphertext_len],
    size_t        auth_data_len,
    const uint8_t auth_data[auth_data_len],
    const uint8_t key[KEY_BYTES],
    const uint8_t nonce[NONCE_BYTES],
    const uint8_t tag[TAG_BYTES],
    uint8_t       message[ciphertext_len]
)
{
    _encrypt_message(key, ciphertext_len, ciphertext, nonce, tag, message);

    uint8_t auth[BLOCK_BYTES];
    process_associated_data(key, auth_data_len, auth_data, auth);

    uint8_t effective_tag[TAG_BYTES];
    _generate_tag(key, ciphertext_len, message, nonce, auth, effective_tag);

    return memcmp(tag, effective_tag, TAG_BYTES) == 0;
}
