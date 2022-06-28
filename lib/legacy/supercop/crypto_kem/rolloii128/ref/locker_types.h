#ifndef LOCKER_TYPES_H
#define LOCKER_TYPES_H

#include "ffi_vec.h"
#include "parameters.h"

typedef struct secretKey {
	ffi_vec F;
	ffi_vec x;
	ffi_vec y;
} secretKey;

typedef struct publicKey {
	ffi_vec h;
} publicKey;

typedef struct ciphertext{
	ffi_vec u;
	unsigned char v[SHA512_BYTES];
	unsigned char d[SHA512_BYTES];
} ciphertext;

#endif