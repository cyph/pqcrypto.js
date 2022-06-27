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

  for (i = 0; i < 16; i++) {
    c[i] = (s[i] >> 1) ^ (s[i] & 0x80) ^ ((s[i] & 0x01) << 7);
  }
  
}

void rho_ad (const unsigned char* m,
	     unsigned char* s,
	     int len8,
	     int ver) {
  int i;
  unsigned char mp [16];  

  //printf("rho in m  = ");display_vector(m,16);
  pad(m,mp,ver,len8);
  //printf("rho in mp = ");display_vector(mp,16);
  //printf("rho in s  = ");display_vector(s,16);
  for (i = 0; i < ver; i++) {
    s[i] = s[i] ^ mp[i];
  }
  //printf("rho out s = ");display_vector(s,16);
  
}

void rho (const unsigned char* m,
	  unsigned char* c,
	  unsigned char* s,
	  int len8,
	  int ver) {
  int i;
  unsigned char mp [16];

  //printf("rho in m  = ");display_vector(m,16);
  pad(m,mp,ver,len8);
  //printf("rho in mp = ");display_vector(mp,16);
  //printf("rho in s  = ");display_vector(s,16);

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
  //printf("rho out s = ");display_vector(s,16);
  //printf("rho out c = ");display_vector(c,16);
  
}

void irho (unsigned char* m,
	  const unsigned char* c,
	  unsigned char* s,
	  int len8,
	  int ver) {
  int i;
  unsigned char cp [16];

  //printf("irho in c  = ");display_vector(c,16);
  pad(c,cp,ver,len8);
  //printf("irho in cp = ");display_vector(cp,16);
  //printf("irho in s  = ");display_vector(s,16);

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
  //printf("irho out s = ");display_vector(s,16);
  //printf("irho out m = ");display_vector(c,16);
  
}

void lfsr_gf128 (unsigned char* CNT) {
  unsigned char fb0;
  
  fb0 = CNT[15] >> 7;

  CNT[15] = (CNT[15] << 1) | (CNT[14] >> 7);
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
    CNT[0] = (CNT[0] << 1) ^ 0x87;
  }
  else {
    CNT[0] = (CNT[0] << 1);
  }
}

void compose_tweakey (unsigned char* KT,
		      unsigned char* CNT,
		      unsigned char D) {

  int i;

  lfsr_gf128(CNT);
  for (i = 0; i < 15; i++) {
    KT[i] = CNT[i];
  }
  KT[i] = D ^ CNT[15];

}

void ice(unsigned char* s,
	 unsigned char* L, unsigned char* V,
	 unsigned char D) {
  unsigned char KT [16];
  unsigned int i;

  lfsr_gf128(V);
  compose_tweakey(KT,L,D);
  //printf("BC in V = ");display_vector(V,16);
  //printf("BC in kt = ");display_vector(KT,16);
  //printf("BC in s  = ");display_vector(s,16);
  for (i = 0; i < 16; i = i + 1) {
    s[i] = s[i] ^ V[i];
  }  
  block_cipher(s,KT);
  for (i = 0; i < 16; i = i + 1) {
    s[i] = s[i] ^ V[i];
  }
  //printf("BC out s = ");display_vector(s,16);

}


void generate_tag (unsigned char* c, unsigned char* s) {
  
  g8A(s, c);

}
unsigned long long msg_encryption (const unsigned char** M, unsigned char** c,
				   unsigned char* L,
				   unsigned char* V,
				   unsigned char*s, unsigned char D,
				   unsigned long long mlen) {
  int len8;

  
  if (mlen >= 16) {
    len8 = 16;
    mlen = mlen - 16;
  }
  else {
    len8 = mlen;
    mlen = 0;
  }
  rho(*M, *c, s, len8, 16);
  *c = *c + len8;
  *M = *M + len8;
  ice(s,L,V,D);
  return mlen;
}

unsigned long long msg_decryption (unsigned char** M, const unsigned char** c,
				   unsigned char* L,
				   unsigned char* V,
				   unsigned char*s, unsigned char D,
				   unsigned long long clen) {
  int len8;

  
  if (clen >= 16) {
    len8 = 16;
    clen = clen - 16;
  }
  else {
    len8 = clen;
    clen = 0;
  }
  irho(*M, *c, s, len8, 16);
  *c = *c + len8;
  *M = *M + len8;
  ice(s,L,V,D);
  return clen;
}


unsigned long long ad_encryption (const unsigned char** M,
				  unsigned char* L,
				  unsigned char* V,
				  unsigned char*s, unsigned char D,
				  unsigned long long adlen) {
  int len8;

  if (adlen >= 16) {
    len8 = 16;
    adlen = adlen - 16;
  }
  else {
    len8 = adlen;
    adlen = 0;
  }
  rho_ad(*M, s, len8, 16);
  *M = *M + len8;
  ice(s,L,V,D);
  return adlen;
}

void kdf (const unsigned char* k,
	  const unsigned char* npub,
	  unsigned char* s,
	  unsigned char* L,
	  unsigned char* V) {

  unsigned int i;
  
  for (i = 0; i < 16; i = i + 1) {
    s[i] = npub[i];
  }  
  block_cipher(s,k);
  g8A(s,L);
  for (i = 0; i < 15; i = i + 1) {
    V[i] = k[i];
  }
  V[15] = k[15] ^ 0x01;
  block_cipher(s,V);
  g8A(s,V);
  for (i = 0; i < 16; i = i + 1) {
    s[i] = 0x00;
  }
  
}

