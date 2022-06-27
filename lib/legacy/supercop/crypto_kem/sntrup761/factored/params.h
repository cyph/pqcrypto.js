#ifndef params_H
#define params_H

#define p 761
#define q 4591
#define w 286

#define ppadsort 768

#include "crypto_verify_1039.h"
#define crypto_verify_clen crypto_verify_1039

#include "crypto_encode_761x4591.h"
#include "crypto_decode_761x4591.h"
#define Rq_bytes crypto_encode_761x4591_STRBYTES
#define Rq_encode crypto_encode_761x4591
#define Rq_decode crypto_decode_761x4591

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

#include "crypto_encode_761xfreeze3.h"
#define crypto_encode_pxfreeze3 crypto_encode_761xfreeze3

#include "crypto_decode_761xint32.h"
#define crypto_decode_pxint32 crypto_decode_761xint32

#include "crypto_decode_761xint16.h"
#define crypto_decode_pxint16 crypto_decode_761xint16

#include "crypto_encode_761xint16.h"
#define crypto_encode_pxint16 crypto_encode_761xint16

#include "crypto_core_wforcesntrup761.h"
#define crypto_core_wforce crypto_core_wforcesntrup761

#include "crypto_core_scale3sntrup761.h"
#define crypto_core_scale3 crypto_core_scale3sntrup761

#include "crypto_core_invsntrup761.h"
#define crypto_core_inv crypto_core_invsntrup761

#include "crypto_core_inv3sntrup761.h"
#define crypto_core_inv3 crypto_core_inv3sntrup761

#include "crypto_core_mult3sntrup761.h"
#define crypto_core_mult3 crypto_core_mult3sntrup761

#include "crypto_core_multsntrup761.h"
#define crypto_core_mult crypto_core_multsntrup761

#endif
