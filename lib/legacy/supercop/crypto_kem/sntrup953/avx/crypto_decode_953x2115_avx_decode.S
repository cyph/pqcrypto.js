	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_953x2115)
.LCPI0_0:
	.short	65420                   # 0xff8c
.LCPI0_1:
	.short	29687                   # 0x73f7
.LCPI0_2:
	.short	468                     # 0x1d4
.LCPI0_3:
	.short	65068                   # 0xfe2c
.LCPI0_4:
	.short	53213                   # 0xcfdd
.LCPI0_5:
	.short	22                      # 0x16
.LCPI0_6:
	.short	17047                   # 0x4297
.LCPI0_7:
	.short	346                     # 0x15a
.LCPI0_8:
	.short	65190                   # 0xfea6
.LCPI0_9:
	.short	25381                   # 0x6325
.LCPI0_10:
	.short	64988                   # 0xfddc
.LCPI0_11:
	.short	62012                   # 0xf23c
.LCPI0_12:
	.short	4761                    # 0x1299
.LCPI0_14:
	.short	8617                    # 0x21a9
.LCPI0_15:
	.short	65522                   # 0xfff2
.LCPI0_16:
	.short	64586                   # 0xfc4a
.LCPI0_17:
	.short	69                      # 0x45
.LCPI0_18:
	.short	4749                    # 0x128d
.LCPI0_19:
	.short	1036                    # 0x40c
.LCPI0_20:
	.short	57604                   # 0xe104
.LCPI0_21:
	.short	2115                    # 0x843
.LCPI0_22:
	.short	33899                   # 0x846b
.LCPI0_23:
	.short	3                       # 0x3
.LCPI0_24:
	.short	62365                   # 0xf39d
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_13:
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_953x2115)
	.type	CRYPTO_NAMESPACE(crypto_decode_953x2115),@function
