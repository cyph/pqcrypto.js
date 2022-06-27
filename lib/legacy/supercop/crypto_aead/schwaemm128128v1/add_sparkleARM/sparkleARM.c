///////////////////////////////////////////////////////////////////////////////
// sparkleARM.c: Assembler implementation for ARM of the SPARKLE permutation.//
// This file is part of the SPARKLE submission to NIST's LW Crypto Project.  //
// Version 0.2.0 (2019-03-28), see <http://www.cryptolux.org/> for updates.  //
// Authors: The SPARKLE Group (C. Beierle, A. Biryukov, L. Cardoso dos       //
// Santos, J. Groszschaedl, L. Perrin, A. Udovenko, V. Velichkov, Q. Wang).  //
// License: GPLv3 (see LICENSE file), other licenses available upon request. //
// Copyright (C) 2019 University of Luxembourg <http://www.uni.lu/>.         //
// ------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify it   //
// under the terms of the GNU General Public License as published by the     //
// Free Software Foundation, either version 3 of the License, or (at your    //
// option) any later version. This program is distributed in the hope that   //
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied     //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  //
// GNU General Public License for more details. You should have received a   //
// copy of the GNU General Public License along with this program. If not,   //
// see <http://www.gnu.org/licenses/>.                                       //
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "sparkle_ref.h"

#define ROT(x, n) (((x) >> (n)) | ((x) << (32-(n))))
#define ELL(x) (ROT(((x) ^ ((x) << 16)), 16))

#define ELLASM(X) \
    __asm__ __volatile__( \
        "EOR %[x], %[x], %[x], LSL #16 \n\t" \
        "ROR %[x], #16 \n\t" \
        : [x] "+r" (X) \
        : \
    )

static const uint32_t RCON[MAX_BRANCHES] = {      \
  0xB7E15162, 0xBF715880, 0x38B4DA56, 0x324E7738, \
  0xBB1185EB, 0x4F7C7B57, 0xCFBFA1C8, 0xC2B3293D  \
};

void ARXBOXasm(uint32_t *x, uint32_t *y, const uint32_t c){
    __asm__ __volatile__ (
        "ADD    %[x], %[x], %[y], ror #31     \n\t"
        "EOR    %[y], %[y], %[x], ror #24     \n\t"
        "EOR    %[x], %[c]                    \n\t"

        "ADD    %[x], %[x], %[y], ror #17     \n\t"
        "EOR    %[y], %[y], %[x], ror #17     \n\t"
        "EOR    %[x], %[c]                    \n\t"

        "ADD    %[x], %[y]                    \n\t"
        "EOR    %[y], %[y], %[x], ror #31     \n\t"
        "EOR    %[x], %[c]                    \n\t"

        "ADD    %[x], %[x], %[y], ror #24     \n\t"
        "EOR    %[y], %[y], %[x], ror #16     \n\t"
        "EOR    %[x], %[c]                    \n\t"
    : [x] "+r" (*x), [y] "+r" (*y)
    : [c] "r" (c)
    );
}

