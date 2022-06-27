#ifndef _SIGN_OPENHFE_H
#define _SIGN_OPENHFE_H

#include "prefix_name.h"
#include "arch.h"
#include <stddef.h>

/* Verify a signature: m is the original message, sm8 is the signed message, 
 * pk is the public key. */
int PREFIX_NAME(sign_openHFE)(const unsigned char* m, size_t len, \
                              const unsigned char* sm8, const UINT* pk);
#define sign_openHFE PREFIX_NAME(sign_openHFE)


#endif
