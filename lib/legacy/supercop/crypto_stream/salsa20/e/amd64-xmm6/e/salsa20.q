int64 a

int64 arg1
int64 arg2
int64 arg3
int64 arg4
input arg1
input arg2
input arg3
input arg4


int64 k
int64 kbits
int64 iv

int64 i

int64 x
int64 m
int64 out
int64 bytes

stack32 eax_stack
stack32 ebx_stack
stack32 esi_stack
stack32 edi_stack
stack32 ebp_stack

int6464 diag0
int6464 diag1
int6464 diag2
int6464 diag3
# situation at beginning of first round:
# diag0: x0 x5 x10 x15
# diag1: x12 x1 x6 x11
# diag2: x8 x13 x2 x7
# diag3: x4 x9 x14 x3
# situation at beginning of second round:
# diag0: x0 x5 x10 x15
# diag1: x1 x6 x11 x12
# diag2: x2 x7 x8 x13
# diag3: x3 x4 x9 x14

int6464 a0
int6464 a1
int6464 a2
int6464 a3
int6464 a4
int6464 a5
int6464 a6
int6464 a7
int6464 b0
int6464 b1
int6464 b2
int6464 b3
int6464 b4
int6464 b5
int6464 b6
int6464 b7

int6464 z0
int6464 z1
int6464 z2
int6464 z3
int6464 z4
int6464 z5
int6464 z6
int6464 z7
int6464 z8
int6464 z9
int6464 z10
int6464 z11
int6464 z12
int6464 z13
int6464 z14
int6464 z15

stack128 z0_stack
stack128 z1_stack
stack128 z2_stack
stack128 z3_stack
stack128 z4_stack
stack128 z5_stack
stack128 z6_stack
stack128 z7_stack
stack128 z8_stack
stack128 z9_stack
stack128 z10_stack
stack128 z11_stack
stack128 z12_stack
stack128 z13_stack
stack128 z14_stack
stack128 z15_stack

int6464 y0
int6464 y1
int6464 y2
int6464 y3
int6464 y4
int6464 y5
int6464 y6
int6464 y7
int6464 y8
int6464 y9
int6464 y10
int6464 y11
int6464 y12
int6464 y13
int6464 y14
int6464 y15

int6464 r0
int6464 r1
int6464 r2
int6464 r3
int6464 r4
int6464 r5
int6464 r6
int6464 r7
int6464 r8
int6464 r9
int6464 r10
int6464 r11
int6464 r12
int6464 r13
int6464 r14
int6464 r15

stack128 orig0
stack128 orig1
stack128 orig2
stack128 orig3
stack128 orig4
stack128 orig5
stack128 orig6
stack128 orig7
stack128 orig8
stack128 orig9
stack128 orig10
stack128 orig11
stack128 orig12
stack128 orig13
stack128 orig14
stack128 orig15

int64 in0
int64 in1
int64 in2
int64 in3
int64 in4
int64 in5
int64 in6
int64 in7
int64 in8
int64 in9
int64 in10
int64 in11
int64 in12
int64 in13
int64 in14
int64 in15

stack512 tmp

int64 ctarget
stack64 bytes_backup


enter crypto_stream_salsa20_e_amd64_xmm6_ECRYPT_keystream_bytes

x = arg1
m = arg2
out = m
bytes = arg3

              unsigned>? bytes - 0
goto done if !unsigned>

a = 0
i = bytes
while (i) { *out++ = a; --i }
out -= bytes

goto start


enter crypto_stream_salsa20_e_amd64_xmm6_ECRYPT_decrypt_bytes

x = arg1
m = arg2
out = arg3
bytes = arg4

              unsigned>? bytes - 0
goto done if !unsigned>

goto start


enter crypto_stream_salsa20_e_amd64_xmm6_ECRYPT_encrypt_bytes

x = arg1
m = arg2
out = arg3
bytes = arg4

              unsigned>? bytes - 0
goto done if !unsigned>


