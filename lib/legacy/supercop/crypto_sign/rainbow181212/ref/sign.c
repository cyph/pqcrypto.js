#include "crypto_sign.h"
#include "Compose.h"
#include "size.h"

// TODO: rand() source

int crypto_sign_keypair(
    unsigned char *pk,
    unsigned char *sk)
{
    unsigned long sklen;
    unsigned long pklen;

    return keypair(sk, &sklen, pk, &pklen);
}

int crypto_sign(
    unsigned char *sm, unsigned long long *smlen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *sk)
{
    unsigned long sklen = SECRETKEY_BYTES;
    int res = signedshortmessage(sm, &smlen, m, mlen, sk, sklen);
    if (res < 0)
    {
        return -1;
    }
    return 0;
}

int crypto_sign_open(
    unsigned char *m, unsigned long long *mlen,
    const unsigned char *sm, unsigned long long smlen,
    const unsigned char *pk)
{
    unsigned long pklen = PUBLICKEY_BYTES;
    int res = shortmessagesigned(m, &mlen, sm, smlen, pk, pklen);
    if (res < 0)
    {
        return -1;
    }
    return 0;
}
