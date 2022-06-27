/**
 * Romulus-T core functions.
 * 
 * @author      Alexandre Adomnicai
 *              alex.adomnicai@gmail.com
 * 
 * @date        March 2022
 */
#include "romulus_t.h"
#include "crypto_aead.h"

//Encryption and authentication using Romulus-T
int crypto_aead_encrypt(
  unsigned char *c, unsigned long long *clen,
  const unsigned char *m, unsigned long long mlen,
  const unsigned char *ad, unsigned long long adlen,
  const unsigned char *nsec,
  const unsigned char *npub,
  const unsigned char *k)
{
    (void)nsec;
    uint8_t tk1[BLOCKBYTES];
    uint8_t state[BLOCKBYTES];
    *clen = mlen + TAGBYTES;
    romulust_init(state, tk1);
    romulust_kdf(state, tk1, npub, k);
    romulust_process_msg(state, tk1, npub, c, m, mlen);
    romulust_generate_tag(c+mlen, tk1, ad, adlen, c, mlen, npub, k);
    return 0;
}

//Encryption and authentication using Romulus-T
int crypto_aead_decrypt(
  unsigned char *m, unsigned long long *mlen,
  unsigned char *nsec,
  const unsigned char *c, unsigned long long clen,
  const unsigned char *ad, unsigned long long adlen,
  const unsigned char *npub,
  const unsigned char *k)
{
    (void)nsec;
    uint8_t tk1[BLOCKBYTES];
    uint8_t state[BLOCKBYTES];
    uint8_t tmp = 0x00;
    // tag verification
    if (clen < TAGBYTES)
      return -1;
    *mlen = clen - TAGBYTES;
    romulust_generate_tag(state, tk1, ad, adlen, c, *mlen, npub, k);
    for(int i = 0; i < TAGBYTES; i++)
        tmp |= state[i] ^ c[clen-TAGBYTES+i];   //constant-time tag comparison
    if (tmp)
      return -1;
    // decryption
    romulust_init(state, tk1);
    romulust_kdf(state, tk1, npub, k);
    romulust_process_msg(state, tk1, npub, m, c, *mlen);
    return 0;
}
