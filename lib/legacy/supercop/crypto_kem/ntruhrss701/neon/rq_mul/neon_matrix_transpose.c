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
#include <arm_neon.h>

#define TRANSPOSE_HALF 16 // Round up of 7*7*3/8

/*
 * Improved in-place tranpose, minimal spill to memory.
 * Input: Memory uint16_t M[16*8]
Before Transpose
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7  |  8   9  10  11  12  13  14  15
After Transpose
  0   0   0   0   0   0   0   0  |  8   8   8   8   8   8   8   8
  1   1   1   1   1   1   1   1  |  9   9   9   9   9   9   9   9
  2   2   2   2   2   2   2   2  | 10  10  10  10  10  10  10  10
  3   3   3   3   3   3   3   3  | 11  11  11  11  11  11  11  11
  4   4   4   4   4   4   4   4  | 12  12  12  12  12  12  12  12
  5   5   5   5   5   5   5   5  | 13  13  13  13  13  13  13  13
  6   6   6   6   6   6   6   6  | 14  14  14  14  14  14  14  14
  7   7   7   7   7   7   7   7  | 15  15  15  15  15  15  15  15
-----------
 */
void half_transpose_8x16(uint16_t *matrix) {
  uint16_t *M = matrix;
  uint16x8_t y0, y1, y2, y3, y4, y5, y6, y7, 
             y8, y9, y10, y11, y12, y13, y14,
             y15, y16, y17, y18, y19, y20, y21, 
             y22, y23, y24, y25, y26, y27;
  for (uint16_t i = 0; i < TRANSPOSE_HALF; i++) {
    // 16x16: LD A1
    y0 = vld1q_u16(M + 16*0);
    y1 = vld1q_u16(M + 16*1);
    y2 = vld1q_u16(M + 16*2);
    y3 = vld1q_u16(M + 16*3);
    y4 = vld1q_u16(M + 16*4);
    y5 = vld1q_u16(M + 16*5);
    y6 = vld1q_u16(M + 16*6);
    y7 = vld1q_u16(M + 16*7);
    // Transpose 8x8
    y16 = vtrn1q_u16(y0, y1);
    y17 = vtrn2q_u16(y0, y1);
    y18 = vtrn1q_u16(y2, y3);
    y19 = vtrn2q_u16(y2, y3);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y8 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y10 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y9 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y11 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = vtrn1q_u16(y4, y5);
    y17 = vtrn2q_u16(y4, y5);
    y18 = vtrn1q_u16(y6, y7);
    y19 = vtrn2q_u16(y6, y7);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y12 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y14 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y13 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y15 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y17 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y18 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y19 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y20 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y21 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y22 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    y23 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    // 16x16: STR A1
    vst1q_u16(M + 16*0, y16);
    vst1q_u16(M + 16*1, y18);
    vst1q_u16(M + 16*2, y20);
    vst1q_u16(M + 16*3, y22);
    vst1q_u16(M + 16*4, y17);
    vst1q_u16(M + 16*5, y19);
    vst1q_u16(M + 16*6, y21);
    vst1q_u16(M + 16*7, y23);

    // 16x16: LD A2
    y0 = vld1q_u16(M + 16*0 + 8);
    y1 = vld1q_u16(M + 16*1 + 8);
    y2 = vld1q_u16(M + 16*2 + 8);
    y3 = vld1q_u16(M + 16*3 + 8);
    y4 = vld1q_u16(M + 16*4 + 8);
    y5 = vld1q_u16(M + 16*5 + 8);
    y6 = vld1q_u16(M + 16*6 + 8);
    y7 = vld1q_u16(M + 16*7 + 8);
    // Transpose 8x8
    y16 = vtrn1q_u16(y0, y1);
    y20 = vtrn2q_u16(y0, y1);
    y17 = vtrn1q_u16(y2, y3);
    y21 = vtrn2q_u16(y2, y3);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y8 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y10 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y9 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y11 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = vtrn1q_u16(y4, y5);
    y20 = vtrn2q_u16(y4, y5);
    y17 = vtrn1q_u16(y6, y7);
    y21 = vtrn2q_u16(y6, y7);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y12 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y14 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y13 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y15 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y20 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y17 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y21 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y18 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y22 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y19 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    y23 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);

    // 16x16: STR A2<-A2
    vst1q_u16(M + 16*0 + 8, y16);
    vst1q_u16(M + 16*1 + 8, y17);
    vst1q_u16(M + 16*2 + 8, y18);
    vst1q_u16(M + 16*3 + 8, y19);
    vst1q_u16(M + 16*4 + 8, y20);
    vst1q_u16(M + 16*5 + 8, y21);
    vst1q_u16(M + 16*6 + 8, y22);
    vst1q_u16(M + 16*7 + 8, y23);

    M += 128;
  }
}


