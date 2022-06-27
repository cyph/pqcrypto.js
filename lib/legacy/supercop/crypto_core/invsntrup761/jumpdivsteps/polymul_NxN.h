#ifndef _POLYMUL_NXN_H_
#define _POLYMUL_NXN_H_ 



#define gf_polymul_32x32_avx2_uvqr_fg_x  CRYPTO_NAMESPACE(gf_polymul_32x32_avx2_uvqr_fg_x)
#define gf_polymul_32x32_avx2_uvqr_vr_x  CRYPTO_NAMESPACE(gf_polymul_32x32_avx2_uvqr_vr_x)

#define gf_polymul_64x64_avx2_divR  CRYPTO_NAMESPACE(gf_polymul_64x64_avx2_divR)
#define gf_polymul_64x64_avx2  CRYPTO_NAMESPACE(gf_polymul_64x64_avx2)
#define gf_polymul_64x64_avx2_uvqr_fg  CRYPTO_NAMESPACE(gf_polymul_64x64_avx2_uvqr_fg)
#define gf_polymul_64x64_avx2_uvqr_vr  CRYPTO_NAMESPACE(gf_polymul_64x64_avx2_uvqr_vr)
#define gf_polymul_64x64_avx2_uvqr_fg_x  CRYPTO_NAMESPACE(gf_polymul_64x64_avx2_uvqr_fg_x)
#define gf_polymul_64x64_avx2_uvqr_vr_x  CRYPTO_NAMESPACE(gf_polymul_64x64_avx2_uvqr_vr_x)

#define gf_polymul_128x128_avx2_divR  CRYPTO_NAMESPACE(gf_polymul_128x128_avx2_divR)
#define gf_polymul_128x128_avx2  CRYPTO_NAMESPACE(gf_polymul_128x128_avx2)
#define gf_polymul_128x128_avx2_uvqr_fg  CRYPTO_NAMESPACE(gf_polymul_128x128_avx2_uvqr_fg)
#define gf_polymul_128x128_avx2_uvqr_vr  CRYPTO_NAMESPACE(gf_polymul_128x128_avx2_uvqr_vr)
#define gf_polymul_128x128_avx2_uvqr_fg_x  CRYPTO_NAMESPACE(gf_polymul_128x128_avx2_uvqr_fg_x)
#define gf_polymul_128x128_avx2_uvqr_vr_x  CRYPTO_NAMESPACE(gf_polymul_128x128_avx2_uvqr_vr_x)

#define gf_polymul_256x256_avx2_divR  CRYPTO_NAMESPACE(gf_polymul_256x256_avx2_divR)
#define gf_polymul_256x256_avx2  CRYPTO_NAMESPACE(gf_polymul_256x256_avx2)
#define gf_polymul_256x256_avx2_uvqr_fg  CRYPTO_NAMESPACE(gf_polymul_256x256_avx2_uvqr_fg)
#define gf_polymul_256x256_avx2_uvqr_vr  CRYPTO_NAMESPACE(gf_polymul_256x256_avx2_uvqr_vr)
#define gf_polymul_256x256_avx2_uvqr_fg_x  CRYPTO_NAMESPACE(gf_polymul_256x256_avx2_uvqr_fg_x)
#define gf_polymul_256x256_avx2_uvqr_vr_x  CRYPTO_NAMESPACE(gf_polymul_256x256_avx2_uvqr_vr_x)

#define gf_polymul_512x512_avx2_divR  CRYPTO_NAMESPACE(gf_polymul_512x512_avx2_divR)
#define gf_polymul_512x512_avx2  CRYPTO_NAMESPACE(gf_polymul_512x512_avx2)
#define gf_polymul_512x512_avx2_uvqr_fg  CRYPTO_NAMESPACE(gf_polymul_512x512_avx2_uvqr_fg)
#define gf_polymul_512x512_avx2_uvqr_vr  CRYPTO_NAMESPACE(gf_polymul_512x512_avx2_uvqr_vr)
#define gf_polymul_512x512_avx2_uvqr_fg_x  CRYPTO_NAMESPACE(gf_polymul_512x512_avx2_uvqr_fg_x)
#define gf_polymul_512x512_avx2_uvqr_vr_x  CRYPTO_NAMESPACE(gf_polymul_512x512_avx2_uvqr_vr_x)


#include "polymul_8x8.h"



#ifdef  __cplusplus
extern  "C" {
#endif

extern void gf_polymul_32x32_avx2_uvqr_fg_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_32x32_avx2_uvqr_vr_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);

extern void gf_polymul_64x64_avx2_divR (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_64x64_avx2 (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_64x64_avx2_uvqr_fg (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_64x64_avx2_uvqr_vr (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_64x64_avx2_uvqr_fg_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_64x64_avx2_uvqr_vr_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);

extern void gf_polymul_128x128_avx2_divR (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_128x128_avx2 (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_128x128_avx2_uvqr_fg (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_128x128_avx2_uvqr_vr (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_128x128_avx2_uvqr_fg_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_128x128_avx2_uvqr_vr_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);

extern void gf_polymul_256x256_avx2_divR (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_256x256_avx2 (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_256x256_avx2_uvqr_fg (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_256x256_avx2_uvqr_vr (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_256x256_avx2_uvqr_fg_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_256x256_avx2_uvqr_vr_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);

extern void gf_polymul_512x512_avx2_divR (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_512x512_avx2 (__m256i *h, __m256i *f, __m256i *g);
extern void gf_polymul_512x512_avx2_uvqr_fg (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_512x512_avx2_uvqr_vr (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_512x512_avx2_uvqr_fg_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);
extern void gf_polymul_512x512_avx2_uvqr_vr_x (__m256i *ff, __m256i *gg, __m256i *uvqr, __m256i *f, __m256i *g);

#ifdef  __cplusplus
}
#endif


#endif
