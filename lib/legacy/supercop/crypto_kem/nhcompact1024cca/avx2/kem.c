#include <string.h>
#include "crypto_kem.h"
#include "api.h"
#include "cpapke.h"
#include "params.h"
#include "randombytes.h"
#include "fips202.h"
#include "verify.h"

/*************************************************
* Name:        crypto_kem_keypair
*
* Description: Generates public and private key
*              for CCA secure NewHope key encapsulation
*              mechanism
*
* Arguments:   - unsigned char *pk: pointer to output public key (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
*              - unsigned char *sk: pointer to output private key (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  cpapke_keypair(pk, sk);                                                   /* First put the actual secret key into sk */
  sk += NEWHOPECMPCT_CPAPKE_SECRETKEYBYTES;
                              
  memcpy(sk,pk,NEWHOPECMPCT_CPAPKE_PUBLICKEYBYTES);                              /* Append the public key for re-encryption */
  sk += NEWHOPECMPCT_CPAPKE_PUBLICKEYBYTES;

  shake256(sk, NEWHOPECMPCT_SYMBYTES, pk, NEWHOPECMPCT_CPAPKE_PUBLICKEYBYTES);        /* Append the hash of the public key */
  sk += NEWHOPECMPCT_SYMBYTES;

  randombytes(sk, NEWHOPECMPCT_SYMBYTES);                                        /* Append the value s for pseudo-random output on reject */

  return 0;
}

/*************************************************
* Name:        crypto_kem_enc
*
* Description: Generates cipher text and shared
*              secret for given public key
*
* Arguments:   - unsigned char *ct:       pointer to output cipher text (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
*              - unsigned char *ss:       pointer to output shared secret (an already allocated array of CRYPTO_BYTES bytes)
*              - const unsigned char *pk: pointer to input public key (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
  unsigned char k_coins_d[3*NEWHOPECMPCT_SYMBYTES];                                                /* Will contain key, coins, qrom-hash */
  unsigned char buf[2*NEWHOPECMPCT_SYMBYTES];

  randombytes(buf,NEWHOPECMPCT_SYMBYTES);

  shake256(buf,NEWHOPECMPCT_SYMBYTES,buf,NEWHOPECMPCT_SYMBYTES);                                        /* Don't release system RNG output */
  shake256(buf+NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, pk, NEWHOPECMPCT_CCAKEM_PUBLICKEYBYTES);        /* Multitarget countermeasure for coins + contributory KEM */
  shake256(k_coins_d, 3*NEWHOPECMPCT_SYMBYTES, buf, 2*NEWHOPECMPCT_SYMBYTES);

  cpapke_enc(ct, buf, pk, k_coins_d+NEWHOPECMPCT_SYMBYTES);                                        /* coins are in k_coins_d+NEWHOPE_SYMBYTES */

  memcpy(ct+NEWHOPECMPCT_CPAPKE_CIPHERTEXTBYTES, k_coins_d+2*NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES);  /* copy Targhi-Unruh hash into ct */

  shake256(k_coins_d+NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, ct, NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in k_coins_d with h(c) */
  shake256(ss, NEWHOPECMPCT_SYMBYTES, k_coins_d, 2*NEWHOPECMPCT_SYMBYTES);                              /* hash concatenation of pre-k and h(c) to ss */
  return 0;
}

/*************************************************
* Name:        crypto_kem_dec
*
* Description: Generates shared secret for given
*              cipher text and private key
*
* Arguments:   - unsigned char *ss:       pointer to output shared secret (an already allocated array of CRYPTO_BYTES bytes)
*              - const unsigned char *ct: pointer to input cipher text (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
*              - const unsigned char *sk: pointer to input private key (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 for sucess or -1 for failure
*
* On failure, ss will contain a randomized value.
**************************************************/
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
  int fail;
  unsigned char ct_cmp[NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES];
  unsigned char buf[2*NEWHOPECMPCT_SYMBYTES];
  unsigned char k_coins_d[3*NEWHOPECMPCT_SYMBYTES];                                                /* Will contain key, coins, qrom-hash */
  const unsigned char *pk = sk+NEWHOPECMPCT_CPAPKE_SECRETKEYBYTES;

  cpapke_dec(buf, ct, sk);
  
  memcpy(buf+NEWHOPECMPCT_SYMBYTES,sk+NEWHOPECMPCT_CCAKEM_SECRETKEYBYTES-2*NEWHOPECMPCT_SYMBYTES,NEWHOPECMPCT_SYMBYTES); /* Use hash of pk stored in sk */
  shake256(k_coins_d, 3*NEWHOPECMPCT_SYMBYTES, buf, 2*NEWHOPECMPCT_SYMBYTES);

  cpapke_enc(ct_cmp, buf, pk, k_coins_d+NEWHOPECMPCT_SYMBYTES);                                    /* coins are in k_coins_d+NEWHOPE_SYMBYTES */

  memcpy(ct_cmp+NEWHOPECMPCT_CPAPKE_CIPHERTEXTBYTES,k_coins_d+2*NEWHOPECMPCT_SYMBYTES,NEWHOPECMPCT_SYMBYTES);

  fail = verify(ct, ct_cmp, NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES);

  shake256(k_coins_d+NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, ct, NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in k_coins_d with h(c)  */
  cmov(k_coins_d, sk+NEWHOPECMPCT_CCAKEM_SECRETKEYBYTES-NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, fail); /* Overwrite pre-k with z on re-encryption failure */
  shake256(ss, NEWHOPECMPCT_SYMBYTES, k_coins_d, 2*NEWHOPECMPCT_SYMBYTES);                              /* hash concatenation of pre-k and h(c) to k */

  return 0;
}
