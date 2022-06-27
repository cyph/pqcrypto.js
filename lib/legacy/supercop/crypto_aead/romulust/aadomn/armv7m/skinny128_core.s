/******************************************************************************
* ARMv7-M assembly implementation of fixsliced Skinny-128-384+.
*
* For more details, see the paper at
* https://csrc.nist.gov/CSRC/media/Events/lightweight-cryptography-workshop-2020
* /documents/papers/fixslicing-lwc2020.pdf
* 
* @author 	Alexandre Adomnicai
* 			alex.adomnicai@gmail.com 
*
* @date     March 2022
******************************************************************************/

.syntax unified
.thumb

.macro swpmv in0, in1, tmp, mask, sh0, sh1
	eor 	\tmp, \in1, \in0, lsr \sh0
	and 	\tmp, \tmp, \mask, lsr \sh1
	eor 	\in1, \in1, \tmp
	eor 	\in0, \in0, \tmp, lsl \sh0
.endm

.macro sbox in0, in1, in2, in3, tmp, mask
	orr 	\tmp, \in0, \in1
	eor 	\in3, \in3, \tmp
	mvn 	\in3, \in3
	swpmv 	\in2, \in1, \tmp, \mask, #1, #0
	swpmv 	\in3, \in2, \tmp, \mask, #1, #0
	orr 	\tmp, \in2, \in3
	eor 	\in1, \in1, \tmp
	mvn 	\in1, \in1
	swpmv 	\in1, \in0, \tmp, \mask, #1, #0
	swpmv 	\in0, \in3, \tmp, \mask, #1, #0
	orr 	\tmp, \in0, \in1
	eor 	\in3, \in3, \tmp
	mvn 	\in3, \in3
	swpmv 	\in2, \in1, \tmp, \mask, #1, #0
	swpmv 	\in3, \in2, \tmp, \mask, #1, #0
	orr 	\tmp, \in2, \in3
	eor 	\in1, \in1, \tmp
	swpmv 	\in0, \in3, \tmp, \mask, #0, #0
.endm

.macro mixcol idx0, idx1, idx2, idx3, idx4, idx5
	and 	r8, r7, r2, ror \idx0
	eor 	r2, r2, r8, ror \idx1
	and 	r8, r7, r2, ror \idx2
	eor 	r2, r2, r8, ror \idx3
	and 	r8, r7, r2, ror \idx4
	eor 	r2, r2, r8, ror \idx5
	and 	r8, r7, r3, ror \idx0
	eor 	r3, r3, r8, ror \idx1
	and 	r8, r7, r3, ror \idx2
	eor 	r3, r3, r8, ror \idx3
	and 	r8, r7, r3, ror \idx4
	eor 	r3, r3, r8, ror \idx5
	and 	r8, r7, r4, ror \idx0
	eor 	r4, r4, r8, ror \idx1
	and 	r8, r7, r4, ror \idx2
	eor 	r4, r4, r8, ror \idx3
	and 	r8, r7, r4, ror \idx4
	eor 	r4, r4, r8, ror \idx5
	and 	r8, r7, r5, ror \idx0
	eor 	r5, r5, r8, ror \idx1
	and 	r8, r7, r5, ror \idx2
	eor 	r5, r5, r8, ror \idx3
	and 	r8, r7, r5, ror \idx4
	eor 	r5, r5, r8, ror \idx5
.endm

