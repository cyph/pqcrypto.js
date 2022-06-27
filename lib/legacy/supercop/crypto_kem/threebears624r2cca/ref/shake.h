#include "namespace.h"
/** Ring arithmetic header */
#ifndef __THREEBEARS_SHAKE_H__
#define __THREEBEARS_SHAKE_H__
#include "common.h"

typedef struct hash_ctx_s {
    uint64_t state[25];
    uint8_t position, squeezing;
} hash_ctx_s, hash_ctx_t[1];

/* Init cSHAKE(N="",S="ThreeBears") */
void INTERNAL threebears_cshake_init(hash_ctx_t ctx);

/* Update hash function by adding len bytes from in */
void INTERNAL hash_update(struct hash_ctx_s *ctx, const uint8_t *in, unsigned len);

/* Output len bytes to out */
void INTERNAL hash_output(hash_ctx_t ctx, uint8_t *out, unsigned len);

/* Destroy hash */
static inline void hash_destroy(hash_ctx_t ctx) {
    secure_bzero(ctx,sizeof(hash_ctx_t));
}

#endif /*__THREEBEARS_SHAKE_H__*/
