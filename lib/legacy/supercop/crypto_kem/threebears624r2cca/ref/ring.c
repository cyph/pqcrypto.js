#include "namespace.h"
/** Ring arithmetic implementation */
#include "ring.h"
#include "crypto_declassify.h"

/** Multiply and accumulate c += a*b */
void mac(gf_t c, const gf_t a, const gf_t b) {
    /* Reference non-Karatsuba MAC */
    dslimb_t accum[2*DIGITS] = {0};
    unsigned i,j;
    
    /* Initialize accumulator = unclarify(c) */
    for (i=0; i<DIGITS; i++) {
        accum[i+DIGITS/2] = c[i];
    }
    
    /* Multiply */
    for (i=0; i<DIGITS; i++) {
        for (j=0; j<DIGITS; j++) {
            accum[i+j] += (dlimb_t)a[i]*b[j];
        }
    }
    
    /* Clarify and reduce */
    for (i=0; i<DIGITS/2; i++) {
        accum[i+DIGITS/2] -= accum[i];
        accum[i+DIGITS]   += accum[i];
        accum[i+DIGITS/2] += accum[i+3*DIGITS/2];
        accum[i+DIGITS]   += accum[i+3*DIGITS/2];
    }
    
    /* Carry propagate */
    dslimb_t chain = accum[3*DIGITS/2-1];
    accum[3*DIGITS/2-1] = chain & LMASK;
    chain >>= LGX;
    accum[DIGITS] += chain;
    for (i=DIGITS/2; i<3*DIGITS/2; i++) {
        chain += accum[i];
        c[i-DIGITS/2] = chain & LMASK;
        chain >>= LGX;
    }
    c[0] += chain;
    c[DIGITS/2] += chain;
}

/** Reduce a gf_t to canonical form, i.e. strictly less than N. */
void canon(gf_t c) {
    const limb_t DELTA = (limb_t)1<<(LGX-1);
    
    /* Reduce to 0..2p */
    slimb_t hi = c[DIGITS-1] - DELTA;
    c[DIGITS-1] = (hi & LMASK) + DELTA;
    c[DIGITS/2] += hi>>LGX;
        
    /* Strong reduce.  First subtract modulus */
    dslimb_t scarry = hi>>LGX;
    for (unsigned i=0; i<DIGITS; i++) {
        scarry = scarry + (slimb_t)c[i] - modulus(i);
        c[i] = scarry & LMASK;
        scarry >>= LGX;
    }

    /* add it back */
    dlimb_t carry = 0;
    for (unsigned i=0; i<DIGITS; i++) {
        carry = carry + c[i] + (scarry & modulus(i));
        c[i] = carry & LMASK;
        carry >>= LGX;
    }

    dslimb_t result = carry+scarry;
    crypto_declassify(&result,sizeof result);
    assert(result==0);
}

/** Serialize a gf_t to bytes */
void contract(uint8_t ch[GF_BYTES], gf_t a) {
    canon(a);
    for (unsigned i=0; i<GF_BYTES; i++) {
        unsigned pos = (i*8)/LGX;
        ch[i] = a[pos] >> ((i*8)%LGX);
        if (i < GF_BYTES-1) {
            ch[i] |= a[pos+1] << (LGX-((i*8)%LGX));
        }
    }
}

/** Deserialize a gf_t from bytes */
void expand(gf_t ll, const uint8_t ch[GF_BYTES]) {
    limb_t buffer=0;
    for (unsigned i=0,j=0,bbits=0;i<GF_BYTES;i++) {
        limb_t tmp = ch[i];
        buffer |= tmp<<bbits;
        bbits += 8;
        if (bbits >= LGX) {
            ll[j++] = buffer & LMASK;
            buffer = tmp>>(LGX-(bbits-8));
            bbits = bbits-LGX;
        }
    }
}
