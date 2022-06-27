#include <string.h>
#include "params.h"
#include "poly.h"
#include "ntt.h"
#include "reduce.h"
#include "fips202.h"
#include "fips202x4.h"

/*************************************************
* Name:        flipabs
*
* Description: Computes |(x mod q) - Q/2|
*
* Arguments:   int16_t x: input coefficient
*
* Returns |(x mod q) - Q/2|
**************************************************/
static uint16_t flipabs(int16_t x)
{
  int16_t r,m;
  
  r = barrett_reduce(x);
  r -= NEWHOPECMPCT_Q/2;
  
  m = r >> 15;
  return (r + m) ^ m;
}

/*************************************************
* Name:        poly_frombytes
*
* Description: De-serialization of a polynomial
*
* Arguments:   - poly *r:                pointer to output polynomial
*              - const unsigned char *a: pointer to input byte array
**************************************************/
void poly_frombytes(poly *r, const unsigned char *a)
{
  int i;
  for(i=0;i<NEWHOPECMPCT_N/2;i++)
  {
    r->coeffs[2*i+0] =  a[3*i+0]       | (((uint16_t)a[3*i+1] & 0x0f) << 8);
    r->coeffs[2*i+1] = (a[3*i+1] >> 4) | (((uint16_t)a[3*i+2] & 0xff) << 4);
  }
}

/*************************************************
* Name:        poly_tobytes
*
* Description: Serialization of a polynomial
*
* Arguments:   - unsigned char *r: pointer to output byte array
*              - const poly *p:    pointer to input polynomial
**************************************************/
void poly_tobytes(unsigned char *r, const poly *p)
{
  int i;
  uint16_t t0,t1;
  for(i=0;i<NEWHOPECMPCT_N/2;i++)
  {
    t0 = barrett_reduce(p->coeffs[2*i+0]);
    t1 = barrett_reduce(p->coeffs[2*i+1]);

    r[3*i+0] =  t0 & 0xff;
    r[3*i+1] = (t0 >> 8) | ((t1 & 0xf) << 4);
    r[3*i+2] = (t1 >> 4);
  }
}

/*************************************************
* Name:        poly_compress
*
* Description: Compression and subsequent serialization of a polynomial
*
* Arguments:   - unsigned char *r: pointer to output byte array
*              - const poly *a:    pointer to input polynomial
**************************************************/
void poly_compress(unsigned char * restrict r, const poly * restrict a)
{
  uint8_t t[8];
  int i,j,k=0;

#if (NEWHOPECMPCT_POLYCOMPRESSEDBITS == 3)
  for(i=0;i<NEWHOPECMPCT_N;i+=8)
  { 
    for(j=0;j<8;j++)
      t[j] = ((((uint16_t)barrett_reduce(a->coeffs[i+j]) << 3) + NEWHOPECMPCT_Q/2) / NEWHOPECMPCT_Q) & 7;

    r[k]   =  t[0]       | (t[1] << 3) | (t[2] << 6);
    r[k+1] = (t[2] >> 2) | (t[3] << 1) | (t[4] << 4) | (t[5] << 7);
    r[k+2] = (t[5] >> 1) | (t[6] << 2) | (t[7] << 5);
    k += 3;
  }
#elif (NEWHOPECMPCT_POLYCOMPRESSEDBITS == 4)
  for(i=0;i<NEWHOPECMPCT_N;i+=8)
  {
    for(j=0;j<8;j++)
      t[j] = ((((uint16_t)barrett_reduce(a->coeffs[i+j]) << 4) + NEWHOPECMPCT_Q/2) / NEWHOPECMPCT_Q) & 15;

    r[k]   = t[0] | (t[1] << 4);
    r[k+1] = t[2] | (t[3] << 4);
    r[k+2] = t[4] | (t[5] << 4);
    r[k+3] = t[6] | (t[7] << 4);
    k += 4;
  }
#else
#error "NEWHOPECMPCT_POLYCOMPRESSEDBITS needs to be in {3, 4}"
#endif
}

