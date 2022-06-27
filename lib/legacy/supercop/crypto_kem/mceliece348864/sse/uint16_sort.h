#ifndef uint16_sort_h
#define uint16_sort_h

#define uint16_sort CRYPTO_NAMESPACE(uint16_sort)

#include <inttypes.h>

#define uint16_MINMAX(a,b) \
do { \
  uint16_t c = b - a; \
  c >>= 15; \
  c = -c; \
  c &= a ^ b; \
  a ^= c; \
  b ^= c; \
} while(0)

static void uint16_sort(uint16_t *x,long long n)
{
  long long top,p,q,r,i;

  if (n < 2) return;
  top = 1;
  while (top < n - top) top += top;

  for (p = top;p > 0;p >>= 1) {
    for (i = 0;i < n - p;++i)
      if (!(i & p))
        uint16_MINMAX(x[i],x[i+p]);
    i = 0;
    for (q = top;q > p;q >>= 1) {
      for (;i < n - q;++i) {
        if (!(i & p)) {
          int16_t a = x[i + p];
          for (r = q;r > p;r >>= 1)
            uint16_MINMAX(a,x[i+r]);
          x[i + p] = a;
        }
      }
    }
  }
}

#endif

