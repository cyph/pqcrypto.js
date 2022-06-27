#include <immintrin.h>
#include "crypto_core.h"

#define p 953
#define q 6343

#include "crypto_decode_953xint16.h"
#define crypto_decode_pxint16 crypto_decode_953xint16
#include "crypto_encode_953xint16.h"
#define crypto_encode_pxint16 crypto_encode_953xint16

#include "crypto_int16.h"
typedef crypto_int16 Fq;

/* out = 3*in in Rq */
int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  int i = p-16;

  __m256i save = _mm256_loadu_si256((__m256i *) (inbytes+2*i));
    /* in case outbytes = inbytes */

  for (;;) {
    do {
      __m256i x = _mm256_loadu_si256((__m256i *) inbytes);
      __m256i xneg;
      x = _mm256_mullo_epi16(x,_mm256_set1_epi16(3));
      x = _mm256_sub_epi16(x,_mm256_set1_epi16((q+1)/2));
      xneg = _mm256_srai_epi16(x,15);
      x = _mm256_add_epi16(x,_mm256_set1_epi16(q)&xneg);
      xneg = _mm256_srai_epi16(x,15);
      x = _mm256_add_epi16(x,_mm256_set1_epi16(q)&xneg);
      x = _mm256_sub_epi16(x,_mm256_set1_epi16((q-1)/2));
      _mm256_storeu_si256((__m256i *) outbytes,x);

      inbytes += 32;
      outbytes += 32;
      i -= 16;
    } while (i >= 0);
    if (i <= -16) break;
    inbytes += 2*i;
    outbytes += 2*i;
    _mm256_storeu_si256((__m256i *) outbytes,save);
  }

  return 0;
}
