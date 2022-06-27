#ifndef params_H
#define params_H

#define p 653
#define ppad 657
#define q 4621
#define qinv -29499 /* reciprocal of q mod 2^16 */

#define q14 4 /* closest integer to 2^14/q */
#define q18 57 /* closest integer to 2^18/q */
#define q27 29045 /* closest integer to 2^27/q */
#define q31 464722 /* floor(2^31/q) */

#include "crypto_encode_653xint16.h"
#define crypto_encode_pxint16 crypto_encode_653xint16

#endif
