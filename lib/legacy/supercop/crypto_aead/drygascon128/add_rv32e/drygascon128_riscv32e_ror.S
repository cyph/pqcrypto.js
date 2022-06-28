//DryGASCON128 F and G function for RISC-V RV32E machines
//Assuming p.ror instruction IS present
//We assume bare metal environment: gp and tp are treated as callee saved general purposes registers
.global drygascon128_g
.global drygascon128_f
//stack frame for G:
	.equ G_STATE,  20
	.equ G_ROUNDS, 24 //address to read the next round constant
	.equ G_ROUND,  28 //count from round-1 to 0
//stack frame for F:
    .equ MIX_BUF, 0
    .equ MIX_BUF_SIZE, (14*2)
    .equ MIX_STACK_SIZE, MIX_BUF_SIZE
	.equ F_STATE,  (G_STATE+MIX_STACK_SIZE)
	.equ F_ROUNDS, (G_ROUNDS+MIX_STACK_SIZE)
	.equ F_ROUND,  (G_ROUND+MIX_STACK_SIZE)

//registers allocation:
	.equ C0L, x4
	.equ C1L, x5
	.equ C2L, x6
	.equ C3L, x7
	.equ C4L, x8
	.equ C0H, x9
	.equ C1H, x10
	.equ C2H, x11
	.equ C3H, x12
	.equ C4H, x13
//working registers
	.equ W0, x1
	.equ W1, x3
	.equ W2, x14
	.equ W4, x15


//offsets in memory, based on state pointer
	.equ R0L,40
	.equ R1L,48
	.equ X32_0, 56
	.equ X32_1, 64
	.equ X32_2, 72
	.equ X32_3, 80

	.equ R0H, (R0L+4)
	.equ R1H, (R1L+4)

    .equ R32_0,R0L
    .equ R32_1,R0H
    .equ R32_2,R1L
    .equ R32_3,R1H


.section .rodata
round_cst:
.byte 0x4b
.byte 0x5a
.byte 0x69
.byte 0x78
.byte 0x87
.byte 0x96
.byte 0xa5
.byte 0xb4
.byte 0xc3
.byte 0xd2
.byte 0xe1
.byte 0xf0
.align 4

.section .text
.type	drygascon128_g, %function
drygascon128_g:
//save context
    addi    sp,sp,-32
	sw		s0, 0(sp)
	sw		s1, 4(sp)
	sw		gp, 8(sp)
	sw		tp,12(sp)
	sw 		ra,16(sp)
//set stack frame
	sw		a0,G_STATE(sp)
	//round=rounds-1
	addi	x1,a1,-1
	sw		x1,G_ROUND(sp)
	//base = round_cst+12-round
    la      x14,round_cst
    addi	x14,x14,12
    sub		x3,x14,a1
	sw		x3,G_ROUNDS(sp)
//load state
	mv		x15,a0
	lw		x4, 0(x15)
	lw		x5, 4(x15)
	lw		x6, 8(x15)
	lw		x7,12(x15)
	lw		x8,16(x15)
	lw		x9,20(x15)
	lw		x10,24(x15)
	lw		x11,28(x15)
	lw		x12,32(x15)
	lw		x13,36(x15)

drygascon128_g_entry_from_f:
//init R
	sw		zero,R32_0(x15)
	sw		zero,R32_1(x15)
	sw		zero,R32_2(x15)
	sw		zero,R32_3(x15)

    //loop entry
	//assume r1>0 at entry