/*************************************************
* Name:        poly_decompress
*
* Description: De-serialization and subsequent decompression of a polynomial;
*              approximate inverse of poly_compress
*
* Arguments:   - poly *r:                pointer to output polynomial
*              - const unsigned char *a: pointer to input byte array
**************************************************/
void poly_decompress(poly * restrict r, const unsigned char * restrict a)
{
  int i;
#if (NEWHOPECMPCT_POLYCOMPRESSEDBITS == 3)
  for(i=0;i<NEWHOPECMPCT_N;i+=8)
  {
    r->coeffs[i+0] =  (((a[0] & 7) * NEWHOPECMPCT_Q) + 4) >> 3;
    r->coeffs[i+1] = ((((a[0] >> 3) & 7) * NEWHOPECMPCT_Q) + 4) >> 3;
    r->coeffs[i+2] = ((((a[0] >> 6) | ((a[1] << 2) & 4)) * NEWHOPECMPCT_Q) + 4) >> 3;
    r->coeffs[i+3] = ((((a[1] >> 1) & 7) * NEWHOPECMPCT_Q) + 4) >> 3;
    r->coeffs[i+4] = ((((a[1] >> 4) & 7) * NEWHOPECMPCT_Q) + 4) >> 3;
    r->coeffs[i+5] = ((((a[1] >> 7) | ((a[2] << 1) & 6)) * NEWHOPECMPCT_Q) + 4) >> 3;
    r->coeffs[i+6] = ((((a[2] >> 2) & 7) * NEWHOPECMPCT_Q) + 4) >> 3;
    r->coeffs[i+7] = ((((a[2] >> 5)) * NEWHOPECMPCT_Q) + 4) >> 3;
    a += 3;
  }
#elif (NEWHOPECMPCT_POLYCOMPRESSEDBITS == 4)
  for(i=0;i<NEWHOPECMPCT_N;i+=8)
  {
    r->coeffs[i+0] = (((a[0] & 15) * NEWHOPECMPCT_Q) + 8) >> 4;
    r->coeffs[i+1] = (((a[0] >> 4) * NEWHOPECMPCT_Q) + 8) >> 4;
    r->coeffs[i+2] = (((a[1] & 15) * NEWHOPECMPCT_Q) + 8) >> 4;
    r->coeffs[i+3] = (((a[1] >> 4) * NEWHOPECMPCT_Q) + 8) >> 4;
    r->coeffs[i+4] = (((a[2] & 15) * NEWHOPECMPCT_Q) + 8) >> 4;
    r->coeffs[i+5] = (((a[2] >> 4) * NEWHOPECMPCT_Q) + 8) >> 4;
    r->coeffs[i+6] = (((a[3] & 15) * NEWHOPECMPCT_Q) + 8) >> 4;
    r->coeffs[i+7] = (((a[3] >> 4) * NEWHOPECMPCT_Q) + 8) >> 4;
    a += 4;
  }
#else
#error "NEWHOPECMPCT_POLYCOMPRESSEDBITS needs to be in {3, 4}"
#endif
}

/*************************************************
* Name:        poly_frommsg
*
* Description: Convert 32-byte message to polynomial
*
* Arguments:   - poly *r:                  pointer to output polynomial
*              - const unsigned char *msg: pointer to input message
**************************************************/
void poly_frommsg(poly *r, const unsigned char *msg)
{
  unsigned int i,j,mask;
  for(i=0;i<NEWHOPECMPCT_SYMBYTES;i++)
  {
    for(j=0;j<8;j++)
    {
      mask = -((msg[i] >> j)&1);
      r->coeffs[8*i+j+  0] = mask & (NEWHOPECMPCT_Q/2);
      r->coeffs[8*i+j+256] = mask & (NEWHOPECMPCT_Q/2);
#if   (NEWHOPECMPCT_N == 768)
      r->coeffs[8*i+j+512] = mask & (NEWHOPECMPCT_Q/2);
#elif (NEWHOPECMPCT_N == 1024)
      r->coeffs[8*i+j+512] = mask & (NEWHOPECMPCT_Q/2);
      r->coeffs[8*i+j+768] = mask & (NEWHOPECMPCT_Q/2);
#endif
    }
  }
}

