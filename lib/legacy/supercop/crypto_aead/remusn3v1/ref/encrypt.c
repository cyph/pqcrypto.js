/*
 * Date: 29 November 2018
 * Contact: Thomas Peyrin - thomas.peyrin@gmail.com
 * Mustafa Khairallah - mustafam001@e.ntu.edu.sg
 * Email: mustafam001@e.ntu.edu.sg
 */

#include "crypto_aead.h"
#include "api.h"
#include "variant.h"
#include "bc.h"
#include <stdio.h>
#include <stdlib.h>

/*void display_vector (const unsigned char* x, int lenx) {
  int i;

  for (i = 0; i < lenx; i++) {
    printf("%02x",x[i]);
  }
  printf("\n");
  
  }*/

void pad (const unsigned char* m, unsigned char* mp, int l, int len8) {
  int i;

  for (i = 0; i < l; i++) {
    if (i < len8) {      
      mp[i] = m[i];
    }
    else if (i == l - 1) {
      mp[i] = (len8 & 0x0f);
    }
    else {
      mp[i] = 0x00;
    }      
  }
  
}

void g8A (unsigned char* s, unsigned char* c) {
  int i;

  for (i = 0; i < 8; i++) {
    c[i] = (s[i] >> 1) ^ (s[i] & 0x80) ^ ((s[i] & 0x01) << 7);
  }
  
}

void rho_ad (const unsigned char* m,
	     unsigned char* s,
	     int len8,
	     int ver) {
  int i;
  unsigned char mp [8];  

  //printf("rho in m  = ");display_vector(m,8);
  pad(m,mp,ver,len8);
  //printf("rho in mp = ");display_vector(mp,8);
  //printf("rho in s  = ");display_vector(s,8);
  for (i = 0; i < ver; i++) {
    s[i] = s[i] ^ mp[i];
  }
  //printf("rho out s = ");display_vector(s,8);
  
}

void rho (const unsigned char* m,
	  unsigned char* c,
	  unsigned char* s,
	  int len8,
	  int ver) {
  int i;
  unsigned char mp [8];

  //printf("rho in m  = ");display_vector(m,8);
  pad(m,mp,ver,len8);
  //printf("rho in mp = ");display_vector(mp,8);
  //printf("rho in s  = ");display_vector(s,8);

  g8A(s,c);
  for (i = 0; i < ver; i++) {
    s[i] = s[i] ^ mp[i];
    if (i < len8) {
      c[i] = c[i] ^ mp[i];
    }
    else {
      c[i] = 0;
    }
  }
  //printf("rho out s = ");display_vector(s,8);
  //printf("rho out c = ");display_vector(c,8);
  
}

void irho (unsigned char* m,
	  const unsigned char* c,
	  unsigned char* s,
	  int len8,
	  int ver) {
  int i;
  unsigned char cp [16];

  //printf("irho in c  = ");display_vector(c,8);
  pad(c,cp,ver,len8);
  //printf("irho in cp = ");display_vector(cp,8);
  //printf("irho in s  = ");display_vector(s,8);

  g8A(s,m);
  for (i = 0; i < ver; i++) {
    if (i < len8) {
      s[i] = s[i] ^ cp[i] ^ m[i];
    }
    else {
      s[i] = s[i] ^ cp[i];
    }
    if (i < len8) {
      m[i] = m[i] ^ cp[i];
    }
    else {
      m[i] = 0;
    }
  }
  //printf("irho out s = ");display_vector(s,8);
  //printf("irho out m = ");display_vector(c,8);
  
}

void lfsr_gf120 (unsigned char* CNT) {
  unsigned char fb0;
  
  fb0 = CNT[14] >> 7;

  CNT[14] = (CNT[14] << 1) | (CNT[13] >> 7);
  CNT[13] = (CNT[13] << 1) | (CNT[12] >> 7);
  CNT[12] = (CNT[12] << 1) | (CNT[11] >> 7);
  CNT[11] = (CNT[11] << 1) | (CNT[10] >> 7);
  CNT[10] = (CNT[10] << 1) | (CNT[9] >> 7);
  CNT[9] = (CNT[9] << 1) | (CNT[8] >> 7);
  CNT[8] = (CNT[8] << 1) | (CNT[7] >> 7);
  CNT[7] = (CNT[7] << 1) | (CNT[6] >> 7);  
  CNT[6] = (CNT[6] << 1) | (CNT[5] >> 7);
  CNT[5] = (CNT[5] << 1) | (CNT[4] >> 7);
  CNT[4] = (CNT[4] << 1) | (CNT[3] >> 7);
  CNT[3] = (CNT[3] << 1) | (CNT[2] >> 7);
  CNT[2] = (CNT[2] << 1) | (CNT[1] >> 7);
  CNT[1] = (CNT[1] << 1) | (CNT[0] >> 7);
  if (fb0 == 1) {
    CNT[0] = (CNT[0] << 1) ^ 0x1b;
  }
  else {
    CNT[0] = (CNT[0] << 1);
  }
}

void compose_tweakey (unsigned char* KT,
		      unsigned char* CNT,
		      unsigned char D) {

  int i;

  lfsr_gf120(CNT);
  for (i = 0; i < 15; i++) {
    KT[i] = CNT[i];
  }
  KT[i] = D ^ CNT[15];

}



void ice(unsigned char* s,
	 unsigned char* L,
	 unsigned char D) {
  unsigned char KT [16];

  compose_tweakey(KT,L,D);
  //printf("BC in kt = ");display_vector(KT,16);
  //printf("BC in s  = ");display_vector(s,8);
  block_cipher(s,KT);
  //printf("BC out s = ");display_vector(s,8);

}