start:

                              unsigned<? bytes - 256
  goto bytesbetween1and255 if unsigned<

  z0 = *(int128 *) (x + 0)
  z5 = z0[1,1,1,1]
  z10 = z0[2,2,2,2]
  z15 = z0[3,3,3,3]
  z0 = z0[0,0,0,0]
  orig5 = z5
  orig10 = z10
  orig15 = z15
  orig0 = z0

  z1 = *(int128 *) (x + 16)
  z6 = z1[2,2,2,2]
  z11 = z1[3,3,3,3]
  z12 = z1[0,0,0,0]
  z1 = z1[1,1,1,1]
  orig6 = z6
  orig11 = z11
  orig12 = z12
  orig1 = z1

  z2 = *(int128 *) (x + 32)
  z7 = z2[3,3,3,3]
  z13 = z2[1,1,1,1]
  z2 = z2[2,2,2,2]
  orig7 = z7
  orig13 = z13
  orig2 = z2

  z3 = *(int128 *) (x + 48)
  z4 = z3[0,0,0,0]
  z14 = z3[2,2,2,2]
  z3 = z3[3,3,3,3]
  orig4 = z4
  orig14 = z14
  orig3 = z3

bytesatleast256:

  in8 = *(uint32 *) (x + 32)
  in9 = *(uint32 *) (x + 52)
  ((uint32 *) &orig8)[0] = in8
  ((uint32 *) &orig9)[0] = in9
  in8 += 1
  in9 <<= 32
  in8 += in9
  in9 = in8
  (uint64) in9 >>= 32
  ((uint32 *) &orig8)[1] = in8
  ((uint32 *) &orig9)[1] = in9
  in8 += 1
  in9 <<= 32
  in8 += in9
  in9 = in8
  (uint64) in9 >>= 32
  ((uint32 *) &orig8)[2] = in8
  ((uint32 *) &orig9)[2] = in9
  in8 += 1
  in9 <<= 32
  in8 += in9
  in9 = in8
  (uint64) in9 >>= 32
  ((uint32 *) &orig8)[3] = in8
  ((uint32 *) &orig9)[3] = in9
  in8 += 1
  in9 <<= 32
  in8 += in9
  in9 = in8
  (uint64) in9 >>= 32
  *(uint32 *) (x + 32) = in8
  *(uint32 *) (x + 52) = in9

  bytes_backup = bytes

i = 20

  z5 = orig5
  z10 = orig10
  z15 = orig15
  z14 = orig14
  z3 = orig3
  z6 = orig6
  z11 = orig11
  z1 = orig1

  z7 = orig7
  z13 = orig13
  z2 = orig2
  z9 = orig9
  z0 = orig0
  z12 = orig12
  z4 = orig4
  z8 = orig8


mainloop1:

						z10_stack = z10
								z15_stack = z15

		y4 = z12
uint32323232	y4 += z0
		r4 = y4
uint32323232	y4 <<= 7
		z4 ^= y4
uint32323232	r4 >>= 25
		z4 ^= r4

				y9 = z1
uint32323232			y9 += z5
				r9 = y9
uint32323232			y9 <<= 7
				z9 ^= y9
uint32323232			r9 >>= 25
				z9 ^= r9

		y8 = z0
uint32323232	y8 += z4
		r8 = y8
uint32323232	y8 <<= 9
		z8 ^= y8
uint32323232	r8 >>= 23
		z8 ^= r8

				y13 = z5
uint32323232			y13 += z9
				r13 = y13
uint32323232			y13 <<= 9
				z13 ^= y13
uint32323232			r13 >>= 23
				z13 ^= r13

		y12 = z4
uint32323232	y12 += z8
		r12 = y12
uint32323232	y12 <<= 13
		z12 ^= y12
uint32323232	r12 >>= 19
		z12 ^= r12

				y1 = z9
uint32323232			y1 += z13
				r1 = y1
uint32323232			y1 <<= 13
				z1 ^= y1
uint32323232			r1 >>= 19
				z1 ^= r1

		y0 = z8
uint32323232	y0 += z12
		r0 = y0
uint32323232	y0 <<= 18
		z0 ^= y0
uint32323232	r0 >>= 14
		z0 ^= r0

						z10 = z10_stack
		z0_stack = z0

				y5 = z13
uint32323232			y5 += z1
				r5 = y5
uint32323232			y5 <<= 18
				z5 ^= y5
uint32323232			r5 >>= 14
				z5 ^= r5

						y14 = z6
uint32323232					y14 += z10
						r14 = y14
uint32323232					y14 <<= 7
						z14 ^= y14
uint32323232					r14 >>= 25
						z14 ^= r14

								z15 = z15_stack
				z5_stack = z5

								y3 = z11
uint32323232							y3 += z15
								r3 = y3
uint32323232							y3 <<= 7
								z3 ^= y3
uint32323232							r3 >>= 25
								z3 ^= r3

						y2 = z10
uint32323232					y2 += z14
						r2 = y2
