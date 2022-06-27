#ifndef _FE_H____
#define _FE_H____

#include "crypto_uint32.h"
#include "crypto_uint64.h"

typedef crypto_uint32 fe[8];
typedef crypto_uint64 fel[16];

#define fe_0 CRYPTO_NAMESPACE(fe_0)
#define fe_1 CRYPTO_NAMESPACE(fe_1)
#define fe_copy CRYPTO_NAMESPACE(fe_copy)
#define fe_cswap CRYPTO_NAMESPACE(fe_cswap)
#define fe_cmov CRYPTO_NAMESPACE(fe_cmov)
#define fe_mul_ CRYPTO_NAMESPACE(fe_mul_)
#define fe_sq_ CRYPTO_NAMESPACE(fe_sq_)
#define fe_reducesmall CRYPTO_NAMESPACE(fe_reducesmall)

extern void fe_0(fe);
extern void fe_1(fe);
extern void fe_copy(fe, const fe);
extern void fe_cswap(fe, fe, crypto_uint32);
extern void fe_cmov(fe, const fe, crypto_uint32);

extern void fe_mul_(fel, const fe, const fe);
extern void fe_sq_(fel, const fe);

extern void fe_reducesmall(fe, const fe, const crypto_uint64);


#endif

