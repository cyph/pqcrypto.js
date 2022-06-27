#include <immintrin.h>
#include "crypto_core.h"

#include "crypto_int8.h"
#include "crypto_int16.h"
#define int8 crypto_int8
#define int16 crypto_int16
#include "crypto_encode_int16.h"

#include "params.h"

/* out = little-endian weight of bottom bits of in */
int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  int8 *in = (void *) inbytes;
  int i;
  __m256i sum,sumhi;
  int16 weight;

  sum = _mm256_loadu_si256((__m256i *) (in+p-32));
  sum &= endingmask;

  for (i = p-32;i >= 0;i -= 32) {
    __m256i bits = _mm256_loadu_si256((__m256i *) in);
    bits &= _mm256_set1_epi8(1);
    sum = _mm256_add_epi8(sum,bits);
    in += 32;
  }

  /* sum is 32xint8; want to add these int8 */
  sumhi = _mm256_srli_epi16(sum,8);
  sum &= _mm256_set1_epi16(0xff);
  sum = _mm256_add_epi16(sum,sumhi);

  /* sum is 16xint16; want to add these int16 */
  sum = _mm256_hadd_epi16(sum,sum);
  /* want sum[0]+sum[1]+sum[2]+sum[3]+sum[8]+sum[9]+sum[10]+sum[11] */
  sum = _mm256_hadd_epi16(sum,sum);
  /* want sum[0]+sum[1]+sum[8]+sum[9] */
  sum = _mm256_hadd_epi16(sum,sum);
  /* want sum[0]+sum[8] */

  weight = _mm256_extract_epi16(sum,0);
  weight += _mm256_extract_epi16(sum,8);

  crypto_encode_int16(outbytes,&weight);
  return 0;
}
