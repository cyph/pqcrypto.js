#include "crypto_sort.h"
#include "crypto_int32.h"
#define int32 crypto_int32

#include "int32_minmax_thumb.inc"

/* k is a multiple of 4p */
/* p is a multiple of 2q */
/* p is a power of 2 */
static inline void twolevels_down(int32 *x,int32 k,int32 p,int32 q)
{
  int32 i;
  x += k;
  while (p > 0) {
    for (i = 0;i < q;++i) {
      int32 x0 = x[0];
      int32 x1 = x[q];
      int32 x2 = x[2*q];
      int32 x3 = x[3*q];
      int32_MINMAX(x2,x0);
      int32_MINMAX(x3,x1);
      int32_MINMAX(x1,x0);
      int32_MINMAX(x3,x2);
      x[0] = x0;
      x[q] = x1;
      x[2*q] = x2;
      x[3*q] = x3;
      ++x;
    }
    x += 3*q;
    p -= 2*q;
  }
}

static inline void twolevels_up(int32 *x,int32 k,int32 p,int32 q)
{
  int32 i;
  x += k;
  while (p > 0) {
    for (i = 0;i < q;++i) {
      int32 x0 = x[0];
      int32 x1 = x[q];
      int32 x2 = x[2*q];
      int32 x3 = x[3*q];
      int32_MINMAX(x0,x2);
      int32_MINMAX(x1,x3);
      int32_MINMAX(x0,x1);
      int32_MINMAX(x2,x3);
      x[0] = x0;
      x[q] = x1;
      x[2*q] = x2;
      x[3*q] = x3;
      ++x;
    }
    x += 3*q;
    p -= 2*q;
  }
}

/* k is a multiple of 4p */
/* p is a multiple of 4q */
/* p is a power of 2 */
static inline void threelevels_down(int32 *x,int32 k,int32 p,int32 q)
{
  int32 i;
  x += k;
  while (p > 0) {
    for (i = 0;i < q;++i) {
      int32 x0 = x[0];
      int32 x1 = x[q];
      int32 x2 = x[2*q];
      int32 x3 = x[3*q];
      int32 x4 = x[4*q];
      int32 x5 = x[5*q];
      int32 x6 = x[6*q];
      int32 x7 = x[7*q];
      int32_MINMAX(x4,x0);
      int32_MINMAX(x5,x1);
      int32_MINMAX(x6,x2);
      int32_MINMAX(x7,x3);
      int32_MINMAX(x2,x0);
      int32_MINMAX(x3,x1);
      int32_MINMAX(x6,x4);
      int32_MINMAX(x7,x5);
      int32_MINMAX(x1,x0);
      int32_MINMAX(x3,x2);
      int32_MINMAX(x5,x4);
      int32_MINMAX(x7,x6);
      x[0] = x0;
      x[q] = x1;
      x[2*q] = x2;
      x[3*q] = x3;
      x[4*q] = x4;
      x[5*q] = x5;
      x[6*q] = x6;
      x[7*q] = x7;
      ++x;
    }
    x += 7*q;
    p -= 4*q;
  }
}

static inline void threelevels_up(int32 *x,int32 k,int32 p,int32 q)
{
  int32 i;
  x += k;
  while (p > 0) {
    for (i = 0;i < q;++i) {
      int32 x0 = x[0];
      int32 x1 = x[q];
      int32 x2 = x[2*q];
      int32 x3 = x[3*q];
      int32 x4 = x[4*q];
      int32 x5 = x[5*q];
      int32 x6 = x[6*q];
      int32 x7 = x[7*q];
      int32_MINMAX(x0,x4);
      int32_MINMAX(x1,x5);
      int32_MINMAX(x2,x6);
      int32_MINMAX(x3,x7);
      int32_MINMAX(x0,x2);
      int32_MINMAX(x1,x3);
      int32_MINMAX(x4,x6);
      int32_MINMAX(x5,x7);
      int32_MINMAX(x0,x1);
      int32_MINMAX(x2,x3);
      int32_MINMAX(x4,x5);
      int32_MINMAX(x6,x7);
      x[0] = x0;
      x[q] = x1;
      x[2*q] = x2;
      x[3*q] = x3;
      x[4*q] = x4;
      x[5*q] = x5;
      x[6*q] = x6;
      x[7*q] = x7;
      ++x;
    }
    x += 7*q;
    p -= 4*q;
  }
}

