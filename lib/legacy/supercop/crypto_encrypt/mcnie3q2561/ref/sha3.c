
/* 
   SHA-3 in C
   Odzhan
*/

#include "sha3.h"

void SHA3_Init (SHA3_CTX *ctx, int type)
{
  memset (ctx, 0, sizeof (SHA3_CTX));
  ctx->rounds = SHA3_ROUNDS;
  
  switch (type)
  {
  case SHA3_224:
    ctx->blklen  = SHA3_224_CBLOCK;
    ctx->dgstlen = SHA3_224_DIGEST_LENGTH;
    break;
  case SHA3_384:
    ctx->blklen  = SHA3_384_CBLOCK;
    ctx->dgstlen = SHA3_384_DIGEST_LENGTH;
    break;
  case SHA3_512:
    ctx->blklen  = SHA3_512_CBLOCK;
    ctx->dgstlen = SHA3_512_DIGEST_LENGTH;
    break;
  default:
    ctx->blklen  = SHA3_256_CBLOCK;
    ctx->dgstlen = SHA3_256_DIGEST_LENGTH;
    break;
  }   
}
const uint64_t keccakf_rndc[24] = 
{
  0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
  0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
  0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
  0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
  0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
  0x8000000000008003, 0x8000000000008002, 0x8000000000000080, 
  0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
  0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

const int keccakf_rotc[24] = 
{
  1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14, 
  27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};

const int keccakf_piln[24] = 
{
  10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4, 
  15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1 
};

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

void SHA3_Transform (SHA3_CTX *ctx)
{
  uint32_t i, j, round;
  uint64_t t, bc[5];
  uint64_t *st=ctx->state;
  uint64_t *p=(uint64_t*)ctx->blk;
  
  /* xor block with state */
  for (i=0; i<ctx->blklen/8; i++) st[i] ^= p[i];
  
  for (round = 0; round < ctx->rounds; round++) 
  {
    /* Theta */
    for (i = 0; i < 5; i++)     
    bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];

    for (i = 0; i < 5; i++) {
      t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
      for (j = 0; j < 25; j += 5)
      st[j + i] ^= t;
    }

    /* Rho Pi */
    t = st[1];
    for (i = 0; i < 24; i++) {
      j = keccakf_piln[i];
      bc[0] = st[j];
      st[j] = ROTL64(t, keccakf_rotc[i]);
      t = bc[0];
    }

    /* Chi */
    for (j = 0; j < 25; j += 5) {
      for (i = 0; i < 5; i++)
      bc[i] = st[j + i];
      for (i = 0; i < 5; i++)
      st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
    }
    
    /* Iota */
    st[0] ^= keccakf_rndc[round];
  }
}

void SHA3_Update (SHA3_CTX* ctx, void *in, size_t len) {
  uint8_t *p;
  size_t  r, idx;

  p   = (uint8_t*)in;
  idx = ctx->index;
  
  do {
    r = MIN(len, ctx->blklen - idx);
    memcpy ((void*)&ctx->blk[idx], p, r);
    if ((idx + r) < ctx->blklen) {
      idx += r;
      break;
    }
    SHA3_Transform (ctx);
    len -= r;
    idx = 0;
    p += r;
  } while (1);
  ctx->index=idx;
}

void SHA3_Final (void* dgst, SHA3_CTX* ctx)
{
  /* add 3 bits, Keccak uses 1 
     a lot of online implementations are using 1 instead of 6
     since the NIST specifications haven't been finalized. */
  ctx->blk[ctx->index] = 6;
  /* or the end bit */
  ctx->blk[ctx->blklen-1] |= 0x80;
  /* update context */
  SHA3_Transform (ctx);
  /* copy digest to buffer */
  memcpy (dgst, (uint8_t*)ctx->state, ctx->dgstlen);
}
