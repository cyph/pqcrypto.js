#ifndef params_H
#define params_H

#define p 953
#define q 6343
#define w 345
#define tau0 2997
#define tau1 82
#define tau2 2798
#define tau3 400
#define I 256

#define ppadsort 960

#define q18 41 /* round(2^18/q) */
#define q27 21160 /* round(2^27/q) */
#define q31 338559 /* floor(2^31/q) */

#include "crypto_verify_1477.h"
#define crypto_verify_clen crypto_verify_1477

#include "crypto_decode_953x2115.h"
#define Rounded_bytes crypto_decode_953x2115_STRBYTES
#define Rounded_decode crypto_decode_953x2115

#include "crypto_encode_953x2115round.h"
#define Round_and_encode crypto_encode_953x2115round

#include "crypto_encode_953x3.h"
#include "crypto_decode_953x3.h"
#define Small_bytes crypto_encode_953x3_STRBYTES
#define Small_encode crypto_encode_953x3
#define Small_decode crypto_decode_953x3

#include "crypto_encode_256x16.h"
#include "crypto_decode_256x16.h"
#define Top_bytes crypto_encode_256x16_STRBYTES
#define Top_encode crypto_encode_256x16
#define Top_decode crypto_decode_256x16

#include "crypto_encode_256x2.h"
#include "crypto_decode_256x2.h"
#define Inputs_bytes crypto_encode_256x2_STRBYTES
#define Inputs_encode crypto_encode_256x2
#define Inputs_decode crypto_decode_256x2

#include "crypto_decode_953xint32.h"
#define crypto_decode_pxint32 crypto_decode_953xint32

#include "crypto_decode_953xint16.h"
#define crypto_decode_pxint16 crypto_decode_953xint16

#include "crypto_encode_953xint16.h"
#define crypto_encode_pxint16 crypto_encode_953xint16

#include "crypto_core_multsntrup953.h"
#define crypto_core_mult crypto_core_multsntrup953

#endif
