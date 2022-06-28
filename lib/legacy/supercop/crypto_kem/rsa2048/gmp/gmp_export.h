#ifndef gmp_export_h
#define gmp_export_h

#include <gmp.h>

#define gmp_export crypto_kem_rsa2048_gmp_export

extern int gmp_export(unsigned char *,unsigned long long,mpz_t);

#endif
