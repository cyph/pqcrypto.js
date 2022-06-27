	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_761x1531)
.LCPI0_0:
	.short	65252                   # 0xfee4
.LCPI0_1:
	.short	38692                   # 0x9724
.LCPI0_2:
	.short	625                     # 0x271
.LCPI0_3:
	.short	32401                   # 0x7e91
.LCPI0_4:
	.short	2816                    # 0xb00
.LCPI0_5:
	.short	62915                   # 0xf5c3
.LCPI0_6:
	.short	6400                    # 0x1900
.LCPI0_8:
	.short	23593                   # 0x5c29
.LCPI0_9:
	.short	256                     # 0x100
.LCPI0_10:
	.short	52429                   # 0xcccd
.LCPI0_11:
	.short	1280                    # 0x500
.LCPI0_12:
	.short	1592                    # 0x638
.LCPI0_13:
	.short	63704                   # 0xf8d8
.LCPI0_14:
	.short	9157                    # 0x23c5
.LCPI0_15:
	.short	25357                   # 0x630d
.LCPI0_16:
	.short	518                     # 0x206
.LCPI0_17:
	.short	54578                   # 0xd532
.LCPI0_18:
	.short	1531                    # 0x5fb
.LCPI0_19:
	.short	15667                   # 0x3d33
.LCPI0_20:
	.short	3                       # 0x3
.LCPI0_21:
	.short	63241                   # 0xf709
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_7:
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.text
	.globl	CRYPTO_NAMESPACE(crypto_decode_761x1531)
	.type	CRYPTO_NAMESPACE(crypto_decode_761x1531),@function
CRYPTO_NAMESPACE(crypto_decode_761x1531): # @CRYPTO_NAMESPACE(crypto_decode_761x1531)
	.cfi_startproc
