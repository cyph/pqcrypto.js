#ifndef gmp_import_h
#define gmp_import_h

#include <gmp.h>

#define gmp_import crypto_kem_pqrsa15_gmp_import

extern void gmp_import(mpz_t,const unsigned char *,unsigned long long);

#endif
