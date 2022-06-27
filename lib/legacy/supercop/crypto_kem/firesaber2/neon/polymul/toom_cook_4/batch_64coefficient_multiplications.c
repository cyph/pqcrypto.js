/*=============================================================================
 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/



#include "../../SABER_indcpa.h"
#include "../../SABER_params.h"

#include "asimd_matrix.c"
#include "asimd_scm_neon.c"


#define SCM_SIZE 16

uint16_t c_avx[16 * 2 * SCM_SIZE];
uint16_t a[16 * (SCM_SIZE + 2)];
uint16_t b[16 * (SCM_SIZE + 2)];
uint16_t c_avx_extra[16 * 4];

uint16_t a_extra[2*16], b_extra[2*16];
uint16x8x2_t tmp;

// load c <= a 
#define vload(c, a) c = vld1q_u16_x2(a);

// store c <= a 
#define vstore(c, a) vst1q_u16_x2(c, a);

// copy c<= a
#define vcopy(c, a) vload(tmp, a); vstore(c, tmp);

// c = a << value 
#define vsl(c, a, value) \
    c.val[0] = vshlq_n_u16(a.val[0], value); \
    c.val[1] = vshlq_n_u16(a.val[1], value); 

// c = a >> value 
#define vsr(c, a, value) \
	  c.val[0] = vshrq_n_u16(a.val[0], value); \
  	c.val[1] = vshrq_n_u16(a.val[1], value);

// c = a + b
#define vadd(c, a, b) \
	c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
	c.val[1] = vaddq_u16(a.val[1], b.val[1]);

// c = a - b
#define vsub(c, a, b) \
	c.val[0] = vsubq_u16(a.val[0], b.val[0]); \
	c.val[1] = vsubq_u16(a.val[1], b.val[1]);

// c = a * value 
#define vmuln(c, a, value) \
	c.val[0] = vmulq_n_u16(a.val[0], value); \
  	c.val[1] = vmulq_n_u16(a.val[1], value);

// c = a ^ b 
#define vxor(c, a, b) \
	c.val[0] = veorq_u16(a.val[0], b.val[0]); \
	c.val[1] = veorq_u16(a.val[1], b.val[1]);


// Position <= 14
static inline 
void karatsuba32_fork_avx_new(uint16x8x2_t *a1, uint16x8x2_t *b1, uint16_t position) {
  uint16x8x2_t tmp;

  vstore(&a[position*16], a1[0]);
  vstore(&b[position*16], b1[0]);
  
  vstore(&a[(position + 1)*16], a1[1]);
  vstore(&b[(position + 1)*16], b1[1]);

  vadd(tmp, a1[0], a1[1]);
  vstore(&a[(position + 2)*16], tmp);

  vadd(tmp, b1[0], b1[1]);
  vstore(&b[(position + 2)*16], tmp);

}

// Position > 14
static inline 
void karatsuba32_fork_avx_new1(uint16x8x2_t *a1, uint16x8x2_t *b1, uint16_t position) {
  uint16x8x2_t tmp;

  vstore(&a[position*16], a1[0]);
  vstore(&b[position*16], b1[0]);
  
  vstore(&a_extra[0*16], a1[1]);
  vstore(&b_extra[0*16], b1[1]);

  vadd(tmp, a1[0], a1[1]);
  vstore(&a_extra[1*16], tmp);

  vadd(tmp, b1[0], b1[1]);
  vstore(&b_extra[1*16], tmp);

}

static inline 
void karatsuba32_fork_avx_partial(uint16x8x2_t *a1, uint16x8x2_t *b1, uint16_t position) {
  uint16x8x2_t tmp;

  vstore(&a[position*16], a1[1]);
  vstore(&b[position*16], b1[1]);

  vadd(tmp, a1[0], a1[1]);
  vstore(&a[(position + 1)*16], tmp);

  vadd(tmp, b1[0], b1[1]);
  vstore(&b[(position + 1)*16], tmp);
}

static inline 
void karatsuba32_fork_avx_partial1(uint16x8x2_t *a1, uint16x8x2_t *b1, uint16_t position) {
  uint16x8x2_t tmp;

  vadd(tmp, a1[0], a1[1]);
  vstore(&a[position*16], tmp);

  vadd(tmp, b1[0], b1[1]);
  vstore(&b[position*16], tmp);
}

// static inline 
void karatsuba32_join_avx_new(uint16_t *result_final, uint16_t position) {

  uint16x8x2_t c1_tmp, c2_tmp;
  uint16x8x2_t b0, b1, b2;
  uint16x8x2_t rf[4];

  vload(rf[0], &c_avx[position*16 ] );
  vload(rf[3], &c_avx[(position + 1 + 16)*16 ] );

  vstore(&result_final[0*16], rf[0]);
  vstore(&result_final[3*16], rf[3]);

  vload(c1_tmp, &c_avx[(position + 16)*16]);
  vload(c2_tmp, &c_avx[(position + 2)*16]);
  // b[0] = resultd0[n-1:n/2] + resultd01[n/2-1:0]
  vadd(b0, c1_tmp, c2_tmp);

  vload(c1_tmp, &c_avx[(position + 2 + 16)*16]);
  vload(c2_tmp, &c_avx[(position + 1)*16]);
  // b[1] = resultd01[n-1:n/2] + resultd1[n/2-1:0]
  vadd(b1, c1_tmp, c2_tmp);

  // b[0] = b[0] - a[0] - a[2]
  vsub(b2, b0, rf[0]);
  vsub(rf[1], b2, c2_tmp);
  vstore(&result_final[1*16], rf[1]);

  vload(c2_tmp, &c_avx[(position + 16)*16]);
  // b[1] = b[1] - a[1] - a[3]
  vsub(b2, b1, c2_tmp);
  vsub(rf[2], b2, rf[3]);
  vstore(&result_final[2*16], rf[2]);

}

// static inline 
void karatsuba32_join_avx_partial(uint16_t *result_final, uint16_t position) {

  uint16x8x2_t c1_tmp, c2_tmp;
  uint16x8x2_t b0, b1, b2;
  uint16x8x2_t rf[4];

  vload(rf[0], &c_avx_extra[0*16]);
  vload(rf[3], &c_avx[(position + 16)*16]);

  vstore(&result_final[0*16], rf[0]);
  vstore(&result_final[3*16], rf[3]);

  vload(c1_tmp, &c_avx_extra[1*16]);
  vload(c2_tmp, &c_avx[(position + 1)*16]);
  // b[0] = resultd0[n-1:n/2] + resultd01[n/2-1:0]
  vadd(b0, c1_tmp, c2_tmp);

  vload(c1_tmp, &c_avx[(position + 1 + 16)*16]);
  vload(c2_tmp, &c_avx[(position)*16]);
  // b[1] = resultd01[n-1:n/2] + resultd1[n/2-1:0]
  vadd(b1, c1_tmp, c2_tmp);

  // b[0] = b[0] - a[0] - a[2]
  vsub(b2, b0, rf[0]);
  vsub(rf[1], b2, c2_tmp);
  vstore(&result_final[1*16], rf[1]);
  
  vload(c2_tmp, &c_avx_extra[1*16]);
  // b[1] = b[1] - a[1] - a[3]
  vsub(b2, b1, c2_tmp);
  vsub(rf[2], b2, rf[3]);
  vstore(&result_final[2*16], rf[2]);
}

// static inline 
void karatsuba32_join_avx_partial2(uint16_t *result_final, uint16_t position) {

  uint16x8x2_t c1_tmp, c2_tmp;
  uint16x8x2_t b0, b1, b2;
  uint16x8x2_t rf[4];

  vload(rf[0], &c_avx_extra[0*16]);
  vload(rf[3], &c_avx_extra[3*16]);

  vstore(&result_final[0*16], rf[0]);
  vstore(&result_final[3*16], rf[3]);

  vload(c1_tmp, &c_avx_extra[1*16]);
  vload(c2_tmp, &c_avx[position*16]);
  // b[0] = resultd0[n-1:n/2] + resultd01[n/2-1:0]
  vadd(b0, c1_tmp, c2_tmp);

  vload(c1_tmp, &c_avx[(position + 16)*16]);
  vload(c2_tmp, &c_avx_extra[2*16]);
  // b[1] = resultd01[n-1:n/2] + resultd1[n/2-1:0]
  vadd(b1, c1_tmp, c2_tmp);

  // b[0] = b[0] - a[0] - a[2]
  vsub(b2, b0, rf[0]);
  vsub(rf[1], b2, c2_tmp);
  vstore(&result_final[1*16], rf[1]);
  
  vload(c2_tmp, &c_avx_extra[1*16]);
  // b[1] = b[1] - a[1] - a[3]
  vsub(b2, b1, c2_tmp);
  vsub(rf[2], b2, rf[3]);
  vstore(&result_final[2*16], rf[2]);
}

// static inline 
void join_32coefficient_results(uint16_t *result_d0, uint16_t *result_d1,
                                uint16_t *result_d01, uint16_t *result_64ks) {
  
  uint16x8x2_t rd0[4], rd1[4];
  uint16x8x2_t b4, b5, b6, b7;
  uint16x8x2_t tmp;
  uint16_t i;
  for (i = 0; i < 4; i++)
  {
    vload(rd0[i], &result_d0[i*16]);
    vload(rd1[i], &result_d1[i*16]);
  }

  // {bb[5],bb[4]} = resultd0[63:32] + resultd01[31:0]
  vload(tmp, &result_d01[0*16]);
  vadd(b4, rd0[2], tmp);
  vload(tmp, &result_d01[1*16]);
  vadd(b5, rd0[3], tmp);

  // {bb[7],bb[6]} = resultd01[63:32] + resultd1[31:0]
  vload(tmp, &result_d01[2*16]);
  vadd(b6, tmp, rd1[0]);
  vload(tmp, &result_d01[3*16]);
  vadd(b7, tmp, rd1[1]);

  vstore(&b[4*16], b4);
  vstore(&b[5*16], b5);
  vstore(&b[6*16], b6);
  vstore(&b[7*16], b7);

  // {bb[7],bb[6],bb[5],bb[4]} <-- {bb[7],bb[6],bb[5],bb[4]} - {a[3],a[2],a[1],a[0]} - {a[7],a[6],a[5],a[4]}
  vsub(b4, b4, rd0[0]);
  vsub(b4, b4, rd1[0]);
  vstore(&result_64ks[2*16], b4);

  vsub(b5, b5, rd0[1]);
  vsub(b5, b5, rd1[1]);
  vstore(&result_64ks[3*16], b5);

  vsub(b6, b6, rd0[2]);
  vsub(b6, b6, rd1[2]);
  vstore(&result_64ks[4*16], b6);

  vsub(b7, b7, rd0[3]);
  vsub(b7, b7, rd1[3]);
  vstore(&result_64ks[5*16], b7);

  vstore(&result_64ks[0*16], rd0[0]);
  vstore(&result_64ks[1*16], rd0[1]);
  vstore(&result_64ks[6*16], rd1[2]);
  vstore(&result_64ks[7*16], rd1[3]);
}

/*
 * batch_64coefficient_multiplications
 * a0, a1, ... a6: In memory [64]
 * b0, b1, ... b6: In memory [64]
 * r...ult_final6: In memory [128]
 */

