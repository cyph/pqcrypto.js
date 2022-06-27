/*******************************************************************************
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
*******************************************************************************/

.syntax unified
.thumb

// swapmove technique for bit manipulations
.macro swpmv in0, in1, tmp, mask, sh0, sh1
	eor 	\tmp, \in1, \in0, lsr \sh0
	and 	\tmp, \tmp, \mask, lsr \sh1
	eor 	\in1, \in1, \tmp
	eor 	\in0, \in0, \tmp, lsl \sh0
.endm

// packing from byte-array to bitsliced representation
packing:
	swpmv 	r2, r2, r12, r10, #3, #0
	swpmv 	r3, r3, r12, r10, #3, #0
	swpmv 	r4, r4, r12, r10, #3, #0
	swpmv 	r5, r5, r12, r10, #3, #0
	swpmv 	r4, r2, r12, r11, #2, #0
	swpmv 	r3, r2, r12, r11, #4, #2
	swpmv 	r5, r2, r12, r11, #6, #4
	swpmv 	r3, r4, r12, r11, #2, #2
	swpmv 	r5, r4, r12, r11, #4, #4
	swpmv 	r5, r3, r12, r11, #2, #4
	bx 		lr

// computes lfsr2 on tk2 in a bitsliced fashion
.macro lfsr2 out, in
	and 	r12, \in, r10
	eor 	r12, r12, \out
	and 	r14, r10, r12, lsl #1
	and 	r12, r12, r10 				
	orr 	\out, r14, r12, lsr #1
.endm

// computes lfsr3 on tk3 in a bitsliced fashion
.macro lfsr3 out, in
	and 	r12, \in, r10
	eor 	r12, \out, r12, lsr #1
	and 	r14, r10, r12, lsl #1
	and 	r12, r12, r10
	orr 	\out, r14, r12, lsr #1
.endm

// computes lfsr2(tk2) ^ lfsr3(tk3) and store the result as round tweakeys
.macro strtk tk2a, tk2b, tk3a, tk3b, inc
	eor 	r11, \tk2a, \tk3a 			//tk2 ^ tk3 (1st word)
	eor 	r12, \tk2b, \tk3b 			//tk2 ^ tk3 (2nd word)
	strd 	r11, r12, [r0], \inc 		//store in tk
.endm

/*
 * Computes LFSR2(TK2) ^ LFSR3(TK3) for all rounds.
 * Processing both at the same time allows to save some memory accesses.
 */
@ void 	tks_lfsr_23(uint32_t* tk, const uint8_t* tk2, const uint8_t* tk3, const int rounds)
.global tks_lfsr_23
.type   tks_lfsr_23,%function
.align	2
tks_lfsr_23:
	push 	{r0-r12, r14}
	// load 128-bit tk2
	ldr.w 	r3, [r1, #8]
	ldr.w 	r4, [r1, #4]
	ldr.w 	r5, [r1, #12]
	ldr.w 	r12, [r1]
	//move tk3 address in r1
	mov 	r1, r2
	//move 1st tk2 word in r2
	mov 	r2, r12 	
	// preload bitmasks for swapmove (packing)				
	movw 	r10, #0x0a0a
	movt 	r10, #0x0a0a
	movw 	r11, #0x3030
	movt 	r11, #0x3030
	// packing tk2 into bitsliced
	bl 		packing
	// move tk2 from r2-r5 to r6-r9
	mov 	r6, r2 						
	mov 	r7, r3
	mov 	r8, r4
	mov 	r9, r5
	// load 128-bit tk3
	ldr.w 	r3, [r1, #8]
	ldr.w 	r4, [r1, #4]
	ldr.w 	r5, [r1, #12]
	ldr.w 	r2, [r1]
	// packing tk3 into bitsliced
	bl 		packing
	eor 	r10, r10, r10, lsl #4
	// load loop counter (#rounds) in r1
	ldr.w 	r1, [sp, #12]
	// store tk2 ^ tk3 in round tweakeys array (r0)
	strtk 	r2, r3, r6, r7, #8
	strtk 	r4, r5, r8, r9, #8
	// Precompute 8 round tweakeys per iteration
	loop_2_3:
		lfsr2 	r6, r8
		lfsr3 	r5, r3
		strtk 	r5, r2, r7, r8, #8
		strtk 	r3, r4, r9, r6, #24
		lfsr2 	r7, r9
		lfsr3 	r4, r2
		strtk 	r4, r5, r8, r9, #8
		strtk 	r2, r3, r6, r7, #24
		lfsr2 	r8, r6
		lfsr3 	r3, r5
		strtk 	r3, r4, r9, r6, #8
		strtk 	r5, r2, r7, r8, #24
		lfsr2 	r9, r7
		lfsr3 	r2, r4
		strtk 	r2, r3, r6, r7, #8
		strtk 	r4, r5, r8, r9, #24
		subs.w 	r1, r1, #8 				//loop counter -= 8
		bne 	loop_2_3
	pop 	{r0-r12, r14}
	bx 		lr
