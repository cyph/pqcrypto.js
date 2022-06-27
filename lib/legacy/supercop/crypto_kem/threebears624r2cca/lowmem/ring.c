#include "namespace.h"
/** Optimized ring arithmetic */
#include "ring.h"
#include "brg_endian.h" /* from Keccak Code Package */
#include "crypto_declassify.h"


/** Karatsuba inner loop */
static inline void __attribute__((always_inline)) triplemac(
    dslimb_t *ac0, dslimb_t *ac1, dslimb_t *ac2,
    limb_t a0, limb_t a1, limb_t b0, limb_t b1
) {
    /* Multiply and accumulate three times into dslimbs, for Karatsuba */
#if defined(__BMI2__) && (WORD_BITS==64)
    /* TODO: non-BMI Intel intrinsics for this */
    limb_t a0lo = *ac0, a0hi = *ac0>>64;
    limb_t a1lo = *ac1, a1hi = *ac1>>64;
    limb_t a2lo = *ac2, a2hi = *ac2>>64;
    limb_t tlo, thi;
    
    __asm__("mulxq %[b0], %[tlo], %[thi]"
    "\n\taddq  %[tlo], %[a0lo]"
    "\n\tadcq  %[thi], %[a0hi]"
    "\n\taddq  %[a0],  %[a1]"
    "\n\tmulxq %[b1], %[tlo], %[thi]"
    "\n\taddq  %[tlo], %[a1lo]"
    "\n\tadcq  %[thi], %[a1hi]"
    "\n\tsubq  %[b1], %[b0]"
    "\n\timulq %[a0]"
    "\n\taddq  %%rax, %[a2lo]"
    "\n\tadcq  %%rdx, %[a2hi]"
        : [b0]"+&a"(b0), [a1]"+&d"(a1), [tlo]"=&r"(tlo), [thi]"=&r"(thi)
        , [a0lo]"+r"(a0lo), [a0hi]"+r"(a0hi)
        , [a1lo]"+r"(a1lo), [a1hi]"+r"(a1hi)
        , [a2lo]"+r"(a2lo), [a2hi]"+r"(a2hi)
        : [b1]"r"(b1), [a0]"r"(a0)
        : "cc"
    );
    
    *ac0 = a0lo | (dlimb_t)a0hi<<64;
    *ac1 = a1lo | (dlimb_t)a1hi<<64;
    *ac2 = a2lo | (dlimb_t)a2hi<<64;
#else
    *ac0 += (dlimb_t)b0 * a1;
    *ac1 += (dlimb_t)b1 * (a1+a0);
    *ac2 += (dslimb_t)(slimb_t)(b0-b1) * (slimb_t)a0;
#endif
}

/** Multiply and accumulate c += a*b */
void mac(gf_t c, const gf_t a, const gf_t b) {
    /* One-level Karatsuba */
    /* PERF: for some reason whether this is signed or unsigned makes
     * a huge difference, depending on platform.
     */
    unsigned i,j; 
    dslimb_t accum0 = 0, accum1 = 0;
    for (i=0; i<NLIMBS/2; i++) {
        dslimb_t accum2 = 0;
        
        for (j=0; j<=i; j++) {
            limb_t b0 = b[j], b1 = b[j+NLIMBS/2];
            limb_t a1 = a[i-j+NLIMBS/2], a0 = a[i-j];
            triplemac(&accum0,&accum1,&accum2,a0,a1,b0,b1);
        }
        
        accum0 = accum0 - accum2 - accum2;
        
        for (; j<NLIMBS/2; j++) {
            limb_t b0 = b[j], b1 = b[j+NLIMBS/2];
            limb_t a1 = a[i+NLIMBS-j], a0 = a[i+NLIMBS/2-j];
            triplemac(&accum1,&accum2,&accum0,a0,a1,b0,b1);
        }
    
        accum0 += accum2 + c[i];
        accum1 += accum2 + c[i+NLIMBS/2];
        c[i]          = accum0 & LMASK;
        c[i+NLIMBS/2] = accum1 & LMASK;

        accum0 >>= LBITS;
        accum1 >>= LBITS;
    }

    accum0 += accum1;
    accum0 += c[NLIMBS/2];
    accum1 += c[0];
    c[NLIMBS/2] = accum0 & LMASK;
    c[0] = accum1 & LMASK;

    accum0 >>= LBITS;
    accum1 >>= LBITS;

    c[NLIMBS/2+1] += accum0;
    c[1] += accum1;
}

