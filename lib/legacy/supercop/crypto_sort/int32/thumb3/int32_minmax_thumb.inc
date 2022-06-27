#define int32_MINMAX(a,b) \
do { \
  asm( \
    "cmp %0,%1\n\t" \
    "ittt gt\n\t" \
    "eorgt %0,%1\n\t" \
    "eorgt %1,%0\n\t" \
    "eorgt %0,%1\n\t" \
    : "+r"(a), "+r"(b) \
    : \
    : "cc" \
  ); \
} while(0)
