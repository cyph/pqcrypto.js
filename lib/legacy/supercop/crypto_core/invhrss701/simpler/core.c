#include <stdint.h>
#include "crypto_core.h"

#define C 701 /* C coeffs in polys */
#define PADDED 704 /* C coeffs stored in PADDED */

static inline uint8_t mod3(uint8_t a) /* a between 0 and 9 */
{
  int16_t t, c;
  a = (a >> 2) + (a & 3); /* between 0 and 4 */
  t = a - 3;
  c = t >> 5;
  return t^(c&(a^t));
}

/* return -1 if x<0 and y<0; otherwise return 0 */
static inline int both_negative_mask(int x,int y)
{
  return (x & y) >> 15;
}

int crypto_core(unsigned char *out,const unsigned char *in,const unsigned char *kunused,const unsigned char *cunused)
{
  uint8_t f[C];
  uint8_t g[C];
  uint8_t v[C];
  uint8_t r[C];
  int i,loop,delta;
  int sign,swap,t;

  for (i = 0;i < C;++i) v[i] = 0;
  for (i = 0;i < C;++i) r[i] = 0;
  r[0] = 1;

  for (i = 0;i < C;++i) f[i] = 1;
  for (i = 0;i < C-1;++i) g[C-2-i] = mod3((in[2*i] & 3) + 2*(in[2*(C-1)] & 3));
  g[C-1] = 0;

  delta = 1;

  for (loop = 0;loop < 2*(C-1)-1;++loop) {
    for (i = C-1;i > 0;--i) v[i] = v[i-1];
    v[0] = 0;

    sign = mod3(2 * g[0] * f[0]);
    swap = both_negative_mask(-delta,-(int) g[0]);
    delta ^= swap & (delta ^ -delta);
    delta += 1;

    for (i = 0;i < C;++i) {
      t = swap&(f[i]^g[i]); f[i] ^= t; g[i] ^= t;
      t = swap&(v[i]^r[i]); v[i] ^= t; r[i] ^= t;
    }

    for (i = 0;i < C;++i) g[i] = mod3(g[i]+sign*f[i]);
    for (i = 0;i < C;++i) r[i] = mod3(r[i]+sign*v[i]);
    for (i = 0;i < C-1;++i) g[i] = g[i+1];
    g[C-1] = 0;
  }

  sign = f[0];
  for (i = 0;i < 2*PADDED;++i) out[i] = 0;
  for (i = 0;i < C-1;++i) out[2*i] = mod3(sign*v[C-2-i]);

  return 0;
}
