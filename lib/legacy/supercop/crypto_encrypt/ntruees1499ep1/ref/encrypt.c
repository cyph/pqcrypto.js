#include <stdio.h>
#include "ntru_crypto.h"
#include "randombytes.h"
#include "sizes.h"

#define EBATS_PARAM_SET_ID NTRU_EES1499EP1

int copyrightclaims(void)
{
    return 0;
}

int patentclaims(void)
{
    return 30;
}

int randombytesfn(unsigned char *x, unsigned long long xlen)
{
    randombytes(x, xlen);
    DRBG_RET(DRBG_OK);
}

int crypto_encrypt(
    unsigned char *c, unsigned long long *clen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *pk)
{

    uint16_t ct_len = 0;
    DRBG_HANDLE drbg;

    if (ntru_crypto_external_drbg_instantiate((RANDOM_BYTES_FN)&randombytesfn, &drbg) != DRBG_OK)
    {
        return -1;
    }

    if (ntru_crypto_ntru_encrypt(drbg, (uint16_t)PUBLICKEY_BYTES, pk, (uint16_t)mlen, m, &ct_len, NULL) != NTRU_OK)
    {
        return -1;
    }

    if (ntru_crypto_ntru_encrypt(drbg, (uint16_t)PUBLICKEY_BYTES, pk, (uint16_t)mlen, m, &ct_len, c) != NTRU_OK)
    {
        return -1;
    }

    *clen = (uint64_t)ct_len;

    if (ntru_crypto_drbg_uninstantiate(drbg) != DRBG_OK)
    {
        return -1;
    }

    return 0;
}

int crypto_encrypt_open(
    unsigned char *m, unsigned long long *mlen,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *sk)
{

    uint16_t pt_len = 0;

    if (!sk || !c || !m || !mlen)
    {
        return -1;
    }

    if (ntru_crypto_ntru_decrypt((uint16_t)SECRETKEY_BYTES, sk, (uint16_t)clen, c, &pt_len, NULL) != NTRU_OK)
    {
        return -1;
    }

    if (ntru_crypto_ntru_decrypt((uint16_t)SECRETKEY_BYTES, sk, (uint16_t)clen, c, &pt_len, m) != NTRU_OK)
    {
        return -1;
    }

    *mlen = (uint64_t)pt_len;

    return 0;
}

int crypto_encrypt_keypair(
    unsigned char *pk,
    unsigned char *sk)
{
    NTRU_ENCRYPT_PARAM_SET_ID param_set_id = EBATS_PARAM_SET_ID;
    uint16_t pk_len = 0;
    uint16_t sk_len = 0;
    DRBG_HANDLE drbg;

    if (!sk || !pk)
    {
        return -1;
    }

    if (ntru_crypto_external_drbg_instantiate((RANDOM_BYTES_FN)&randombytesfn, &drbg) != DRBG_OK)
    {
        return -1;
    }

    if (ntru_crypto_ntru_encrypt_keygen(drbg, param_set_id, &pk_len, NULL, &sk_len, NULL) != NTRU_OK)
    {
        return -1;
    }

    if (ntru_crypto_ntru_encrypt_keygen(drbg, param_set_id, &pk_len, pk, &sk_len, sk) != NTRU_OK)
    {
        return -1;
    }

    if (ntru_crypto_drbg_uninstantiate(drbg) != DRBG_OK)
    {
        return -1;
    }

    return 0;
}
