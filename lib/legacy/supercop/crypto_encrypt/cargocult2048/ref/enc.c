#include "crypto_kem_rsa2048.h"
#define crypto_kem_enc crypto_kem_rsa2048_enc
#define crypto_kem_BYTES crypto_kem_rsa2048_BYTES
#define crypto_kem_CIPHERTEXTBYTES crypto_kem_rsa2048_CIPHERTEXTBYTES

#include "crypto_aead_aes256gcmv1.h"
#define crypto_aead_encrypt crypto_aead_aes256gcmv1_encrypt
#define crypto_aead_KEYBYTES crypto_aead_aes256gcmv1_KEYBYTES
#define crypto_aead_NPUBBYTES crypto_aead_aes256gcmv1_NPUBBYTES
#define crypto_aead_ABYTES crypto_aead_aes256gcmv1_ABYTES

#include "crypto_encrypt.h"
#if crypto_aead_KEYBYTES != crypto_kem_BYTES
#error "aead_KEYBYTES must be kem_BYTES"
#endif
#if crypto_encrypt_BYTES != crypto_aead_ABYTES + crypto_aead_NPUBBYTES + crypto_kem_CIPHERTEXTBYTES
#error "encrypt_BYTES must be aead_ABYTES + aead_NPUBBYTES + kem_CIPHERTEXTBYTES"
#endif

#include "randombytes.h"
#include <string.h>

static const unsigned char nsec[1];
static const unsigned char ad[1];

int crypto_encrypt(
  unsigned char *c,unsigned long long *clen,
  const unsigned char *m,const unsigned long long mlen,
  const unsigned char *pk
)
{
  unsigned char c2[crypto_kem_CIPHERTEXTBYTES];
  unsigned char k[crypto_kem_BYTES];
  unsigned char nonce[crypto_aead_NPUBBYTES];

  if (crypto_kem_enc(c2,k,pk) < 0) goto error;
  randombytes(nonce,sizeof nonce);

  crypto_aead_encrypt(c,clen,m,mlen,ad,0,nsec,nonce,k);
  c += *clen;

  memcpy(c,nonce,sizeof nonce);
  c += sizeof nonce;
  *clen += sizeof nonce;

  memcpy(c,c2,sizeof c2);
  c += crypto_kem_CIPHERTEXTBYTES;
  *clen += crypto_kem_CIPHERTEXTBYTES;
  
  return 0;

  error:
  *clen = -1;
  return -1;
}
