#include "api.h"
#if SUPERCOP
    #include "crypto_sign.h"
#endif
#include <string.h>

#include "sign_keypairHFE.h"
#include "signHFE.h"
#include "sign_openHFE.h"


/**
 * @brief   Generation of a keypair for the digital signature.
 * @param[out]   pk  The public key.
 * @param[out]   sk  The secret key.
 * @return  Zero if the function runs correctly, non-zero else.
 */
#if SUPERCOP
int crypto_sign_keypair(unsigned char *pk,unsigned char *sk)
#else
int PREFIX_NAME(crypto_sign_keypair)(unsigned char *pk,unsigned char *sk)
#endif
{
    return sign_keypairHFE((UINT*)pk,(UINT*)sk);
}


/**
 * @brief   Generation of the signature of a document.
 * @param[out]  sm  The signature, the document will be stored at the end.
 * @param[out]  smlen   The length of the signature (including the document).
 * @param[in]   m   The document.
 * @param[in]   mlen    The length of the document.
 * @param[in]   sk  The secret key.
 * @return  Zero if the function runs correctly, non-zero else.
 */
#if SUPERCOP
int crypto_sign(
#else
int PREFIX_NAME(crypto_sign)(
#endif
  unsigned char *sm,unsigned long long *smlen,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *sk)
{
    *smlen=mlen+CRYPTO_BYTES;
    memcpy(sm+CRYPTO_BYTES,m,(size_t)mlen);
    return signHFE(sm,m,(size_t)mlen,(UINT*)sk);
}


/**
 * @brief   Verification of the signature of a document.
 * @param[out]   m   The original document.
 * @param[out]   mlen    The length of the document.
 * @param[in]  sm  The signature, the document is stored at the end.
 * @param[in]  smlen   The length of the signature (including the document).
 * @param[in]   pk  The public key.
 * @return  Zero if the function runs correctly, non-zero else.
 */
#if SUPERCOP
int crypto_sign_open(
#else
int PREFIX_NAME(crypto_sign_open)(
#endif
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