/*************************************************
* Name:        poly_tomsg
*
* Description: Convert polynomial to 32-byte message
*
* Arguments:   - unsigned char *msg: pointer to output message
*              - const poly *x:      pointer to input polynomial
**************************************************/
void poly_tomsg(unsigned char *msg, const poly *x)
{
  unsigned int i;
  uint16_t t;

  memset(msg,0,NEWHOPECMPCT_SYMBYTES);

  for(i=0;i<256;i++)
  {
    t  = flipabs(x->coeffs[i+  0]);
    t += flipabs(x->coeffs[i+256]);
#if   (NEWHOPECMPCT_N == 512)    
    t = (t - NEWHOPECMPCT_Q/2);
#elif (NEWHOPECMPCT_N == 768)
    t += flipabs(x->coeffs[i+512]);
    t = (t - 3*NEWHOPECMPCT_Q/4);
#elif (NEWHOPECMPCT_N == 1024)
    t += flipabs(x->coeffs[i+512]);
    t += flipabs(x->coeffs[i+768]);
    t = (t - NEWHOPECMPCT_Q);
#endif

    t >>= 15;
    msg[i>>3] |= t<<(i&7);
  }
}
 
/*************************************************
* Name:        poly_uniform
*
* Description: Sample a polynomial deterministically from a seed,
*              with output polynomial looking uniformly random
*
* Arguments:   - poly *a:                   pointer to output polynomial
*              - const unsigned char *seed: pointer to input seed
**************************************************/
static void poly_uniform_ref(poly *a, const unsigned char *seed)
{
  unsigned int ctr=0;
  uint16_t val;
  uint64_t state[25];
  uint8_t buf[SHAKE128_RATE];
  uint8_t extseed[(NEWHOPECMPCT_SYMBYTES+1)];
  int i,j;
    
  for(i=0;i<NEWHOPECMPCT_SYMBYTES;i++)
    extseed[i] = seed[i];

  for(i=0;i<NEWHOPECMPCT_N/64;i++) /* generate a in blocks of 256 coefficients */
  {
    ctr = 0;
    extseed[NEWHOPECMPCT_SYMBYTES] = i; /* domain-separate the 16 independent calls */
    shake128_absorb(state, extseed, NEWHOPECMPCT_SYMBYTES+1);
    while(ctr < 64) /* Very unlikely to run more than once */
    {
      shake128_squeezeblocks(buf,1,state);
      for(j=0;j<SHAKE128_RATE && ctr < 64;j+=2)
      {
        val = (buf[j] | ((uint16_t) buf[j+1] << 8));
#if   (NEWHOPECMPCT_N == 512 || NEWHOPECMPCT_N == 1024)
        if(val < 19*NEWHOPECMPCT_Q)
#elif (NEWHOPECMPCT_N == 768)
        if(val < 18*NEWHOPECMPCT_Q)      
#endif
        {
          a->coeffs[i*64+ctr] = val;
          ctr++;
        }
      }
    }
  }
}
static int rej_sample(int16_t *r, unsigned char *buf, size_t buflen)
{
    /* Rejection sampling */
    unsigned int ctr = 0, j;
    uint16_t val;

    for(j=0;j<buflen && ctr < 64;j+=2)
    {
      val = (buf[j] | ((uint16_t) buf[j+1] << 8));
#if   (NEWHOPECMPCT_N == 512 || NEWHOPECMPCT_N == 1024)
        if(val < 19*NEWHOPECMPCT_Q)
#elif (NEWHOPECMPCT_N == 768)
        if(val < 18*NEWHOPECMPCT_Q)      
#endif
        r[ctr++] = val;
    }
    if(ctr != 64) 
      return -1;
    return 0;
}