drygascon128_g_main_loop:
    //x15: state pointer
    //x3: base for round constants
    //x1: round, counting from rounds-1 to 0
    add     x3,x3,x1
    lbu   	x3,0(x3)
        // addition of round constant
    xor	    x8,x8,x3

    // substitution layer, lower half
	xor 	x4,x4,x12
    xor 	x12,x12,x10
    xor 	x8,x8,x6
	not 	x1,x4
    not 	x3,x10
    not 	x14,x12
	and 	x1,x1,x6
    and 	x3,x3,x12
    xor 	x12,x12,x1
    and 	x14,x14,x4
    not 	x1,x8
    and 	x1,x1,x10
    xor 	x10,x10,x14
    not 	x14,x6
    and 	x14,x14,x8
    xor 	x8,x8,x3
    xor 	x10,x10,x8
    not 	x8,x8
    xor 	x4,x4,x14
    xor 	x6,x6,x1
	xor 	x6,x6,x4
    xor 	x4,x4,x12

    // substitution layer, upper half
	xor 	x5,x5,x13
    xor 	x13,x13,x11
    xor 	x9,x9,x7
	not 	x1,x5
    not 	x3,x11
    not 	x14,x13
	and 	x1,x1,x7
    and 	x3,x3,x13
    xor 	x13,x13,x1
    and 	x14,x14,x5
    not 	x1,x9
    and 	x1,x1,x11
    xor 	x11,x11,x14
    not 	x14,x7
    and 	x14,x14,x9
    xor 	x9,x9,x3
    xor 	x11,x11,x9
    not 	x9,x9
    xor 	x5,x5,x14
    xor 	x7,x7,x1
	xor 	x7,x7,x5
    xor 	x5,x5,x13

    // linear diffusion layer
    //c4 ^= gascon_rotr64_interleaved(c4, 40) ^ gascon_rotr64_interleaved(c4, 7);
    //c4 high part
    mv      x3,x13
    li     x1, 20
    p.ror    x3,x3,x1
    xor     x13,x13,x3
    mv      x1,x12
    li     x14, 4
    p.ror    x1,x1,x14
    xor     x13,x13,x1
    //c4 low part
    mv      x1,x12
    li     x14, (32-20+3)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x12
    li     x14, 20
    p.ror    x1,x1,x14
    xor     x12,x3,x1

    //c0 ^= gascon_rotr64_interleaved(c0, 28) ^ gascon_rotr64_interleaved(c0, 19);
    //c0 high part
    mv      x3,x5
    li     x1, 14
    p.ror    x3,x3,x1
    xor     x5,x5,x3
    mv      x1,x4
    li     x14, 10
    p.ror    x1,x1,x14
    xor     x5,x5,x1
    lw      x14,R32_1(x15)
    xor     x14,x14,x5
    sw      x14,R32_1(x15)
    //c0 low part
    mv      x1,x4
    li     x14, (32-14+9)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x4
    li     x14, 14
    p.ror    x1,x1,x14
    xor     x4,x3,x1
    lw      x14,R32_0(x15)
    xor     x14,x14,x4
    sw      x14,R32_0(x15)

    //c1 ^= gascon_rotr64_interleaved(c1, 38) ^ gascon_rotr64_interleaved(c1, 61);
    //c1 high part
    mv      x3,x7
    li     x1, 19
    p.ror    x3,x3,x1
    xor     x7,x7,x3
    mv      x1,x6
    li     x14, 31
    p.ror    x1,x1,x14
    xor     x7,x7,x1
    lw      x14,R32_3(x15)
    xor     x14,x14,x7
    sw      x14,R32_3(x15)
    //c1 low part
    mv      x1,x6
    li     x14, (32-19+30)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x6
    li     x14, 19
    p.ror    x1,x1,x14
    xor     x6,x3,x1
    lw      x14,R32_2(x15)
    xor     x14,x14,x6
    sw      x14,R32_2(x15)

    //c2 ^= gascon_rotr64_interleaved(c2, 6) ^ gascon_rotr64_interleaved(c2, 1);
    //c2 high part
    mv      x3,x9
    li     x1, 3
    p.ror    x3,x3,x1
    xor     x9,x9,x3
    mv      x1,x8
    li     x14, 1
    p.ror    x1,x1,x14
    xor     x9,x9,x1
    lw      x14,R32_0(x15)
    xor     x14,x14,x9
    sw      x14,R32_0(x15)
    //c2 low part
    mv      x1,x8
    li     x14, (32-3+0)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x8
    li     x14, 3
    p.ror    x1,x1,x14
    xor     x8,x3,x1
    lw      x14,R32_3(x15)
    xor     x14,x14,x8
    sw      x14,R32_3(x15)

    //c3 ^= gascon_rotr64_interleaved(c3, 10) ^ gascon_rotr64_interleaved(c3, 17);
    //c3 high part
    mv      x3,x11
    li     x1, 5
    p.ror    x3,x3,x1
    xor     x11,x11,x3
    mv      x1,x10
    li     x14, 9
    p.ror    x1,x1,x14
    xor     x11,x11,x1
    lw      x14,R32_2(x15)
    xor     x14,x14,x11
    sw      x14,R32_2(x15)
    //c3 low part
    mv      x1,x10
    li     x14, (32-5+8)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x10
    li     x14, 5
    p.ror    x1,x1,x14
    xor     x10,x3,x1
    lw      x14,R32_1(x15)
    xor     x14,x14,x10
    sw      x14,R32_1(x15)

    lw      x3,G_ROUNDS(sp)
    lw      x1,G_ROUND(sp)
    addi    x1,x1,-1
    blt     x1,zero,drygascon128_g_exit

    sw      x1,G_ROUND(sp)
	j    	drygascon128_g_main_loop
drygascon128_g_exit:

//store state
    sw		x4, 0(x15)
    sw		x5, 4(x15)
    sw		x6, 8(x15)
    sw		x7,12(x15)
    sw		x8,16(x15)
    sw		x9,20(x15)
    sw		x10,24(x15)
    sw		x11,28(x15)
    sw		x12,32(x15)
    sw		x13,36(x15)