/*
 * Improved in-place tranpose, minimal spill to memory.
 * Input: Memory uint16_t M[16*8]
Input Before Transpose
  0   0   0   0   0   0   0   0  |   8   8   8   8   8   8   8   8  |  16   16   16   16   16   16   16   16  |  24   24   24   24   24   24   24   24
  1   1   1   1   1   1   1   1  |   9   9   9   9   9   9   9   9  |  17   17   17   17   17   17   17   17  |  25   25   25   25   25   25   25   25
  2   2   2   2   2   2   2   2  |  10  10  10  10  10  10  10  10  |  18   18   18   18   18   18   18   18  |  26   26   26   26   26   26   26   26
  3   3   3   3   3   3   3   3  |  11  11  11  11  11  11  11  11  |  19   19   19   19   19   19   19   19  |  27   27   27   27   27   27   27   27
  4   4   4   4   4   4   4   4  |  12  12  12  12  12  12  12  12  |  20   20   20   20   20   20   20   20  |  28   28   28   28   28   28   28   28
  5   5   5   5   5   5   5   5  |  13  13  13  13  13  13  13  13  |  21   21   21   21   21   21   21   21  |  29   29   29   29   29   29   29   29
  6   6   6   6   6   6   6   6  |  14  14  14  14  14  14  14  14  |  22   22   22   22   22   22   22   22  |  30   30   30   30   30   30   30   30
  7   7   7   7   7   7   7   7  |  15  15  15  15  15  15  15  15  |  23   23   23   23   23   23   23   23  |   x    x    x    x    x    x    x    x
Output After Transpose
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
---------------------
 */
