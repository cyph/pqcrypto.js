	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_857x1723)
.LCPI0_0:
	.short	65277                   # 0xfefd
.LCPI0_1:
	.short	61565                   # 0xf07d
.LCPI0_2:
	.short	4225                    # 0x1081
.LCPI0_4:
	.short	12161                   # 0x2f81
.LCPI0_5:
	.short	16                      # 0x10
.LCPI0_6:
	.short	64528                   # 0xfc10
.LCPI0_7:
	.short	65                      # 0x41
.LCPI0_8:
	.short	4033                    # 0xfc1
.LCPI0_9:
	.short	100                     # 0x64
.LCPI0_10:
	.short	57364                   # 0xe014
.LCPI0_11:
	.short	2053                    # 0x805
.LCPI0_12:
	.short	33997                   # 0x84cd
.LCPI0_13:
	.short	61893                   # 0xf1c5
.LCPI0_14:
	.short	64089                   # 0xfa59
.LCPI0_15:
	.short	11597                   # 0x2d4d
.LCPI0_16:
	.short	54149                   # 0xd385
.LCPI0_17:
	.short	365                     # 0x16d
.LCPI0_18:
	.short	55799                   # 0xd9f7
.LCPI0_19:
	.short	1723                    # 0x6bb
.LCPI0_20:
	.short	20083                   # 0x4e73
.LCPI0_21:
	.short	3                       # 0x3
.LCPI0_22:
	.short	62953                   # 0xf5e9
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_3:
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_857x1723)
	.type	CRYPTO_NAMESPACE(crypto_decode_857x1723),@function
