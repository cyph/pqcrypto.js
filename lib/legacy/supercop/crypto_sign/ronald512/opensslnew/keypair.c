#include <stddef.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <string.h>
#include "sizes.h"

int crypto_sign_keypair(unsigned char *pk,unsigned char *sk)
{
  const BIGNUM *x;
  BIGNUM *b;
  RSA *r;
  unsigned char *out = sk;
  int len;

  memset(sk,0,SECRETKEY_BYTES);
  memset(pk,0,PUBLICKEY_BYTES);

  r = RSA_new();
  if (!r) return -1;

  b = BN_new();
  if (!b) goto error;

  if (!BN_set_word(b,3)) { BN_free(b); goto error; }

  if (!RSA_generate_key_ex(r,MODULUS_BYTES*8,b,0)) { BN_free(b); goto error; }

  BN_free(b);

  if (RSA_size(r) != MODULUS_BYTES) goto error;

  x = RSA_get0_n(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  x = RSA_get0_e(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  x = RSA_get0_d(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  x = RSA_get0_p(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  x = RSA_get0_q(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  x = RSA_get0_dmp1(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  x = RSA_get0_dmq1(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  x = RSA_get0_iqmp(r);
  len = BN_num_bytes(x); if (len > MODULUS_BYTES) goto error;
  out += MODULUS_BYTES; BN_bn2bin(x,out - len);

  memcpy(pk,sk,MODULUS_BYTES);
  RSA_free(r);
  return 0;

  error:
  RSA_free(r);
  return -1;
}