void half_transpose_8x32(uint16_t *matrix) {
  uint16_t *M = matrix;
  uint16x8_t y0, y1, y2, y3, y4, y5, y6, y7, 
             y8, y9, y10, y11, y12, y13, y14,
             y15, y16, y17, y18, y19, y20, y21, 
             y22, y23, y24, y25, y26, y27;
  for (uint16_t i = 0; i < TRANSPOSE_HALF; i++) {
    // 16x16: LD A1
    y0 = vld1q_u16(M + 32*0);
    y1 = vld1q_u16(M + 32*1);
    y2 = vld1q_u16(M + 32*2);
    y3 = vld1q_u16(M + 32*3);
    y4 = vld1q_u16(M + 32*4);
    y5 = vld1q_u16(M + 32*5);
    y6 = vld1q_u16(M + 32*6);
    y7 = vld1q_u16(M + 32*7);
    // Transpose 8x8
    y16 = vtrn1q_u16(y0, y1);
    y17 = vtrn2q_u16(y0, y1);
    y18 = vtrn1q_u16(y2, y3);
    y19 = vtrn2q_u16(y2, y3);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y8 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y10 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y9 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y11 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = vtrn1q_u16(y4, y5);
    y17 = vtrn2q_u16(y4, y5);
    y18 = vtrn1q_u16(y6, y7);
    y19 = vtrn2q_u16(y6, y7);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y17);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y18, (uint32x4_t)y19);
    y12 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y14 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y13 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y15 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y17 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y18 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y19 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y20 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y21 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y22 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    y23 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    // 16x16: STR A1
    vst1q_u16(M + 32*0, y16);
    vst1q_u16(M + 32*1, y18);
    vst1q_u16(M + 32*2, y20);
    vst1q_u16(M + 32*3, y22);
    vst1q_u16(M + 32*4, y17);
    vst1q_u16(M + 32*5, y19);
    vst1q_u16(M + 32*6, y21);
    vst1q_u16(M + 32*7, y23);
    // -------------------
    // 16x16: LD A2
    y0 = vld1q_u16(M + 32*0 + 8);
    y1 = vld1q_u16(M + 32*1 + 8);
    y2 = vld1q_u16(M + 32*2 + 8);
    y3 = vld1q_u16(M + 32*3 + 8);
    y4 = vld1q_u16(M + 32*4 + 8);
    y5 = vld1q_u16(M + 32*5 + 8);
    y6 = vld1q_u16(M + 32*6 + 8);
    y7 = vld1q_u16(M + 32*7 + 8);
    // Transpose 8x8
    y16 = vtrn1q_u16(y0, y1);
    y20 = vtrn2q_u16(y0, y1);
    y17 = vtrn1q_u16(y2, y3);
    y21 = vtrn2q_u16(y2, y3);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y8 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y10 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y9 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y11 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = vtrn1q_u16(y4, y5);
    y20 = vtrn2q_u16(y4, y5);
    y17 = vtrn1q_u16(y6, y7);
    y21 = vtrn2q_u16(y6, y7);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y12 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y14 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y13 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y15 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y20 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y17 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y21 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y18 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y22 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y19 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    y23 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);

    // 16x16: STR A2<-A2
    vst1q_u16(M + 32*0 + 8, y16);
    vst1q_u16(M + 32*1 + 8, y17);
    vst1q_u16(M + 32*2 + 8, y18);
    vst1q_u16(M + 32*3 + 8, y19);
    vst1q_u16(M + 32*4 + 8, y20);
    vst1q_u16(M + 32*5 + 8, y21);
    vst1q_u16(M + 32*6 + 8, y22);
    vst1q_u16(M + 32*7 + 8, y23);
    // -------------------
    // 16x16: LD A3
    y0 = vld1q_u16(M + 32*0 + 16);
    y1 = vld1q_u16(M + 32*1 + 16);
    y2 = vld1q_u16(M + 32*2 + 16);
    y3 = vld1q_u16(M + 32*3 + 16);
    y4 = vld1q_u16(M + 32*4 + 16);
    y5 = vld1q_u16(M + 32*5 + 16);
    y6 = vld1q_u16(M + 32*6 + 16);
    y7 = vld1q_u16(M + 32*7 + 16);
    // Transpose 8x8
    y16 = vtrn1q_u16(y0, y1);
    y20 = vtrn2q_u16(y0, y1);
    y17 = vtrn1q_u16(y2, y3);
    y21 = vtrn2q_u16(y2, y3);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y8 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y10 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y9 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y11 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = vtrn1q_u16(y4, y5);
    y20 = vtrn2q_u16(y4, y5);
    y17 = vtrn1q_u16(y6, y7);
    y21 = vtrn2q_u16(y6, y7);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y12 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y14 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y13 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y15 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y20 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y17 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y21 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y18 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y22 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y19 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    y23 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);

    // 16x16: STR A3<-A3
    vst1q_u16(M + 32*0 + 16, y16);
    vst1q_u16(M + 32*1 + 16, y17);
    vst1q_u16(M + 32*2 + 16, y18);
    vst1q_u16(M + 32*3 + 16, y19);
    vst1q_u16(M + 32*4 + 16, y20);
    vst1q_u16(M + 32*5 + 16, y21);
    vst1q_u16(M + 32*6 + 16, y22);
    vst1q_u16(M + 32*7 + 16, y23);
    // -------------------
    // 16x16: LD A4
    y0 = vld1q_u16(M + 32*0 + 24);
    y1 = vld1q_u16(M + 32*1 + 24);
    y2 = vld1q_u16(M + 32*2 + 24);
    y3 = vld1q_u16(M + 32*3 + 24);
    y4 = vld1q_u16(M + 32*4 + 24);
    y5 = vld1q_u16(M + 32*5 + 24);
    y6 = vld1q_u16(M + 32*6 + 24);
    y7 = vld1q_u16(M + 32*7 + 24);
    // Transpose 8x8
    y16 = vtrn1q_u16(y0, y1);
    y20 = vtrn2q_u16(y0, y1);
    y17 = vtrn1q_u16(y2, y3);
    y21 = vtrn2q_u16(y2, y3);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y8 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y10 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y9 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y11 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = vtrn1q_u16(y4, y5);
    y20 = vtrn2q_u16(y4, y5);
    y17 = vtrn1q_u16(y6, y7);
    y21 = vtrn2q_u16(y6, y7);
    y24 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y25 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y16, (uint32x4_t)y20);
    y26 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y27 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y17, (uint32x4_t)y21);
    y12 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y14 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y24, (uint32x4_t)y26);
    y13 = (uint16x8_t)vtrn1q_u32((uint32x4_t)y25, (uint32x4_t)y27);
    y15 = (uint16x8_t)vtrn2q_u32((uint32x4_t)y25, (uint32x4_t)y27);

    y16 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y20 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y8, (uint64x2_t)y12);
    y17 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y21 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y10, (uint64x2_t)y14);
    y18 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y22 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y9, (uint64x2_t)y13);
    y19 = (uint16x8_t)vtrn1q_u64((uint64x2_t)y11, (uint64x2_t)y15);
    y23 = (uint16x8_t)vtrn2q_u64((uint64x2_t)y11, (uint64x2_t)y15);

    // 16x16: STR A4<-A4
    vst1q_u16(M + 32*0 + 24, y16);
    vst1q_u16(M + 32*1 + 24, y17);
    vst1q_u16(M + 32*2 + 24, y18);
    vst1q_u16(M + 32*3 + 24, y19);
    vst1q_u16(M + 32*4 + 24, y20);
    vst1q_u16(M + 32*5 + 24, y21);
    vst1q_u16(M + 32*6 + 24, y22);
    vst1q_u16(M + 32*7 + 24, y23);


    M += 256;
  }
}
