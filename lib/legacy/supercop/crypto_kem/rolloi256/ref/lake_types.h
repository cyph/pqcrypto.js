/** 
 * \file lake_types.h
 * \brief File defining publicKey, secretKey and cipherText types for LAKE
 */

#ifndef LAKE_TYPES_H
#define LAKE_TYPES_H

#include "ffi_vec.h"

typedef struct secretKey {
	ffi_vec F;
	ffi_vec x;
	ffi_vec y;
} secretKey;

typedef struct publicKey {
	ffi_vec h;
} publicKey;

typedef struct ciphertext{
	ffi_vec syndrom;
} ciphertext;

#endif