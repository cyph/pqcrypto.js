``
set HAS_ROTR 1
``
//DryGASCON128 F and G function for RISC-V RV32E machines
``if {$HAS_ROTR} {``
//Assuming p.ror instruction IS present
``} else {``
//Assuming p.ror instruction IS NOT present
``}``
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
``
proc STATE  {} {return "G_STATE"}
proc ROUNDS {} {return "G_ROUNDS"}
proc ROUND  {} {return "G_ROUND"}
``

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

``
proc C0L {} {return "x4"}
proc C0H {} {return "x5"}
proc C1L {} {return "x6"}
proc C1H {} {return "x7"}
proc C2L {} {return "x8"}
proc C2H {} {return "x9"}
proc C3L {} {return "x10"}
proc C3H {} {return "x11"}
proc C4L {} {return "x12"}
proc C4H {} {return "x13"}

proc W0 {} {return "x1"}
proc W1 {} {return "x3"}
proc W2 {} {return "x14"}
proc W3 {} {return "x15"}



proc sublayer {part r0 r1 r2 r3 r4 r5 r6 r7} {
    ``// substitution layer, `$part` half
	xor 	`$r0`,`$r0`,`$r4`
    xor 	`$r4`,`$r4`,`$r3`
    xor 	`$r2`,`$r2`,`$r1`
	not 	`$r5`,`$r0`
    not 	`$r6`,`$r3`
    not 	`$r7`,`$r4`
	and 	`$r5`,`$r5`,`$r1`
    and 	`$r6`,`$r6`,`$r4`
    xor 	`$r4`,`$r4`,`$r5`
    and 	`$r7`,`$r7`,`$r0`
    not 	`$r5`,`$r2`
    and 	`$r5`,`$r5`,`$r3`
    xor 	`$r3`,`$r3`,`$r7`
    not 	`$r7`,`$r1`
    and 	`$r7`,`$r7`,`$r2`
    xor 	`$r2`,`$r2`,`$r6`
    xor 	`$r3`,`$r3`,`$r2`
    not 	`$r2`,`$r2`
    xor 	`$r0`,`$r0`,`$r7`
    xor 	`$r1`,`$r1`,`$r5`
	xor 	`$r1`,`$r1`,`$r0`
    xor 	`$r0`,`$r0`,`$r4`
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}

if {$HAS_ROTR} {
::tgpp::renameOutput "drygascon128_riscv32e_ror.S"
proc rotr32 {reg shift tmp} {
    ``li     `$tmp`, `$shift`
    p.ror    `$reg`,`$reg`,`$tmp`
``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}
} else {
proc rotr32 {reg shift tmp} {
    ``slli    `$tmp`,`$reg`,`expr 32-$shift`
    srli    `$reg`,`$reg`,`$shift`
    or      `$reg`,`$reg`,`$tmp`
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}
}

proc xr32 {d s shift tmp} {
    ```rotr32 $s $shift $tmp`
    xor     `$d`,`$d`,`$s`
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}
proc rx32 {d s shift tmp} {
    ```rotr32 $d $shift $tmp`
    xor     `$d`,`$d`,`$s`
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}
proc birotr64 {i eshift oshift accumulate} {
    set s0 [expr $eshift>>1]
    set s1 [expr ($oshift>>1)+1]
    set s2 [expr $s1-1]
    set s2 (32-$s0+$s2)%32
    set s3 $s0
    if $i<2 {
        set ril [expr (2*$i)]
        set rih [expr (2*$i)+1]
    }
    if $i>=2 {
        set ril [expr (2*($i-2)+3)%4]
        set rih [expr (2*($i-2)+0)%4]
    }

    set xil "x[expr 2*$i+4]"
    set xih "x[expr 2*$i+5]"
    ``//c`$i` ^= gascon_rotr64_interleaved(c`$i`, `$eshift`) ^ gascon_rotr64_interleaved(c`$i`, `$oshift`);
    //c`$i` high part
    mv      `W1`,`$xih`
    `xr32   $xih [W1] $s0 [W0]`
    mv      `W0`,`$xil`
    `xr32   $xih [W0] $s1 [W2]`
    ``
    if $accumulate {
``    lw      `W2`,R32_`$rih`(`W3`)
    xor     `W2`,`W2`,`$xih`
    sw      `W2`,R32_`$rih`(`W3`)
    ``
    }
    ``
    //c`$i` low part
    mv      `W0`,`$xil`
    `rx32   [W1] $xil $s2 [W2]`
    `rotr32   [W0] $s3 [W2]`
    xor     `$xil`,`W1`,`W0`
    ``
    if $accumulate {
``    lw      `W2`,R32_`$ril`(`W3`)
    xor     `W2`,`W2`,`$xil`
    sw      `W2`,R32_`$ril`(`W3`)
    ``
    }
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}

