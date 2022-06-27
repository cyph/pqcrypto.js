/*=============================================================================
This file has been adapted from the implementation 
(available at, CC0-1.0 License https://github.com/jschanck/ntru) 
of "NTRU:A submission to the NIST post-quantum standardization effort"
by : Cong Chen, Oussama Danba, Jeffrey Hoffstein, Andreas Hülsing, 
Joost Rijneveld, Tsunekazu Saito, John M. Schanck, Peter Schwabe, 
William Whyte,Keita Xagawa, Takashi Yamakawa, Zhenfei Zhang.
=============================================================================*/

#include "poly.h"

void poly_Sq_tobytes(unsigned char *r, const poly *a)
{
  int i;

  for(i=0;i<NTRU_PACK_DEG/2;i++)
  {
    r[3*i+ 0] =  MODQ(a->coeffs[2*i+0]) & 0xff;
    r[3*i+ 1] = (MODQ(a->coeffs[2*i+0]) >>  8) | ((MODQ(a->coeffs[2*i+1]) & 0x0f) << 4);
    r[3*i+ 2] = (MODQ(a->coeffs[2*i+1]) >>  4);
  }
}

void poly_Sq_frombytes(poly *r, const unsigned char *a)
{
  int i;
  for(i=0;i<NTRU_PACK_DEG/2;i++)
  {
    r->coeffs[2*i+0] = (a[3*i+ 0] >> 0) | (((uint16_t)a[3*i+ 1] & 0x0f) << 8);
    r->coeffs[2*i+1] = (a[3*i+ 1] >> 4) | (((uint16_t)a[3*i+ 2] & 0xff) << 4);
  }
  r->coeffs[NTRU_N-1] = 0;
}

void poly_Rq_sum_zero_tobytes(unsigned char *r, const poly *a)
{
  poly_Sq_tobytes(r, a);
}

void poly_Rq_sum_zero_frombytes(poly *r, const unsigned char *a)
{
  int i;
  poly_Sq_frombytes(r,a);

  /* Set r[n-1] so that the sum of coefficients is zero mod q */
  r->coeffs[NTRU_N-1] = 0;
  for(i=0;i<NTRU_PACK_DEG;i++)
    r->coeffs[NTRU_N-1] -= r->coeffs[i];
}
