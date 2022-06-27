#ifndef params_H
#define params_H

#define p 1277
#define q 7879
#define w 429
#define tau0 3724
#define tau1 66
#define tau2 3469
#define tau3 496
#define I 256

#define ppadsort 1280

#define q18 33 /* round(2^18/q) */
#define q27 17035 /* round(2^27/q) */
#define q31 272557 /* floor(2^31/q) */

#include "crypto_verify_1975.h"
#define crypto_verify_clen crypto_verify_1975

#include "crypto_decode_1277x2627.h"
#define Rounded_bytes crypto_decode_1277x2627_STRBYTES
#define Rounded_decode crypto_decode_1277x2627

#include "crypto_encode_1277x2627round.h"
#define Round_and_encode crypto_encode_1277x2627round

#include "crypto_encode_1277x3.h"
#include "crypto_decode_1277x3.h"
#define Small_bytes crypto_encode_1277x3_STRBYTES
#define Small_encode crypto_encode_1277x3
#define Small_decode crypto_decode_1277x3

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

#include "crypto_decode_1277xint32.h"
#define crypto_decode_pxint32 crypto_decode_1277xint32

#include "crypto_decode_1277xint16.h"
#define crypto_decode_pxint16 crypto_decode_1277xint16

#include "crypto_encode_1277xint16.h"
#define crypto_encode_pxint16 crypto_encode_1277xint16

#include "crypto_core_multsntrup1277.h"
#define crypto_core_mult crypto_core_multsntrup1277

#endif