void ARXBoxfullasm(uint32_t *state, int nb, int ns){
//    for(int j = 0; j < 2*nb; j += 2) {
//      ARXBOXasm(&state[j], &state[j+1], RCON[j>>1]);
//    }
    __asm__ __volatile__(
        //save non-scratch registers
        "push	{r4, r5, r6, r7, r8, r9}       \n\t"
        "1:                                    \n\t" //loop entry point
        "cmp %[nb], #0                         \n\t"
        "ble 2f                                \n\t" //if nb is zero, then jump to ending
        //load 4 words and 2 constants
        "ldmia.w %[state], {r4, r5, r6, r7}    \n\t"
        "ldmia.w %[rcon]!, {r8, r9}            \n\t" //loads, and move pointer ahead 4 bytes ahead
#ifndef INTERLEAVED_ARXBOX
        //apply arxbox to 2 branches
        "ADD    r4, r4, r5, ror #31     \n\t"
        "EOR    r5, r5, r4, ror #24     \n\t"
        "EOR    r4, r8                  \n\t"

        "ADD    r4, r4, r5, ror #17     \n\t"
        "EOR    r5, r5, r4, ror #17     \n\t"
        "EOR    r4, r8                  \n\t"

        "ADD    r4, r5                  \n\t"
        "EOR    r5, r5, r4, ror #31     \n\t"
        "EOR    r4, r8                  \n\t"

        "ADD    r4, r4, r5, ror #24     \n\t"
        "EOR    r5, r5, r4, ror #16     \n\t"
        "EOR    r4, r8                  \n\t"
        //-----------------------------------
        "ADD    r6, r6, r7, ror #31     \n\t"
        "EOR    r7, r7, r6, ror #24     \n\t"
        "EOR    r6, r9                  \n\t"

        "ADD    r6, r6, r7, ror #17     \n\t"
        "EOR    r7, r7, r6, ror #17     \n\t"
        "EOR    r6, r9                  \n\t"

        "ADD    r6, r7                  \n\t"
        "EOR    r7, r7, r6, ror #31     \n\t"
        "EOR    r6, r9                  \n\t"

        "ADD    r6, r6, r7, ror #24     \n\t"
        "EOR    r7, r7, r6, ror #16     \n\t"
        "EOR    r6, r9                  \n\t"
#else
        //apply arxbox to 2 branches, interweaving for pipelining. Only good for Cortex M4
        "ADD    r4, r4, r5, ror #31     \n\t"
        "ADD    r6, r6, r7, ror #31     \n\t"
        "EOR    r5, r5, r4, ror #24     \n\t"
        "EOR    r7, r7, r6, ror #24     \n\t"
        "EOR    r4, r8                  \n\t"
        "EOR    r6, r9                  \n\t"

        "ADD    r4, r4, r5, ror #17     \n\t"
        "ADD    r6, r6, r7, ror #17     \n\t"
        "EOR    r5, r5, r4, ror #17     \n\t"
        "EOR    r7, r7, r6, ror #17     \n\t"
        "EOR    r4, r8                  \n\t"
        "EOR    r6, r9                  \n\t"

        "ADD    r4, r5                  \n\t"
        "ADD    r6, r7                  \n\t"
        "EOR    r5, r5, r4, ror #31     \n\t"
        "EOR    r7, r7, r6, ror #31     \n\t"
        "EOR    r4, r8                  \n\t"
        "EOR    r6, r9                  \n\t"

        "ADD    r4, r4, r5, ror #24     \n\t"
        "ADD    r6, r6, r7, ror #24     \n\t"
        "EOR    r5, r5, r4, ror #16     \n\t"
        "EOR    r7, r7, r6, ror #16     \n\t"
        "EOR    r4, r8                  \n\t"
        "EOR    r6, r9                  \n\t"
#endif
        //store 4 words
        "stmia %[state]!, {r4, r5, r6, r7}     \n\t" //store with writeback to [state]
        "sub %[nb], #2                         \n\t" //update nb index
        "b 1b                                  \n\t" //loop back
        //recover non-scratch registers
        "2:                                    \n\t"
        "pop	{r4, r5, r6, r7, r8, r9}       \n\t"
    :[state] "+r" (state), [nb] "+r" (nb)
    :[rcon] "r" (RCON)
);
}

void sparkle_optARM(uint32_t *state, int nb, int ns)
{
  int i, j;  // Step and branch counter
  uint32_t tmpx, tmpy, x0, y0;

  for(i = 0; i < ns; i ++) {
    // Add step counter
    state[1] ^= RCON[i%MAX_BRANCHES];
    state[3] ^= i;
    // ARXBox layer
    ARXBoxfullasm(state, nb, ns);
    // Linear layer
    //feistel round
    tmpx = x0 = state[0];
    tmpy = y0 = state[1];
    for(j = 2; j < nb; j += 2) {
      tmpx ^= state[j];
      tmpy ^= state[j+1];
    }
    tmpx = ELL(tmpx);
    tmpy = ELL(tmpy);

    //branch rotation
    for (j = 2; j < nb; j += 2) {
      state[j-2]    = state[j+nb] ^ state[j] ^ tmpy;
      state[j+nb]   = state[j];
      state[j-1]    = state[j+nb+1] ^ state[j+1] ^ tmpx;
      state[j+nb+1] = state[j+1];
    }
    state[nb-2] = state[nb] ^ x0 ^ tmpy;
    state[nb]   = x0;
    state[nb-1] = state[nb+1] ^ y0 ^ tmpx;
    state[nb+1] = y0;
  }

}
