#ifndef params_H
#define params_H

#define p 953
#define q 6343
#define w 396

#define ppadsort 960

#include "crypto_verify_1349.h"
#define crypto_verify_clen crypto_verify_1349

#include "crypto_encode_953x6343.h"
#include "crypto_decode_953x6343.h"
#define Rq_bytes crypto_encode_953x6343_STRBYTES
#define Rq_encode crypto_encode_953x6343
#define Rq_decode crypto_decode_953x6343

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

#include "crypto_encode_953xfreeze3.h"
#define crypto_encode_pxfreeze3 crypto_encode_953xfreeze3

#include "crypto_decode_953xint32.h"
#define crypto_decode_pxint32 crypto_decode_953xint32

#include "crypto_decode_953xint16.h"
#define crypto_decode_pxint16 crypto_decode_953xint16

#include "crypto_encode_953xint16.h"
#define crypto_encode_pxint16 crypto_encode_953xint16

#include "crypto_core_wforcesntrup953.h"
#define crypto_core_wforce crypto_core_wforcesntrup953

#include "crypto_core_scale3sntrup953.h"
#define crypto_core_scale3 crypto_core_scale3sntrup953

#include "crypto_core_invsntrup953.h"
#define crypto_core_inv crypto_core_invsntrup953

#include "crypto_core_inv3sntrup953.h"
#define crypto_core_inv3 crypto_core_inv3sntrup953

#include "crypto_core_mult3sntrup953.h"
#define crypto_core_mult3 crypto_core_mult3sntrup953

#include "crypto_core_multsntrup953.h"
#define crypto_core_mult crypto_core_multsntrup953

#endif
