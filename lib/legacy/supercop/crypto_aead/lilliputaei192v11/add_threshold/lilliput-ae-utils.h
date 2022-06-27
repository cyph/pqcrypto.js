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

This file provides functions used by both authenticated encryption modes.
*/

#ifndef LILLIPUT_AE_UTILS_H
#define LILLIPUT_AE_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "cipher.h"
#include "constants.h"


static inline void encrypt(const uint8_t K[KEY_BYTES],
                           const uint8_t T[TWEAK_BYTES],
                           const uint8_t M[BLOCK_BYTES],
                           uint8_t C[BLOCK_BYTES])
{
    lilliput_tbc_encrypt(K, T, M, C);
}

static inline void decrypt(const uint8_t K[KEY_BYTES],
                           const uint8_t T[TWEAK_BYTES],
                           const uint8_t C[BLOCK_BYTES],
                           uint8_t M[BLOCK_BYTES])
{
    lilliput_tbc_decrypt(K, T, C, M);
}

static inline void xor_into(uint8_t dest[BLOCK_BYTES], const uint8_t src[BLOCK_BYTES])
{
    for (size_t i=0; i<BLOCK_BYTES; i++)
        dest[i] ^= src[i];
}

static inline void xor_arrays(size_t len, uint8_t out[len], const uint8_t a[len], const uint8_t b[len])
{
    for (size_t i=0; i<len; i++)
        out[i] = a[i] ^ b[i];
}

static inline void pad10(size_t X_len, const uint8_t X[X_len], uint8_t padded[BLOCK_BYTES])
{
    /* Assuming 0 < |X| < n:
     *
     * pad10*(X) = X || 1 || 0^{n-|X|-1}
     *
     * For example, with uint8_t X[3] = { [0]=0x01, [1]=0x02, [2]=0x03 }
     *
     * pad10*(X) =
     *       X[0]     X[1]     X[2]   1 0*
     *     00000001 00000010 00000011 1 0000000 00000000...
     *
     * - padded[0, 2]:  X[0, 2]
     * - padded[3]:     10000000
     * - padded[4, 15]: zeroes
     */

    memcpy(padded, X, X_len);
    padded[X_len] = 0x80;

    /* memset(&padded[BLOCK_BYTES], 0, 0) may or may not constitute
     * undefined behaviour; use a straight loop instead. */

    for (size_t i=X_len+1; i<BLOCK_BYTES; i++)
    {
        padded[i] = 0;
    }
}

static inline void copy_block_index(size_t index, uint8_t tweak[TWEAK_BYTES])
{
    size_t s = sizeof(index);
    uint8_t *dest = &tweak[TWEAK_BYTES-s];

    for (size_t i=0; i<s; i++)
    {
        dest[i] = index >> 8*(s-1-i);
    }
}

static inline void fill_index_tweak(
    uint8_t prefix,
    size_t  block_index,
    uint8_t tweak[TWEAK_BYTES]
)
{
    /* The t-bit tweak is filled as follows:
     *
     *   1    4    5         t
     * [ prefix || block index ]
     *
     * The s-bit block index is encoded as follows:
     *
     *   5        t-s    t-s+1                t
     * [ zero padding || block index, MSB first ]
     */

    tweak[0] = prefix<<4;

    /* Assume padding bytes have already been set to 0. */

    copy_block_index(block_index, tweak);
}

static void process_associated_data(
    const uint8_t key[KEY_BYTES],
    size_t        A_len,
    const uint8_t A[A_len],
    uint8_t       Auth[BLOCK_BYTES]
)
{
    uint8_t Ek_Ai[BLOCK_BYTES];
    uint8_t tweak[TWEAK_BYTES];

    memset(tweak, 0, TWEAK_BYTES);
    memset(Auth, 0, BLOCK_BYTES);

    size_t l_a = A_len / BLOCK_BYTES;
    size_t rest = A_len % BLOCK_BYTES;

    for (size_t i=0; i<l_a; i++)
    {
        fill_index_tweak(0x2, i, tweak);
        encrypt(key, tweak, &A[i*BLOCK_BYTES], Ek_Ai);
        xor_into(Auth, Ek_Ai);
    }

    if (rest != 0)
    {
        uint8_t A_rest[BLOCK_BYTES];
        pad10(rest, &A[l_a*BLOCK_BYTES], A_rest);
        fill_index_tweak(0x6, l_a, tweak);
        encrypt(key, tweak, A_rest, Ek_Ai);
        xor_into(Auth, Ek_Ai);
    }
}


#endif /* LILLIPUT_AE_UTILS_H */
