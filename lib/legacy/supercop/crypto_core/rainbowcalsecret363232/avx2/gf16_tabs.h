/// @file gf16_tabs.h
/// @brief Defining the constant tables for performing GF arithmetics.
///

#ifndef _GF16_TABS_H_
#define _GF16_TABS_H_

#include <stdint.h>


#define __mask_0x55 CRYPTO_NAMESPACE(__mask_0x55)
#define __mask_low CRYPTO_NAMESPACE(__mask_low)
#define __mask_16 CRYPTO_NAMESPACE(__mask_16)
#define __gf16_inv CRYPTO_NAMESPACE(__gf16_inv)
#define __gf16_squ CRYPTO_NAMESPACE(__gf16_squ)
#define __gf16_squ_x8 CRYPTO_NAMESPACE(__gf16_squ_x8)
#define __gf16_squ_sl4 CRYPTO_NAMESPACE(__gf16_squ_sl4)
#define __gf16_exp CRYPTO_NAMESPACE(__gf16_exp)
#define __gf16_log CRYPTO_NAMESPACE(__gf16_log)
#define __gf16_mul CRYPTO_NAMESPACE(__gf16_mul)
#define __gf256_mul CRYPTO_NAMESPACE(__gf256_mul)
#define __gf16_mulx2 CRYPTO_NAMESPACE(__gf16_mulx2)


#ifdef  __cplusplus
extern  "C" {
#endif

extern const unsigned char __mask_0x55[];
extern const unsigned char __mask_low[];
extern const unsigned char __mask_16[];
extern const unsigned char __gf16_inv[];
extern const unsigned char __gf16_squ[];
extern const unsigned char __gf16_squ_x8[];
extern const unsigned char __gf16_squ_sl4[];
extern const unsigned char __gf16_exp[];
extern const char __gf16_log[];
extern const unsigned char * __gf16_mul;
extern const unsigned char __gf256_mul[];
extern const unsigned char __gf16_mulx2[];


#ifdef  __cplusplus
}
#endif



#endif // _GF16_TABS_H_
