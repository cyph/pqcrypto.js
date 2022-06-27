#ifndef _CORE_32X32_H_
#define _CORE_32X32_H_

#define mult3_32x32 CRYPTO_NAMESPACE(mult3_32x32)
#define mult3_768_karatsuba3 CRYPTO_NAMESPACE(mult3_768_karatsuba3)
#define mult3_768_refined_karatsuba3 CRYPTO_NAMESPACE(mult3_768_refined_karatsuba3)


void mult3_32x32( unsigned char * h , const unsigned char * f , const unsigned char * g );

void mult3_768_karatsuba3( unsigned char * c , const unsigned char * a , const unsigned char * b );

// assume: a[512+254]=a[512+255]=0  b[512+254]=b[512+255]=0
void mult3_768_refined_karatsuba3( unsigned char * c , const unsigned char * a , const unsigned char * b );

#endif
