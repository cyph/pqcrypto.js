#ifndef gmp_export_h
#define gmp_export_h

#include <gmp.h>

#define gmp_export CRYPTO_NAMESPACE(gmp_export)

extern int gmp_export(unsigned char *,unsigned long long,mpz_t);

#endif