void generate_tag (unsigned char** c, unsigned char* s,
		   int n, unsigned long long* clen) {
  
  g8A(s, *c);
  *c = *c + n;
  *c = *c - *clen;

}

unsigned long long msg_encryption (const unsigned char** M, unsigned char** c,
				   unsigned char* L,
				   unsigned char*s, unsigned char D,
				   unsigned long long mlen) {
  int len8;

  
  if (mlen >= 8) {
    len8 = 8;
    mlen = mlen - 8;
  }
  else {
    len8 = mlen;
    mlen = 0;
  }
  rho(*M, *c, s, len8, 8);
  *c = *c + len8;
  *M = *M + len8;
  ice(s,L,D);
  return mlen;
}

unsigned long long msg_decryption (unsigned char** M, const unsigned char** c,
				   unsigned char* L,
				   unsigned char*s, unsigned char D,
				   unsigned long long clen) {
  int len8;

  
  if (clen >= 8) {
    len8 = 8;
    clen = clen - 8;
  }
  else {
    len8 = clen;
    clen = 0;
  }
  irho(*M, *c, s, len8, 8);
  *c = *c + len8;
  *M = *M + len8;
  ice(s,L,D);
  return clen;
}

unsigned long long ad_encryption (const unsigned char** M,
				  unsigned char* L,
				  unsigned char*s, unsigned char D,
				  unsigned long long adlen) {
  int len8;

  if (adlen >= 8) {
    len8 = 8;
    adlen = adlen - 8;
  }
  else {
    len8 = adlen;
    adlen = 0;
  }
  rho_ad(*M, s, len8, 8);
  *M = *M + len8;
  ice(s,L,D);
  return adlen;
  
}

void kdf (const unsigned char* k,
	  const unsigned char* npub,
	  unsigned char* s,
	  unsigned char* L) {
  unsigned int i;
  
  for (i = 0; i < 12; i = i + 1) {
    L[i] = npub[i]^k[i];
  }
  for (i = 12; i < 16; i++) {
    L[i] = k[i];
  }    
  for (i = 0; i < 8; i = i + 1) {
    s[i] = 0x00;
  }

  
  
}

int crypto_aead_encrypt (
			 unsigned char* c, unsigned long long* clen,
			 const unsigned char* m, unsigned long long mlen,
			 const unsigned char* ad, unsigned long long adlen,
			 const unsigned char* nsec,
			 const unsigned char* npub,
			 const unsigned char* k
			 )
{

  unsigned char s[8];
  unsigned char L[16];

  (void) nsec;
  kdf(k,npub,s,L);

  //printf("s = ");display_vector(s,8);
  //printf("L = ");display_vector(L,16);

  // AD Len
  if (adlen == 0) { // Empty AD string
    ice(s,L,141);
  }
  else while (adlen > 0) {
      if (adlen < 8) { // Last AD block, partial
	adlen = ad_encryption(&ad,L,s,141,adlen);
      }
      else if (adlen == 8) {// Last AD block, full
	adlen = ad_encryption(&ad,L,s,140,adlen);
      }
      else { // Normal full AD block
	adlen = ad_encryption(&ad,L,s,132,adlen);
      }
    }

  *clen = mlen + 8;
  
  // MSG Len
  if (mlen == 0) { // Empty MSG string
    ice(s,L,139);
  }
  else while (mlen > 0) {
      if (mlen < 8) { // Last MSG block, partial
	mlen = msg_encryption(&m,&c,L,s,139,mlen);
      }
      else if (mlen == 8) {// Last MSG block, full
	mlen = msg_encryption(&m,&c,L,s,138,mlen);
      }
      else { // Normal full MSG block
	mlen = msg_encryption(&m,&c,L,s,130,mlen);
      }
    }

  // Tag Generation
  generate_tag(&c,s,8,clen);
  //printf("T = ");display_vector(c+*clen-8,8);
  
  return 0;
  
}

int crypto_aead_decrypt(
unsigned char *m,unsigned long long *mlen,
unsigned char *nsec,
const unsigned char *c,unsigned long long clen,
const unsigned char *ad,unsigned long long adlen,
const unsigned char *npub,
const unsigned char *k
)
{
  unsigned char s[8];
  unsigned char L[16];
  unsigned char T[8];
  unsigned int i;

  (void) nsec;
  kdf(k,npub,s,L);

  //printf("s = ");display_vector(s,8);
  //printf("L = ");display_vector(L,16);

  // AD Len
  if (adlen == 0) { // Empty AD string
    ice(s,L,141);
  }
  else while (adlen > 0) {
      if (adlen < 8) { // Last AD block, partial
	adlen = ad_encryption(&ad,L,s,141,adlen);
      }
      else if (adlen == 8) {// Last AD block, full
	adlen = ad_encryption(&ad,L,s,140,adlen);
      }
      else { // Normal full AD block
	adlen = ad_encryption(&ad,L,s,132,adlen);
      }
    }

  clen = clen - 8;
  *mlen = clen;
  
  // MSG Len
  if (clen == 0) { // Empty MSG string
    ice(s,L,139);
  }
  else while (clen > 0) {
      if (clen < 8) { // Last MSG block, partial
	clen = msg_decryption(&m,&c,L,s,139,clen);
      }
      else if (clen == 8) {// Last MSG block, full
	clen = msg_decryption(&m,&c,L,s,138,clen);
      }
      else { // Normal full MSG block
	clen = msg_decryption(&m,&c,L,s,130,clen);
      }
    }

  m = m - *mlen;
  // Tag generation 
  g8A(s, T);
  //printf("T = ");display_vector(T,8);
  for (i = 0; i < 8; i++) {
    if (T[i] != (*(c+i))) {
      return -1;
    }    
  }

  return 0;

}
