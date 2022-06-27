#include <string.h>
#include <stdio.h>
#include "api.h"
#include "crypto_aead.h"

#define const1  0xfe
#define const2  0xff
#define const3  0xff
#define const4  0xff
#define const_ad 0xff


#define LengthA  80
#define LengthB  88
#define LengthC  88
#define NLa  73
#define NLb  65
#define NLc  77
#define FBa  74
#define FBb  66
#define FBc  84
#define FFa  68
#define FFb  64
#define FFc  68
#define INb  85
#define INc  85

#define INIT_ROUNDS  1024
#define FINA_ROUNDS  1024



void Stateupdate(unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3, unsigned char exbit)
{
	unsigned char p = 0, q = 0, r = 0;
	int i;

	p = (FSR1[NLa - 1] & FSR1[LengthA - 1 - 1]) ^ FSR1[LengthA - 1] ^ FSR1[FFa - 1] ^ FSR2[FBb - 1] ^ (FSR2[INb - 1] & FSR3[INc - 1]);
	q = (FSR2[NLb - 1] & FSR2[LengthB - 1 - 1]) ^ FSR2[LengthB - 1] ^ FSR2[FFb - 1] ^ FSR3[FBc - 1];
	r = (FSR3[NLc - 1] & FSR3[LengthC - 1 - 1]) ^ FSR3[LengthC - 1] ^ FSR3[FFc - 1] ^ FSR1[FBa - 1];

	for (i = LengthA - 1; i >= 1; i--)
    FSR1[i] = FSR1[i - 1];
	FSR1[0] = (r ^ exbit);
	
  for (i = LengthB - 1; i >= 1; i--)
    FSR2[i] = FSR2[i - 1];
  FSR2[0] = (p ^ exbit);
	
  for (i = LengthC - 1; i >= 1; i--)
    FSR3[i] = FSR3[i - 1];
  FSR3[0] = (q ^ exbit);

}


unsigned char filter(unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3)
{
	unsigned char out = 0;

	out = FSR1[LengthA - 1] ^ FSR1[FFa - 1] ^ FSR2[LengthB - 1] ^ FSR2[FFb - 1] ^ FSR3[LengthC - 1] ^ FSR3[FFc - 1] ^ (FSR2[INb - 1] & FSR3[INc - 1]);
	return(out);

}




void Init_loading(unsigned char *FSR, unsigned char *sr)
{
	unsigned char i;
	
	for (i = 0; i < 8; i++)
	{
		FSR[7-i] = sr[i];
	}
}





void Initialization(unsigned char k[CRYPTO_KEYBYTES * 8], unsigned char iv[CRYPTO_NPUBBYTES * 8], unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3)
{
	unsigned int t;
	unsigned char d, c1[8], c2[8], c3[8], c4[8];

	d = 0;

	memset(FSR1, 0x00, LengthA);
	memset(FSR2, 0x00, LengthB);
	memset(FSR3, 0x00, LengthC);


	memset(c1, 0x00, 8);
	memset(c2, 0x00, 8);
	memset(c3, 0x00, 8);
	memset(c4, 0x00, 8);


	for (t = 0; t < 8; t++)
    c1[t] = (const1 >> t) & 0x1;
	for (t = 0; t < 8; t++)
    c2[t] = (const2 >> t) & 0x1;
	for (t = 0; t < 8; t++)
    c3[t] = (const3 >> t) & 0x1;
	for (t = 0; t < 8; t++)
    c4[t] = (const4 >> t) & 0x1;

	Init_loading(FSR1, iv); 
  Init_loading(FSR1 + 8, k + 32); 
  Init_loading(FSR1 + 16, c4); 
  Init_loading(FSR1 + 24, k + 24); 
  Init_loading(FSR1 + 32, c3);
	Init_loading(FSR1 + 40, k + 16); 
  Init_loading(FSR1 + 48, c2); 
  Init_loading(FSR1 + 56, k + 8); 
  Init_loading(FSR1 + 64, c1); 
  Init_loading(FSR1 + 72, k);
	
	Init_loading(FSR2, iv + 88); 
  Init_loading(FSR2 + 8, iv + 80); 
  Init_loading(FSR2 + 16, iv + 72); 
  Init_loading(FSR2 + 24, iv + 64); 
  Init_loading(FSR2 + 32, iv + 56);
	Init_loading(FSR2 + 40, iv + 48); 
  Init_loading(FSR2 + 48, iv + 40); 
  Init_loading(FSR2 + 56, iv + 32); 
  Init_loading(FSR2 + 64, iv + 24); 
  Init_loading(FSR2 + 72, iv + 16);
	Init_loading(FSR2 + 80, iv + 8);
	
	Init_loading(FSR3, k + 120); 
  Init_loading(FSR3 + 8, k + 112); 
  Init_loading(FSR3 + 16, k + 104); 
  Init_loading(FSR3 + 24, k + 96); 
  Init_loading(FSR3 + 32, k + 88);
	Init_loading(FSR3 + 40, k + 80); 
  Init_loading(FSR3 + 48, k + 72); 
  Init_loading(FSR3 + 56, k + 64); 
  Init_loading(FSR3 + 64, k + 56); 
  Init_loading(FSR3 + 72, k + 48);
	Init_loading(FSR3 + 80, k + 40);
	
	for (t = 0; t < INIT_ROUNDS; t++)
    Stateupdate(FSR1, FSR2, FSR3, 0);

}

