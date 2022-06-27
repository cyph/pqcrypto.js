#include "crypto_core.h"

#define p 857
#define q 5167

#include "crypto_decode_857xint16.h"
#define crypto_decode_pxint16 crypto_decode_857xint16
#include "crypto_encode_857xint16.h"
#define crypto_encode_pxint16 crypto_encode_857xint16

#include "crypto_int16.h"
typedef crypto_int16 Fq;

/* out = 3*in in Rq */
int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  Fq f[p];
  int i;

  crypto_decode_pxint16(f,inbytes);
  for (i = 0;i < p;++i) {
    Fq x = f[i];
    x *= 3; /* (-3q+3)/2 ... (3q-3)/2 */
    x -= (q+1)/2; /* -2q+1 ... q-2 */
    x += q&(x>>15); /* -q+1 ... q-1 */
    x += q&(x>>15); /* 0 ... q-1 */
    x -= (q-1)/2; /* -(q-1)/2 ... (q-1)/2 */
    f[i] = x;
  }
  crypto_encode_pxint16(outbytes,f);

  return 0;
}
