#ifndef params_H
#define params_H

#define p 1013
#define q 7177
#define w 448

#define ppadsort 1024

#include "crypto_verify_1455.h"
#define crypto_verify_clen crypto_verify_1455

#include "crypto_encode_1013x7177.h"
#include "crypto_decode_1013x7177.h"
#define Rq_bytes crypto_encode_1013x7177_STRBYTES
#define Rq_encode crypto_encode_1013x7177
#define Rq_decode crypto_decode_1013x7177

#include "crypto_decode_1013x2393.h"
#define Rounded_bytes crypto_decode_1013x2393_STRBYTES
#define Rounded_decode crypto_decode_1013x2393

#include "crypto_encode_1013x2393round.h"
#define Round_and_encode crypto_encode_1013x2393round

#include "crypto_encode_1013x3.h"
#include "crypto_decode_1013x3.h"
#define Small_bytes crypto_encode_1013x3_STRBYTES
#define Small_encode crypto_encode_1013x3
#define Small_decode crypto_decode_1013x3

#include "crypto_encode_1013xfreeze3.h"
#define crypto_encode_pxfreeze3 crypto_encode_1013xfreeze3

#include "crypto_decode_1013xint32.h"
#define crypto_decode_pxint32 crypto_decode_1013xint32

#include "crypto_decode_1013xint16.h"
#define crypto_decode_pxint16 crypto_decode_1013xint16

#include "crypto_encode_1013xint16.h"
#define crypto_encode_pxint16 crypto_encode_1013xint16

#include "crypto_core_wforcesntrup1013.h"
#define crypto_core_wforce crypto_core_wforcesntrup1013

#include "crypto_core_scale3sntrup1013.h"
#define crypto_core_scale3 crypto_core_scale3sntrup1013

#include "crypto_core_invsntrup1013.h"
#define crypto_core_inv crypto_core_invsntrup1013

#include "crypto_core_inv3sntrup1013.h"
#define crypto_core_inv3 crypto_core_inv3sntrup1013

#include "crypto_core_mult3sntrup1013.h"
#define crypto_core_mult3 crypto_core_mult3sntrup1013

#include "crypto_core_multsntrup1013.h"
#define crypto_core_mult crypto_core_multsntrup1013

#endif
