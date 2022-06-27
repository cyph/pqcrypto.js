/**
 * Skinny-128-384+ ARMv8-A (AArch64) Neon assembly implementation.
 *
 * The tweakey schedule is processed on-the-fly during the Skinny-128-384+
 * encryption function.
 * This implementation have been optimized for Cortex-A72 processors by taking
 * into consideration the latency cycles of tbl instructions on this platform.
 *
 * This version also include '*_notk2' macros and functions to avoid useless
 * calculations regarding TK2, which is null for many internal calls within
 * Romulus-T leakage-resilient variant.
 *
 * More details in the paper available at:
 *
 * @author 	Alexandre Adomnicai
 * 			alex.adomnicai@gmail.com
 *
 * @date 	March 2022
 */
.text

/*******************************************************************************
 * Round constants in order to speed up the round tweakeys calculation.
 ******************************************************************************/
.align 2
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

/*******************************************************************************
 * Loads constant values into registers before encryption.
 * q21 		contains c2 round constant
 * q22 		contains tbl value to permute tk0
 * q23-q25 	contains tbl/mask values for the shiftrows/mixcolumns operations
 * q26 		contains 0x0f...0f for bitmasks
 * q27-q31 	contains the 4-bit and 5-bit Sboxes used for decomposition
 ******************************************************************************/
.macro prepare_encrypt
	ldr 	q20, =0x0b0c0e0a0d080f090304060205000701 	// for TK permutation
	ldr 	q21, =0x00000000000000020000000000000000
	ldr 	q22, =0x00000000000000000000000000000000
	ldr 	q23, =0x09080b0a06050407030201000c0f0e0d
	ldr 	q24, =0x0302010009080b0a1010101009080b0a
	ldr 	q25, =0x000000000000000000000000ffffffff
	ldr 	q26, =0x0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f
	ldr 	q27, =0x2e0e26062c0c34142a02220a00281830
	ldr 	q28, =0xd1915111c1814101d090501080c00040
	ldr 	q29, =0x1a181b190a080b091310121103000201
	ldr 	q30, =0x65646363454443433534333385848180
	ldr 	q31, =0xe5e4e1e0c5c4c1c0a5a4a1a015141313
	//eor 	v3.16b, v3.16b, v21.16b  	// add rc2 to rtk0
.endm


/*******************************************************************************
 * Runs two rounds of skinny-128-384.
 * The S-box is computed thanks to the D_4454 decomposition and the round
 * tweakeys are computed on-the-fly. On some architectures (e.g. Cortex-A72)
 * those calculations on-the-fly have no impact on performance because of stall
 * cycles due to latency.
 ******************************************************************************/
