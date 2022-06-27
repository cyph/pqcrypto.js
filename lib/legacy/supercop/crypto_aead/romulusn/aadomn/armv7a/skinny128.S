/**
 * Skinny-128-384+ ARMv7-A Neon assembly implementation.
 *
 * The tweakey schedule is processed on-the-fly during the Skinny-128-384+
 * encryption function.
 * This implementation have been optimized for Cortex-A7 processors by taking
 * into consideration the latency cycles of vtbl instructions on this platform.
 *
 * More details in the paper available at:
 *
 * @author 	Alexandre Adomnicai
 * 			alex.adomnicai@gmail.com
 *
 * @date 	March 2022
 */

.text
.syntax unified

/******************************************************************************
 * Round constants c0 || c1 used in 'skinny128_384_plus'.
 * Note that we do not store c0 || c1 || c2 as it would double the amount of
 * memory required. Instead we pay an additional XOR (^c2) in the core routine.
 *****************************************************************************/
.type rconst,%object
rconst:
	.word 0x01, 0x00, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00
	.word 0x0f, 0x01, 0x0e, 0x03, 0x0d, 0x03, 0x0b, 0x03
	.word 0x07, 0x03, 0x0f, 0x02, 0x0e, 0x01, 0x0c, 0x03
	.word 0x09, 0x03, 0x03, 0x03, 0x07, 0x02, 0x0e, 0x00
	.word 0x0d, 0x01, 0x0a, 0x03, 0x05, 0x03, 0x0b, 0x02
	.word 0x06, 0x01, 0x0c, 0x02, 0x08, 0x01, 0x00, 0x03
	.word 0x01, 0x02, 0x02, 0x00, 0x05, 0x00, 0x0b, 0x00
	.word 0x07, 0x01, 0x0e, 0x02, 0x0c, 0x01, 0x08, 0x03
	.word 0x01, 0x03, 0x03, 0x02, 0x06, 0x00, 0x0d, 0x00
	.word 0x0b, 0x01, 0x06, 0x03, 0x0d, 0x02, 0x0a, 0x01
	.word 0x04, 0x03, 0x09, 0x02, 0x02, 0x01, 0x04, 0x02
	.word 0x08, 0x00, 0x01, 0x01, 0x02, 0x02, 0x04, 0x00
	.word 0x09, 0x00, 0x03, 0x01, 0x06, 0x02, 0x0c, 0x00
	.word 0x09, 0x01, 0x02, 0x03, 0x05, 0x02, 0x0a, 0x00

/******************************************************************************
 * Load into Neon registers the values required by the vtbl instructions.
 * After this macro we have:
 * 	- d23-d31 	contain the 4 inner S-boxes S0, S1, S2, S3
 * 	- d23 		contains the look-up table for the tweakey permutation
 * 	- d20-d22 	contain the look-up tables for the linear layer
 * 	- d12 		contain 0x0f...0f (to extract low nibbles)
 *****************************************************************************/
.macro load_vtbl
	// vtbl for sb_ark
	movw 	r2, #0x0004
	movt 	r2, #0x080c
	movw 	r3, #0x0501
	movt 	r3, #0x0d09
	vmov 	d24, r2, r3
	movw 	r2, #0x1410
	movt 	r2, #0x1c18
	movw 	r3, #0x1511
	movt 	r3, #0x1d19
	vmov 	d25, r2, r3
	movw 	r2, #0x8103
	movt 	r2, #0x0082
	movw 	r3, #0x22a0
	movt 	r3, #0xa220
	vmov 	d26, r2, r3
	movw 	r2, #0x4341
	movt 	r2, #0xc2c0
	movw 	r3, #0x6260
	movt 	r3, #0xe2e0
	vmov 	d27, r2, r3
	movw 	r2, #0x0201
	movt 	r2, #0x0300
	movw 	r3, #0x1211
	movt 	r3, #0x1310
	vmov 	d28, r2, r3
	movw 	r2, #0x0b09
	movt 	r2, #0x0a08
	movw 	r3, #0x1b19
	movt 	r3, #0x1a18
	vmov 	d29, r2, r3
	movw 	r2, #0x8480
	movt 	r2, #0x3433
	movw 	r3, #0x4443
	movt 	r3, #0x6463
	vmov 	d30, r2, r3
	movw 	r2, #0x1413
	movt 	r2, #0xa4a0
	movw 	r3, #0xc4c0
	movt 	r3, #0xe4e0
	vmov 	d31, r2, r3
	vmov.i8 d12, #0x0f
	// vtbl for sr_mc
	movw 	r2, #0x0100
	movt 	r2, #0x0302
	movw 	r3, #0x0407
	movt 	r3, #0x0605
	vmov 	d20, r3, r2
	movw 	r2, #0x0302
	movt 	r2, #0x0100
	vmov 	d21, r2, r2
	movw 	r2, #0x0605
	movt 	r2, #0x0407
	mvn 	r3, r2
	vmov 	d22, r3, r2
	// for TK perm
	movw 	r2, #0x0701
	movt	r2, #0x0500
	movw 	r3, #0x0602
	movt 	r3, #0x0304
	vmov 	d23, r2, r3