.macro rtk
	ldr.w 	r9, [r1, #4]
	ldr.w 	r10, [r1, #8]
	ldr.w 	r11, [r1, #12]
	ldr.w 	r8, [r1], #16
	eor 	r3, r3, r9
	eor 	r4, r4, r10
	eor 	r5, r5, r11
	eor 	r2, r2, r8
	ldr.w 	r9, [r0, #4]
	ldr.w 	r10, [r0, #8]
	ldr.w 	r11, [r0, #12]
	ldr.w 	r8, [r0], #16
	eor 	r3, r3, r9
	eor 	r4, r4, r10
	eor 	r5, r5, r11
	eor 	r2, r2, r8
.endm

quadruple_round:
	sbox 	r2, r3, r4, r5, r8, r6
	rtk
	mixcol 	#30, #24, #18, #2, #6, #4
	sbox 	r4, r5, r2, r3, r8, r6
	rtk
	mixcol 	#16, #30, #28, #0, #16, #2
	sbox 	r2, r3, r4, r5, r8, r6
	rtk
	mixcol 	#10, #4, #6, #6, #26, #0
	sbox 	r4, r5, r2, r3, r8, r6
	rtk
	mixcol 	#4, #26, #0, #4, #4, #22
	bx 		lr

@ void 	skinny128_384_plus(u8* out, const u8* in, const u8* rtk1, const u8* rtk23)
.global skinny128_384_plus
.type   skinny128_384_plus,%function
.align 2
skinny128_384_plus:
	push 	{r0-r12, r14}
	mov.w 	r0, r2
	// load input words
	ldr.w 	r3, [r1, #8]
	ldr.w 	r4, [r1, #4]
	ldr.w 	r5, [r1, #12]
	ldr.w 	r2, [r1]
	// preload bitmasks for swapmove (packing into bitsliced)
	movw 	r6, #0x0a0a
	movt 	r6, #0x0a0a
	movw 	r7, #0x3030
	movt 	r7, #0x3030
	swpmv 	r2, r2, r12, r6, #3, #0
	swpmv 	r3, r3, r12, r6, #3, #0
	swpmv 	r4, r4, r12, r6, #3, #0
	swpmv 	r5, r5, r12, r6, #3, #0
	swpmv 	r4, r2, r12, r7, #2, #0
	swpmv 	r3, r2, r12, r7, #4, #2
	swpmv 	r5, r2, r12, r7, #6, #4
	swpmv 	r3, r4, r12, r7, #2, #2
	swpmv 	r5, r4, r12, r7, #4, #4
	swpmv 	r5, r3, r12, r7, #2, #4
	// preload bitmasks for swapmove (s-box)
	movw 	r6, #0x5555
	movt 	r6, #0x5555
	// reload pointer to round tweakeys (erased when loading ptext)
	ldr.w 	r1, [sp, #12]
	// run 16 rounds
	bl 		quadruple_round
	bl 		quadruple_round
	bl 		quadruple_round
	bl 		quadruple_round
	// run 16 rounds
	sub.w 	r0, #256 	// reset rtk1
	bl 		quadruple_round
	bl 		quadruple_round
	bl 		quadruple_round
	bl 		quadruple_round
	// run 8 rounds (16*2+8 = 40 rounds for skinny128-384+)
	sub.w 	r0, #256 	// reset rtk1
	bl 		quadruple_round
	bl 		quadruple_round
	// preload bitmasks for swapmove (unpacking)
	movw 	r6, #0x0a0a
	movt 	r6, #0x0a0a
	// restore output buffer
	ldr.w 	r0, [sp], #4
	swpmv 	r5, r3, r12, r7, #2, #4
	swpmv 	r5, r4, r12, r7, #4, #4
	swpmv 	r3, r4, r12, r7, #2, #2
	swpmv 	r5, r2, r12, r7, #6, #4
	swpmv 	r3, r2, r12, r7, #4, #2
	swpmv 	r4, r2, r12, r7, #2, #0
	swpmv 	r5, r5, r12, r6, #3, #0
	swpmv 	r4, r4, r12, r6, #3, #0
	swpmv 	r3, r3, r12, r6, #3, #0
	swpmv 	r2, r2, r12, r6, #3, #0
	str.w 	r2, [r0]
	str.w 	r4, [r0, #4]
	str.w 	r3, [r0, #8]
	str.w 	r5, [r0, #12]
    pop 	{r1-r12,r14}
    bx 		lr
    