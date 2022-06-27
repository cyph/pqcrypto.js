#include <gmp.h>

void gmp_import(mpz_t z,const unsigned char *s,unsigned long long slen)
{
  mpz_import(z,slen,-1,1,0,0,s);
}
