#ifndef params_H
#define params_H

#define p 761
#define ppad 769
#define q 4591
#define qinv 15631 /* reciprocal of q mod 2^16 */

#define q14 4 /* closest integer to 2^14/q */
#define q18 57 /* closest integer to 2^18/q */
#define q27 29235 /* closest integer to 2^27/q */
#define q31 467759 /* floor(2^31/q) */

#include "crypto_encode_761xint16.h"
#define crypto_encode_pxint16 crypto_encode_761xint16

#endif
