#include <stdint.h>
#include "crypto_core.h"

extern void CRYPTO_NAMESPACE(internal)(void *,const void *);

int crypto_core(unsigned char *rbytes,const unsigned char *abytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  uint16_t *a = (void *) abytes;
  uint16_t *r = (void *) rbytes;
  uint16_t aclamp[704];
  int i;

  for (i = 0;i < 701;++i) {
    uint16_t ai = a[i] & 3;
    ai &= (ai - 3) >> 5;
    aclamp[i] = ai;
  }
  for (i = 701;i < 704;++i) aclamp[i] = 0;

  CRYPTO_NAMESPACE(internal)(rbytes,aclamp);

  for (i = 701;i < 704;++i) r[i] = 0;
  return 0;
}