void Initialization_mac(unsigned char k[CRYPTO_KEYBYTES * 8], unsigned char iv[CRYPTO_NPUBBYTES * 8], unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3)
{
	unsigned int t;
	unsigned char d, c1[8], c2[8], c3[8], c4[8];

	d = 0;

	memset(FSR1, 0x00, LengthA);
	memset(FSR2, 0x00, LengthB);
	memset(FSR3, 0x00, LengthC);


	memset(c1, 0x00, 8);
	memset(c2, 0x00, 8);
	memset(c3, 0x00, 8);
	memset(c4, 0x00, 8);


	for (t = 0; t < 8; t++)
    c1[t] = (const1 >> t) & 0x1;
	for (t = 0; t < 8; t++)
    c2[t] = (const2 >> t) & 0x1;
	for (t = 0; t < 8; t++)
    c3[t] = (const3 >> t) & 0x1;
	for (t = 0; t < 8; t++)
    c4[t] = (const4 >> t) & 0x1;

	Init_loading(FSR1, iv); 
  Init_loading(FSR1 + 8, k + 32); 
  Init_loading(FSR1 + 16, c4); 
  Init_loading(FSR1 + 24, k + 24); 
  Init_loading(FSR1 + 32, c3);
	Init_loading(FSR1 + 40, k + 16); 
  Init_loading(FSR1 + 48, c2); 
  Init_loading(FSR1 + 56, k + 8); 
  Init_loading(FSR1 + 64, c1); 
  Init_loading(FSR1 + 72, k);

	Init_loading(FSR2, iv + 88); 
  Init_loading(FSR2 + 8, iv + 80); 
  Init_loading(FSR2 + 16, iv + 72); 
  Init_loading(FSR2 + 24, iv + 64); 
  Init_loading(FSR2 + 32, iv + 56);
	Init_loading(FSR2 + 40, iv + 48); 
  Init_loading(FSR2 + 48, iv + 40); 
  Init_loading(FSR2 + 56, iv + 32); 
  Init_loading(FSR2 + 64, iv + 24); 
  Init_loading(FSR2 + 72, iv + 16);
	Init_loading(FSR2 + 80, iv + 8);

	Init_loading(FSR3, k + 120); 
  Init_loading(FSR3 + 8, k + 112); 
  Init_loading(FSR3 + 16, k + 104); 
  Init_loading(FSR3 + 24, k + 96); 
  Init_loading(FSR3 + 32, k + 88);
	Init_loading(FSR3 + 40, k + 80); 
  Init_loading(FSR3 + 48, k + 72); 
  Init_loading(FSR3 + 56, k + 64); 
  Init_loading(FSR3 + 64, k + 56); 
  Init_loading(FSR3 + 72, k + 48);
	Init_loading(FSR3 + 80, k + 40);

	Stateupdate(FSR1, FSR2, FSR3, 1);
	for (t = 1; t < INIT_ROUNDS; t++)
    Stateupdate(FSR1, FSR2, FSR3, 0);

}

void Keystreamgen(unsigned int n, unsigned char *ks, unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3)
{
	unsigned long long i;

  for (i = 0; i < n; i++)
  {
    ks[i] = filter(FSR1, FSR2, FSR3);
    Stateupdate(FSR1, FSR2, FSR3, 0);
  }

}

void Keystreamgen_byte(unsigned int n, unsigned char *ks, unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3, unsigned char *ksbyte)
{
	unsigned long long i;

  for (i = 0; i < n; i++)
  {
    ks[i] = filter(FSR1, FSR2, FSR3);
    Stateupdate(FSR1, FSR2, FSR3, 0);
    *(ksbyte + i / 8) |= (ks[i] << (i % 8));
  }

}


