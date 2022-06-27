#include "crypto_sort.h"
#include "crypto_int32.h"
#define int32 crypto_int32

#include "int32_minmax.inc"

void crypto_sort(void *array,long long n)
{
  long long top,p,q,r,i;
  int32 *x = array;

  if (n < 2) return;
  top = 1;
  while (top < n - top) top += top;

  for (p = top;p > 0;p >>= 1) {
    for (i = 0;i < n - p;++i)
      if (!(i & p))
        int32_MINMAX(x[i],x[i+p]);
    i = 0;
    for (q = top;q > p;q >>= 1) {
      for (;i < n - q;++i) {
        if (!(i & p)) {
          int32 a = x[i + p];
          for (r = q;r > p;r >>= 1)
            int32_MINMAX(a,x[i+r]);
	  x[i + p] = a;
	}
      }
    }
  }
}
