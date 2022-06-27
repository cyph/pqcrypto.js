#ifndef params_h
#define params_h

#define q 4591
/* XXX: also built into modq in various ways */

#define qshift 2295
#define p 761
#define w 286

#define rq_encode_len 1218
#define small_encode_len 191


#include "crypto_encode_761xint16.h"
#define crypto_encode_pxint16 crypto_encode_761xint16


#endif