void encrypt_tag (unsigned char* s,
		  unsigned char* CNT,
		  unsigned char* V,
		  unsigned char D) {
  unsigned char KT [16];
  int i;

  for (i = 0; i < 16; i++) {
    s[i] = s[i] ^ V[i];
  }
  for (i = 0; i < 15; i++) {
    KT[i] = CNT[i];    
  }
  KT[i] = D ^ CNT[15];
  //printf("BC in kt = ");display_vector(KT,16);
  //printf("BC in s  = ");display_vector(s,16);
  block_cipher(s,KT);
  for (i = 0; i < 16; i++) {
    s[i] = s[i] ^ V[i];
  }

  //printf("BC out s = ");display_vector(s,16);
  
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

  unsigned long long mauthlen;
  unsigned char s[16];
  unsigned char L[16];
  unsigned char V[16];
  unsigned char Lenc[16];
  unsigned char Venc[16];
  unsigned char T[16];
  unsigned int i;

  (void) nsec;
  mauthlen = mlen;
  kdf(k,npub,s,L,V);

  //printf("s = ");display_vector(s,16);
  //printf("L = ");display_vector(L,16);

  for (i = 0; i < 16; i = i +1) {//Store the mask for the encryption part
    Lenc[i] = L[i];
    Venc[i] = V[i];
  }

  // AD Len
  if (adlen == 0) { // Empty AD string
    ice(s,L,V,109);
  }
  else while (adlen > 0) {
      if (adlen < 16) { // Last AD block, partial
	adlen = ad_encryption(&ad,L,V,s,109,adlen);
      }
      else if (adlen == 16) {// Last AD block, full
	adlen = ad_encryption(&ad,L,V,s,108,adlen);
      }
      else { // Normal full AD block
	adlen = ad_encryption(&ad,L,V,s,100,adlen);
      }
    }

  // MSG_AUTH Len
  if (mauthlen == 0) { // Empty MSG string
    ice(s,L,V,111);
  }
  else while (mauthlen > 0) {
      if (mauthlen < 16) { // Last MSG block, partial
	mauthlen = ad_encryption(&m,L,V,s,111,mauthlen);
      }
      else if (mauthlen == 16) {// Last MSG block, full
	mauthlen = ad_encryption(&m,L,V,s,110,mauthlen);
      }
      else { // Normal full MSG block
	mauthlen = ad_encryption(&m,L,V,s,102,mauthlen);
      }
    }

  // Tag Generation
  generate_tag(T,s);
  //printf("T = ");display_vector(T,16);

  *clen = mlen + 16;
  m = m - mlen;
  
  // MSG Len
  if (mlen > 0) {
    for (i = 0; i < 16; i = i + 1) {
      s[i] = T[i];
    }
    encrypt_tag(s,Lenc,Venc,98);
    while (mlen > 16) {
      mlen = msg_encryption(&m,&c,Lenc,Venc,s,98,mlen);
    }
    rho(m, c, s, mlen, 16);
    c = c + mlen;
    m = m + mlen;
  }

  // Tag Concatenation
  for (i = 0; i < 16; i = i + 1) {
    *(c + i) = T[i];
  }

  c = c - *clen;
  
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
  unsigned long long mauthlen;
  unsigned char s[16];
  unsigned char L[16];
  unsigned char V[16];  
  unsigned char Lenc[16];
  unsigned char Venc[16];  
  unsigned char T[16];
  unsigned int i;
  const unsigned char* mauth;

  (void) nsec;
  mauth = m;

  mauthlen = clen - 16;
  kdf(k,npub,s,L,V);

  //printf("s = ");display_vector(s,16);

  for (i = 0; i < 16; i = i +1) {//Store the mask for the encryption part
    Lenc[i] = L[i];
    Venc[i] = V[i];
  }

  for (i = 0; i < 16; i++) {
    T[i] = *(c + clen - 16 + i);
  }

  clen = clen - 16;
  *mlen = clen;
  
  // MSG Len
  if (clen > 0) {
    for (i = 0; i < 16; i = i + 1) {
      s[i] = T[i];
    }
    encrypt_tag(s,Lenc,Venc,98);
    while (clen > 16) {
      clen = msg_decryption(&m,&c,Lenc,Venc,s,98,clen);
    }
    irho(m, c, s, clen, 16);
    c = c + clen;
    m = m + clen;
  }

  m = m - *mlen;  

  for (i = 0; i < 16; i = i + 1) {
    s[i] = 0;
  }
  // AD Len
  if (adlen == 0) { // Empty AD string
    ice(s,L,V,109);
  }
  else while (adlen > 0) {
      if (adlen < 16) { // Last AD block, partial
	adlen = ad_encryption(&ad,L,V,s,109,adlen);
      }
      else if (adlen == 16) {// Last AD block, full
	adlen = ad_encryption(&ad,L,V,s,108,adlen);
      }
      else { // Normal full AD block
	adlen = ad_encryption(&ad,L,V,s,100,adlen);
      }
    }

  // MSG_AUTH Len
  if (mauthlen == 0) { // Empty MSG string
    ice(s,L,V,111);
  }
  else while (mauthlen > 0) {
      if (mauthlen < 16) { // Last MSG block, partial
	mauthlen = ad_encryption(&mauth,L,V,s,111,mauthlen);
      }
      else if (mauthlen == 16) {// Last MSG block, full
	mauthlen = ad_encryption(&mauth,L,V,s,110,mauthlen);
      }
      else { // Normal full MSG block
	mauthlen = ad_encryption(&mauth,L,V,s,102,mauthlen);
      }
    }

  // Tag generation 
  g8A(s, T);
  //printf("T = ");display_vector(T,16);
  for (i = 0; i < 16; i++) {
    if (T[i] != (*(c+i))) {
      return -1;
    }    
  }

  return 0;

}
