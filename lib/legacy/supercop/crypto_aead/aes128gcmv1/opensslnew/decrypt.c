#include <openssl/evp.h>
#include "crypto_aead.h"

int crypto_aead_decrypt(
  unsigned char *m,unsigned long long *mlen,
  unsigned char *nsec,
  const unsigned char *c,unsigned long long clen,
  const unsigned char *ad,unsigned long long adlen,
  const unsigned char *npub,
  const unsigned char *k
)
{
  int result = 0;
  EVP_CIPHER_CTX *x = 0;
  int outlen = 0;

  if (adlen > 536870912) goto error;
  /* OpenSSL needs to put lengths into an int */
  if (clen > 536870912) goto error;

  if (clen < 16) goto forgery;
  clen -= 16;

  x = EVP_CIPHER_CTX_new(); if (!x) goto error;
  if (!EVP_DecryptInit_ex(x,EVP_aes_128_gcm(),0,0,0)) goto error;
  if (!EVP_CIPHER_CTX_ctrl(x,EVP_CTRL_GCM_SET_IVLEN,12,0)) goto error;
  if (!EVP_CIPHER_CTX_ctrl(x,EVP_CTRL_GCM_SET_TAG,16,(unsigned char *) c + clen)) goto error;
  if (!EVP_DecryptInit_ex(x,0,0,k,npub)) goto error;
  if (!EVP_DecryptUpdate(x,0,&outlen,ad,adlen)) goto error;
  if (!EVP_DecryptUpdate(x,m,&outlen,c,clen)) goto error;
  if (!EVP_DecryptFinal_ex(x,m + clen,&outlen)) goto error;
  *mlen = clen;

  goto cleanup;
  forgery: result = -1; goto cleanup;
  error: result = -111; /* XXX: maybe out of memory, maybe forgery! */
  cleanup:
  if (x) { EVP_CIPHER_CTX_free(x); x = 0; }

  return result;
}