uint32323232					y2 <<= 9
						z2 ^= y2
uint32323232					r2 >>= 23
						z2 ^= r2

								y7 = z15
uint32323232							y7 += z3
								r7 = y7
uint32323232							y7 <<= 9
								z7 ^= y7
uint32323232							r7 >>= 23
								z7 ^= r7

						y6 = z14
uint32323232					y6 += z2
						r6 = y6
uint32323232					y6 <<= 13
						z6 ^= y6
uint32323232					r6 >>= 19
						z6 ^= r6

								y11 = z3
uint32323232							y11 += z7
								r11 = y11
uint32323232							y11 <<= 13
								z11 ^= y11
uint32323232							r11 >>= 19
								z11 ^= r11

						y10 = z2
uint32323232					y10 += z6
						r10 = y10
uint32323232					y10 <<= 18
						z10 ^= y10
uint32323232					r10 >>= 14
						z10 ^= r10

		z0 = z0_stack
						z10_stack = z10

		y1 = z3
uint32323232	y1 += z0
		r1 = y1
uint32323232	y1 <<= 7
		z1 ^= y1
uint32323232	r1 >>= 25
		z1 ^= r1

								y15 = z7
uint32323232							y15 += z11
								r15 = y15
uint32323232							y15 <<= 18
								z15 ^= y15
uint32323232							r15 >>= 14
								z15 ^= r15

				z5 = z5_stack
								z15_stack = z15

				y6 = z4
uint32323232			y6 += z5
				r6 = y6
uint32323232			y6 <<= 7
				z6 ^= y6
uint32323232			r6 >>= 25
				z6 ^= r6

		y2 = z0
uint32323232	y2 += z1
		r2 = y2
uint32323232	y2 <<= 9
		z2 ^= y2
uint32323232	r2 >>= 23
		z2 ^= r2

				y7 = z5
uint32323232			y7 += z6
				r7 = y7
uint32323232			y7 <<= 9
				z7 ^= y7
uint32323232			r7 >>= 23
				z7 ^= r7

		y3 = z1
uint32323232	y3 += z2
		r3 = y3
uint32323232	y3 <<= 13
		z3 ^= y3
uint32323232	r3 >>= 19
		z3 ^= r3

				y4 = z6
uint32323232			y4 += z7
				r4 = y4
uint32323232			y4 <<= 13
				z4 ^= y4
uint32323232			r4 >>= 19
				z4 ^= r4

		y0 = z2
uint32323232	y0 += z3
		r0 = y0
uint32323232	y0 <<= 18
		z0 ^= y0
uint32323232	r0 >>= 14
		z0 ^= r0

						z10 = z10_stack
		z0_stack = z0

				y5 = z7
uint32323232			y5 += z4
				r5 = y5
uint32323232			y5 <<= 18
				z5 ^= y5
uint32323232			r5 >>= 14
				z5 ^= r5

						y11 = z9
uint32323232					y11 += z10
						r11 = y11
uint32323232					y11 <<= 7
						z11 ^= y11
uint32323232					r11 >>= 25
						z11 ^= r11

								z15 = z15_stack
				z5_stack = z5

								y12 = z14
uint32323232							y12 += z15
								r12 = y12
uint32323232							y12 <<= 7
								z12 ^= y12
uint32323232							r12 >>= 25
								z12 ^= r12

						y8 = z10
uint32323232					y8 += z11
						r8 = y8
uint32323232					y8 <<= 9
						z8 ^= y8
uint32323232					r8 >>= 23
						z8 ^= r8

								y13 = z15
uint32323232							y13 += z12
								r13 = y13
uint32323232							y13 <<= 9
								z13 ^= y13
uint32323232							r13 >>= 23
								z13 ^= r13

						y9 = z11
uint32323232					y9 += z8
						r9 = y9
uint32323232					y9 <<= 13
						z9 ^= y9
uint32323232					r9 >>= 19
						z9 ^= r9

								y14 = z12
uint32323232							y14 += z13
								r14 = y14
uint32323232							y14 <<= 13
								z14 ^= y14
uint32323232							r14 >>= 19
								z14 ^= r14

						y10 = z8
uint32323232					y10 += z9
						r10 = y10
uint32323232					y10 <<= 18
						z10 ^= y10
uint32323232					r10 >>= 14
						z10 ^= r10

								y15 = z13
uint32323232							y15 += z14
								r15 = y15
uint32323232							y15 <<= 18
								z15 ^= y15
