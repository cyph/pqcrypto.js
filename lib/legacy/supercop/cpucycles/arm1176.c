/*
cpucycles/arm1176.c version 20200114
D. J. Bernstein
Public domain.

Target: ARM1176 core, as in Raspberry Pi 1A, 1A+, 1B, 1B+, CM1, 0, 0W.

Need kernel to do asm volatile("mcr p15,0,%0,c15,c9,0" :: "r"(1));
since hardware does not allow user access to cycle counter by default.
*/

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

static unsigned int readticks(void)
{
  static int enabled;
  unsigned int r;
  if (!enabled) {
    asm volatile("mrc p15, 0, %0, c15, c12, 0" : "=r"(r));
    asm volatile("mcr p15, 0, %0, c15, c12, 0" :: "r"(r|1));
    enabled = 1;
  }
  asm volatile("mrc p15, 0, %0, c15, c12, 1" : "=r"(r));
  return r;
}

/* startup state: all prev are 0 */
/* XXX: gettimeofday() clock assumed to not be 1970 */

/* previous observation of 32-bit cycle counter */
static long long cycles32prev;

/* previous observation of gettimeofday() */
static long long usecprev;
static long long secprev;

/* previous computed 64-bit cycle counter */
static unsigned long long cycles64prev;

/* previous computed cycles per second */
static long long cyclespersec;

long long cpucycles_arm1176(void)
{
  unsigned int cycles32now;
  struct timeval t;
  long long usecnow;
  long long secnow;
  long long usecdiff;
  long long secdiff;
  unsigned long long cycles32diff;
  unsigned long long guesscycles;
  
  cycles32now = readticks();

  gettimeofday(&t,(struct timezone *) 0);
  usecnow = t.tv_usec;
  secnow = t.tv_sec;

  cycles32diff = (unsigned int) (cycles32now - cycles32prev); /* unsigned change in number of cycles mod 2^32 */
  usecdiff = usecnow - usecprev; /* signed change in number of usec mod 10^9 */
  secdiff = secnow - secprev; /* signed change in number of sec */
  if ((secdiff == 0 && usecdiff < 20000) || (secdiff == 1 && usecdiff < -980000))
    return cycles64prev + cycles32diff;

  cycles32prev = cycles32now;
  usecprev = usecnow;
  secprev = secnow;

  usecdiff += 1000000 * (long long) secdiff;
  if (usecdiff > 0 && usecdiff < 30000) {
    /* XXX: assuming cycles do not increase by 2^32 in <0.03 seconds */
    cyclespersec = 1000000 * (unsigned long long) cycles32diff;
    cyclespersec /= usecdiff;
  } else {
    guesscycles = (usecdiff * cyclespersec) / 1000000;
    while (cycles32diff + 2147483648ULL < guesscycles) cycles32diff += 4294967296ULL;
    /* XXX: could do longer-term extrapolation here */
  }

  cycles64prev += cycles32diff;
  return cycles64prev;
}

long long cpucycles_arm1176_persecond(void)
{
  while (!cyclespersec) cpucycles_arm1176();
  return cyclespersec;
}
