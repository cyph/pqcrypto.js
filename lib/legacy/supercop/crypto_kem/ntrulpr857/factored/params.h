#ifndef params_H
#define params_H

#define p 857
#define q 5167
#define w 281
#define tau0 2433
#define tau1 101
#define tau2 2265
#define tau3 324
#define I 256

#define ppadsort 857

#define q18 51 /* round(2^18/q) */
#define q27 25976 /* round(2^27/q) */
#define q31 415615 /* floor(2^31/q) */

#include "crypto_verify_1312.h"
#define crypto_verify_clen crypto_verify_1312

#include "crypto_decode_857x1723.h"
#define Rounded_bytes crypto_decode_857x1723_STRBYTES
#define Rounded_decode crypto_decode_857x1723

#include "crypto_encode_857x1723round.h"
#define Round_and_encode crypto_encode_857x1723round

#include "crypto_encode_857x3.h"
#include "crypto_decode_857x3.h"
#define Small_bytes crypto_encode_857x3_STRBYTES
#define Small_encode crypto_encode_857x3
#define Small_decode crypto_decode_857x3

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

#include "crypto_decode_857xint32.h"
#define crypto_decode_pxint32 crypto_decode_857xint32

#include "crypto_decode_857xint16.h"
#define crypto_decode_pxint16 crypto_decode_857xint16

#include "crypto_encode_857xint16.h"
#define crypto_encode_pxint16 crypto_encode_857xint16

#include "crypto_core_multsntrup857.h"
#define crypto_core_mult crypto_core_multsntrup857

#endif
