/**
 * Romulus-M implementation following the SUPERCOP API.
 * 
 * @author      Alexandre Adomnicai
 *              alex.adomnicai@gmail.com
 * 
 * @date        March 2022
 */
#include "romulus_m.h"
#include "tk_schedule.h"
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
    uint32_t rtk_23[BLOCKBYTES*SKINNY128_384_ROUNDS/4];
    *clen = mlen + TAGBYTES;
    romulusm_init(state, tk1);
    romulusm_process_ad(state, ad, adlen, m, mlen, rtk_23, tk1, npub, k);
    romulusm_generate_tag(c + mlen, state);
    romulusm_process_msg(c, m, mlen, state, rtk_23, tk1, ENCRYPT_MODE);
    zeroize((uint8_t *)rtk_23, sizeof(rtk_23));
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
    uint32_t rtk_23[BLOCKBYTES*SKINNY128_384_ROUNDS/4];

    if (clen < TAGBYTES)
        return -1;

    clen -= TAGBYTES;
    *mlen = clen;
    romulusm_init(state, tk1);
    tk_schedule_23(rtk_23, npub, k);
    romulusm_process_msg(m, c, clen, state, rtk_23, tk1, DECRYPT_MODE);
    romulusm_init(state, tk1);  
    romulusm_process_ad(state, ad, adlen, m, clen, rtk_23, tk1, npub, k);
    zeroize((uint8_t *)rtk_23, sizeof(rtk_23));
    return romulusm_verify_tag(c + *mlen, state);
}
