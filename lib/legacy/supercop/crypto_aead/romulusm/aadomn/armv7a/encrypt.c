/**
 * Romulus-M implementation following the SUPERCOP API.
 * 
 * @author      Alexandre Adomnicai
 *              alex.adomnicai@gmail.com
 * 
 * @date        March 2022
 */
#include "romulus_m.h"
#include "crypto_aead.h"


//Encryption and authentication using Romulus-M
int crypto_aead_encrypt
    (unsigned char *c, unsigned long long *clen,
     const unsigned char *m, unsigned long long mlen,
     const unsigned char *ad, unsigned long long adlen,
     const unsigned char *nsec,
     const unsigned char *npub,
     const unsigned char *k)
{
    (void)nsec;
    uint8_t state[BLOCKBYTES];
    uint8_t tk1[TWEAKEYBYTES];
    *clen = mlen + TAGBYTES;
    romulusm_init(state, tk1);
    romulusm_process_ad(state, ad, adlen, m, mlen, tk1, npub, k);
    romulusm_generate_tag(c + mlen, state);
    romulusm_process_msg(c, m, mlen, state, tk1, npub, k, ENCRYPT_MODE);
    return 0;
}

//Decryption and tag verification using Romulus-M
int crypto_aead_decrypt
    (unsigned char *m, unsigned long long *mlen,
     unsigned char *nsec,
     const unsigned char *c, unsigned long long clen,
     const unsigned char *ad, unsigned long long adlen,
     const unsigned char *npub,
     const unsigned char *k)
{
    (void)nsec;
    uint8_t tk1[TWEAKEYBYTES];
    uint8_t state[BLOCKBYTES];

    if (clen < TAGBYTES)
        return -1;

    clen -= TAGBYTES;
    *mlen = clen;
    romulusm_init(state, tk1);
    romulusm_process_msg(m, c, clen, state, tk1, npub, k, DECRYPT_MODE);
    romulusm_init(state, tk1);  
    romulusm_process_ad(state, ad, adlen, m, clen, tk1, npub, k);
    return romulusm_verify_tag(c + *mlen, state);
}