#if PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN
typedef struct { limb_t x; } __attribute__((packed)) unaligned_limb_t;
static inline void write_limb(uint8_t out[sizeof(limb_t)], limb_t x) {
    ((unaligned_limb_t *)out)->x = x;
}

static inline limb_t read_limb(const uint8_t *in) {
    return ((const unaligned_limb_t *)in)->x;
}

/** Serialize a gf_t to bytes */
void contract(uint8_t ch[GF_BYTES], gf_t ll) {
    canon(ll);
    signed bbits=0;
    unsigned j=0;
    limb_t buffer=0;
    for (unsigned i=0;i<NLIMBS;i++) {
        if ((unsigned)bbits+LBITS >= 8*sizeof(limb_t)) {
            limb_t tmp = ll[i];
            buffer |= tmp<<bbits;
            write_limb(&ch[j], buffer);
            j += sizeof(limb_t);
            buffer = tmp>>(8*sizeof(limb_t)-bbits);
            bbits += LBITS - 8*sizeof(limb_t);
        } else {
            buffer |= ll[i]<<bbits;
            bbits += LBITS;
        }
    }
    for (; bbits>0; bbits-=8,buffer>>=8) {
        ch[j++] = buffer;
    }
}

/** Deserialize a gf_t from bytes */
void expand(gf_t ll, const uint8_t ch[GF_BYTES]) {
    limb_t buffer=0;
    for (unsigned i=0,j=0,bbits=0;;i+=sizeof(limb_t)) {
        if (i+sizeof(limb_t) >= GF_BYTES) {
            unsigned target = GF_BYTES-sizeof(limb_t);
            limb_t tmp = read_limb(&ch[target]) >> 8*(i-target);
            ll[j++] = (buffer | tmp<<bbits) & LMASK;
            return;
        } else {
            limb_t tmp = read_limb(&ch[i]);
            ll[j++] = (buffer | tmp<<bbits) & LMASK;
            buffer = tmp>>(LBITS-bbits);
            bbits += 8*sizeof(limb_t) - LBITS;
            while (bbits >= LBITS) {
                ll[j++] = buffer & LMASK;
                buffer >>= LBITS;
                bbits -= LBITS;
            }
        }
    }
}
#else
/** Serialize a gf_t to bytes */
void contract(uint8_t ch[GF_BYTES], gf_t ll) {
    canon(ll);
    for (unsigned i=0; i<GF_BYTES; i++) {
        ch[i] = bits_starting_at(ll,8*i);
    }
}

/** Deserialize a gf_t from bytes */
void expand(gf_t ll, const uint8_t ch[GF_BYTES]) {
    limb_t buffer=0;
    for (unsigned i=0,j=0,bbits=0;i<GF_BYTES;i++) {
        limb_t tmp = ch[i];
        buffer |= tmp<<bbits;
        bbits += 8;
        if (bbits >= LBITS) {
            ll[j++] = buffer & LMASK;
            buffer = tmp>>(LBITS-(bbits-8));
            bbits = bbits-LBITS;
        }
    }
}
#endif

/** Reduce a gf_t to canonical form, i.e. strictly less than N. */
void canon(gf_t c) {
    const limb_t DELTA = (limb_t)1<<(LBITS-1);
    
    /* Reduce to 0..2p */
    slimb_t hi = c[NLIMBS-1] - DELTA;
    c[NLIMBS-1] = (hi & LMASK) + DELTA;
    c[NLIMBS/2] += hi>>LBITS;
        
    /* Strong reduce.  First subtract modulus */
    dslimb_t scarry = hi>>LBITS;
    for (unsigned i=0; i<NLIMBS; i++) {
        scarry = scarry + (slimb_t)c[i] - modulus(i);
        c[i] = scarry & LMASK;
        scarry >>= LBITS;
    }

    /* add it back */
    dlimb_t carry = 0;
    for (unsigned i=0; i<NLIMBS; i++) {
        carry = carry + c[i] + (scarry & modulus(i));
        c[i] = carry & LMASK;
        carry >>= LBITS;
    }

    dslimb_t result = carry+scarry;
    crypto_declassify(&result,sizeof result);
    assert(result==0);
}
