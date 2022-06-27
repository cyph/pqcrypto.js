#include "crypto_kem_rsa2048.h"
#define crypto_kem_dec crypto_kem_rsa2048_dec
#define crypto_kem_BYTES crypto_kem_rsa2048_BYTES
#define crypto_kem_CIPHERTEXTBYTES crypto_kem_rsa2048_CIPHERTEXTBYTES

#include "crypto_aead_aes256gcmv1.h"
#define crypto_aead_decrypt crypto_aead_aes256gcmv1_decrypt
#define crypto_aead_KEYBYTES crypto_aead_aes256gcmv1_KEYBYTES
#define crypto_aead_NPUBBYTES crypto_aead_aes256gcmv1_NPUBBYTES
#define crypto_aead_ABYTES crypto_aead_aes256gcmv1_ABYTES

#include "crypto_encrypt.h"

#include <string.h>

static const unsigned char ad[1];

int crypto_encrypt_open(
  unsigned char *m,unsigned long long *mlen,
  const unsigned char *c,unsigned long long clen,
  const unsigned char *sk
)
{
  unsigned char k[crypto_kem_BYTES];
  unsigned char nonce[crypto_aead_NPUBBYTES];
  unsigned char nsec[1];
  int result;

  if (clen < crypto_kem_CIPHERTEXTBYTES) goto formaterror;
  clen -= crypto_kem_CIPHERTEXTBYTES;

  result = crypto_kem_dec(k,c + clen,sk);
  if (result < 0) goto error;

  if (clen < sizeof nonce) goto formaterror;
  clen -= sizeof nonce;
  memcpy(nonce,c + clen,sizeof nonce);

  result = crypto_aead_decrypt(m,mlen,nsec,c,clen,ad,0,nonce,k);
  if (result < 0) goto error;

  return 0;

  formaterror:
  result = -100;

  error:
  *mlen = -1;
  return result;
}
