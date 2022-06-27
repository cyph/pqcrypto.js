#include <stdint.h>
#include <algorithm>
#include "crypto_sort.h"

void crypto_sort(void *array,long long n)
{
  int32_t *x = (int32_t *) array;
  std::sort(x,x + n);
}
