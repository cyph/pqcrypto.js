/*! @file api.c
 *  @brief Implementation of the NIST/SUPERCOP API.
 *
 *  This file is part of the reference implementation of the Picnic signature scheme.
 *  See the accompanying documentation for complete details.
 *
 *  The code is provided under the MIT license, see LICENSE for
 *  more details.
 *  SPDX-License-Identifier: MIT
 */

#include "picnic.h"

#include "api.h"

#ifdef SUPERCOP
    #include "crypto_sign.h"
#endif

#include <string.h>

//#ifndef htole32
static uint32_t bswap32(uint32_t x) 
{
      return ((x & 0xff000000) >> 24) | ((x & 0x00ff0000) >> 8) | ((x & 0x0000ff00) << 8) |
                   ((x & 0x000000ff) << 24);
}

static int isBigEndianSystem()
{
    uint32_t x = 1;
    uint8_t* xp = (uint8_t*) &x;
    if(xp[3] == 1) {
        return 1;
    }
    return 0;
}

static uint32_t htole32_portable(uint32_t x) 
{
    if(isBigEndianSystem()) {
        return bswap32(x);
    }
    return x;
}
static uint32_t le32toh_portable(uint32_t x) 
{
    if(isBigEndianSystem()) {
        return bswap32(x);
    }
    return x;
}       

//#endif

picnic_params_t params = Picnic3_L3;

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
    picnic_publickey_t pubkey;
    picnic_privatekey_t secret;

    int ret = picnic_keygen(params, &pubkey, &secret);

    if (ret != 0) {
        return ret;
    }

    ret = picnic_write_public_key(&pubkey, pk, CRYPTO_PUBLICKEYBYTES);
    if (ret < 1) {
        return ret;
    }

    ret = picnic_write_private_key(&secret, sk, CRYPTO_SECRETKEYBYTES);
    if (ret < 1) {
        return ret;
    }

    return 0;
}

int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk)
{
    picnic_privatekey_t secret;

    int ret = picnic_read_private_key(&secret, sk, CRYPTO_SECRETKEYBYTES);

    if (ret != 0) {
        return ret;
    }

    /* Write out sm as
     *      4-byte integer signature length | message | signature
     */
    size_t signature_len = CRYPTO_BYTES;
    ret = picnic_sign(&secret, m, mlen, sm + mlen + 4, &signature_len);
    if (ret != 0) {
        return ret;
    }
    if (signature_len > CRYPTO_BYTES) {
        return -1;
    }

    *smlen = 4 + mlen + signature_len;
    signature_len = htole32_portable(signature_len);
    memcpy(sm, (uint8_t*)&signature_len, 4);
    memcpy(sm + 4, m, mlen);

    return 0;
}

int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk)
{
    picnic_publickey_t pubkey;

    int ret = picnic_read_public_key(&pubkey, pk, CRYPTO_PUBLICKEYBYTES);

    if (ret != 0) {
        return -2;
    }

    uint32_t signature_len;
    memcpy((uint8_t*)&signature_len, sm, 4);
    signature_len = le32toh_portable(signature_len);
    if (signature_len > smlen - 1 - 4) {
        return -2;
    }

    size_t message_len = smlen - signature_len - 4;
    ret = picnic_verify(&pubkey, sm + 4, message_len,
                        sm + 4 + message_len, signature_len);
    if (ret != 0) {
        return -1;
    }

    memmove(m, sm + 4, message_len);
    *mlen = message_len;

    return 0;
}

