/*******************************************************************************
 * ARMv8-A NEON implementation of Skinny-128-384+ fine tuned for Romulus-T AEAD.
 * 
 * @author	Alexandre Adomnicai
 *		alex.adomnicai@gmail.com
 *
 * @date 	March 2022
 ******************************************************************************/

.text

/*******************************************************************************
 * Loads constant values into registers before encryption.
 * q21 		contains c2 round constant
 * q22 		contains tbl value to permute tk0
 * q23-q25 	contains tbl/mask values for the shiftrows/mixcolumns operations
 * q26 		contains 0x0f...0f for bitmasks
 * q27-q31 	contains the 4-bit and 5-bit Sboxes used for decomposition
 ******************************************************************************/
.macro prepare_encrypt
	ldr 	q21, =0x00000000000000020000000000000000
	ldr 	q22, =0x0f0e0d0c0a0b09080304060205000701
	ldr 	q23, =0x09080b0a06050407030201000c0f0e0d
	ldr 	q24, =0x0302010009080b0a1010101009080b0a
	ldr 	q25, =0x000000000000000000000000ffffffff
	ldr 	q26, =0x0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f
	ldr 	q27, =0x2e0e26062c0c34142a02220a00281830
	ldr 	q28, =0xd1915111c1814101d090501080c00040
	ldr 	q29, =0x1a181b190a080b091310121103000201
	ldr 	q30, =0x65646363454443433534333385848180
	ldr 	q31, =0xe5e4e1e0c5c4c1c0a5a4a1a015141313
.endm

/*******************************************************************************
 * Computes the 8-bit Sbox according to a decomposition into three 4-bit Sboxes
 * and one 5-bit Sbox.
 * The round tweakeys and constants are also added to the internal state to hide
 * some latency cycles.
 *******************************************************************************/
.macro sbox_ark tk0_h tk0_l
	// 1st layer
	and 	v1.16b, v0.16b, v26.16b 			// extract low nibbles
	ushr 	v2.16b, v0.16b, #4 					// extract high nibbles
	tbl 	v0.16b, {v27.16b}, v1.16b 			// computes the 1st 4-bit sbox
	tbl 	v2.16b, {v28.16b}, v2.16b 			// computes the 2nd 4-bit sbox
	ldr 	d5, [x3], #8 						// load rtk2 ^ rtk3
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	eor 	v5.16b, v5.16b, \tk0_h 				// add rtk1 to rtk2 ^ rtk3
	// 2nd layer
	ushr 	v2.16b, v0.16b, #3 					// extract bits 4-0
	and 	v1.16b, v0.16b, v26.16b 			// extract high nibbles
	tbl 	v2.16b, {v30.16b-v31.16b}, v2.16b 	// computes the 5-bit sbox
	tbl 	v0.16b, {v29.16b}, v1.16b 			// computes the 3rd 4-bt sbox
	tbl 	\tk0_l, {\tk0_l}, v22.16b 			// permute the next rtk1
	eor 	v0.16b, v0.16b, v2.16b 				// merge sboxes outputs
	eor 	v0.16b, v0.16b, v5.16b 				// add round tweakey
	eor 	v0.16b, v0.16b, v21.16b 			// add rconst c2
.endm

/*******************************************************************************
 * Computes the ShiftRows and MixColumns operations.
 * The tbl instructions permute the rows and shift the rows at the same time.
 ******************************************************************************/
.macro sr_mc
	tbl 	v1.16b, {v0.16b}, v23.16b 	// v1 contains rows [3,0,1,2]
	and 	v2.16b, v0.16b, v25.16b 	// v2 contais row [0]
	tbl 	v0.16b, {v0.16b}, v24.16b 	// v0 contains rows [2,-,2,0]
	eor 	v1.16b, v1.16b, v2.16b 		// [3^0,0,1,2]
	eor 	v0.16b, v0.16b, v1.16b 		// [3^0^2,0,1^2,2^0]
.endm

/*******************************************************************************
 * Double round routine for Skinny-128.
 * The first round adds v3.16b (upper tk1) to the state and updates v4.16b 
 * (lower tk1) while the second round does the opposite.
 ******************************************************************************/
.macro double_round
	sbox_ark v3.16b v4.16b
	sr_mc
	sbox_ark v4.16b v3.16b
	sr_mc
.endm

