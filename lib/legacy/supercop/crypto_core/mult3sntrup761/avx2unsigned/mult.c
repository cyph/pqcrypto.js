#include "crypto_core.h"

#include "crypto_int8.h"
#include "crypto_int16.h"
#include "crypto_int32.h"
#include "crypto_int64.h"
#include "crypto_uint64.h"
#define int8 crypto_int8
#define int16 crypto_int16
#define int32 crypto_int32
#define int64 crypto_int64
#define uint64 crypto_uint64
typedef int8 small;



#include "immintrin.h"

#include "const_inline.h"

#include "mult3_32x32.h"

#define LEN 768

#include "string.h"


#define ALIGNED __attribute((aligned(32)))

#define p 761


int crypto_core(unsigned char *outbytes,const unsigned char *inbytes,const unsigned char *kbytes,const unsigned char *cbytes)
{

  ALIGNED unsigned char a[LEN];
  ALIGNED unsigned char b[LEN];
  ALIGNED unsigned char c[LEN*2];
  int i;

  memcpy( a , inbytes , p ); for(i=p; i<LEN;i++) a[i] = 0;
  memcpy( b , kbytes  , p ); for(i=p; i<LEN;i++) b[i] = 0;

  // convert:
  // only use least 2 bits
  // 00->0 , 01->1, 10->0, 11->2
  for(i=0;i<LEN;i+=32) {
    _mm256_store_si256( (__m256i*)(a+i) , cvt_to_unsigned(_mm256_load_si256((__m256i*)(a+i))) );
  }
  for(i=0;i<LEN;i+=32) {
    _mm256_store_si256( (__m256i*)(b+i) , cvt_to_unsigned(_mm256_load_si256((__m256i*)(b+i))) );
  }

  //mult3_recursive_karatsuba( f , LEN );
  //mult3_768_karatsuba3( c , a , b );
  mult3_768_refined_karatsuba3( c , a , b );

  for (i = p+p-2;i >= p;--i) {
    c[i-p] = c[i-p] + c[i];
    c[i-p+1] = c[i-p+1] + c[i];
  }

  for(i=0;i<LEN;i+=32) {
    _mm256_store_si256( (__m256i*)(c+i) , cvt_to_int(_mm256_load_si256((__m256i*)(c+i))) );
  }

  for (i = 0;i < p;++i) outbytes[i] = c[i];
  return 0;
}
