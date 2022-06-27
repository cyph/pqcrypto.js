#include <gmp.h>
#include "gmpxx_import.h"

mpz_class gmpxx_import(const unsigned char *s,unsigned long long slen)
{
  mpz_class z;
  mpz_import(z.get_mpz_t(),slen,-1,1,0,0,s);
  return z;
}
