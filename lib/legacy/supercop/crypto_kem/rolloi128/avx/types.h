/** 
 * \file rolloI_types.h
 * \brief File defining publicKey, secretKey and cipherText types for ROLLO-I
 */

#ifndef ROLLOI_TYPES_H
#define ROLLOI_TYPES_H

#include "rbc_qre.h"
#include "rbc_vspace.h"

typedef struct rolloI_secretKey {
	rbc_vspace F;
	rbc_qre x;
	rbc_qre y;
} rolloI_secretKey;

typedef struct rolloI_publicKey {
	rbc_qre h;
} rolloI_publicKey;

typedef struct rolloI_ciphertext{
	rbc_qre syndrom;
} rolloI_ciphertext;

#endif
