
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

# qhasm: reg128 msg_blk

# qhasm: reg128 cipher_blk

# qhasm: reg128 s0_blk

# qhasm: reg128 s1_blk

# qhasm: reg128 s2_blk

# qhasm: reg128 s3_blk

# qhasm: reg128 s4_blk

# qhasm: reg128 temp

# qhasm: reg128 temp2

# qhasm: stack32 r4_stack

# qhasm: stack32 r5_stack

# qhasm: int32 loop_counter_right

# qhasm: int32 loop_counter_left

# qhasm: enter morus_enc_aut_step_looped
.align 4
.global _morus_enc_aut_step_looped
.global morus_enc_aut_step_looped
.type _morus_enc_aut_step_looped STT_FUNC
.type morus_enc_aut_step_looped STT_FUNC
_morus_enc_aut_step_looped:
morus_enc_aut_step_looped:
sub sp,sp,#32

# qhasm: r4_stack = caller_r4
# asm 1: str <caller_r4=int32#5,>r4_stack=stack32#1
# asm 2: str <caller_r4=r4,>r4_stack=[sp,#0]
str r4,[sp,#0]

# qhasm: r5_stack = caller_r5
# asm 1: str <caller_r5=int32#6,>r5_stack=stack32#2
# asm 2: str <caller_r5=r5,>r5_stack=[sp,#4]
str r5,[sp,#4]

# qhasm: s0_blk aligned= mem128[input_2]
# asm 1: vld1.8 {>s0_blk=reg128#1%bot->s0_blk=reg128#1%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s0_blk=d0->s0_blk=d1},[<input_2=r2,: 128]
vld1.8 {d0-d1},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s1_blk aligned= mem128[input_2]
# asm 1: vld1.8 {>s1_blk=reg128#2%bot->s1_blk=reg128#2%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s1_blk=d2->s1_blk=d3},[<input_2=r2,: 128]
vld1.8 {d2-d3},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s2_blk aligned= mem128[input_2]
# asm 1: vld1.8 {>s2_blk=reg128#3%bot->s2_blk=reg128#3%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s2_blk=d4->s2_blk=d5},[<input_2=r2,: 128]
vld1.8 {d4-d5},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s3_blk aligned= mem128[input_2]
# asm 1: vld1.8 {>s3_blk=reg128#4%bot->s3_blk=reg128#4%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s3_blk=d6->s3_blk=d7},[<input_2=r2,: 128]
vld1.8 {d6-d7},[r2,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#3,<input_2=int32#3,#16
# asm 2: add >input_2=r2,<input_2=r2,#16
add r2,r2,#16

# qhasm: s4_blk aligned= mem128[input_2]
# asm 1: vld1.8 {>s4_blk=reg128#9%bot->s4_blk=reg128#9%top},[<input_2=int32#3,: 128]
# asm 2: vld1.8 {>s4_blk=d16->s4_blk=d17},[<input_2=r2,: 128]
vld1.8 {d16-d17},[r2,: 128]

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

# qhasm: msg_blk aligned= mem128[input_0]
# asm 1: vld1.8 {>msg_blk=reg128#10%bot->msg_blk=reg128#10%top},[<input_0=int32#1,: 128]
# asm 2: vld1.8 {>msg_blk=d18->msg_blk=d19},[<input_0=r0,: 128]
vld1.8 {d18-d19},[r0,: 128]

# qhasm: input_0 += 16
# asm 1: add >input_0=int32#1,<input_0=int32#1,#16
# asm 2: add >input_0=r0,<input_0=r0,#16
add r0,r0,#16

# qhasm: temp = s2_blk & s3_blk
# asm 1: vand >temp=reg128#11,<s2_blk=reg128#3,<s3_blk=reg128#4
# asm 2: vand >temp=q10,<s2_blk=q2,<s3_blk=q3
vand q10,q2,q3

# qhasm: temp2 = s1_blk[1,2,3]s1_blk[0]
# asm 1: vext.32 >temp2=reg128#12,<s1_blk=reg128#2,<s1_blk=reg128#2,#1
# asm 2: vext.32 >temp2=q11,<s1_blk=q1,<s1_blk=q1,#1
vext.32 q11,q1,q1,#1

# qhasm: temp ^= temp2
# asm 1: veor >temp=reg128#11,<temp=reg128#11,<temp2=reg128#12
# asm 2: veor >temp=q10,<temp=q10,<temp2=q11
veor q10,q10,q11

# qhasm: temp ^= s0_blk
# asm 1: veor >temp=reg128#11,<temp=reg128#11,<s0_blk=reg128#1
# asm 2: veor >temp=q10,<temp=q10,<s0_blk=q0
veor q10,q10,q0

# qhasm: cipher_blk = msg_blk ^ temp
# asm 1: veor >cipher_blk=reg128#11,<msg_blk=reg128#10,<temp=reg128#11
# asm 2: veor >cipher_blk=q10,<msg_blk=q9,<temp=q10
veor q10,q9,q10

# qhasm: mem128[input_1] aligned= cipher_blk
# asm 1: vst1.8 {<cipher_blk=reg128#11%bot-<cipher_blk=reg128#11%top},[<input_1=int32#2,: 128]
# asm 2: vst1.8 {<cipher_blk=d20-<cipher_blk=d21},[<input_1=r1,: 128]
vst1.8 {d20-d21},[r1,: 128]

# qhasm: input_1 += 16
# asm 1: add >input_1=int32#2,<input_1=int32#2,#16
# asm 2: add >input_1=r1,<input_1=r1,#16
add r1,r1,#16

# qhasm: s0_blk ^= s3_blk
# asm 1: veor >s0_blk=reg128#1,<s0_blk=reg128#1,<s3_blk=reg128#4
# asm 2: veor >s0_blk=q0,<s0_blk=q0,<s3_blk=q3
veor q0,q0,q3

# qhasm: temp = s1_blk & s2_blk
# asm 1: vand >temp=reg128#11,<s1_blk=reg128#2,<s2_blk=reg128#3
# asm 2: vand >temp=q10,<s1_blk=q1,<s2_blk=q2
vand q10,q1,q2

# qhasm: s0_blk ^= temp
# asm 1: veor >s0_blk=reg128#1,<s0_blk=reg128#1,<temp=reg128#11
# asm 2: veor >s0_blk=q0,<s0_blk=q0,<temp=q10
veor q0,q0,q10

# qhasm: 4x temp = s0_blk << 5
# asm 1: vshl.i32 >temp=reg128#11,<s0_blk=reg128#1,#5
# asm 2: vshl.i32 >temp=q10,<s0_blk=q0,#5
vshl.i32 q10,q0,#5

# qhasm: 4x s0_blk unsigned>>= 27
# asm 1: vshr.u32 >s0_blk=reg128#1,<s0_blk=reg128#1,#27
# asm 2: vshr.u32 >s0_blk=q0,<s0_blk=q0,#27
vshr.u32 q0,q0,#27

# qhasm: s0_blk ^= temp
# asm 1: veor >s0_blk=reg128#1,<s0_blk=reg128#1,<temp=reg128#11
# asm 2: veor >s0_blk=q0,<s0_blk=q0,<temp=q10
veor q0,q0,q10

# qhasm: s3_blk = s3_blk[3]s3_blk[0,1,2]
# asm 1: vext.32 >s3_blk=reg128#4,<s3_blk=reg128#4,<s3_blk=reg128#4,#3
# asm 2: vext.32 >s3_blk=q3,<s3_blk=q3,<s3_blk=q3,#3
vext.32 q3,q3,q3,#3

# qhasm: s1_blk ^= s4_blk
# asm 1: veor >s1_blk=reg128#2,<s1_blk=reg128#2,<s4_blk=reg128#9
# asm 2: veor >s1_blk=q1,<s1_blk=q1,<s4_blk=q8
veor q1,q1,q8

# qhasm: s1_blk ^= msg_blk
# asm 1: veor >s1_blk=reg128#2,<s1_blk=reg128#2,<msg_blk=reg128#10
# asm 2: veor >s1_blk=q1,<s1_blk=q1,<msg_blk=q9
veor q1,q1,q9

# qhasm: temp = s2_blk & s3_blk
# asm 1: vand >temp=reg128#11,<s2_blk=reg128#3,<s3_blk=reg128#4
# asm 2: vand >temp=q10,<s2_blk=q2,<s3_blk=q3
vand q10,q2,q3

# qhasm: s1_blk ^= temp
# asm 1: veor >s1_blk=reg128#2,<s1_blk=reg128#2,<temp=reg128#11
# asm 2: veor >s1_blk=q1,<s1_blk=q1,<temp=q10
veor q1,q1,q10

# qhasm: 4x temp = s1_blk << 31
# asm 1: vshl.i32 >temp=reg128#11,<s1_blk=reg128#2,#31
# asm 2: vshl.i32 >temp=q10,<s1_blk=q1,#31
vshl.i32 q10,q1,#31

# qhasm: 4x s1_blk unsigned>>= 1
# asm 1: vshr.u32 >s1_blk=reg128#2,<s1_blk=reg128#2,#1
# asm 2: vshr.u32 >s1_blk=q1,<s1_blk=q1,#1
vshr.u32 q1,q1,#1

# qhasm: s1_blk ^= temp
# asm 1: veor >s1_blk=reg128#2,<s1_blk=reg128#2,<temp=reg128#11
# asm 2: veor >s1_blk=q1,<s1_blk=q1,<temp=q10
veor q1,q1,q10

# qhasm: s4_blk = s4_blk[2,3]s4_blk[0,1]
# asm 1: vswp <s4_blk=reg128#9%bot,<s4_blk=reg128#9%top
# asm 2: vswp <s4_blk=d16,<s4_blk=d17
vswp d16,d17

# qhasm: s2_blk ^= s0_blk
# asm 1: veor >s2_blk=reg128#3,<s2_blk=reg128#3,<s0_blk=reg128#1
# asm 2: veor >s2_blk=q2,<s2_blk=q2,<s0_blk=q0
veor q2,q2,q0

# qhasm: s2_blk ^= msg_blk
# asm 1: veor >s2_blk=reg128#3,<s2_blk=reg128#3,<msg_blk=reg128#10
# asm 2: veor >s2_blk=q2,<s2_blk=q2,<msg_blk=q9
veor q2,q2,q9

# qhasm: temp = s3_blk & s4_blk
# asm 1: vand >temp=reg128#11,<s3_blk=reg128#4,<s4_blk=reg128#9
# asm 2: vand >temp=q10,<s3_blk=q3,<s4_blk=q8
vand q10,q3,q8

# qhasm: s2_blk ^= temp
# asm 1: veor >s2_blk=reg128#3,<s2_blk=reg128#3,<temp=reg128#11
# asm 2: veor >s2_blk=q2,<s2_blk=q2,<temp=q10
veor q2,q2,q10

# qhasm: 4x temp = s2_blk << 7
# asm 1: vshl.i32 >temp=reg128#11,<s2_blk=reg128#3,#7
# asm 2: vshl.i32 >temp=q10,<s2_blk=q2,#7
vshl.i32 q10,q2,#7

# qhasm: 4x s2_blk unsigned>>= 25
# asm 1: vshr.u32 >s2_blk=reg128#3,<s2_blk=reg128#3,#25
# asm 2: vshr.u32 >s2_blk=q2,<s2_blk=q2,#25
vshr.u32 q2,q2,#25

# qhasm: s2_blk ^= temp
# asm 1: veor >s2_blk=reg128#3,<s2_blk=reg128#3,<temp=reg128#11
# asm 2: veor >s2_blk=q2,<s2_blk=q2,<temp=q10
veor q2,q2,q10

# qhasm: s0_blk = s0_blk[1,2,3]s0_blk[0]
# asm 1: vext.32 >s0_blk=reg128#1,<s0_blk=reg128#1,<s0_blk=reg128#1,#1
# asm 2: vext.32 >s0_blk=q0,<s0_blk=q0,<s0_blk=q0,#1
vext.32 q0,q0,q0,#1

# qhasm: s3_blk ^= s1_blk
# asm 1: veor >s3_blk=reg128#4,<s3_blk=reg128#4,<s1_blk=reg128#2
# asm 2: veor >s3_blk=q3,<s3_blk=q3,<s1_blk=q1
veor q3,q3,q1

# qhasm: s3_blk ^= msg_blk
# asm 1: veor >s3_blk=reg128#4,<s3_blk=reg128#4,<msg_blk=reg128#10
# asm 2: veor >s3_blk=q3,<s3_blk=q3,<msg_blk=q9
veor q3,q3,q9

# qhasm: temp = s4_blk & s0_blk
# asm 1: vand >temp=reg128#11,<s4_blk=reg128#9,<s0_blk=reg128#1
# asm 2: vand >temp=q10,<s4_blk=q8,<s0_blk=q0
vand q10,q8,q0

# qhasm: s3_blk ^= temp
# asm 1: veor >s3_blk=reg128#4,<s3_blk=reg128#4,<temp=reg128#11
# asm 2: veor >s3_blk=q3,<s3_blk=q3,<temp=q10
veor q3,q3,q10

# qhasm: 4x temp = s3_blk << 22
# asm 1: vshl.i32 >temp=reg128#11,<s3_blk=reg128#4,#22
# asm 2: vshl.i32 >temp=q10,<s3_blk=q3,#22
vshl.i32 q10,q3,#22

# qhasm: 4x s3_blk unsigned>>= 10
# asm 1: vshr.u32 >s3_blk=reg128#4,<s3_blk=reg128#4,#10
# asm 2: vshr.u32 >s3_blk=q3,<s3_blk=q3,#10
vshr.u32 q3,q3,#10

# qhasm: s3_blk ^= temp
# asm 1: veor >s3_blk=reg128#4,<s3_blk=reg128#4,<temp=reg128#11
# asm 2: veor >s3_blk=q3,<s3_blk=q3,<temp=q10
veor q3,q3,q10

# qhasm: s1_blk = s1_blk[2,3]s1_blk[0,1]
# asm 1: vswp <s1_blk=reg128#2%bot,<s1_blk=reg128#2%top
# asm 2: vswp <s1_blk=d2,<s1_blk=d3
vswp d2,d3

# qhasm: s4_blk ^= s2_blk
# asm 1: veor >s4_blk=reg128#9,<s4_blk=reg128#9,<s2_blk=reg128#3
# asm 2: veor >s4_blk=q8,<s4_blk=q8,<s2_blk=q2
veor q8,q8,q2

# qhasm: s4_blk ^= msg_blk
# asm 1: veor >s4_blk=reg128#9,<s4_blk=reg128#9,<msg_blk=reg128#10
# asm 2: veor >s4_blk=q8,<s4_blk=q8,<msg_blk=q9
veor q8,q8,q9

# qhasm: s2_blk = s2_blk[3]s2_blk[0,1,2]
# asm 1: vext.32 >s2_blk=reg128#3,<s2_blk=reg128#3,<s2_blk=reg128#3,#3
# asm 2: vext.32 >s2_blk=q2,<s2_blk=q2,<s2_blk=q2,#3
vext.32 q2,q2,q2,#3

# qhasm: temp = s0_blk & s1_blk
# asm 1: vand >temp=reg128#10,<s0_blk=reg128#1,<s1_blk=reg128#2
# asm 2: vand >temp=q9,<s0_blk=q0,<s1_blk=q1
vand q9,q0,q1

# qhasm: s4_blk ^= temp
# asm 1: veor >s4_blk=reg128#9,<s4_blk=reg128#9,<temp=reg128#10
# asm 2: veor >s4_blk=q8,<s4_blk=q8,<temp=q9
veor q8,q8,q9

# qhasm: 4x temp = s4_blk << 13
# asm 1: vshl.i32 >temp=reg128#10,<s4_blk=reg128#9,#13
# asm 2: vshl.i32 >temp=q9,<s4_blk=q8,#13
vshl.i32 q9,q8,#13

# qhasm: 4x s4_blk unsigned>>= 19
# asm 1: vshr.u32 >s4_blk=reg128#9,<s4_blk=reg128#9,#19
# asm 2: vshr.u32 >s4_blk=q8,<s4_blk=q8,#19
vshr.u32 q8,q8,#19

# qhasm: s4_blk ^= temp
# asm 1: veor >s4_blk=reg128#9,<s4_blk=reg128#9,<temp=reg128#10
# asm 2: veor >s4_blk=q8,<s4_blk=q8,<temp=q9
veor q8,q8,q9

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

# qhasm: input_2 -= 64
# asm 1: sub >input_2=int32#1,<input_2=int32#3,#64
# asm 2: sub >input_2=r0,<input_2=r2,#64
sub r0,r2,#64

# qhasm: mem128[input_2] aligned= s0_blk
# asm 1: vst1.8 {<s0_blk=reg128#1%bot-<s0_blk=reg128#1%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s0_blk=d0-<s0_blk=d1},[<input_2=r0,: 128]
vst1.8 {d0-d1},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s1_blk
# asm 1: vst1.8 {<s1_blk=reg128#2%bot-<s1_blk=reg128#2%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s1_blk=d2-<s1_blk=d3},[<input_2=r0,: 128]
vst1.8 {d2-d3},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s2_blk
# asm 1: vst1.8 {<s2_blk=reg128#3%bot-<s2_blk=reg128#3%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s2_blk=d4-<s2_blk=d5},[<input_2=r0,: 128]
vst1.8 {d4-d5},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s3_blk
# asm 1: vst1.8 {<s3_blk=reg128#4%bot-<s3_blk=reg128#4%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s3_blk=d6-<s3_blk=d7},[<input_2=r0,: 128]
vst1.8 {d6-d7},[r0,: 128]

# qhasm: input_2 += 16
# asm 1: add >input_2=int32#1,<input_2=int32#1,#16
# asm 2: add >input_2=r0,<input_2=r0,#16
add r0,r0,#16

# qhasm: mem128[input_2] aligned= s4_blk
# asm 1: vst1.8 {<s4_blk=reg128#9%bot-<s4_blk=reg128#9%top},[<input_2=int32#1,: 128]
# asm 2: vst1.8 {<s4_blk=d16-<s4_blk=d17},[<input_2=r0,: 128]
vst1.8 {d16-d17},[r0,: 128]

# qhasm: caller_r4 = r4_stack
# asm 1: ldr >caller_r4=int32#5,<r4_stack=stack32#1
# asm 2: ldr >caller_r4=r4,<r4_stack=[sp,#0]
ldr r4,[sp,#0]

# qhasm: caller_r5 = r5_stack
# asm 1: ldr >caller_r5=int32#6,<r5_stack=stack32#2
# asm 2: ldr >caller_r5=r5,<r5_stack=[sp,#4]
ldr r5,[sp,#4]

# qhasm: return
add sp,sp,#32
bx lr
