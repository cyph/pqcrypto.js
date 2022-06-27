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

/*
 * Improved in-place tranpose, minimal spill to memory.
 * Input: Memory uint16_t *M
 * Output: Memory uint16_t *M
 */
void transpose(uint16_t *M) {

  uint16x8_t y0, y1, y2, y3, y4, y5, y6, y7, y8;
  uint16x8_t y9, y10, y11, y12, y13, y14, y15, y16, y17;
  uint16x8_t y18, y19, y20, y21, y22, y23, y24, y25, y26;
  uint16x8_t y27, y28, y29, y30, y31, y32, y33, y34, y35;
  // 16x16: LD A1
  y0 = vld1q_u16(M + 0);
  y1 = vld1q_u16(M + 16);
  y2 = vld1q_u16(M + 32);
  y3 = vld1q_u16(M + 48);
  y4 = vld1q_u16(M + 64);
  y5 = vld1q_u16(M + 80);
  y6 = vld1q_u16(M + 96);
  y7 = vld1q_u16(M + 112);
  // Transpose 8x8
  y16 = vtrn1q_u16(y0, y1);
  y17 = vtrn2q_u16(y0, y1);
  y18 = vtrn1q_u16(y2, y3);
  y19 = vtrn2q_u16(y2, y3);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y17);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y17);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y18, (uint32x4_t)y19);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y18, (uint32x4_t)y19);
  y8 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y10 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y9 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y11 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

  y16 = vtrn1q_u16(y4, y5);
  y17 = vtrn2q_u16(y4, y5);
  y18 = vtrn1q_u16(y6, y7);
  y19 = vtrn2q_u16(y6, y7);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y17);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y17);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y18, (uint32x4_t)y19);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y18, (uint32x4_t)y19);
  y12 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y14 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y13 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y15 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

  y16 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y17 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y18 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y19 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y20 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y21 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y22 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  y23 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  // 16x16: STR A1
  vst1q_u16(M + 0, y16);
  vst1q_u16(M + 16, y18);
  vst1q_u16(M + 32, y20);
  vst1q_u16(M + 48, y22);
  vst1q_u16(M + 64, y17);
  vst1q_u16(M + 80, y19);
  vst1q_u16(M + 96, y21);
  vst1q_u16(M + 112, y23);
  // 16x16: LD A4
  y0 = vld1q_u16(M + 136);
  y1 = vld1q_u16(M + 152);
  y2 = vld1q_u16(M + 168);
  y3 = vld1q_u16(M + 184);
  y4 = vld1q_u16(M + 200);
  y5 = vld1q_u16(M + 216);
  y6 = vld1q_u16(M + 232);
  y7 = vld1q_u16(M + 248);
  // Transpose 8x8
  y16 = vtrn1q_u16(y0, y1);
  y18 = vtrn2q_u16(y0, y1);
  y20 = vtrn1q_u16(y2, y3);
  y22 = vtrn2q_u16(y2, y3);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y18);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y18);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y20, (uint32x4_t)y22);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y20, (uint32x4_t)y22);
  y8 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y10 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y9 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y11 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

  y16 = vtrn1q_u16(y4, y5);
  y18 = vtrn2q_u16(y4, y5);
  y20 = vtrn1q_u16(y6, y7);
  y22 = vtrn2q_u16(y6, y7);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y18);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y18);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y20, (uint32x4_t)y22);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y20, (uint32x4_t)y22);
  y12 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y14 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y13 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y15 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

  y16 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y18 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y20 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y22 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y17 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y19 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y21 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  y23 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  // 16x16: STR A4
  vst1q_u16(M + 136, y16);
  vst1q_u16(M + 152, y20);
  vst1q_u16(M + 168, y17);
  vst1q_u16(M + 184, y21);
  vst1q_u16(M + 200, y18);
  vst1q_u16(M + 216, y22);
  vst1q_u16(M + 232, y19);
  vst1q_u16(M + 248, y23);
  // 16x16: LD A2
  y0 = vld1q_u16(M + 8);
  y1 = vld1q_u16(M + 24);
  y2 = vld1q_u16(M + 40);
  y3 = vld1q_u16(M + 56);
  y4 = vld1q_u16(M + 72);
  y5 = vld1q_u16(M + 88);
  y6 = vld1q_u16(M + 104);
  y7 = vld1q_u16(M + 120);
  // Transpose 8x8
  y16 = vtrn1q_u16(y0, y1);
  y20 = vtrn2q_u16(y0, y1);
  y17 = vtrn1q_u16(y2, y3);
  y21 = vtrn2q_u16(y2, y3);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
  y8 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y10 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y9 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y11 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

  y16 = vtrn1q_u16(y4, y5);
  y20 = vtrn2q_u16(y4, y5);
  y17 = vtrn1q_u16(y6, y7);
  y21 = vtrn2q_u16(y6, y7);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
  y12 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y14 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y13 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y15 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

  y16 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y20 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y17 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y21 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y18 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y22 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y19 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  y23 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  // 16x16: LD A3
  y0 = vld1q_u16(M + 128);
  y1 = vld1q_u16(M + 144);
  y2 = vld1q_u16(M + 160);
  y3 = vld1q_u16(M + 176);
  y4 = vld1q_u16(M + 192);
  y5 = vld1q_u16(M + 208);
  y6 = vld1q_u16(M + 224);
  y7 = vld1q_u16(M + 240);
  // Transpose 8x8
  y28 = vtrn1q_u16(y0, y1);
  y29 = vtrn2q_u16(y0, y1);
  y30 = vtrn1q_u16(y2, y3);
  y31 = vtrn2q_u16(y2, y3);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y28, (uint32x4_t)y29);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y28, (uint32x4_t)y29);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y30, (uint32x4_t)y31);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y30, (uint32x4_t)y31);
  y8 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y10 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y9 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y11 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

  y28 = vtrn1q_u16(y4, y5);
  y29 = vtrn2q_u16(y4, y5);
  y30 = vtrn1q_u16(y6, y7);
  y31 = vtrn2q_u16(y6, y7);
  y24 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y28, (uint32x4_t)y29);
  y25 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y28, (uint32x4_t)y29);
  y26 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y30, (uint32x4_t)y31);
  y27 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y30, (uint32x4_t)y31);
  y12 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y14 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
  y13 = (uint16x8_t) vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  y15 = (uint16x8_t) vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);
  
  y28 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y29 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
  y30 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y31 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
  y32 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y33 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
  y34 = (uint16x8_t) vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  y35 = (uint16x8_t) vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);
  // 16x16: STR A2<-A3
  vst1q_u16(M + 8, y28);
  vst1q_u16(M + 24, y30);
  vst1q_u16(M + 40, y32);
  vst1q_u16(M + 56, y34);
  vst1q_u16(M + 72, y29);
  vst1q_u16(M + 88, y31);
  vst1q_u16(M + 104, y33);
  vst1q_u16(M + 120, y35);
  // 16x16: STR A3<-A2
  vst1q_u16(M + 128, y16);
  vst1q_u16(M + 144, y17);
  vst1q_u16(M + 160, y18);
  vst1q_u16(M + 176, y19);
  vst1q_u16(M + 192, y20);
  vst1q_u16(M + 208, y21);
  vst1q_u16(M + 224, y22);
  vst1q_u16(M + 240, y23);
}
