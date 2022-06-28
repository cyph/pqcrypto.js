/**
 * \file hash.h
 * \brief A generic declaration of SHA512
 */

#ifndef HASH_H
#define HASH_H

#include <stdint.h>

void sha512(unsigned char* output, unsigned char* input, uint64_t size);

#endif
