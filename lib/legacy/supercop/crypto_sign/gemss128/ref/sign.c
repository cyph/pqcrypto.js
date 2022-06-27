#include "api.h"
#include <string.h>
#include "sign_keypairHFE.hpp"
#include "signHFE.hpp"
#include "sign_openHFE.h"

#if SUPERCOP
    #include "crypto_sign.h"
#endif


int crypto_sign_keypair(unsigned char *pk,unsigned char *sk)
{
    return sign_keypairHFE((UINT*)pk,(UINT*)sk);
}

int crypto_sign(
  unsigned char *sm,unsigned long long *smlen,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *sk)
{
    *smlen=mlen+CRYPTO_BYTES;
    memcpy(sm+CRYPTO_BYTES,m,(size_t)mlen);
    return signHFE(sm,m,(size_t)mlen,(UINT*)sk);
}

int crypto_sign_open(
  unsigned char *m,unsigned long long *mlen,
  const unsigned char *sm,unsigned long long smlen,
  const unsigned char *pk)
{
    int result;
    *mlen=smlen-CRYPTO_BYTES;
    result=sign_openHFE(sm+CRYPTO_BYTES,(size_t)(*mlen),sm,(UINT*)pk);
    /* For compatibily with SUPERCOP, the memcpy is done only after sign_open */
    memcpy(m,sm+CRYPTO_BYTES,(size_t)(*mlen));
    return result;
}
