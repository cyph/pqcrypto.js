#define int32_MINMAX(a,b) \
do { \
  int32 temp1; \
  asm( \
    "cmp %w0,%w1\n\t" \
    "mov %w2,%w0\n\t" \
    "csel %w0,%w1,%w0,gt\n\t" \
    "csel %w1,%w2,%w1,gt\n\t" \
    : "+r"(a), "+r"(b), "=r"(temp1) \
    : \
    : "cc" \
  ); \
} while(0)
