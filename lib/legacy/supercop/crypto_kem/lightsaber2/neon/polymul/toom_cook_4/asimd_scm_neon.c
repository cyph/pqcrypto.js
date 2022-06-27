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

// c = aa * bb
#define sb_vmul(c, aa, bb) c = vmulq_u16(aa, bb);

// c += aa*bb
#define sb_vmla(c, aa, bb) c = vmlaq_u16(c, aa, bb);

// load c <= a
#define sb_vload(c, a) c = vld1q_u16(a);

// store c <= a
#define sb_vstore(c, a) vst1q_u16(c, a);

// c = aa ^ bb
#define sb_vxor(c, aa, bb) c = veorq_u16(aa, bb);

/*
 * schoolbook_neon_new
 * Assume aa, bb are already loaded
 */
void schoolbook_neon_new(uint16_t *restrict c_mem, uint16_t *restrict a_mem, uint16_t *restrict b_mem) {
  uint16x8_t tmp, aa[16], bb[16];
  uint16_t i;
  uint16_t pad = 0;
  for (i = 0; i < 2; i++) {
    sb_vload(aa[0], &a_mem[0 * 16 + pad]);
    sb_vload(bb[0], &b_mem[0 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[0]);
    vst1q_u16(&c_mem[0 + pad], tmp);
    //----
    sb_vload(aa[1], &a_mem[1 * 16 + pad]);
    sb_vload(bb[1], &b_mem[1 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[1]);
    sb_vmla(tmp, aa[1], bb[0]);
    vst1q_u16(&c_mem[16 + pad], tmp);
    //----
    sb_vload(aa[2], &a_mem[2 * 16 + pad]);
    sb_vload(bb[2], &b_mem[2 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[2]);
    sb_vmla(tmp, aa[1], bb[1]);
    sb_vmla(tmp, aa[2], bb[0]);
    vst1q_u16(&c_mem[32 + pad], tmp);
    //----
    sb_vload(aa[3], &a_mem[3 * 16 + pad]);
    sb_vload(bb[3], &b_mem[3 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[3]);
    sb_vmla(tmp, aa[1], bb[2]);
    sb_vmla(tmp, aa[2], bb[1]);
    sb_vmla(tmp, aa[3], bb[0]);
    vst1q_u16(&c_mem[48 + pad], tmp);
    //----
    sb_vload(aa[4], &a_mem[4 * 16 + pad]);
    sb_vload(bb[4], &b_mem[4 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[4]);
    sb_vmla(tmp, aa[1], bb[3]);
    sb_vmla(tmp, aa[2], bb[2]);
    sb_vmla(tmp, aa[3], bb[1]);
    sb_vmla(tmp, aa[4], bb[0]);
    vst1q_u16(&c_mem[64 + pad], tmp);
    //----
    sb_vload(aa[5], &a_mem[5 * 16 + pad]);
    sb_vload(bb[5], &b_mem[5 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[5]);
    sb_vmla(tmp, aa[1], bb[4]);
    sb_vmla(tmp, aa[2], bb[3]);
    sb_vmla(tmp, aa[3], bb[2]);
    sb_vmla(tmp, aa[4], bb[1]);
    sb_vmla(tmp, aa[5], bb[0]);
    vst1q_u16(&c_mem[80 + pad], tmp);
    //----
    sb_vload(aa[6], &a_mem[6 * 16 + pad]);
    sb_vload(bb[6], &b_mem[6 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[6]);
    sb_vmla(tmp, aa[1], bb[5]);
    sb_vmla(tmp, aa[2], bb[4]);
    sb_vmla(tmp, aa[3], bb[3]);
    sb_vmla(tmp, aa[4], bb[2]);
    sb_vmla(tmp, aa[5], bb[1]);
    sb_vmla(tmp, aa[6], bb[0]);
    vst1q_u16(&c_mem[96 + pad], tmp);
    //----
    sb_vload(aa[7], &a_mem[7 * 16 + pad]);
    sb_vload(bb[7], &b_mem[7 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[7]);
    sb_vmla(tmp, aa[1], bb[6]);
    sb_vmla(tmp, aa[2], bb[5]);
    sb_vmla(tmp, aa[3], bb[4]);
    sb_vmla(tmp, aa[4], bb[3]);
    sb_vmla(tmp, aa[5], bb[2]);
    sb_vmla(tmp, aa[6], bb[1]);
    sb_vmla(tmp, aa[7], bb[0]);
    vst1q_u16(&c_mem[112 + pad], tmp);
    //----
    sb_vload(aa[8], &a_mem[8 * 16 + pad]);
    sb_vload(bb[8], &b_mem[8 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[8]);
    sb_vmla(tmp, aa[1], bb[7]);
    sb_vmla(tmp, aa[2], bb[6]);
    sb_vmla(tmp, aa[3], bb[5]);
    sb_vmla(tmp, aa[4], bb[4]);
    sb_vmla(tmp, aa[5], bb[3]);
    sb_vmla(tmp, aa[6], bb[2]);
    sb_vmla(tmp, aa[7], bb[1]);
    sb_vmla(tmp, aa[8], bb[0]);
    vst1q_u16(&c_mem[128 + pad], tmp);
    //----
    sb_vload(aa[9], &a_mem[9 * 16 + pad]);
    sb_vload(bb[9], &b_mem[9 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[9]);
    sb_vmla(tmp, aa[1], bb[8]);
    sb_vmla(tmp, aa[2], bb[7]);
    sb_vmla(tmp, aa[3], bb[6]);
    sb_vmla(tmp, aa[4], bb[5]);
    sb_vmla(tmp, aa[5], bb[4]);
    sb_vmla(tmp, aa[6], bb[3]);
    sb_vmla(tmp, aa[7], bb[2]);
    sb_vmla(tmp, aa[8], bb[1]);
    sb_vmla(tmp, aa[9], bb[0]);
    vst1q_u16(&c_mem[144 + pad], tmp);
    //----
    sb_vload(aa[10], &a_mem[10 * 16 + pad]);
    sb_vload(bb[10], &b_mem[10 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[10]);
    sb_vmla(tmp, aa[1], bb[9]);
    sb_vmla(tmp, aa[2], bb[8]);
    sb_vmla(tmp, aa[3], bb[7]);
    sb_vmla(tmp, aa[4], bb[6]);
    sb_vmla(tmp, aa[5], bb[5]);
    sb_vmla(tmp, aa[6], bb[4]);
    sb_vmla(tmp, aa[7], bb[3]);
    sb_vmla(tmp, aa[8], bb[2]);
    sb_vmla(tmp, aa[9], bb[1]);
    sb_vmla(tmp, aa[10], bb[0]);
    vst1q_u16(&c_mem[160 + pad], tmp);
    //----
    sb_vload(aa[11], &a_mem[11 * 16 + pad]);
    sb_vload(bb[11], &b_mem[11 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[11]);
    sb_vmla(tmp, aa[1], bb[10]);
    sb_vmla(tmp, aa[2], bb[9]);
    sb_vmla(tmp, aa[3], bb[8]);
    sb_vmla(tmp, aa[4], bb[7]);
    sb_vmla(tmp, aa[5], bb[6]);
    sb_vmla(tmp, aa[6], bb[5]);
    sb_vmla(tmp, aa[7], bb[4]);
    sb_vmla(tmp, aa[8], bb[3]);
    sb_vmla(tmp, aa[9], bb[2]);
    sb_vmla(tmp, aa[10], bb[1]);
    sb_vmla(tmp, aa[11], bb[0]);
    vst1q_u16(&c_mem[176 + pad], tmp);
    //----
    sb_vload(aa[12], &a_mem[12 * 16 + pad]);
    sb_vload(bb[12], &b_mem[12 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[12]);
    sb_vmla(tmp, aa[1], bb[11]);
    sb_vmla(tmp, aa[2], bb[10]);
    sb_vmla(tmp, aa[3], bb[9]);
    sb_vmla(tmp, aa[4], bb[8]);
    sb_vmla(tmp, aa[5], bb[7]);
    sb_vmla(tmp, aa[6], bb[6]);
    sb_vmla(tmp, aa[7], bb[5]);
    sb_vmla(tmp, aa[8], bb[4]);
    sb_vmla(tmp, aa[9], bb[3]);
    sb_vmla(tmp, aa[10], bb[2]);
    sb_vmla(tmp, aa[11], bb[1]);
    sb_vmla(tmp, aa[12], bb[0]);
    vst1q_u16(&c_mem[192 + pad], tmp);
    //----
    sb_vload(aa[13], &a_mem[13 * 16 + pad]);
    sb_vload(bb[13], &b_mem[13 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[13]);
    sb_vmla(tmp, aa[1], bb[12]);
    sb_vmla(tmp, aa[2], bb[11]);
    sb_vmla(tmp, aa[3], bb[10]);
    sb_vmla(tmp, aa[4], bb[9]);
    sb_vmla(tmp, aa[5], bb[8]);
    sb_vmla(tmp, aa[6], bb[7]);
    sb_vmla(tmp, aa[7], bb[6]);
    sb_vmla(tmp, aa[8], bb[5]);
    sb_vmla(tmp, aa[9], bb[4]);
    sb_vmla(tmp, aa[10], bb[3]);
    sb_vmla(tmp, aa[11], bb[2]);
    sb_vmla(tmp, aa[12], bb[1]);
    sb_vmla(tmp, aa[13], bb[0]);
    vst1q_u16(&c_mem[208 + pad], tmp);
    //----
    sb_vload(aa[14], &a_mem[14 * 16 + pad]);
    sb_vload(bb[14], &b_mem[14 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[14]);
    sb_vmla(tmp, aa[1], bb[13]);
    sb_vmla(tmp, aa[2], bb[12]);
    sb_vmla(tmp, aa[3], bb[11]);
    sb_vmla(tmp, aa[4], bb[10]);
    sb_vmla(tmp, aa[5], bb[9]);
    sb_vmla(tmp, aa[6], bb[8]);
    sb_vmla(tmp, aa[7], bb[7]);
    sb_vmla(tmp, aa[8], bb[6]);
    sb_vmla(tmp, aa[9], bb[5]);
    sb_vmla(tmp, aa[10], bb[4]);
    sb_vmla(tmp, aa[11], bb[3]);
    sb_vmla(tmp, aa[12], bb[2]);
    sb_vmla(tmp, aa[13], bb[1]);
    sb_vmla(tmp, aa[14], bb[0]);
    vst1q_u16(&c_mem[224 + pad], tmp);
    //----
    sb_vload(bb[15], &b_mem[15 * 16 + pad]);

    sb_vmul(tmp, aa[0], bb[15]);
    sb_vmla(tmp, aa[1], bb[14]);
    sb_vmla(tmp, aa[2], bb[13]);
    sb_vmla(tmp, aa[3], bb[12]);
    sb_vmla(tmp, aa[4], bb[11]);
    sb_vmla(tmp, aa[5], bb[10]);
    sb_vmla(tmp, aa[6], bb[9]);
    sb_vmla(tmp, aa[7], bb[8]);
    sb_vmla(tmp, aa[8], bb[7]);
    sb_vmla(tmp, aa[9], bb[6]);
    sb_vmla(tmp, aa[10], bb[5]);
    sb_vmla(tmp, aa[11], bb[4]);
    sb_vmla(tmp, aa[12], bb[3]);
    sb_vmla(tmp, aa[13], bb[2]);
    sb_vmla(tmp, aa[14], bb[1]);

    sb_vload(aa[15], &a_mem[15 * 16 + pad]);

    sb_vmla(tmp, aa[15], bb[0]);
    vst1q_u16(&c_mem[240 + pad], tmp);
    //----
    //=======
    // ci =  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} 16
    // ai =  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} 16
    // bi =  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} 16
    // ----------------PART 2----------------
    sb_vmul(tmp, aa[1], bb[15]);
    sb_vmla(tmp, aa[2], bb[14]);
    sb_vmla(tmp, aa[3], bb[13]);
    sb_vmla(tmp, aa[4], bb[12]);
    sb_vmla(tmp, aa[5], bb[11]);
    sb_vmla(tmp, aa[6], bb[10]);
    sb_vmla(tmp, aa[7], bb[9]);
    sb_vmla(tmp, aa[8], bb[8]);
    sb_vmla(tmp, aa[9], bb[7]);
    sb_vmla(tmp, aa[10], bb[6]);
    sb_vmla(tmp, aa[11], bb[5]);
    sb_vmla(tmp, aa[12], bb[4]);
    sb_vmla(tmp, aa[13], bb[3]);
    sb_vmla(tmp, aa[14], bb[2]);
    sb_vmla(tmp, aa[15], bb[1]);
    vst1q_u16(&c_mem[256 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[2], bb[15]);
    sb_vmla(tmp, aa[3], bb[14]);
    sb_vmla(tmp, aa[4], bb[13]);
    sb_vmla(tmp, aa[5], bb[12]);
    sb_vmla(tmp, aa[6], bb[11]);
    sb_vmla(tmp, aa[7], bb[10]);
    sb_vmla(tmp, aa[8], bb[9]);
    sb_vmla(tmp, aa[9], bb[8]);
    sb_vmla(tmp, aa[10], bb[7]);
    sb_vmla(tmp, aa[11], bb[6]);
    sb_vmla(tmp, aa[12], bb[5]);
    sb_vmla(tmp, aa[13], bb[4]);
    sb_vmla(tmp, aa[14], bb[3]);
    sb_vmla(tmp, aa[15], bb[2]);
    vst1q_u16(&c_mem[272 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[3], bb[15]);
    sb_vmla(tmp, aa[4], bb[14]);
    sb_vmla(tmp, aa[5], bb[13]);
    sb_vmla(tmp, aa[6], bb[12]);
    sb_vmla(tmp, aa[7], bb[11]);
    sb_vmla(tmp, aa[8], bb[10]);
    sb_vmla(tmp, aa[9], bb[9]);
    sb_vmla(tmp, aa[10], bb[8]);
    sb_vmla(tmp, aa[11], bb[7]);
    sb_vmla(tmp, aa[12], bb[6]);
    sb_vmla(tmp, aa[13], bb[5]);
    sb_vmla(tmp, aa[14], bb[4]);
    sb_vmla(tmp, aa[15], bb[3]);
    vst1q_u16(&c_mem[288 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[4], bb[15]);
    sb_vmla(tmp, aa[5], bb[14]);
    sb_vmla(tmp, aa[6], bb[13]);
    sb_vmla(tmp, aa[7], bb[12]);
    sb_vmla(tmp, aa[8], bb[11]);
    sb_vmla(tmp, aa[9], bb[10]);
    sb_vmla(tmp, aa[10], bb[9]);
    sb_vmla(tmp, aa[11], bb[8]);
    sb_vmla(tmp, aa[12], bb[7]);
    sb_vmla(tmp, aa[13], bb[6]);
    sb_vmla(tmp, aa[14], bb[5]);
    sb_vmla(tmp, aa[15], bb[4]);
    vst1q_u16(&c_mem[304 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[5], bb[15]);
    sb_vmla(tmp, aa[6], bb[14]);
    sb_vmla(tmp, aa[7], bb[13]);
    sb_vmla(tmp, aa[8], bb[12]);
    sb_vmla(tmp, aa[9], bb[11]);
    sb_vmla(tmp, aa[10], bb[10]);
    sb_vmla(tmp, aa[11], bb[9]);
    sb_vmla(tmp, aa[12], bb[8]);
    sb_vmla(tmp, aa[13], bb[7]);
    sb_vmla(tmp, aa[14], bb[6]);
    sb_vmla(tmp, aa[15], bb[5]);
    vst1q_u16(&c_mem[320 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[6], bb[15]);
    sb_vmla(tmp, aa[7], bb[14]);
    sb_vmla(tmp, aa[8], bb[13]);
    sb_vmla(tmp, aa[9], bb[12]);
    sb_vmla(tmp, aa[10], bb[11]);
    sb_vmla(tmp, aa[11], bb[10]);
    sb_vmla(tmp, aa[12], bb[9]);
    sb_vmla(tmp, aa[13], bb[8]);
    sb_vmla(tmp, aa[14], bb[7]);
    sb_vmla(tmp, aa[15], bb[6]);
    vst1q_u16(&c_mem[336 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[7], bb[15]);
    sb_vmla(tmp, aa[8], bb[14]);
    sb_vmla(tmp, aa[9], bb[13]);
    sb_vmla(tmp, aa[10], bb[12]);
    sb_vmla(tmp, aa[11], bb[11]);
    sb_vmla(tmp, aa[12], bb[10]);
    sb_vmla(tmp, aa[13], bb[9]);
    sb_vmla(tmp, aa[14], bb[8]);
    sb_vmla(tmp, aa[15], bb[7]);
    vst1q_u16(&c_mem[352 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[8], bb[15]);
    sb_vmla(tmp, aa[9], bb[14]);
    sb_vmla(tmp, aa[10], bb[13]);
    sb_vmla(tmp, aa[11], bb[12]);
    sb_vmla(tmp, aa[12], bb[11]);
    sb_vmla(tmp, aa[13], bb[10]);
    sb_vmla(tmp, aa[14], bb[9]);
    sb_vmla(tmp, aa[15], bb[8]);
    vst1q_u16(&c_mem[368 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[9], bb[15]);
    sb_vmla(tmp, aa[10], bb[14]);
    sb_vmla(tmp, aa[11], bb[13]);
    sb_vmla(tmp, aa[12], bb[12]);
    sb_vmla(tmp, aa[13], bb[11]);
    sb_vmla(tmp, aa[14], bb[10]);
    sb_vmla(tmp, aa[15], bb[9]);
    vst1q_u16(&c_mem[384 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[10], bb[15]);
    sb_vmla(tmp, aa[11], bb[14]);
    sb_vmla(tmp, aa[12], bb[13]);
    sb_vmla(tmp, aa[13], bb[12]);
    sb_vmla(tmp, aa[14], bb[11]);
    sb_vmla(tmp, aa[15], bb[10]);
    vst1q_u16(&c_mem[400 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[11], bb[15]);
    sb_vmla(tmp, aa[12], bb[14]);
    sb_vmla(tmp, aa[13], bb[13]);
    sb_vmla(tmp, aa[14], bb[12]);
    sb_vmla(tmp, aa[15], bb[11]);
    vst1q_u16(&c_mem[416 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[12], bb[15]);
    sb_vmla(tmp, aa[13], bb[14]);
    sb_vmla(tmp, aa[14], bb[13]);
    sb_vmla(tmp, aa[15], bb[12]);
    vst1q_u16(&c_mem[432 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[13], bb[15]);
    sb_vmla(tmp, aa[14], bb[14]);
    sb_vmla(tmp, aa[15], bb[13]);
    vst1q_u16(&c_mem[448 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[14], bb[15]);
    sb_vmla(tmp, aa[15], bb[14]);
    vst1q_u16(&c_mem[464 + pad], tmp);
    //-----
    sb_vmul(tmp, aa[15], bb[15]);
    vst1q_u16(&c_mem[480 + pad], tmp);
    //-----

    pad = 8;
  }
  sb_vxor(tmp, tmp, tmp);
  vst1q_u16(&c_mem[31 * 16], tmp);
  vst1q_u16(&c_mem[31 * 16 + 8], tmp);
}
