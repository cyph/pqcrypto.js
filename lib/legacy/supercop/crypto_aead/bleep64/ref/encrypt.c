#include "api.h"
#include "crypto_aead.h"
#include <stdint.h>
#define OCS(a, b) a += (b) + (a + (b) < a ? 1 : 0)  // 1s complement sum
#define Swap(a, b) (a ^= b, b ^= a, a ^= b)
//---------------------------------------------------------------------- Bleep64
inline int Bleep64(B_iv, B_key, B_pt, B_ct, B_ad, adlen, inlen, outlen, encrypt)
  unsigned char *B_iv;        // pointer to initialization vector
  unsigned char *B_key;       // pointer to key
  unsigned char *B_pt, *B_ct; // plaintext and ciphertext pointers respectively
  unsigned char *B_ad;        // pointer to associated data
  int32_t       adlen, inlen; // number of bytes in associated date, and input
  int32_t       *outlen;      // number of bytes in output
  char          encrypt;      // 0 means decrypt, else encrypt
  // local variables
{ register uint32_t t, u;    // short-term temporaries
  register uint32_t x, y;    // auto-key and LFSR filter state
  register uint64_t L0, L1;  // LFSR x^127 + x^63 + 1, left-justified
  register int32_t n;        // byte to process
  uint32_t *iv, *key, *pt, *ct, *ad;    // word casts of byte pointers
  unsigned char *bpt, *bct, *bad; // byte sized pointers for remainders
  unsigned char valid[CRYPTO_ABYTES + 8] = {0}; // buffer for extra validation data

  inline void crypt(char odd, char assoc) // do one word & update crypto state
  { t = (((uint64_t)x * y) >> 32) - (x * y);  // filter LFSR, diffuse autokey
    if (assoc)        u = *ad++ ^ t;  
    else if (encrypt) u = *pt++ ^ t, *ct++ = u + (L0 >> 32*odd);
    else              u = *ct++ - (L0 >> 32*odd), *pt++ = u ^ t;
    if (odd) OCS(y, u), OCS(x, L1 >> 32);      // feedback from 1st half of loop
    else     OCS(x, u), OCS(y, L1 & 0xFFFF);   // feedback from 2nd half of loop
    if (odd || y >> 31) L1 |= (L0 ^ L1) >> 63, L0 = (L0 ^ L1) << 1, Swap(L0,L1); }

  // casting kludges
  iv  = (uint32_t*)B_iv;  // word pointer to initialization vector
  key = (uint32_t*)B_key; // word pointer to key
  pt  = (uint32_t*)B_pt;  // plaintext word pointer
  ct  = (uint32_t*)B_ct;  // ciphertext word pointer
  ad  = (uint32_t*)B_ad;  // word pointer to associated data
  // IV processing
  t  = key[0] * (iv[0] + key[4]);
  u  = key[1] * (iv[1] + t);
  L0 = key[2] * (iv[2] + u);
  L1 = key[3] * (iv[0] + (L0 & 0xFFFF));
  x  = key[0] * (iv[1] + (L1 & 0xFFFF));
  y  = key[1] * (iv[2] + x);
  L0 = ((uint64_t) t << 32) | (L0 & 0xFFFF);   OCS(x, 1);   // x != 0
  L1 = ((uint64_t) u << 32) | x;               OCS(y, x);   // L1 != 0, y != 0
  // process associated data
  n = adlen;
  while ( (n -= 8) >= 0 ) { crypt(1, 1); crypt(0, 1); } // do 8 bytes per loop
  if (n > -5) crypt(1, 1);  // if 4 to 7 bytes left, do another full word
  for (n &= 3, bad = (unsigned char *) ad; n--; OCS(x, *bad), bad++);   // do last 1 to 3 bytes
  // do en/de-cryption; decryption needs to end sooner due to validation data
  n = inlen - (encrypt? 0 : CRYPTO_ABYTES);
  while ( (n -= 8) >= 0 ) { crypt(1, 0); crypt(0, 0); } // do 8 bytes per loop
  if (encrypt) {
    // Copy last n bytes to buffer and encrypt from there instead
    bpt = (unsigned char *) pt;
    for (n = 0; n < (inlen % 8); n++) valid[n] = bpt[n];
    pt = (uint32_t *) valid; // use the buffer for pt instead
    n = (inlen % 8) + CRYPTO_ABYTES; 
    while ( (n -= 8) >= 0 ) { crypt(1, 0); crypt(0, 0); } // do 8 bytes per loop
  } else {
    // Switch to decrypting into our buffer, B_pt is about to run out of space
    pt = (uint32_t *) valid;
    n = (inlen % 8) + CRYPTO_ABYTES;
    while ( (n -= 8) >= 0 ) { crypt(1, 0); crypt(0, 0); } // do 8 bytes per loop
  }
  if (n > -5) crypt(1, 0);  // if 4 to 7 bytes left, do another full word
  t = (((uint64_t)x * y) >> 32) - (x * y);  // filter LFSR and diffuse autokey
  bpt = (unsigned char *) pt; bct = (unsigned char *) ct; // need char sized pointers
  for (n &= 3; n--;)   // do last 1 to 3 bytes little-endian
    if (encrypt) *bct++ = (char) (*bpt++ ^ (t >> 8*(3-n))) + (L0 >> 8*(3-n));
    else         *bpt++ = (char) (*bct++ - (L0 >> 8*(3-n))) ^ (t >> 8*(3-n));
  if (encrypt) *outlen = inlen + CRYPTO_ABYTES;
  else {
    *outlen = inlen - CRYPTO_ABYTES;
    // Copy first few bytes from the temp buffer to the end of the real one
    bpt = B_pt + (((inlen-CRYPTO_ABYTES)/8) * 8);
    for (n = 0; n < (inlen % 8); n++) bpt[n] = valid[n];
    // And the rest should be zeros
    for (n = 0; n < CRYPTO_ABYTES; n++) if (valid[n+(inlen%8)]) return -1;
  }
  return 0;}
//---------------------------------------------------------------------- encrypt
int crypto_aead_encrypt(unsigned char *c,        unsigned long long *clen,
                        const unsigned char *m,  unsigned long long mlen,
                        const unsigned char *ad, unsigned long long adlen,
                        const unsigned char *nsec,
                        const unsigned char *npub,
                        const unsigned char *k)
{ return Bleep64(npub, k, m, c, ad, adlen, mlen, clen, 1); }
//---------------------------------------------------------------------- decrypt
int crypto_aead_decrypt(unsigned char *m,        unsigned long long *mlen,
                        unsigned char *nsec,
                        const unsigned char *c,  unsigned long long clen,
                        const unsigned char *ad, unsigned long long adlen,
                        const unsigned char *npub,
                        const unsigned char *k)
{ return Bleep64(npub, k, m, c, ad, adlen, clen, mlen, 0); }
