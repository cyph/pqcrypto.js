#include <x86intrin.h>

static inline __m128i shift(__m128i x,int bits)
{
  if (bits == 0) return x;
  return _mm_slli_epi32(x,bits);
}

static inline __m128i rotate(__m128i x,int bits)
{
  if (bits == 0) return x;
  return _mm_slli_epi32(x,bits) | _mm_srli_epi32(x,32 - bits);
}

static inline __m128i rotate24(__m128i x)
{
  return _mm_shuffle_epi8(x,
    _mm_set_epi8(
      12,15,14,13,8,11,10,9,4,7,6,5,0,3,2,1
    )
  );
}

static void gimli(uint32_t *state)
{
  int round;
  __m128i x = _mm_loadu_si128((void *) (state + 0));
  __m128i y = _mm_loadu_si128((void *) (state + 4));
  __m128i z = _mm_loadu_si128((void *) (state + 8));
  __m128i newx, newy, newz;

  for (round = 24;round > 0;--round) {
    x = rotate24(x);
    y = rotate(y,9);

    newz = x ^ shift(z,1) ^ shift(y&z,2);
    newy = y ^ x          ^ shift(x|z,1);
    newx = z ^ y          ^ shift(x&y,3);

    x = newx;
    y = newy;
    z = newz;

    if ((round & 3) == 0) // small swap: pattern s...s...s... etc.
      x = _mm_shuffle_epi32(x,_MM_SHUFFLE(2,3,0,1));
    if ((round & 3) == 2) // big swap: pattern ..S...S...S. etc.
      x = _mm_shuffle_epi32(x,_MM_SHUFFLE(1,0,3,2));

    if ((round & 3) == 0) { // add constant
      x ^= _mm_set_epi32(0,0,0,0x9e377900);
      x ^= _mm_set_epi32(0,0,0,round);
    }
  }

  _mm_storeu_si128((void *) (state + 0),x);
  _mm_storeu_si128((void *) (state + 4),y);
  _mm_storeu_si128((void *) (state + 8),z);
}
