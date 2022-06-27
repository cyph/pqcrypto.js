#ifndef params_H
#define params_H

#define p 653
#define q 4621
#define w 288

#define ppadsort 653

#include "crypto_verify_897.h"
#define crypto_verify_clen crypto_verify_897

#include "crypto_encode_653x4621.h"
#include "crypto_decode_653x4621.h"
#define Rq_bytes crypto_encode_653x4621_STRBYTES
#define Rq_encode crypto_encode_653x4621
#define Rq_decode crypto_decode_653x4621

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

#include "crypto_encode_653xfreeze3.h"
#define crypto_encode_pxfreeze3 crypto_encode_653xfreeze3

#include "crypto_decode_653xint32.h"
#define crypto_decode_pxint32 crypto_decode_653xint32

#include "crypto_decode_653xint16.h"
#define crypto_decode_pxint16 crypto_decode_653xint16

#include "crypto_encode_653xint16.h"
#define crypto_encode_pxint16 crypto_encode_653xint16

#include "crypto_core_wforcesntrup653.h"
#define crypto_core_wforce crypto_core_wforcesntrup653

#include "crypto_core_scale3sntrup653.h"
#define crypto_core_scale3 crypto_core_scale3sntrup653

#include "crypto_core_invsntrup653.h"
#define crypto_core_inv crypto_core_invsntrup653

#include "crypto_core_inv3sntrup653.h"
#define crypto_core_inv3 crypto_core_inv3sntrup653

#include "crypto_core_mult3sntrup653.h"
#define crypto_core_mult3 crypto_core_mult3sntrup653

#include "crypto_core_multsntrup653.h"
#define crypto_core_mult crypto_core_multsntrup653

#endif
