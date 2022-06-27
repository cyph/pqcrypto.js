#ifndef MPFQ_H_
#define MPFQ_H_

/* This header contains common declarations used by mpfq modules */

#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif



/*** Constants for field_specify ***/

#define MPFQ_PRIME 1
#define MPFQ_POLYNOMIAL 2
#define MPFQ_DEGREE 3
#define MPFQ_IO_TYPE 4 /* for setopt */

/***  Some useful macros ***/

#define MALLOC_FAILED()                                                 \
        do {                                                            \
                fprintf(stderr, "malloc failed in %s\n", __func__);     \
                abort();                                                \
        } while (0)

#define BUILD_BITMASK(x) ((x) == GMP_LIMB_BITS ? ((mp_limb_t) - 1) : (~ - ((mp_limb_t) 1 << (x))))

#define GNUC_VERSION(X,Y,Z)     \
    (defined(__GNUC__) &&        \
    (__GNUC__ == X && __GNUC_MINOR__ == Y && __GNUC_PATCHLEVEL__ == Z))

/* typedef unsigned long ulong; */

#ifdef  __GNUC__

#define clzl(x)         __builtin_clzl(x)
#define ctzl(x)         __builtin_ctzl(x)
#define parityl(x)      __builtin_parityl(x)
#ifndef	MAYBE_UNUSED
#define	MAYBE_UNUSED	__attribute__((unused))
#endif

#ifndef MAX
#define MAX(h,i) ((h) > (i) ? (h) : (i))
#endif

#ifndef MIN
#define MIN(l,o) ((l) < (o) ? (l) : (o))
#endif

#ifndef ABS
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#endif

#ifndef EXPECT
#define EXPECT(x,val)   __builtin_expect(x,val)
#endif
#ifndef	UNLIKELY
#define	UNLIKELY(x)	EXPECT(x,0)
#endif
#ifndef	LIKELY
#define	LIKELY(x)	EXPECT(x, 1)
#endif

#else

#ifndef	MAYBE_UNUSED
#define	MAYBE_UNUSED	/**/
#endif


/* provide slow fallbacks */
static inline int clzl(unsigned long x)
{
        static const int t[4] = { 2, 1, 0, 0 };
        int a = 0;
        int res;
#if (GMP_LIMB_BITS == 64)
        if (x >> 32) { a += 32; x >>= 32; }
#endif  
        if (x >> 16) { a += 16; x >>= 16; }
        if (x >>  8) { a +=  8; x >>=  8; }
        if (x >>  4) { a +=  4; x >>=  4; }
        if (x >>  2) { a +=  2; x >>=  2; }
        res = GMP_LIMB_BITS - 2 - a + t[x];
        return res;
}
static inline int ctzl(unsigned long x)
{
	return GMP_LIMB_BITS - clzl(x & - x);
}
static inline int parityl(unsigned long x)
{
	static const int t[4] = { 0, 1, 1, 0, };
#if (GMP_LIMB_BITS == 64)
	x ^= (x >> 32);
#endif
	x ^= (x >> 16);
	x ^= (x >>  8);
	x ^= (x >>  4);
	x ^= (x >>  2);
	return t[x & 3UL];
}

#ifndef __cplusplus
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#endif	/* __cplusplus */

#ifndef EXPECT
#define EXPECT(x,val)   (x)
#endif
#ifndef	UNLIKELY
#define	UNLIKELY(x)	(x)
#endif
#ifndef	LIKELY
#define	LIKELY(x)	(x)
#endif

#endif	/* __GNUC__ */

static inline int clzlx(unsigned long * x, int n)
{
	int r = 0;
	for( ; n > 0 && UNLIKELY(!x[n-1]) ; --n) r+=GMP_LIMB_BITS;
	if (n == 0) return r;
	r += clzl(x[n-1]);
	return r;
}
static inline int ctzlx(unsigned long * x, int n)
{
	int r = 0;
	for( ; n > 0 && UNLIKELY(!*x) ; --n,++x) r+=GMP_LIMB_BITS;
	if (n == 0) return r;
	r += ctzl(*x);
	return r;
}


#ifdef __cplusplus
}
#endif

#endif	/* MPFQ_H_ */
