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

// tweakey permutation applied twice on 32-bit input
.macro perm2 in
	and 	r11, r1, \in, ror #14
	bfi 	r11, \in, #16, #8
	and 	r12, \in, #0xcc000000
	orr 	r11, r11, r12, lsr #2
	and 	r12, r10, \in
	orr 	r11, r11, r12, lsr #8
	and 	r12, \in, #0x00cc0000
	orr 	\in, r11, r12, lsr #18
.endm

// tweakey permutation applied twice on full tweakey
p2:
	// bitmasks for perm2
	movw 	r1, #0xcc00
	movt 	r1, #0xcc00
	movw 	r10, #0xcc00
	movt 	r10, #0x0033
	perm2 r6
	perm2 r7
	perm2 r8
	perm2 r9
	bx 		lr

// tweakey permutation applied 4 times on 32-bit input
.macro perm4 in
 	and 	r10, r14, \in, ror #22
 	and 	r1, r12, \in, ror #16
 	orr 	r10, r10,  r1
 	and 	r1, \in, r11
 	orr 	r10, r10, r1, lsr #2
	movw 	r1, #0xcc33
 	and 	\in, \in, r1
 	orr 	\in, r10, \in, ror #24
.endm

// tweakey permutation applied 4 times on full tweakey
p4:
	str.w 	r14, [sp]
	// bitmasks for perm4
	movw 	r14, #0x00cc
	movt 	r14, #0xcc00
	movw 	r12, #0xcc00
	movt 	r12, #0x3300
	movw 	r11, #0x00cc
	movt 	r11, #0x00cc
	perm4 r6
	perm4 r7
	perm4 r8
	perm4 r9
 	ldr.w 	r14, [sp] 					//restore r14
 	bx 		lr

// tweakey permutation applied 6 times on 32-bit input
.macro perm6 in
	and 	r10, \in, r1, ror #8
	and 	r11, r12, \in, ror #24
	orr 	r11, r11, r10, ror #6
	and 	r10, r1, \in, ror #10
	orr 	r11, r11, r10
	and 	r10, \in, #0x000000cc
	orr 	r11, r11, r10, lsl #14
	and 	r10, \in, #0x00003300
	orr 	\in, r11, r10, lsl #2
.endm

// tweakey permutation applied 6 times on full tweakey
p6:
	// bitmasks for perm6
	movw 	r1, #0x3333
	movw 	r12, #0x00cc
	movt 	r12, #0x3300
	perm6 r6
	perm6 r7
	perm6 r8
	perm6 r9
 	bx 		lr
	
// tweakey permutation applied 8 times on 32-bit input
.macro perm8 in
	and 	r10, \in, r1
	and 	r11, r1, \in, ror #8
	orr 	r11, r11, r10, ror #24
	and 	r10, \in, r12, lsl #2
	orr 	r11, r11, r10, ror #26
	and 	r10, \in, r12, lsl #8
	orr 	\in, r11, r10, lsr #6
.endm

// tweakey permutation applied 8 times on full tweakey
p8:
	// bitmasks for perm8
	movw 	r12, #0x3333
	movw 	r1, #0x0000
	movt 	r1, #0x33cc
	perm8 r6
	perm8 r7
	perm8 r8
	perm8 r9
 	bx 		lr

// tweakey permutation applied 10 times on 32-bit input
.macro perm10 in
	and 	r10, \in, r1, ror #8
	and 	r11, r12, \in, ror #26
	orr 	r11, r11, r10, ror #8
	and 	r10, \in, r12, ror #24
	orr 	r11, r11, r10, ror #22
	and 	r10, \in, #0x00330000
	orr 	r11, r11, r10, lsr #14
	and 	r10, \in, #0x0000cc00
	orr 	\in, r11, r10, lsr #2
.endm

// tweakey permutation applied 10 times on full tweakey
p10:
	// bitmasks for perm10
	movw 	r12, #0x0033
	movt 	r12, #0x3300
	movw 	r1, #0xcc33
	perm10 r6
	perm10 r7
	perm10 r8
	perm10 r9
 	bx 		lr

// tweakey permutation applied 12 times on 32-bit input
.macro perm12 in
	and 	r10, r14, \in, ror #8
	and 	r11, r12, \in, ror #30
	orr 	r11, r11, r10
	and 	r10, r1, \in, ror #16
	orr 	r11, r11, r10
	movw 	r10, #0xcccc
	and 	r10, \in, r10, ror #8
	orr 	\in, r11, r10, ror #10
