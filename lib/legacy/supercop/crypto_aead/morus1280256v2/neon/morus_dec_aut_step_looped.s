
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

# qhasm: stack32 r4_stack

# qhasm: stack32 r5_stack

# qhasm: stack128 q4_stack

# qhasm: stack128 q5_stack

# qhasm: int32 loop_counter_right

# qhasm: int32 loop_counter_left

# qhasm: enter morus_dec_aut_step_looped
.align 4
.global _morus_dec_aut_step_looped
.global morus_dec_aut_step_looped
.type _morus_dec_aut_step_looped STT_FUNC
.type morus_dec_aut_step_looped STT_FUNC
_morus_dec_aut_step_looped:
morus_dec_aut_step_looped:
sub sp,sp,#64

# qhasm: r4_stack = caller_r4
# asm 1: str <caller_r4=int32#5,>r4_stack=stack32#1
# asm 2: str <caller_r4=r4,>r4_stack=[sp,#0]
str r4,[sp,#0]

# qhasm: r5_stack = caller_r5
# asm 1: str <caller_r5=int32#6,>r5_stack=stack32#2
# asm 2: str <caller_r5=r5,>r5_stack=[sp,#4]
str r5,[sp,#4]

# qhasm: q4_stack = q4_stack[0]caller_q4[1]
# asm 1: vstr <caller_q4=reg128#5%top,<q4_stack=stack128#1
# asm 2: vstr <caller_q4=d9,<q4_stack=[sp,#40]
vstr d9,[sp,#40]

# qhasm: q4_stack = caller_q4[0]q4_stack[1]
# asm 1: vstr <caller_q4=reg128#5%bot,<q4_stack=stack128#1
# asm 2: vstr <caller_q4=d8,<q4_stack=[sp,#32]
vstr d8,[sp,#32]

# qhasm: q5_stack = q5_stack[0]caller_q5[1]
# asm 1: vstr <caller_q5=reg128#6%top,<q5_stack=stack128#2
# asm 2: vstr <caller_q5=d11,<q5_stack=[sp,#56]
vstr d11,[sp,#56]

# qhasm: q5_stack = caller_q5[0]q5_stack[1]
# asm 1: vstr <caller_q5=reg128#6%bot,<q5_stack=stack128#2
# asm 2: vstr <caller_q5=d10,<q5_stack=[sp,#48]
vstr d10,[sp,#48]

# qhasm: s0_blk1 aligned= mem128[input_2]
# asm 1: vld1.8 {>s0_blk1=reg128#1%bot->s0_blk1=reg128#1%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s0_blk1=d0->s0_blk1=d1},[<input_2=r2,: 128]
vld1.8 {d0-d1},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s0_blk2 aligned= mem128[input_2]
# asm 1: vld1.8 {>s0_blk2=reg128#2%bot->s0_blk2=reg128#2%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s0_blk2=d2->s0_blk2=d3},[<input_2=r2,: 128]
vld1.8 {d2-d3},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s1_blk1 aligned= mem128[input_2]
# asm 1: vld1.8 {>s1_blk1=reg128#3%bot->s1_blk1=reg128#3%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s1_blk1=d4->s1_blk1=d5},[<input_2=r2,: 128]
vld1.8 {d4-d5},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s1_blk2 aligned= mem128[input_2]
# asm 1: vld1.8 {>s1_blk2=reg128#4%bot->s1_blk2=reg128#4%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s1_blk2=d6->s1_blk2=d7},[<input_2=r2,: 128]
vld1.8 {d6-d7},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s2_blk1 aligned= mem128[input_2]
# asm 1: vld1.8 {>s2_blk1=reg128#9%bot->s2_blk1=reg128#9%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s2_blk1=d16->s2_blk1=d17},[<input_2=r2,: 128]
vld1.8 {d16-d17},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s2_blk2 aligned= mem128[input_2]
# asm 1: vld1.8 {>s2_blk2=reg128#10%bot->s2_blk2=reg128#10%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s2_blk2=d18->s2_blk2=d19},[<input_2=r2,: 128]
vld1.8 {d18-d19},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s3_blk1 aligned= mem128[input_2]
# asm 1: vld1.8 {>s3_blk1=reg128#11%bot->s3_blk1=reg128#11%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s3_blk1=d20->s3_blk1=d21},[<input_2=r2,: 128]
vld1.8 {d20-d21},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s3_blk2 aligned= mem128[input_2]
# asm 1: vld1.8 {>s3_blk2=reg128#12%bot->s3_blk2=reg128#12%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s3_blk2=d22->s3_blk2=d23},[<input_2=r2,: 128]
vld1.8 {d22-d23},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s4_blk1 aligned= mem128[input_2]
# asm 1: vld1.8 {>s4_blk1=reg128#13%bot->s4_blk1=reg128#13%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s4_blk1=d24->s4_blk1=d25},[<input_2=r2,: 128]
vld1.8 {d24-d25},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s4_blk2 aligned= mem128[input_2]
# asm 1: vld1.8 {>s4_blk2=reg128#14%bot->s4_blk2=reg128#14%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s4_blk2=d26->s4_blk2=d27},[<input_2=r2,: 128]
vld1.8 {d26-d27},[r2,: 128]

# qhasm: assign r4 r5 to loop_counter_right loop_counter_left = mem64[input_3 + 0]
# asm 1: ldrd >loop_counter_right=int32#5,[<input_3=int32#4,#0]
# asm 2: ldrd >loop_counter_right=r4,[<input_3=r3,#0]
ldrd r4,[r3,#0]

# qhasm: loop:
._loop:

# qhasm: =? loop_counter_right - 0
# asm 1: cmp <loop_counter_right=int32#5,#0
# asm 2: cmp <loop_counter_right=r4,#0
cmp r4,#0

# qhasm: goto start if !=
bne ._start

# qhasm: =? loop_counter_left - 0
# asm 1: cmp <loop_counter_left=int32#6,#0
# asm 2: cmp <loop_counter_left=r5,#0
cmp r5,#0

# qhasm: goto end if =
beq ._end

# qhasm: start:
._start:

# qhasm: msg_blk1 aligned= mem128[input_1]
# asm 1: vld1.8 {>msg_blk1=reg128#5%bot->msg_blk1=reg128#5%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>msg_blk1=d8->msg_blk1=d9},[<input_1=r1,: 128]
vld1.8 {d8-d9},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: msg_blk2 aligned= mem128[input_1]
# asm 1: vld1.8 {>msg_blk2=reg128#6%bot->msg_blk2=reg128#6%top},[<input_1=int32#2,: 128]
# asm 2: vld1.8 {>msg_blk2=d10->msg_blk2=d11},[<input_1=r1,: 128]
vld1.8 {d10-d11},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: caller_q4 = s2_blk1 & s3_blk1
# asm 1: vand >caller_q4=reg128#15,<s2_blk1=reg128#9,<s3_blk1=reg128#11
# asm 2: vand >caller_q4=q14,<s2_blk1=q8,<s3_blk1=q10
vand q14,q8,q10

# qhasm: caller_q5 = s1_blk1[2,3]s1_blk2[0,1]
# asm 1: vext.32 >caller_q5=reg128#16,<s1_blk1=reg128#3,<s1_blk2=reg128#4,#2
# asm 2: vext.32 >caller_q5=q15,<s1_blk1=q2,<s1_blk2=q3,#2
vext.32 q15,q2,q3,#2

# qhasm: caller_q4 ^= caller_q5
# asm 1: veor >caller_q4=reg128#15,<caller_q4=reg128#15,<caller_q5=reg128#16
# asm 2: veor >caller_q4=q14,<caller_q4=q14,<caller_q5=q15
veor q14,q14,q15

# qhasm: caller_q4 ^= s0_blk1
# asm 1: veor >caller_q4=reg128#15,<caller_q4=reg128#15,<s0_blk1=reg128#1
# asm 2: veor >caller_q4=q14,<caller_q4=q14,<s0_blk1=q0
veor q14,q14,q0

# qhasm: msg_blk1 ^= caller_q4
# asm 1: veor >msg_blk1=reg128#5,<msg_blk1=reg128#5,<caller_q4=reg128#15
# asm 2: veor >msg_blk1=q4,<msg_blk1=q4,<caller_q4=q14
veor q4,q4,q14

# qhasm: mem128[input_0] aligned= msg_blk1
# asm 1: vst1.8 {<msg_blk1=reg128#5%bot-<msg_blk1=reg128#5%top},[<input_0=int32#1,: 128]
# asm 2: vst1.8 {<msg_blk1=d8-<msg_blk1=d9},[<input_0=r0,: 128]
vst1.8 {d8-d9},[r0,: 128]

# qhasm: input_0 += 16
# asm 1: add >input_0=int32#1,<input_0=int32#1,#16
# asm 2: add >input_0=r0,<input_0=r0,#16
add r0,r0,#16

# qhasm: caller_q4 = s2_blk2 & s3_blk2
# asm 1: vand >caller_q4=reg128#15,<s2_blk2=reg128#10,<s3_blk2=reg128#12
# asm 2: vand >caller_q4=q14,<s2_blk2=q9,<s3_blk2=q11
vand q14,q9,q11

# qhasm: caller_q5 = s1_blk2[2,3]s1_blk1[0,1]
# asm 1: vext.32 >caller_q5=reg128#16,<s1_blk2=reg128#4,<s1_blk1=reg128#3,#2
# asm 2: vext.32 >caller_q5=q15,<s1_blk2=q3,<s1_blk1=q2,#2
vext.32 q15,q3,q2,#2

# qhasm: caller_q4 ^= caller_q5
# asm 1: veor >caller_q4=reg128#15,<caller_q4=reg128#15,<caller_q5=reg128#16
# asm 2: veor >caller_q4=q14,<caller_q4=q14,<caller_q5=q15
veor q14,q14,q15

# qhasm: caller_q4 ^= s0_blk2
# asm 1: veor >caller_q4=reg128#15,<caller_q4=reg128#15,<s0_blk2=reg128#2
# asm 2: veor >caller_q4=q14,<caller_q4=q14,<s0_blk2=q1
veor q14,q14,q1

# qhasm: msg_blk2 ^= caller_q4
# asm 1: veor >msg_blk2=reg128#6,<msg_blk2=reg128#6,<caller_q4=reg128#15
# asm 2: veor >msg_blk2=q5,<msg_blk2=q5,<caller_q4=q14
veor q5,q5,q14

# qhasm: mem128[input_0] aligned= msg_blk2
# asm 1: vst1.8 {<msg_blk2=reg128#6%bot-<msg_blk2=reg128#6%top},[<input_0=int32#1,: 128]
# asm 2: vst1.8 {<msg_blk2=d10-<msg_blk2=d11},[<input_0=r0,: 128]
vst1.8 {d10-d11},[r0,: 128]

# qhasm: input_0 += 16
# asm 1: add >input_0=int32#1,<input_0=int32#1,#16
# asm 2: add >input_0=r0,<input_0=r0,#16
add r0,r0,#16

# qhasm: s0_blk1 ^= s3_blk1
# asm 1: veor >s0_blk1=reg128#1,<s0_blk1=reg128#1,<s3_blk1=reg128#11
# asm 2: veor >s0_blk1=q0,<s0_blk1=q0,<s3_blk1=q10
veor q0,q0,q10

# qhasm: s0_blk2 ^= s3_blk2
# asm 1: veor >s0_blk2=reg128#2,<s0_blk2=reg128#2,<s3_blk2=reg128#12
# asm 2: veor >s0_blk2=q1,<s0_blk2=q1,<s3_blk2=q11
veor q1,q1,q11

# qhasm: caller_q4 = s1_blk1 & s2_blk1
# asm 1: vand >caller_q4=reg128#15,<s1_blk1=reg128#3,<s2_blk1=reg128#9
# asm 2: vand >caller_q4=q14,<s1_blk1=q2,<s2_blk1=q8
vand q14,q2,q8

# qhasm: caller_q5 = s1_blk2 & s2_blk2
# asm 1: vand >caller_q5=reg128#16,<s1_blk2=reg128#4,<s2_blk2=reg128#10
# asm 2: vand >caller_q5=q15,<s1_blk2=q3,<s2_blk2=q9
vand q15,q3,q9

# qhasm: s0_blk1 ^= caller_q4
# asm 1: veor >s0_blk1=reg128#1,<s0_blk1=reg128#1,<caller_q4=reg128#15
# asm 2: veor >s0_blk1=q0,<s0_blk1=q0,<caller_q4=q14
veor q0,q0,q14

# qhasm: s0_blk2 ^= caller_q5
# asm 1: veor >s0_blk2=reg128#2,<s0_blk2=reg128#2,<caller_q5=reg128#16
# asm 2: veor >s0_blk2=q1,<s0_blk2=q1,<caller_q5=q15
veor q1,q1,q15

# qhasm: 2x caller_q4 = s0_blk1 << 13
# asm 1: vshl.i64 >caller_q4=reg128#15,<s0_blk1=reg128#1,#13
# asm 2: vshl.i64 >caller_q4=q14,<s0_blk1=q0,#13
vshl.i64 q14,q0,#13

# qhasm: 2x s0_blk1 unsigned>>= 51
# asm 1: vshr.u64 >s0_blk1=reg128#1,<s0_blk1=reg128#1,#51
# asm 2: vshr.u64 >s0_blk1=q0,<s0_blk1=q0,#51
vshr.u64 q0,q0,#51

# qhasm: s0_blk1 ^= caller_q4
# asm 1: veor >s0_blk1=reg128#1,<s0_blk1=reg128#1,<caller_q4=reg128#15
# asm 2: veor >s0_blk1=q0,<s0_blk1=q0,<caller_q4=q14
veor q0,q0,q14

# qhasm: caller_q4 = s3_blk2[2,3]s3_blk1[0,1]
# asm 1: vext.32 >caller_q4=reg128#15,<s3_blk2=reg128#12,<s3_blk1=reg128#11,#2
# asm 2: vext.32 >caller_q4=q14,<s3_blk2=q11,<s3_blk1=q10,#2
vext.32 q14,q11,q10,#2

# qhasm: 2x caller_q5 = s0_blk2 << 13
# asm 1: vshl.i64 >caller_q5=reg128#16,<s0_blk2=reg128#2,#13
# asm 2: vshl.i64 >caller_q5=q15,<s0_blk2=q1,#13
vshl.i64 q15,q1,#13

# qhasm: 2x s0_blk2 unsigned>>= 51
# asm 1: vshr.u64 >s0_blk2=reg128#2,<s0_blk2=reg128#2,#51
# asm 2: vshr.u64 >s0_blk2=q1,<s0_blk2=q1,#51
vshr.u64 q1,q1,#51

# qhasm: s0_blk2 ^= caller_q5
# asm 1: veor >s0_blk2=reg128#2,<s0_blk2=reg128#2,<caller_q5=reg128#16
# asm 2: veor >s0_blk2=q1,<s0_blk2=q1,<caller_q5=q15
veor q1,q1,q15

# qhasm: s3_blk2 = s3_blk1[2,3]s3_blk2[0,1]
# asm 1: vext.32 >s3_blk2=reg128#11,<s3_blk1=reg128#11,<s3_blk2=reg128#12,#2
# asm 2: vext.32 >s3_blk2=q10,<s3_blk1=q10,<s3_blk2=q11,#2
vext.32 q10,q10,q11,#2

# qhasm: s3_blk1 = caller_q4
# asm 1: vmov >s3_blk1=reg128#12,<caller_q4=reg128#15
# asm 2: vmov >s3_blk1=q11,<caller_q4=q14
vmov q11,q14

# qhasm: s1_blk1 ^= s4_blk1
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<s4_blk1=reg128#13
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<s4_blk1=q12
veor q2,q2,q12

# qhasm: s1_blk2 ^= s4_blk2
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<s4_blk2=reg128#14
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<s4_blk2=q13
veor q3,q3,q13

# qhasm: s1_blk1 ^= msg_blk1
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<msg_blk1=reg128#5
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<msg_blk1=q4
veor q2,q2,q4

# qhasm: s1_blk2 ^= msg_blk2
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<msg_blk2=reg128#6
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<msg_blk2=q5
veor q3,q3,q5

# qhasm: caller_q4 = s2_blk1 & s3_blk1
# asm 1: vand >caller_q4=reg128#15,<s2_blk1=reg128#9,<s3_blk1=reg128#12
# asm 2: vand >caller_q4=q14,<s2_blk1=q8,<s3_blk1=q11
vand q14,q8,q11

# qhasm: caller_q5 = s2_blk2 & s3_blk2
# asm 1: vand >caller_q5=reg128#16,<s2_blk2=reg128#10,<s3_blk2=reg128#11
# asm 2: vand >caller_q5=q15,<s2_blk2=q9,<s3_blk2=q10
vand q15,q9,q10

# qhasm: s1_blk1 ^= caller_q4
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<caller_q4=reg128#15
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<caller_q4=q14
veor q2,q2,q14

# qhasm: s1_blk2 ^= caller_q5
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<caller_q5=reg128#16
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<caller_q5=q15
veor q3,q3,q15

# qhasm: 2x caller_q4 = s1_blk1 << 46
# asm 1: vshl.i64 >caller_q4=reg128#15,<s1_blk1=reg128#3,#46
# asm 2: vshl.i64 >caller_q4=q14,<s1_blk1=q2,#46
vshl.i64 q14,q2,#46

# qhasm: 2x s1_blk1 unsigned>>= 18
# asm 1: vshr.u64 >s1_blk1=reg128#3,<s1_blk1=reg128#3,#18
# asm 2: vshr.u64 >s1_blk1=q2,<s1_blk1=q2,#18
vshr.u64 q2,q2,#18

# qhasm: s1_blk1 ^= caller_q4
# asm 1: veor >s1_blk1=reg128#3,<s1_blk1=reg128#3,<caller_q4=reg128#15
# asm 2: veor >s1_blk1=q2,<s1_blk1=q2,<caller_q4=q14
veor q2,q2,q14

# qhasm: 2x caller_q5 = s1_blk2 << 46
# asm 1: vshl.i64 >caller_q5=reg128#15,<s1_blk2=reg128#4,#46
# asm 2: vshl.i64 >caller_q5=q14,<s1_blk2=q3,#46
vshl.i64 q14,q3,#46

# qhasm: 2x s1_blk2 unsigned>>= 18
# asm 1: vshr.u64 >s1_blk2=reg128#4,<s1_blk2=reg128#4,#18
# asm 2: vshr.u64 >s1_blk2=q3,<s1_blk2=q3,#18
vshr.u64 q3,q3,#18

# qhasm: s1_blk2 ^= caller_q5
# asm 1: veor >s1_blk2=reg128#4,<s1_blk2=reg128#4,<caller_q5=reg128#15
# asm 2: veor >s1_blk2=q3,<s1_blk2=q3,<caller_q5=q14
veor q3,q3,q14

# qhasm: s4_blk1 >=< s4_blk2
# asm 1: vswp <s4_blk1=reg128#13,<s4_blk2=reg128#14
# asm 2: vswp <s4_blk1=q12,<s4_blk2=q13
vswp q12,q13

# qhasm: s2_blk1 ^= s0_blk1
# asm 1: veor >s2_blk1=reg128#9,<s2_blk1=reg128#9,<s0_blk1=reg128#1
# asm 2: veor >s2_blk1=q8,<s2_blk1=q8,<s0_blk1=q0
veor q8,q8,q0

# qhasm: s2_blk2 ^= s0_blk2
# asm 1: veor >s2_blk2=reg128#10,<s2_blk2=reg128#10,<s0_blk2=reg128#2
# asm 2: veor >s2_blk2=q9,<s2_blk2=q9,<s0_blk2=q1
veor q9,q9,q1

# qhasm: s2_blk1 ^= msg_blk1
# asm 1: veor >s2_blk1=reg128#9,<s2_blk1=reg128#9,<msg_blk1=reg128#5
# asm 2: veor >s2_blk1=q8,<s2_blk1=q8,<msg_blk1=q4
veor q8,q8,q4

# qhasm: s2_blk2 ^= msg_blk2
# asm 1: veor >s2_blk2=reg128#10,<s2_blk2=reg128#10,<msg_blk2=reg128#6
# asm 2: veor >s2_blk2=q9,<s2_blk2=q9,<msg_blk2=q5
veor q9,q9,q5

# qhasm: caller_q4 = s3_blk1 & s4_blk1
# asm 1: vand >caller_q4=reg128#15,<s3_blk1=reg128#12,<s4_blk1=reg128#13
# asm 2: vand >caller_q4=q14,<s3_blk1=q11,<s4_blk1=q12
vand q14,q11,q12

# qhasm: caller_q5 = s3_blk2 & s4_blk2
# asm 1: vand >caller_q5=reg128#16,<s3_blk2=reg128#11,<s4_blk2=reg128#14
# asm 2: vand >caller_q5=q15,<s3_blk2=q10,<s4_blk2=q13
vand q15,q10,q13

# qhasm: s2_blk1 ^= caller_q4
# asm 1: veor >s2_blk1=reg128#9,<s2_blk1=reg128#9,<caller_q4=reg128#15
# asm 2: veor >s2_blk1=q8,<s2_blk1=q8,<caller_q4=q14
veor q8,q8,q14

# qhasm: s2_blk2 ^= caller_q5
# asm 1: veor >s2_blk2=reg128#10,<s2_blk2=reg128#10,<caller_q5=reg128#16
# asm 2: veor >s2_blk2=q9,<s2_blk2=q9,<caller_q5=q15
veor q9,q9,q15

# qhasm: 2x caller_q4 = s2_blk1 << 38
# asm 1: vshl.i64 >caller_q4=reg128#15,<s2_blk1=reg128#9,#38
# asm 2: vshl.i64 >caller_q4=q14,<s2_blk1=q8,#38
vshl.i64 q14,q8,#38

# qhasm: 2x s2_blk1 unsigned>>= 26
# asm 1: vshr.u64 >s2_blk1=reg128#9,<s2_blk1=reg128#9,#26
# asm 2: vshr.u64 >s2_blk1=q8,<s2_blk1=q8,#26
vshr.u64 q8,q8,#26

# qhasm: s2_blk1 ^= caller_q4
# asm 1: veor >s2_blk1=reg128#9,<s2_blk1=reg128#9,<caller_q4=reg128#15
# asm 2: veor >s2_blk1=q8,<s2_blk1=q8,<caller_q4=q14
veor q8,q8,q14

# qhasm: caller_q4 = s0_blk2[2,3]s0_blk1[0,1]
# asm 1: vext.32 >caller_q4=reg128#15,<s0_blk2=reg128#2,<s0_blk1=reg128#1,#2
# asm 2: vext.32 >caller_q4=q14,<s0_blk2=q1,<s0_blk1=q0,#2
vext.32 q14,q1,q0,#2

# qhasm: 2x caller_q5 = s2_blk2 << 38
# asm 1: vshl.i64 >caller_q5=reg128#16,<s2_blk2=reg128#10,#38
# asm 2: vshl.i64 >caller_q5=q15,<s2_blk2=q9,#38
vshl.i64 q15,q9,#38

# qhasm: 2x s2_blk2 unsigned>>= 26
# asm 1: vshr.u64 >s2_blk2=reg128#10,<s2_blk2=reg128#10,#26
# asm 2: vshr.u64 >s2_blk2=q9,<s2_blk2=q9,#26
vshr.u64 q9,q9,#26

# qhasm: s2_blk2 ^= caller_q5
# asm 1: veor >s2_blk2=reg128#10,<s2_blk2=reg128#10,<caller_q5=reg128#16
# asm 2: veor >s2_blk2=q9,<s2_blk2=q9,<caller_q5=q15
veor q9,q9,q15

# qhasm: s0_blk1 = s0_blk1[2,3]s0_blk2[0,1]
# asm 1: vext.32 >s0_blk1=reg128#1,<s0_blk1=reg128#1,<s0_blk2=reg128#2,#2
# asm 2: vext.32 >s0_blk1=q0,<s0_blk1=q0,<s0_blk2=q1,#2
vext.32 q0,q0,q1,#2

# qhasm: s0_blk2 = caller_q4
# asm 1: vmov >s0_blk2=reg128#2,<caller_q4=reg128#15
# asm 2: vmov >s0_blk2=q1,<caller_q4=q14
vmov q1,q14

# qhasm: s3_blk1 ^= s1_blk1
# asm 1: veor >s3_blk1=reg128#12,<s3_blk1=reg128#12,<s1_blk1=reg128#3
# asm 2: veor >s3_blk1=q11,<s3_blk1=q11,<s1_blk1=q2
veor q11,q11,q2

# qhasm: s3_blk2 ^= s1_blk2
# asm 1: veor >s3_blk2=reg128#11,<s3_blk2=reg128#11,<s1_blk2=reg128#4
# asm 2: veor >s3_blk2=q10,<s3_blk2=q10,<s1_blk2=q3
veor q10,q10,q3

# qhasm: s3_blk1 ^= msg_blk1
# asm 1: veor >s3_blk1=reg128#12,<s3_blk1=reg128#12,<msg_blk1=reg128#5
# asm 2: veor >s3_blk1=q11,<s3_blk1=q11,<msg_blk1=q4
veor q11,q11,q4

# qhasm: s3_blk2 ^= msg_blk2
# asm 1: veor >s3_blk2=reg128#11,<s3_blk2=reg128#11,<msg_blk2=reg128#6
# asm 2: veor >s3_blk2=q10,<s3_blk2=q10,<msg_blk2=q5
veor q10,q10,q5

# qhasm: caller_q4 = s4_blk1 & s0_blk1
# asm 1: vand >caller_q4=reg128#15,<s4_blk1=reg128#13,<s0_blk1=reg128#1
# asm 2: vand >caller_q4=q14,<s4_blk1=q12,<s0_blk1=q0
vand q14,q12,q0

# qhasm: caller_q5 = s4_blk2 & s0_blk2
# asm 1: vand >caller_q5=reg128#16,<s4_blk2=reg128#14,<s0_blk2=reg128#2
# asm 2: vand >caller_q5=q15,<s4_blk2=q13,<s0_blk2=q1
vand q15,q13,q1

# qhasm: s3_blk1 ^= caller_q4
# asm 1: veor >s3_blk1=reg128#12,<s3_blk1=reg128#12,<caller_q4=reg128#15
# asm 2: veor >s3_blk1=q11,<s3_blk1=q11,<caller_q4=q14
veor q11,q11,q14

# qhasm: s3_blk2 ^= caller_q5
# asm 1: veor >s3_blk2=reg128#15,<s3_blk2=reg128#11,<caller_q5=reg128#16
# asm 2: veor >s3_blk2=q14,<s3_blk2=q10,<caller_q5=q15
veor q14,q10,q15

# qhasm: 2x caller_q4 = s3_blk1 << 7
# asm 1: vshl.i64 >caller_q4=reg128#11,<s3_blk1=reg128#12,#7
# asm 2: vshl.i64 >caller_q4=q10,<s3_blk1=q11,#7
vshl.i64 q10,q11,#7

# qhasm: 2x s3_blk1 unsigned>>= 57
# asm 1: vshr.u64 >s3_blk1=reg128#12,<s3_blk1=reg128#12,#57
# asm 2: vshr.u64 >s3_blk1=q11,<s3_blk1=q11,#57
vshr.u64 q11,q11,#57

# qhasm: s3_blk1 ^= caller_q4
# asm 1: veor >s3_blk1=reg128#11,<s3_blk1=reg128#12,<caller_q4=reg128#11
# asm 2: veor >s3_blk1=q10,<s3_blk1=q11,<caller_q4=q10
veor q10,q11,q10

# qhasm: 2x caller_q5 = s3_blk2 << 7
# asm 1: vshl.i64 >caller_q5=reg128#12,<s3_blk2=reg128#15,#7
# asm 2: vshl.i64 >caller_q5=q11,<s3_blk2=q14,#7
vshl.i64 q11,q14,#7

# qhasm: 2x s3_blk2 unsigned>>= 57
# asm 1: vshr.u64 >s3_blk2=reg128#15,<s3_blk2=reg128#15,#57
# asm 2: vshr.u64 >s3_blk2=q14,<s3_blk2=q14,#57
vshr.u64 q14,q14,#57

# qhasm: s3_blk2 ^= caller_q5
# asm 1: veor >s3_blk2=reg128#12,<s3_blk2=reg128#15,<caller_q5=reg128#12
# asm 2: veor >s3_blk2=q11,<s3_blk2=q14,<caller_q5=q11
veor q11,q14,q11

# qhasm: s1_blk1 >=< s1_blk2
# asm 1: vswp <s1_blk1=reg128#3,<s1_blk2=reg128#4
# asm 2: vswp <s1_blk1=q2,<s1_blk2=q3
vswp q2,q3

# qhasm: s4_blk1 ^= s2_blk1
# asm 1: veor >s4_blk1=reg128#13,<s4_blk1=reg128#13,<s2_blk1=reg128#9
# asm 2: veor >s4_blk1=q12,<s4_blk1=q12,<s2_blk1=q8
veor q12,q12,q8

# qhasm: s4_blk2 ^= s2_blk2
# asm 1: veor >s4_blk2=reg128#14,<s4_blk2=reg128#14,<s2_blk2=reg128#10
# asm 2: veor >s4_blk2=q13,<s4_blk2=q13,<s2_blk2=q9
veor q13,q13,q9

# qhasm: s4_blk1 ^= msg_blk1
# asm 1: veor >s4_blk1=reg128#5,<s4_blk1=reg128#13,<msg_blk1=reg128#5
# asm 2: veor >s4_blk1=q4,<s4_blk1=q12,<msg_blk1=q4
veor q4,q12,q4

# qhasm: s4_blk2 ^= msg_blk2
# asm 1: veor >s4_blk2=reg128#6,<s4_blk2=reg128#14,<msg_blk2=reg128#6
# asm 2: veor >s4_blk2=q5,<s4_blk2=q13,<msg_blk2=q5
veor q5,q13,q5

# qhasm: caller_q4 = s0_blk1 & s1_blk1
# asm 1: vand >caller_q4=reg128#13,<s0_blk1=reg128#1,<s1_blk1=reg128#3
# asm 2: vand >caller_q4=q12,<s0_blk1=q0,<s1_blk1=q2
vand q12,q0,q2

# qhasm: caller_q5 = s0_blk2 & s1_blk2
# asm 1: vand >caller_q5=reg128#14,<s0_blk2=reg128#2,<s1_blk2=reg128#4
# asm 2: vand >caller_q5=q13,<s0_blk2=q1,<s1_blk2=q3
vand q13,q1,q3

# qhasm: s4_blk1 ^= caller_q4
# asm 1: veor >s4_blk1=reg128#5,<s4_blk1=reg128#5,<caller_q4=reg128#13
# asm 2: veor >s4_blk1=q4,<s4_blk1=q4,<caller_q4=q12
veor q4,q4,q12

# qhasm: s4_blk2 ^= caller_q5
# asm 1: veor >s4_blk2=reg128#14,<s4_blk2=reg128#6,<caller_q5=reg128#14
# asm 2: veor >s4_blk2=q13,<s4_blk2=q5,<caller_q5=q13
veor q13,q5,q13

# qhasm: 2x caller_q4 = s4_blk1 << 4
# asm 1: vshl.i64 >caller_q4=reg128#6,<s4_blk1=reg128#5,#4
# asm 2: vshl.i64 >caller_q4=q5,<s4_blk1=q4,#4
vshl.i64 q5,q4,#4

# qhasm: 2x s4_blk1 unsigned>>= 60
# asm 1: vshr.u64 >s4_blk1=reg128#5,<s4_blk1=reg128#5,#60
# asm 2: vshr.u64 >s4_blk1=q4,<s4_blk1=q4,#60
vshr.u64 q4,q4,#60

# qhasm: s4_blk1 ^= caller_q4
# asm 1: veor >s4_blk1=reg128#13,<s4_blk1=reg128#5,<caller_q4=reg128#6
# asm 2: veor >s4_blk1=q12,<s4_blk1=q4,<caller_q4=q5
veor q12,q4,q5

# qhasm: caller_q4 = s2_blk2[2,3]s2_blk1[0,1]
# asm 1: vext.32 >caller_q4=reg128#5,<s2_blk2=reg128#10,<s2_blk1=reg128#9,#2
# asm 2: vext.32 >caller_q4=q4,<s2_blk2=q9,<s2_blk1=q8,#2
vext.32 q4,q9,q8,#2

# qhasm: 2x caller_q5 = s4_blk2 << 4
# asm 1: vshl.i64 >caller_q5=reg128#6,<s4_blk2=reg128#14,#4
# asm 2: vshl.i64 >caller_q5=q5,<s4_blk2=q13,#4
vshl.i64 q5,q13,#4

# qhasm: 2x s4_blk2 unsigned>>= 60
# asm 1: vshr.u64 >s4_blk2=reg128#14,<s4_blk2=reg128#14,#60
# asm 2: vshr.u64 >s4_blk2=q13,<s4_blk2=q13,#60
vshr.u64 q13,q13,#60

# qhasm: s4_blk2 ^= caller_q5
# asm 1: veor >s4_blk2=reg128#14,<s4_blk2=reg128#14,<caller_q5=reg128#6
# asm 2: veor >s4_blk2=q13,<s4_blk2=q13,<caller_q5=q5
veor q13,q13,q5

# qhasm: s2_blk2 = s2_blk1[2,3]s2_blk2[0,1]
# asm 1: vext.32 >s2_blk2=reg128#10,<s2_blk1=reg128#9,<s2_blk2=reg128#10,#2
# asm 2: vext.32 >s2_blk2=q9,<s2_blk1=q8,<s2_blk2=q9,#2
vext.32 q9,q8,q9,#2

# qhasm: s2_blk1 = caller_q4
# asm 1: vmov >s2_blk1=reg128#9,<caller_q4=reg128#5
# asm 2: vmov >s2_blk1=q8,<caller_q4=q4
vmov q8,q4

# qhasm: =? loop_counter_right - 0
# asm 1: cmp <loop_counter_right=int32#5,#0
# asm 2: cmp <loop_counter_right=r4,#0
cmp r4,#0

# qhasm: goto decrement_left if =
beq ._decrement_left

# qhasm: loop_counter_right -= 1
# asm 1: sub >loop_counter_right=int32#5,<loop_counter_right=int32#5,#1
# asm 2: sub >loop_counter_right=r4,<loop_counter_right=r4,#1
sub r4,r4,#1

# qhasm: goto loop
b ._loop

# qhasm: decrement_left:
._decrement_left:

# qhasm: loop_counter_left -= 1
# asm 1: sub >loop_counter_left=int32#6,<loop_counter_left=int32#6,#1
# asm 2: sub >loop_counter_left=r5,<loop_counter_left=r5,#1
sub r5,r5,#1

# qhasm: loop_counter_right -= 1
# asm 1: sub >loop_counter_right=int32#5,<loop_counter_right=int32#5,#1
# asm 2: sub >loop_counter_right=r4,<loop_counter_right=r4,#1
sub r4,r4,#1

# qhasm: goto loop
b ._loop

# qhasm: end:
._end:

# qhasm: input_2 -= 144
# asm 1: sub >input_2=int32#1,<input_2=int32#3,#144
# asm 2: sub >input_2=r0,<input_2=r2,#144
sub r0,r2,#144

# qhasm: mem128[input_2] aligned= s0_blk1
# asm 1: vst1.8 {<s0_blk1=reg128#1%bot-<s0_blk1=reg128#1%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s0_blk1=d0-<s0_blk1=d1},[<input_2=r0,: 128]
vst1.8 {d0-d1},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s0_blk2
# asm 1: vst1.8 {<s0_blk2=reg128#2%bot-<s0_blk2=reg128#2%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s0_blk2=d2-<s0_blk2=d3},[<input_2=r0,: 128]
vst1.8 {d2-d3},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s1_blk1
# asm 1: vst1.8 {<s1_blk1=reg128#3%bot-<s1_blk1=reg128#3%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s1_blk1=d4-<s1_blk1=d5},[<input_2=r0,: 128]
vst1.8 {d4-d5},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s1_blk2
# asm 1: vst1.8 {<s1_blk2=reg128#4%bot-<s1_blk2=reg128#4%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s1_blk2=d6-<s1_blk2=d7},[<input_2=r0,: 128]
vst1.8 {d6-d7},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s2_blk1
# asm 1: vst1.8 {<s2_blk1=reg128#9%bot-<s2_blk1=reg128#9%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s2_blk1=d16-<s2_blk1=d17},[<input_2=r0,: 128]
vst1.8 {d16-d17},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s2_blk2
# asm 1: vst1.8 {<s2_blk2=reg128#10%bot-<s2_blk2=reg128#10%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s2_blk2=d18-<s2_blk2=d19},[<input_2=r0,: 128]
vst1.8 {d18-d19},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s3_blk1
# asm 1: vst1.8 {<s3_blk1=reg128#11%bot-<s3_blk1=reg128#11%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s3_blk1=d20-<s3_blk1=d21},[<input_2=r0,: 128]
vst1.8 {d20-d21},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s3_blk2
# asm 1: vst1.8 {<s3_blk2=reg128#12%bot-<s3_blk2=reg128#12%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s3_blk2=d22-<s3_blk2=d23},[<input_2=r0,: 128]
vst1.8 {d22-d23},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s4_blk1
# asm 1: vst1.8 {<s4_blk1=reg128#13%bot-<s4_blk1=reg128#13%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s4_blk1=d24-<s4_blk1=d25},[<input_2=r0,: 128]
vst1.8 {d24-d25},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s4_blk2
# asm 1: vst1.8 {<s4_blk2=reg128#14%bot-<s4_blk2=reg128#14%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s4_blk2=d26-<s4_blk2=d27},[<input_2=r0,: 128]
vst1.8 {d26-d27},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: caller_q4 = caller_q4[0]q4_stack[1]
# asm 1: vldr <caller_q4=reg128#5%top,<q4_stack=stack128#1
# asm 2: vldr <caller_q4=d9,<q4_stack=[sp,#40]
vldr d9,[sp,#40]

# qhasm: caller_q4 = q4_stack[0]caller_q4[1]
# asm 1: vldr <caller_q4=reg128#5%bot,<q4_stack=stack128#1
# asm 2: vldr <caller_q4=d8,<q4_stack=[sp,#32]
vldr d8,[sp,#32]

# qhasm: caller_q5 = caller_q5[0]q5_stack[1]
# asm 1: vldr <caller_q5=reg128#6%top,<q5_stack=stack128#2
# asm 2: vldr <caller_q5=d11,<q5_stack=[sp,#56]
vldr d11,[sp,#56]

# qhasm: caller_q5 = q5_stack[0]caller_q5[1]
# asm 1: vldr <caller_q5=reg128#6%bot,<q5_stack=stack128#2
# asm 2: vldr <caller_q5=d10,<q5_stack=[sp,#48]
vldr d10,[sp,#48]

# qhasm: caller_r4 = r4_stack
# asm 1: ldr >caller_r4=int32#5,<r4_stack=stack32#1
# asm 2: ldr >caller_r4=r4,<r4_stack=[sp,#0]
ldr r4,[sp,#0]

# qhasm: caller_r5 = r5_stack
# asm 1: ldr >caller_r5=int32#6,<r5_stack=stack32#2
# asm 2: ldr >caller_r5=r5,<r5_stack=[sp,#4]
ldr r5,[sp,#4]

# qhasm: return
add sp,sp,#64
bx lr
