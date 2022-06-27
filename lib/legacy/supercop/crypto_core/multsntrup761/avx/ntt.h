#ifndef ntt_H
#define ntt_H

#include <stdint.h>

#define ntt512_7681 CRYPTO_NAMESPACE(ntt512_7681)
#define ntt512_10753 CRYPTO_NAMESPACE(ntt512_10753)
#define invntt512_7681 CRYPTO_NAMESPACE(invntt512_7681)
#define invntt512_10753 CRYPTO_NAMESPACE(invntt512_10753)

extern void ntt512_7681(int16_t *,int);
extern void ntt512_10753(int16_t *,int);
extern void invntt512_7681(int16_t *,int);
extern void invntt512_10753(int16_t *,int);

#endif
