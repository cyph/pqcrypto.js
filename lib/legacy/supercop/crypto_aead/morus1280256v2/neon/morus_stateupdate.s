
# qhasm: int32 input_0

# qhasm: int32 input_1

# qhasm: int32 input_2

# qhasm: int32 input_3

# qhasm: stack32 input_4

# qhasm: stack32 input_5

# qhasm: stack32 input_6

# qhasm: stack32 input_7

# qhasm: int32 caller_r4

# qhasm: int32 caller_r5

# qhasm: int32 caller_r6

# qhasm: int32 caller_r7

# qhasm: int32 caller_r8

# qhasm: int32 caller_r9

# qhasm: int32 caller_r10

# qhasm: int32 caller_r11

# qhasm: int32 caller_r12

# qhasm: int32 caller_r14

# qhasm: reg128 caller_q4

# qhasm: reg128 caller_q5

# qhasm: reg128 caller_q6

# qhasm: reg128 caller_q7

# qhasm: startcode
.fpu neon
.text

# qhasm: reg128 msg_blk1

# qhasm: reg128 msg_blk2

# qhasm: reg128 s0_blk1

# qhasm: reg128 s0_blk2

# qhasm: reg128 s1_blk1

# qhasm: reg128 s1_blk2

# qhasm: reg128 s2_blk1

# qhasm: reg128 s2_blk2

# qhasm: reg128 s3_blk1

# qhasm: reg128 s3_blk2

# qhasm: reg128 s4_blk1

# qhasm: reg128 s4_blk2

# qhasm: stack128 q4_stack

# qhasm: stack128 q5_stack

# qhasm: enter morus_stateupdate
.align 4
.global _morus_stateupdate
.global morus_stateupdate
.type _morus_stateupdate STT_FUNC
.type morus_stateupdate STT_FUNC
_morus_stateupdate:
morus_stateupdate:
sub sp,sp,#32