.endm

// tweakey permutation applied 12 times on full tweakey
p12:
	str.w 	r14, [sp]
	// bitmasks for perm12
	movw 	r14, #0xcc33
	movw 	r12, #0x00cc
	movt 	r12, #0x00cc
	movw 	r1, #0x3300
	movt 	r1, #0xcc00
	perm12 	r6
	perm12 	r7
	perm12 	r8
	perm12 	r9
 	ldr.w 	r14, [sp] 					//restore r14
 	bx 		lr

// tweakey permutation applied 14 times on 32-bit input
.macro perm14 in
	and 	r10, r1, \in, ror #24
	and 	r11, \in, #0x00000033
	orr 	r11, r10, r11, ror #14
	and 	r10, \in, #0x33000000
	orr 	r11, r11, r10, ror #30
	and 	r10, \in, #0x00ff0000
	orr 	r11, r11, r10, ror #16
	and 	r10, \in, r12
	orr 	\in, r11, r10, ror #18
.endm

// tweakey permutation applied 14 times on full tweakey
p14:
	// bitmasks for perm14
	movw 	r1, #0xcc00
	movt 	r1, #0x0033
	movw 	r12, #0xcc00
	movt 	r12, #0xcc00
	perm14 	r6
	perm14 	r7
	perm14 	r8
	perm14 	r9
 	bx 		lr

// tweakey permutation applied 16 times is the identity function
.macro perm16 in
.endm

// bitmasks and rotations to match fixslicing (odd rounds)
.macro bs2fs_odd sh0, sh1, sh2
	and 	r10, r10, r10, lsr #6 		//r10<- 0x03030303
	and 	r11, r10, r6, ror \sh0
	and 	r6, r6, r10, lsl \sh1
	orr 	r6, r11, r6, ror \sh2
	and 	r11, r10, r7, ror \sh0
	and 	r7, r7, r10, lsl \sh1
	orr 	r7, r11, r7, ror \sh2
	and 	r11, r10, r8, ror \sh0
	and 	r8, r8, r10, lsl \sh1
	orr 	r8, r11, r8, ror \sh2
	and 	r11, r10, r9, ror \sh0
	and 	r9, r9, r10, lsl \sh1
	orr 	r9, r11, r9, ror \sh2
.endm

// bitmasks and rotations to match fixslicing (even rounds)
.macro bs2fs_even sh0, sh1, sh2
	and 	r10, r10, r10, lsr #2 		//r10<- 0x30303030
	and 	r11, r10, r6, ror \sh0
	and 	r6, r6, r10, ror \sh1
	orr 	r6, r11, r6, ror \sh2
	and 	r11, r10, r7, ror \sh0
	and 	r7, r7, r10, ror \sh1
	orr 	r7, r11, r7, ror \sh2
	and 	r11, r10, r8, ror \sh0
	and 	r8, r8, r10, ror \sh1
	orr 	r8, r11, r8, ror \sh2
	and 	r11, r10, r9, ror \sh0
	and 	r9, r9, r10, ror \sh1
	orr 	r9, r11, r9, ror \sh2
.endm


