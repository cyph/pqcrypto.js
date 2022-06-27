/*
   SHA-3 in C
   Odzhan
*/

#ifndef SHA3_H
#define SHA3_H

#include <stdint.h>
#include <string.h>

#define SHA3_ROUNDS       24
#define SHA3_STATE_LEN    25

#define SHA3_224                 0
#define SHA3_224_DIGEST_LENGTH  28
#define SHA3_224_CBLOCK        144

#define SHA3_256                 1
#define SHA3_256_DIGEST_LENGTH  32
#define SHA3_256_CBLOCK        136

#define SHA3_384                 2
#define SHA3_384_DIGEST_LENGTH  48
#define SHA3_384_CBLOCK        104

#define SHA3_512                3
#define SHA3_512_DIGEST_LENGTH 64
#define SHA3_512_CBLOCK        72

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#pragma pack(push, 1)
typedef struct _SHA3_CTX {
  uint64_t state[SHA3_STATE_LEN];
  uint32_t index;
  size_t   dgstlen;
  uint32_t rounds;
  size_t   blklen;
  uint8_t  blk[256];
} SHA3_CTX;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

  void SHA3_Init (SHA3_CTX *, int);
  void SHA3_Update (SHA3_CTX*, void *, size_t);
  void SHA3_Final (void*, SHA3_CTX*);

#ifdef __cplusplus
}
#endif

#endif
