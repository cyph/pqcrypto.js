#ifndef __RIJNDAEL256_H
#define __RIJNDAEL256_H

#include <stdint.h>
#include "rijndael-alg-ref.h"

extern word8 rk[MAXROUNDS+1][4][MAXBC];

void rijndael256KeySched (const unsigned char *key_State);

void rijndael256Encrypt (unsigned char *state_inout, const uint8_t  domain);

#endif  /* __RIJNDAEL256_H */