void enc_onebyte(unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3, unsigned char plaintextbyte,
	unsigned char *ciphertextbyte)
{
	unsigned char i;
	unsigned char plaintextbit, ciphertextbit, kstem;

	*ciphertextbyte = 0;
	kstem = 0;

  for (i = 0; i < 8; i++)
  {
    plaintextbit = (plaintextbyte >> i) & 0x1;
    kstem = filter(FSR1, FSR2, FSR3);
    ciphertextbit = kstem ^ plaintextbit;
    Stateupdate(FSR1, FSR2, FSR3, 0);
    *ciphertextbyte |= (ciphertextbit << i);
  }

}


void dec_onebyte(unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3, unsigned char *plaintextbyte,
	unsigned char ciphertextbyte)
{
	unsigned char i;
	unsigned char plaintextbit, ciphertextbit, kstem;

	*plaintextbyte = 0;
	kstem = 0;
  for (i = 0; i < 8; i++)
  {
    ciphertextbit = (ciphertextbyte >> i) & 0x1;
    kstem = filter(FSR1, FSR2, FSR3);
    plaintextbit = kstem ^ ciphertextbit;
    Stateupdate(FSR1, FSR2, FSR3, 0);
    *plaintextbyte |= (plaintextbit << i);
  }

}


void mac_onebyte_message(unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3, unsigned char messagebyte)
{
	unsigned char i;
	unsigned char plaintextbit;

  for (i = 0; i < 8; i++)
  {
    plaintextbit = (messagebyte >> i) & 0x1;
    Stateupdate(FSR1, FSR2, FSR3, plaintextbit);
  }

}


void mac_onebyte_ad(unsigned char *FSR1, unsigned char *FSR2, unsigned char *FSR3, unsigned char adbyte)
{
	unsigned char i;
	unsigned char adbit=0;

  for (i = 0; i < 8; i++)
  {
    adbit = (adbyte >> i) & 0x1;
    Stateupdate(FSR1, FSR2, FSR3, adbit);
  }

}

void encryption_part(unsigned char *c, unsigned long long *clen,
	const unsigned char *m, unsigned long long mlen, const unsigned char *npub,
	const unsigned char *k)
{
	unsigned char FSR1[LengthA], FSR2[LengthB], FSR3[LengthC];
	unsigned char key[CRYPTO_KEYBYTES * 8], iv[CRYPTO_NPUBBYTES * 8];
	unsigned long long i;

	memset(key, 0x0, CRYPTO_KEYBYTES * 8);
	memset(iv, 0x0, CRYPTO_NPUBBYTES * 8);

	for (i = 0; i < CRYPTO_KEYBYTES * 8; i++)
    key[i] = ((k[i / 8] >> (i % 8))) & 0x1;
	for (i = 0; i < CRYPTO_NPUBBYTES * 8; i++)
    iv[i] = ((npub[i / 8] >> (i % 8))) & 0x1;

	Initialization(key, iv, FSR1, FSR2, FSR3);

	
	for (i = 0; i < mlen; i++)
		enc_onebyte(FSR1, FSR2, FSR3, m[i], c + i);

}

void macgeneration_part(unsigned char *c, unsigned long long *clen, const unsigned char *m, unsigned long long mlen, 
	const unsigned char *ad, unsigned long long adlen, const unsigned char *npub,
	const unsigned char *k)
{
	unsigned char FSR1[LengthA], FSR2[LengthB], FSR3[LengthC];
	unsigned char key[CRYPTO_KEYBYTES * 8], iv[CRYPTO_NPUBBYTES * 8], constbit[8];
	unsigned char ks[64], mac[8], kstem = 0;
	unsigned long long i;

	memset(key, 0x0, CRYPTO_KEYBYTES * 8);
	memset(iv, 0x0, CRYPTO_NPUBBYTES * 8);
	memset(ks, 0x0, 64);
	memset(mac, 0x0, 8);
	memset(constbit, 0x0, 8);
	kstem = 0;

	for (i = 0; i < CRYPTO_KEYBYTES * 8; i++)
    key[i] = ((k[i / 8] >> (i % 8))) & 0x1;
	for (i = 0; i < CRYPTO_NPUBBYTES * 8; i++)
    iv[i] = ((npub[i / 8] >> (i % 8))) & 0x1;
	

	Initialization_mac(key, iv, FSR1, FSR2, FSR3);

	
	for (i = 0; i < adlen; i++)
		mac_onebyte_ad(FSR1, FSR2, FSR3, ad[i]);
	
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 0) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 8) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 16) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 24) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 32) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 40) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 48) & 0xff);

	
	for (i = 0; i < mlen; i++)
		mac_onebyte_message(FSR1, FSR2, FSR3, m[i]);

	
	Stateupdate(FSR1, FSR2, FSR3, 1);
	for (i = 1; i < FINA_ROUNDS; i++)
		Stateupdate(FSR1, FSR2, FSR3, 0);
	Keystreamgen_byte(64, ks, FSR1, FSR2, FSR3, mac);

	*clen = mlen + 8;
	memcpy(c + mlen, mac, 8);

}

