#include "crypto_sort.h"
#include "crypto_int32.h"
#define int32 crypto_int32
#include "crypto_uint32.h"
#define uint32 crypto_uint32
#include "crypto_uint16.h"
#define uint16 crypto_uint16

#define sortmedium_LIMIT 4294967295LL
/* assumes 2 <= n <= sortmedium_LIMIT */
static void sortmedium(int32 *x,uint32 n)
{
  int32 y[n];
  uint32 i,t0,t1,t2,t3,c0[256],c1[256],c2[256],c3[256];
#include "radix.inc"
}

static void sortlarge(int32 *x,long long n)
{
  int32 y[n];
  long long i,t0,t1,t2,t3,c0[256],c1[256],c2[256],c3[256];
#include "radix.inc"
}

void crypto_sort(void *array,long long n)
{
  if (n > sortmedium_LIMIT)
    sortlarge(array,n);
  else if (n > 1)
    sortmedium(array,n);
}
