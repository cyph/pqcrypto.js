#include <gmp.h>
#include "gmp_export.h"

int gmp_export(unsigned char *s,unsigned long long slen,mpz_t z)
{
  unsigned long long i;
  if (mpz_sizeinbase(z,256) > slen) return -1;
  for (i = 0;i < slen;++i) s[i] = 0;
  mpz_export(s,0,-1,1,0,0,z);
  return 0;
}