//restore context
	lw		s0, 0(sp)
	lw		s1, 4(sp)
    lw      gp, 8(sp)
	lw		tp,12(sp)
	lw 		ra,16(sp)
    addi    sp,sp,32
    ret
.size	drygascon128_g, .-drygascon128_g


.type	drygascon128_f, %function
drygascon128_f:
    //a0:state c r x
    //a1:input -> shall be 32 bit aligned
    //a2:ds
    //a3:rounds
//save context
    addi    sp,sp,-32
	sw		s0, 0(sp)
	sw		s1, 4(sp)
	sw		gp, 8(sp)
	sw		tp,12(sp)
	sw 		ra,16(sp)
//set stack frame
	sw		a0,G_STATE(sp)
    sw      a3,G_ROUNDS(sp)
    addi    sp,sp,-MIX_STACK_SIZE

    li      x1,0x3FF

    lw      x15, 0(a1)
    and     x3,x15,x1
    sh      x3,MIX_BUF+26(sp)

    srli    x15,x15,10
    and     x3,x15,x1
    sh      x3,MIX_BUF+24(sp)

    srli    x15,x15,10
    and     x3,x15,x1
    sh      x3,MIX_BUF+22(sp)

    srli    x3,x15,10
    lw      x15, 4(a1)
    slli    x14,x15,2
    srli    x15,x15,8
    or      x14,x14,x3
    and     x3,x14,x1
    sh      x3,MIX_BUF+20(sp)
    and     x3,x15,x1
    sh      x3,MIX_BUF+18(sp)

    srli    x15,x15,10
    and     x3,x15,x1
    sh      x3,MIX_BUF+16(sp)

    srli    x3,x15,10
    lw      x15, 8(a1)
    slli    x14,x15,4
    srli    x15,x15,6
    or      x14,x14,x3
    and     x3,x14,x1
    sh      x3,MIX_BUF+14(sp)
    and     x3,x15,x1
    sh      x3,MIX_BUF+12(sp)

    srli    x15,x15,10
    and     x3,x15,x1
    sh      x3,MIX_BUF+10(sp)

    srli    x3,x15,10
    lw      x15, 12(a1)
    slli    x14,x15,6
    srli    x15,x15,4
    or      x14,x14,x3
    and     x3,x14,x1
    sh      x3,MIX_BUF+8(sp)
    and     x3,x15,x1
    sh      x3,MIX_BUF+6(sp)

    srli    x15,x15,10
    and     x3,x15,x1
    sh      x3,MIX_BUF+4(sp)

    srli    x3,x15,10
    mv      x15, a2
    slli    x14,x15,8
    srli    x15,x15,2
    or      x14,x14,x3
    and     x3,x14,x1
    sh      x3,MIX_BUF+2(sp)
    and     x3,x15,x1
    sh      x3,MIX_BUF+0(sp)

//load state
	mv		x15,a0
	lw		x4, 0(x15)
	lw		x5, 4(x15)
	lw		x6, 8(x15)
	lw		x7,12(x15)
	lw		x8,16(x15)
	lw		x9,20(x15)
	lw		x10,24(x15)
	lw		x11,28(x15)
	lw		x12,32(x15)
	lw		x13,36(x15)

    li      x1,26
    sw      x1,F_ROUND(sp)
drygascon128_f_mix128_main_loop:
    //x1 is the offset in stack to the 10 bits input
    add     x1,x1,sp
    lh      x1,0(x1)
    //x1 is the 10 bits input

    //x15 is the pointer to the state C
    //x14 is the pointer to X
    addi    x14,x15,40+16

    andi    x3,x1,0x3
    slli    x3,x3,2
    add     x3,x3,x14
    lw      x3,0(x3)
    xor     x4,x4,x3

    andi    x3,x1,0xc
    add     x3,x3,x14
    lw      x3,0(x3)
    xor     x6,x6,x3

    srli    x1,x1,2
    andi    x3,x1,0xc
    add     x3,x3,x14
    lw      x3,0(x3)
    xor     x8,x8,x3

    srli    x1,x1,2
    andi    x3,x1,0xc
    add     x3,x3,x14
    lw      x3,0(x3)
    xor     x10,x10,x3

    srli    x1,x1,2
    andi    x3,x1,0xc
    add     x3,x3,x14
    lw      x3,0(x3)
    xor     x12,x12,x3

    lw      x1,F_ROUND(sp)
    addi    x1,x1,-2
    blt     x1,zero,drygascon128_f_mix128_exit