.macro double_round
	// S-box (1st layer)
	and 	v1.16b, v0.16b, v26.16b 			// extract low nibbles
	ushr 	v2.16b, v0.16b, #4 					// extract high nibbles
	tbl 	v0.16b, {v27.16b}, v1.16b 			// computes the 1st 4-bit sbox
	tbl 	v2.16b, {v28.16b}, v2.16b 			// computes the 2nd 4-bit sbox
	// Tweakey schedule calculation to hide latency cycles
	ldr 	d16, [x1], #8 						// load rconsts (1st round)
	eor 	v7.8b, v5.8b, v6.8b 				// tk2 ^ tk3
	eor 	v7.8b, v7.8b, v4.8b 				// tk1 ^ tk2 ^ tk3
	eor 	v7.8b, v7.8b, v16.8b 				// tk1 ^ tk2 ^ tk3 ^ rc0 ^ rc1
	eor 	v7.16b, v7.16b, v21.16b 				// tk1 ^ tk2 ^ tk3 ^ rc0 ^ rc1 ^ rc2
	tbl 	v5.16b, {v5.16b}, v20.16b 			// permuting tk2
	tbl 	v6.16b, {v6.16b}, v20.16b 			// permuting tk3
	ldr 	x3, [x1], #8 						// load rconsts (2nd round)
	ushr 	v16.16b, v6.16b, #6 				// [LFSR3]
	// S-box (1st layer)
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// S-box (2nd layer)
	ushr 	v2.16b, v0.16b, #3 					// extract bits 4-0
	and 	v1.16b, v0.16b, v26.16b 			// extract high nibbles
	tbl 	v2.16b, {v30.16b-v31.16b}, v2.16b 	// computes the 5-bit sbox
	tbl 	v0.16b, {v29.16b}, v1.16b 			// computes the 3rd 4-bt sbox
	// Tweakey schedule calculation to hide latency cycles
	tbl 	v4.16b, {v4.16b}, v20.16b 			// permute the next rtk0
	ushr 	v17.16b, v6.16b, #1 				// [LFSR3] (d5 & 0xee..ee) >> 1
	shl 	v18.16b, v5.16b, #2 				// [LFSR2] q10 <- (x5,x4,x3,x2,x1,x0,- ,-)
	shl 	v19.16b, v5.16b, #1 				// [LFSR2] q11 <- (x6,x5,x4,x3,x2,x1,x0,-)
	eor 	v6.16b, v6.16b, v16.16b 			// [LFSR3] q2  <- (-,-,-,-,-,-,x7^x1,x6^x0)
	eor 	v18.16b, v5.16b, v18.16b 			// [LFSR2] q10 <- (x7^x5,-,-,-,-,-,-,-)
	// S-box (2nd layer)
	eor 	v0.16b, v0.16b, v7.16b 				// add round tweakey
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// linear layer
	tbl 	v1.16b, {v0.16b}, v23.16b 			// v1 contains rows [3,0,1,2]
	and 	v2.16b, v0.16b, v25.16b 			// v2 contais row [0]
	tbl 	v0.16b, {v0.16b}, v24.16b 			// v0 contains rows [2,-,2,0]
	mov 	v16.2d[1], x3 						// move rconsts into v16
	eor 	v1.16b, v1.16b, v2.16b 				// [3^0,0,1,2]
	eor 	v0.16b, v0.16b, v1.16b 				// [3^0^2,0,1^2,2^0]
	// S-box (1st layer)
	and 	v1.16b, v0.16b, v26.16b 			// extract low nibbles
	ushr 	v2.16b, v0.16b, #4 					// extract high nibbles
	tbl 	v0.16b, {v27.16b}, v1.16b 			// computes the 1st 4-bit sbox
	tbl 	v2.16b, {v28.16b}, v2.16b 			// computes the 2nd 4-bit sbox
	// Tweakey schedule calculation to hide latency cycles
	shl 	v6.16b, v6.16b, #7 					// [LFSR3] q2  <- (x6^x0,-,-,-,-,-,-,-)
	ushr 	v5.16b, v18.16b, #7 				// [LFSR2] q10 <- (-,-,-,-,-,-,-,x7^x5)
	orr 	v6.16b, v6.16b, v17.16b 			// [LFSR3] (x0^x6,x7,x6,x5,x4,x3,x2,x1)
	orr 	v5.16b, v5.16b, v19.16b 			// [LFSR2] (x6,x5,x4,x3,x2,x1,x0,x7^x5)
	eor 	v7.16b, v6.16b, v5.16b
	eor 	v7.16b, v7.16b, v16.16b
	eor 	v7.16b, v7.16b, v4.16b
	// S-box (1st layer)
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// S-box (2nd layer)
	ushr 	v2.16b, v0.16b, #3 					// extract bits 4-0
	and 	v1.16b, v0.16b, v26.16b 			// extract high nibbles
	tbl 	v2.16b, {v30.16b-v31.16b}, v2.16b 	// computes the 5-bit sbox
	tbl 	v0.16b, {v29.16b}, v1.16b 			// computes the 3rd 4-bt sbox
	ext  	v7.16b, v7.16b, v22.16b, #8 		// rotates the rtk to the right
	eor 	v7.16b, v7.16b, v21.16b 			// adds rc2
	eor 	v0.16b, v0.16b, v7.16b 				// add round tweakey
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// linear layer
	tbl 	v1.16b, {v0.16b}, v23.16b 			// v1 contains rows [3,0,1,2]
	and 	v2.16b, v0.16b, v25.16b 			// v2 contais row [0]
	tbl 	v0.16b, {v0.16b}, v24.16b 			// v0 contains rows [2,-,2,0]	
	eor 	v1.16b, v1.16b, v2.16b 				// [3^0,0,1,2]
	eor 	v0.16b, v0.16b, v1.16b 				// [3^0^2,0,1^2,2^0]
.endm


/*******************************************************************************
 * Runs two rounds of skinny-128-384 where TK2 = 0x00...00.
 * The S-box is computed thanks to the D_4454 decomposition and the round
 * tweakeys are computed on-the-fly. On some architectures (e.g. Cortex-A72)
 * those calculations on-the-fly have no impact on performance because of stall
 * cycles due to latency.
 ******************************************************************************/
