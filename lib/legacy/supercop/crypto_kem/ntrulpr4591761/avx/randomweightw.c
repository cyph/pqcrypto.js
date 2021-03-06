#include "params.h"
#include "randombytes.h"
#include "crypto_sort_int32.h"
#include "small.h"
#include "crypto_stream_aes256ctr.h"

static const unsigned char n[16] = {0};

void small_seeded_weightw(small *f,const unsigned char *k)
{
  crypto_int32 r[768];
  int i;

  crypto_stream_aes256ctr((unsigned char *) r,sizeof r,n,k);
  for (i = 0;i < p;++i) r[i] ^= 0x80000000;

  for (i = 0;i < w;++i) r[i] &= -2;
  for (i = w;i < p;++i) r[i] = (r[i] & -3) | 1;
  crypto_sort_int32(r,p);
  for (i = 0;i < p;++i) f[i] = ((small) (r[i] & 3)) - 1;
  for (i = p;i < 768;++i) f[i] = 0;
}

void small_random_weightw(small *f)
{
  unsigned char k[32];
  randombytes(k,32);
  small_seeded_weightw(f,k);
}