drygascon128_f_mix128_coreround:
    sw      x1,F_ROUND(sp)
    li      x3,0xf0
        // addition of round constant
    xor	    x8,x8,x3

    // substitution layer, lower half
	xor 	x4,x4,x12
    xor 	x12,x12,x10
    xor 	x8,x8,x6
	not 	x1,x4
    not 	x3,x10
    not 	x14,x12
	and 	x1,x1,x6
    and 	x3,x3,x12
    xor 	x12,x12,x1
    and 	x14,x14,x4
    not 	x1,x8
    and 	x1,x1,x10
    xor 	x10,x10,x14
    not 	x14,x6
    and 	x14,x14,x8
    xor 	x8,x8,x3
    xor 	x10,x10,x8
    not 	x8,x8
    xor 	x4,x4,x14
    xor 	x6,x6,x1
	xor 	x6,x6,x4
    xor 	x4,x4,x12

    // substitution layer, upper half
	xor 	x5,x5,x13
    xor 	x13,x13,x11
    xor 	x9,x9,x7
	not 	x1,x5
    not 	x3,x11
    not 	x14,x13
	and 	x1,x1,x7
    and 	x3,x3,x13
    xor 	x13,x13,x1
    and 	x14,x14,x5
    not 	x1,x9
    and 	x1,x1,x11
    xor 	x11,x11,x14
    not 	x14,x7
    and 	x14,x14,x9
    xor 	x9,x9,x3
    xor 	x11,x11,x9
    not 	x9,x9
    xor 	x5,x5,x14
    xor 	x7,x7,x1
	xor 	x7,x7,x5
    xor 	x5,x5,x13

    // linear diffusion layer
    //c4 ^= gascon_rotr64_interleaved(c4, 40) ^ gascon_rotr64_interleaved(c4, 7);
    //c4 high part
    mv      x3,x13
    li     x1, 20
    p.ror    x3,x3,x1
    xor     x13,x13,x3
    mv      x1,x12
    li     x14, 4
    p.ror    x1,x1,x14
    xor     x13,x13,x1
    //c4 low part
    mv      x1,x12
    li     x14, (32-20+3)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x12
    li     x14, 20
    p.ror    x1,x1,x14
    xor     x12,x3,x1

    //c0 ^= gascon_rotr64_interleaved(c0, 28) ^ gascon_rotr64_interleaved(c0, 19);
    //c0 high part
    mv      x3,x5
    li     x1, 14
    p.ror    x3,x3,x1
    xor     x5,x5,x3
    mv      x1,x4
    li     x14, 10
    p.ror    x1,x1,x14
    xor     x5,x5,x1
    //c0 low part
    mv      x1,x4
    li     x14, (32-14+9)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x4
    li     x14, 14
    p.ror    x1,x1,x14
    xor     x4,x3,x1

    //c1 ^= gascon_rotr64_interleaved(c1, 38) ^ gascon_rotr64_interleaved(c1, 61);
    //c1 high part
    mv      x3,x7
    li     x1, 19
    p.ror    x3,x3,x1
    xor     x7,x7,x3
    mv      x1,x6
    li     x14, 31
    p.ror    x1,x1,x14
    xor     x7,x7,x1
    //c1 low part
    mv      x1,x6
    li     x14, (32-19+30)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x6
    li     x14, 19
    p.ror    x1,x1,x14
    xor     x6,x3,x1

    //c2 ^= gascon_rotr64_interleaved(c2, 6) ^ gascon_rotr64_interleaved(c2, 1);
    //c2 high part
    mv      x3,x9
    li     x1, 3
    p.ror    x3,x3,x1
    xor     x9,x9,x3
    mv      x1,x8
    li     x14, 1
    p.ror    x1,x1,x14
    xor     x9,x9,x1
    //c2 low part
    mv      x1,x8
    li     x14, (32-3+0)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x8
    li     x14, 3
    p.ror    x1,x1,x14
    xor     x8,x3,x1

    //c3 ^= gascon_rotr64_interleaved(c3, 10) ^ gascon_rotr64_interleaved(c3, 17);
    //c3 high part
    mv      x3,x11
    li     x1, 5
    p.ror    x3,x3,x1
    xor     x11,x11,x3
    mv      x1,x10
    li     x14, 9
    p.ror    x1,x1,x14
    xor     x11,x11,x1
    //c3 low part
    mv      x1,x10
    li     x14, (32-5+8)%32
    p.ror    x3,x3,x14
    xor     x3,x3,x10
    li     x14, 5
    p.ror    x1,x1,x14
    xor     x10,x3,x1

    lw      x1,F_ROUND(sp)
    j       drygascon128_f_mix128_main_loop

drygascon128_f_mix128_exit:
    lw      x3,F_ROUNDS(sp)
    //round=rounds-1
    addi	x1,x3,-1
    sw		x1,F_ROUND(sp)
    //base = round_cst+12-round
    la      x14,round_cst
    addi	x14,x14,12
    sub		x3,x14,x3
    sw		x3,F_ROUNDS(sp)

    addi    sp,sp,MIX_STACK_SIZE
    j       drygascon128_g_entry_from_f
.size	drygascon128_f, .-drygascon128_f
