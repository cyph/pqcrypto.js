#ifndef params_H
#define params_H

#define p 1277
#define ppad 1281
#define q 7879
#define qinv 17143 /* reciprocal of q mod 2^16 */

#define q14 2 /* closest integer to 2^14/q */
#define q18 33 /* closest integer to 2^18/q */
#define q27 17035 /* closest integer to 2^27/q */
#define q31 272557 /* floor(2^31/q) */

#include "crypto_encode_1277xint16.h"
#define crypto_encode_pxint16 crypto_encode_1277xint16

#endif
