/**
 * @author Takuya HAYASHI (t-hayashi@eedept.kobe-u.ac.jp)
 * @brief An implementation of LOTUS-PKE
 */

#include "type.h"
#include "param.h"
#include "crypto.h"
#include "cpa-pke_opt.h"
#include "sampler.h"
#include "randombytes.h"
#include "pack_avx2.h"
#include "crypto_hash_sha512.h"
#include "crypto_encrypt.h"

#include <string.h>

int util_cmp_const(const void *a, const void *b, const size_t size);

/**
 * @brief LOTUS-PKE KeyGen algorithm
 * @param[out] pk a public key, contains matrices A and P
 * @param[out] sk a secret key, contains matrix S, and public key pk
 * @return 0 as success (and always)
 * @note pk is required in the decapsulation process, so embedded into sk in this implementation.
 */
int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk){
  U16 pkt[_LOTUS_LWE_DIM * _LOTUS_LWE_DIM + _LOTUS_LWE_DIM * _LOTUS_LWE_PT],
    skt[_LOTUS_LWE_DIM * _LOTUS_LWE_PT];

  /* Generate key pair */
  lotus_cpa_pke_keypair(pkt, skt);

  /* Pack key pair */
  pack_pk(pk, pkt);
  pack_sk(sk, skt);

  /* Append public key to secret key */
  memcpy(sk + _LOTUS_PACK_PRVKEY_BYTES, pk, _LOTUS_PACK_PUBKEY_BYTES);

  /* Cleanse memory positions stored secrets */
  OPENSSL_cleanse(skt, _LOTUS_LWE_DIM * _LOTUS_LWE_PT * sizeof(U16));
  
  return 0;
}

/**
 * @brief LOTUS-PKE Encrypt algorithm
 * @param[out] c a ciphertext corresponding to the message m
 * @param[out] clen ciphertext length (bytes)
 * @param[in] m a message to be encrypted
 * @param[in] mlen message length (bytes)
 * @param[in] pk a public key
 * @return 0 as success (and always)
 */
int crypto_encrypt(unsigned char *c, unsigned long long *clen, const unsigned char *m, const unsigned long long mlen, const unsigned char *pk){
  U8 sigma[_LOTUS_LWE_PT_BYTES + 1], digest[_LOTUS_HASH_DIGEST_BYTES], *buf;
  /* accessor */
  U8 *csym = c + _LOTUS_PACK_CT_BYTES;

  /* Encrypt message m by csym = Enc(m, H(sigma)) */
  randombytes(sigma, _LOTUS_LWE_PT_BYTES);
  sigma[_LOTUS_LWE_PT_BYTES] = _LOTUS_HASH_FLAG_G;
  crypto_hash_sha512(digest, sigma, _LOTUS_LWE_PT_BYTES + 1);
  *clen = crypto_symenc_encrypt(csym, m, mlen, digest); /* use truncated digest as a key */

  /* compute seed by H(sigma, csym) and set the seed to discrete gaussian sampler */
  buf = (U8*)malloc(_LOTUS_LWE_PT_BYTES + *clen + 1);
  memcpy(buf, sigma, _LOTUS_LWE_PT_BYTES);
  memcpy(buf + _LOTUS_LWE_PT_BYTES, csym, *clen);
  buf[_LOTUS_LWE_PT_BYTES + *clen] = _LOTUS_HASH_FLAG_H;
  crypto_hash_sha512(digest, buf, _LOTUS_LWE_PT_BYTES + *clen + 1);
  OPENSSL_cleanse(buf, _LOTUS_LWE_PT_BYTES + *clen + 1);
  free(buf);

  *clen += _LOTUS_PACK_CT_BYTES;
  sampler_set_seed(digest); /* use truncated digest as a seed */
  lotus_cpa_pke_enc_packed(c, sigma, pk); /* output ciphertext is already packed */

  /* Cleanse memory positions stored secrets */
  OPENSSL_cleanse(sigma, _LOTUS_LWE_PT_BYTES * 2 + 1);
  OPENSSL_cleanse(digest, _LOTUS_HASH_DIGEST_BYTES);

  return 0;
}

/**
 * @brief LOTUS-PKE Decrypt algorithm
 * @param[out] m a message corresponding to the ciphertext c
 * @param[out] mlen message length (bytes)
 * @param[in] c a ciphertext to be decrypted
 * @param[in] clen ciphertext length (bytes)
 * @param[in] sk a secret key
 * @return 0 if decrypted successfully, otherwise -1
 * @note Since current API does not allow to have public key as an argument, we embed it into secret key in this implementation.
 */
int crypto_encrypt_open(unsigned char *m, unsigned long long *mlen, const unsigned char *c, unsigned long long clen, const unsigned char *sk){
  U8 sigma[_LOTUS_LWE_PT_BYTES + 1], digest[_LOTUS_HASH_DIGEST_BYTES], *buf,
    cc[_LOTUS_PACK_CT_BYTES];
  /* accessors */
  const U8 *csym = c + _LOTUS_PACK_CT_BYTES,
    *pk = sk + _LOTUS_PACK_PRVKEY_BYTES;
  int r;

  lotus_cpa_pke_dec_packed(sigma, c, sk);

  /* Extract a seed from sigma. */
  clen -= _LOTUS_PACK_CT_BYTES;
  buf = (U8*)malloc(_LOTUS_LWE_PT_BYTES + clen + 1);
  memcpy(buf, sigma, _LOTUS_LWE_PT_BYTES);
  memcpy(buf + _LOTUS_LWE_PT_BYTES, csym, clen);
  buf[_LOTUS_LWE_PT_BYTES + clen] = _LOTUS_HASH_FLAG_H;
  crypto_hash_sha512(digest, buf, _LOTUS_LWE_PT_BYTES + clen + 1);
  OPENSSL_cleanse(buf, _LOTUS_LWE_PT_BYTES + clen + 1);
  free(buf);

  /* Validate the ciphertext. */
  sampler_set_seed(digest);
  lotus_cpa_pke_enc_packed(cc, sigma, pk);
  r = util_cmp_const(c, cc, _LOTUS_PACK_CT_BYTES) > 0;

  /* Decrypt a message m by m = Dec(m, H(sigma)) */
  sigma[_LOTUS_LWE_PT_BYTES] = _LOTUS_HASH_FLAG_G;
  crypto_hash_sha512(digest, sigma, _LOTUS_LWE_PT_BYTES + 1);
  *mlen = crypto_symenc_decrypt(m, csym, clen, digest); /* use truncated digest as a key */

  /* Cleanse memory positions stored secrets */
  OPENSSL_cleanse(sigma, _LOTUS_LWE_PT_BYTES * 2 + 1);
  OPENSSL_cleanse(digest, _LOTUS_HASH_DIGEST_BYTES);
  
  return -r;
}


/**
 * @brief constant-time comparison, from https://cryptocoding.net/index.php/Coding_rules
 */
int util_cmp_const(const void *a, const void *b, const size_t size) 
{
  const unsigned char *_a = (const unsigned char *) a;
  const unsigned char *_b = (const unsigned char *) b;
  unsigned char result = 0;
  size_t i;
 
  for (i = 0; i < size; i++) {
    result |= _a[i] ^ _b[i];
  }
  return result; /* returns 0 if equal, nonzero otherwise */
}
