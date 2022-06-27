#include <stdbool.h>

#include "crypto_aead.h"
#include "lilliput-ae.h"


int crypto_aead_encrypt(
    unsigned char *c, unsigned long long *clen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k
)
{
    (void)nsec;

    lilliput_ae_encrypt(mlen, m, adlen, ad, k, npub, c, c+mlen);
    *clen = mlen + TAG_BYTES;

    return 0;
}


int crypto_aead_decrypt(
    unsigned char *m, unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k
)
{
    (void)nsec;

    size_t tagless_len = clen-TAG_BYTES;

    bool valid = lilliput_ae_decrypt(
        tagless_len, c, adlen, ad, k, npub, c+tagless_len, m
    );

    if (!valid)
        return -1;

    *mlen = tagless_len;

    return 0;
}
