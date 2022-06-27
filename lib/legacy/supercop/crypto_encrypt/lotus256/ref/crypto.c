/**
 * @file crypto.c
 * @author Takuya HAYASHI (t-hayashi@eedept.kobe-u.ac.jp)
 * @brief Wrapper for implementations (currently OpenSSL) of crypto primitives
 */

#include "crypto.h"

/**
 * @brief wrapper for key-set-up function of symmetric key encryption 
 * @param[in] ctx context to be destoryed, can be NULL
 * @param[in] key a key to be used to initialize ctx, assumed to have _LOTUS_SYMENC_KEY_BYTES bytes
 * @return memory allocated ctx
 */
LOTUS_SYMENC_ctx *crypto_symenc_keysetup(LOTUS_SYMENC_ctx *ctx, const unsigned char *key){
  EVP_CIPHER_CTX_free(ctx);
  ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(ctx, _LOTUS_SYMENC_TYPE, NULL, key, NULL);
  return ctx;
}

/**
 * @brief wrapper for function to output key-stream, used to obtain cryptographically random bytes
 * @param[in,out] ctx context of symmetric key encryption
 * @param[out] stream key stream (or cryptographically random bytes) 
 * @param[in] len length of key stream (bytes)
 */
void crypto_symenc_keystream(LOTUS_SYMENC_ctx *ctx, unsigned char *stream, const unsigned int len){
  const unsigned char zero[_LOTUS_SYMENC_BLOCK_BYTES] = {0};
  unsigned char tmp[_LOTUS_SYMENC_BLOCK_BYTES];
  int i, q = len / _LOTUS_SYMENC_BLOCK_BYTES, r = len % _LOTUS_SYMENC_BLOCK_BYTES, olen;
  for(i = 0; i < q; ++i){
    EVP_EncryptUpdate(ctx, stream + i * _LOTUS_SYMENC_BLOCK_BYTES, &olen,
                      zero, _LOTUS_SYMENC_BLOCK_BYTES);
  }

  if(r){
    EVP_EncryptUpdate(ctx, tmp, &olen, zero, _LOTUS_SYMENC_BLOCK_BYTES);
  }
  
  for(i = 0; i < r; ++i){
    stream[q * _LOTUS_SYMENC_BLOCK_BYTES + i] = tmp[i];
  }
}

/**
 * @brief wrapper for symmetric key encryption
 * @param[out] c a ciphertext c = Enc(m, k)
 * @param[in] m a message to be encrypted
 * @param[in] mlen message length (bytes)
 * @param[in] k a key to be used for encryption
 * @return ciphertext length (bytes), must be equal to mlen on aes-ctr
 */
int crypto_symenc_encrypt(unsigned char *c, const unsigned char *m, const unsigned int mlen, const unsigned char *k){
  int clen, plen;
  EVP_CIPHER_CTX *ctx;
  ctx = EVP_CIPHER_CTX_new();
  /* Since the key is always fresh, iv is unnecessary. */  
  EVP_EncryptInit_ex(ctx, _LOTUS_SYMENC_TYPE, NULL, k, NULL);
  EVP_EncryptUpdate(ctx, c, &clen, m, mlen);
  EVP_EncryptFinal_ex(ctx, c + clen, &plen); /* always plen = 0 on ctr mode */
  EVP_CIPHER_CTX_cleanup(ctx);
  EVP_CIPHER_CTX_free(ctx);
  clen += plen;
  return clen;
}

/**
 * @brief wrapper for symmetric key encryption
 * @param[out] m a message m = Dec(c, k)
 * @param[in] c a ciphertext to be decrypted
 * @param[in] clen ciphertext length (bytes)
 * @param[in] k a key to be used for decryption
 * @return message length (bytes), must be equal to clen on aes-ctr
 */
int crypto_symenc_decrypt(unsigned char *m, const unsigned char *c, const unsigned int clen, const unsigned char *k){
  int mlen, plen;
  EVP_CIPHER_CTX *ctx;
  ctx = EVP_CIPHER_CTX_new();
  EVP_DecryptInit_ex(ctx, _LOTUS_SYMENC_TYPE, NULL, k, NULL);
  EVP_DecryptUpdate(ctx, m, &mlen, c, clen);
  EVP_DecryptFinal_ex(ctx, m, &plen);  /* always plen = 0 on ctr mode */
  EVP_CIPHER_CTX_cleanup(ctx);
  EVP_CIPHER_CTX_free(ctx);
  mlen += plen;
  return mlen;
}
