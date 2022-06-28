reg128 a0
reg128 a1
reg128 a2
reg128 a3
reg128 a4
reg128 a5
reg128 a6
reg128 a7
reg128 a8
reg128 a9
reg128 a10
reg128 a11
reg128 b0
reg128 b1
reg128 r0
reg128 r1
reg128 r2
reg128 r3
reg128 r4
reg128 r5
reg128 r6
reg128 r7
reg128 r8
reg128 r9
reg128 r10
reg128 r11
reg128 r12
reg128 r13
reg128 r14
reg128 r15
reg128 r16
reg128 r17
reg128 r18
reg128 r19
reg128 r20
reg128 r21
reg128 r22
reg128 r
enter vec128_mul_asm
b0 = mem128[ input_2 + 0 ]
a11 = mem128[ input_1 + 176 ]
r11 = a11 & b0
r12 = a11 & mem128[input_2 + 16]
r13 = a11 & mem128[input_2 + 32]
r14 = a11 & mem128[input_2 + 48]
r15 = a11 & mem128[input_2 + 64]
r16 = a11 & mem128[input_2 + 80]
r17 = a11 & mem128[input_2 + 96]
r18 = a11 & mem128[input_2 + 112]
r19 = a11 & mem128[input_2 + 128]
r20 = a11 & mem128[input_2 + 144]
r21 = a11 & mem128[input_2 + 160]
r22 = a11 & mem128[input_2 + 176]
r13 ^= r22
r10 = r22
a10 = mem128[ input_1 + 160 ]
r = a10 & b0
r10 ^= r
r = a10 & mem128[input_2 + 16]
r11 ^= r
r = a10 & mem128[input_2 + 32]
r12 ^= r
r = a10 & mem128[input_2 + 48]
r13 ^= r
r = a10 & mem128[input_2 + 64]
r14 ^= r
r = a10 & mem128[input_2 + 80]
r15 ^= r
r = a10 & mem128[input_2 + 96]
r16 ^= r
r = a10 & mem128[input_2 + 112]
r17 ^= r
r = a10 & mem128[input_2 + 128]
r18 ^= r
r = a10 & mem128[input_2 + 144]
r19 ^= r
r = a10 & mem128[input_2 + 160]
r20 ^= r
r = a10 & mem128[input_2 + 176]
r21 ^= r
r12 ^= r21
r9 = r21
a9 = mem128[ input_1 + 144 ]
r = a9 & b0
r9 ^= r
r = a9 & mem128[input_2 + 16]
r10 ^= r
r = a9 & mem128[input_2 + 32]
r11 ^= r
r = a9 & mem128[input_2 + 48]
r12 ^= r
r = a9 & mem128[input_2 + 64]
r13 ^= r
r = a9 & mem128[input_2 + 80]
r14 ^= r
r = a9 & mem128[input_2 + 96]
r15 ^= r
r = a9 & mem128[input_2 + 112]
r16 ^= r
r = a9 & mem128[input_2 + 128]
r17 ^= r
r = a9 & mem128[input_2 + 144]
r18 ^= r
r = a9 & mem128[input_2 + 160]
r19 ^= r
r = a9 & mem128[input_2 + 176]
r20 ^= r
r11 ^= r20
r8 = r20
a8 = mem128[ input_1 + 128 ]
r = a8 & b0
r8 ^= r
r = a8 & mem128[input_2 + 16]
r9 ^= r
r = a8 & mem128[input_2 + 32]
r10 ^= r
r = a8 & mem128[input_2 + 48]
r11 ^= r
r = a8 & mem128[input_2 + 64]
r12 ^= r
r = a8 & mem128[input_2 + 80]
r13 ^= r
r = a8 & mem128[input_2 + 96]
r14 ^= r
r = a8 & mem128[input_2 + 112]
r15 ^= r
r = a8 & mem128[input_2 + 128]
r16 ^= r
r = a8 & mem128[input_2 + 144]
r17 ^= r
r = a8 & mem128[input_2 + 160]
r18 ^= r
r = a8 & mem128[input_2 + 176]
r19 ^= r
r10 ^= r19
r7 = r19
a7 = mem128[ input_1 + 112 ]
r = a7 & b0
r7 ^= r
r = a7 & mem128[input_2 + 16]
r8 ^= r
r = a7 & mem128[input_2 + 32]
r9 ^= r
r = a7 & mem128[input_2 + 48]
r10 ^= r
r = a7 & mem128[input_2 + 64]
r11 ^= r
r = a7 & mem128[input_2 + 80]
r12 ^= r
r = a7 & mem128[input_2 + 96]
r13 ^= r
r = a7 & mem128[input_2 + 112]
r14 ^= r
r = a7 & mem128[input_2 + 128]
r15 ^= r
r = a7 & mem128[input_2 + 144]
r16 ^= r
r = a7 & mem128[input_2 + 160]
r17 ^= r
r = a7 & mem128[input_2 + 176]
r18 ^= r
r9 ^= r18
r6 = r18
a6 = mem128[ input_1 + 96 ]
r = a6 & b0
r6 ^= r
r = a6 & mem128[input_2 + 16]
r7 ^= r
r = a6 & mem128[input_2 + 32]
r8 ^= r
r = a6 & mem128[input_2 + 48]
r9 ^= r
r = a6 & mem128[input_2 + 64]
r10 ^= r
r = a6 & mem128[input_2 + 80]
r11 ^= r
r = a6 & mem128[input_2 + 96]
r12 ^= r
r = a6 & mem128[input_2 + 112]
r13 ^= r
r = a6 & mem128[input_2 + 128]
r14 ^= r
r = a6 & mem128[input_2 + 144]
r15 ^= r
r = a6 & mem128[input_2 + 160]
r16 ^= r
r = a6 & mem128[input_2 + 176]
r17 ^= r
r8 ^= r17
r5 = r17
a5 = mem128[ input_1 + 80 ]
r = a5 & b0
r5 ^= r
r = a5 & mem128[input_2 + 16]
r6 ^= r
r = a5 & mem128[input_2 + 32]
r7 ^= r
r = a5 & mem128[input_2 + 48]
r8 ^= r
r = a5 & mem128[input_2 + 64]
r9 ^= r
r = a5 & mem128[input_2 + 80]
r10 ^= r
r = a5 & mem128[input_2 + 96]
r11 ^= r
r = a5 & mem128[input_2 + 112]
r12 ^= r
r = a5 & mem128[input_2 + 128]
r13 ^= r
r = a5 & mem128[input_2 + 144]
r14 ^= r
r = a5 & mem128[input_2 + 160]
r15 ^= r
r = a5 & mem128[input_2 + 176]
r16 ^= r
r7 ^= r16
r4 = r16
a4 = mem128[ input_1 + 64 ]
r = a4 & b0
r4 ^= r
r = a4 & mem128[input_2 + 16]
r5 ^= r
r = a4 & mem128[input_2 + 32]
r6 ^= r
r = a4 & mem128[input_2 + 48]
r7 ^= r
r = a4 & mem128[input_2 + 64]
r8 ^= r
r = a4 & mem128[input_2 + 80]
r9 ^= r
r = a4 & mem128[input_2 + 96]
r10 ^= r
r = a4 & mem128[input_2 + 112]
r11 ^= r
r = a4 & mem128[input_2 + 128]
r12 ^= r
r = a4 & mem128[input_2 + 144]
r13 ^= r
r = a4 & mem128[input_2 + 160]
r14 ^= r
r = a4 & mem128[input_2 + 176]
r15 ^= r
r6 ^= r15
r3 = r15
a3 = mem128[ input_1 + 48 ]
r = a3 & b0
r3 ^= r
r = a3 & mem128[input_2 + 16]
r4 ^= r
r = a3 & mem128[input_2 + 32]
r5 ^= r
r = a3 & mem128[input_2 + 48]
r6 ^= r
r = a3 & mem128[input_2 + 64]
r7 ^= r
r = a3 & mem128[input_2 + 80]
r8 ^= r
r = a3 & mem128[input_2 + 96]
r9 ^= r
r = a3 & mem128[input_2 + 112]
r10 ^= r
r = a3 & mem128[input_2 + 128]
r11 ^= r
r = a3 & mem128[input_2 + 144]
r12 ^= r
r = a3 & mem128[input_2 + 160]
r13 ^= r
r = a3 & mem128[input_2 + 176]
r14 ^= r
r5 ^= r14
r2 = r14
a2 = mem128[ input_1 + 32 ]
r = a2 & b0
r2 ^= r
r = a2 & mem128[input_2 + 16]
r3 ^= r
r = a2 & mem128[input_2 + 32]
r4 ^= r
r = a2 & mem128[input_2 + 48]
r5 ^= r
r = a2 & mem128[input_2 + 64]
r6 ^= r
r = a2 & mem128[input_2 + 80]
r7 ^= r
r = a2 & mem128[input_2 + 96]
r8 ^= r
r = a2 & mem128[input_2 + 112]
r9 ^= r
r = a2 & mem128[input_2 + 128]
r10 ^= r
r = a2 & mem128[input_2 + 144]
r11 ^= r
r = a2 & mem128[input_2 + 160]
r12 ^= r
r = a2 & mem128[input_2 + 176]
r13 ^= r
r4 ^= r13
r1 = r13
a1 = mem128[ input_1 + 16 ]
r = a1 & b0
r1 ^= r
r = a1 & mem128[input_2 + 16]
r2 ^= r
r = a1 & mem128[input_2 + 32]
r3 ^= r
r = a1 & mem128[input_2 + 48]
r4 ^= r
r = a1 & mem128[input_2 + 64]
r5 ^= r
r = a1 & mem128[input_2 + 80]
r6 ^= r
r = a1 & mem128[input_2 + 96]
r7 ^= r
r = a1 & mem128[input_2 + 112]
r8 ^= r
r = a1 & mem128[input_2 + 128]
r9 ^= r
r = a1 & mem128[input_2 + 144]
r10 ^= r
r = a1 & mem128[input_2 + 160]
r11 ^= r
r = a1 & mem128[input_2 + 176]
r12 ^= r
r3 ^= r12
r0 = r12
a0 = mem128[ input_1 + 0 ]
r = a0 & b0
r0 ^= r
r = a0 & mem128[input_2 + 16]
r1 ^= r
r = a0 & mem128[input_2 + 32]
r2 ^= r
r = a0 & mem128[input_2 + 48]
r3 ^= r
r = a0 & mem128[input_2 + 64]
r4 ^= r
r = a0 & mem128[input_2 + 80]
r5 ^= r
r = a0 & mem128[input_2 + 96]
r6 ^= r
r = a0 & mem128[input_2 + 112]
r7 ^= r
r = a0 & mem128[input_2 + 128]
r8 ^= r
r = a0 & mem128[input_2 + 144]
r9 ^= r
r = a0 & mem128[input_2 + 160]
r10 ^= r
r = a0 & mem128[input_2 + 176]
r11 ^= r
mem128[ input_0 + 176 ] = r11
mem128[ input_0 + 160 ] = r10
mem128[ input_0 + 144 ] = r9
mem128[ input_0 + 128 ] = r8
mem128[ input_0 + 112 ] = r7
mem128[ input_0 + 96 ] = r6
mem128[ input_0 + 80 ] = r5
mem128[ input_0 + 64 ] = r4
mem128[ input_0 + 48 ] = r3
mem128[ input_0 + 32 ] = r2
mem128[ input_0 + 16 ] = r1
mem128[ input_0 + 0 ] = r0
return
