#include <stdint.h>
#include <assert.h>
#include "polmul_params.h"
#include "poly.h"
#include "polyvec.h"
#include "consts256.h"

/*
void polyvec_uniform(polyvec *r, const uint8_t seed[POLYMUL_SYMBYTES], uint16_t nonce) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    poly_uniform(&r->vec[i], seed, (nonce << 8) + i);
}

void polyvec_noise(polyvec *r, const uint8_t seed[POLYMUL_SYMBYTES], uint16_t nonce) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    poly_noise(&r->vec[i], seed, (nonce << 8) + i);
}
*/
void polyvec_ntt(nttpolyvec *r, const polyvec *a, const int16_t *pdata) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    poly_ntt(&r->vec[i], &a->vec[i], pdata);
}

void polyvec_invntt_tomont(nttpolyvec *r, const nttpolyvec *a, const int16_t *pdata) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    poly_invntt_tomont(&r->vec[i], &a->vec[i], pdata);
}

void polyvec_crt(polyvec *r, const nttpolyvec *a, const nttpolyvec *b) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    poly_crt(&r->vec[i], &a->vec[i], &b->vec[i]);
}

void polyvec_matrix_vector_mul(polyvec *t, const polyvec a[KEM_K], const polyvec *s, int transpose) {
  unsigned int i, j;
  nttpolyvec shat, ahat, t0, t1;

  polyvec_ntt(&shat,s,PDATA0);
  for(i=0;i<KEM_K;i++) {
    for(j=0;j<KEM_K;j++) {
      if(transpose)
        poly_ntt(&ahat.vec[j],&a[j].vec[i],PDATA0);
      else
        poly_ntt(&ahat.vec[j],&a[i].vec[j],PDATA0);
    }
    polyvec_basemul_acc_montgomery(&t0.vec[i],&ahat,&shat,PDATA0);
  }

  polyvec_ntt(&shat,s,PDATA1);
  for(i=0;i<KEM_K;i++) {
    for(j=0;j<KEM_K;j++) {
      if(transpose)
        poly_ntt(&ahat.vec[j],&a[j].vec[i],PDATA1);
      else
        poly_ntt(&ahat.vec[j],&a[i].vec[j],PDATA1);
    }
    polyvec_basemul_acc_montgomery(&t1.vec[i],&ahat,&shat,PDATA1);
  }

  polyvec_invntt_tomont(&t0,&t0,PDATA0);
  polyvec_invntt_tomont(&t1,&t1,PDATA1);
  polyvec_crt(t,&t0,&t1);
}

void polyvec_matrix_vector_mul2(polyvec *t, const polyvec a[KEM_K], const polyvec *s, 
															nttpolyvec *shat1, nttpolyvec *shat2, int compute_shat, int transpose) {
  unsigned int i, j;
  nttpolyvec ahat, t0, t1;
	
	if(compute_shat)
  	polyvec_ntt(shat1,s,PDATA0);

  for(i=0;i<KEM_K;i++) {
    for(j=0;j<KEM_K;j++) {
      if(transpose)
        poly_ntt(&ahat.vec[j],&a[j].vec[i],PDATA0);
      else
        poly_ntt(&ahat.vec[j],&a[i].vec[j],PDATA0);
    }
    polyvec_basemul_acc_montgomery(&t0.vec[i],&ahat,shat1,PDATA0);
  }

	if(compute_shat)
  	polyvec_ntt(shat2,s,PDATA1);

  for(i=0;i<KEM_K;i++) {
    for(j=0;j<KEM_K;j++) {
      if(transpose)
        poly_ntt(&ahat.vec[j],&a[j].vec[i],PDATA1);
      else
        poly_ntt(&ahat.vec[j],&a[i].vec[j],PDATA1);
    }
    polyvec_basemul_acc_montgomery(&t1.vec[i],&ahat,shat2,PDATA1);
  }

  polyvec_invntt_tomont(&t0,&t0,PDATA0);
  polyvec_invntt_tomont(&t1,&t1,PDATA1);
  polyvec_crt(t,&t0,&t1);
}

/*
void vector_vector_mul(poly *t, const polyvec *a, const polyvec *s, 
															nttpolyvec *shat1, nttpolyvec *shat2, int compute_shat) {
  unsigned int i, j;
  nttpolyvec ahat;
	nttpoly t0, t1;
	
	if(compute_shat)
  	polyvec_ntt(shat1,s,PDATA0);

	polyvec_ntt(&ahat,a,PDATA0);
  polyvec_basemul_acc_montgomery(&t0,&ahat,shat1,PDATA0);

	if(compute_shat)
  	polyvec_ntt(shat2,s,PDATA1);

	polyvec_ntt(&ahat,a,PDATA1);
  polyvec_basemul_acc_montgomery(&t1,&ahat,shat2,PDATA1);


	if(compute_shat)
  	polyvec_ntt(shat2,s,PDATA1);

  for(i=0;i<KEM_K;i++) {
    for(j=0;j<KEM_K;j++) {
      if(transpose)
        poly_ntt(&ahat.vec[j],&a[j].vec[i],PDATA1);
      else
        poly_ntt(&ahat.vec[j],&a[i].vec[j],PDATA1);
    }
    polyvec_basemul_acc_montgomery(&t1.vec[i],&ahat,shat2,PDATA1);
  }

  polyvec_invntt_tomont(&t0,&t0,PDATA0);
  polyvec_invntt_tomont(&t1,&t1,PDATA1);
  polyvec_crt(t,&t0,&t1);
}
*/

void polyvec_iprod(poly *r, const polyvec *a, const polyvec *b) {
  nttpoly r0, r1;
  nttpolyvec ahat;
  nttpolyvec bhat;

  polyvec_ntt(&ahat,a,PDATA0);
  polyvec_ntt(&bhat,b,PDATA0);
  polyvec_basemul_acc_montgomery(&r0,&ahat,&bhat,PDATA0);

  polyvec_ntt(&ahat,a,PDATA1);
  polyvec_ntt(&bhat,b,PDATA1);
  polyvec_basemul_acc_montgomery(&r1,&ahat,&bhat,PDATA1);

  poly_invntt_tomont(&r0,&r0,PDATA0);
  poly_invntt_tomont(&r1,&r1,PDATA1);
  poly_crt(r,&r0,&r1);
}

void polyvec_iprod2(poly *r, const polyvec *a, const polyvec *s, 
										nttpolyvec *shat0, nttpolyvec *shat1, int compute_shat) {
  nttpoly r0, r1;
  nttpolyvec ahat;

	if(compute_shat){
  	polyvec_ntt(shat0,s,PDATA0);
  	polyvec_ntt(shat1,s,PDATA1);
	}

  polyvec_ntt(&ahat,a,PDATA0);
  polyvec_basemul_acc_montgomery(&r0,&ahat,shat0,PDATA0);

  polyvec_ntt(&ahat,a,PDATA1);
  polyvec_basemul_acc_montgomery(&r1,&ahat,shat1,PDATA1);

  poly_invntt_tomont(&r0,&r0,PDATA0);
  poly_invntt_tomont(&r1,&r1,PDATA1);
  poly_crt(r,&r0,&r1);
}
