#include <stdint.h>
#include "crypto_sort.h"
#include "aspas.h"

void crypto_sort(void *x,long long n)
{
  aspas::sort((int32_t *) x,n);
}
