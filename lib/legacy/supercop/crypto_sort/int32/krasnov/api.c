#include <stdint.h>
#include "crypto_sort.h"

#define qsort_AVX2 CRYPTO_NAMESPACE(qsort_AVX2)

extern int qsort_AVX2(int32_t *, int32_t *, unsigned long n);

static void sorts(int32_t *d, unsigned long n) {
  if (n <= 1)
    return;
  long i, j;
  for (i = 1; i < n; i++) {
    int32_t tmp = d[i];
    for (j = i; j >= 1 && tmp < d[j - 1]; j--)
      d[j] = d[j - 1];
    d[j] = tmp;
  }
}

static void my_qsort_AVX2(int32_t *unsorted_array, int32_t *tmp_array,
                   unsigned long n) {
  int new_n;
  while (n > 32) {
    new_n = qsort_AVX2(unsorted_array, tmp_array, n);
    n = n - new_n - 1;
    my_qsort_AVX2(&unsorted_array[n + 1], tmp_array, new_n);
  }
  sorts(unsorted_array, n);
}
  
void crypto_sort(void *x,long long n)
{
  int32_t tmp_array[n];
  my_qsort_AVX2(x,tmp_array,n);
}