# qhasm: q4_stack = q4_stack[0]caller_q4[1]
# asm 1: vstr <caller_q4=reg128#5%top,<q4_stack=stack128#1
# asm 2: vstr <caller_q4=d9,<q4_stack=[sp,#8]
vstr d9,[sp,#8]

# qhasm: q4_stack = caller_q4[0]q4_stack[1]
# asm 1: vstr <caller_q4=reg128#5%bot,<q4_stack=stack128#1
# asm 2: vstr <caller_q4=d8,<q4_stack=[sp,#0]
vstr d8,[sp,#0]

# qhasm: q5_stack = q5_stack[0]caller_q5[1]
# asm 1: vstr <caller_q5=reg128#6%top,<q5_stack=stack128#2
# asm 2: vstr <caller_q5=d11,<q5_stack=[sp,#24]
vstr d11,[sp,#24]

# qhasm: q5_stack = caller_q5[0]q5_stack[1]
# asm 1: vstr <caller_q5=reg128#6%bot,<q5_stack=stack128#2
# asm 2: vstr <caller_q5=d10,<q5_stack=[sp,#16]
vstr d10,[sp,#16]

# qhasm: s0_blk1 aligned= mem128[input_1]
# asm 1: vld1.8 {>s0_blk1=reg128#1%bot->s0_blk1=reg128#1%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s0_blk1=d0->s0_blk1=d1},[<input_1=r1,: 128]
vld1.8 {d0-d1},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s0_blk2 aligned= mem128[input_1]
# asm 1: vld1.8 {>s0_blk2=reg128#2%bot->s0_blk2=reg128#2%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s0_blk2=d2->s0_blk2=d3},[<input_1=r1,: 128]
vld1.8 {d2-d3},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s1_blk1 aligned= mem128[input_1]
# asm 1: vld1.8 {>s1_blk1=reg128#3%bot->s1_blk1=reg128#3%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s1_blk1=d4->s1_blk1=d5},[<input_1=r1,: 128]
vld1.8 {d4-d5},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s1_blk2 aligned= mem128[input_1]
# asm 1: vld1.8 {>s1_blk2=reg128#4%bot->s1_blk2=reg128#4%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s1_blk2=d6->s1_blk2=d7},[<input_1=r1,: 128]
vld1.8 {d6-d7},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s2_blk1 aligned= mem128[input_1]
# asm 1: vld1.8 {>s2_blk1=reg128#5%bot->s2_blk1=reg128#5%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s2_blk1=d8->s2_blk1=d9},[<input_1=r1,: 128]
vld1.8 {d8-d9},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s2_blk2 aligned= mem128[input_1]
# asm 1: vld1.8 {>s2_blk2=reg128#6%bot->s2_blk2=reg128#6%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s2_blk2=d10->s2_blk2=d11},[<input_1=r1,: 128]
vld1.8 {d10-d11},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s3_blk1 aligned= mem128[input_1]
# asm 1: vld1.8 {>s3_blk1=reg128#9%bot->s3_blk1=reg128#9%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s3_blk1=d16->s3_blk1=d17},[<input_1=r1,: 128]
vld1.8 {d16-d17},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s3_blk2 aligned= mem128[input_1]
# asm 1: vld1.8 {>s3_blk2=reg128#10%bot->s3_blk2=reg128#10%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s3_blk2=d18->s3_blk2=d19},[<input_1=r1,: 128]
vld1.8 {d18-d19},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s4_blk1 aligned= mem128[input_1]
# asm 1: vld1.8 {>s4_blk1=reg128#11%bot->s4_blk1=reg128#11%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s4_blk1=d20->s4_blk1=d21},[<input_1=r1,: 128]
vld1.8 {d20-d21},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s4_blk2 aligned= mem128[input_1]
# asm 1: vld1.8 {>s4_blk2=reg128#12%bot->s4_blk2=reg128#12%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>s4_blk2=d22->s4_blk2=d23},[<input_1=r1,: 128]
vld1.8 {d22-d23},[r1,: 128]

# qhasm: s0_blk1 ^= s3_blk1
# asm 1: veor >s0_blk1=reg128#1,<s0_blk1=reg128#1,<s3_blk1=reg128#9
# asm 2: veor >s0_blk1=q0,<s0_blk1=q0,<s3_blk1=q8
veor q0,q0,q8

# qhasm: msg_blk1 aligned= mem128[input_0]
# asm 1: vld1.8 {>msg_blk1=reg128#13%bot->msg_blk1=reg128#13%top},[<input_0=int32#1,: 128]
# asm 2: vld1.8 {>msg_blk1=d24->msg_blk1=d25},[<input_0=r0,: 128]
vld1.8 {d24-d25},[r0,: 128]

# qhasm: s0_blk2 ^= s3_blk2
# asm 1: veor >s0_blk2=reg128#2,<s0_blk2=reg128#2,<s3_blk2=reg128#10
# asm 2: veor >s0_blk2=q1,<s0_blk2=q1,<s3_blk2=q9
veor q1,q1,q9

# qhasm: caller_q4 = s1_blk1 & s2_blk1
# asm 1: vand >caller_q4=reg128#14,<s1_blk1=reg128#3,<s2_blk1=reg128#5
# asm 2: vand >caller_q4=q13,<s1_blk1=q2,<s2_blk1=q4
vand q13,q2,q4

# qhasm: input_0 += 16
# asm 1: add >input_0=int32#1,<input_0=int32#1,#16
# asm 2: add >input_0=r0,<input_0=r0,#16
add r0,r0,#16

# qhasm: caller_q5 = s1_blk2 & s2_blk2
# asm 1: vand >caller_q5=reg128#15,<s1_blk2=reg128#4,<s2_blk2=reg128#6
# asm 2: vand >caller_q5=q14,<s1_blk2=q3,<s2_blk2=q5
vand q14,q3,q5

# qhasm: msg_blk2 aligned= mem128[input_0]
# asm 1: vld1.8 {>msg_blk2=reg128#16%bot->msg_blk2=reg128#16%top},[<input_0=int32#1,: 128]
# asm 2: vld1.8 {>msg_blk2=d30->msg_blk2=d31},[<input_0=r0,: 128]
vld1.8 {d30-d31},[r0,: 128]

# qhasm: s0_blk1 ^= caller_q4
# asm 1: veor >s0_blk1=reg128#1,<s0_blk1=reg128#1,<caller_q4=reg128#14
# asm 2: veor >s0_blk1=q0,<s0_blk1=q0,<caller_q4=q13
veor q0,q0,q13

# qhasm: s0_blk2 ^= caller_q5
# asm 1: veor >s0_blk2=reg128#2,<s0_blk2=reg128#2,<caller_q5=reg128#15
# asm 2: veor >s0_blk2=q1,<s0_blk2=q1,<caller_q5=q14
veor q1,q1,q14

# qhasm: 2x caller_q4 = s0_blk1 << 13
# asm 1: vshl.i64 >caller_q4=reg128#14,<s0_blk1=reg128#1,#13
# asm 2: vshl.i64 >caller_q4=q13,<s0_blk1=q0,#13
vshl.i64 q13,q0,#13

# qhasm: 2x s0_blk1 unsigned>>= 51
# asm 1: vshr.u64 >s0_blk1=reg128#1,<s0_blk1=reg128#1,#51
# asm 2: vshr.u64 >s0_blk1=q0,<s0_blk1=q0,#51
vshr.u64 q0,q0,#51

# qhasm: s0_blk1 ^= caller_q4
# asm 1: veor >s0_blk1=reg128#1,<s0_blk1=reg128#1,<caller_q4=reg128#14
# asm 2: veor >s0_blk1=q0,<s0_blk1=q0,<caller_q4=q13
veor q0,q0,q13

# qhasm: caller_q4 = s3_blk2[2,3]s3_blk1[0,1]
# asm 1: vext.32 >caller_q4=reg128#14,<s3_blk2=reg128#10,<s3_blk1=reg128#9,#2
# asm 2: vext.32 >caller_q4=q13,<s3_blk2=q9,<s3_blk1=q8,#2
vext.32 q13,q9,q8,#2

# qhasm: 2x caller_q5 = s0_blk2 << 13
# asm 1: vshl.i64 >caller_q5=reg128#15,<s0_blk2=reg128#2,#13
# asm 2: vshl.i64 >caller_q5=q14,<s0_blk2=q1,#13
vshl.i64 q14,q1,#13

# qhasm: 2x s0_blk2 unsigned>>= 51
# asm 1: vshr.u64 >s0_blk2=reg128#2,<s0_blk2=reg128#2,#51
# asm 2: vshr.u64 >s0_blk2=q1,<s0_blk2=q1,#51
vshr.u64 q1,q1,#51

# qhasm: s0_blk2 ^= caller_q5
# asm 1: veor >s0_blk2=reg128#2,<s0_blk2=reg128#2,<caller_q5=reg128#15
# asm 2: veor >s0_blk2=q1,<s0_blk2=q1,<caller_q5=q14
veor q1,q1,q14

# qhasm: s3_blk2 = s3_blk1[2,3]s3_blk2[0,1]
# asm 1: vext.32 >s3_blk2=reg128#9,<s3_blk1=reg128#9,<s3_blk2=reg128#10,#2
# asm 2: vext.32 >s3_blk2=q8,<s3_blk1=q8,<s3_blk2=q9,#2
vext.32 q8,q8,q9,#2

# qhasm: s3_blk1 = caller_q4
# asm 1: vmov >s3_blk1=reg128#10,<caller_q4=reg128#14
# asm 2: vmov >s3_blk1=q9,<caller_q4=q13
vmov q9,q13

# qhasm: s1_blk1 ^= s4_blk1
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<s4_blk1=reg128#11
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<s4_blk1=q10
veor q2,q2,q10

# qhasm: s1_blk2 ^= s4_blk2
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<s4_blk2=reg128#12
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<s4_blk2=q11
veor q3,q3,q11

# qhasm: s1_blk1 ^= msg_blk1
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<msg_blk1=reg128#13
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<msg_blk1=q12
veor q2,q2,q12

# qhasm: s1_blk2 ^= msg_blk2
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<msg_blk2=reg128#16
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<msg_blk2=q15
veor q3,q3,q15

# qhasm: caller_q4 = s2_blk1 & s3_blk1
# asm 1: vand >caller_q4=reg128#14,<s2_blk1=reg128#5,<s3_blk1=reg128#10
# asm 2: vand >caller_q4=q13,<s2_blk1=q4,<s3_blk1=q9
vand q13,q4,q9

# qhasm: caller_q5 = s2_blk2 & s3_blk2
# asm 1: vand >caller_q5=reg128#15,<s2_blk2=reg128#6,<s3_blk2=reg128#9
# asm 2: vand >caller_q5=q14,<s2_blk2=q5,<s3_blk2=q8
vand q14,q5,q8

# qhasm: s1_blk1 ^= caller_q4
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<caller_q4=reg128#14
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<caller_q4=q13
veor q2,q2,q13

# qhasm: s1_blk2 ^= caller_q5
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<caller_q5=reg128#15
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<caller_q5=q14
veor q3,q3,q14

# qhasm: 2x caller_q4 = s1_blk1 << 46
# asm 1: vshl.i64 >caller_q4=reg128#14,<s1_blk1=reg128#3,#46
# asm 2: vshl.i64 >caller_q4=q13,<s1_blk1=q2,#46
vshl.i64 q13,q2,#46

# qhasm: 2x s1_blk1 unsigned>>= 18
# asm 1: vshr.u64 >s1_blk1=reg128#3,<s1_blk1=reg128#3,#18
# asm 2: vshr.u64 >s1_blk1=q2,<s1_blk1=q2,#18
vshr.u64 q2,q2,#18

# qhasm: s1_blk1 ^= caller_q4
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<caller_q4=reg128#14
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<caller_q4=q13
veor q2,q2,q13

# qhasm: 2x caller_q5 = s1_blk2 << 46
# asm 1: vshl.i64 >caller_q5=reg128#14,<s1_blk2=reg128#4,#46
# asm 2: vshl.i64 >caller_q5=q13,<s1_blk2=q3,#46
vshl.i64 q13,q3,#46

# qhasm: 2x s1_blk2 unsigned>>= 18
# asm 1: vshr.u64 >s1_blk2=reg128#4,<s1_blk2=reg128#4,#18
# asm 2: vshr.u64 >s1_blk2=q3,<s1_blk2=q3,#18
vshr.u64 q3,q3,#18

# qhasm: s1_blk2 ^= caller_q5
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<caller_q5=reg128#14
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<caller_q5=q13
veor q3,q3,q13

# qhasm: s4_blk1 >=< s4_blk2
# asm 1: vswp <s4_blk1=reg128#11,<s4_blk2=reg128#12
# asm 2: vswp <s4_blk1=q10,<s4_blk2=q11
vswp q10,q11

# qhasm: s2_blk1 ^= s0_blk1
# asm 1: veor >s2_blk1=reg128#5,<s2_blk1=reg128#5,<s0_blk1=reg128#1
# asm 2: veor >s2_blk1=q4,<s2_blk1=q4,<s0_blk1=q0
veor q4,q4,q0

# qhasm: s2_blk2 ^= s0_blk2
# asm 1: veor >s2_blk2=reg128#6,<s2_blk2=reg128#6,<s0_blk2=reg128#2
# asm 2: veor >s2_blk2=q5,<s2_blk2=q5,<s0_blk2=q1
veor q5,q5,q1

# qhasm: s2_blk1 ^= msg_blk1
# asm 1: veor >s2_blk1=reg128#5,<s2_blk1=reg128#5,<msg_blk1=reg128#13
# asm 2: veor >s2_blk1=q4,<s2_blk1=q4,<msg_blk1=q12
veor q4,q4,q12

# qhasm: s2_blk2 ^= msg_blk2
# asm 1: veor >s2_blk2=reg128#6,<s2_blk2=reg128#6,<msg_blk2=reg128#16
# asm 2: veor >s2_blk2=q5,<s2_blk2=q5,<msg_blk2=q15
veor q5,q5,q15

# qhasm: caller_q4 = s3_blk1 & s4_blk1
# asm 1: vand >caller_q4=reg128#14,<s3_blk1=reg128#10,<s4_blk1=reg128#11
# asm 2: vand >caller_q4=q13,<s3_blk1=q9,<s4_blk1=q10
vand q13,q9,q10

# qhasm: caller_q5 = s3_blk2 & s4_blk2
# asm 1: vand >caller_q5=reg128#15,<s3_blk2=reg128#9,<s4_blk2=reg128#12
# asm 2: vand >caller_q5=q14,<s3_blk2=q8,<s4_blk2=q11
vand q14,q8,q11

# qhasm: s2_blk1 ^= caller_q4
# asm 1: veor >s2_blk1=reg128#5,<s2_blk1=reg128#5,<caller_q4=reg128#14
# asm 2: veor >s2_blk1=q4,<s2_blk1=q4,<caller_q4=q13
veor q4,q4,q13

# qhasm: s2_blk2 ^= caller_q5
# asm 1: veor >s2_blk2=reg128#6,<s2_blk2=reg128#6,<caller_q5=reg128#15
# asm 2: veor >s2_blk2=q5,<s2_blk2=q5,<caller_q5=q14
veor q5,q5,q14

# qhasm: 2x caller_q4 = s2_blk1 << 38
# asm 1: vshl.i64 >caller_q4=reg128#14,<s2_blk1=reg128#5,#38
# asm 2: vshl.i64 >caller_q4=q13,<s2_blk1=q4,#38
vshl.i64 q13,q4,#38

# qhasm: 2x s2_blk1 unsigned>>= 26
# asm 1: vshr.u64 >s2_blk1=reg128#5,<s2_blk1=reg128#5,#26
# asm 2: vshr.u64 >s2_blk1=q4,<s2_blk1=q4,#26
vshr.u64 q4,q4,#26

# qhasm: s2_blk1 ^= caller_q4
# asm 1: veor >s2_blk1=reg128#14,<s2_blk1=reg128#5,<caller_q4=reg128#14
# asm 2: veor >s2_blk1=q13,<s2_blk1=q4,<caller_q4=q13
veor q13,q4,q13

# qhasm: caller_q4 = s0_blk2[2,3]s0_blk1[0,1]
# asm 1: vext.32 >caller_q4=reg128#5,<s0_blk2=reg128#2,<s0_blk1=reg128#1,#2
# asm 2: vext.32 >caller_q4=q4,<s0_blk2=q1,<s0_blk1=q0,#2
vext.32 q4,q1,q0,#2

# qhasm: 2x caller_q5 = s2_blk2 << 38
# asm 1: vshl.i64 >caller_q5=reg128#15,<s2_blk2=reg128#6,#38
# asm 2: vshl.i64 >caller_q5=q14,<s2_blk2=q5,#38
vshl.i64 q14,q5,#38

# qhasm: 2x s2_blk2 unsigned>>= 26
# asm 1: vshr.u64 >s2_blk2=reg128#6,<s2_blk2=reg128#6,#26
# asm 2: vshr.u64 >s2_blk2=q5,<s2_blk2=q5,#26
vshr.u64 q5,q5,#26

# qhasm: s2_blk2 ^= caller_q5
# asm 1: veor >s2_blk2=reg128#15,<s2_blk2=reg128#6,<caller_q5=reg128#15
# asm 2: veor >s2_blk2=q14,<s2_blk2=q5,<caller_q5=q14
veor q14,q5,q14

# qhasm: s0_blk1 = s0_blk1[2,3]s0_blk2[0,1]
# asm 1: vext.32 >s0_blk1=reg128#1,<s0_blk1=reg128#1,<s0_blk2=reg128#2,#2
# asm 2: vext.32 >s0_blk1=q0,<s0_blk1=q0,<s0_blk2=q1,#2
vext.32 q0,q0,q1,#2

# qhasm: s0_blk2 = caller_q4
# asm 1: vmov >s0_blk2=reg128#2,<caller_q4=reg128#5
# asm 2: vmov >s0_blk2=q1,<caller_q4=q4
vmov q1,q4

# qhasm: s3_blk1 ^= s1_blk1
# asm 1: veor >s3_blk1=reg128#5,<s3_blk1=reg128#10,<s1_blk1=reg128#3
# asm 2: veor >s3_blk1=q4,<s3_blk1=q9,<s1_blk1=q2
veor q4,q9,q2

# qhasm: s3_blk2 ^= s1_blk2
# asm 1: veor >s3_blk2=reg128#6,<s3_blk2=reg128#9,<s1_blk2=reg128#4
# asm 2: veor >s3_blk2=q5,<s3_blk2=q8,<s1_blk2=q3
veor q5,q8,q3

# qhasm: s3_blk1 ^= msg_blk1
# asm 1: veor >s3_blk1=reg128#5,<s3_blk1=reg128#5,<msg_blk1=reg128#13
# asm 2: veor >s3_blk1=q4,<s3_blk1=q4,<msg_blk1=q12
veor q4,q4,q12

# qhasm: s3_blk2 ^= msg_blk2
# asm 1: veor >s3_blk2=reg128#6,<s3_blk2=reg128#6,<msg_blk2=reg128#16
# asm 2: veor >s3_blk2=q5,<s3_blk2=q5,<msg_blk2=q15
veor q5,q5,q15

# qhasm: caller_q4 = s4_blk1 & s0_blk1
# asm 1: vand >caller_q4=reg128#9,<s4_blk1=reg128#11,<s0_blk1=reg128#1
# asm 2: vand >caller_q4=q8,<s4_blk1=q10,<s0_blk1=q0
vand q8,q10,q0

# qhasm: caller_q5 = s4_blk2 & s0_blk2
# asm 1: vand >caller_q5=reg128#10,<s4_blk2=reg128#12,<s0_blk2=reg128#2
# asm 2: vand >caller_q5=q9,<s4_blk2=q11,<s0_blk2=q1
vand q9,q11,q1

# qhasm: s3_blk1 ^= caller_q4
# asm 1: veor >s3_blk1=reg128#5,<s3_blk1=reg128#5,<caller_q4=reg128#9
# asm 2: veor >s3_blk1=q4,<s3_blk1=q4,<caller_q4=q8
veor q4,q4,q8

# qhasm: s3_blk2 ^= caller_q5
# asm 1: veor >s3_blk2=reg128#6,<s3_blk2=reg128#6,<caller_q5=reg128#10
# asm 2: veor >s3_blk2=q5,<s3_blk2=q5,<caller_q5=q9
veor q5,q5,q9

# qhasm: 2x caller_q4 = s3_blk1 << 7
# asm 1: vshl.i64 >caller_q4=reg128#9,<s3_blk1=reg128#5,#7
# asm 2: vshl.i64 >caller_q4=q8,<s3_blk1=q4,#7
vshl.i64 q8,q4,#7

# qhasm: 2x s3_blk1 unsigned>>= 57
# asm 1: vshr.u64 >s3_blk1=reg128#5,<s3_blk1=reg128#5,#57
# asm 2: vshr.u64 >s3_blk1=q4,<s3_blk1=q4,#57
vshr.u64 q4,q4,#57

# qhasm: s3_blk1 ^= caller_q4
# asm 1: veor >s3_blk1=reg128#9,<s3_blk1=reg128#5,<caller_q4=reg128#9
# asm 2: veor >s3_blk1=q8,<s3_blk1=q4,<caller_q4=q8
veor q8,q4,q8

# qhasm: 2x caller_q5 = s3_blk2 << 7
# asm 1: vshl.i64 >caller_q5=reg128#5,<s3_blk2=reg128#6,#7
# asm 2: vshl.i64 >caller_q5=q4,<s3_blk2=q5,#7
vshl.i64 q4,q5,#7

# qhasm: 2x s3_blk2 unsigned>>= 57
# asm 1: vshr.u64 >s3_blk2=reg128#6,<s3_blk2=reg128#6,#57
# asm 2: vshr.u64 >s3_blk2=q5,<s3_blk2=q5,#57
vshr.u64 q5,q5,#57

# qhasm: s3_blk2 ^= caller_q5
# asm 1: veor >s3_blk2=reg128#10,<s3_blk2=reg128#6,<caller_q5=reg128#5
# asm 2: veor >s3_blk2=q9,<s3_blk2=q5,<caller_q5=q4
veor q9,q5,q4

# qhasm: s1_blk1 >=< s1_blk2
# asm 1: vswp <s1_blk1=reg128#3,<s1_blk2=reg128#4
# asm 2: vswp <s1_blk1=q2,<s1_blk2=q3
vswp q2,q3

# qhasm: s4_blk1 ^= s2_blk1
# asm 1: veor >s4_blk1=reg128#5,<s4_blk1=reg128#11,<s2_blk1=reg128#14
# asm 2: veor >s4_blk1=q4,<s4_blk1=q10,<s2_blk1=q13
veor q4,q10,q13

# qhasm: s4_blk2 ^= s2_blk2
# asm 1: veor >s4_blk2=reg128#6,<s4_blk2=reg128#12,<s2_blk2=reg128#15
# asm 2: veor >s4_blk2=q5,<s4_blk2=q11,<s2_blk2=q14
veor q5,q11,q14

# qhasm: s4_blk1 ^= msg_blk1
# asm 1: veor >s4_blk1=reg128#5,<s4_blk1=reg128#5,<msg_blk1=reg128#13
# asm 2: veor >s4_blk1=q4,<s4_blk1=q4,<msg_blk1=q12
veor q4,q4,q12

# qhasm: s4_blk2 ^= msg_blk2
# asm 1: veor >s4_blk2=reg128#6,<s4_blk2=reg128#6,<msg_blk2=reg128#16
# asm 2: veor >s4_blk2=q5,<s4_blk2=q5,<msg_blk2=q15
veor q5,q5,q15

# qhasm: caller_q4 = s0_blk1 & s1_blk1
# asm 1: vand >caller_q4=reg128#11,<s0_blk1=reg128#1,<s1_blk1=reg128#3
# asm 2: vand >caller_q4=q10,<s0_blk1=q0,<s1_blk1=q2
vand q10,q0,q2

# qhasm: caller_q5 = s0_blk2 & s1_blk2
# asm 1: vand >caller_q5=reg128#12,<s0_blk2=reg128#2,<s1_blk2=reg128#4
# asm 2: vand >caller_q5=q11,<s0_blk2=q1,<s1_blk2=q3
vand q11,q1,q3

# qhasm: s4_blk1 ^= caller_q4
# asm 1: veor >s4_blk1=reg128#5,<s4_blk1=reg128#5,<caller_q4=reg128#11
# asm 2: veor >s4_blk1=q4,<s4_blk1=q4,<caller_q4=q10
veor q4,q4,q10

# qhasm: s4_blk2 ^= caller_q5
# asm 1: veor >s4_blk2=reg128#11,<s4_blk2=reg128#6,<caller_q5=reg128#12
# asm 2: veor >s4_blk2=q10,<s4_blk2=q5,<caller_q5=q11
veor q10,q5,q11

# qhasm: 2x caller_q4 = s4_blk1 << 4
# asm 1: vshl.i64 >caller_q4=reg128#6,<s4_blk1=reg128#5,#4
# asm 2: vshl.i64 >caller_q4=q5,<s4_blk1=q4,#4
vshl.i64 q5,q4,#4

# qhasm: 2x s4_blk1 unsigned>>= 60
# asm 1: vshr.u64 >s4_blk1=reg128#5,<s4_blk1=reg128#5,#60
# asm 2: vshr.u64 >s4_blk1=q4,<s4_blk1=q4,#60
vshr.u64 q4,q4,#60

# qhasm: s4_blk1 ^= caller_q4
# asm 1: veor >s4_blk1=reg128#12,<s4_blk1=reg128#5,<caller_q4=reg128#6
# asm 2: veor >s4_blk1=q11,<s4_blk1=q4,<caller_q4=q5
veor q11,q4,q5

# qhasm: caller_q4 = s2_blk2[2,3]s2_blk1[0,1]
# asm 1: vext.32 >caller_q4=reg128#5,<s2_blk2=reg128#15,<s2_blk1=reg128#14,#2
# asm 2: vext.32 >caller_q4=q4,<s2_blk2=q14,<s2_blk1=q13,#2
vext.32 q4,q14,q13,#2

# qhasm: 2x caller_q5 = s4_blk2 << 4
# asm 1: vshl.i64 >caller_q5=reg128#6,<s4_blk2=reg128#11,#4
# asm 2: vshl.i64 >caller_q5=q5,<s4_blk2=q10,#4
vshl.i64 q5,q10,#4

# qhasm: 2x s4_blk2 unsigned>>= 60
# asm 1: vshr.u64 >s4_blk2=reg128#11,<s4_blk2=reg128#11,#60
# asm 2: vshr.u64 >s4_blk2=q10,<s4_blk2=q10,#60
vshr.u64 q10,q10,#60

# qhasm: s4_blk2 ^= caller_q5
# asm 1: veor >s4_blk2=reg128#11,<s4_blk2=reg128#11,<caller_q5=reg128#6
# asm 2: veor >s4_blk2=q10,<s4_blk2=q10,<caller_q5=q5
veor q10,q10,q5

# qhasm: s2_blk2 = s2_blk1[2,3]s2_blk2[0,1]
# asm 1: vext.32 >s2_blk2=reg128#13,<s2_blk1=reg128#14,<s2_blk2=reg128#15,#2
# asm 2: vext.32 >s2_blk2=q12,<s2_blk1=q13,<s2_blk2=q14,#2
vext.32 q12,q13,q14,#2

# qhasm: s2_blk1 = caller_q4
# asm 1: vmov >s2_blk1=reg128#14,<caller_q4=reg128#5
# asm 2: vmov >s2_blk1=q13,<caller_q4=q4
vmov q13,q4

# qhasm: input_1 -= 144
# asm 1: sub >input_1=int32#1,<input_1=int32#2,#144
# asm 2: sub >input_1=r0,<input_1=r1,#144
sub r0,r1,#144

# qhasm: mem128[input_1] aligned= s0_blk1
# asm 1: vst1.8 {<s0_blk1=reg128#1%bot-<s0_blk1=reg128#1%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s0_blk1=d0-<s0_blk1=d1},[<input_1=r0,: 128]
vst1.8 {d0-d1},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s0_blk2
# asm 1: vst1.8 {<s0_blk2=reg128#2%bot-<s0_blk2=reg128#2%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s0_blk2=d2-<s0_blk2=d3},[<input_1=r0,: 128]
vst1.8 {d2-d3},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s1_blk1
# asm 1: vst1.8 {<s1_blk1=reg128#3%bot-<s1_blk1=reg128#3%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s1_blk1=d4-<s1_blk1=d5},[<input_1=r0,: 128]
vst1.8 {d4-d5},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s1_blk2
# asm 1: vst1.8 {<s1_blk2=reg128#4%bot-<s1_blk2=reg128#4%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s1_blk2=d6-<s1_blk2=d7},[<input_1=r0,: 128]
vst1.8 {d6-d7},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s2_blk1
# asm 1: vst1.8 {<s2_blk1=reg128#14%bot-<s2_blk1=reg128#14%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s2_blk1=d26-<s2_blk1=d27},[<input_1=r0,: 128]
vst1.8 {d26-d27},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s2_blk2
# asm 1: vst1.8 {<s2_blk2=reg128#13%bot-<s2_blk2=reg128#13%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s2_blk2=d24-<s2_blk2=d25},[<input_1=r0,: 128]
vst1.8 {d24-d25},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s3_blk1
# asm 1: vst1.8 {<s3_blk1=reg128#9%bot-<s3_blk1=reg128#9%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s3_blk1=d16-<s3_blk1=d17},[<input_1=r0,: 128]
vst1.8 {d16-d17},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s3_blk2
# asm 1: vst1.8 {<s3_blk2=reg128#10%bot-<s3_blk2=reg128#10%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s3_blk2=d18-<s3_blk2=d19},[<input_1=r0,: 128]
vst1.8 {d18-d19},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s4_blk1
# asm 1: vst1.8 {<s4_blk1=reg128#12%bot-<s4_blk1=reg128#12%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s4_blk1=d22-<s4_blk1=d23},[<input_1=r0,: 128]
vst1.8 {d22-d23},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: mem128[input_1] aligned= s4_blk2
# asm 1: vst1.8 {<s4_blk2=reg128#11%bot-<s4_blk2=reg128#11%top},[<input_1=int32#1,: 128]
# asm 2: vst1.8 {<s4_blk2=d20-<s4_blk2=d21},[<input_1=r0,: 128]
vst1.8 {d20-d21},[r0,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#1,<input_1=int32#1,#16
# asm 2: add >input_1=r0,<input_1=r0,#16
add r0,r0,#16

# qhasm: caller_q4 = caller_q4[0]q4_stack[1]
# asm 1: vldr <caller_q4=reg128#5%top,<q4_stack=stack128#1
# asm 2: vldr <caller_q4=d9,<q4_stack=[sp,#8]
vldr d9,[sp,#8]

# qhasm: caller_q4 = q4_stack[0]caller_q4[1]
# asm 1: vldr <caller_q4=reg128#5%bot,<q4_stack=stack128#1
# asm 2: vldr <caller_q4=d8,<q4_stack=[sp,#0]
vldr d8,[sp,#0]

# qhasm: caller_q5 = caller_q5[0]q5_stack[1]
# asm 1: vldr <caller_q5=reg128#6%top,<q5_stack=stack128#2
# asm 2: vldr <caller_q5=d11,<q5_stack=[sp,#24]
vldr d11,[sp,#24]

# qhasm: caller_q5 = q5_stack[0]caller_q5[1]
# asm 1: vldr <caller_q5=reg128#6%bot,<q5_stack=stack128#2
# asm 2: vldr <caller_q5=d10,<q5_stack=[sp,#16]
vldr d10,[sp,#16]

# qhasm: return
add sp,sp,#32
bx lr
