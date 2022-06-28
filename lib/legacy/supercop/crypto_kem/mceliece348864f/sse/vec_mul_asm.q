reg128 b0
reg128 b1
reg128 b2
reg128 b3
reg128 b4
reg128 b5
reg128 b6
reg128 b7
reg128 b8
reg128 b9
reg128 b10
reg128 b11
reg128 a0
reg128 a1
reg128 a2
reg128 a3
reg128 a4
reg128 a5
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
int64 h0
int64 h1
int64 h2
int64 h3
int64 h4
int64 h5
int64 h6
int64 h7
int64 h8
int64 h9
int64 h10
int64 h11
int64 h12
int64 h13
int64 h14
int64 h15
int64 h16
int64 h17
int64 h18
int64 h19
int64 h20
int64 h21
int64 h22
stack2432 buf
int64 ptr
int64 tmp
stack64 r11_stack
stack64 r12_stack
stack64 r13_stack
stack64 r14_stack
stack64 r15_stack
stack64 rbx_stack
stack64 rbp_stack

enter vec_mul_asm

r11_stack = caller_r11
r12_stack = caller_r12
r13_stack = caller_r13
r14_stack = caller_r14
r15_stack = caller_r15
rbx_stack = caller_rbx
ptr = &buf
tmp = input_3
tmp *= 11
input_2 += tmp
b11 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
a5[0] = mem64[ input_1 + 40 ]
a5[1] = mem64[ input_1 + 88 ]
r16 = b11 & a5
mem128[ ptr + 256 ] = r16
a4[0] = mem64[ input_1 + 32 ]
a4[1] = mem64[ input_1 + 80 ]
r15 = b11 & a4
a3[0] = mem64[ input_1 + 24 ]
a3[1] = mem64[ input_1 + 72 ]
r14 = b11 & a3
a2[0] = mem64[ input_1 + 16 ]
a2[1] = mem64[ input_1 + 64 ]
r13 = b11 & a2
a1[0] = mem64[ input_1 + 8 ]
a1[1] = mem64[ input_1 + 56 ]
r12 = b11 & a1
a0[0] = mem64[ input_1 + 0 ]
a0[1] = mem64[ input_1 + 48 ]
r11 = b11 & a0
b10 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b10 & a5
r15 ^= r
mem128[ ptr + 240 ] = r15
r = b10 & a4
r14 ^= r
r = b10 & a3
r13 ^= r
r = b10 & a2
r12 ^= r
r = b10 & a1
r11 ^= r
r10 = b10 & a0
b9 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b9 & a5
r14 ^= r
mem128[ ptr + 224 ] = r14
r = b9 & a4
r13 ^= r
r = b9 & a3
r12 ^= r
r = b9 & a2
r11 ^= r
r = b9 & a1
r10 ^= r
r9 = b9 & a0
b8 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b8 & a5
r13 ^= r
mem128[ ptr + 208 ] = r13
r = b8 & a4
r12 ^= r
r = b8 & a3
r11 ^= r
r = b8 & a2
r10 ^= r
r = b8 & a1
r9 ^= r
r8 = b8 & a0
b7 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b7 & a5
r12 ^= r
mem128[ ptr + 192 ] = r12
r = b7 & a4
r11 ^= r
r = b7 & a3
r10 ^= r
r = b7 & a2
r9 ^= r
r = b7 & a1
r8 ^= r
r7 = b7 & a0
b6 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b6 & a5
r11 ^= r
mem128[ ptr + 176 ] = r11
r = b6 & a4
r10 ^= r
r = b6 & a3
r9 ^= r
r = b6 & a2
r8 ^= r
r = b6 & a1
r7 ^= r
r6 = b6 & a0
b5 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b5 & a5
r10 ^= r
mem128[ ptr + 160 ] = r10
r = b5 & a4
r9 ^= r
r = b5 & a3
r8 ^= r
r = b5 & a2
r7 ^= r
r = b5 & a1
r6 ^= r
r5 = b5 & a0
b4 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b4 & a5
r9 ^= r
mem128[ ptr + 144 ] = r9
r = b4 & a4
r8 ^= r
r = b4 & a3
r7 ^= r
r = b4 & a2
r6 ^= r
r = b4 & a1
r5 ^= r
r4 = b4 & a0
b3 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b3 & a5
r8 ^= r
mem128[ ptr + 128 ] = r8
r = b3 & a4
r7 ^= r
r = b3 & a3
r6 ^= r
r = b3 & a2
r5 ^= r
r = b3 & a1
r4 ^= r
r3 = b3 & a0
b2 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b2 & a5
r7 ^= r
mem128[ ptr + 112 ] = r7
r = b2 & a4
r6 ^= r
r = b2 & a3
r5 ^= r
r = b2 & a2
r4 ^= r
r = b2 & a1
r3 ^= r
r2 = b2 & a0
b1 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b1 & a5
r6 ^= r
mem128[ ptr + 96 ] = r6
r = b1 & a4
r5 ^= r
r = b1 & a3
r4 ^= r
r = b1 & a2
r3 ^= r
r = b1 & a1
r2 ^= r
r1 = b1 & a0
b0 = mem64[ input_2 + 0 ] x2
input_2 -= input_3
r = b0 & a5
r5 ^= r
mem128[ ptr + 80 ] = r5
r = b0 & a4
r4 ^= r
r = b0 & a3
r3 ^= r
r = b0 & a2
r2 ^= r
r = b0 & a1
r1 ^= r
r0 = b0 & a0
mem128[ ptr + 64 ] = r4
mem128[ ptr + 48 ] = r3
mem128[ ptr + 32 ] = r2
mem128[ ptr + 16 ] = r1
mem128[ ptr + 0 ] = r0
h22 = mem64[ ptr + 264 ]
h13 = h22
h10 = h22
h21 = mem64[ ptr + 248 ]
h12 = h21
h9 = h21
h20 = mem64[ ptr + 232 ]
h11 = h20
h8 = h20
h19 = mem64[ ptr + 216 ]
h10 ^= h19
h7 = h19
h18 = mem64[ ptr + 200 ]
h9 ^= h18
h6 = h18
h17 = mem64[ ptr + 184 ]
h8 ^= h17
h5 = h17
h16 = mem64[ ptr + 168 ]
h16 ^= *(uint64 *) ( ptr + 256 )
h7 ^= h16
h4 = h16
h15 = mem64[ ptr + 152 ]
h15 ^= *(uint64 *) ( ptr + 240 )
h6 ^= h15
h3 = h15
h14 = mem64[ ptr + 136 ]
h14 ^= *(uint64 *) ( ptr + 224 )
h5 ^= h14
h2 = h14
h13 ^= *(uint64 *) ( ptr + 120 )
h13 ^= *(uint64 *) ( ptr + 208 )
h4 ^= h13
h1 = h13
h12 ^= *(uint64 *) ( ptr + 104 )
h12 ^= *(uint64 *) ( ptr + 192 )
h3 ^= h12
h0 = h12
h11 ^= *(uint64 *) ( ptr + 176 )
h11 ^= *(uint64 *) ( ptr + 88 )
mem64[ input_0 + 88 ] = h11
h10 ^= *(uint64 *) ( ptr + 160 )
h10 ^= *(uint64 *) ( ptr + 72 )
mem64[ input_0 + 80 ] = h10
h9 ^= *(uint64 *) ( ptr + 144 )
h9 ^= *(uint64 *) ( ptr + 56 )
mem64[ input_0 + 72 ] = h9
h8 ^= *(uint64 *) ( ptr + 128 )
h8 ^= *(uint64 *) ( ptr + 40 )
mem64[ input_0 + 64 ] = h8
h7 ^= *(uint64 *) ( ptr + 112 )
h7 ^= *(uint64 *) ( ptr + 24 )
mem64[ input_0 + 56 ] = h7
h6 ^= *(uint64 *) ( ptr + 96 )
h6 ^= *(uint64 *) ( ptr + 8 )
mem64[ input_0 + 48 ] = h6
h5 ^= *(uint64 *) ( ptr + 80 )
mem64[ input_0 + 40 ] = h5
h4 ^= *(uint64 *) ( ptr + 64 )
mem64[ input_0 + 32 ] = h4
h3 ^= *(uint64 *) ( ptr + 48 )
mem64[ input_0 + 24 ] = h3
h2 ^= *(uint64 *) ( ptr + 32 )
mem64[ input_0 + 16 ] = h2
h1 ^= *(uint64 *) ( ptr + 16 )
mem64[ input_0 + 8 ] = h1
h0 ^= *(uint64 *) ( ptr + 0 )
mem64[ input_0 + 0 ] = h0
caller_r11 = r11_stack
caller_r12 = r12_stack
caller_r13 = r13_stack
caller_r14 = r14_stack
caller_r15 = r15_stack
caller_rbx = rbx_stack
return
