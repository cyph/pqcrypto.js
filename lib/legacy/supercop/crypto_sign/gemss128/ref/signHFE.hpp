#ifndef _SIGNHFE_HPP
#define _SIGNHFE_HPP

#include "config_HFE.h"
#include <stddef.h>

/* Sign a message: sm is the signed message, m is the original message,
 *                 and sk is the secret key. */
int signHFE(UINT* sm, const unsigned char* m, size_t len, const UINT* sk);


#endif
