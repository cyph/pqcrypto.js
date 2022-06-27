#include <string.h>
#include "api.h"
#include "cpapke.h"
#include "params.h"
#include "randombytes.h"
#include "fips202.h"
#include "verify.h"

 
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  size_t i;

  cpapke_keypair(pk, sk);                                                   /* First put the actual secret key into sk */
  sk += NEWHOPECMPCT_CPAPKE_SECRETKEYBYTES;

  for(i=0;i<NEWHOPECMPCT_CPAPKE_PUBLICKEYBYTES;i++)                              /* Append the public key for re-encryption */
    sk[i] = pk[i];
  sk += NEWHOPECMPCT_CPAPKE_PUBLICKEYBYTES;

  shake256(sk, NEWHOPECMPCT_SYMBYTES, pk, NEWHOPECMPCT_CPAPKE_PUBLICKEYBYTES);        /* Append the hash of the public key */
  sk += NEWHOPECMPCT_SYMBYTES;

  randombytes(sk, NEWHOPECMPCT_SYMBYTES);                                        /* Append the value z for pseudo-random output on reject */

  return 0;
}


int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
  unsigned char krq[3*NEWHOPECMPCT_SYMBYTES];                                                /* Will contain key, coins, qrom-hash */
  unsigned char buf[2*NEWHOPECMPCT_SYMBYTES];
  int i;

  randombytes(buf,NEWHOPECMPCT_SYMBYTES);
  
  shake256(buf,NEWHOPECMPCT_SYMBYTES,buf,NEWHOPECMPCT_SYMBYTES);                                  /* Don't release system RNG output */
  shake256(buf+NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, pk, NEWHOPECMPCT_CCAKEM_PUBLICKEYBYTES);  /* Multitarget countermeasure for coins + contributory KEM */
  shake256(krq, 3*NEWHOPECMPCT_SYMBYTES, buf, 2*NEWHOPECMPCT_SYMBYTES);

  cpapke_enc(ct, buf, pk, krq+NEWHOPECMPCT_SYMBYTES);                                        /* coins are in krq+NEWHOPE_SYMBYTES */

  for(i=0;i<NEWHOPECMPCT_SYMBYTES;i++)
    ct[i+NEWHOPECMPCT_CPAPKE_CIPHERTEXTBYTES] = krq[i+2*NEWHOPECMPCT_SYMBYTES];                   /* copy Targhi-Unruh hash into ct */

  shake256(krq+NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, ct, NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in krq with h(c) */
  shake256(ss, NEWHOPECMPCT_SYMBYTES, krq, 2*NEWHOPECMPCT_SYMBYTES);                              /* hash concatenation of pre-k and h(c) to ss */
  return 0;
}

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
  int i, fail;
  unsigned char cmp[NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES];
  unsigned char buf[2*NEWHOPECMPCT_SYMBYTES];
  unsigned char krq[3*NEWHOPECMPCT_SYMBYTES];                                         /* Will contain key, coins, qrom-hash */
  const unsigned char *pk = sk+NEWHOPECMPCT_CPAPKE_SECRETKEYBYTES;

  cpapke_dec(buf, ct, sk);

  for(i=0;i<NEWHOPECMPCT_SYMBYTES;i++)                                                /* Use hash of pk stored in sk */
    buf[NEWHOPECMPCT_SYMBYTES+i] = sk[NEWHOPECMPCT_CCAKEM_SECRETKEYBYTES-2*NEWHOPECMPCT_SYMBYTES+i];
  shake256(krq, 3*NEWHOPECMPCT_SYMBYTES, buf, 2*NEWHOPECMPCT_SYMBYTES);

  cpapke_enc(cmp, buf, pk, krq+NEWHOPECMPCT_SYMBYTES);                                /* coins are in krq+NEWHOPE_SYMBYTES */

  for(i=0;i<NEWHOPECMPCT_SYMBYTES;i++)
    cmp[i+NEWHOPECMPCT_CPAPKE_CIPHERTEXTBYTES] = krq[i+2*NEWHOPECMPCT_SYMBYTES];

  fail = verify(ct, cmp, NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES);

  shake256(krq+NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, ct, NEWHOPECMPCT_CCAKEM_CIPHERTEXTBYTES); /* overwrite coins in krq with h(c)  */
  cmov(krq, sk+NEWHOPECMPCT_CCAKEM_SECRETKEYBYTES-NEWHOPECMPCT_SYMBYTES, NEWHOPECMPCT_SYMBYTES, fail); /* Overwrite pre-k with z on re-encryption failure */
  shake256(ss, NEWHOPECMPCT_SYMBYTES, krq, 2*NEWHOPECMPCT_SYMBYTES);                       /* hash concatenation of pre-k and h(c) to k */

  return 0;
}
