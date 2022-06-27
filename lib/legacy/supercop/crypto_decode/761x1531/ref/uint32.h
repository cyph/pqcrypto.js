#ifndef UINT32_H
#define UINT32_H

#include "crypto_uint32.h"
#define uint32 crypto_uint32

#include "uint16.h"

/*
assuming 1 <= m < 16384:
q = uint32_div_uint14(x,m) means q = x/m
r = uint32_mod_uint14(x,m) means r = x/m
uint32_moddiv_uint14(&q,&r,x,m) means q = x/m, r = x%m
*/

#define uint32_div_uint14 CRYPTO_NAMESPACE(uint32_div_uint14)
#define uint32_mod_uint14 CRYPTO_NAMESPACE(uint32_mod_uint14)
#define uint32_divmod_uint14 CRYPTO_NAMESPACE(uint32_divmod_uint14)

extern uint32 uint32_div_uint14(uint32,uint16);
extern uint16 uint32_mod_uint14(uint32,uint16);
extern void uint32_divmod_uint14(uint32 *,uint16 *,uint32,uint16);

#endif
