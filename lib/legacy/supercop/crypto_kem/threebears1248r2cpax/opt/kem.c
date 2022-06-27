#include "namespace.h"
#include <string.h> /* for memcpy */
#include "crypto_kem.h"
#include "api.h"
#include "randombytes.h"
#include "threebears.h"

int crypto_kem_keypair(
    unsigned char *pk,
    unsigned char *sk
) {
    uint8_t seed[PQCRYPTO_KEM_THREEBEARS1248R2CPAX_IMPL_PRIV_KEYGEN_SEED_BYTES];
    randombytes(seed,sizeof(seed));
    pqcrypto_kem_threebears1248r2cpax_impl_priv_keygen(pk,sk,seed);
    secure_bzero(seed,sizeof(seed));
    return 0;
}

int crypto_kem_enc(
    unsigned char *ct,
    unsigned char *ss,
    const unsigned char *pk
) {
    unsigned char seed[PQCRYPTO_KEM_THREEBEARS1248R2CPAX_IMPL_PRIV_ENC_SEED_AND_IV_BYTES];
    randombytes(seed,sizeof(seed));
    pqcrypto_kem_threebears1248r2cpax_impl_priv_encapsulate(ss,ct,pk,seed);
    secure_bzero(seed,sizeof(seed));
    return 0;   
}

int crypto_kem_dec(
    unsigned char *ss,
    const unsigned char *ct,
    const unsigned char *sk
) {
    /* ThreeBears API says no aliasing, but libpqc requires aliasing */
    unsigned char tmp[PQCRYPTO_KEM_THREEBEARS1248R2CPAX_IMPL_PRIV_SHARED_SECRET_BYTES];
    pqcrypto_kem_threebears1248r2cpax_impl_priv_decapsulate(tmp,ct,sk);
    memcpy(ss,tmp,sizeof(tmp));
    secure_bzero(tmp,sizeof(tmp));
    return 0;
}

