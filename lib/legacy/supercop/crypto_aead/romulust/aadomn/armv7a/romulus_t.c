/**
 * Romulus-T core functions.
 * 
 * @author      Alexandre Adomnicai
 *              alex.adomnicai@gmail.com
 * 
 * @date        March 2022
 */
#include "skinny128.h"
#include "romulus_t.h"

/**
 * Equivalent to 'memset(buf, 0x00, buflen)'.
 */
static void zeroize(uint8_t buf[], int buflen)
{
  int i;
  for(i = 0; i < buflen; i++)
    buf[i] = 0x00;
}

/**
 * Hirose's double-block length compression function used in Romulus-H.
 */
static void hirose_128_128_256(
  unsigned char h[],
  unsigned char g[],
  const unsigned char m[])
{
  uint8_t i;
  uint8_t tmp[BLOCKBYTES];

  skinny128_384_plus(tmp, h, g, m, m+BLOCKBYTES);
  h[0] ^= 0x01;
  skinny128_384_plus(g, h, g, m, m+BLOCKBYTES);
  for (i = 0; i < BLOCKBYTES; i++) {
    g[i] ^= h[i];
    h[i] ^= tmp[i];
  }
  h[0] ^= 0x01;
}

/**
 * Padding function used in Romulus-H.
 */
static void ipad_256(
  const unsigned char m[],
  unsigned char mp[],
  int l,
  int len8)
{
  int i;
  for (i = 0; i < l; i++) {
    if (i < len8) {      
      mp[i] = m[i];
    } else if (i == l - 1) {
      mp[i] = (len8 & 0x1f);
    } else {
      mp[i] = 0x00;
    }      
  }
}

/**
 * Padding function used in Romulus-H.
 */
static void ipad_128(
  const unsigned char m[],
  unsigned char mp[],
  int l,
  int len8)
{
  int i;
  for (i = 0; i < l; i++) {
    if (i < len8) {      
      mp[i] = m[i];
    } else if (i == l - 1) {
      mp[i] = (len8 & 0xf);
    } else {
      mp[i] = 0x00;
    }      
  }
}

/**
 * Romulus-H implementation used within Romulus-T.
 * It is not convenient to mutualize the code with Romulus-H since some padding
 * needs to be done within the function execution.
 */
int romulusht(
  unsigned char out[],
  const unsigned char a[],
  unsigned long long  adlen,
  const unsigned char c[],
  unsigned long long clen,
  const unsigned char npub[],
  unsigned char tk1[])
{
  uint8_t h[BLOCKBYTES];
  uint8_t g[BLOCKBYTES];
  uint8_t p[2*BLOCKBYTES];
  uint8_t i, n, adempty, cempty;
  uint32_t tmp;

  n = BLOCKBYTES;

  if (adlen == 0) {
    adempty = 1;
  } else {
    adempty = 0;
  }
  if (clen == 0) {
    cempty = 1;
  } else {
    cempty = 0;
  }
  
  zeroize(tk1+1, BLOCKBYTES-1);
  tk1[0] = 0x01;

  zeroize(h, BLOCKBYTES);
  zeroize(g, BLOCKBYTES);
  while (adlen >= 2*BLOCKBYTES) { // AD Normal loop
    hirose_128_128_256(h, g, a);
    a += 2*BLOCKBYTES;
    adlen -= 2*BLOCKBYTES;
  }
  // Partial block (or in case there is no partial block we add a 0^2n block)
  if (adlen >= BLOCKBYTES) {
    ipad_128(a, p, 2*BLOCKBYTES, adlen);
    hirose_128_128_256(h, g, p);
  }
  else if (adempty == 0) {
    ipad_128(a,p,BLOCKBYTES,adlen);
    adlen = 0;
    if (clen >= BLOCKBYTES) {
      for (i = 0; i < BLOCKBYTES; i++)
        p[i+BLOCKBYTES] = c[i]; 
      hirose_128_128_256(h, g, p);
      UPDATE_CTR(tk1);
      clen -= BLOCKBYTES;
      c += BLOCKBYTES;      
    }
    else if (clen > 0) {
      ipad_128(c, p+BLOCKBYTES, BLOCKBYTES, clen);
      hirose_128_128_256(h,g,p);
      clen = 0;
      cempty = 1;
      c += BLOCKBYTES;
      UPDATE_CTR(tk1);
    }
    else {
      for (i = 0; i < BLOCKBYTES; i++) // Pad the nonce
        p[i+BLOCKBYTES] = npub[i];
      hirose_128_128_256(h,g,p);
      n = 0;
    }
  }
  
  while (clen >= 2*BLOCKBYTES) { // C Normal loop
    hirose_128_128_256(h,g,c);
    c += 2*BLOCKBYTES;
    clen -= 2*BLOCKBYTES;
    UPDATE_CTR(tk1);
    UPDATE_CTR(tk1);
  }
  if (clen > BLOCKBYTES) {
    ipad_128(c,p,2*BLOCKBYTES,clen);
    hirose_128_128_256(h,g,p);
    UPDATE_CTR(tk1);
    UPDATE_CTR(tk1);
  }
  else if (clen == BLOCKBYTES) {
    ipad_128(c,p,2*BLOCKBYTES,clen);
    hirose_128_128_256(h,g,p);
    UPDATE_CTR(tk1);
  }
  else if (cempty == 0) {
    ipad_128(c,p,BLOCKBYTES,clen);
    if (clen > 0) {
      UPDATE_CTR(tk1);
    }
    for (i = 0; i < BLOCKBYTES; i++) { // Pad the nonce
      p[i+BLOCKBYTES] = npub[i];  
    }
    hirose_128_128_256(h,g,p);
    n = 0;
  }

  if (n == BLOCKBYTES) {
    for (i = 0; i < 16; i++) { // Pad the nonce and counter
      p[i] = npub[i];      
    }
    for (i = 16; i < 23; i++) {
      p[i] = tk1[i-16];      
    }
    ipad_256(p,p,2*BLOCKBYTES,23);
  }
  else {
    ipad_256(tk1,p,2*BLOCKBYTES,7);
  }
  h[0] ^= 2;
  hirose_128_128_256(h,g,p);
  
  for (i = 0; i < BLOCKBYTES; i++) { // Assign the output tag
    out[i] = h[i];
    out[i+TAGBYTES] = g[i];
  }
  return 0;
}

