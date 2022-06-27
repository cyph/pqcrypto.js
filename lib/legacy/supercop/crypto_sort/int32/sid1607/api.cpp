#include <stdint.h>
#include <algorithm>
#include "merge_sort.h"
#include "crypto_sort.h"

static int flaginit = 0;

void crypto_sort(void *array,long long n)
{
  int32_t *x = (int32_t *) array;

  if (!flaginit) {
    initialize();
    flaginit = 1;
  }

  if (n % 64) {
    std::sort(x,x + n);
    return;
  }

  __attribute__((aligned(32))) int32_t y[n];

  std::pair<int *,int *> result = merge_sort(x,y,n);
  if (result.first == y) {
    long long i;
    for (i = 0;i < n;++i) x[i] = y[i];
  }
}