uint32323232							r15 >>= 14
								z15 ^= r15

		z0 = z0_stack
				z5 = z5_stack

                  unsigned>? i -= 2
goto mainloop1 if unsigned>

  uint32323232 z0 += orig0
  uint32323232 z1 += orig1
  uint32323232 z2 += orig2
  uint32323232 z3 += orig3
  in0 = z0
  in1 = z1
  in2 = z2
  in3 = z3
  z0 <<<= 96
  z1 <<<= 96
  z2 <<<= 96
  z3 <<<= 96
  (uint32) in0 ^= *(uint32 *) (m + 0)
  (uint32) in1 ^= *(uint32 *) (m + 4)
  (uint32) in2 ^= *(uint32 *) (m + 8)
  (uint32) in3 ^= *(uint32 *) (m + 12)
  *(uint32 *) (out + 0) = in0
  *(uint32 *) (out + 4) = in1
  *(uint32 *) (out + 8) = in2
  *(uint32 *) (out + 12) = in3
  in0 = z0
  in1 = z1
  in2 = z2
  in3 = z3
  z0 <<<= 96
  z1 <<<= 96
  z2 <<<= 96
  z3 <<<= 96
  (uint32) in0 ^= *(uint32 *) (m + 64)
  (uint32) in1 ^= *(uint32 *) (m + 68)
  (uint32) in2 ^= *(uint32 *) (m + 72)
  (uint32) in3 ^= *(uint32 *) (m + 76)
  *(uint32 *) (out + 64) = in0
  *(uint32 *) (out + 68) = in1
  *(uint32 *) (out + 72) = in2
  *(uint32 *) (out + 76) = in3
  in0 = z0
  in1 = z1
  in2 = z2
  in3 = z3
  z0 <<<= 96
  z1 <<<= 96
  z2 <<<= 96
  z3 <<<= 96
  (uint32) in0 ^= *(uint32 *) (m + 128)
  (uint32) in1 ^= *(uint32 *) (m + 132)
  (uint32) in2 ^= *(uint32 *) (m + 136)
  (uint32) in3 ^= *(uint32 *) (m + 140)
  *(uint32 *) (out + 128) = in0
  *(uint32 *) (out + 132) = in1
  *(uint32 *) (out + 136) = in2
  *(uint32 *) (out + 140) = in3
  in0 = z0
  in1 = z1
  in2 = z2
  in3 = z3
  (uint32) in0 ^= *(uint32 *) (m + 192)
  (uint32) in1 ^= *(uint32 *) (m + 196)
  (uint32) in2 ^= *(uint32 *) (m + 200)
  (uint32) in3 ^= *(uint32 *) (m + 204)
  *(uint32 *) (out + 192) = in0
  *(uint32 *) (out + 196) = in1
  *(uint32 *) (out + 200) = in2
  *(uint32 *) (out + 204) = in3

  uint32323232 z4 += orig4
  uint32323232 z5 += orig5
  uint32323232 z6 += orig6
  uint32323232 z7 += orig7
  in4 = z4
  in5 = z5
  in6 = z6
  in7 = z7
  z4 <<<= 96
  z5 <<<= 96
  z6 <<<= 96
  z7 <<<= 96
  (uint32) in4 ^= *(uint32 *) (m + 16)
  (uint32) in5 ^= *(uint32 *) (m + 20)
  (uint32) in6 ^= *(uint32 *) (m + 24)
  (uint32) in7 ^= *(uint32 *) (m + 28)
  *(uint32 *) (out + 16) = in4
  *(uint32 *) (out + 20) = in5
  *(uint32 *) (out + 24) = in6
  *(uint32 *) (out + 28) = in7
  in4 = z4
  in5 = z5
  in6 = z6
  in7 = z7
  z4 <<<= 96
  z5 <<<= 96
  z6 <<<= 96
  z7 <<<= 96
  (uint32) in4 ^= *(uint32 *) (m + 80)
  (uint32) in5 ^= *(uint32 *) (m + 84)
  (uint32) in6 ^= *(uint32 *) (m + 88)
  (uint32) in7 ^= *(uint32 *) (m + 92)
  *(uint32 *) (out + 80) = in4
  *(uint32 *) (out + 84) = in5
  *(uint32 *) (out + 88) = in6
  *(uint32 *) (out + 92) = in7
  in4 = z4
  in5 = z5
  in6 = z6
  in7 = z7
  z4 <<<= 96
  z5 <<<= 96
  z6 <<<= 96
  z7 <<<= 96
  (uint32) in4 ^= *(uint32 *) (m + 144)
  (uint32) in5 ^= *(uint32 *) (m + 148)
  (uint32) in6 ^= *(uint32 *) (m + 152)
  (uint32) in7 ^= *(uint32 *) (m + 156)
  *(uint32 *) (out + 144) = in4
  *(uint32 *) (out + 148) = in5
  *(uint32 *) (out + 152) = in6
  *(uint32 *) (out + 156) = in7
  in4 = z4
  in5 = z5
  in6 = z6
  in7 = z7
  (uint32) in4 ^= *(uint32 *) (m + 208)
  (uint32) in5 ^= *(uint32 *) (m + 212)
  (uint32) in6 ^= *(uint32 *) (m + 216)
  (uint32) in7 ^= *(uint32 *) (m + 220)
  *(uint32 *) (out + 208) = in4
  *(uint32 *) (out + 212) = in5
  *(uint32 *) (out + 216) = in6
  *(uint32 *) (out + 220) = in7

  uint32323232 z8 += orig8
  uint32323232 z9 += orig9
  uint32323232 z10 += orig10
  uint32323232 z11 += orig11
  in8 = z8
  in9 = z9
  in10 = z10
  in11 = z11
  z8 <<<= 96
  z9 <<<= 96
  z10 <<<= 96
  z11 <<<= 96
  (uint32) in8 ^= *(uint32 *) (m + 32)
  (uint32) in9 ^= *(uint32 *) (m + 36)
  (uint32) in10 ^= *(uint32 *) (m + 40)
  (uint32) in11 ^= *(uint32 *) (m + 44)
  *(uint32 *) (out + 32) = in8
  *(uint32 *) (out + 36) = in9
  *(uint32 *) (out + 40) = in10
  *(uint32 *) (out + 44) = in11
  in8 = z8
  in9 = z9
  in10 = z10
  in11 = z11
  z8 <<<= 96
  z9 <<<= 96
  z10 <<<= 96
  z11 <<<= 96
  (uint32) in8 ^= *(uint32 *) (m + 96)
  (uint32) in9 ^= *(uint32 *) (m + 100)
  (uint32) in10 ^= *(uint32 *) (m + 104)
  (uint32) in11 ^= *(uint32 *) (m + 108)
  *(uint32 *) (out + 96) = in8
  *(uint32 *) (out + 100) = in9
  *(uint32 *) (out + 104) = in10
  *(uint32 *) (out + 108) = in11
  in8 = z8
  in9 = z9
  in10 = z10
  in11 = z11
  z8 <<<= 96
  z9 <<<= 96
  z10 <<<= 96
  z11 <<<= 96
  (uint32) in8 ^= *(uint32 *) (m + 160)
  (uint32) in9 ^= *(uint32 *) (m + 164)
  (uint32) in10 ^= *(uint32 *) (m + 168)
  (uint32) in11 ^= *(uint32 *) (m + 172)
  *(uint32 *) (out + 160) = in8
  *(uint32 *) (out + 164) = in9
  *(uint32 *) (out + 168) = in10
  *(uint32 *) (out + 172) = in11
  in8 = z8
  in9 = z9
  in10 = z10
  in11 = z11
  (uint32) in8 ^= *(uint32 *) (m + 224)
  (uint32) in9 ^= *(uint32 *) (m + 228)
  (uint32) in10 ^= *(uint32 *) (m + 232)
  (uint32) in11 ^= *(uint32 *) (m + 236)
  *(uint32 *) (out + 224) = in8
  *(uint32 *) (out + 228) = in9
  *(uint32 *) (out + 232) = in10
  *(uint32 *) (out + 236) = in11

  uint32323232 z12 += orig12
  uint32323232 z13 += orig13
  uint32323232 z14 += orig14
  uint32323232 z15 += orig15
  in12 = z12
  in13 = z13
  in14 = z14
  in15 = z15
  z12 <<<= 96
  z13 <<<= 96
  z14 <<<= 96
  z15 <<<= 96
  (uint32) in12 ^= *(uint32 *) (m + 48)
  (uint32) in13 ^= *(uint32 *) (m + 52)
  (uint32) in14 ^= *(uint32 *) (m + 56)
  (uint32) in15 ^= *(uint32 *) (m + 60)
  *(uint32 *) (out + 48) = in12
  *(uint32 *) (out + 52) = in13
  *(uint32 *) (out + 56) = in14
  *(uint32 *) (out + 60) = in15
  in12 = z12
  in13 = z13
  in14 = z14
  in15 = z15
  z12 <<<= 96
  z13 <<<= 96
  z14 <<<= 96
  z15 <<<= 96
  (uint32) in12 ^= *(uint32 *) (m + 112)
  (uint32) in13 ^= *(uint32 *) (m + 116)
  (uint32) in14 ^= *(uint32 *) (m + 120)
  (uint32) in15 ^= *(uint32 *) (m + 124)
  *(uint32 *) (out + 112) = in12
  *(uint32 *) (out + 116) = in13
  *(uint32 *) (out + 120) = in14
  *(uint32 *) (out + 124) = in15
  in12 = z12
  in13 = z13
  in14 = z14
  in15 = z15
  z12 <<<= 96
  z13 <<<= 96
  z14 <<<= 96
  z15 <<<= 96
  (uint32) in12 ^= *(uint32 *) (m + 176)
  (uint32) in13 ^= *(uint32 *) (m + 180)
  (uint32) in14 ^= *(uint32 *) (m + 184)
  (uint32) in15 ^= *(uint32 *) (m + 188)
  *(uint32 *) (out + 176) = in12
  *(uint32 *) (out + 180) = in13
  *(uint32 *) (out + 184) = in14
  *(uint32 *) (out + 188) = in15
  in12 = z12
  in13 = z13
  in14 = z14
  in15 = z15
  (uint32) in12 ^= *(uint32 *) (m + 240)
  (uint32) in13 ^= *(uint32 *) (m + 244)
  (uint32) in14 ^= *(uint32 *) (m + 248)
  (uint32) in15 ^= *(uint32 *) (m + 252)
  *(uint32 *) (out + 240) = in12
  *(uint32 *) (out + 244) = in13
  *(uint32 *) (out + 248) = in14
  *(uint32 *) (out + 252) = in15

  bytes = bytes_backup

  bytes -= 256
  m += 256
  out += 256
                           unsigned<? bytes - 256
  goto bytesatleast256 if !unsigned<

                unsigned>? bytes - 0
  goto done if !unsigned>

