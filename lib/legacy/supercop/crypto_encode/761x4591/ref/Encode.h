#ifndef Encode_H
#define Encode_H

#define Encode crypto_encode_761x4591_ref_Encode

#include "crypto_uint16.h"

/* Encode(s,R,M,len) */
/* assumes 0 <= R[i] < M[i] < 16384 */
extern void Encode(unsigned char *,const crypto_uint16 *,const crypto_uint16 *,long long);

#endif