CRYPTO_NAMESPACE(crypto_decode_953x2115): # @CRYPTO_NAMESPACE(crypto_decode_953x2115)
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$1808, %rsp             # imm = 0x710
	.cfi_def_cfa_offset 1824
	.cfi_offset %rbx, -16
	movzbl	1316(%rsi), %eax
	leal	-248(%rax), %ecx
	shrl	$15, %ecx
	movl	$124, %edx
	andl	%edx, %ecx
	addl	%eax, %ecx
	addl	$-248, %ecx
	movl	%ecx, %eax
	shrl	$15, %eax
	andl	%edx, %eax
	addl	%ecx, %eax
	imull	$964, %eax, %edx        # imm = 0x3C4
	shrl	$16, %edx
	imull	$-285868032, %eax, %ecx # imm = 0xEEF60000
	sarl	$16, %ecx
	imull	$3846, %ecx, %ecx       # imm = 0xF06
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1315(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %ecx
	imull	$964, %ecx, %ecx        # imm = 0x3C4
	shrl	$16, %ecx
	imull	$-4362, %edx, %edx      # imm = 0xEEF6
	movswl	%dx, %edx
	imull	$3846, %edx, %edx       # imm = 0xF06
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1314(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %r9d
	shrl	$15, %r9d
	movl	$3846, %r10d            # imm = 0xF06
	andl	%r9d, %r10d
	addl	%ecx, %r10d
	shll	$15, %eax
	shll	$7, %r8d
	orl	%eax, %r8d
	movswl	%r10w, %r9d
	subl	%r9d, %edx
	shrl	%edx
	addl	%r8d, %edx
	imull	$-16597, %edx, %eax     # imm = 0xBF2B
	leal	-2107(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$2107, %ecx             # imm = 0x83B
	addl	%ecx, %eax
	addl	$-2107, %eax            # imm = 0xF7C5
	movswl	%ax, %r11d
	imull	$-3716, %r11d, %ecx     # imm = 0xF17C
	shrl	$16, %ecx
	imull	$-1057, %eax, %eax      # imm = 0xFBDF
	cwtl
	imull	$15876, %eax, %eax      # imm = 0x3E04
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1313(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %eax
	imull	$-3716, %eax, %ebx      # imm = 0xF17C
	shrl	$16, %ebx
	imull	$-1057, %ecx, %eax      # imm = 0xFBDF
	cwtl
	imull	$15876, %eax, %eax      # imm = 0x3E04
	shrl	$16, %eax
	subl	%eax, %ebx
	movzbl	1312(%rsi), %ecx
	addl	%ecx, %ebx
	movswl	%bx, %eax
	shrl	$15, %eax
	movl	$15876, %r8d            # imm = 0x3E04
	andl	%r8d, %eax
	addl	%ebx, %eax
	shll	$14, %r11d
	shll	$6, %edx
	orl	%r11d, %edx
	movswl	%ax, %ebx
	subl	%ebx, %ecx
	shrl	$2, %ecx
	addl	%edx, %ecx
	imull	$12417, %ecx, %ecx      # imm = 0x3081
	leal	-8694(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$8694, %edx             # imm = 0x21F6
	leal	(%rdx,%rcx), %r11d
	addl	$-8694, %r11d           # imm = 0xDE0A
	movw	%bx, -116(%rsp)
	movw	%r11w, -114(%rsp)
	imull	$-3716, %r9d, %ebx      # imm = 0xF17C
	shrl	$16, %ebx
	imull	$-1057, %r10d, %edx     # imm = 0xFBDF
	movswl	%dx, %edx
	imull	$15876, %edx, %edx      # imm = 0x3E04
	shrl	$16, %edx
	subl	%edx, %ebx
	movzbl	1311(%rsi), %r10d
	addl	%r10d, %ebx
	movswl	%bx, %ecx
	imull	$-3716, %ecx, %ecx      # imm = 0xF17C
	shrl	$16, %ecx
	imull	$-1057, %ebx, %ebx      # imm = 0xFBDF
	movswl	%bx, %ebx
	imull	$15876, %ebx, %ebx      # imm = 0x3E04
	shrl	$16, %ebx
	subl	%ebx, %ecx
	movzbl	1310(%rsi), %ebx
	addl	%ebx, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%ecx, %edx
	shll	$14, %r9d
	shll	$6, %r10d
	orl	%r9d, %r10d
	movswl	%dx, %ecx
	subl	%ecx, %ebx
	shrl	$2, %ebx
	addl	%r10d, %ebx
	imull	$12417, %ebx, %edx      # imm = 0x3081
	leal	-15876(%rdx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%ebx, %edx
	addl	$-15876, %edx           # imm = 0xC1FC
	movw	%cx, -120(%rsp)
	movw	%dx, -118(%rsp)
	movswl	%r11w, %r8d
	movl	%r8d, %edx
	sarl	$12, %edx
	imull	$-520, %r11d, %ebx      # imm = 0xFDF8
	movswl	%bx, %ecx
	movl	%ecx, %ebx
	shll	$7, %ebx
	subl	%ecx, %ebx
	subl	%ecx, %ebx
	shrl	$16, %ebx
	subl	%ebx, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	andl	$126, %ecx
	addl	%edx, %ecx
	movswl	%cx, %ecx
	subl	%ecx, %r8d
	shrl	%r8d
	imull	$-4161, %r8d, %edx      # imm = 0xEFBF
	leal	-69(%rdx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	$69, %ebx
	addl	%ebx, %edx
	addl	$-69, %edx
	movw	%cx, -100(%rsp)
	movw	%dx, -98(%rsp)
	xorl	%r9d, %r9d
	movl	$126, %r8d
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%ax, %edx
	movl	%edx, %ebx
	sarl	$12, %ebx
	imull	$-520, %eax, %eax       # imm = 0xFDF8
	movswl	%ax, %ecx
	movl	%ecx, %eax
	shll	$7, %eax
	subl	%ecx, %eax
	subl	%ecx, %eax
	shrl	$16, %eax
	subl	%eax, %ebx
	movswl	%bx, %eax
	shrl	$15, %eax
	andl	%r8d, %eax
	addl	%ebx, %eax
	cwtl
	subl	%eax, %edx
	shrl	%edx
	imull	$-4161, %edx, %ecx      # imm = 0xEFBF
	leal	-126(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %ecx
	addl	$-126, %ecx
	movw	%ax, -104(%rsp,%r9,4)
	movw	%cx, -102(%rsp,%r9,4)
	cmpq	$-2, %r9
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-118(%rsp,%r9,2), %eax
	decq	%r9
	jmp	.LBB0_1
.LBB0_3:
	movzwl	-98(%rsp), %eax
	movw	%ax, -68(%rsp)
	xorl	%eax, %eax
	movl	$2863, %r8d             # imm = 0xB2F
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%rax,2), %edx
	leal	(,%rdx,4), %ebx
	leal	(%rbx,%rbx,8), %ebx
	shrl	$16, %ebx
	imull	$-5860, %edx, %edx      # imm = 0xE91C
	movswl	%dx, %edx
	imull	$2863, %edx, %edx       # imm = 0xB2F
	shrl	$16, %edx
	subl	%edx, %ebx
	movzbl	1309(%rsi,%rax,2), %edx
	addl	%edx, %ebx
	movswl	%bx, %ecx
	shll	$2, %ecx
	leal	(%rcx,%rcx,8), %ecx
	shrl	$16, %ecx
	imull	$-5860, %ebx, %ebx      # imm = 0xE91C
	movswl	%bx, %ebx
	imull	$2863, %ebx, %ebx       # imm = 0xB2F
	shrl	$16, %ebx
	subl	%ebx, %ecx
	movzbl	1308(%rsi,%rax,2), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%ecx, %ebx
	shll	$8, %edx
	orl	%r9d, %edx
	subl	%ebx, %edx
	imull	$7119, %edx, %ecx       # imm = 0x1BCF
	leal	-2863(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %ecx
	addl	$-2863, %ecx            # imm = 0xF4D1
	movw	%bx, -72(%rsp,%rax,4)
	movw	%cx, -70(%rsp,%rax,4)
	decq	%rax
	cmpq	$-7, %rax
	jne	.LBB0_4
# %bb.5:
	movswl	-68(%rsp), %r8d
	movl	%r8d, %eax
	shll	$7, %eax
	leal	(%rax,%rax,2), %eax
	negl	%eax
	shrl	$16, %eax
	imull	$-19600, %r8d, %edx     # imm = 0xB370
	movswl	%dx, %edx
	imull	$856, %edx, %edx        # imm = 0x358
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1295(%rsi), %r9d
	addl	%r9d, %eax
	movswl	%ax, %ebx
	shll	$7, %ebx
	leal	(%rbx,%rbx,2), %ebx
	negl	%ebx
	shrl	$16, %ebx
	imull	$-19600, %eax, %eax     # imm = 0xB370
	cwtl
	imull	$856, %eax, %eax        # imm = 0x358
	shrl	$16, %eax
	subl	%eax, %ebx
	movzbl	1294(%rsi), %ecx
	addl	%ecx, %ebx
	movswl	%bx, %edx
	shrl	$15, %edx
	movl	$856, %r10d             # imm = 0x358
	andl	%r10d, %edx
	addl	%ebx, %edx
	shll	$13, %r8d
	shll	$5, %r9d
	orl	%r8d, %r9d
	movswl	%dx, %edx
	subl	%edx, %ecx
	shrl	$3, %ecx
	addl	%r9d, %ecx
	imull	$-21437, %ecx, %ecx     # imm = 0xAC43
	leal	-5227(%rcx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	$5227, %ebx             # imm = 0x146B
	addl	%ebx, %ecx
	addl	$-5227, %ecx            # imm = 0xEB95
	movw	%dx, -8(%rsp)
	movw	%cx, -6(%rsp)
	xorl	%ecx, %ecx
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-70(%rsp,%rcx,2), %r8d
	movl	%r8d, %ebx
	shll	$7, %ebx
	leal	(%rbx,%rbx,2), %ebx
	negl	%ebx
	shrl	$16, %ebx
	imull	$-19600, %r8d, %eax     # imm = 0xB370
	cwtl
	imull	$856, %eax, %eax        # imm = 0x358
	shrl	$16, %eax
	subl	%eax, %ebx
	movzbl	1293(%rsi,%rcx), %eax
	addl	%eax, %ebx
	movswl	%bx, %edx
	shrl	$15, %edx
	andl	%r10d, %edx
	addl	%ebx, %edx
	shll	$5, %r8d
	movswl	%dx, %edx
	subl	%edx, %eax
	shrl	$3, %eax
	addl	%r8d, %eax
	imull	$-21437, %eax, %eax     # imm = 0xAC43
	leal	-856(%rax), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r10d, %ebx
	addl	%ebx, %eax
	addl	$-856, %eax             # imm = 0xFCA8
	movw	%dx, -12(%rsp,%rcx,4)
	movw	%ax, -10(%rsp,%rcx,4)
	decq	%rcx
	cmpq	$-14, %rcx
	jne	.LBB0_6
# %bb.7:
	movswl	-6(%rsp), %eax
	imull	$-116, %eax, %edx
	shrl	$16, %edx
	imull	$29687, %eax, %ecx      # imm = 0x73F7
	movswl	%cx, %ecx
	imull	$468, %ecx, %ebx        # imm = 0x1D4
	shrl	$16, %ebx
	movzbl	1279(%rsi), %ecx
	addl	%ecx, %edx
	subl	%ebx, %edx
	leal	-468(%rdx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	movl	$468, %r8d              # imm = 0x1D4
	andl	%r8d, %ebx
	addl	%ebx, %edx
	addl	$-468, %edx             # imm = 0xFE2C
	movswl	%dx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%edx, %ebx
	shll	$6, %eax
	movswl	%bx, %edx
	subl	%edx, %ecx
	shrl	$2, %ecx
	addl	%eax, %ecx
	imull	$-12323, %ecx, %eax     # imm = 0xCFDD
	leal	-2859(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$2859, %ecx             # imm = 0xB2B
	vpbroadcastw	.LCPI0_0(%rip), %ymm0 # ymm0 = [65420,65420,65420,65420,65420,65420,65420,65420,65420,65420,65420,65420,65420,65420,65420,65420]
	addl	%ecx, %eax
	addl	$-2859, %eax            # imm = 0xF4D5
	vpbroadcastw	.LCPI0_1(%rip), %ymm1 # ymm1 = [29687,29687,29687,29687,29687,29687,29687,29687,29687,29687,29687,29687,29687,29687,29687,29687]
	movw	%dx, 116(%rsp)
	vpbroadcastw	.LCPI0_2(%rip), %ymm2 # ymm2 = [468,468,468,468,468,468,468,468,468,468,468,468,468,468,468,468]
	movw	%ax, 118(%rsp)
	vpbroadcastw	.LCPI0_3(%rip), %ymm3 # ymm3 = [65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068]
	movl	$13, %eax
	vpbroadcastw	.LCPI0_4(%rip), %ymm4 # ymm4 = [53213,53213,53213,53213,53213,53213,53213,53213,53213,53213,53213,53213,53213,53213,53213,53213]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm5
	vpmovzxbw	1250(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm5, %ymm7
	vpmullw	%ymm1, %ymm5, %ymm8
	vpmulhw	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm3, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$6, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$2, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm2, %ymm6
	vpandn	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, (%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 32(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_10
# %bb.9:                                #   in Loop: Header=BB0_8 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_8
.LBB0_10:
	movswl	118(%rsp), %eax
	leal	(%rax,%rax,4), %ecx
	leal	(%rax,%rcx,4), %edx
	addl	%eax, %edx
	shrl	$16, %edx
	imull	$17047, %eax, %ecx      # imm = 0x4297
	movswl	%cx, %ecx
	imull	$346, %ecx, %ebx        # imm = 0x15A
	shrl	$16, %ebx
	movzbl	1249(%rsi), %ecx
	addl	%ecx, %edx
	subl	%ebx, %edx
	leal	-346(%rdx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	movl	$346, %r8d              # imm = 0x15A
	andl	%r8d, %ebx
	addl	%ebx, %edx
	addl	$-346, %edx             # imm = 0xFEA6
	movswl	%dx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%edx, %ebx
	shll	$7, %eax
	movswl	%bx, %edx
	subl	%edx, %ecx
	shrl	%ecx
	addl	%eax, %ecx
	imull	$25381, %ecx, %eax      # imm = 0x6325
	leal	-2115(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$2115, %ecx             # imm = 0x843
	vpbroadcastw	.LCPI0_5(%rip), %ymm0 # ymm0 = [22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22]
	addl	%ecx, %eax
	addl	$-2115, %eax            # imm = 0xF7BD
	vpbroadcastw	.LCPI0_6(%rip), %ymm1 # ymm1 = [17047,17047,17047,17047,17047,17047,17047,17047,17047,17047,17047,17047,17047,17047,17047,17047]
	movw	%dx, 364(%rsp)
	vpbroadcastw	.LCPI0_7(%rip), %ymm2 # ymm2 = [346,346,346,346,346,346,346,346,346,346,346,346,346,346,346,346]
	movw	%ax, 366(%rsp)
	vpbroadcastw	.LCPI0_8(%rip), %ymm3 # ymm3 = [65190,65190,65190,65190,65190,65190,65190,65190,65190,65190,65190,65190,65190,65190,65190,65190]
	movl	$43, %eax
	vpbroadcastw	.LCPI0_9(%rip), %ymm4 # ymm4 = [25381,25381,25381,25381,25381,25381,25381,25381,25381,25381,25381,25381,25381,25381,25381,25381]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	(%rsp,%rax,2), %ymm5
	vpmovzxbw	1190(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm5, %ymm7
	vpmullw	%ymm1, %ymm5, %ymm8
	vpmulhw	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm3, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$7, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$1, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm2, %ymm6
	vpandn	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 128(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 160(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_13
# %bb.12:                               #   in Loop: Header=BB0_11 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_11
.LBB0_13:
	movzwl	366(%rsp), %eax
	vpbroadcastw	.LCPI0_10(%rip), %ymm1 # ymm1 = [64988,64988,64988,64988,64988,64988,64988,64988,64988,64988,64988,64988,64988,64988,64988,64988]
	movw	%ax, 844(%rsp)
	vpbroadcastw	.LCPI0_11(%rip), %ymm2 # ymm2 = [62012,62012,62012,62012,62012,62012,62012,62012,62012,62012,62012,62012,62012,62012,62012,62012]
	movl	$103, %eax
	vpbroadcastw	.LCPI0_12(%rip), %ymm3 # ymm3 = [4761,4761,4761,4761,4761,4761,4761,4761,4761,4761,4761,4761,4761,4761,4761,4761]
	vmovdqa	.LCPI0_13(%rip), %ymm0  # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_14(%rip), %ymm4 # ymm4 = [8617,8617,8617,8617,8617,8617,8617,8617,8617,8617,8617,8617,8617,8617,8617,8617]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	128(%rsp,%rax,2), %ymm5
	vmovdqu	952(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm6, 368(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 400(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_15(%rip), %ymm1 # ymm1 = [65522,65522,65522,65522,65522,65522,65522,65522,65522,65522,65522,65522,65522,65522,65522,65522]
	movzwl	844(%rsp), %eax
	vpbroadcastw	.LCPI0_16(%rip), %ymm2 # ymm2 = [64586,64586,64586,64586,64586,64586,64586,64586,64586,64586,64586,64586,64586,64586,64586,64586]
	movw	%ax, 1800(%rsp)
	vpbroadcastw	.LCPI0_17(%rip), %ymm3 # ymm3 = [69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69]
	movl	$222, %eax
	vpbroadcastw	.LCPI0_18(%rip), %ymm4 # ymm4 = [4749,4749,4749,4749,4749,4749,4749,4749,4749,4749,4749,4749,4749,4749,4749,4749]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	368(%rsp,%rax,2), %ymm5
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
	vmovdqu	%ymm6, 848(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 880(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	vpbroadcastw	.LCPI0_19(%rip), %ymm1 # ymm1 = [1036,1036,1036,1036,1036,1036,1036,1036,1036,1036,1036,1036,1036,1036,1036,1036]
	vpbroadcastw	.LCPI0_20(%rip), %ymm2 # ymm2 = [57604,57604,57604,57604,57604,57604,57604,57604,57604,57604,57604,57604,57604,57604,57604,57604]
	movl	1800(%rsp), %eax
	vpbroadcastw	.LCPI0_21(%rip), %ymm3 # ymm3 = [2115,2115,2115,2115,2115,2115,2115,2115,2115,2115,2115,2115,2115,2115,2115,2115]
	leal	(%rax,%rax,2), %eax
	addl	$-3171, %eax            # imm = 0xF39D
	vpbroadcastw	.LCPI0_22(%rip), %ymm4 # ymm4 = [33899,33899,33899,33899,33899,33899,33899,33899,33899,33899,33899,33899,33899,33899,33899,33899]
	movw	%ax, 1904(%rdi)
	vpbroadcastw	.LCPI0_23(%rip), %ymm5 # ymm5 = [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
	movl	$460, %eax              # imm = 0x1CC
	vpbroadcastw	.LCPI0_24(%rip), %ymm6 # ymm6 = [62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	848(%rsp,%rax,2), %ymm7
	vmovdqu	(%rsi,%rax,2), %ymm8
	vpsrlw	$8, %ymm8, %ymm9
	vpmulhw	%ymm1, %ymm7, %ymm10
	vpmullw	%ymm2, %ymm7, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm10, %ymm7
	vpaddw	%ymm7, %ymm9, %ymm7
	vpmulhw	%ymm1, %ymm7, %ymm9
	vpmullw	%ymm2, %ymm7, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm9, %ymm7
	vpand	%ymm0, %ymm8, %ymm9
	vpaddw	%ymm7, %ymm9, %ymm7
	vpsraw	$15, %ymm7, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm7, %ymm9, %ymm7
	vpsubw	%ymm7, %ymm8, %ymm8
	vpmullw	%ymm4, %ymm8, %ymm8
	vpcmpgtw	%ymm8, %ymm3, %ymm9
	vpandn	%ymm3, %ymm9, %ymm9
	vpsubw	%ymm9, %ymm8, %ymm8
	vpmullw	%ymm5, %ymm7, %ymm7
	vpmullw	%ymm5, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm8, %ymm8
	vpunpcklwd	%ymm8, %ymm7, %ymm9 # ymm9 = ymm7[0],ymm8[0],ymm7[1],ymm8[1],ymm7[2],ymm8[2],ymm7[3],ymm8[3],ymm7[8],ymm8[8],ymm7[9],ymm8[9],ymm7[10],ymm8[10],ymm7[11],ymm8[11]
	vpunpckhwd	%ymm8, %ymm7, %ymm7 # ymm7 = ymm7[4],ymm8[4],ymm7[5],ymm8[5],ymm7[6],ymm8[6],ymm7[7],ymm8[7],ymm7[12],ymm8[12],ymm7[13],ymm8[13],ymm7[14],ymm8[14],ymm7[15],ymm8[15]
	vinserti128	$1, %xmm7, %ymm9, %ymm8
	vmovdqu	%ymm8, (%rdi,%rax,4)
	vperm2i128	$49, %ymm7, %ymm9, %ymm7 # ymm7 = ymm9[2,3],ymm7[2,3]
	vmovdqu	%ymm7, 32(%rdi,%rax,4)
	testq	%rax, %rax
	je	.LBB0_22
# %bb.21:                               #   in Loop: Header=BB0_20 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_20
.LBB0_22:
	addq	$1808, %rsp             # imm = 0x710
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_953x2115), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_953x2115)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
