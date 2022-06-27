#include "namespace.h"
/** Public header */
#ifndef __THREE_BEARS_THREEBEARS936R2CPAX_H__
#define __THREE_BEARS_THREEBEARS936R2CPAX_H__

#include <stdint.h>
#include <stddef.h> /* for size_t */

#define PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_KEYGEN_SEED_BYTES 40
#define PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_PRIVATE_KEY_BYTES 1194
#define PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_SHARED_SECRET_BYTES 32
#define PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_ENC_SEED_AND_IV_BYTES 32
#define PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_PUBLIC_KEY_BYTES 1194
#define PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_CAPSULE_BYTES 1307

/**
 * Expand a secret seed to a public/private keypair.
 *
 * @param[out] pk The public key.
 * @param[in] sk The private key, which must be uniformly random.
 */
void pqcrypto_kem_threebears936r2cpax_impl_priv_keygen (
    uint8_t pk[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_PUBLIC_KEY_BYTES],
    uint8_t sk[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_PRIVATE_KEY_BYTES],
    const uint8_t seed[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_KEYGEN_SEED_BYTES]
);
    
/**
 * Create a shared secret using a random seed and another party's public key.
 *
 * Input and output parameters may not alias.
 *
 * @param[out] shared_secret The shared secret key.
 * @param[out] capsule A ciphertext to send to the other party.
 * @param[in] pk The other party's public key.
 * @param[in] seed A random seed.
 */
void pqcrypto_kem_threebears936r2cpax_impl_priv_encapsulate (
    uint8_t shared_secret[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_SHARED_SECRET_BYTES],
    uint8_t capsule[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_CAPSULE_BYTES],
    const uint8_t pk[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_PUBLIC_KEY_BYTES],
    const uint8_t seed[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_ENC_SEED_AND_IV_BYTES]
);

/**
 * Extract the shared secret from a capsule using the private key.
 * Has a negligible but nonzero probability of failure.
 *
 * Input and output parameters may not alias.
 *
 * @param[out] shared_secret The shared secret.
 * @param[in] capsule The capsule produced by encapsulate_cca2.
 * @param[in] sk The private key.
 */
void pqcrypto_kem_threebears936r2cpax_impl_priv_decapsulate ( 
    uint8_t shared_secret[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_SHARED_SECRET_BYTES],
    const uint8_t capsule[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_CAPSULE_BYTES],
    const uint8_t sk[PQCRYPTO_KEM_THREEBEARS936R2CPAX_IMPL_PRIV_PRIVATE_KEY_BYTES]
);

void secure_bzero (void *s,size_t size);

#endif /*__THREE_BEARS_THREEBEARS936R2CPAX_H__*/

