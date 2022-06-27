#include "crypto_core.h"
#include "params.h"

#include "crypto_int8.h"
#include "crypto_int16.h"
#include "crypto_int32.h"
#include "crypto_int64.h"
#include "crypto_uint64.h"
#define int8 crypto_int8
#define int16 crypto_int16
#define int32 crypto_int32
#define int64 crypto_int64
#define uint64 crypto_uint64
typedef int8 small;

/* works for -16384 <= x < 16384 */
static small F3_freeze(int16 x)
{
  return x-3*((10923*x+16384)>>15);
}

int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{
  small *h = (void *) outbytes;
  small f[ppad];
  small g[ppad];
  small fg[ppad+ppad];
  int32 fpack[ppad4];
  int32 gpack[ppad4];
  uint64 result,top;
  int i,j;

  for (i = 0;i < p;++i) {
    small fi = inbytes[i];
    small fi0 = fi&1;
    f[i] = fi0-(fi&(fi0<<1));
  }
  for (i = p;i < ppad;++i) f[i] = 0;
  for (i = 0;i < p;++i) {
    small gi = kbytes[i];
    small gi0 = gi&1;
    g[i] = gi0-(gi&(gi0<<1));
  }
  for (i = p;i < ppad;++i) g[i] = 0;

  for (i = 0;i < ppad4;++i) {
    int32 f0 = f[4*i+0];
    int32 f1 = f[4*i+1];
    int32 f2 = f[4*i+2];
    int32 f3 = f[4*i+3];
    fpack[i] = f0+(f1<<8)+(f2<<16)+(f3<<24);
  }
  for (i = 0;i < ppad4;++i) {
    int32 g0 = g[4*i+0];
    int32 g1 = g[4*i+1];
    int32 g2 = g[4*i+2];
    int32 g3 = g[4*i+3];
    gpack[i] = g0+(g1<<8)+(g2<<16)+(g3<<24);
  }

  result = 0;
  for (i = 0;i < ppad4;++i) {
    for (j = 0;j <= i;++j) {
      result += fpack[j]*(crypto_int64) gpack[i-j];
      result += 0x6f6f6f6f6f6f6f6f;
      top = result & 0xf0f0f0f0f0f0f0f0;
      result &= 0x0f0f0f0f0f0f0f0f;
      result += top>>4;
    }
    fg[i*4+0] = result&0xff; result >>= 8;
    fg[i*4+1] = result&0xff; result >>= 8;
    fg[i*4+2] = result&0xff; result >>= 8;
    fg[i*4+3] = result&0xff; result >>= 8;
  }
  for (i = ppad4;i < 2*ppad4;++i) {
    for (j = i-ppad4+1;j < ppad4;++j) {
      result += fpack[j]*(crypto_int64) gpack[i-j];
      result += 0x6f6f6f6f6f6f6f6f;
      top = result & 0xf0f0f0f0f0f0f0f0;
      result &= 0x0f0f0f0f0f0f0f0f;
      result += top>>4;
    }
    fg[i*4+0] = result&0xff; result >>= 8;
    fg[i*4+1] = result&0xff; result >>= 8;
    fg[i*4+2] = result&0xff; result >>= 8;
    fg[i*4+3] = result&0xff; result >>= 8;
  }

  for (i = p+p-2;i >= p;--i) {
    fg[i-p] = F3_freeze(fg[i-p]+fg[i]);
    fg[i-p+1] = F3_freeze(fg[i-p+1]+fg[i]);
  }

  for (i = 0;i < p;++i) h[i] = fg[i];
  return 0;
}