void poly_uniform(poly *a, const unsigned char *seed)
{
  uint8_t buf0[SHAKE128_RATE];
  uint8_t buf1[SHAKE128_RATE];
  uint8_t buf2[SHAKE128_RATE];
  uint8_t buf3[SHAKE128_RATE];
  uint8_t extseed0[NEWHOPECMPCT_SYMBYTES+1];
  uint8_t extseed1[NEWHOPECMPCT_SYMBYTES+1];
  uint8_t extseed2[NEWHOPECMPCT_SYMBYTES+1];
  uint8_t extseed3[NEWHOPECMPCT_SYMBYTES+1];
  int i;

  for(i=0;i<NEWHOPECMPCT_SYMBYTES;i++)
  {
    extseed0[i] = seed[i];
    extseed1[i] = seed[i];
    extseed2[i] = seed[i];
    extseed3[i] = seed[i];
  }

  for(i=0;i<NEWHOPECMPCT_N/256;i++)
  {
    extseed0[NEWHOPECMPCT_SYMBYTES] = 4*i;
    extseed1[NEWHOPECMPCT_SYMBYTES] = 4*i+1;
    extseed2[NEWHOPECMPCT_SYMBYTES] = 4*i+2;
    extseed3[NEWHOPECMPCT_SYMBYTES] = 4*i+3;

    shake128x4(buf0, buf1, buf2, buf3, SHAKE128_RATE, extseed0, extseed1, extseed2, extseed3, NEWHOPECMPCT_SYMBYTES+1);

    /* Rejection sampling */
    if(rej_sample(a->coeffs+i*256,buf0,SHAKE128_RATE))
    {
      poly_uniform_ref(a, seed);
      return;
    }
    if(rej_sample(a->coeffs+i*256+64,buf1,SHAKE128_RATE))
    {
      poly_uniform_ref(a, seed);
      return;
    }
    if(rej_sample(a->coeffs+i*256+128,buf2,SHAKE128_RATE))
    {
      poly_uniform_ref(a, seed);
      return;
    }
    if(rej_sample(a->coeffs+i*256+192,buf3,SHAKE128_RATE))
    {
      poly_uniform_ref(a, seed);
      return;
    }
  }
}
/*************************************************
* Name:        poly_sample
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter k=8
*
* Arguments:   - poly *r:                   pointer to output polynomial
*              - const unsigned char *seed: pointer to input seed
*              - unsigned char nonce:       one-byte input nonce
**************************************************/
static void cbd(int16_t *r, const unsigned char *buf)
{
  uint32_t t, d, a, b;
  int j,k;
  for(j=0;j<128;j+=4)
  {
    t = buf[j] | ((uint32_t)buf[j+1] << 8) | ((uint32_t)buf[j+2] << 16) | ((uint32_t)buf[j+3] << 24);
    d = t & 0x55555555;
    d += (t >> 1) & 0x55555555;
      
    for(k=0;k<8;k++)
    {
      a = (d >>  4*k)    & 0x3;
      b = (d >> (4*k+2)) & 0x3;
      r[2*j+k] = a - b;
    }
  }
}