/*******************************************************************************
 * Skinny-128-384+ encryption.
 * The internal state is stored in q0, while tk0 (upper part only, since the
 * lower part always equals 0 in Romulus) is stored in q3.
 ******************************************************************************/
.align 4
.global skinny128_384_plus
.type skinny128_384_plus, %function
skinny128_384_plus:
	ldr 	q0, [x1] 		// load the 128-bit block
	ldr 	d3, [x2]		// load the first 2 lanes of tk1
	ldr 	d4, [x2, #8] 	// load the last 2 lanes of tk1
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
 * Computes two round tweakeys at the same time in order to take advantage of
 * 128-bit registers.
 ******************************************************************************/
.macro dbl_tk_update
	tbl 	v1.16b, {v1.16b}, v31.16b 	// permuting tk3
	tbl 	v0.16b, {v0.16b}, v31.16b 	// permuting tk2
	ldr 	x3, [x1], #8 				// load rconsts (1st round)
	ushr 	v3.16b, v1.16b, #6 			// [LFSR3]
	ushr 	v2.16b, v1.16b, #1 			// [LFSR3] (d5 & 0xee..ee) >> 1
	shl 	v4.16b, v0.16b, #2 			// [LFSR2] q10 <- (x5,x4,x3,x2,x1,x0,- ,-)
	shl 	v5.16b, v0.16b, #1 			// [LFSR2] q11 <- (x6,x5,x4,x3,x2,x1,x0,-)
	ldr 	d6, [x1], #8 				// load rconsts (2nd round)
	eor 	v1.16b, v1.16b, v3.16b 		// [LFSR3] q2  <- (-,-,-,-,-,-,x7^x1,x6^x0)
	eor 	v4.16b, v0.16b, v4.16b 		// [LFSR2] q10 <- (x7^x5,-,-,-,-,-,-,-)
	shl 	v1.16b, v1.16b, #7 			// [LFSR3] q2  <- (x6^x0,-,-,-,-,-,-,-)
	ushr 	v4.16b, v4.16b, #7 			// [LFSR2] q10 <- (-,-,-,-,-,-,-,x7^x5)
	mov 	v6.2d[1], x3 				// move rconsts into v6
	orr 	v1.16b, v1.16b, v2.16b 		// [LFSR3] (x0^x6,x7,x6,x5,x4,x3,x2,x1)
	orr 	v0.16b, v5.16b, v4.16b 		// [LFSR2] (x6,x5,x4,x3,x2,x1,x0,x7^x5)
	eor 	v3.16b, v6.16b, v0.16b
	eor 	v3.16b, v3.16b, v1.16b
	st1 	{v3.d}[1], [x0], #8
	st1	{v3.d}[0], [x0], #8
.endm


/*******************************************************************************
 * Precomputes rtk2 ^ rtk3 ^ c0 ^ c1 for 40 rounds of Skinny-128-384+.
 * Do not consider rtk1 since it changes for each message block within Romulus
 * AEAD. Also do not consider c2 as it would double the amount of RAM needed.
 ******************************************************************************/
.align 4
.global tk_schedule_23
.type tk_schedule_23, %function
tk_schedule_23:
	ldr 	q0, [x1]
	ldr 	q1, [x2]
	adr 	x1, rconst
	ldr 	q31, =0x0b0c0e0a0d080f090304060205000701
	// 1st round tweakey
	ldr 	d3, [x1], #8
	eor 	v2.16b, v0.16b, v1.16b
	eor 	v3.16b, v3.16b, v2.16b
	str 	d3, [x0], #8
	// reamining round tweakeys
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	dbl_tk_update
	// last round tweakey
	tbl 	v1.16b, {v1.16b}, v31.16b 	// permuting tk3
	tbl 	v0.16b, {v0.16b}, v31.16b 	// permuting tk2
	ldr 	x3, [x1] 					// load rconsts (1st round)
	ushr 	v3.16b, v1.16b, #6 			// [LFSR3]
	ushr 	v2.16b, v1.16b, #1 			// [LFSR3] (d5 & 0xee..ee) >> 1
	shl 	v4.16b, v0.16b, #2 			// [LFSR2] q10 <- (x5,x4,x3,x2,x1,x0,- ,-)
	shl 	v5.16b, v0.16b, #1 			// [LFSR2] q11 <- (x6,x5,x4,x3,x2,x1,x0,-)
	eor 	v1.16b, v1.16b, v3.16b 		// [LFSR3] q2  <- (-,-,-,-,-,-,x7^x1,x6^x0)
	eor 	v4.16b, v0.16b, v4.16b 		// [LFSR2] q10 <- (x7^x5,-,-,-,-,-,-,-)
	shl 	v1.16b, v1.16b, #7 			// [LFSR3] q2  <- (x6^x0,-,-,-,-,-,-,-)
	ushr 	v4.16b, v4.16b, #7 			// [LFSR2] q10 <- (-,-,-,-,-,-,-,x7^x5)
	mov 	v6.2d[1], x3 				// move rconsts into v6
	orr 	v1.16b, v1.16b, v2.16b 		// [LFSR3] (x0^x6,x7,x6,x5,x4,x3,x2,x1)
	orr 	v0.16b, v5.16b, v4.16b 		// [LFSR2] (x6,x5,x4,x3,x2,x1,x0,x7^x5)
	eor 	v3.16b, v6.16b, v0.16b
	eor 	v3.16b, v3.16b, v1.16b
	st1 	{v3.d}[1], [x0]
	ret

/*******************************************************************************
 * Computes two round tweakeys at the same time in order to take advantage of
 * 128-bit registers.
 ******************************************************************************/
.macro dbl_tk_update_notk2
	tbl 	v1.16b, {v1.16b}, v31.16b 	// permuting tk3
	ldr 	x3, [x1], #8 				// load rconsts (1st round)
	ushr 	v3.16b, v1.16b, #6 			// [LFSR3]
	ushr 	v2.16b, v1.16b, #1 			// [LFSR3] (d5 & 0xee..ee) >> 1
	ldr 	d6, [x1], #8 				// load rconsts (2nd round)
	eor 	v1.16b, v1.16b, v3.16b 		// [LFSR3] q2  <- (-,-,-,-,-,-,x7^x1,x6^x0)
	shl 	v1.16b, v1.16b, #7 			// [LFSR3] q2  <- (x6^x0,-,-,-,-,-,-,-)
	mov 	v6.2d[1], x3 				// move rconsts into v6
	orr 	v1.16b, v1.16b, v2.16b 		// [LFSR3] (x0^x6,x7,x6,x5,x4,x3,x2,x1)
	eor 	v3.16b, v6.16b, v1.16b
	st1 	{v3.d}[1], [x0], #8
	st1	{v3.d}[0], [x0], #8
.endm

/*******************************************************************************
 * Precomputes rtk1 ^ rtk2 ^ c0 ^ c1 for 40 rounds of Skinny-128-384+.
 * Do not consider rtk0 since it changes for each message block within Romulus
 * AEAD. Also do not consider c2 as it would double the amount of RAM needed.
 ******************************************************************************/
.align 4
.global tk_schedule_3
.type tk_schedule_3, %function
tk_schedule_3:
	ldr 	q1, [x1]
	adr 	x1, rconst
	ldr 	q31, =0x0b0c0e0a0d080f090304060205000701
	// 1st round tweakey
	ldr 	d3, [x1], #8
	eor 	v3.16b, v3.16b, v1.16b
	str 	d3, [x0], #8
	// reamining round tweakeys
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	dbl_tk_update_notk2
	// last round tweakey
	tbl 	v1.16b, {v1.16b}, v31.16b 	// permuting tk3
	ldr 	x3, [x1] 					// load rconsts (1st round)
	ushr 	v3.16b, v1.16b, #6 			// [LFSR3]
	ushr 	v2.16b, v1.16b, #1 			// [LFSR3] (d5 & 0xee..ee) >> 1
	eor 	v1.16b, v1.16b, v3.16b 		// [LFSR3] q2  <- (-,-,-,-,-,-,x7^x1,x6^x0)
	shl 	v1.16b, v1.16b, #7 			// [LFSR3] q2  <- (x6^x0,-,-,-,-,-,-,-)
	mov 	v6.2d[1], x3 				// move rconsts into v6
	orr 	v1.16b, v1.16b, v2.16b 		// [LFSR3] (x0^x6,x7,x6,x5,x4,x3,x2,x1)
	eor 	v3.16b, v6.16b, v1.16b
	st1 	{v3.d}[1], [x0]
	ret
