#include <stdint.h>
#include "modp.h"



// (mod q) x
int to_mod_q(modp_context_t* ctx, int x)
{
    x %= ctx->q;
    if (x < 0) x += ctx->q;
    return x;
}

/*  (int) x
 *   - Needed as in optimized variant the representations
 *     are different
 */
int to_int(modp_context_t* ctx, int x)
{
    return (x%ctx->q);
}

// c=a*b mod q
int mul_mod_q(modp_context_t* ctx, int a, int b)
{
    return ((int64_t)a * b) % ctx->q;
}

// c=a+b mod q
int add_mod_q(modp_context_t* ctx, int a, int b)
{
    return (a + b) % ctx->q;
}

// d=a*b+c mod q
int mul_add_mod_q(modp_context_t* ctx, int a, int b, int c)
{
    return ((int64_t)a * b + c) % ctx->q;
}

// c=a-b mod q
int sub_mod_q(modp_context_t* ctx, int a, int b)
{
    return (a + ctx->q - b) % ctx->q;
}

/* c=base**exp mod q
 *   - Only need naive version as it is just used for set up purposes
 */
int exp_mod_q(modp_context_t* ctx, int base, int exp)
{
    int value = to_mod_q(ctx, 1);
    while (exp != 0)
    {
        if (exp & 1) value = mul_mod_q(ctx, value, base);
        base = mul_mod_q(ctx, base, base);
        exp = exp >> 1;
    }

    return value;
}

/*
 * q, log_256 q
 */
modp_context_t mod133121   = { 133121, 3 };
modp_context_t mod184321   = { 184321, 3 };
modp_context_t mod12521473 = { 12521473, 3 };
modp_context_t mod48181249 = { 48181249, 4 };
modp_context_t mod44802049 = { 44802049, 4 };
modp_context_t mod16900097 = { 16900097, 4 };