void poly_sample(poly *r, const unsigned char *seed, unsigned char nonce)
{
#if NEWHOPECMPCT_K != 2
#error "poly_getnoise in poly.c only supports k=2"
#endif
  unsigned char buf0[128];
  unsigned char buf1[128];
  unsigned char buf2[128];
  unsigned char buf3[128];
  int i;

  unsigned char extseed0[NEWHOPECMPCT_SYMBYTES+2];
  unsigned char extseed1[NEWHOPECMPCT_SYMBYTES+2];
  unsigned char extseed2[NEWHOPECMPCT_SYMBYTES+2];
  unsigned char extseed3[NEWHOPECMPCT_SYMBYTES+2];

  for(i=0;i<NEWHOPECMPCT_SYMBYTES;i++)
  {
    extseed0[i] = seed[i];
    extseed1[i] = seed[i];
    extseed2[i] = seed[i];
    extseed3[i] = seed[i];
  }
  extseed0[NEWHOPECMPCT_SYMBYTES] = nonce;
  extseed1[NEWHOPECMPCT_SYMBYTES] = nonce;
  extseed2[NEWHOPECMPCT_SYMBYTES] = nonce;
  extseed3[NEWHOPECMPCT_SYMBYTES] = nonce;

    extseed0[NEWHOPECMPCT_SYMBYTES+1] = 4*i;
    extseed1[NEWHOPECMPCT_SYMBYTES+1] = 4*i+1;
    extseed2[NEWHOPECMPCT_SYMBYTES+1] = 4*i+2;
    extseed3[NEWHOPECMPCT_SYMBYTES+1] = 4*i+3;

    shake256x4(buf0, buf1, buf2, buf3,128,extseed0, extseed1, extseed2, extseed3, NEWHOPECMPCT_SYMBYTES+2);

    cbd(r->coeffs+0*256, buf0);
    cbd(r->coeffs+1*256, buf1);
#if (NEWHOPECMPCT_N==768)    
    cbd(r->coeffs+2*256, buf2);
#elif (NEWHOPECMPCT_N==1024)
    cbd(r->coeffs+2*256, buf2);
    cbd(r->coeffs+3*256, buf3);
#endif
}
/*************************************************
* Name:        poly_basemul
*
* Description: Multiply two polynomials in NTT domain.
*
* Arguments:   - poly *r:       pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/

void poly_basemul(poly *r, const poly *a, const poly *b)
{
  int i;
  for(i=0;i<NEWHOPECMPCT_NTT_LENGTH/16;i++) {
    basemul(r->coeffs + i*16,
        a->coeffs + i*16,
        b->coeffs + i*16,
        i*32);
  }
}
/*************************************************
* Name:        poly_add
*
* Description: Add two polynomials
*
* Arguments:   - poly *r:       pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void poly_add(poly *r, const poly *a, const poly *b)
{
  int i;
  for(i=0;i<NEWHOPECMPCT_N;i++)
    r->coeffs[i] = (a->coeffs[i] + b->coeffs[i]);
}

/*************************************************
* Name:        poly_sub
*
* Description: Subtract two polynomials
*
* Arguments:   - poly *r:       pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void poly_sub(poly *r, const poly *a, const poly *b)
{
  int i;
  for(i=0;i<NEWHOPECMPCT_N;i++)
    r->coeffs[i] = (a->coeffs[i] - b->coeffs[i]);
}

/*************************************************
* Name:        poly_ntt
*
* Description: Forward NTT transform of a polynomial in place
*              Input is assumed to have coefficients in bitreversed order
*              Output has coefficients in normal order
*
* Arguments:   - poly *r: pointer to in/output polynomial
**************************************************/
void poly_ntt(poly *r)
{
  int i;
  for(i=0;i<NEWHOPECMPCT_NTT_POLY;i++)
    ntt(r->coeffs+i*NEWHOPECMPCT_NTT_LENGTH);
}

/*************************************************
* Name:        poly_invntt
*
* Description: Inverse NTT transform of a polynomial in place
*              Input is assumed to have coefficients in normal order
*              Output has coefficients in normal order
*
* Arguments:   - poly *r: pointer to in/output polynomial
**************************************************/
void poly_invntt(poly *r)
{  
  int i;
  for(i=0;i<NEWHOPECMPCT_NTT_POLY;i++)  
    invntt(r->coeffs+i*NEWHOPECMPCT_NTT_LENGTH);
}

