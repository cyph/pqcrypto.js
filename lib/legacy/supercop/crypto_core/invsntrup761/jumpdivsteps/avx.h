#ifndef avx_H
#define avx_H

#include <inttypes.h>
#include <immintrin.h>

typedef int16_t int16;
typedef int32_t int32;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef __m128i int16x8;
typedef __m256i int16x16;



#define mult1_over65536_x16 CRYPTO_NAMESPACE(mult1_over65536_x16)
#define mult2_over65536_x16  CRYPTO_NAMESPACE(mult2_over65536_x16)
#define mult4_over65536_x16  CRYPTO_NAMESPACE(mult4_over65536_x16)
#define mult8_nega_over65536_x16  CRYPTO_NAMESPACE(mult8_nega_over65536_x16)
#define mult64_nega_over4096_x16  CRYPTO_NAMESPACE(mult64_nega_over4096_x16)
#define mult768_over64  CRYPTO_NAMESPACE(mult768_over64)
#define mult768_over64_2 CRYPTO_NAMESPACE(mult768_over64_2)

#define transpose    CRYPTO_NAMESPACE(transpose)
#define untranspose  CRYPTO_NAMESPACE(untranspose)

#define fft64              CRYPTO_NAMESPACE(fft64)
#define unfft64_scale16    CRYPTO_NAMESPACE(unfft64_scale16)
#define fft8_64            CRYPTO_NAMESPACE(fft8_64)
#define unfft8_64_scale8   CRYPTO_NAMESPACE(unfft8_64_scale8)
#define fft48_64           CRYPTO_NAMESPACE(fft48_64)
#define unfft48_64_scale64 CRYPTO_NAMESPACE(unfft48_64_scale64)




#ifdef  __cplusplus
extern  "C" {
#endif


extern void mult1_over65536_x16(int16x16 *,const int16x16 *,const int16x16 *);
extern void mult2_over65536_x16(int16x16 *,const int16x16 *,const int16x16 *);
extern void mult4_over65536_x16(int16x16 *,const int16x16 *,const int16x16 *);
extern void mult8_nega_over65536_x16(int16x16 *,const int16x16 *,const int16x16 *);
extern void mult64_nega_over4096_x16(int16x16 *,const int16x16 *,const int16x16 *);
extern void mult768_over64(int16 *,const int16 *,const int16 *);

extern void transpose(int16x16 *,const int16 (*)[64]);
extern void untranspose(int16 (*)[64],const int16x16 *);

extern void fft64(int16x16 (*)[8],const int16x16 *);
extern void unfft64_scale16(int16x16 *,int16x16 (*)[8]);
extern void fft8_64(int16 (*)[64]);
extern void unfft8_64_scale8(int16 (*)[64]);
extern void fft48_64(int16 (*)[64],const int16 *);
extern void unfft48_64_scale64(int16 (*)[64]);


extern void mult768_over64_2(int16 *,const int16 *,const int16 *,const int16 *,const int16 *);

#ifdef  __cplusplus
}
#endif


#endif
