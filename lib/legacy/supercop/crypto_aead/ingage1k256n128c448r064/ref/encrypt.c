/*
 * Ingage2 Reference C Implementation
 *
 * Copyright 2018,2019:
 *     Danilo Gligoroski <danilog@ntnu.no>
 *     Daniel Otte <bg@nerilex.org>
 *     Hristina Mihajloska <hristina.mihajloska@finki.ukim.mk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <stdio.h>
#include "crypto_aead.h"
#include "constants.h"
#include "api.h"


void print_state(const struct state *s) {
  int i;
  printf(" [ ");
  for (i = 0; i < INTERNAL_STATE_SIZE_BYTES - 1; ++i) {
    printf("%02x, ", s->a[i]);
  }
  printf("%02x ]\n", s->a[i]);
}

void shuffle_state_bits(struct state *s) {
  struct state t;

  const uint8_t mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
  uint8_t i, tmp, bit;

  for (i = 0; i < INTERNAL_STATE_SIZE_BYTES; ++i) {
    t.a[i] = 0;

    bit = 0;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 4;
    t.a[i] |= tmp;

    bit = 1;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 2;
    t.a[i] |= tmp;

    bit = 2;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] );
    t.a[i] |= tmp;

    bit = 3;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 2;
    t.a[i] |= tmp;

    bit = 4;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) >> 4;
    t.a[i] |= tmp;

    bit = 5;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] ) << 1;
    t.a[i] |= tmp;

    bit = 6;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] )  >> 5;
    t.a[i] |= tmp;

    bit = 7;
    tmp = ( s->a[(i + bit) % INTERNAL_STATE_SIZE_BYTES] & mask[bit] );
    t.a[i] |= tmp;
  }

  for (i = 0; i < INTERNAL_STATE_SIZE_BYTES; ++i) {
    s->a[i] = t.a[i];
  }
}


void dtransform(const uint8_t l, struct state *s) {
  uint8_t ldr, nextldr;
  uint8_t tmp;
  int i;

  ldr = l;
  for (i = 0; i < INTERNAL_STATE_SIZE_BYTES; ++i) {
    /* compute the new upper 2 bits */
    nextldr = s->a[i] >> 6;
    tmp = 	(Quas[ldr][ nextldr ]) << 6;

    /* compute the next 2 bits */
    ldr = nextldr;
    nextldr = (s->a[i] & 0x30) >> 4;
    tmp |= 	(Quas[ ldr ][ nextldr ]) << 4;

    /* compute the next 2 bits */
    ldr = nextldr;
    nextldr = (s->a[i] & 0x0c) >> 2;
    tmp |=	(Quas[ ldr ][ nextldr ]) << 2;

    /* compute the new lower 2 bits */
    ldr = nextldr;
    nextldr = s->a[i] & 0x03;
    tmp |=	Quas[ ldr ][ nextldr ];

    /* prepare the leader for the next byte in the state to be transformed */
    ldr = nextldr;

    /* in-place replacemet of the old byte with just computed tmp */
    s->a[i] = tmp;
  }
}

void qpermutation(struct state *s, uint8_t rounds) {
  int i;

  dtransform(leader[0], s);
  for (i = 1; i < rounds; ++i) {
    shuffle_state_bits(s);
    dtransform(leader[i], s);
  }
}


void Padd(unsigned char *mpad, const unsigned char *in, unsigned long long inlen) {
  unsigned long long i, lastbytes;
  int j;

  /* determine how many bytes from the original message go in the mpad block */
  lastbytes = inlen % RATE_BYTES;
  /* and put them in mpad */
  j = 0;
  for (i = inlen - lastbytes; i < inlen; ++i) {
    mpad[j++] = in[i];
  }

  /* append the byte 0x80 */
  mpad[j++] = 0x80;

  /* if there are still some remaining bytes in mpad, set them to 0 */
  while (j<RATE_BYTES) mpad[j++] = 0;
}

