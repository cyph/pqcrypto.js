#ifndef params_H
#define params_H

#define p 857
#define ppad 865
#define q 5167
#define qinv -19761 /* reciprocal of q mod 2^16 */

#define q14 3 /* closest integer to 2^14/q */
#define q18 51 /* closest integer to 2^18/q */
#define q27 25976 /* closest integer to 2^27/q */
#define q31 415615 /* floor(2^31/q) */

#include "crypto_encode_857xint16.h"
#define crypto_encode_pxint16 crypto_encode_857xint16

#endif