proc core_round {accumulate} {
    #Input state:
    #`W1`: constant to add as round contant
    #`W3`: state pointer
    #Output state:
    #`W3`: state pointer
    ``
    // addition of round constant
    xor	    `C2L`,`C2L`,`W1`

    `sublayer lower [C0L] [C1L] [C2L] [C3L] [C4L] [W0] [W1] [W2]`

    `sublayer upper [C0H] [C1H] [C2H] [C3H] [C4H] [W0] [W1] [W2]`

    // linear diffusion layer
    `birotr64 4 40  7 0`

    `birotr64 0 28 19 $accumulate`

    `birotr64 1 38 61 $accumulate`

    `birotr64 2  6  1 $accumulate`

    `birotr64 3 10 17 $accumulate`
``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}
``

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
	sw		a0,`STATE`(sp)
	//round=rounds-1
	addi	`W0`,a1,-1
	sw		`W0`,`ROUND`(sp)
	//base = round_cst+12-round
    la      `W2`,round_cst
    addi	`W2`,`W2`,12
    sub		`W1`,`W2`,a1
	sw		`W1`,`ROUNDS`(sp)
//load state
	mv		`W3`,a0
	lw		`C0L`, 0(`W3`)
	lw		`C0H`, 4(`W3`)
	lw		`C1L`, 8(`W3`)
	lw		`C1H`,12(`W3`)
	lw		`C2L`,16(`W3`)
	lw		`C2H`,20(`W3`)
	lw		`C3L`,24(`W3`)
	lw		`C3H`,28(`W3`)
	lw		`C4L`,32(`W3`)
	lw		`C4H`,36(`W3`)

drygascon128_g_entry_from_f:
//init R
	sw		zero,R32_0(`W3`)
	sw		zero,R32_1(`W3`)
	sw		zero,R32_2(`W3`)
	sw		zero,R32_3(`W3`)

    //loop entry
	//assume r1>0 at entry
drygascon128_g_main_loop:
    //`W3`: state pointer
    //`W1`: base for round constants
    //`W0`: round, counting from rounds-1 to 0
    add     `W1`,`W1`,`W0`
    lbu   	`W1`,0(`W1`)
    `core_round 1`

    lw      `W1`,`ROUNDS`(sp)
    lw      `W0`,`ROUND`(sp)
    addi    `W0`,`W0`,-1
    blt     `W0`,zero,drygascon128_g_exit

    sw      `W0`,`ROUND`(sp)
	j    	drygascon128_g_main_loop
drygascon128_g_exit:

//store state
    sw		`C0L`, 0(`W3`)
    sw		`C0H`, 4(`W3`)
    sw		`C1L`, 8(`W3`)
    sw		`C1H`,12(`W3`)
    sw		`C2L`,16(`W3`)
    sw		`C2H`,20(`W3`)
    sw		`C3L`,24(`W3`)
    sw		`C3H`,28(`W3`)
    sw		`C4L`,32(`W3`)
    sw		`C4H`,36(`W3`)

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
	sw		a0,`STATE`(sp)
    sw      a3,`ROUNDS`(sp)