# %bb.0:
	subq	$1432, %rsp             # imm = 0x598
	.cfi_def_cfa_offset 1440
	movzbl	1006(%rsi), %eax
	imull	$-84, %eax, %ecx
	shrl	$16, %ecx
	imull	$-316407808, %eax, %eax # imm = 0xED240000
	sarl	$16, %eax
	imull	$3475, %eax, %eax       # imm = 0xD93
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1005(%rsi), %edx
	addl	%ecx, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$3475, %eax             # imm = 0xD93
	andl	%ecx, %eax
	addl	%edx, %eax
	movswl	%ax, %ecx
	imull	$60, %ecx, %ecx
	shrl	$16, %ecx
	imull	$-28292, %eax, %edx     # imm = 0x917C
	movswl	%dx, %edx
	imull	$593, %edx, %edx        # imm = 0x251
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1004(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %r9d
	shrl	$15, %r9d
	movl	$593, %edx              # imm = 0x251
	andl	%r9d, %edx
	addl	%ecx, %edx
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%edx, %eax
	imull	$-31055, %eax, %eax     # imm = 0x86B1
	leal	-1500(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$1500, %ecx             # imm = 0x5DC
	leal	(%rcx,%rax), %r9d
	addl	$-1500, %r9d            # imm = 0xFA24
	movw	%r9w, -110(%rsp)
	movswl	%dx, %r8d
	imull	$672, %r8d, %ecx        # imm = 0x2A0
	shrl	$16, %ecx
	imull	$-2692, %edx, %edx      # imm = 0xF57C
	movswl	%dx, %edx
	imull	$6232, %edx, %edx       # imm = 0x1858
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1003(%rsi), %r10d
	addl	%r10d, %ecx
	movswl	%cx, %eax
	imull	$672, %eax, %eax        # imm = 0x2A0
	shrl	$16, %eax
	imull	$-2692, %ecx, %ecx      # imm = 0xF57C
	movswl	%cx, %ecx
	imull	$6232, %ecx, %ecx       # imm = 0x1858
	shrl	$16, %ecx
	subl	%ecx, %eax
	movzbl	1002(%rsi), %ecx
	addl	%ecx, %eax
	movswl	%ax, %edx
	shrl	$15, %edx
	movl	$6232, %r11d            # imm = 0x1858
	andl	%r11d, %edx
	addl	%eax, %edx
	shll	$13, %r8d
	shll	$5, %r10d
	orl	%r8d, %r10d
	movswl	%dx, %eax
	subl	%eax, %ecx
	shrl	$3, %ecx
	addl	%r10d, %ecx
	imull	$12451, %ecx, %ecx      # imm = 0x30A3
	leal	-6232(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r11d, %edx
	addl	%edx, %ecx
	addl	$-6232, %ecx            # imm = 0xE7A8
	movw	%ax, -114(%rsp)
	movw	%cx, -112(%rsp)
	movswl	%r9w, %eax
	imull	$-476, %eax, %eax       # imm = 0xFE24
	shrl	$16, %eax
	imull	$-13284, %r9d, %edx     # imm = 0xCC1C
	movswl	%dx, %edx
	imull	$1263, %edx, %edx       # imm = 0x4EF
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1001(%rsi), %r8d
	addl	%r8d, %eax
	movswl	%ax, %r10d
	shrl	$15, %r10d
	movl	$1263, %r11d            # imm = 0x4EF
	andl	%r11d, %r10d
	addl	%eax, %r10d
	shll	$8, %r9d
	orl	%r8d, %r9d
	subl	%r10d, %r9d
	imull	$-22001, %r9d, %r8d     # imm = 0xAA0F
	leal	-304(%r8), %eax
	cwtl
	shrl	$15, %eax
	andl	$304, %eax              # imm = 0x130
	addl	%r8d, %eax
	addl	$-304, %eax             # imm = 0xFED0
	movw	%r10w, -100(%rsp)
	movw	%ax, -98(%rsp)
	xorl	%r8d, %r8d
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%cx, %edx
	imull	$-476, %edx, %edx       # imm = 0xFE24
	shrl	$16, %edx
	imull	$-13284, %ecx, %eax     # imm = 0xCC1C
	cwtl
	imull	$1263, %eax, %eax       # imm = 0x4EF
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1000(%rsi,%r8), %r9d
	addl	%r9d, %edx
	movswl	%dx, %eax
	shrl	$15, %eax
	andl	%r11d, %eax
	addl	%edx, %eax
	shll	$8, %ecx
	orl	%r9d, %ecx
	subl	%eax, %ecx
	imull	$-22001, %ecx, %ecx     # imm = 0xAA0F
	leal	-1263(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r11d, %edx
	addl	%edx, %ecx
	addl	$-1263, %ecx            # imm = 0xFB11
	movw	%ax, -104(%rsp,%r8,4)
	movw	%cx, -102(%rsp,%r8,4)
	cmpq	$-1, %r8
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-114(%rsp,%r8,2), %ecx
	decq	%r8
	jmp	.LBB0_1
.LBB0_3:
	movswl	-98(%rsp), %eax
	imull	$2348, %eax, %edx       # imm = 0x92C
	shrl	$16, %edx
	imull	$-1844, %eax, %eax      # imm = 0xF8CC
	cwtl
	imull	$9097, %eax, %eax       # imm = 0x2389
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	998(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %eax
	imull	$2348, %eax, %ecx       # imm = 0x92C
	shrl	$16, %ecx
	imull	$-1844, %edx, %eax      # imm = 0xF8CC
	cwtl
	imull	$9097, %eax, %eax       # imm = 0x2389
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	997(%rsi), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$9097, %r10d            # imm = 0x2389
	andl	%r10d, %edx
	addl	%ecx, %edx
	shll	$8, %r8d
	orl	%r9d, %r8d
	subl	%edx, %r8d
	imull	$17081, %r8d, %r8d      # imm = 0x42B9
	leal	-2188(%r8), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$2188, %ecx             # imm = 0x88C
	addl	%r8d, %ecx
	addl	$-2188, %ecx            # imm = 0xF774
	movw	%dx, -76(%rsp)
	movw	%cx, -74(%rsp)
	xorl	%r8d, %r8d
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%r8,2), %edx
	imull	$2348, %edx, %eax       # imm = 0x92C
	shrl	$16, %eax
	imull	$-1844, %edx, %edx      # imm = 0xF8CC
	movswl	%dx, %edx
	imull	$9097, %edx, %edx       # imm = 0x2389
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	996(%rsi,%r8,2), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	imull	$2348, %ecx, %ecx       # imm = 0x92C
	shrl	$16, %ecx
	imull	$-1844, %eax, %eax      # imm = 0xF8CC
	cwtl
	imull	$9097, %eax, %eax       # imm = 0x2389
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	995(%rsi,%r8,2), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	andl	%r10d, %eax
	addl	%ecx, %eax
	shll	$8, %edx
	orl	%r9d, %edx
	subl	%eax, %edx
	imull	$17081, %edx, %ecx      # imm = 0x42B9
	leal	-9097(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r10d, %edx
	addl	%edx, %ecx
	addl	$-9097, %ecx            # imm = 0xDC77
	movw	%ax, -80(%rsp,%r8,4)
	movw	%cx, -78(%rsp,%r8,4)
	decq	%r8
	cmpq	$-5, %r8
	jne	.LBB0_4
# %bb.5:
	movswl	-74(%rsp), %r8d
	imull	$372, %r8d, %edx        # imm = 0x174
	shrl	$16, %edx
	imull	$-10994, %r8d, %eax     # imm = 0xD50E
	cwtl
	imull	$1526, %eax, %eax       # imm = 0x5F6
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	986(%rsi), %r9d
	addl	%r9d, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$1526, %r11d            # imm = 0x5F6
	andl	%r11d, %ecx
	addl	%edx, %ecx
	shll	$7, %r8d
	movswl	%cx, %r10d
	subl	%r10d, %r9d
	shrl	%r9d
	addl	%r8d, %r9d
	imull	$-18381, %r9d, %edx     # imm = 0xB833
	leal	-367(%rdx), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$367, %ecx              # imm = 0x16F
	addl	%edx, %ecx
	addl	$-367, %ecx             # imm = 0xFE91
	movw	%r10w, -20(%rsp)
	movw	%cx, -18(%rsp)
	xorl	%r9d, %r9d
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-76(%rsp,%r9,2), %r8d
	imull	$372, %r8d, %eax        # imm = 0x174
	shrl	$16, %eax
	imull	$-10994, %r8d, %edx     # imm = 0xD50E
	movswl	%dx, %edx
	imull	$1526, %edx, %edx       # imm = 0x5F6
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	985(%rsi,%r9), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	andl	%r11d, %ecx
	addl	%eax, %ecx
	shll	$7, %r8d
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	%edx
	addl	%r8d, %edx
	imull	$-18381, %edx, %ecx     # imm = 0xB833
	leal	-1526(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r11d, %edx
	addl	%edx, %ecx
	addl	$-1526, %ecx            # imm = 0xFA0A
	movw	%ax, -24(%rsp,%r9,4)
	movw	%cx, -22(%rsp,%r9,4)
	decq	%r9
	cmpq	$-11, %r9
	jne	.LBB0_6
# %bb.7:
	movswl	-18(%rsp), %eax
	imull	$-284, %eax, %ecx       # imm = 0xFEE4
	shrl	$16, %ecx
	imull	$-26844, %eax, %edx     # imm = 0x9724
	movswl	%dx, %edx
	imull	$625, %edx, %edx        # imm = 0x271
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	974(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$625, %r9d              # imm = 0x271
	andl	%edx, %r9d
	addl	%ecx, %r9d
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%r9d, %eax
	imull	$32401, %eax, %eax      # imm = 0x7E91
	leal	-150(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$150, %ecx
	addl	%ecx, %eax
	addl	$-150, %eax
	vpbroadcastw	.LCPI0_0(%rip), %ymm0 # ymm0 = [65252,65252,65252,65252,65252,65252,65252,65252,65252,65252,65252,65252,65252,65252,65252,65252]
	movw	%r9w, 76(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %ymm1 # ymm1 = [38692,38692,38692,38692,38692,38692,38692,38692,38692,38692,38692,38692,38692,38692,38692,38692]
	movw	%ax, 78(%rsp)
	vpbroadcastw	.LCPI0_2(%rip), %ymm2 # ymm2 = [625,625,625,625,625,625,625,625,625,625,625,625,625,625,625,625]
	movl	$7, %eax
	vpbroadcastw	.LCPI0_3(%rip), %ymm3 # ymm3 = [32401,32401,32401,32401,32401,32401,32401,32401,32401,32401,32401,32401,32401,32401,32401,32401]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm4
	vpmovzxbw	951(%rsi,%rax), %ymm5 # ymm5 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm4, %ymm6
	vpmullw	%ymm1, %ymm4, %ymm7
	vpmulhw	%ymm2, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm2, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsllw	$8, %ymm4, %ymm4
	vpor	%ymm5, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm4, %ymm4
	vpmullw	%ymm3, %ymm4, %ymm4
	vpcmpgtw	%ymm4, %ymm2, %ymm5
	vpandn	%ymm2, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm4, %ymm4
	vpunpcklwd	%ymm4, %ymm6, %ymm5 # ymm5 = ymm6[0],ymm4[0],ymm6[1],ymm4[1],ymm6[2],ymm4[2],ymm6[3],ymm4[3],ymm6[8],ymm4[8],ymm6[9],ymm4[9],ymm6[10],ymm4[10],ymm6[11],ymm4[11]
	vpunpckhwd	%ymm4, %ymm6, %ymm4 # ymm4 = ymm6[4],ymm4[4],ymm6[5],ymm4[5],ymm6[6],ymm4[6],ymm6[7],ymm4[7],ymm6[12],ymm4[12],ymm6[13],ymm4[13],ymm6[14],ymm4[14],ymm6[15],ymm4[15]
	vinserti128	$1, %xmm4, %ymm5, %ymm6
	vmovdqu	%ymm6, -16(%rsp,%rax,4)
	vperm2i128	$49, %ymm4, %ymm5, %ymm4 # ymm4 = ymm5[2,3],ymm4[2,3]
	vmovdqu	%ymm4, 16(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_10
# %bb.9:                                #   in Loop: Header=BB0_8 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_8
.LBB0_10:
	movswl	78(%rsp), %r9d
	imull	$2816, %r9d, %ecx       # imm = 0xB00
	shrl	$16, %ecx
	imull	$-2621, %r9d, %edx      # imm = 0xF5C3
	movswl	%dx, %edx
	imull	$6400, %edx, %edx       # imm = 0x1900
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	950(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %edx
	imull	$2816, %edx, %edx       # imm = 0xB00
	shrl	$16, %edx
	imull	$-2621, %ecx, %ecx      # imm = 0xF5C3
	movswl	%cx, %ecx
	imull	$6400, %ecx, %ecx       # imm = 0x1900
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	949(%rsi), %ecx
	addl	%ecx, %edx
	movswl	%dx, %r10d
	shrl	$15, %r10d
	movl	$6400, %eax             # imm = 0x1900
	andl	%r10d, %eax
	addl	%edx, %eax
	shll	$8, %r9d
	orl	%r8d, %r9d
	cwtl
	subl	%eax, %ecx
	shrl	$8, %ecx
	addl	%r9d, %ecx
	imull	$23593, %ecx, %ecx      # imm = 0x5C29
	leal	-1531(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$1531, %edx             # imm = 0x5FB
	addl	%edx, %ecx
	addl	$-1531, %ecx            # imm = 0xFA05
	movw	%ax, 268(%rsp)
	vpbroadcastw	.LCPI0_4(%rip), %ymm1 # ymm1 = [2816,2816,2816,2816,2816,2816,2816,2816,2816,2816,2816,2816,2816,2816,2816,2816]
	movw	%cx, 270(%rsp)
	vpbroadcastw	.LCPI0_5(%rip), %ymm2 # ymm2 = [62915,62915,62915,62915,62915,62915,62915,62915,62915,62915,62915,62915,62915,62915,62915,62915]
	movl	$31, %eax
	vpbroadcastw	.LCPI0_6(%rip), %ymm3 # ymm3 = [6400,6400,6400,6400,6400,6400,6400,6400,6400,6400,6400,6400,6400,6400,6400,6400]
	vmovdqa	.LCPI0_7(%rip), %ymm0   # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_8(%rip), %ymm4 # ymm4 = [23593,23593,23593,23593,23593,23593,23593,23593,23593,23593,23593,23593,23593,23593,23593,23593]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	-16(%rsp,%rax,2), %ymm5
	vmovdqu	855(%rsi,%rax,2), %ymm6
	vpsrlw	$8, %ymm6, %ymm7
	vpmulhw	%ymm1, %ymm5, %ymm8
	vpmullw	%ymm2, %ymm5, %ymm9
	vpmulhw	%ymm3, %ymm9, %ymm9
	vpsubw	%ymm9, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm8
	vpmulhw	%ymm1, %ymm8, %ymm9
	vpmullw	%ymm2, %ymm8, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm9, %ymm8
	vpand	%ymm0, %ymm6, %ymm6
	vpaddw	%ymm6, %ymm8, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsllw	$8, %ymm5, %ymm5
	vpor	%ymm5, %ymm7, %ymm5
	vpsubw	%ymm8, %ymm6, %ymm6
	vpsraw	$8, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm8, %ymm6 # ymm6 = ymm8[0],ymm5[0],ymm8[1],ymm5[1],ymm8[2],ymm5[2],ymm8[3],ymm5[3],ymm8[8],ymm5[8],ymm8[9],ymm5[9],ymm8[10],ymm5[10],ymm8[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm8, %ymm5 # ymm5 = ymm8[4],ymm5[4],ymm8[5],ymm5[5],ymm8[6],ymm5[6],ymm8[7],ymm5[7],ymm8[12],ymm5[12],ymm8[13],ymm5[13],ymm8[14],ymm5[14],ymm8[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 80(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 112(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_13
# %bb.12:                               #   in Loop: Header=BB0_11 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_11
.LBB0_13:
	movzwl	270(%rsp), %eax
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256]
	movw	%ax, 652(%rsp)
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [52429,52429,52429,52429,52429,52429,52429,52429,52429,52429,52429,52429,52429,52429,52429,52429]
	movl	$79, %eax
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [1280,1280,1280,1280,1280,1280,1280,1280,1280,1280,1280,1280,1280,1280,1280,1280]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	80(%rsp,%rax,2), %ymm4
	vpmovzxbw	760(%rsi,%rax), %ymm5 # ymm5 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm4, %ymm6
	vpmullw	%ymm2, %ymm4, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpsraw	$8, %ymm5, %ymm5
	vpaddw	%ymm4, %ymm5, %ymm4
	vpmullw	%ymm2, %ymm4, %ymm4
	vpcmpgtw	%ymm4, %ymm3, %ymm5
	vpandn	%ymm3, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm4, %ymm4
	vpunpcklwd	%ymm4, %ymm6, %ymm5 # ymm5 = ymm6[0],ymm4[0],ymm6[1],ymm4[1],ymm6[2],ymm4[2],ymm6[3],ymm4[3],ymm6[8],ymm4[8],ymm6[9],ymm4[9],ymm6[10],ymm4[10],ymm6[11],ymm4[11]
	vpunpckhwd	%ymm4, %ymm6, %ymm4 # ymm4 = ymm6[4],ymm4[4],ymm6[5],ymm4[5],ymm6[6],ymm4[6],ymm6[7],ymm4[7],ymm6[12],ymm4[12],ymm6[13],ymm4[13],ymm6[14],ymm4[14],ymm6[15],ymm4[15]
	vinserti128	$1, %xmm4, %ymm5, %ymm6
	vmovdqu	%ymm6, 272(%rsp,%rax,4)
	vperm2i128	$49, %ymm4, %ymm5, %ymm4 # ymm4 = ymm5[2,3],ymm4[2,3]
	vmovdqu	%ymm4, 304(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_12(%rip), %ymm1 # ymm1 = [1592,1592,1592,1592,1592,1592,1592,1592,1592,1592,1592,1592,1592,1592,1592,1592]
	movzwl	652(%rsp), %eax
	vpbroadcastw	.LCPI0_13(%rip), %ymm2 # ymm2 = [63704,63704,63704,63704,63704,63704,63704,63704,63704,63704,63704,63704,63704,63704,63704,63704]
	movw	%ax, 1416(%rsp)
	vpbroadcastw	.LCPI0_14(%rip), %ymm3 # ymm3 = [9157,9157,9157,9157,9157,9157,9157,9157,9157,9157,9157,9157,9157,9157,9157,9157]
	movl	$174, %eax
	vpbroadcastw	.LCPI0_15(%rip), %ymm4 # ymm4 = [25357,25357,25357,25357,25357,25357,25357,25357,25357,25357,25357,25357,25357,25357,25357,25357]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	272(%rsp,%rax,2), %ymm5
	vmovdqu	380(%rsi,%rax,2), %ymm6
	vpsrlw	$8, %ymm6, %ymm7
	vpmulhw	%ymm1, %ymm5, %ymm8
	vpmullw	%ymm2, %ymm5, %ymm5
	vpmulhw	%ymm3, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm8, %ymm5
	vpaddw	%ymm7, %ymm5, %ymm5
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm5
	vpmulhw	%ymm3, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm7, %ymm5
	vpand	%ymm0, %ymm6, %ymm7
	vpaddw	%ymm7, %ymm5, %ymm5
	vpsraw	$15, %ymm5, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm5, %ymm7, %ymm5
	vpsubw	%ymm5, %ymm6, %ymm6
	vpmullw	%ymm4, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm5, %ymm7 # ymm7 = ymm5[0],ymm6[0],ymm5[1],ymm6[1],ymm5[2],ymm6[2],ymm5[3],ymm6[3],ymm5[8],ymm6[8],ymm5[9],ymm6[9],ymm5[10],ymm6[10],ymm5[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm5, %ymm5 # ymm5 = ymm5[4],ymm6[4],ymm5[5],ymm6[5],ymm5[6],ymm6[6],ymm5[7],ymm6[7],ymm5[12],ymm6[12],ymm5[13],ymm6[13],ymm5[14],ymm6[14],ymm5[15],ymm6[15]
	vinserti128	$1, %xmm5, %ymm7, %ymm6
	vmovdqu	%ymm6, 656(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 688(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	vpbroadcastw	.LCPI0_16(%rip), %ymm0 # ymm0 = [518,518,518,518,518,518,518,518,518,518,518,518,518,518,518,518]
	vpbroadcastw	.LCPI0_17(%rip), %ymm1 # ymm1 = [54578,54578,54578,54578,54578,54578,54578,54578,54578,54578,54578,54578,54578,54578,54578,54578]
	movl	1416(%rsp), %eax
	vpbroadcastw	.LCPI0_18(%rip), %ymm2 # ymm2 = [1531,1531,1531,1531,1531,1531,1531,1531,1531,1531,1531,1531,1531,1531,1531,1531]
	leal	(%rax,%rax,2), %eax
	addl	$-2295, %eax            # imm = 0xF709
	vpbroadcastw	.LCPI0_19(%rip), %ymm3 # ymm3 = [15667,15667,15667,15667,15667,15667,15667,15667,15667,15667,15667,15667,15667,15667,15667,15667]
	movw	%ax, 1520(%rdi)
	vpbroadcastw	.LCPI0_20(%rip), %ymm4 # ymm4 = [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
	movl	$364, %eax              # imm = 0x16C
	vpbroadcastw	.LCPI0_21(%rip), %ymm5 # ymm5 = [63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	656(%rsp,%rax,2), %ymm6
	vpmovzxbw	(%rsi,%rax), %ymm7 # ymm7 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm6, %ymm8
	vpmullw	%ymm1, %ymm6, %ymm9
	vpmulhw	%ymm2, %ymm9, %ymm9
	vpsubw	%ymm9, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm2, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsllw	$8, %ymm6, %ymm6
	vpor	%ymm7, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm6, %ymm6
	vpmullw	%ymm3, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm2, %ymm7
	vpandn	%ymm2, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpmullw	%ymm4, %ymm8, %ymm7
	vpmullw	%ymm4, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm7, %ymm7
	vpaddw	%ymm5, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm7, %ymm8 # ymm8 = ymm7[0],ymm6[0],ymm7[1],ymm6[1],ymm7[2],ymm6[2],ymm7[3],ymm6[3],ymm7[8],ymm6[8],ymm7[9],ymm6[9],ymm7[10],ymm6[10],ymm7[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm7, %ymm6 # ymm6 = ymm7[4],ymm6[4],ymm7[5],ymm6[5],ymm7[6],ymm6[6],ymm7[7],ymm6[7],ymm7[12],ymm6[12],ymm7[13],ymm6[13],ymm7[14],ymm6[14],ymm7[15],ymm6[15]
	vinserti128	$1, %xmm6, %ymm8, %ymm7
	vmovdqu	%ymm7, (%rdi,%rax,4)
	vperm2i128	$49, %ymm6, %ymm8, %ymm6 # ymm6 = ymm8[2,3],ymm6[2,3]
	vmovdqu	%ymm6, 32(%rdi,%rax,4)
	testq	%rax, %rax
	je	.LBB0_22
# %bb.21:                               #   in Loop: Header=BB0_20 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_20
.LBB0_22:
	addq	$1432, %rsp             # imm = 0x598
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_761x1531), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_761x1531)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
