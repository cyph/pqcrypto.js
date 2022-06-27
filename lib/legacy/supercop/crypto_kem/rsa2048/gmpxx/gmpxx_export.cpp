#include <gmp.h>
#include "gmpxx_export.h"

void gmpxx_export(unsigned char *s,unsigned long long slen,mpz_class z)
{
  if (mpz_sizeinbase(z.get_mpz_t(),256) > slen) throw "gmpxx_export out of space";
  for (unsigned long long i = 0;i < slen;++i) s[i] = 0;
  mpz_export(s,0,-1,1,0,0,z.get_mpz_t());
}
