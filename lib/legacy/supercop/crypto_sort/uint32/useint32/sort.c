#include "crypto_sort_int32.h"
#include "crypto_sort.h"
#include "crypto_uint32.h"

/* can save time by vectorizing xor loops */
/* can save time by integrating xor loops with int32_sort */

void crypto_sort(void *array,long long n)
{
  crypto_uint32 *x = array;
  long long j;
  for (j = 0;j < n;++j) x[j] ^= 0x80000000;
  crypto_sort_int32(array,n);
  for (j = 0;j < n;++j) x[j] ^= 0x80000000;
}
