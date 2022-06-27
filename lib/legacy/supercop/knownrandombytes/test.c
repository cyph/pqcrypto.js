#include "randombytes.h"

#define TESTS 21
unsigned char x[65536];
unsigned long long freq[256];

void randombytes_callback(unsigned char *x,unsigned long long xlen)
{
}

int main()
{
  int i;
  int j;

  for (j = 0;j <= TESTS;++j) {
    randombytes(x,sizeof x);
    for (i = 0;i < 256;++i) freq[i] = 0;
    for (i = 0;i < sizeof x;++i) ++freq[255 & (int) x[i]];
    for (i = 0;i < 256;++i) if (!freq[i]) return 111;
  }

  return 0;
}