bytesbetween1and255:

                  unsigned<? bytes - 64
  goto nocopy if !unsigned<

    ctarget = out

    out = &tmp
    i = bytes
    while (i) { *out++ = *m++; --i }
    out = &tmp
    m = &tmp

  nocopy:

  bytes_backup = bytes



diag0 = *(int128 *) (x + 0)
diag1 = *(int128 *) (x + 16)
diag2 = *(int128 *) (x + 32)
diag3 = *(int128 *) (x + 48)


                    a0 = diag1
i = 20

mainloop2:

uint32323232        a0 += diag0
                                a1 = diag0
                    b0 = a0
uint32323232        a0 <<= 7
uint32323232        b0 >>= 25
                diag3 ^= a0

                diag3 ^= b0

uint32323232                        a1 += diag3
                                                a2 = diag3
                                    b1 = a1
uint32323232                        a1 <<= 9
uint32323232                        b1 >>= 23
                                diag2 ^= a1
                diag3 <<<= 32
                                diag2 ^= b1

uint32323232                                        a2 += diag2
                                                                a3 = diag2
                                                    b2 = a2
uint32323232                                        a2 <<= 13
uint32323232                                        b2 >>= 19
                                                diag1 ^= a2
                                diag2 <<<= 64
                                                diag1 ^= b2

