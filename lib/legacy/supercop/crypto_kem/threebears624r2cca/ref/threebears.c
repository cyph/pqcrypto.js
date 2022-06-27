#include "namespace.h"
/** ThreeBears reference implementation */
#include "common.h"
#include "shake.h"
#include "params.h"
#include "ring.h"
#include "threebears.h"

#define FEC_BYTES ((FEC_BITS+7)/8)
#define ENC_BITS  (ENC_SEED_BYTES*8 + FEC_BITS)
#if FEC_BITS
#include "melas_fec.h"
#endif

/* Securely erase size bytes from s */
#ifdef __STDC_LIB_EXT1__
void WEAK secure_bzero (void *s,size_t size) { memset_s(s,size,0,size); }
#else
void WEAK __attribute__((noinline)) secure_bzero (void *s,size_t size) { memset(s,0,size); }
#endif

enum { HASH_PURPOSE_UNIFORM=0, HASH_PURPOSE_KEYGEN=1, HASH_PURPOSE_ENCAPS=2, HASH_PURPOSE_PRF=3 };

/** Initialize the hash function with a given purpose */
static void threebears_hash_init(
    hash_ctx_t ctx,
    uint8_t purpose
) {
    threebears_cshake_init(ctx);
    const uint8_t pblock[15] ={
         VERSION, PRIVATE_KEY_BYTES, MATRIX_SEED_BYTES, ENC_SEED_BYTES,
         IV_BYTES, SHARED_SECRET_BYTES, LGX, DIGITS&0xFF, DIGITS>>8, DIM,
         VAR_TIMES_128-1, LPR_BITS, FEC_BITS, CCA, 0 /* padding */
    };
    hash_update(ctx,(const uint8_t*)pblock,sizeof(pblock));
    hash_update(ctx,&purpose,1);
}

/** Sample n gf_t's uniformly from a seed */
static void uniform(gf_t matrix, const uint8_t *seed, uint8_t iv) {
    uint8_t c[GF_BYTES];
    hash_ctx_t ctx;
    threebears_hash_init(ctx,HASH_PURPOSE_UNIFORM);
    hash_update(ctx,seed,MATRIX_SEED_BYTES);
    hash_update(ctx,&iv,1);
    hash_output(ctx,c,sizeof(c));
    expand(matrix,c);
}

/** The ThreeBears error distribution */
static slimb_t psi(uint8_t ci) {
    int sample=0, var=VAR_TIMES_128;
    for (; var > 64; var -= 64, ci<<=2) {
        sample += ((ci+64)>>8) + ((ci-64)>>8);
    }
    return sample + ((ci+var)>>8) + ((ci-var)>>8);
}

/** Sample a vector of n noise elements */
static void noise(gf_t x, const hash_ctx_t ctx, uint8_t iv) {
    uint8_t c[DIGITS];
    hash_ctx_t ctx2;
    memcpy(ctx2,ctx,sizeof(ctx2));
    hash_update(ctx2,&iv,1);
    hash_output(ctx2,c,DIGITS);
    for (unsigned i=0; i<DIGITS; i++) {
        x[i] = psi(c[i]) + modulus(i);
    }
}

/* Expand public key from private key */
void get_pubkey(uint8_t *pk, const uint8_t *seed) {
    hash_ctx_t ctx;
    threebears_hash_init(ctx,HASH_PURPOSE_KEYGEN);
    hash_update(ctx,seed,PRIVATE_KEY_BYTES);

    hash_ctx_t ctx2;
    memcpy(ctx2,ctx,sizeof(ctx2));
    hash_output(ctx2,pk,MATRIX_SEED_BYTES);
    
    gf_t sk_expanded[DIM],b,c;
    for (unsigned i=0; i<DIM; i++) {
        noise(sk_expanded[i],ctx,i);
    }
    for (unsigned i=0; i<DIM; i++) {
        noise(c,ctx,i+DIM);
        for (unsigned j=0; j<DIM; j++) {
            uniform(b,pk,i+DIM*j);
            mac(c,b,sk_expanded[j]);
        }
        contract(&pk[MATRIX_SEED_BYTES+i*GF_BYTES], c);
    }
}

/* Encapsulate a shared secret and return it */
void encapsulate(
    uint8_t *shared_secret,
    uint8_t *capsule,
    const uint8_t *pk,
    const uint8_t *seed
) {
    uint8_t *lpr_data = &capsule[GF_BYTES*DIM];
#if IV_BYTES
    uint8_t *iv = &lpr_data[(ENC_BITS*LPR_BITS+7)/8];
    memcpy(iv,&seed[ENC_SEED_BYTES],IV_BYTES);
#endif
    
    hash_ctx_t ctx;
    threebears_hash_init(ctx,HASH_PURPOSE_ENCAPS);
    hash_update(ctx,pk,MATRIX_SEED_BYTES);
    hash_update(ctx,seed,ENC_SEED_BYTES + IV_BYTES);
    
    gf_t sk_expanded[DIM],b,c;
    for (unsigned i=0; i<DIM; i++) {
        noise(sk_expanded[i],ctx,i);
    }
    for (unsigned i=0; i<DIM; i++) {
        noise(c,ctx,i+DIM);
        for (unsigned j=0; j<DIM; j++) {
            uniform(b,pk,j+DIM*i);
            mac(c,b,sk_expanded[j]);
        }
        contract(&capsule[i*GF_BYTES], c);
    }
    noise(c,ctx,2*DIM);
    
    /* Calculate approximate shared secret */
    for (unsigned i=0; i<DIM; i++) {
        expand(b, &pk[MATRIX_SEED_BYTES+i*GF_BYTES]);
        mac(c,b,sk_expanded[i]);
    }
    canon(c);
    
    uint8_t tbi[ENC_SEED_BYTES+FEC_BYTES];
#if CCA
    memcpy(tbi,seed,ENC_SEED_BYTES);
#else
    /* Hash the seed for proof reasons */
    hash_output(ctx,tbi,ENC_SEED_BYTES);
    threebears_hash_init(ctx,HASH_PURPOSE_ENCAPS);
    hash_update(ctx,pk,MATRIX_SEED_BYTES);
    hash_update(ctx,tbi,ENC_SEED_BYTES);
#if IV_BYTES
    hash_update(ctx,iv,IV_BYTES);
#endif
#endif
#if FEC_BITS
    melas_fec_set(&tbi[ENC_SEED_BYTES],tbi,ENC_SEED_BYTES);
#endif
    
    /* Export with rounding */
    for (unsigned i=0; i<ENC_BITS; i+=2) {
        limb_t h = tbi[i/8] >> (i%8);
        unsigned rlimb0 = (c[i/2]          >> (LGX-LPR_BITS)) + (h<<3);
        unsigned rlimb1 = (c[DIGITS-i/2-1] >> (LGX-LPR_BITS)) + ((h>>1)<<3);
        lpr_data[i/2] = (rlimb0 & 0xF) | rlimb1<<4;
    }
    
    /* Output shared secret */
    hash_output(ctx,shared_secret,SHARED_SECRET_BYTES);
}

