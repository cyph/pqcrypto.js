#ifndef _SIGNHFE_HPP
#define _SIGNHFE_HPP

#include "config_HFE.h"
#include <stddef.h>

/* Sign a message: sm8 is the signed message, m is the original message,
 *                 and sk is the secret key. */
int signHFE(unsigned char* sm8, const unsigned char* m, size_t len, \
            const UINT* sk);


#endif