uint32323232                                                        a3 += diag1
                a4 = diag3
                                                                    b3 = a3
uint32323232                                                        a3 <<= 18
uint32323232                                                        b3 >>= 14
                                                                diag0 ^= a3
                                                diag1 <<<= 96
                                                                diag0 ^= b3

uint32323232        a4 += diag0
                                a5 = diag0
                    b4 = a4
uint32323232        a4 <<= 7
uint32323232        b4 >>= 25
                diag1 ^= a4

                diag1 ^= b4

uint32323232                        a5 += diag1
                                                a6 = diag1
                                    b5 = a5
uint32323232                        a5 <<= 9
uint32323232                        b5 >>= 23
                                diag2 ^= a5
                diag1 <<<= 32
                                diag2 ^= b5

uint32323232                                        a6 += diag2
                                                                a7 = diag2
                                                    b6 = a6
uint32323232                                        a6 <<= 13
uint32323232                                        b6 >>= 19
                                                diag3 ^= a6
                                diag2 <<<= 64
                                                diag3 ^= b6

uint32323232                                                        a7 += diag3
                a0 = diag1
                                                                    b7 = a7
uint32323232                                                        a7 <<= 18
uint32323232                                                        b7 >>= 14
                                                                diag0 ^= a7
                                                diag3 <<<= 96
                                                                diag0 ^= b7