/* Decapsulate a shared secret and return it */
void decapsulate(
    uint8_t shared_secret[SHARED_SECRET_BYTES],
    const uint8_t capsule[CAPSULE_BYTES],
    const uint8_t sk[PRIVATE_KEY_BYTES]
) {
    const uint8_t *lpr_data = &capsule[GF_BYTES*DIM];
    
    /* Calculate approximate shared secret */
    hash_ctx_t ctx;
    threebears_hash_init(ctx,HASH_PURPOSE_KEYGEN);
    hash_update(ctx,sk,PRIVATE_KEY_BYTES);
    
    gf_t ska,b,c={0};
    for (unsigned i=0; i<DIM; i++) {
        expand(b,&capsule[i*GF_BYTES]);
        noise(ska,ctx,i);
        mac(c,ska,b);
    }
    
    /* Recover seed from LPR data */
    uint8_t seed[ENC_SEED_BYTES+FEC_BYTES+IV_BYTES];
    canon(c);
    unsigned rounding = 1<<(LPR_BITS-1), out=0;
    for (signed i=ENC_BITS-1; i>=0; i--) {
        unsigned j = (i&1) ? (int)(DIGITS-i/2-1) : i/2;
        unsigned our_rlimb = c[j] >> (LGX-LPR_BITS-1);
        unsigned their_rlimb = lpr_data[i*LPR_BITS/8] >> ((i*LPR_BITS) % 8);
        unsigned delta =  their_rlimb*2 - our_rlimb + rounding;
        out |= ((delta>>LPR_BITS) & 1)<<(i%8);
        if (i%8==0) {
            seed[i/8] = out;
            out = 0;
        }
    }
#if FEC_BITS
    melas_fec_correct(seed,ENC_SEED_BYTES,&seed[ENC_SEED_BYTES]);
#endif
    
#if CCA
    /* Re-encapsulate and check; encapsulate will compute the shared secret */
    uint8_t pk[PUBLIC_KEY_BYTES], capsule2[CAPSULE_BYTES];
    get_pubkey(pk,sk);
#if IV_BYTES
    memcpy(&seed[ENC_SEED_BYTES],&lpr_data[(ENC_BITS*LPR_BITS+7)/8],IV_BYTES);
#endif
    encapsulate(shared_secret,capsule2,pk,seed);
    
    /* Check capsule == capsule2 in constant time */
    unsigned char ret = 0;
    for (unsigned i=0; i<CAPSULE_BYTES; i++) {
        ret |= capsule[i] ^ capsule2[i];
    }
    unsigned ok = ((int)ret - 1) >> 8;

    /* Calculate PRF key */
    uint8_t sep = 0xFF;
    uint8_t prfk[PRF_KEY_BYTES];
    hash_update(ctx,&sep,1);
    hash_output(ctx,prfk,PRF_KEY_BYTES);

    /* Calculate PRF */
    threebears_hash_init(ctx,HASH_PURPOSE_PRF);
    hash_update(ctx,prfk,PRF_KEY_BYTES);
    hash_update(ctx,capsule,CAPSULE_BYTES);
    uint8_t prfout[SHARED_SECRET_BYTES];
    hash_output(ctx,prfout,SHARED_SECRET_BYTES);

    for (unsigned i=0; i<SHARED_SECRET_BYTES; i++) {
        shared_secret[i] = (shared_secret[i] & ok) | (prfout[i] & ~ok);
    }
#else /* !CCA */
    /* Recalculate matrix seed */
    uint8_t matrix_seed[MATRIX_SEED_BYTES];
    hash_output(ctx,matrix_seed,MATRIX_SEED_BYTES);
    
    /* Re-run the key derivation from encaps */
    threebears_hash_init(ctx,HASH_PURPOSE_ENCAPS);
    hash_update(ctx,matrix_seed,MATRIX_SEED_BYTES);
    hash_update(ctx,seed,ENC_SEED_BYTES);
#if IV_BYTES
    hash_update(ctx,&lpr_data[(ENC_BITS*LPR_BITS+7)/8],IV_BYTES);
#endif
    hash_output(ctx,shared_secret,SHARED_SECRET_BYTES);
#endif
}