unsigned char tagverification_part(const unsigned char *c, unsigned long long *clen, const unsigned char *m, unsigned long long mlen,
	const unsigned char *ad, unsigned long long adlen, const unsigned char *npub,
	const unsigned char *k)
{
	unsigned char FSR1[LengthA], FSR2[LengthB], FSR3[LengthC];
	unsigned char key[CRYPTO_KEYBYTES * 8], iv[CRYPTO_NPUBBYTES * 8], constbit[8];
	unsigned char ks[64], tag[8], kstem = 0,check = 0;
	unsigned long long i;

	memset(key, 0x0, 128);
	memset(iv, 0x0, CRYPTO_NPUBBYTES * 8);
	memset(ks, 0x0, 64);
	memset(tag, 0x0, 8);
	memset(constbit, 0x0, 8);
	kstem = 0;

	for (i = 0; i < CRYPTO_KEYBYTES * 8; i++)
    key[i] = ((k[i / 8] >> (i % 8))) & 0x1;
	for (i = 0; i < CRYPTO_NPUBBYTES * 8; i++)
    iv[i] = ((npub[i / 8] >> (i % 8))) & 0x1;


	Initialization_mac(key, iv, FSR1, FSR2, FSR3);

	for (i = 0; i < adlen; i++)
		mac_onebyte_ad(FSR1, FSR2, FSR3, ad[i]);
	
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 0) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 8) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 16) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 24) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 32) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 40) & 0xff);
	mac_onebyte_ad(FSR1, FSR2, FSR3, (adlen >> 48) & 0xff);


	
  for (i = 0; i < mlen; i++)
    mac_onebyte_message(FSR1, FSR2, FSR3, m[i]);
  


	
	Stateupdate(FSR1, FSR2, FSR3, 1);
	for (i = 1; i < FINA_ROUNDS; i++)
		Stateupdate(FSR1, FSR2, FSR3, 0);
	
	Keystreamgen_byte(64, ks, FSR1, FSR2, FSR3, tag);


	for (i = 0; i < 8; i++) 
    check |= (tag[i] ^ c[*clen - 8 + i]);
	return(check);

}

void decryption_part(const unsigned char *c, unsigned long long clen,
  unsigned char *m, unsigned long long *mlen, const unsigned char *npub,
  const unsigned char *k)
{
  unsigned char FSR1[LengthA], FSR2[LengthB], FSR3[LengthC];
  unsigned char key[CRYPTO_KEYBYTES * 8], iv[CRYPTO_NPUBBYTES * 8];
  unsigned long long i;

  memset(key, 0x0, CRYPTO_KEYBYTES * 8);
  memset(iv, 0x0, CRYPTO_NPUBBYTES * 8);

  for (i = 0; i < CRYPTO_KEYBYTES * 8; i++)
    key[i] = ((k[i / 8] >> (i % 8))) & 0x1;
  for (i = 0; i < CRYPTO_NPUBBYTES * 8; i++)
    iv[i] = ((npub[i / 8] >> (i % 8))) & 0x1;

  Initialization(key, iv, FSR1, FSR2, FSR3);

  *mlen = clen - 8;

  for (i = 0; i < *mlen; i++)
    dec_onebyte(FSR1, FSR2, FSR3, m + i, c[i]);

}


int crypto_aead_encrypt(
  unsigned char *c, unsigned long long *clen,
  const unsigned char *m, unsigned long long mlen,
  const unsigned char *ad, unsigned long long adlen,
  const unsigned char *nsec,
  const unsigned char *npub,
  const unsigned char *k
)
{
  encryption_part(c, clen, m, mlen, npub, k);

  macgeneration_part(c, clen, m, mlen, ad, adlen, npub, k);

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
  unsigned char check = 0;

  decryption_part(c, clen, m, mlen, npub, k);
  check = tagverification_part(c, &clen, m, *mlen, ad, adlen, npub, k);

  if (check == 0) return 0;
  else return -1;
}