uint32323232        a0 += diag0
                                a1 = diag0
                    b0 = a0
uint32323232        a0 <<= 7
uint32323232        b0 >>= 25
                diag3 ^= a0

                diag3 ^= b0

uint32323232                        a1 += diag3
                                                a2 = diag3
                                    b1 = a1
uint32323232                        a1 <<= 9
uint32323232                        b1 >>= 23
                                diag2 ^= a1
                diag3 <<<= 32
                                diag2 ^= b1

uint32323232                                        a2 += diag2
                                                                a3 = diag2
                                                    b2 = a2
uint32323232                                        a2 <<= 13
uint32323232                                        b2 >>= 19
                                                diag1 ^= a2
                                diag2 <<<= 64
                                                diag1 ^= b2

uint32323232                                                        a3 += diag1
                a4 = diag3
                                                                    b3 = a3
uint32323232                                                        a3 <<= 18
uint32323232                                                        b3 >>= 14
                                                                diag0 ^= a3
                                                diag1 <<<= 96
                                                                diag0 ^= b3

uint32323232        a4 += diag0
                                a5 = diag0
                    b4 = a4
uint32323232        a4 <<= 7
uint32323232        b4 >>= 25
                diag1 ^= a4

                diag1 ^= b4

uint32323232                        a5 += diag1
                                                a6 = diag1
                                    b5 = a5
uint32323232                        a5 <<= 9
uint32323232                        b5 >>= 23
                                diag2 ^= a5
                diag1 <<<= 32
                                diag2 ^= b5

uint32323232                                        a6 += diag2
                                                                a7 = diag2
                                                    b6 = a6
uint32323232                                        a6 <<= 13
uint32323232                                        b6 >>= 19
                                                diag3 ^= a6
                                diag2 <<<= 64
                                                diag3 ^= b6
                 unsigned>? i -= 4
uint32323232                                                        a7 += diag3
                a0 = diag1
                                                                    b7 = a7
uint32323232                                                        a7 <<= 18
                b0 = 0
uint32323232                                                        b7 >>= 14
                                                                diag0 ^= a7
                                                diag3 <<<= 96
                                                                diag0 ^= b7
goto mainloop2 if unsigned>


uint32323232 diag0 += *(int128 *) (x + 0)
uint32323232 diag1 += *(int128 *) (x + 16)
uint32323232 diag2 += *(int128 *) (x + 32)
uint32323232 diag3 += *(int128 *) (x + 48)


in0 = diag0
in12 = diag1
in8 = diag2
in4 = diag3
diag0 <<<= 96
diag1 <<<= 96
diag2 <<<= 96
diag3 <<<= 96
(uint32) in0 ^= *(uint32 *) (m + 0)
(uint32) in12 ^= *(uint32 *) (m + 48)
(uint32) in8 ^= *(uint32 *) (m + 32)
(uint32) in4 ^= *(uint32 *) (m + 16)
*(uint32 *) (out + 0) = in0
*(uint32 *) (out + 48) = in12
*(uint32 *) (out + 32) = in8
*(uint32 *) (out + 16) = in4

in5 = diag0
in1 = diag1
in13 = diag2
in9 = diag3
diag0 <<<= 96
diag1 <<<= 96
diag2 <<<= 96
diag3 <<<= 96
(uint32) in5 ^= *(uint32 *) (m + 20)
(uint32) in1 ^= *(uint32 *) (m + 4)
(uint32) in13 ^= *(uint32 *) (m + 52)
(uint32) in9 ^= *(uint32 *) (m + 36)
*(uint32 *) (out + 20) = in5
*(uint32 *) (out + 4) = in1
*(uint32 *) (out + 52) = in13
*(uint32 *) (out + 36) = in9

in10 = diag0
in6 = diag1
in2 = diag2
in14 = diag3
diag0 <<<= 96
diag1 <<<= 96
diag2 <<<= 96
diag3 <<<= 96
(uint32) in10 ^= *(uint32 *) (m + 40)
(uint32) in6 ^= *(uint32 *) (m + 24)
(uint32) in2 ^= *(uint32 *) (m + 8)
(uint32) in14 ^= *(uint32 *) (m + 56)
*(uint32 *) (out + 40) = in10
*(uint32 *) (out + 24) = in6
*(uint32 *) (out + 8) = in2
*(uint32 *) (out + 56) = in14