/**
 * TK1 and internal state are initialized to 0.
 */
void romulust_init(uint8_t *state, uint8_t *tk1)
{
    zeroize(tk1, BLOCKBYTES);
    zeroize(state, BLOCKBYTES);
}

/**
 * Key derivation function used in Romulus-T.
 * This function requires side-channel countermeasure since the secret key is
 * directly manipulated.
 * The derived key is then stored in the internal state.
 */
void romulust_kdf(
  uint8_t *state,
  uint8_t *tk1,
  const unsigned char *npub,
  const unsigned char *k)
{
  uint8_t zeroes[TWEAKEYBYTES];
  zeroize(zeroes, TWEAKEYBYTES);
	SET_DOMAIN(tk1, 0x42);
	skinny128_384_plus(state, npub, tk1, zeroes, k);
  tk1[0] = 0x01;  // init counter
}

/**
 * Process the input message.
 * Update the internal state and the output buffer.
 */
void romulust_process_msg(
  uint8_t *state,
  uint8_t *tk1,
  const unsigned char *npub,
  unsigned char *c,
  const unsigned char *m,
  unsigned long long mlen)
{
  unsigned long long i;
  uint32_t tmp;
	uint8_t out[BLOCKBYTES];
  uint8_t zeroes[TWEAKEYBYTES];
  zeroize(zeroes, TWEAKEYBYTES);
	while(mlen > BLOCKBYTES) {
		SET_DOMAIN(tk1, 0x40);
    skinny128_384_plus(out, npub, tk1, zeroes, state);
		SET_DOMAIN(tk1, 0x41);
    skinny128_384_plus(state, npub, tk1, zeroes, state);
    UPDATE_CTR(tk1);
		XOR_BLOCK(c, m, out);
		c     += BLOCKBYTES;
		m     += BLOCKBYTES;
    mlen  -= BLOCKBYTES;
	}
  SET_DOMAIN(tk1, 0x40);
  skinny128_384_plus(out, npub, tk1, zeroes, state);
  UPDATE_CTR(tk1);
	for(i = 0; i < mlen; i++)
		c[i] = m[i] ^ out[i];
}

/**
 * Generation of the authentication tag from the internal state and additional
 * data.
 * This function requires side-channel countermeasure since the secret key is
 * directly manipulated.
 */
void romulust_generate_tag(
  uint8_t *tag,
  unsigned char *tk1,
  const unsigned char *ad,
  unsigned long long adlen,
  const unsigned char *c,
  unsigned long long mlen,
  const unsigned char *npub,
  const unsigned char *k)
{
	uint8_t hash[2*BLOCKBYTES];
  romulusht(hash, ad, adlen, c, mlen, npub, tk1);
  zeroize(tk1, BLOCKBYTES);
  SET_DOMAIN(tk1, 0x44);
  skinny128_384_plus(tag, hash, tk1, hash+BLOCKBYTES, k);
}
