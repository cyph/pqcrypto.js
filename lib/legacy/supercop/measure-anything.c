/*
 * measure-anything.c version 20170713
 * D. J. Bernstein
 * Public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include "kernelrandombytes.h"
#include "cpucycles.h"
#include "cpuid.h"
#include "measure.h"

static void printword(const char *s)
{
  if (!*s) putchar('-');
  while (*s) {
    if (*s == ' ') putchar('_');
    else if (*s == '\t') putchar('_');
    else if (*s == '\r') putchar('_');
    else if (*s == '\n') putchar('_');
    else putchar(*s);
    ++s;
  }
  putchar(' ');
}

static void printnum(long long x)
{
  printf("%lld ",x);
}

static void fail(const char *why)
{
  fprintf(stderr,"measure: fatal: %s\n",why);
  exit(111);
}

unsigned char *alignedcalloc(unsigned long long len)
{
  unsigned char *x = (unsigned char *) calloc(1,len + 128);
  if (!x) fail("out of memory");
  /* will never deallocate so shifting is ok */
  x += 63 & (-(unsigned long) x);
  return x;
}

static long long cyclespersecond;

static void printimplementations(void)
{
  int i;

  printword("implementation");
  printword(primitiveimplementation);
  printword(implementationversion);
  printf("\n"); fflush(stdout);

  for (i = 0;sizenames[i];++i) {
    printword(sizenames[i]);
    printnum(sizes[i]);
    printf("\n"); fflush(stdout);
  }

  printword("cpuid");
  printword(cpuid);
  printf("\n"); fflush(stdout);

  printword("cpucycles_persecond");
  printnum(cyclespersecond);
  printf("\n"); fflush(stdout);

  printword("cpucycles_implementation");
  printword(cpucycles_implementation);
  printf("\n"); fflush(stdout);

  printword("compiler");
  printword(COMPILER);
#if defined(__VERSION__) && !defined(__ICC)
  printword(__VERSION__);
#elif defined(__xlc__)
  printword(__xlc__);
#elif defined(__ICC)
  {
    char buf[256];

    sprintf(buf, "%d.%d.%d", __ICC/100, __ICC%100,
            __INTEL_COMPILER_BUILD_DATE);
    printword(buf);
  }
#elif defined(__PGIC__)
  {
    char buf[256];

    sprintf(buf, "%d.%d.%d", __PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__);
    printword(buf);
  }
#elif defined(__SUNPRO_C)
  {
    char buf[256];
    int major, minor, micro;

    micro = __SUNPRO_C & 0xf;
    minor = (__SUNPRO_C >> 4) & 0xf;
    major = (__SUNPRO_C >> 8) & 0xf;

    if (micro)
      sprintf(buf, "%d.%d.%d", major, minor, micro);
    else
      sprintf(buf, "%d.%d", major, minor);
    printword(buf);
  }
#else
  printword("unknown compiler version");
#endif
  printf("\n"); fflush(stdout);
}

void printentry(long long mbytes,const char *measuring,long long *m,long long mlen)
{
  long long i;
  long long j;
  long long belowj;
  long long abovej;

  printword(measuring);
  if (mbytes >= 0) printnum(mbytes); else printword("");
  if (mlen > 0) { 
    for (j = 0;j + 1 < mlen;++j) { 
      belowj = 0;
      for (i = 0;i < mlen;++i) if (m[i] < m[j]) ++belowj;
      abovej = 0;
      for (i = 0;i < mlen;++i) if (m[i] > m[j]) ++abovej;
      if (belowj * 2 < mlen && abovej * 2 < mlen) break;
    } 
    printnum(m[j]);
    if (mlen > 1) { 
      for (i = 0;i < mlen;++i) printnum(m[i]);
    } 
  } 
  printf("\n"); fflush(stdout);
}

void limits()
{
#ifdef RLIM_INFINITY
  struct rlimit r;
  r.rlim_cur = 0;
  r.rlim_max = 0;
#ifdef RLIMIT_NOFILE
  setrlimit(RLIMIT_NOFILE,&r);
#endif
#ifdef RLIMIT_NPROC
  setrlimit(RLIMIT_NPROC,&r);
#endif
#ifdef RLIMIT_CORE
  setrlimit(RLIMIT_CORE,&r);
#endif
#endif
}

static unsigned char randombyte[1];

int main()
{
  cyclespersecond = cpucycles();
  cyclespersecond = cpucycles();
  cyclespersecond = cpucycles_persecond();
  kernelrandombytes(randombyte,1);
  preallocate();
  limits();
  printimplementations();
  allocate();
  measure();
  return 0;
}