in15 = diag0
in11 = diag1
in7 = diag2
in3 = diag3
(uint32) in15 ^= *(uint32 *) (m + 60)
(uint32) in11 ^= *(uint32 *) (m + 44)
(uint32) in7 ^= *(uint32 *) (m + 28)
(uint32) in3 ^= *(uint32 *) (m + 12)
*(uint32 *) (out + 60) = in15
*(uint32 *) (out + 44) = in11
*(uint32 *) (out + 28) = in7
*(uint32 *) (out + 12) = in3


  bytes = bytes_backup

  in8 = *(uint32 *) (x + 32)
  in9 = *(uint32 *) (x + 52)
  in8 += 1
  in9 <<= 32
  in8 += in9
  in9 = in8
  (uint64) in9 >>= 32
  *(uint32 *) (x + 32) = in8
  *(uint32 *) (x + 52) = in9

                         unsigned>? unsigned<? bytes - 64
  goto bytesatleast65 if unsigned>

    goto bytesatleast64 if !unsigned<
      m = out
      out = ctarget
      i = bytes
      while (i) { *out++ = *m++; --i }
    bytesatleast64:
    done:

    leave

  bytesatleast65:

  bytes -= 64
  out += 64
  m += 64
goto bytesbetween1and255


enter crypto_stream_salsa20_e_amd64_xmm6_ECRYPT_init
leave


enter crypto_stream_salsa20_e_amd64_xmm6_ECRYPT_keysetup

  k = arg2
  kbits = arg3
  x = arg1

# situation at beginning of first round:
# diag0: x0 x5 x10 x15
# diag1: x12 x1 x6 x11
# diag2: x8 x13 x2 x7
# diag3: x4 x9 x14 x3

  in1 = *(uint32 *) (k + 0)
  in2 = *(uint32 *) (k + 4)
  in3 = *(uint32 *) (k + 8)
  in4 = *(uint32 *) (k + 12)
  *(uint32 *) (x + 20) = in1
  *(uint32 *) (x + 40) = in2
  *(uint32 *) (x + 60) = in3
  *(uint32 *) (x + 48) = in4

                   unsigned<? kbits - 256
  goto kbits128 if unsigned<

  kbits256:

    in11 = *(uint32 *) (k + 16)
    in12 = *(uint32 *) (k + 20)
    in13 = *(uint32 *) (k + 24)
    in14 = *(uint32 *) (k + 28)
    *(uint32 *) (x + 28) = in11
    *(uint32 *) (x + 16) = in12
    *(uint32 *) (x + 36) = in13
    *(uint32 *) (x + 56) = in14

    in0 = 1634760805
    in5 = 857760878
    in10 = 2036477234
    in15 = 1797285236
    *(uint32 *) (x + 0) = in0
    *(uint32 *) (x + 4) = in5
    *(uint32 *) (x + 8) = in10
    *(uint32 *) (x + 12) = in15

  goto keysetupdone

  kbits128:

    in11 = *(uint32 *) (k + 0)
    in12 = *(uint32 *) (k + 4)
    in13 = *(uint32 *) (k + 8)
    in14 = *(uint32 *) (k + 12)
    *(uint32 *) (x + 28) = in11
    *(uint32 *) (x + 16) = in12
    *(uint32 *) (x + 36) = in13
    *(uint32 *) (x + 56) = in14

    in0 = 1634760805
    in5 = 824206446
    in10 = 2036477238
    in15 = 1797285236
    *(uint32 *) (x + 0) = in0
    *(uint32 *) (x + 4) = in5
    *(uint32 *) (x + 8) = in10
    *(uint32 *) (x + 12) = in15

  keysetupdone:

leave


enter crypto_stream_salsa20_e_amd64_xmm6_ECRYPT_ivsetup

  iv = arg2
  x = arg1

# situation at beginning of first round:
# diag0: x0 x5 x10 x15
# diag1: x12 x1 x6 x11
# diag2: x8 x13 x2 x7
# diag3: x4 x9 x14 x3

  in6 = *(uint32 *) (iv + 0)
  in7 = *(uint32 *) (iv + 4)
  in8 = 0
  in9 = 0
  *(uint32 *) (x + 24) = in6
  *(uint32 *) (x + 44) = in7
  *(uint32 *) (x + 32) = in8
  *(uint32 *) (x + 52) = in9

leave