.endm

/******************************************************************************
 * Apply the 8-bit S-box and the round tweakeys to the internal state.
 * Some latency cycles are hidden by tweakey-related calculations.
 * The internal state is supposed to be stored in q0 = d0||d1 register.
 *****************************************************************************/
.macro sbox_ark tk1_h, tk1_l, tk2_h, tk2_l, tk3_h, tk3_l
	// first inner S-boxes layer
	ldrd 	r2, r3, [r1], #8 		// load the round constants
	vand 	d2, d0, d12 			// extract low nibbles
	vand 	d3, d1, d12 			// extract low nibbles
	vshr.u8 d4, d0, #4 				// extract high nibbles
	vshr.u8 d5, d1, #4 				// extract high nibbles
	vtbl.8 	d0, {d26,d27}, d2 		// apply inner S-box S0
	vtbl.8 	d1, {d26,d27}, d3 		// apply inner S-box S0
	vtbl.8 	d4, {d24,d25}, d4 		// apply inner S-box S1
	vtbl.8 	d5, {d24,d25}, d5 		// apply inner S-box S1
	veor 	d0, d0, d4 				// recombine inner S-boxes' outputs
	veor 	d1, d1, d5 				// recombine inner S-boxes' outputs
	// hide 2 latency cycles with tweakey-related calculations
	vmov 	d18, r2, r3 			// build double word rc = c0 || c1
	veor 	d16, \tk1_h, \tk2_h 	// tk1 ^ tk2
	// second inner S-boxes layer
	vshr.u8 d4, d0, #4 				// extract high nibbles
	vshr.u8 d5, d1, #4 				// extract high nibbles
	veor 	d18, d18, \tk3_h 		// rc ^ tk3
	vand 	d2, d0, d12 			// extract low nibbles
	vand 	d3, d1, d12 			// extract low nibbles
	vshr.u8 d0, d0, #7 				// extract additional bit (D4444)
	vshr.u8 d1, d1, #7 				// extract additional bit (D4444)
	veor 	d18, d18, d16 			// rc ^ tk1 ^ tk2 ^ tk3
	vtbl.8 	d4, {d28,d29}, d4 		// apply inner S-box S2
	vtbl.8 	d5, {d28,d29}, d5 		// apply inner S-box S2
	vtbl.8 	d2, {d30,d31}, d2 		// apply inner S-box S3
	vtbl.8 	d3, {d30,d31}, d3 		// apply inner S-box S3
	veor 	d4, d4, d18 			// add round tweakeay and rconst (c0||c1)
	veor 	d5, d5, d19 			// add rconst (c2)
	vorr 	d0, d0, d2 				// additional bitwise OR (D4444)
	vorr 	d1, d1, d3 				// additional bitwise OR (D4444)
	// hide 2 latency cycles with tweakey-related calculations
	vtbl.8 	\tk2_l, {\tk2_l}, d23 	// permuting tk2 (1st half)
	vtbl.8 	\tk3_l, {\tk3_l}, d23 	// permuting tk3 (1st half)
	// resume S-box calculations
	veor 	d0, d0, d4 				// recombine inner S-boxes' outputs
	veor 	d1, d1, d5 				// recombine inner S-boxes' outputs
	// hide 2 latency cycles with tweakey-related calculations
	vshl.u8 d5, \tk2_l, #2 			// [LFSR2] d5 <- (x5,x4,x3,x2,x1,x0,- ,- )
	vshr.u8 d17, \tk3_l, #6 		// [LFSR3] d7 <- (- ,- ,- ,- ,- ,- ,x7,x6)
