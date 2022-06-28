.cpu cortex-m0
.syntax unified
.code	16
.thumb_func

.align	1
.global	drygascon128_g
.global	drygascon128_f

	.equ C0, 0
	.equ C1, 8
	.equ C2, 16
	.equ C3, 24
	.equ C4, 32
	.equ R0, 40
	.equ R1, 48
	.equ X0, 56
	.equ X1, 64
	.equ X2, 72
	.equ X3, 80


	.equ C0L, C0
	.equ C1L, C1
	.equ C2L, C2
	.equ C3L, C3
	.equ C4L, C4
	.equ R0L, R0
	.equ R1L, R1
	.equ X0L, X0
	.equ X1L, X1
	.equ X2L, X2
	.equ X3L, X3

	.equ C0H, C0+4
	.equ C1H, C1+4
	.equ C2H, C2+4
	.equ C3H, C3+4
	.equ C4H, C4+4
	.equ R0H, R0+4
	.equ R1H, R1+4
	.equ X0H, X0+4
	.equ X1H, X1+4
	.equ X2H, X2+4
	.equ X3H, X3+4

    .equ R32_0,R0L
    .equ R32_1,R0H
    .equ R32_2,R1L
    .equ R32_3,R1H


``


proc rotr32 {reg shift tmp} {
    ``movs    `$tmp`,#(`$shift`)
    rors    `$reg`,`$reg`,`$tmp`
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}
proc xr32 {d s shift tmp} {
    ```rotr32 $s $shift $tmp`
    eors    `$d`,`$d`,`$s`
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}
proc rx32 {d s shift tmp} {
    ```rotr32 $d $shift $tmp`
    eors    `$d`,`$d`,`$s`
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

    if 4==$i {set tmp "r7"} else {set tmp "r4"}
    ``//c`$i` ^= gascon_rotr64_interleaved(c`$i`, `$eshift`) ^ gascon_rotr64_interleaved(c`$i`, `$oshift`);
    //c`$i` high part
    movs    r6,r`$i`
    `xr32   r6 r$i $s0 r5`
    ldr     r5,[r7,#C`$i`L]
    `xr32   r6 r5 $s1 $tmp`
    ``
    if 4==$i {
``    ldr		r7,[sp,#`$::stack_c`]
    ``
    }
    ``
    str     r6,[r7,#C`$i`H]
    ``
    if $accumulate {
``    ldr    `$tmp`,[r7,#R32_`$rih`]
    eors    `$tmp`,`$tmp`,r6
    str     `$tmp`,[r7,#R32_`$rih`]
    ``
    }
    ``
    //c`$i` low part
    movs    `$tmp`,#(32-`$s1`)
    rors    r5,r5,`$tmp`
    movs    r6,r5
    `rx32   r$i r6 $s2 $tmp`
    `xr32   r$i r5 $s3 $tmp`
    ``
    if $accumulate {
``    ldr    `$tmp`,[r7,#R32_`$ril`]
    eors    `$tmp`,`$tmp`,r`$i`
    str    `$tmp`,[r7,#R32_`$ril`]
    ``
    }
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}

proc sublayer {part} {
    ``// substitution layer, `$part` half
	eors	r0,r0,r4
    eors	r4,r4,r3
    eors	r2,r2,r1

	mvns	r5,r0
    mvns	r6,r3
    mvns	r7,r4
	ands	r5,r5,r1
    ands	r6,r6,r4
    eors	r4,r4,r5

    ands	r7,r7,r0
    mvns	r5,r2
    ands	r5,r5,r3
    eors	r3,r3,r7

    mvns	r7,r1
    ands	r7,r7,r2
    eors	r2,r2,r6

    eors	r3,r3,r2
    mvns	r2,r2

    eors	r0,r0,r7
    eors	r1,r1,r5
	eors	r1,r1,r0
    eors	r0,r0,r4
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}

proc core_round {accumulate} {
    #Input state:
    #r6: constant to add as round contant
    #r0 to r4: lower half of c
    #Output state:
    #r7: pointer on c
    #r0 to r3: lower half of c
    #r4 invalid
    ``
    // addition of round constant
    //r2 ^= r6;
    eors	r2,r2,r6

    `sublayer lower`

    ldr		r7,[sp,#`$::stack_c`]
    str		r4,[r7,#C4L]
    str		r3,[r7,#C3L]
    str		r2,[r7,#C2L]
    str		r1,[r7,#C1L]
    str		r0,[r7,#C0L]

    ldr		r4,[r7,#C4H]
    ldr		r3,[r7,#C3H]
    ldr		r2,[r7,#C2H]
    ldr		r1,[r7,#C1H]
    ldr		r0,[r7,#C0H]

    `sublayer upper`

    // linear diffusion layer
    ldr		r7,[sp,#`$::stack_c`]

    `birotr64 4 40  7 0`
    ldr		r7,[sp,#`$::stack_c`]
    str     r4,[r7,#C4L]

    `birotr64 0 28 19 $accumulate`

    `birotr64 1 38 61 $accumulate`

    `birotr64 2  6  1 $accumulate`

    `birotr64 3 10 17 $accumulate`
``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}

proc mix_step {ci} {
    if {$ci > 0} {
        set shift [expr 2*$ci-2]
        set d "r"
    } else {
        set shift 2
        set d "l"
    }
    set ci1 [expr $ci+1]
    ``ls`$d`s    r`$ci`,r6,#`$shift`
    ands    r`$ci`,r`$ci`,r4
    ldr     r`$ci1`,[r5,r`$ci`]
    ldr     r`$ci`,[r7,#`$ci`*8]
    eors    r`$ci`,r`$ci`,r`$ci1`
    ``
    #return all except the last new line character
    string range [::tgpp::getProcOutput] 0 end-1
}

``

.type	drygascon128_g, %function
drygascon128_g:
	//r0: state: c,r,x
	//r1: rounds
	push	{r4, r5, r6, r7, lr}
``
    set stack_c 0
    set stack_rounds 4
    set stack_round 8
    set stack_size 12
``
	//stack vars:
    // 8 round
	// 4 rounds
	// 0 state address

    //r=0
    movs    r5,#0
    str     r5,[r0,#R32_0]
    str     r5,[r0,#R32_1]
    str     r5,[r0,#R32_2]
    str     r5,[r0,#R32_3]

    //round=r5=rounds-1;
    subs    r6,r1,#1
    //base = round_cst+12-rounds
    adr		r5, round_cst
    adds    r5,r5,#12
    subs    r5,r5,r1

    push	{r0,r5,r6}

	ldr		r4,[r0,#C4L]
	ldr		r3,[r0,#C3L]
	ldr		r2,[r0,#C2L]
	ldr		r1,[r0,#C1L]
	ldr		r0,[r0,#C0L]

    //loop entry
	//assume r1>0 at entry
drygascon128_g_main_loop:
    //r0~r4: lower half of each words of the state
    //r5: base for round constants
    //r6: round, counting from rounds-1 to 0

    //r6 = ((0xf - r6) << 4) | r6;
    ldrb 	r6,[r5,r6]
    `core_round 1`

    ldr     r4,[r7,#C4L]
    ldr     r5,[sp,#`$stack_rounds`]

    ldr		r6,[sp,#`$stack_round`]
    subs    r6,#1
    bmi     drygascon128_g_exit

    str     r6,[sp,#`$stack_round`]
	b    	drygascon128_g_main_loop
drygascon128_g_exit:

    str		r3,[r7,#C3L]
	str		r2,[r7,#C2L]
	str		r1,[r7,#C1L]
	str		r0,[r7,#C0L]

	add		sp,sp,#`$stack_size`
	pop 	{r4, r5, r6, r7, pc}
.size	drygascon128_g, .-drygascon128_g

.align 2
.type	drygascon128_f, %function
drygascon128_f:
    //r0:state c r x
    //r1:input -> shall be 32 bit aligned
    //r2:ds
    //r3:rounds
    push	{r4, r5, r6, r7, lr}

``
set stack_buf    0
set stack_buf_size [expr 14*2]
set stack_c [expr $stack_buf + $stack_buf_size]
set stack_rounds  [expr $stack_c+4]
set stack_round  [expr $stack_c+8]
set stack_size [expr $stack_round+4]
``
    //stack frame:
    //0 ~ `$stack_buf_size-1`: buf
    //`$stack_c` :pointer on c
    //`$stack_rounds` : rounds for g
    //`$stack_round` :mix round / g round

    movs    r4,#26
    push    {r0,r3,r4}
    sub     sp,sp,#`$stack_buf_size`

    //load 10 bit mask in r4 = 0x3FF
    movs    r4,#0xFF
    lsls    r4,r4,#2
    adds    r4,r4,#3

    movs    r7,#0
    //r=0
    str     r7,[r0,#R32_0]
    str     r7,[r0,#R32_1]
    str     r7,[r0,#R32_2]
    str     r7,[r0,#R32_3]

    //r7 = sp
    add     r7,r7,sp

    ldr     r3,[r1]
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+26]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+24]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+22]

    lsrs    r5,r3,#10
    ldr     r3,[r1,#4]
    lsls    r6,r3,#2
    lsrs    r3,r3,#8
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+20]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+18]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+16]

    lsrs    r5,r3,#10
    ldr     r3,[r1,#8]
    lsls    r6,r3,#4
    lsrs    r3,r3,#6
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+14]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+12]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+10]

    lsrs    r5,r3,#10
    ldr     r3,[r1,#12]
    lsls    r6,r3,#6
    lsrs    r3,r3,#4
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+8]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+6]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+4]

    lsrs    r5,r3,#10
    lsls    r6,r2,#8
    lsrs    r3,r2,#2
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+2]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+0]

    movs    r7,#26

drygascon128_f_mix128_main_loop:
    movs    r6,#0
    add     r6,r6,sp
    ldrh 	r6,[r6,r7]

    ldr     r5,[sp,#`$stack_c`]
    movs    r7,r5
    adds    r5,r5,#40+16
    movs    r4,#0xc

    `mix_step 0`

    `mix_step 1`

    `mix_step 2`

    `mix_step 3`

    lsrs    r4,r6,#6+2
    lsls    r4,r4,#2
    ldr     r6,[r5,r4]
    ldr     r4,[r7,#4*8]
    eors    r4,r4,r6

    ldr		r6,[sp,#`$stack_round`]
    subs    r6,#2
    bpl     drygascon128_f_mix128_coreround
    b		drygascon128_f_mix128_exit
drygascon128_f_mix128_coreround:
    str     r6,[sp,#`$stack_round`]

    movs    r6,#0xf0
    `core_round 0`

    str		r3,[r7,#C3L]
    str		r2,[r7,#C2L]
    str		r1,[r7,#C1L]
    str		r0,[r7,#C0L]

    ldr		r7,[sp,#`$stack_round`]

    b    	drygascon128_f_mix128_main_loop
drygascon128_f_mix128_exit:
    ldr     r7,[sp,#`$stack_rounds`]
    //round=r5=rounds-1;
    subs    r6,r7,#1
    //base = round_cst+12-rounds
    adr		r5, round_cst
    adds    r5,r5,#12
    subs    r5,r5,r7

    add		sp,sp,#`$stack_buf_size`
    str     r5,[sp,#`expr $stack_rounds-$stack_buf_size`]
    str     r6,[sp,#`expr $stack_round-$stack_buf_size`]
    b       drygascon128_g_main_loop

.align 2
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
.align 2

.size	drygascon128_f, .-drygascon128_f


``if {0} {``
.global	drygascon128_mix128
.align 2
.type	drygascon128_mix128, %function
drygascon128_mix128:
    //r0:state c r x
    //r1:input -> shall be 32 bit aligned
    //r2:ds
    push	{r4, r5, r6, r7, lr}

``
set stack_buf    0
set stack_buf_size [expr 14*2]
set stack_c [expr $stack_buf + $stack_buf_size]
set stack_round  [expr $stack_c+4]
set stack_size [expr $stack_round+4]
``
    //stack frame:
    //0 ~ `$stack_buf_size-1`: buf
    //`$stack_c` :pointer on c
    //`$stack_round` :mix round

    movs    r7,#26
    push    {r0,r7}
    sub     sp,sp,#`$stack_buf_size`

    //load 10 bit mask in r4 = 0x3FF
    movs    r4,#0xFF
    lsls    r4,r4,#2
    adds    r4,r4,#3

    //r7 = sp
    movs    r7,#0
    add     r7,r7,sp

    ldr     r3,[r1]
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+26]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+24]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+22]

    lsrs    r5,r3,#10
    ldr     r3,[r1,#4]
    lsls    r6,r3,#2
    lsrs    r3,r3,#8
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+20]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+18]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+16]

    lsrs    r5,r3,#10
    ldr     r3,[r1,#8]
    lsls    r6,r3,#4
    lsrs    r3,r3,#6
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+14]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+12]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+10]

    lsrs    r5,r3,#10
    ldr     r3,[r1,#12]
    lsls    r6,r3,#6
    lsrs    r3,r3,#4
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+8]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+6]

    lsrs    r3,r3,#10
    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+4]

    lsrs    r5,r3,#10
    lsls    r6,r2,#8
    lsrs    r3,r2,#2
    orrs    r6,r6,r5
    movs    r5,r4
    ands    r5,r5,r6
    strh    r5,[r7,#`$stack_buf`+2]

    movs    r5,r4
    ands    r5,r5,r3
    strh    r5,[r7,#`$stack_buf`+0]

    movs    r7,#26

drygascon128_mix128_main_loop:
    movs    r6,#0
    add     r6,r6,sp
    ldrh 	r6,[r6,r7]

    ldr     r5,[sp,#`$stack_c`]
    movs    r7,r5
    adds    r5,r5,#40+16
    movs    r4,#0xc

    `mix_step 0`

    `mix_step 1`

    `mix_step 2`

    `mix_step 3`

    lsrs    r4,r6,#6+2
    lsls    r4,r4,#2
    ldr     r6,[r5,r4]
    ldr     r4,[r7,#4*8]
    eors    r4,r4,r6

    ldr		r6,[sp,#`$stack_round`]
    subs    r6,#2
    bpl     drygascon128_mix128_coreround
    b		drygascon128_mix128_exit
drygascon128_mix128_coreround:
    str     r6,[sp,#`$stack_round`]

    movs    r6,#0xf0
    `core_round 0`

    str		r3,[r7,#C3L]
    str		r2,[r7,#C2L]
    str		r1,[r7,#C1L]
    str		r0,[r7,#C0L]

    ldr		r7,[sp,#`$stack_round`]

    b    	drygascon128_mix128_main_loop
drygascon128_mix128_exit:

    str		r4,[r7,#C4L]
    str		r3,[r7,#C3L]
    str		r2,[r7,#C2L]
    str		r1,[r7,#C1L]
    str		r0,[r7,#C0L]

    add		sp,sp,#`$stack_size`
    pop 	{r4, r5, r6, r7, pc}
.size	drygascon128_mix128, .-drygascon128_mix128
``}``
