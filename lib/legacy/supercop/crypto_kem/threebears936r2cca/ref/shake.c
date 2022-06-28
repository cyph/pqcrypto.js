#include "namespace.h"
/* SHAKE implementation, compact */
#include "common.h"
#include "shake.h"

#define CSHAKE256_RATE (200-256/4)

static inline uint64_t rol(uint64_t x, int s) {
    return (x << s) | (x >> (64 - s));
}

static void keccakf(hash_ctx_t ctx) {
    /** Constants. **/
    static const uint8_t pi[24] = {
        10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
        15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
    };

    #define RC_B(x,n) ((((x##ull)>>n)&1)<<((1<<n)-1))
    #define RC_X(x) (RC_B(x,0)|RC_B(x,1)|RC_B(x,2)|RC_B(x,3)|RC_B(x,4)|RC_B(x,5)|RC_B(x,6))
    static const uint64_t RC[24] = {
        RC_X(0x01), RC_X(0x1a), RC_X(0x5e), RC_X(0x70), RC_X(0x1f), RC_X(0x21),
        RC_X(0x79), RC_X(0x55), RC_X(0x0e), RC_X(0x0c), RC_X(0x35), RC_X(0x26),
        RC_X(0x3f), RC_X(0x4f), RC_X(0x5d), RC_X(0x53), RC_X(0x52), RC_X(0x48),
        RC_X(0x16), RC_X(0x66), RC_X(0x79), RC_X(0x58), RC_X(0x21), RC_X(0x74)
    };
    
    uint64_t *a = ctx->state;
    for (unsigned x,y,i=0; i < 24; i++) {
        uint64_t b[5] = {0,0,0,0,0};
        for (y=0; y<25; y+=5)
            for (x=0; x<5; x++)
                b[x] ^= a[x + y];
        
        for (y=0; y<25; y+=5)
            for (x=0; x<5; x++)
                a[y + x] ^= b[(x + 4) % 5] ^ rol(b[(x + 1) % 5], 1);
        
        uint64_t t = a[1], u;
        for (y=x=0; x<24; x++) {
            u = a[pi[x]];
            y += x+1;
            a[pi[x]] = rol(t, y % 64);
            t = u;
        }
        
        for (y=0; y<25; y+=5) {
            for (x=0; x<5; x++)
                b[x] = a[y + x];
            for (x=0; x<5; x++)
                a[y + x] = b[x] ^ ((~b[(x + 1) % 5]) & b[(x + 2) % 5]);
        }
        a[0] ^= RC[i];
    }
    ctx->position = 0;
}

/* Update the hash with a single byte */
static inline void hash_update_byte (hash_ctx_t ctx, uint8_t b) {
    ctx->state[ctx->position/8] ^= ((uint64_t)b)<<((ctx->position%8)*8);
    if (++ctx->position == CSHAKE256_RATE) keccakf(ctx);
}

/* Update the hash with len bytes */
void hash_update(
    struct hash_ctx_s *ctx,
    const uint8_t *in, unsigned len
) {
    for (; len; len--, in++) hash_update_byte(ctx,*in);
}

/* Output len bytes from the hash */
void hash_output(hash_ctx_t ctx, uint8_t *out, unsigned len) {
    if (!ctx->squeezing) {
        hash_update_byte(ctx,0x04);
        ctx->position = CSHAKE256_RATE-1;
        hash_update_byte(ctx,0x80);
        ctx->squeezing = 1;
    }
    for (; len; len--, out++) {
        *out = ctx->state[ctx->position/8] >> ((ctx->position%8)*8);
        if (++ctx->position == CSHAKE256_RATE) keccakf(ctx);
    }
}

/* cSHAKE left encode */
static void left_encode(hash_ctx_t ctx, unsigned s) {
    unsigned n=0, ss=s;
    do { n++; ss>>=8; } while(ss);
    hash_update_byte(ctx,n);
    do { hash_update_byte(ctx,s>>(8 * --n)); } while (n);
}

void threebears_cshake_init(hash_ctx_t ctx) {
    const char S[] = "ThreeBears";
    memset(ctx,0,sizeof(hash_ctx_t));
    left_encode(ctx,CSHAKE256_RATE);
    left_encode(ctx,0);
    left_encode(ctx,8*strlen(S));
    hash_update(ctx,(const unsigned char *)S,strlen(S));
    keccakf(ctx);
}