void batch_64coefficient_multiplications(
    uint16_t *restrict a0, uint16_t *restrict b0, uint16_t *restrict result_final0, 
    uint16_t *restrict a1, uint16_t *restrict b1, uint16_t *restrict result_final1, 
    uint16_t *restrict a2, uint16_t *restrict b2, uint16_t *restrict result_final2, 
    uint16_t *restrict a3, uint16_t *restrict b3, uint16_t *restrict result_final3,
    uint16_t *restrict a4, uint16_t *restrict b4, uint16_t *restrict result_final4, 
    uint16_t *restrict a5, uint16_t *restrict b5, uint16_t *restrict result_final5, 
    uint16_t *restrict a6, uint16_t *restrict b6, uint16_t *restrict result_final6)
{
  uint16x8x2_t a_lu_temp[2], b_lu_temp[2];
  uint16x8x2_t a_tmp[4], b_tmp[4];
  
  uint16_t  result_d0 [16*16], 
            result_d1 [16*16], 
            result_d01[16*16];

  uint16_t i;

  // KS splitting of 1st 64-coeff multiplication
  for (i = 0; i < 2; i++)
  {
    vload(a_tmp[i], &a0[i*16]);
    vload(a_tmp[2 + i], &a0[(2+i)*16]);
    vadd(a_lu_temp[i], a_tmp[i], a_tmp[2+i]);

    vload(b_tmp[i], &b0[i*16]);
    vload(b_tmp[2 + i], &b0[(2+i)*16]);
    vadd(b_lu_temp[i], b_tmp[i], b_tmp[2 + i]);
  }

  karatsuba32_fork_avx_new(&a_tmp[0], &b_tmp[0], 0); // a,b: 0, 1, 2
  karatsuba32_fork_avx_new(&a_tmp[2], &b_tmp[2], 3); // a,b: 3, 4, 5
  karatsuba32_fork_avx_new(a_lu_temp, b_lu_temp, 6); // a,b: 6, 7, 8

  // KS splitting of 2nd 64-coeff multiplication
  for (i = 0; i < 2; i++) {
    vload(a_tmp[i], &a1[i*16]);
    vload(a_tmp[2 + i], &a1[(2+i)*16]);
    vadd(a_lu_temp[i], a_tmp[i], a_tmp[2+i]);

    vload(b_tmp[i], &b1[i*16]);
    vload(b_tmp[2 + i], &b1[(2+i)*16]);
    vadd(b_lu_temp[i], b_tmp[i], b_tmp[2 + i]);
  }
  
  karatsuba32_fork_avx_new(&a_tmp[0], &b_tmp[0], 9); // a,b: 9, 10, 11
  karatsuba32_fork_avx_new(&a_tmp[2], &b_tmp[2], 12); // a,b: 12, 13, 14
  // Partial: loads only one of the three elements in the bucket
  karatsuba32_fork_avx_new1(a_lu_temp, b_lu_temp, 15);  // a,b: 15; a,b_extra: 0, 1

  // Compute 16 school-book multiplications in a batch.
  transpose(a);
  transpose(b);
  schoolbook_neon_new(c_avx, a, b);

  transpose(&c_avx[0]);
  transpose(&c_avx[256]);

  // store the partial multiplication result.
  vcopy(&c_avx_extra[16*0], &c_avx[15*16]);
  vcopy(&c_avx_extra[16*1], &c_avx[31*16]);

  karatsuba32_join_avx_new(result_d0, 0); // 0, 1, 2, 3
  karatsuba32_join_avx_new(result_d1, 3); // 3, 4, 5, 6
  karatsuba32_join_avx_new(result_d01, 6); // 6, 7 , 8, 9

  // Final result of 1st 64-coeff multiplication
  join_32coefficient_results(result_d0, result_d1, result_d01, result_final0);

  karatsuba32_join_avx_new(result_d0, 9);
  karatsuba32_join_avx_new(result_d1, 12);

  // Fork 2 parts of previous operands
  karatsuba32_fork_avx_partial(a_lu_temp, b_lu_temp, 0); // a,b: 0, 1

  // Fork multiplication of a2*b2
  for (i = 0; i < 2; i++) {
    vload(a_tmp[i], &a2[i*16]);
    vload(a_tmp[2 + i], &a2[(2+i)*16]);
    vadd(a_lu_temp[i], a_tmp[i], a_tmp[2+i]);

    vload(b_tmp[i], &b2[i*16]);
    vload(b_tmp[2 + i], &b2[(2+i)*16]);
    vadd(b_lu_temp[i], b_tmp[i], b_tmp[2 + i]);
  }
  
  karatsuba32_fork_avx_new(&a_tmp[0], &b_tmp[0], 2); // a,b: 2, 3, 4
  karatsuba32_fork_avx_new(&a_tmp[2], &b_tmp[2], 5); // a,b: 5, 6, 7
  karatsuba32_fork_avx_new(a_lu_temp, b_lu_temp, 8); // a,b: 8, 9, 10

  // Fork multiplication of a3*b3
  for (i = 0; i < 2; i++) {
    vload(a_tmp[i], &a3[i*16]);
    vload(a_tmp[2 + i], &a3[(2+i)*16]);
    vadd(a_lu_temp[i], a_tmp[i], a_tmp[2+i]);

    vload(b_tmp[i], &b3[i*16]);
    vload(b_tmp[2 + i], &b3[(2+i)*16]);
    vadd(b_lu_temp[i], b_tmp[i], b_tmp[2 + i]);
  }
  

  // Partial: loads only two of the three elements in the bucket
  karatsuba32_fork_avx_new(&a_tmp[0], &b_tmp[0], 11); // a,b: 11, 12, 13
  karatsuba32_fork_avx_new(&a_tmp[2], &b_tmp[2], 14); // a,b: 14, 15, 16

  transpose(a);
  transpose(b);
  schoolbook_neon_new(c_avx, a, b);

  transpose(&c_avx[16*0]);
  transpose(&c_avx[256]);

  karatsuba32_join_avx_partial(result_d01, 0); // Combine results of this computation with previous computation
  // Final result of 2nd 64-coeff multiplication
  join_32coefficient_results(result_d0, result_d1, result_d01, result_final1);

  // store the partial multiplication result. they will be combined after next batch multiplication
  vcopy(&c_avx_extra[16*0], &c_avx[16*(14)]);
  vcopy(&c_avx_extra[16*1], &c_avx[16*(14 + 16)]);
  vcopy(&c_avx_extra[16*2], &c_avx[16*(15)]);
  vcopy(&c_avx_extra[16*3], &c_avx[16*(15 + 16)]);

  karatsuba32_join_avx_new(result_d0, 2);
  karatsuba32_join_avx_new(result_d1, 5);
  karatsuba32_join_avx_new(result_d01, 8);

  // Final result of 3rd 64-coeff multiplication
  join_32coefficient_results(result_d0, result_d1, result_d01, result_final2);

  // Join d0 of 4th 64-coeff multiplication
  karatsuba32_join_avx_new(result_d0, 11);

  // Fork 1 part of previous operands
  karatsuba32_fork_avx_partial1(&a_tmp[2], &b_tmp[2], 0); // a,b: 0
  karatsuba32_fork_avx_new(a_lu_temp, b_lu_temp, 1); // a,b: 1, 2, 3

  // Fork multiplication of a4*b4
  for (i = 0; i < 2; i++) {
    vload(a_tmp[i], &a4[i*16]);
    vload(a_tmp[2 + i], &a4[(2+i)*16]);
    vadd(a_lu_temp[i], a_tmp[i], a_tmp[2+i]);

    vload(b_tmp[i], &b4[i*16]);
    vload(b_tmp[2 + i], &b4[(2+i)*16]);
    vadd(b_lu_temp[i], b_tmp[i], b_tmp[2 + i]);
  }
  
  karatsuba32_fork_avx_new(&a_tmp[0], &b_tmp[0], 4); // a,b: 4, 5, 6
  karatsuba32_fork_avx_new(&a_tmp[2], &b_tmp[2], 7); // a,b: 7, 8, 9
  karatsuba32_fork_avx_new(a_lu_temp, b_lu_temp, 10); // a,b: 10, 11, 12

  // Fork multiplication of a5*b5
  for (i = 0; i < 2; i++) {
    vload(a_tmp[i], &a5[i*16]);
    vload(a_tmp[2 + i], &a5[(2+i)*16]);
    vadd(a_lu_temp[i], a_tmp[i], a_tmp[2+i]);

    vload(b_tmp[i], &b5[i*16]);
    vload(b_tmp[2 + i], &b5[(2+i)*16]);
    vadd(b_lu_temp[i], b_tmp[i], b_tmp[2 + i]);
  }
  
  
  karatsuba32_fork_avx_new(&a_tmp[0], &b_tmp[0], 13); // a,b: 13, 14, 15

  transpose(a);
  transpose(b);
  schoolbook_neon_new(c_avx, a, b);

  transpose(&c_avx[16*0]);
  transpose(&c_avx[256]);

  karatsuba32_join_avx_partial2(result_d1, 0);
  karatsuba32_join_avx_new(result_d01, 1);

  // Final result of 4th 64-coeff multiplication
  join_32coefficient_results(result_d0, result_d1, result_d01, result_final3);

  karatsuba32_join_avx_new(result_d0, 4);
  karatsuba32_join_avx_new(result_d1, 7);
  karatsuba32_join_avx_new(result_d01, 10);

  // Final result of 5th 64-coeff multiplication
  join_32coefficient_results(result_d0, result_d1, result_d01, result_final4);

  karatsuba32_join_avx_new(result_d0, 13);

  // Fork remaining 2 parts of a5*b5
  karatsuba32_fork_avx_new(&a_tmp[2], &b_tmp[2], 0); // a,b: 0, 1, 2
  karatsuba32_fork_avx_new(a_lu_temp, b_lu_temp, 3); // a,b: 3, 4, 5

  // Fork multiplication of a6*b6
  for (i = 0; i < 2; i++) {
    vload(a_tmp[i], &a6[i*16]);
    vload(a_tmp[2 + i], &a6[(2+i)*16]);
    vadd(a_lu_temp[i], a_tmp[i], a_tmp[2+i]);

    vload(b_tmp[i], &b6[i*16]);
    vload(b_tmp[2 + i], &b6[(2+i)*16]);
    vadd(b_lu_temp[i], b_tmp[i], b_tmp[2 + i]);
  }
  
  karatsuba32_fork_avx_new(&a_tmp[0], &b_tmp[0], 6); // a,b: 6, 7, 8
  karatsuba32_fork_avx_new(&a_tmp[2], &b_tmp[2], 9); // a,b: 9, 10, 11
  karatsuba32_fork_avx_new(a_lu_temp, b_lu_temp, 12); // a,b: 12, 13, 14

  transpose(a);
  transpose(b);
  schoolbook_neon_new(c_avx, a, b);

  transpose(&c_avx[16*0]);
  transpose(&c_avx[256]);

  karatsuba32_join_avx_new(result_d1, 0);
  karatsuba32_join_avx_new(result_d01, 3);

  // Final result of 6th 64-coeff multiplication
  join_32coefficient_results(result_d0, result_d1, result_d01, result_final5);

  karatsuba32_join_avx_new(result_d0, 6);
  karatsuba32_join_avx_new(result_d1, 9);
  karatsuba32_join_avx_new(result_d01, 12);

  // Final result of 6th 64-coeff multiplication
  join_32coefficient_results(result_d0, result_d1, result_d01, result_final6);
}