.macro double_round_notk2
	// S-box (1st layer)
	and 	v1.16b, v0.16b, v26.16b 			// extract low nibbles
	ushr 	v2.16b, v0.16b, #4 					// extract high nibbles
	tbl 	v0.16b, {v27.16b}, v1.16b 			// computes the 1st 4-bit sbox
	tbl 	v2.16b, {v28.16b}, v2.16b 			// computes the 2nd 4-bit sbox
	// Tweakey schedule calculation to hide latency cycles
	ldr 	d16, [x1], #8 						// load rconsts (1st round)
	eor 	v7.8b, v6.8b, v4.8b 				// tk1 ^ tk3
	eor 	v7.8b, v7.8b, v16.8b 				// tk1 ^ tk3 ^ rc0 ^ rc1
	eor 	v7.16b, v7.16b, v21.16b 			// tk1 ^ tk3 ^ rc0 ^ rc1 ^ rc2
	tbl 	v6.16b, {v6.16b}, v20.16b 			// permuting tk3
	ldr 	x3, [x1], #8 						// load rconsts (2nd round)
	ushr 	v16.16b, v6.16b, #6 				// [LFSR3]
	// S-box (1st layer)
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// S-box (2nd layer)
	ushr 	v2.16b, v0.16b, #3 					// extract bits 4-0
	and 	v1.16b, v0.16b, v26.16b 			// extract high nibbles
	tbl 	v2.16b, {v30.16b-v31.16b}, v2.16b 	// computes the 5-bit sbox
	tbl 	v0.16b, {v29.16b}, v1.16b 			// computes the 3rd 4-bt sbox
	// Tweakey schedule calculation to hide latency cycles
	tbl 	v4.16b, {v4.16b}, v20.16b 			// permute the next rtk0
	ushr 	v17.16b, v6.16b, #1 				// [LFSR3] (d5 & 0xee..ee) >> 1
	eor 	v6.16b, v6.16b, v16.16b 			// [LFSR3] q2  <- (-,-,-,-,-,-,x7^x1,x6^x0)
	// S-box (2nd layer)
	eor 	v0.16b, v0.16b, v7.16b 				// add round tweakey
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// linear layer
	tbl 	v1.16b, {v0.16b}, v23.16b 			// v1 contains rows [3,0,1,2]
	and 	v2.16b, v0.16b, v25.16b 			// v2 contais row [0]
	tbl 	v0.16b, {v0.16b}, v24.16b 			// v0 contains rows [2,-,2,0]
	mov 	v16.2d[1], x3 						// move rconsts into v16
	eor 	v1.16b, v1.16b, v2.16b 				// [3^0,0,1,2]
	eor 	v0.16b, v0.16b, v1.16b 				// [3^0^2,0,1^2,2^0]
	// S-box (1st layer)
	and 	v1.16b, v0.16b, v26.16b 			// extract low nibbles
	ushr 	v2.16b, v0.16b, #4 					// extract high nibbles
	tbl 	v0.16b, {v27.16b}, v1.16b 			// computes the 1st 4-bit sbox
	tbl 	v2.16b, {v28.16b}, v2.16b 			// computes the 2nd 4-bit sbox
	// Tweakey schedule calculation to hide latency cycles
	shl 	v6.16b, v6.16b, #7 					// [LFSR3] q2  <- (x6^x0,-,-,-,-,-,-,-)
	orr 	v6.16b, v6.16b, v17.16b 			// [LFSR3] (x0^x6,x7,x6,x5,x4,x3,x2,x1)
	eor 	v7.16b, v6.16b, v16.16b
	eor 	v7.16b, v7.16b, v4.16b
	// S-box (1st layer)
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// S-box (2nd layer)
	ushr 	v2.16b, v0.16b, #3 					// extract bits 4-0
	and 	v1.16b, v0.16b, v26.16b 			// extract high nibbles
	tbl 	v2.16b, {v30.16b-v31.16b}, v2.16b 	// computes the 5-bit sbox
	tbl 	v0.16b, {v29.16b}, v1.16b 			// computes the 3rd 4-bt sbox
	ext  	v7.16b, v7.16b, v22.16b, #8 		// rotates the rtk to the right
	eor 	v7.16b, v7.16b, v21.16b 			// adds rc2
	eor 	v0.16b, v0.16b, v7.16b 				// add round tweakey
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	// linear layer
	tbl 	v1.16b, {v0.16b}, v23.16b 			// v1 contains rows [3,0,1,2]
	and 	v2.16b, v0.16b, v25.16b 			// v2 contais row [0]
	tbl 	v0.16b, {v0.16b}, v24.16b 			// v0 contains rows [2,-,2,0]	
	eor 	v1.16b, v1.16b, v2.16b 				// [3^0,0,1,2]
	eor 	v0.16b, v0.16b, v1.16b 				// [3^0^2,0,1^2,2^0]
.endm



/*******************************************************************************
 * Skinny-128-384+ encryption.
 * The internal state is stored in q0, while tk1, tk2, tk3 are respectively
 * stored in q4, q5 and q6, respectively.
 ******************************************************************************/
.align 4
.global skinny128_384_plus
.type skinny128_384_plus, %function
skinny128_384_plus:
	ldr 	q0, [x1] 		// load the 128-bit input block
	ldr 	q4, [x2] 		// load tk1
	ldr 	q5, [x3] 		// load tk2
	ldr 	q6, [x4] 		// load tk3
	adr 	x1, rconst
	prepare_encrypt 		// load the constant values
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	double_round
	str 	q0, [x0] 		// store the 128-bit output block
	ret
	
.align 4
.global skinny128_384_plus_notk2
.type skinny128_384_plus_notk2, %function
skinny128_384_plus_notk2:
	ldr 	q0, [x1] 		// load the 128-bit input block
	ldr 	q4, [x2] 		// load tk1
	ldr 	q6, [x3] 		// load tk3
	adr 	x1, rconst
	prepare_encrypt 		// load the constant values
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	double_round_notk2
	str 	q0, [x0] 		// store the 128-bit output block
	ret
