#ifndef SHA_2_H
#define SHA_2_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"

#define SHA2_NAMESPACE(s) KYBER_NAMESPACE(s)

#define sha256 SHA2_NAMESPACE(sha256)
void sha256(uint8_t out[32], const uint8_t *in, size_t inlen);
#define sha512 SHA2_NAMESPACE(sha512)
void sha512(uint8_t out[64], const uint8_t *in, size_t inlen);

#endif