CRYPTO_NAMESPACE(crypto_decode_857x1723): # @CRYPTO_NAMESPACE(crypto_decode_857x1723)
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$1640, %rsp             # imm = 0x668
	.cfi_def_cfa_offset 1664
	.cfi_offset %rbx, -24
	.cfi_offset %rbp, -16
	movzbl	1151(%rsi), %eax
	leal	-160(%rax), %ecx
	shrl	$15, %ecx
	andl	$160, %ecx
	addl	%ecx, %eax
	addl	$-160, %eax
	imull	$276, %eax, %ecx        # imm = 0x114
	shrl	$16, %ecx
	imull	$-1479802880, %eax, %eax # imm = 0xA7CC0000
	sarl	$16, %eax
	imull	$743, %eax, %eax        # imm = 0x2E7
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1150(%rsi), %eax
	addl	%eax, %ecx
	movswl	%cx, %edx
	imull	$276, %edx, %edx        # imm = 0x114
	shrl	$16, %edx
	imull	$-22580, %ecx, %ecx     # imm = 0xA7CC
	movswl	%cx, %ecx
	imull	$743, %ecx, %ecx        # imm = 0x2E7
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1149(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$743, %r11d             # imm = 0x2E7
	andl	%ecx, %r11d
	addl	%edx, %r11d
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%r11d, %eax
	imull	$-3881, %eax, %eax      # imm = 0xF0D7
	leal	-14044(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$14044, %ecx            # imm = 0x36DC
	addl	%ecx, %eax
	addl	$-14044, %eax           # imm = 0xC924
	movswl	%ax, %r9d
	shll	$6, %r9d
	movl	%r9d, %ecx
	negl	%ecx
	shrl	$16, %ecx
	imull	$27056, %eax, %eax      # imm = 0x69B0
	cwtl
	imull	$436, %eax, %eax        # imm = 0x1B4
	shrl	$16, %eax
	movzbl	1148(%rsi), %edx
	addl	%edx, %ecx
	subl	%eax, %ecx
	movl	$-436, %r10d            # imm = 0xFE4C
	leal	(%rcx,%r10), %eax
	cwtl
	shrl	$15, %eax
	movl	$436, %r8d              # imm = 0x1B4
	andl	%r8d, %eax
	addl	%eax, %ecx
	addl	$-436, %ecx             # imm = 0xFE4C
	movswl	%cx, %eax
	shrl	$15, %eax
	andl	%r8d, %eax
	addl	%ecx, %eax
	movswl	%ax, %ebx
	subl	%ebx, %edx
	shrl	$2, %edx
	addl	%r9d, %edx
	imull	$2405, %edx, %edx       # imm = 0x965
	leal	-8246(%rdx), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$8246, %ecx             # imm = 0x2036
	leal	(%rcx,%rdx), %r9d
	addl	$-8246, %r9d            # imm = 0xDFCA
	movw	%bx, -114(%rsp)
	movw	%r9w, -112(%rsp)
	movswl	%r11w, %ebp
	shll	$6, %ebp
	movl	%ebp, %ebx
	negl	%ebx
	shrl	$16, %ebx
	imull	$27056, %r11d, %edx     # imm = 0x69B0
	movswl	%dx, %edx
	imull	$436, %edx, %ecx        # imm = 0x1B4
	shrl	$16, %ecx
	movzbl	1147(%rsi), %edx
	addl	%edx, %ebx
	subl	%ecx, %ebx
	leal	(%rbx,%r10), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	%r8d, %ecx
	addl	%ebx, %ecx
	addl	$-436, %ecx             # imm = 0xFE4C
	movswl	%cx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%ecx, %ebx
	movswl	%bx, %ecx
	subl	%ecx, %edx
	shrl	$2, %edx
	addl	%ebp, %edx
	imull	$2405, %edx, %edx       # imm = 0x965
	addl	%edx, %r10d
	movswl	%r10w, %ebp
	shrl	$15, %ebp
	andl	%r8d, %ebp
	addl	%ebp, %edx
	addl	$-436, %edx             # imm = 0xFE4C
	movw	%cx, -118(%rsp)
	movw	%dx, -116(%rsp)
	movw	%r9w, -98(%rsp)
	xorl	%ecx, %ecx
	movl	$-334, %r8d             # imm = 0xFEB2
	movl	$334, %r9d              # imm = 0x14E
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%ax, %edx
	movl	%edx, %ebp
	shll	$6, %ebp
	subl	%edx, %ebp
	subl	%edx, %ebp
	shrl	$16, %ebp
	imull	$15305, %eax, %eax      # imm = 0x3BC9
	cwtl
	imull	$334, %eax, %ebx        # imm = 0x14E
	shrl	$16, %ebx
	movzbl	1146(%rsi,%rcx), %eax
	addl	%eax, %ebp
	subl	%ebx, %ebp
	leal	(%r8,%rbp), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebx, %ebp
	addl	$-334, %ebp             # imm = 0xFEB2
	movswl	%bp, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebp, %ebx
	shll	$7, %edx
	movswl	%bx, %ebp
	subl	%ebp, %eax
	shrl	%eax
	addl	%edx, %eax
	imull	$-22761, %eax, %eax     # imm = 0xA717
	leal	(%rax,%r8), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%edx, %eax
	addl	$-334, %eax             # imm = 0xFEB2
	movw	%bp, -102(%rsp,%rcx,4)
	movw	%ax, -100(%rsp,%rcx,4)
	cmpq	$-2, %rcx
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-116(%rsp,%rcx,2), %eax
	decq	%rcx
	jmp	.LBB0_1
.LBB0_3:
	movswl	-98(%rsp), %ecx
	movl	%ecx, %eax
	sarl	$10, %eax
	imull	$8080, %ecx, %edx       # imm = 0x1F90
	movswl	%dx, %edx
	imull	$292, %edx, %ebp        # imm = 0x124
	shrl	$16, %ebp
	movzbl	1143(%rsi), %edx
	addl	%edx, %eax
	subl	%ebp, %eax
	leal	-292(%rax), %ebp
	movswl	%bp, %ebp
	shrl	$15, %ebp
	movl	$292, %r9d              # imm = 0x124
	andl	%r9d, %ebp
	addl	%ebp, %eax
	addl	$-292, %eax             # imm = 0xFEDC
	movswl	%ax, %ebp
	shrl	$15, %ebp
	andl	%r9d, %ebp
	addl	%eax, %ebp
	shll	$6, %ecx
	movswl	%bp, %eax
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%ecx, %edx
	imull	$-3591, %edx, %ecx      # imm = 0xF1F9
	leal	-7229(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$7229, %edx             # imm = 0x1C3D
	addl	%edx, %ecx
	addl	$-7229, %ecx            # imm = 0xE3C3
	movw	%ax, -72(%rsp)
	movw	%cx, -70(%rsp)
	xorl	%ecx, %ecx
	movl	$-292, %r8d             # imm = 0xFEDC
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%rcx,2), %edx
	movl	%edx, %ebp
	sarl	$10, %ebp
	imull	$8080, %edx, %eax       # imm = 0x1F90
	cwtl
	imull	$292, %eax, %ebx        # imm = 0x124
	shrl	$16, %ebx
	movzbl	1142(%rsi,%rcx), %eax
	addl	%eax, %ebp
	subl	%ebx, %ebp
	leal	(%r8,%rbp), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebx, %ebp
	addl	$-292, %ebp             # imm = 0xFEDC
	movswl	%bp, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebp, %ebx
	shll	$6, %edx
	movswl	%bx, %ebp
	subl	%ebp, %eax
	shrl	$2, %eax
	addl	%edx, %eax
	imull	$-3591, %eax, %eax      # imm = 0xF1F9
	leal	(%rax,%r8), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%edx, %eax
	addl	$-292, %eax             # imm = 0xFEDC
	movw	%bp, -76(%rsp,%rcx,4)
	movw	%ax, -74(%rsp,%rcx,4)
	decq	%rcx
	cmpq	$-6, %rcx
	jne	.LBB0_4
# %bb.5:
	movzwl	-70(%rsp), %eax
	movw	%ax, -12(%rsp)
	xorl	%eax, %eax
	movl	$-273, %r8d             # imm = 0xFEEF
	movl	$273, %r9d              # imm = 0x111
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-72(%rsp,%rax,2), %ecx
	movl	%ecx, %ebp
	sarl	$15, %ebp
	imull	$4081, %ecx, %ebx       # imm = 0xFF1
	movswl	%bx, %ebx
	imull	$273, %ebx, %ebx        # imm = 0x111
	shrl	$16, %ebx
	movzbl	1136(%rsi,%rax), %edx
	addl	%edx, %ebp
	subl	%ebx, %ebp
	leal	(%r8,%rbp), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebx, %ebp
	addl	$-273, %ebp             # imm = 0xFEEF
	movswl	%bp, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebp, %ebx
	shll	$8, %ecx
	orl	%edx, %ecx
	subl	%ebx, %ecx
	imull	$4081, %ecx, %ecx       # imm = 0xFF1
	leal	(%rcx,%r8), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%edx, %ecx
	addl	$-273, %ecx             # imm = 0xFEEF
	movw	%bx, -16(%rsp,%rax,4)
	movw	%cx, -14(%rsp,%rax,4)
	decq	%rax
	cmpq	$-13, %rax
	jne	.LBB0_6
# %bb.7:
	movswl	-12(%rsp), %eax
	imull	$-259, %eax, %ecx       # imm = 0xFEFD
	shrl	$16, %ecx
	imull	$-3971, %eax, %edx      # imm = 0xF07D
	movswl	%dx, %edx
	imull	$4225, %edx, %edx       # imm = 0x1081
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1123(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %ebp
	shrl	$15, %ebp
	movl	$4225, %ebx             # imm = 0x1081
	andl	%ebp, %ebx
	addl	%ecx, %ebx
	shll	$8, %eax
	orl	%edx, %eax
	subl	%ebx, %eax
	imull	$12161, %eax, %eax      # imm = 0x2F81
	leal	-438(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$438, %ecx              # imm = 0x1B6
	addl	%ecx, %eax
	addl	$-438, %eax             # imm = 0xFE4A
	movw	%bx, 104(%rsp)
	vpbroadcastw	.LCPI0_0(%rip), %ymm1 # ymm1 = [65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277]
	movw	%ax, 106(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %ymm2 # ymm2 = [61565,61565,61565,61565,61565,61565,61565,61565,61565,61565,61565,61565,61565,61565,61565,61565]
	movl	$10, %eax
	vpbroadcastw	.LCPI0_2(%rip), %ymm3 # ymm3 = [4225,4225,4225,4225,4225,4225,4225,4225,4225,4225,4225,4225,4225,4225,4225,4225]
	vmovdqa	.LCPI0_3(%rip), %ymm0   # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_4(%rip), %ymm4 # ymm4 = [12161,12161,12161,12161,12161,12161,12161,12161,12161,12161,12161,12161,12161,12161,12161,12161]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm5
	vmovdqu	1071(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm6, (%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 32(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_10
# %bb.9:                                #   in Loop: Header=BB0_8 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_8
.LBB0_10:
	movswl	106(%rsp), %eax
	movl	%eax, %ecx
	sarl	$15, %ecx
	imull	$4033, %eax, %edx       # imm = 0xFC1
	movswl	%dx, %ebp
	movl	%ebp, %edx
	shll	$6, %edx
	addl	%ebp, %edx
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1070(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %ebp
	sarl	$12, %ebp
	imull	$-1008, %ecx, %ecx      # imm = 0xFC10
	movswl	%cx, %ebx
	movl	%ebx, %ecx
	shll	$6, %ecx
	addl	%ebx, %ecx
	shrl	$16, %ecx
	subl	%ecx, %ebp
	movswl	%bp, %ecx
	shrl	$15, %ecx
	andl	$65, %ecx
	addl	%ebp, %ecx
	shll	$8, %eax
	orl	%edx, %eax
	subl	%ecx, %eax
	imull	$4033, %eax, %eax       # imm = 0xFC1
	leal	-1723(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$1723, %edx             # imm = 0x6BB
	addl	%edx, %eax
	addl	$-1723, %eax            # imm = 0xF945
	vpbroadcastw	.LCPI0_5(%rip), %ymm1 # ymm1 = [16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16]
	movw	%cx, 324(%rsp)
	vpbroadcastw	.LCPI0_6(%rip), %ymm2 # ymm2 = [64528,64528,64528,64528,64528,64528,64528,64528,64528,64528,64528,64528,64528,64528,64528,64528]
	movw	%ax, 326(%rsp)
	vpbroadcastw	.LCPI0_7(%rip), %ymm3 # ymm3 = [65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65]
	movl	$37, %eax
	vpbroadcastw	.LCPI0_8(%rip), %ymm4 # ymm4 = [4033,4033,4033,4033,4033,4033,4033,4033,4033,4033,4033,4033,4033,4033,4033,4033]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	(%rsp,%rax,2), %ymm5
	vpmulhw	%ymm1, %ymm5, %ymm6
	vpmullw	%ymm2, %ymm5, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm6, %ymm7 # ymm7 = ymm6[0],ymm5[0],ymm6[1],ymm5[1],ymm6[2],ymm5[2],ymm6[3],ymm5[3],ymm6[8],ymm5[8],ymm6[9],ymm5[9],ymm6[10],ymm5[10],ymm6[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm6, %ymm5 # ymm5 = ymm6[4],ymm5[4],ymm6[5],ymm5[5],ymm6[6],ymm5[6],ymm6[7],ymm5[7],ymm6[12],ymm5[12],ymm6[13],ymm5[13],ymm6[14],ymm5[14],ymm6[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm7, %ymm6
	vmovdqu	%ymm6, 112(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 144(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_13
# %bb.12:                               #   in Loop: Header=BB0_11 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_11
.LBB0_13:
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100]
	movzwl	326(%rsp), %eax
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [57364,57364,57364,57364,57364,57364,57364,57364,57364,57364,57364,57364,57364,57364,57364,57364]
	movw	%ax, 764(%rsp)
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [2053,2053,2053,2053,2053,2053,2053,2053,2053,2053,2053,2053,2053,2053,2053,2053]
	movl	$91, %eax
	vpbroadcastw	.LCPI0_12(%rip), %ymm4 # ymm4 = [33997,33997,33997,33997,33997,33997,33997,33997,33997,33997,33997,33997,33997,33997,33997,33997]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	112(%rsp,%rax,2), %ymm5
	vmovdqu	856(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm6, 336(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 368(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_13(%rip), %ymm1 # ymm1 = [61893,61893,61893,61893,61893,61893,61893,61893,61893,61893,61893,61893,61893,61893,61893,61893]
	movzwl	764(%rsp), %eax
	vpbroadcastw	.LCPI0_14(%rip), %ymm2 # ymm2 = [64089,64089,64089,64089,64089,64089,64089,64089,64089,64089,64089,64089,64089,64089,64089,64089]
	movw	%ax, 1624(%rsp)
	vpbroadcastw	.LCPI0_15(%rip), %ymm3 # ymm3 = [11597,11597,11597,11597,11597,11597,11597,11597,11597,11597,11597,11597,11597,11597,11597,11597]
	movl	$198, %eax
	vpbroadcastw	.LCPI0_16(%rip), %ymm4 # ymm4 = [54149,54149,54149,54149,54149,54149,54149,54149,54149,54149,54149,54149,54149,54149,54149,54149]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	336(%rsp,%rax,2), %ymm5
	vmovdqu	428(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm6, 768(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 800(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	vpbroadcastw	.LCPI0_17(%rip), %ymm0 # ymm0 = [365,365,365,365,365,365,365,365,365,365,365,365,365,365,365,365]
	vpbroadcastw	.LCPI0_18(%rip), %ymm1 # ymm1 = [55799,55799,55799,55799,55799,55799,55799,55799,55799,55799,55799,55799,55799,55799,55799,55799]
	movl	1624(%rsp), %eax
	vpbroadcastw	.LCPI0_19(%rip), %ymm2 # ymm2 = [1723,1723,1723,1723,1723,1723,1723,1723,1723,1723,1723,1723,1723,1723,1723,1723]
	leal	(%rax,%rax,2), %eax
	addl	$-2583, %eax            # imm = 0xF5E9
	vpbroadcastw	.LCPI0_20(%rip), %ymm3 # ymm3 = [20083,20083,20083,20083,20083,20083,20083,20083,20083,20083,20083,20083,20083,20083,20083,20083]
	movw	%ax, 1712(%rdi)
	vpbroadcastw	.LCPI0_21(%rip), %ymm4 # ymm4 = [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
	movl	$412, %eax              # imm = 0x19C
	vpbroadcastw	.LCPI0_22(%rip), %ymm5 # ymm5 = [62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	768(%rsp,%rax,2), %ymm6
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
	addq	$1640, %rsp             # imm = 0x668
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_857x1723), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_857x1723)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
