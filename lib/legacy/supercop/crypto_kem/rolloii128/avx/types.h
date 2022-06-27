/** 
 * \file rolloII_types.h
 * \brief File defining publicKey, secretKey and cipherText types for ROLLO-II
 */

#ifndef LAKE_TYPES_H
#define LAKE_TYPES_H

#include "rbc_qre.h"
#include "rbc_vspace.h"
#include "parameters.h"

typedef struct rolloII_secretKey {
	rbc_vspace F;
	rbc_qre x;
	rbc_qre y;
} rolloII_secretKey;

typedef struct rolloII_publicKey {
	rbc_qre h;
} rolloII_publicKey;

typedef struct rolloII_ciphertext{
	rbc_qre syndrom;
	unsigned char v[SHA512_BYTES];
	unsigned char d[SHA512_BYTES];
} rolloII_ciphertext;

#endif
