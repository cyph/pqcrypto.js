#ifndef params_H
#define params_H

#define p 653
#define q 4621
#define w 252
#define tau0 2175
#define tau1 113
#define tau2 2031
#define tau3 290
#define I 256

#define ppadsort 653

#define q18 57 /* round(2^18/q) */
#define q27 29045 /* round(2^27/q) */
#define q31 464722 /* floor(2^31/q) */

#include "crypto_verify_1025.h"
#define crypto_verify_clen crypto_verify_1025

#include "crypto_decode_653x1541.h"
#define Rounded_bytes crypto_decode_653x1541_STRBYTES
#define Rounded_decode crypto_decode_653x1541

#include "crypto_encode_653x1541round.h"
#define Round_and_encode crypto_encode_653x1541round

#include "crypto_encode_653x3.h"
#include "crypto_decode_653x3.h"
#define Small_bytes crypto_encode_653x3_STRBYTES
#define Small_encode crypto_encode_653x3
#define Small_decode crypto_decode_653x3

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

#include "crypto_decode_653xint32.h"
#define crypto_decode_pxint32 crypto_decode_653xint32

#include "crypto_decode_653xint16.h"
#define crypto_decode_pxint16 crypto_decode_653xint16

#include "crypto_encode_653xint16.h"
#define crypto_encode_pxint16 crypto_encode_653xint16

#include "crypto_core_multsntrup653.h"
#define crypto_core_mult crypto_core_multsntrup653

#endif
