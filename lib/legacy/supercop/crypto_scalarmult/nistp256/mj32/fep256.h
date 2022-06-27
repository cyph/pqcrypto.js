#ifndef _FEP256_H____
#define _FEP256_H____

#include "fe.h"

#define fep256_mul CRYPTO_NAMESPACE(fep256_mul)
#define fep256_sq CRYPTO_NAMESPACE(fep256_sq)
#define fep256_mul2 CRYPTO_NAMESPACE(fep256_mul2)
#define fep256_mulx_ CRYPTO_NAMESPACE(fep256_mulx_)
#define fep256_inv CRYPTO_NAMESPACE(fep256_inv)
#define fep256_add CRYPTO_NAMESPACE(fep256_add)
#define fep256_sub CRYPTO_NAMESPACE(fep256_sub)
#define fep256_tobytes CRYPTO_NAMESPACE(fep256_tobytes)
#define fep256_frombytes CRYPTO_NAMESPACE(fep256_frombytes)
#define fep256_isnonzero CRYPTO_NAMESPACE(fep256_isnonzero)

extern void fep256_mul(fe, const fe, const fe);
extern void fep256_sq(fe, const fe);
extern void fep256_mul2(fe, const fe);
extern void fep256_mulx_(fe, const fe, const crypto_uint64);
#define fep256_mul3(x, y) fep256_mulx_(x, y, 3)
#define fep256_mul6(x, y) fep256_mulx_(x, y, 6)
extern void fep256_inv(fe, const fe);
extern void fep256_add(fe, const fe, const fe);
extern void fep256_sub(fe, const fe, const fe);

extern void fep256_tobytes(unsigned char *, const fe);
extern void fep256_frombytes(fe, const unsigned char *);

extern int fep256_isnonzero(const fe);

#endif
