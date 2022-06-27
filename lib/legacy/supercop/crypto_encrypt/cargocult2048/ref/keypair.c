#include "crypto_kem_rsa2048.h"
#define crypto_kem_keypair crypto_kem_rsa2048_keypair
#define crypto_kem_SECRETKEYBYTES crypto_kem_rsa2048_SECRETKEYBYTES
#define crypto_kem_PUBLICKEYBYTES crypto_kem_rsa2048_PUBLICKEYBYTES

#include "crypto_encrypt.h"
#if crypto_encrypt_SECRETKEYBYTES != crypto_kem_SECRETKEYBYTES
#error "encrypt_SECRETKEYBYTES must match kem_SECRETKEYBYTES"
#endif
#if crypto_encrypt_PUBLICKEYBYTES != crypto_kem_PUBLICKEYBYTES
#error "encrypt_PUBLICKEYBYTES must match kem_PUBLICKEYBYTES"
#endif

int crypto_encrypt_keypair(unsigned char *pk,unsigned char *sk)
{
  return crypto_kem_keypair(pk,sk);
}