int crypto_aead_encrypt(
    unsigned char *c, unsigned long long *clen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k) {

  unsigned long long i, j, lastblockbyteindex;
  struct state st;
  unsigned char mpad[RATE_BYTES];
  unsigned char apad[RATE_BYTES];
  (void)nsec; /* avoid warning */

  // Initialize st
  j = 0;
  // Inject npub in the state st
  for (i = 0; i < CRYPTO_NPUBBYTES; ++i)
    st.a[j++] = npub[i];

  // Inject the secret key k in the state st
  for (i = 0; i < CRYPTO_KEYBYTES; ++i)
    st.a[j++] = k[i];
  // printf("initial state:"); print_state(&st);

  // If there are some uninitialized bytes in the state, set them to 0
  while (j < INTERNAL_STATE_SIZE_BYTES)
    st.a[j++] = 0;

  qpermutation(&st, ROUNDS);

  // Inject once more the secret key
  for (i = 0; i < CRYPTO_KEYBYTES; ++i)
    st.a[CRYPTO_NPUBBYTES + i] ^= k[i];
  // printf("after injecting the key bits for the second time:"); print_state(&st);


  // pad associated data
  Padd(apad, ad, adlen);
  // make the padding of associated data different than padding the plaintext
  apad[RATE_BYTES - 1] ^= 1;
  // pad plaintext
  Padd(mpad, m, mlen);


  // Absorb associated data
  lastblockbyteindex = adlen - (adlen % RATE_BYTES);
  i = 0;
  while (i < lastblockbyteindex) {
    for (j = 0; j < RATE_BYTES; ++j)
      st.a[j] ^= ad[i++];
    qpermutation(&st, ROUNDS/2);
  }
  /* absorb the final apad part */
  for (j = 0; j < RATE_BYTES; ++j)
    st.a[j] ^= apad[j];
  qpermutation(&st, ROUNDS/2);
  st.a[INTERNAL_STATE_SIZE_BYTES - 1] ^= 0x01;
  // printf("after absorbing the associated data:"); print_state(&st);


  // Absorb and encrypt the plaintext
  lastblockbyteindex = mlen - (mlen % RATE_BYTES);
  i = 0;
  while (i < lastblockbyteindex) {
    for (j = 0; j < RATE_BYTES; ++j) {
      st.a[j] ^= m[i];
      c[i++] = st.a[j];
    }
    qpermutation(&st, ROUNDS/2);
  }
  /* absorb the final mpad part */
  for (j = 0; j < RATE_BYTES; ++j) {
    if (i < mlen){
      st.a[j] ^= mpad[j];
      c[i++] = st.a[j];
    }
    else {
      st.a[j] ^= mpad[j];
    }
  }
  // printf("after absorbing and encrypting the plaintext (before the final call):"); print_state(&st);
  st.a[INTERNAL_STATE_SIZE_BYTES - 1] ^= 0x02;
  qpermutation(&st, ROUNDS);
  // printf("after absorbing and encrypting the plaintext:"); print_state(&st);


  // finalization
  // return tag
  for (i = 0; i < CRYPTO_ABYTES; ++i)
    c[mlen + i] = st.a[RATE_BYTES + i];
  *clen = mlen + CRYPTO_ABYTES;

  return 0;
}

int crypto_aead_decrypt(
    unsigned char *m, unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k) {

  unsigned long long i, j, lastblockbyteindex;
  struct state st;
  unsigned char cpad[RATE_BYTES];
  unsigned char apad[RATE_BYTES];
  (void)nsec; /* avoid warning */


  *mlen = 0;

  // Initialize st
  j = 0;
  // Inject npub in the state st
  for (i = 0; i < CRYPTO_NPUBBYTES; ++i)
    st.a[j++] = npub[i];

  // Inject the secret key k in the state st
  for (i = 0; i < CRYPTO_KEYBYTES; ++i)
    st.a[j++] = k[i];
  // printf("initial state:"); print_state(&st);

  // If there are some uninitialized bytes in the state, set them to 0
  while (j < INTERNAL_STATE_SIZE_BYTES)
    st.a[j++] = 0;

  qpermutation(&st, ROUNDS);

  // Inject once more the secret key
  for (i = 0; i < CRYPTO_KEYBYTES; ++i)
    st.a[CRYPTO_NPUBBYTES + i] ^= k[i];
  // printf("after injecting the key bits for the second time:"); print_state(&st);


  // pad associated data
  Padd(apad, ad, adlen);
  // make the padding of associated data different than padding the plaintext
  apad[RATE_BYTES - 1] ^= 1;
  // pad plaintext
  Padd(cpad, c, clen - CRYPTO_ABYTES);


  // Absorb associated data
  lastblockbyteindex = adlen - (adlen % RATE_BYTES);
  i = 0;
  while (i < lastblockbyteindex) {
    for (j = 0; j < RATE_BYTES; ++j)
      st.a[j] ^= ad[i++];
    qpermutation(&st, ROUNDS/2);
  }
  /* absorb the final apad part */
  for (j = 0; j < RATE_BYTES; ++j)
    st.a[j] ^= apad[j];
  qpermutation(&st, ROUNDS/2);
  // printf("after absorbing the associated data:"); print_state(&st);
  st.a[INTERNAL_STATE_SIZE_BYTES - 1] ^= 0x01;

  // Absorb and decrypt the ciphertext
  lastblockbyteindex = (clen - CRYPTO_ABYTES) - ((clen - CRYPTO_ABYTES) % RATE_BYTES);
  i = 0;
  while (i < lastblockbyteindex) {
    for (j = 0; j < RATE_BYTES; ++j) {
      m[i] = st.a[j] ^ c[i];
      st.a[j] = c[i++];
    }
    qpermutation(&st, ROUNDS/2);
  }
  /* absorb the final cpad part */
  for (j = 0; j < RATE_BYTES; ++j) {
    if (i < (clen - CRYPTO_ABYTES)){
      m[i++] = st.a[j] ^ cpad[j];
      st.a[j] = cpad[j];
    }
    else {
      st.a[j] ^= cpad[j];
    }
  }

  // printf("after absorbing and decrypting the ciphertext (before the final call):"); print_state(&st);
  st.a[INTERNAL_STATE_SIZE_BYTES - 1] ^= 0x02;
  qpermutation(&st, ROUNDS);
  // printf("after absorbing and decrypting the ciphertext:"); print_state(&st);


  // finalization
  // return -1 if verification fails
  for (i = 0; i < CRYPTO_ABYTES; ++i) {
    if (c[clen - CRYPTO_ABYTES + i] != st.a[RATE_BYTES + i])
      return -1;
  }

  // return plaintext
  *mlen = clen - CRYPTO_ABYTES;

  return 0;
}

