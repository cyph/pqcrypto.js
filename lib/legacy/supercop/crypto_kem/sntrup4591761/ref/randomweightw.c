/* See https://ntruprime.cr.yp.to/software.html for detailed documentation. */

#include "params.h"
#include "randombytes.h"
#include "crypto_sort_int32.h"
#include "small.h"

void small_random_weightw(small *f)
{
  crypto_int32 r[p];
  int i;

  for (i = 0;i < p;++i) r[i] = small_random32();
  for (i = 0;i < w;++i) r[i] &= -2;
  for (i = w;i < p;++i) r[i] = (r[i] & -3) | 1;
  crypto_sort_int32(r,p);
  for (i = 0;i < p;++i) f[i] = ((small) (r[i] & 3)) - 1;
}
