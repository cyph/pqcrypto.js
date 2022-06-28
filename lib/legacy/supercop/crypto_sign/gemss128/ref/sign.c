#include "api.h"
#include "crypto_sign.h"
#include <string.h>
#include "encrypt_keypairHFE.hpp"
#include "signHFE.hpp"
#include "sign_openHFE.h"


int crypto_sign_keypair(unsigned char *pk,unsigned char *sk)
{
    return sign_keypairHFE((UINT*)pk,(UINT*)sk);
}

int crypto_sign(
  unsigned char *sm,unsigned long long *smlen,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *sk)
{
    *smlen=mlen+SIZE_SIGNATURE_BYTES;
    memmove(sm+SIZE_SIGNATURE_BYTES,m,(size_t)mlen);
    return signHFE((UINT*)sm,m,(size_t)mlen,(UINT*)sk);
}

int crypto_sign_open(
  unsigned char *m,unsigned long long *mlen,
  const unsigned char *sm,unsigned long long smlen,
  const unsigned char *pk)
{
    int result;
    *mlen=smlen-SIZE_SIGNATURE_BYTES;
    result = sign_openHFE(sm+SIZE_SIGNATURE_BYTES,(size_t)(*mlen),(UINT*)sm,(UINT*)pk);
    memmove(m,sm+SIZE_SIGNATURE_BYTES,(size_t)(*mlen));
    return result;
}