@ void tks_perm_23(uint8_t *rtk)
.global tks_perm_23
.type   tks_perm_23,%function
.align	2
tks_perm_23:
	push 		{r0-r12, lr}
	sub.w 		sp, #4 						//to store r14 in subroutines
	ldm 		r0, {r6-r9} 				//load tk
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r6, r6, r10 				//tk &= 0xf0f0f0f0 (1st word)
	and 		r7, r7, r10 				//tk &= 0xf0f0f0f0 (2nd word)
	and 		r8, r8, r10 				//tk &= 0xf0f0f0f0 (3rd word)
	and 		r9, r9, r10 				//tk &= 0xf0f0f0f0 (4th word)
	eor 		r8, r8, #0x00000004 		//add rconst
	eor 		r9, r9, #0x00000040 		//add rconst
	mvn 		r9, r9 						//to remove a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 1st round
	strd 		r6, r7, [r0], #8  			//store 2nd half tk for 1st round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p2 							//apply the permutation twice
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #26 		//ror and mask to match fixslicing
	strd		r11, r12, [r0], #8 			//store 1st half tk for 2nd round
	and 		r11, r10, r8, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #26 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x10000000 		//add rconst
	eor 		r11, r11, #0x00000100 		//add rconst
	eor 		r12, r12, #0x00000100 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd		r11, r12, [r0], #8 			//store 2nd half tk for 2nd round
	bs2fs_odd 	28, 6, 12
	eor 		r7, r7, #0x04000000 		//add rconst
	eor 		r8, r8, #0x44000000 		//add rconst
	eor 		r9, r9, #0x04000000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 3rd round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 3rd round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p4 							//apply the permutation 4 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00400000 		//add rconst
	eor 		r12, r12, #0x00400000 		//add rconst
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 5th round
	and 		r11, r10, r8, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00440000 		//add rconst
	eor 		r12, r12, #0x00500000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 5th round
	bs2fs_even 	14, 4, 6
	eor 		r6, r6, #0x00100000 		//add rconst
	eor 		r7, r7, #0x00100000 		//add rconst
	eor 		r8, r8, #0x00100000 		//add rconst
	eor 		r8, r8, #0x00000001 		//add rconst
	eor 		r9, r9, #0x00100000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 4th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 4th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p6 							//apply the permutation 6 times
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x01000000 		//add rconst
	eor 		r12, r12, #0x01000000 		//add rconst
	strd 		r11, r12, [r0], #8 			//store 1st half tk for 6th round
	and 		r11, r10, r8, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x01400000 		//add rconst
	eor 		r11, r11, #0x00001000 		//add rconst
	eor 		r12, r12, #0x00400000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0], #8 			//store 2nd half tk for 6th round
	bs2fs_odd 	12, 6, 28
	eor 		r6, r6, #0x00000400 		//add rconst
	eor 		r7, r7, #0x00000400 		//add rconst
	eor 		r8, r8, #0x01000000 		//add rconst
	eor 		r8, r8, #0x00004000 		//add rconst
	eor 		r9, r9, #0x01000000 		//add rconst
	eor 		r9, r9, #0x00000400 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 7th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 7th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p8 							//apply the permutation 8 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6 				//ror and mask to match fixslicing
	and 		r12, r10, r7 				//ror and mask to match fixslicing
	eor 		r12, r12, #0x00000040 		//add rconst
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 9th round
	and 		r11, r10, r8 				//ror and mask to match fixslicing
	and 		r12, r10, r9 				//ror and mask to match fixslicing
	eor 		r11, r11, #0x00000054 		//add rconst
	eor 		r12, r12, #0x00000050 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 9th round
	bs2fs_even 	30, 4, 22
	eor 		r6 ,r6, #0x00000010
	eor 		r8, r8, #0x00010000
	eor 		r8, r8, #0x00000410
	eor 		r9, r9, #0x00000410
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 8th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 8th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p10 						//apply the permutation 10 times
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #26 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00000100 		//add rconst
	eor 		r12, r12, #0x00000100 		//add rconst
	strd		r11, r12, [r0], #8 			//store 1st half tk for 10th round
	and 		r11, r10, r8, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #26 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x10000000 		//add rconst
	eor 		r11, r11, #0x00000140 		//add rconst
	eor 		r12, r12, #0x00000100 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd		r11, r12, [r0], #8 			//store 2nd half tk for 10th round
	bs2fs_odd 	28, 6, 12
	eor 		r6, r6, #0x04000000 		//add rconst
	eor 		r7, r7, #0x04000000 		//add rconst
	eor 		r8, r8, #0x44000000 		//add rconst
	eor 		r9, r9, #0x00000100 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 11th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 11th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p12 						//apply the permutation 4 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00400000 		//add rconst
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 13th round
	and 		r11, r10, r8, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00140000 		//add rconst
	eor 		r12, r12, #0x00500000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 13th round
	bs2fs_even 	14, 4, 6
	eor 		r6, r6, #0x00100000 		//add rconst
	eor 		r7, r7, #0x00100000 		//add rconst
	eor 		r8, r8, #0x04000000 		//add rconst
	eor 		r8, r8, #0x00000001 		//add rconst
	eor 		r9, r9, #0x04000000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 12th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 12th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p14 						//apply the permutation 6 times
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #10 		//ror and mask to match fixslicing
	strd 		r11, r12, [r0], #8 			//store 1st half tk for 14th round
	and 		r11, r10, r8, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x01400000 		//add rconst
	eor 		r11, r11, #0x00001000 		//add rconst
	eor 		r12, r12, #0x01400000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0], #8 			//store 2nd half tk for 14th round
	bs2fs_odd 	12, 6, 28
	eor 		r7, r7, #0x00000400 		//add rconst
	eor 		r8, r8, #0x01000000 		//add rconst
	eor 		r8, r8, #0x00004400 		//add rconst
	eor 		r9, r9, #0x00000400 		//add const
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 15th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 15th round
	ldm 		r0, {r6-r9} 				//load tk
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6 				//ror and mask to match fixslicing
	and 		r12, r10, r7 				//ror and mask to match fixslicing
	eor 		r11, r11, #0x00000040 		//add rconst
	eor 		r12, r12, #0x00000040 		//add rconst
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 17th round
	and 		r11, r10, r8 				//ror and mask to match fixslicing
	and 		r12, r10, r9 				//ror and mask to match fixslicing
	eor 		r11, r11, #0x00000004 		//add rconst
	eor 		r12, r12, #0x00000050 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 	r11, r12, [r0, #16] 		//store 1st half tk for 17th round
	bs2fs_even 	30, 4, 22
	eor 		r6 ,r6, #0x00000010
	eor 		r7 ,r7, #0x00000010
	eor 		r8, r8, #0x00000010
	eor 		r8, r8, #0x00010000
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 16th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 16th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p2 							//apply the permutation twice
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #26 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00000100 		//add rconst
	strd		r11, r12, [r0], #8 			//store 1st half tk for 18th round
	and 		r11, r10, r8, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #26 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x10000000 		//add rconst
	eor 		r11, r11, #0x00000140 		//add rconst
	eor 		r12, r12, #0x00000040 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd		r11, r12, [r0], #8 			//store 2nd half tk for 18th round
	bs2fs_odd 	28, 6, 12
	eor 		r7, r7, #0x04000000 		//add rconst
	eor 		r8, r8, #0x40000000 		//add rconst
	eor 		r8, r8, #0x00000100 		//add rconst
	eor 		r9, r9, #0x04000000 		//add rconst
	eor 		r9, r9, #0x00000100 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 19th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 19th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p4 							//apply the permutation 4 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #16 		//ror and mask to match fixslicing
	eor 		r12, r12, #0x00400000 		//add rconst
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 21th round
	and 		r11, r10, r8, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00440000 		//add rconst
	eor 		r12, r12, #0x00100000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 21th round
	bs2fs_even 	14, 4, 6
	eor 		r6, r6, #0x00100000 		//add rconst
	eor 		r8, r8, #0x04100000 		//add rconst
	eor 		r8, r8, #0x00000001 		//add rconst
	eor 		r9, r9, #0x00100000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 20th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 20th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p6 							//apply the permutation 6 times
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x01000000 		//add rconst
	eor 		r12, r12, #0x01000000 		//add rconst
	strd 		r11, r12, [r0], #8 			//store 1st half tk for 22th round
	and 		r11, r10, r8, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00400000 		//add rconst
	eor 		r11, r11, #0x00001000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0], #8 			//store 2nd half tk for 22th round
	bs2fs_odd 	12, 6, 28
	eor 		r6, r6, #0x00000400 		//add rconst
	eor 		r8, r8, #0x00004000 		//add rconst
	eor 		r9, r9, #0x01000000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 23th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 23th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p8 							//apply the permutation 8 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6 				//ror and mask to match fixslicing
	and 		r12, r10, r7 				//ror and mask to match fixslicing
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 25th round
	and 		r11, r10, r8 				//ror and mask to match fixslicing
	and 		r12, r10, r9 				//ror and mask to match fixslicing
	eor 		r11, r11, #0x00000014 		//add rconst
	eor 		r12, r12, #0x00000040 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 25th round
	bs2fs_even 	30, 4, 22
	eor 		r8, r8, #0x00010400
	eor 		r9, r9, #0x00000400
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 24th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 24th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p10 						//apply the permutation 10 times
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #26 		//ror and mask to match fixslicing
	strd		r11, r12, [r0], #8 			//store 1st half tk for 26th round
	and 		r11, r10, r8, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #26 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x10000000 		//add rconst
	eor 		r11, r11, #0x00000100 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd		r11, r12, [r0], #8 			//store 2nd half tk for 26th round
	bs2fs_odd 	28, 6, 12
	eor 		r7, r7, #0x04000000 		//add rconst
	eor 		r8, r8, #0x40000000 		//add rconst
	eor 		r9, r9, #0x04000000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 27th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 27th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p12 						//apply the permutation 4 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #16 		//ror and mask to match fixslicing
	eor 		r12, r12, #0x00400000 		//add rconst
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 29th round
	and 		r11, r10, r8, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00440000 		//add rconst
	eor 		r12, r12, #0x00500000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 29th round
	bs2fs_even 	14, 4, 6
	eor 		r6, r6, #0x00100000 		//add rconst
	eor 		r8, r8, #0x00100000 		//add rconst
	eor 		r8, r8, #0x00000001 		//add rconst
	eor 		r9, r9, #0x00100000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 28th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 28th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p14 						//apply the permutation 6 times
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x01000000 		//add rconst
	eor 		r12, r12, #0x01000000 		//add rconst
	strd 		r11, r12, [r0], #8 			//store 1st half tk for 30th round
	and 		r11, r10, r8, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x01400000 		//add rconst
	eor 		r11, r11, #0x00001000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0], #8 			//store 2nd half tk for 30th round
	bs2fs_odd 	12, 6, 28
	eor 		r6, r6, #0x00000400 		//add rconst
	eor 		r7, r7, #0x00000400 		//add rconst
	eor 		r8, r8, #0x00004000 		//add rconst
	eor 		r9, r9, #0x01000000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 31th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 31th round
	ldm 		r0, {r6-r9} 				//load tk
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6 				//ror and mask to match fixslicing
	and 		r12, r10, r7 				//ror and mask to match fixslicing
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 33th round
	and 		r11, r10, r8 				//ror and mask to match fixslicing
	and 		r12, r10, r9 				//ror and mask to match fixslicing
	eor 		r11, r11, #0x00000014 		//add rconst
	eor 		r12, r12, #0x00000050 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 33th round
	bs2fs_even 	30, 4, 22
	eor 		r6 ,r6, #0x00000010
	eor 		r8, r8, #0x00010400
	eor 		r9, r9, #0x00000400
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 32th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 32th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p2 							//apply the permutation twice
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #26 		//ror and mask to match fixslicing
	strd		r11, r12, [r0], #8 			//store 1st half tk for 34th round
	and 		r11, r10, r8, ror #26 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #26 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x10000000 		//add rconst
	eor 		r11, r11, #0x00000140 		//add rconst
	eor 		r12, r12, #0x00000100 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd		r11, r12, [r0], #8 			//store 2nd half tk for 34th round
	bs2fs_odd 	28, 6, 12
	eor 		r7, r7, #0x04000000 		//add rconst
	eor 		r8, r8, #0x44000000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 35th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 35th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p4 							//apply the permutation 4 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	and 		r11, r10, r6, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00400000 		//add rconst
	strd 		r11, r12, [r0, #24] 		//store 2nd half tk for 37th round
	and 		r11, r10, r8, ror #16 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #16 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x00440000 		//add rconst
	eor 		r12, r12, #0x00500000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0, #16] 		//store 1st half tk for 37th round
	bs2fs_even 	14, 4, 6
	eor 		r6, r6, #0x00100000 		//add rconst
	eor 		r7, r7, #0x00100000 		//add rconst
	eor 		r8, r8, #0x00000001 		//add rconst
	eor 		r9, r9, #0x00100000 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 36th round
	strd 		r8, r9, [r0], #24 			//store 2nd half tk for 36th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p6 							//apply the permutation 6 times
	movw 		r10, #0xc3c3
	movt 		r10, #0xc3c3 				//r10<- 0xc3c3c3c3
	and 		r11, r10, r6, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r7, ror #10 		//ror and mask to match fixslicing
	eor 		r12, r12, #0x01000000 		//add rconst
	strd 		r11, r12, [r0], #8 			//store 1st half tk for 38th round
	and 		r11, r10, r8, ror #10 		//ror and mask to match fixslicing
	and 		r12, r10, r9, ror #10 		//ror and mask to match fixslicing
	eor 		r11, r11, #0x01400000 		//add rconst
	eor 		r11, r11, #0x00001000 		//add rconst
	eor 		r12, r12, #0x00400000 		//add rconst
	mvn 		r12, r12 					//to save a NOT in sbox calculations
	strd 		r11, r12, [r0], #8 			//store 2nd half tk for 38th round
	bs2fs_odd 	12, 6, 28
	eor 		r6, r6, #0x00000400 		//add rconst
	eor 		r7, r7, #0x00000400 		//add rconst
	eor 		r8, r8, #0x01000000
	eor 		r8, r8, #0x00004000 		//add rconst
	eor 		r9, r9, #0x00000400 		//add rconst
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r8, r9, [r0], #8 			//store 1st half tk for 39th round
	strd 		r6, r7, [r0], #8 			//store 2nd half tk for 39th round
	ldm 		r0, {r6-r9} 				//load tk
	bl 			p8 							//apply the permutation 8 times
	movw 		r10, #0xf0f0
	movt 		r10, #0xf0f0 				//r10<- 0xf0f0f0f0
	bs2fs_even 	30, 4, 22
	eor 		r6, r6, #0x00000010
	eor 		r8, r8, #0x00010000
	eor 		r8, r8, #0x00000010
	eor 		r9, r9, #0x00000400
	mvn 		r9, r9 						//to save a NOT in sbox calculations
	strd 		r6, r7, [r0], #8 			//store 1st half tk for 40th round
	strd 		r8, r9, [r0] 			//store 2nd half tk for 40th round
	add.w 		sp, #4
	pop 		{r0-r12, lr}
	bx 			lr

// Applies the permutations P^2, ..., P^14 to TK1 for rounds 0 to 16.
// Since P^16=Id, we  don't need more calculations as no LFSR is applied to TK1
@ void 	tk_schedule_1(uint8_t* rtk1, const uint8_t* tk1)
.global tk_schedule_1
.type   tk_schedule_1,%function
.align	2
tk_schedule_1:
	push 	{r0-r12, lr}
	ldr.w 	r3, [r1, #8] 				//load tk1 (3rd word)
	ldr.w 	r4, [r1, #4] 				//load tk1 (2nd word)
	ldr.w 	r5, [r1, #12] 				//load tk1 (4th word)
	ldr.w 	r2, [r1] 					//load tk1 (1st word)
	movw 	r10, #0x0a0a
	movt 	r10, #0x0a0a 				//r6 <- 0x0a0a0a0a
	movw 	r11, #0x3030
	movt 	r11, #0x3030 				//r7 <- 0x30303030
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
	mov 	r6, r2 						//move tk1 from r2-r5 to r6-r9
	mov 	r7, r3 						//move tk1 from r2-r5 to r6-r9
	mov 	r8, r4 						//move tk1 from r2-r5 to r6-r9
	mov 	r9, r5 						//move tk1 from r2-r5 to r6-r9
	movw 	r2, #0xf0f0
	movt 	r2, #0xf0f0 				//r2<- 0xf0f0f0f0
	and 	r11, r8, r2 				//tk &= 0xf0f0f0f0 (3rd word)
	and 	r12, r9, r2 				//tk &= 0xf0f0f0f0 (4th word)
	strd 	r11, r12, [r0], #8 			//store 1st half tk for 1st round
	and 	r11, r6, r2 				//tk &= 0xf0f0f0f0 (1st word)
	and 	r12, r7, r2 				//tk &= 0xf0f0f0f0 (2nd word)
	strd 	r11, r12, [r0], #8  		//store 2nd half tk for 1st round
	movw 	r3, #0x3030
	movt 	r3, #0x3030 				//r3 <- 0x30303030
	and 	r11, r3, r6, ror #30 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, ror #4
	orr 	r12, r11, r12, ror #22
	str.w 	r12, [r0, #224]
	and 	r11, r3, r7, ror #30
	and 	r12, r7, r3, ror #4
	orr 	r12, r11, r12, ror #22
	str.w 	r12, [r0, #228]
	and 	r11, r3, r8, ror #30
	and 	r12, r8, r3, ror #4
	orr 	r12, r11, r12, ror #22
	str.w 	r12, [r0, #232]
	and 	r11, r3, r9, ror #30
	and 	r12, r9, r3, ror #4
	orr 	r12, r11, r12, ror #22		//ror and masks to match fixslicing ---
	str.w 	r12, [r0, #236]
	bl 		p2 							//apply the permutation twice
	movw 	r3, #0xc3c3
	movt 	r3, #0xc3c3 				//r3 <- 0xc3c3c3c3
	and 	r11, r3, r6, ror #26 		//ror and mask to match fixslicing
	and 	r12, r3, r7, ror #26 		//ror and mask to match fixslicing
	strd	r11, r12, [r0], #8 			//store 1st half tk for 2nd round
	and 	r11, r3, r8, ror #26 		//ror and mask to match fixslicing
	and 	r12, r3, r9, ror #26 		//ror and mask to match fixslicing
	strd	r11, r12, [r0], #8 			//store 2nd half tk for 2nd round
	and 	r3, r3, r3, lsr #6 			//r3<- 0x03030303
	and 	r11, r3, r6, ror #28 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0, #8]
	and 	r11, r3, r7, ror #28
	and 	r12, r7, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0, #12]
	and 	r11, r3, r9, ror #28
	and 	r12, r9, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0, #4]
	and 	r11, r3, r8, ror #28
	and 	r12, r8, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0], #16				//ror and masks to match fixslicing ---
	bl 		p2 							//apply the permutation 4 times
	lsl 	r3, r3, #4 					//r3 <- 0x30303030
	and 	r11, r3, r6, ror #14 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, ror #4
	orr 	r12, r11, r12, ror #6
	str.w 	r12, [r0], #4
	and 	r11, r3, r7, ror #14
	and 	r12, r7, r3, ror #4
	orr 	r12, r11, r12, ror #6
	str.w 	r12, [r0], #4
	and 	r11, r3, r8, ror #14
	and 	r12, r8, r3, ror #4
	orr 	r12, r11, r12, ror #6
	str.w 	r12, [r0], #4
	and 	r11, r3, r9, ror #14
	and 	r12, r9, r3, ror #4
	orr 	r12, r11, r12, ror #6		//ror and masks to match fixslicing ---
	str.w 	r12, [r0], #4
	and 	r11, r2, r6, ror #16 		//ror and mask to match fixslicing
	and 	r12, r2, r7, ror #16 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0, #8] 			//store 2nd half tk for 5th round
	and 	r11, r2, r8, ror #16 		//ror and mask to match fixslicing
	and 	r12, r2, r9, ror #16 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0], #16 		//store 1st half tk for 5th round
	bl 		p2 							//apply the permutation twice
	movw 	r3, #0xc3c3
	movt 	r3, #0xc3c3 				//r3<- 0xc3c3c3c3
	and 	r11, r3, r6, ror #10 		//ror and mask to match fixslicing
	and 	r12, r3, r7, ror #10 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0], #8 			//store 1st half tk for 6th round
	and 	r11, r3, r8, ror #10 		//ror and mask to match fixslicing
	and 	r12, r3, r9, ror #10 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0], #8 			//store 2nd half tk for 6th round
	and 	r3, r3, r3, lsr #6 			//r3<- 0x03030303
	and 	r11, r3, r6, ror #12 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0, #8]
	and 	r11, r3, r7, ror #12
	and 	r12, r7, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0, #12]
	and 	r11, r3, r9, ror #12
	and 	r12, r9, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0, #4]
	and 	r11, r3, r8, ror #12
	and 	r12, r8, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0], #16 				//ror and masks to match fixslicing ---
	bl 		p2 							//apply the permutation 8 times
	lsl 	r3, r3, #4 					//r3 <- 0x30303030
	and 	r11, r3, r6, ror #30 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, ror #4
	orr 	r12, r11, r12, ror #22
	str.w 	r12, [r0], #4
	and 	r11, r3, r7, ror #30
	and 	r12, r7, r3, ror #4
	orr 	r12, r11, r12, ror #22
	str.w 	r12, [r0], #4
	and 	r11, r3, r8, ror #30
	and 	r12, r8, r3, ror #4
	orr 	r12, r11, r12, ror #22
	str.w 	r12, [r0], #4
	and 	r11, r3, r9, ror #30
	and 	r12, r9, r3, ror #4
	orr 	r12, r11, r12, ror #22		//ror and masks to match fixslicing ---
	str.w 	r12, [r0], #4
	and 	r11, r2, r6 				//ror and mask to match fixslicing
	and 	r12, r2, r7 				//ror and mask to match fixslicing
	strd 	r11, r12, [r0, #8] 			//store 2nd half tk for 9th round
	and 	r11, r2, r8 				//ror and mask to match fixslicing
	and 	r12, r2, r9 				//ror and mask to match fixslicing
	strd 	r11, r12, [r0], #16 		//store 1st half tk for 9th round
	bl 		p2 							//apply the permutation 10
	movw 	r3, #0xc3c3
	movt 	r3, #0xc3c3 				//r3 <- 0xc3c3c3c3
	and 	r11, r3, r6, ror #26 		//ror and mask to match fixslicing
	and 	r12, r3, r7, ror #26 		//ror and mask to match fixslicing
	strd	r11, r12, [r0], #8 			//store 1st half tk for 10th round
	and 	r11, r3, r8, ror #26 		//ror and mask to match fixslicing
	and 	r12, r3, r9, ror #26 		//ror and mask to match fixslicing
	strd	r11, r12, [r0], #8 			//store 2nd half tk for 10th round
	and 	r3, r3, r3, lsr #6 			//r3 <- 0x03030303
	and 	r11, r3, r6, ror #28 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0, #8]
	and 	r11, r3, r7, ror #28
	and 	r12, r7, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0, #12]
	and 	r11, r3, r9, ror #28
	and 	r12, r9, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0, #4]
	and 	r11, r3, r8, ror #28
	and 	r12, r8, r3, lsl #6
	orr 	r12, r11, r12, ror #12
	str.w 	r12, [r0], #16				//ror and masks to match fixslicing ---
	bl 		p2 							//apply the permutation 12 times
	lsl 	r3, r3, #4 					//r3 <- 0x30303030
	and 	r11, r3, r6, ror #14 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, ror #4
	orr 	r12, r11, r12, ror #6
	str.w 	r12, [r0], #4
	and 	r11, r3, r7, ror #14
	and 	r12, r7, r3, ror #4
	orr 	r12, r11, r12, ror #6
	str.w 	r12, [r0], #4
	and 	r11, r3, r8, ror #14
	and 	r12, r8, r3, ror #4
	orr 	r12, r11, r12, ror #6
	str.w 	r12, [r0], #4
	and 	r11, r3, r9, ror #14
	and 	r12, r9, r3, ror #4
	orr 	r12, r11, r12, ror #6		//ror and masks to match fixslicing ---
	str.w 	r12, [r0], #4
	and 	r11, r2, r6, ror #16 		//ror and mask to match fixslicing
	and 	r12, r2, r7, ror #16 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0, #8] 			//store 2nd half tk for 5th round
	and 	r11, r2, r8, ror #16 		//ror and mask to match fixslicing
	and 	r12, r2, r9, ror #16 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0], #16 		//store 1st half tk for 5th round
	bl 		p2 							//apply the permutation 14 times
	movw 	r3, #0xc3c3
	movt 	r3, #0xc3c3 				//r3 <- 0xc3c3c3c3
	and 	r11, r3, r6, ror #10 		//ror and mask to match fixslicing
	and 	r12, r3, r7, ror #10 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0], #8 			//store 1st half tk for 14th round
	and 	r11, r3, r8, ror #10 		//ror and mask to match fixslicing
	and 	r12, r3, r9, ror #10 		//ror and mask to match fixslicing
	strd 	r11, r12, [r0], #8 			//store 2nd half tk for 14th round
	and 	r3, r3, r3, lsr #6 			//r3 <- 0x03030303
	and 	r11, r3, r6, ror #12 		//--- ror and masks to match fixslicing
	and 	r12, r6, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0, #8]
	and 	r11, r3, r7, ror #12
	and 	r12, r7, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0, #12]
	and 	r11, r3, r9, ror #12
	and 	r12, r9, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0, #4]
	and 	r11, r3, r8, ror #12
	and 	r12, r8, r3, lsl #6
	orr 	r12, r11, r12, ror #28
	str.w 	r12, [r0], #16 				//ror and masks to match fixslicing ---
	pop 	{r0-r12, lr}
	bx 		lr