static inline void crypto_sort_smallindices(int32 *x,int32 n)
{ int32 t,p,q,i,j,k;
  int32 x0,x1,x2,x3,x4,x5,x6,x7;

  if (n < 2) return;

  if (n == 2) {
    int32_MINMAX(x[0],x[1]);
    return;
  }

  t = 2;
  while (t < n-t) t += t;

  /* n > t >= 2 */

  for (k = 0;k+8 <= n;k += 8) {
    x0 = x[k];
    x1 = x[k+1];
    x2 = x[k+2];
    x3 = x[k+3];
#ifdef TRUEBITONIC
    int32_MINMAX(x1,x0);
    int32_MINMAX(x2,x3);
    int32_MINMAX(x2,x0);
    int32_MINMAX(x3,x1);
    int32_MINMAX(x1,x0);
    int32_MINMAX(x3,x2);
#else /* odd-even */
    int32_MINMAX(x1,x0);
    int32_MINMAX(x3,x2);
    int32_MINMAX(x2,x0);
    int32_MINMAX(x3,x1);
    int32_MINMAX(x2,x1);
#endif
    x[k] = x0;
    x[k+1] = x1;
    x[k+2] = x2;
    x[k+3] = x3;

    x4 = x[k+4];
    x5 = x[k+5];
    x6 = x[k+6];
    x7 = x[k+7];
#ifdef TRUEBITONIC
    int32_MINMAX(x5,x4);
    int32_MINMAX(x6,x7);
    int32_MINMAX(x4,x6);
    int32_MINMAX(x5,x7);
    int32_MINMAX(x4,x5);
    int32_MINMAX(x6,x7);
#else /* odd-even */
    int32_MINMAX(x4,x5);
    int32_MINMAX(x6,x7);
    int32_MINMAX(x4,x6);
    int32_MINMAX(x5,x7);
    int32_MINMAX(x5,x6);
#endif
    x[k+4] = x4;
    x[k+5] = x5;
    x[k+6] = x6;
    x[k+7] = x7;
  }

  if (k+4 <= n) {
    int32_MINMAX(x[k+1],x[k]);
    int32_MINMAX(x[k+2],x[k+3]);
    if (k+6 < n)
      int32_MINMAX(x[k+5],x[k+4]);
    else if (k+6 <= n)
      int32_MINMAX(x[k+4],x[k+5]);
  } else {
    if (k+2 < n)
      int32_MINMAX(x[k+1],x[k]);
    else if (k+2 <= n)
      int32_MINMAX(x[k],x[k+1]);
  }

  if (k+4 < n) {
    x0 = x[k];
    x1 = x[k+1];
    x2 = x[k+2];
    x3 = x[k+3];
    int32_MINMAX(x2,x0);
    int32_MINMAX(x3,x1);
    int32_MINMAX(x1,x0);
    int32_MINMAX(x3,x2);
    x[k] = x0;
    x[k+1] = x1;
    x[k+2] = x2;
    x[k+3] = x3;

    k += 4;
  }

  for (q = 2;q >= 1;q >>= 1) {
    for (j = k;j+q+q <= n;j += q+q)
      for (i = j;i < j+q;++i)
        int32_MINMAX(x[i],x[i+q]);
    for (i = j;i < n-q;++i)
      int32_MINMAX(x[i],x[i+q]);
  }

  if (t == 2) return;

  p = 4;

  for (;;) {
    /* n > t >= p */
    k = 0;
    for (;;) {
      /* k is a multiple of 4p */
      if (k+p+p >= n) break;

      q = p;
      while (q >= 1024) { q >>= 2; threelevels_down(x,k,p,q); q >>= 1; }
      switch(q) {
        case 256: threelevels_down(x,k,p,64);
        case  32: threelevels_down(x,k,p,8);
        case   4: threelevels_down(x,k,p,1);
                  break;
        case 128: threelevels_down(x,k,p,32);
        case  16: threelevels_down(x,k,p,4);
                  twolevels_down(x,k,p,1);
                  break;
        case 512: threelevels_down(x,k,p,128);
        case  64: threelevels_down(x,k,p,16);
        case   8: twolevels_down(x,k,p,4);
        case   2: twolevels_down(x,k,p,1);
      }

      k += p+p;

      if (k+p+p > n) break;

      q = p;
      while (q >= 1024) { q >>= 2; threelevels_up(x,k,p,q); q >>= 1; }
      switch(q) {
        case 256: threelevels_up(x,k,p,64);
        case  32: threelevels_up(x,k,p,8);
        case   4: threelevels_up(x,k,p,1);
                  break;
        case 128: threelevels_up(x,k,p,32);
        case  16: threelevels_up(x,k,p,4);
                  twolevels_up(x,k,p,1);
                  break;
        case 512: threelevels_up(x,k,p,128);
        case  64: threelevels_up(x,k,p,16);
        case   8: twolevels_up(x,k,p,4);
        case   2: twolevels_up(x,k,p,1);
      }

      k += p+p;
    }

    for (q = p;q >= 1;q >>= 1) {
      for (j = k;j+q+q <= n;j += q+q)
        for (i = j;i < j+q;++i)
          int32_MINMAX(x[i],x[i+q]);
      for (i = j;i < n-q;++i)
        int32_MINMAX(x[i],x[i+q]);
    }

    if (p == t) break;
    p += p;
  }
}

void crypto_sort(void *array,long long n)
{
  long long top,p,q,r,i,j;
  int32 *x = array;

  if (n < 2) return;
  if (n < 0x40000000) {
    crypto_sort_smallindices(x,n);
    return;
  }
  top = 1;
  while (top < n - top) top += top;

  for (p = top;p >= 1;p >>= 1) {
    i = 0;
    while (i + 2 * p <= n) {
      for (j = i;j < i + p;++j)
        int32_MINMAX(x[j],x[j+p]);
      i += 2 * p;
    }
    for (j = i;j < n - p;++j)
      int32_MINMAX(x[j],x[j+p]);

    i = 0;
    j = 0;
    for (q = top;q > p;q >>= 1) {
      if (j != i) for (;;) {
        if (j == n - q) goto done;
        int32 a = x[j + p];
        for (r = q;r > p;r >>= 1)
          int32_MINMAX(a,x[j + r]);
        x[j + p] = a;
        ++j;
        if (j == i + p) {
          i += 2 * p;
          break;
        }
      }
      while (i + p <= n - q) {
        for (j = i;j < i + p;++j) {
          int32 a = x[j + p];
          for (r = q;r > p;r >>= 1)
            int32_MINMAX(a,x[j+r]);
          x[j + p] = a;
        }
        i += 2 * p;
      }
      /* now i + p > n - q */
      j = i;
      while (j < n - q) {
        int32 a = x[j + p];
        for (r = q;r > p;r >>= 1)
          int32_MINMAX(a,x[j+r]);
        x[j + p] = a;
        ++j;
      }

      done: ;
    }
  }
}