``
proc BUF    {} {return "MIX_BUF"}
proc STATE  {} {return "F_STATE"}
proc ROUNDS {} {return "F_ROUNDS"}
proc ROUND  {} {return "F_ROUND"}
``
    addi    sp,sp,-MIX_STACK_SIZE

    li      `W0`,0x3FF

    lw      `W3`, 0(a1)
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+26(sp)

    srli    `W3`,`W3`,10
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+24(sp)

    srli    `W3`,`W3`,10
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+22(sp)

    srli    `W1`,`W3`,10
    lw      `W3`, 4(a1)
    slli    `W2`,`W3`,2
    srli    `W3`,`W3`,8
    or      `W2`,`W2`,`W1`
    and     `W1`,`W2`,`W0`
    sh      `W1`,`BUF`+20(sp)
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+18(sp)

    srli    `W3`,`W3`,10
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+16(sp)

    srli    `W1`,`W3`,10
    lw      `W3`, 8(a1)
    slli    `W2`,`W3`,4
    srli    `W3`,`W3`,6
    or      `W2`,`W2`,`W1`
    and     `W1`,`W2`,`W0`
    sh      `W1`,`BUF`+14(sp)
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+12(sp)

    srli    `W3`,`W3`,10
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+10(sp)

    srli    `W1`,`W3`,10
    lw      `W3`, 12(a1)
    slli    `W2`,`W3`,6
    srli    `W3`,`W3`,4
    or      `W2`,`W2`,`W1`
    and     `W1`,`W2`,`W0`
    sh      `W1`,`BUF`+8(sp)
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+6(sp)

    srli    `W3`,`W3`,10
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+4(sp)

    srli    `W1`,`W3`,10
    mv      `W3`, a2
    slli    `W2`,`W3`,8
    srli    `W3`,`W3`,2
    or      `W2`,`W2`,`W1`
    and     `W1`,`W2`,`W0`
    sh      `W1`,`BUF`+2(sp)
    and     `W1`,`W3`,`W0`
    sh      `W1`,`BUF`+0(sp)

//load state
	mv		`W3`,a0
	lw		`C0L`, 0(`W3`)
	lw		`C0H`, 4(`W3`)
	lw		`C1L`, 8(`W3`)
	lw		`C1H`,12(`W3`)
	lw		`C2L`,16(`W3`)
	lw		`C2H`,20(`W3`)
	lw		`C3L`,24(`W3`)
	lw		`C3H`,28(`W3`)
	lw		`C4L`,32(`W3`)
	lw		`C4H`,36(`W3`)

    li      `W0`,26
    sw      `W0`,`ROUND`(sp)
drygascon128_f_mix128_main_loop:
    //`W0` is the offset in stack to the 10 bits input
    add     `W0`,`W0`,sp
    lh      `W0`,0(`W0`)
    //`W0` is the 10 bits input

    //`W3` is the pointer to the state C
    //`W2` is the pointer to X
    addi    `W2`,`W3`,40+16

    andi    `W1`,`W0`,0x3
    slli    `W1`,`W1`,2
    add     `W1`,`W1`,`W2`
    lw      `W1`,0(`W1`)
    xor     `C0L`,`C0L`,`W1`

    andi    `W1`,`W0`,0xc
    add     `W1`,`W1`,`W2`
    lw      `W1`,0(`W1`)
    xor     `C1L`,`C1L`,`W1`

    srli    `W0`,`W0`,2
    andi    `W1`,`W0`,0xc
    add     `W1`,`W1`,`W2`
    lw      `W1`,0(`W1`)
    xor     `C2L`,`C2L`,`W1`

    srli    `W0`,`W0`,2
    andi    `W1`,`W0`,0xc
    add     `W1`,`W1`,`W2`
    lw      `W1`,0(`W1`)
    xor     `C3L`,`C3L`,`W1`

    srli    `W0`,`W0`,2
    andi    `W1`,`W0`,0xc
    add     `W1`,`W1`,`W2`
    lw      `W1`,0(`W1`)
    xor     `C4L`,`C4L`,`W1`

    lw      `W0`,`ROUND`(sp)
    addi    `W0`,`W0`,-2
    blt     `W0`,zero,drygascon128_f_mix128_exit
drygascon128_f_mix128_coreround:
    sw      `W0`,`ROUND`(sp)
    li      `W1`,0xf0
    `core_round 0`

    lw      `W0`,`ROUND`(sp)
    j       drygascon128_f_mix128_main_loop

drygascon128_f_mix128_exit:
    lw      `W1`,`ROUNDS`(sp)
    //round=rounds-1
    addi	`W0`,`W1`,-1
    sw		`W0`,`ROUND`(sp)
    //base = round_cst+12-round
    la      `W2`,round_cst
    addi	`W2`,`W2`,12
    sub		`W1`,`W2`,`W1`
    sw		`W1`,`ROUNDS`(sp)

    addi    sp,sp,MIX_STACK_SIZE
    j       drygascon128_g_entry_from_f
.size	drygascon128_f, .-drygascon128_f
