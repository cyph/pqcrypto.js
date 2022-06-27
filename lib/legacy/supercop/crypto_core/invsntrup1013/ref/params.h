#ifndef params_H
#define params_H

#define p 1013
#define q 7177

#define q14 2 /* closest integer to 2^14/q */
#define q18 37 /* closest integer to 2^18/q */
#define q27 18701 /* closest integer to 2^27/q */
#define q31 299217 /* floor(2^31/q) */

#include "crypto_encode_1013xint16.h"
#define crypto_encode_pxint16 crypto_encode_1013xint16

#endif
