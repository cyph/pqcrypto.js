/*
cpucycles/riscv.c version 20210422
D. J. Bernstein
Romain Dolbeau
Public domain.
*/

#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

long long cpucycles_riscv(void)
{
  long long result;
#if defined(__riscv_xlen)
#if __riscv_xlen == 64
  asm volatile("rdcycle %0" : "=r" (result));
#elif __riscv_xlen == 32
  unsigned int l, h, h2;
  asm volatile( "start%=:\n"
                "rdcycleh %0\n"
                "rdcycle %1\n"
                "rdcycleh %2\n"
                "bne %0, %2, start%=\n"
                : "=r" (h), "=r" (l), "=r" (h2));

  result = (((unsigned long long)h)<<32) | ((unsigned long long)l);
#else
#error "unknown __riscv_xlen"
#endif
#else // __riscv_xlen
#error "__riscv_xlen required for RISC-V support"
#endif // __riscv_xlen
  return result;
}

static long long microseconds(void)
{
  struct timeval t;
  gettimeofday(&t,(struct timezone *) 0);
  return t.tv_sec * (long long) 1000000 + t.tv_usec;
}

static double guessfreq(void)
{
  long long tb0; long long us0;
  long long tb1; long long us1;

  tb0 = cpucycles_riscv();
  us0 = microseconds();
  do {
    tb1 = cpucycles_riscv();
    us1 = microseconds();
  } while (us1 - us0 < 10000 || tb1 - tb0 < 1000);
  if (tb1 <= tb0) return 0;
  tb1 -= tb0;
  us1 -= us0;
  return ((double) tb1) / (0.000001 * (double) us1);
}

static long long cpufrequency = 0;

static void init(void)
{
  double guess1;
  double guess2;
  int loop;

  for (loop = 0;loop < 100;++loop) {
    guess1 = guessfreq();
    guess2 = guessfreq();
    if (guess1 > 1.01 * guess2) continue;
    if (guess2 > 1.01 * guess1) continue;
    cpufrequency = 0.5 * (guess1 + guess2);
    break;
  }
}

long long cpucycles_riscv_persecond(void)
{
  if (!cpufrequency) init();
  return cpufrequency;
}
