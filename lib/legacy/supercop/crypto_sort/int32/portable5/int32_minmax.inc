#define int32_MINMAX(a,b) \
do { \
  int32 ab = b ^ a; \
  int32 c = b - a; \
  c ^= ab & (c ^ b); \
  c >>= 31; \
  c &= ab; \
  a ^= c; \
  b ^= c; \
} while(0)