.endm

/******************************************************************************
 * Apply the linear layer (ShiftRows and MixColumns) to the internal state.
 * Some latency cycles are hidden by tweakey-related calculations.
 * The internal state is supposed to be stored in q0 = d0||d1 register.
 *****************************************************************************/
.macro sr_mc  tk1_h, tk1_l, tk2_h, tk2_l, tk3_h, tk3_l
	vtbl.8 	d3, {d0}, d20
	vtbl.8 	d4, {d1}, d21
	vtbl.8 	d2, {d1}, d22
	veor 	d1, d3, d4
	// hide 2 latency cycles with tweakey-related calculations
	veor 	d5, d5, \tk2_h 			// [LFSR2] d5 <- (x7^x5,-,-,-,-,-,-,-)
	vshr.u8 d16, \tk3_h, #1 		// [LFSR3] d6 <- (- ,x7,x6,x5,x4,x3,x2,x1)
	veor 	\tk3_h, \tk3_h, d17 	// [LFSR3] d13  <- (-,-,-,-,-,-,x7^x1,x6^x0)
	// resume linear layer
	veor 	d2, d1, d2
	// hide 2 latency cycles with tweakey-related calculations
	vshl.u8 d4, \tk2_h, #1 			// [LFSR2] d4  <- (x6,x5,x4,x3,x2,x1,x0,- )
	vshr.u8 d5, d5, #7 				// [LFSR2] d5  <- (-,-,-,-,-,-,-,x7^x5)
	vshl.u8 \tk3_h, \tk3_h, #7		// [LFSR3] d13 <- (x6^x0,-,-,-,-,-,-,-)
	// resume linear layer
	vext.8 	d0, d2, d0, #4
	// hide 2 latency cycles with tweakey-related calculations
	vtbl.8 	\tk1_h, {\tk1_h}, d23 	// permuting tk1 (1st half)
	vorr 	\tk2_h, d4, d5 			// [LFSR2] (x6,x5,x4,x3,x2,x1,x0,x7^x5)
	vorr 	\tk3_h, \tk3_h, d16 	// [LFSR3] (x0^x6,x7,x6,x5,x4,x3,x2,x1)
.endm

/******************************************************************************
 * Apply two rounds of the SKinny-128-384 tweakable block cipher.
 * The internal state is supposed to be stored in q0 = d0||d1 register.
 *****************************************************************************/
.macro skinny_double_round
	sbox_ark 	d6, d7, d8, d9, d10, d11
	sr_mc 		d7, d6, d9, d8, d11, d10
	sbox_ark 	d7, d6, d9, d8, d11, d10
	sr_mc 		d6, d7, d8, d9, d10, d11
.endm

/******************************************************************************
 * Function to compute skinny-128-384+.
 * The prototype of the function is 
 *****************************************************************************/
.align 4
.global skinny128_384_plus
.type skinny128_384_plus, %function
.thumb
.thumb_func
skinny128_384_plus:
	push 	{r1-r4} 		// save current context
	ldr 	r4, [sp, #16] 	// load the fifth argument from the stack
	vpush 	{d8-d12} 		// save Neon context
	vld1.8 	{q0}, [r1] 		// loading the 128-bit input block
	vld1.8 	{q3}, [r2] 		// load tk1
	vld1.8 	{q4}, [r3] 		// load tk2
	vld1.8 	{q5}, [r4] 		// load tk3
	load_vtbl 				// prepare registers for the encryption process
	adr 	r1, rconst 		// r1 now points to the round constants
	movw 	r2, #0x0002
	movw 	r3, #0x0000
	vmov 	d19, r2, r3 	// build rconst c2 = 0x00020000
	// skinny-128-384+ has 40 rounds
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	skinny_double_round
	vst1.8 	{q0}, [r0] 		// storing the 128-bit output block
	vpop 	{d8-d12}
	pop 	{r1-r4}
	bx 		lr
