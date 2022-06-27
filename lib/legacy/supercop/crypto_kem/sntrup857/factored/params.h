#ifndef params_H
#define params_H

#define p 857
#define q 5167
#define w 322

#define ppadsort 857

#include "crypto_verify_1184.h"
#define crypto_verify_clen crypto_verify_1184

#include "crypto_encode_857x5167.h"
#include "crypto_decode_857x5167.h"
#define Rq_bytes crypto_encode_857x5167_STRBYTES
#define Rq_encode crypto_encode_857x5167
#define Rq_decode crypto_decode_857x5167

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

#include "crypto_encode_857xfreeze3.h"
#define crypto_encode_pxfreeze3 crypto_encode_857xfreeze3

#include "crypto_decode_857xint32.h"
#define crypto_decode_pxint32 crypto_decode_857xint32

#include "crypto_decode_857xint16.h"
#define crypto_decode_pxint16 crypto_decode_857xint16

#include "crypto_encode_857xint16.h"
#define crypto_encode_pxint16 crypto_encode_857xint16

#include "crypto_core_wforcesntrup857.h"
#define crypto_core_wforce crypto_core_wforcesntrup857

#include "crypto_core_scale3sntrup857.h"
#define crypto_core_scale3 crypto_core_scale3sntrup857

#include "crypto_core_invsntrup857.h"
#define crypto_core_inv crypto_core_invsntrup857

#include "crypto_core_inv3sntrup857.h"
#define crypto_core_inv3 crypto_core_inv3sntrup857

#include "crypto_core_mult3sntrup857.h"
#define crypto_core_mult3 crypto_core_mult3sntrup857

#include "crypto_core_multsntrup857.h"
#define crypto_core_mult crypto_core_multsntrup857

#endif
