#ifndef params_H
#define params_H

#define p 761
#define q 4591
#define w 250
#define tau0 2156
#define tau1 114
#define tau2 2007
#define tau3 287
#define I 256

#define ppadsort 768

#define q18 57 /* round(2^18/q) */
#define q27 29235 /* round(2^27/q) */
#define q31 467759 /* floor(2^31/q) */

#include "crypto_verify_1167.h"
#define crypto_verify_clen crypto_verify_1167

#include "crypto_decode_761x1531.h"
#define Rounded_bytes crypto_decode_761x1531_STRBYTES
#define Rounded_decode crypto_decode_761x1531

#include "crypto_encode_761x1531round.h"
#define Round_and_encode crypto_encode_761x1531round

#include "crypto_encode_761x3.h"
#include "crypto_decode_761x3.h"
#define Small_bytes crypto_encode_761x3_STRBYTES
#define Small_encode crypto_encode_761x3
#define Small_decode crypto_decode_761x3

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

#include "crypto_decode_761xint32.h"
#define crypto_decode_pxint32 crypto_decode_761xint32

#include "crypto_decode_761xint16.h"
#define crypto_decode_pxint16 crypto_decode_761xint16

#include "crypto_encode_761xint16.h"
#define crypto_encode_pxint16 crypto_encode_761xint16

#include "crypto_core_multsntrup761.h"
#define crypto_core_mult crypto_core_multsntrup761

#endif
