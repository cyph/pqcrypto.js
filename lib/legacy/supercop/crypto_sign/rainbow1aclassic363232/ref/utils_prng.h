/// @file utils_prng.h
/// @brief the interface for adapting PRNG functions.
///
///

#ifndef _UTILS_PRNG_H_
#define _UTILS_PRNG_H_


#ifdef  __cplusplus
extern  "C" {
#endif



#ifndef _DEBUG_
#include "rng.h"

typedef AES256_CTR_DRBG_struct prng_t;

#else
#include "hash_utils.h"

typedef
struct prng_context {
    unsigned char buf[_HASH_LEN];
    unsigned used;
} prng_t;

#endif



int prng_set(prng_t *ctx, const void *prng_seed, unsigned long prng_seedlen);

int prng_gen(prng_t *ctx, unsigned char *out, unsigned long outlen);




#ifdef  __cplusplus
}
#endif



#endif // _UTILS_PRNG_H_


