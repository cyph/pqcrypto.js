	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_953x6343)
.LCPI0_0:
	.short	65234                   # 0xfed2
.LCPI0_1:
	.short	50242                   # 0xc442
.LCPI0_2:
	.short	1097                    # 0x449
.LCPI0_3:
	.short	11769                   # 0x2df9
.LCPI0_4:
	.short	3212                    # 0xc8c
.LCPI0_5:
	.short	63557                   # 0xf845
.LCPI0_6:
	.short	8476                    # 0x211c
.LCPI0_8:
	.short	8567                    # 0x2177
.LCPI0_9:
	.short	65282                   # 0xff02
.LCPI0_10:
	.short	54146                   # 0xd382
.LCPI0_11:
	.short	1473                    # 0x5c1
.LCPI0_12:
	.short	2625                    # 0xa41
.LCPI0_13:
	.short	280                     # 0x118
.LCPI0_14:
	.short	38212                   # 0x9544
.LCPI0_15:
	.short	614                     # 0x266
.LCPI0_16:
	.short	64922                   # 0xfd9a
.LCPI0_17:
	.short	57851                   # 0xe1fb
.LCPI0_18:
	.short	65517                   # 0xffed
.LCPI0_19:
	.short	62891                   # 0xf5ab
.LCPI0_20:
	.short	6343                    # 0x18c7
.LCPI0_21:
	.short	10487                   # 0x28f7
.LCPI0_22:
	.short	62365                   # 0xf39d
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_953x6343)
	.type	CRYPTO_NAMESPACE(crypto_decode_953x6343),@function
CRYPTO_NAMESPACE(crypto_decode_953x6343): # @CRYPTO_NAMESPACE(crypto_decode_953x6343)
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	subq	$1808, %rsp             # imm = 0x710
	.cfi_def_cfa_offset 1840
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	.cfi_offset %rbp, -16
	movzbl	1504(%rsi), %eax
	imull	$-272, %eax, %ecx       # imm = 0xFEF0
	shrl	$16, %ecx
	imull	$-87556096, %eax, %eax  # imm = 0xFAC80000
	sarl	$16, %eax
	imull	$12558, %eax, %eax      # imm = 0x310E
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1503(%rsi), %edx
	addl	%ecx, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$12558, %eax            # imm = 0x310E
	andl	%ecx, %eax
	addl	%edx, %eax
	movswl	%ax, %ecx
	imull	$896, %ecx, %edx        # imm = 0x380
	shrl	$16, %edx
	imull	$-7040, %eax, %ecx      # imm = 0xE480
	movswl	%cx, %ecx
	imull	$2383, %ecx, %ecx       # imm = 0x94F
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1502(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$2383, %r9d             # imm = 0x94F
	andl	%ecx, %r9d
	addl	%edx, %r9d
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%r9d, %eax
	imull	$28079, %eax, %eax      # imm = 0x6DAF
	leal	-1349(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$1349, %ecx             # imm = 0x545
	leal	(%rcx,%rax), %edx
	addl	$-1349, %edx            # imm = 0xFABB
	movswl	%dx, %eax
	imull	$-226, %eax, %ecx
	shrl	$16, %ecx
	imull	$-21482, %edx, %eax     # imm = 0xAC16
	cwtl
	imull	$781, %eax, %eax        # imm = 0x30D
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1501(%rsi), %r10d
	addl	%r10d, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	movl	$781, %r8d              # imm = 0x30D
	andl	%r8d, %eax
	addl	%ecx, %eax
	shll	$8, %edx
	orl	%r10d, %edx
	subl	%eax, %edx
	imull	$-31803, %edx, %ecx     # imm = 0x83C5
	leal	-442(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$442, %edx              # imm = 0x1BA
	leal	(%rdx,%rcx), %r10d
	addl	$-442, %r10d            # imm = 0xFE46
	movw	%ax, -116(%rsp)
	movw	%r10w, -114(%rsp)
	movswl	%r9w, %ecx
	imull	$-226, %ecx, %ecx
	shrl	$16, %ecx
	imull	$-21482, %r9d, %edx     # imm = 0xAC16
	movswl	%dx, %edx
	imull	$781, %edx, %edx        # imm = 0x30D
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1500(%rsi), %r11d
	addl	%r11d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%ecx, %edx
	shll	$8, %r9d
	orl	%r11d, %r9d
	subl	%edx, %r9d
	imull	$-31803, %r9d, %r9d     # imm = 0x83C5
	leal	-781(%r9), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	%r8d, %ecx
	addl	%r9d, %ecx
	addl	$-781, %ecx             # imm = 0xFCF3
	movw	%dx, -120(%rsp)
	movw	%cx, -118(%rsp)
	movswl	%r10w, %ecx
	imull	$-35, %ecx, %r9d
	shrl	$16, %r9d
	imull	$28003, %r10d, %ecx     # imm = 0x6D63
	movswl	%cx, %ecx
	imull	$447, %ecx, %ecx        # imm = 0x1BF
	shrl	$16, %ecx
	movzbl	1499(%rsi), %r8d
	addl	%r8d, %r9d
	subl	%ecx, %r9d
	leal	-447(%r9), %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$447, %r14d             # imm = 0x1BF
	andl	%r14d, %edx
	addl	%edx, %r9d
	addl	$-447, %r9d             # imm = 0xFE41
	movswl	%r9w, %r11d
	shrl	$15, %r11d
	andl	%r14d, %r11d
	addl	%r9d, %r11d
	shll	$8, %r10d
	orl	%r8d, %r10d
	subl	%r11d, %r10d
	imull	$-4545, %r10d, %r8d     # imm = 0xEE3F
	leal	-253(%r8), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$253, %edx
	addl	%r8d, %edx
	addl	$-253, %edx
	movw	%r11w, -100(%rsp)
	movw	%dx, -98(%rsp)
	xorl	%edx, %edx
	movl	$-447, %r8d             # imm = 0xFE41
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%ax, %ecx
	imull	$-35, %ecx, %ecx
	shrl	$16, %ecx
	imull	$28003, %eax, %ebx      # imm = 0x6D63
	movswl	%bx, %ebx
	imull	$447, %ebx, %ebx        # imm = 0x1BF
	shrl	$16, %ebx
	movzbl	1498(%rsi,%rdx), %ebp
	addl	%ebp, %ecx
	subl	%ebx, %ecx
	leal	(%rcx,%r8), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r14d, %ebx
	addl	%ebx, %ecx
	addl	$-447, %ecx             # imm = 0xFE41
	movswl	%cx, %ebx
	shrl	$15, %ebx
	andl	%r14d, %ebx
	addl	%ecx, %ebx
	shll	$8, %eax
	orl	%ebp, %eax
	subl	%ebx, %eax
	imull	$-4545, %eax, %eax      # imm = 0xEE3F
	leal	(%rax,%r8), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	%r14d, %ecx
	addl	%ecx, %eax
	addl	$-447, %eax             # imm = 0xFE41
	movw	%bx, -104(%rsp,%rdx,4)
	movw	%ax, -102(%rsp,%rdx,4)
	cmpq	$-2, %rdx
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-118(%rsp,%rdx,2), %eax
	decq	%rdx
	jmp	.LBB0_1
.LBB0_3:
	movzwl	-98(%rsp), %eax
	movw	%ax, -68(%rsp)
	xorl	%eax, %eax
	movl	$-338, %r8d             # imm = 0xFEAE
	movl	$338, %r9d              # imm = 0x152
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%rax,2), %ecx
	imull	$-90, %ecx, %ebp
	shrl	$16, %ebp
	imull	$15899, %ecx, %edx      # imm = 0x3E1B
	movswl	%dx, %edx
	imull	$338, %edx, %ebx        # imm = 0x152
	shrl	$16, %ebx
	movzbl	1495(%rsi,%rax), %edx
	addl	%edx, %ebp
	subl	%ebx, %ebp
	leal	(%r8,%rbp), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebx, %ebp
	addl	$-338, %ebp             # imm = 0xFEAE
	movswl	%bp, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebp, %ebx
	shll	$7, %ecx
	movswl	%bx, %ebp
	subl	%ebp, %edx
	shrl	%edx
	addl	%ecx, %edx
	imull	$-23655, %edx, %ecx     # imm = 0xA399
	leal	(%rcx,%r8), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%edx, %ecx
	addl	$-338, %ecx             # imm = 0xFEAE
	movw	%bp, -72(%rsp,%rax,4)
	movw	%cx, -70(%rsp,%rax,4)
	decq	%rax
	cmpq	$-7, %rax
	jne	.LBB0_4
# %bb.5:
	movswl	-68(%rsp), %eax
	imull	$-653, %eax, %edx       # imm = 0xFD73
	shrl	$16, %edx
	imull	$-3569, %eax, %eax      # imm = 0xF20F
	cwtl
	imull	$4701, %eax, %eax       # imm = 0x125D
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1488(%rsi), %ecx
	addl	%ecx, %edx
	movswl	%dx, %eax
	imull	$-653, %eax, %ebp       # imm = 0xFD73
	shrl	$16, %ebp
	imull	$-3569, %edx, %eax      # imm = 0xF20F
	cwtl
	imull	$4701, %eax, %eax       # imm = 0x125D
	shrl	$16, %eax
	subl	%eax, %ebp
	movzbl	1487(%rsi), %edx
	addl	%edx, %ebp
	movswl	%bp, %ebx
	shrl	$15, %ebx
	movl	$4701, %r8d             # imm = 0x125D
	andl	%r8d, %ebx
	addl	%ebp, %ebx
	shll	$8, %ecx
	orl	%edx, %ecx
	subl	%ebx, %ecx
	imull	$20981, %ecx, %ecx      # imm = 0x51F5
	leal	-3519(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$3519, %edx             # imm = 0xDBF
	addl	%edx, %ecx
	addl	$-3519, %ecx            # imm = 0xF241
	movw	%bx, -8(%rsp)
	movw	%cx, -6(%rsp)
	xorl	%ecx, %ecx
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-70(%rsp,%rcx,2), %edx
	imull	$-653, %edx, %ebp       # imm = 0xFD73
	shrl	$16, %ebp
	imull	$-3569, %edx, %edx      # imm = 0xF20F
	movswl	%dx, %edx
	imull	$4701, %edx, %edx       # imm = 0x125D
	shrl	$16, %edx
	subl	%edx, %ebp
	movzbl	1486(%rsi,%rcx,2), %edx
	addl	%edx, %ebp
	movswl	%bp, %ebx
	imull	$-653, %ebx, %ebx       # imm = 0xFD73
	shrl	$16, %ebx
	imull	$-3569, %ebp, %ebp      # imm = 0xF20F
	movswl	%bp, %ebp
	imull	$4701, %ebp, %ebp       # imm = 0x125D
	shrl	$16, %ebp
	subl	%ebp, %ebx
	movzbl	1485(%rsi,%rcx,2), %ebp
	addl	%ebp, %ebx
	movswl	%bx, %eax
	shrl	$15, %eax
	andl	%r8d, %eax
	addl	%ebx, %eax
	shll	$8, %edx
	orl	%ebp, %edx
	subl	%eax, %edx
	imull	$20981, %edx, %edx      # imm = 0x51F5
	leal	-4701(%rdx), %ebp
	movswl	%bp, %ebp
	shrl	$15, %ebp
	andl	%r8d, %ebp
	addl	%ebp, %edx
	addl	$-4701, %edx            # imm = 0xEDA3
	movw	%ax, -12(%rsp,%rcx,4)
	movw	%dx, -10(%rsp,%rcx,4)
	decq	%rcx
	cmpq	$-14, %rcx
	jne	.LBB0_6
# %bb.7:
	movswl	-6(%rsp), %eax
	imull	$-302, %eax, %ecx       # imm = 0xFED2
	shrl	$16, %ecx
	imull	$-15294, %eax, %edx     # imm = 0xC442
	movswl	%dx, %edx
	imull	$1097, %edx, %edx       # imm = 0x449
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1458(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %ebp
	shrl	$15, %ebp
	movl	$1097, %ebx             # imm = 0x449
	andl	%ebp, %ebx
	addl	%ecx, %ebx
	shll	$8, %eax
	orl	%edx, %eax
	subl	%ebx, %eax
	imull	$11769, %eax, %eax      # imm = 0x2DF9
	leal	-821(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$821, %ecx              # imm = 0x335
	addl	%ecx, %eax
	addl	$-821, %eax             # imm = 0xFCCB
	vpbroadcastw	.LCPI0_0(%rip), %ymm0 # ymm0 = [65234,65234,65234,65234,65234,65234,65234,65234,65234,65234,65234,65234,65234,65234,65234,65234]
	movw	%bx, 116(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %ymm1 # ymm1 = [50242,50242,50242,50242,50242,50242,50242,50242,50242,50242,50242,50242,50242,50242,50242,50242]
	movw	%ax, 118(%rsp)
	vpbroadcastw	.LCPI0_2(%rip), %ymm2 # ymm2 = [1097,1097,1097,1097,1097,1097,1097,1097,1097,1097,1097,1097,1097,1097,1097,1097]
	movl	$13, %eax
	vpbroadcastw	.LCPI0_3(%rip), %ymm3 # ymm3 = [11769,11769,11769,11769,11769,11769,11769,11769,11769,11769,11769,11769,11769,11769,11769,11769]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm4
	vpmovzxbw	1429(%rsi,%rax), %ymm5 # ymm5 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
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
	vmovdqu	%ymm6, (%rsp,%rax,4)
	vperm2i128	$49, %ymm4, %ymm5, %ymm4 # ymm4 = ymm5[2,3],ymm4[2,3]
	vmovdqu	%ymm4, 32(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_10
# %bb.9:                                #   in Loop: Header=BB0_8 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_8
.LBB0_10:
	movswl	118(%rsp), %r8d
	imull	$3212, %r8d, %edx       # imm = 0xC8C
	shrl	$16, %edx
	imull	$-1979, %r8d, %ecx      # imm = 0xF845
	movswl	%cx, %ecx
	imull	$8476, %ecx, %ecx       # imm = 0x211C
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1428(%rsi), %ecx
	addl	%ecx, %edx
	movswl	%dx, %ebp
	imull	$3212, %ebp, %ebp       # imm = 0xC8C
	shrl	$16, %ebp
	imull	$-1979, %edx, %edx      # imm = 0xF845
	movswl	%dx, %edx
	imull	$8476, %edx, %edx       # imm = 0x211C
	shrl	$16, %edx
	subl	%edx, %ebp
	movzbl	1427(%rsi), %edx
	addl	%edx, %ebp
	movswl	%bp, %ebx
	shrl	$15, %ebx
	movl	$8476, %eax             # imm = 0x211C
	andl	%ebx, %eax
	addl	%ebp, %eax
	shll	$14, %r8d
	shll	$6, %ecx
	orl	%r8d, %ecx
	cwtl
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%ecx, %edx
	imull	$8567, %edx, %ecx       # imm = 0x2177
	leal	-6343(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$6343, %edx             # imm = 0x18C7
	addl	%edx, %ecx
	addl	$-6343, %ecx            # imm = 0xE739
	movw	%ax, 364(%rsp)
	vpbroadcastw	.LCPI0_4(%rip), %ymm1 # ymm1 = [3212,3212,3212,3212,3212,3212,3212,3212,3212,3212,3212,3212,3212,3212,3212,3212]
	movw	%cx, 366(%rsp)
	vpbroadcastw	.LCPI0_5(%rip), %ymm2 # ymm2 = [63557,63557,63557,63557,63557,63557,63557,63557,63557,63557,63557,63557,63557,63557,63557,63557]
	movl	$43, %eax
	vpbroadcastw	.LCPI0_6(%rip), %ymm3 # ymm3 = [8476,8476,8476,8476,8476,8476,8476,8476,8476,8476,8476,8476,8476,8476,8476,8476]
	vmovdqa	.LCPI0_7(%rip), %ymm0   # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_8(%rip), %ymm4 # ymm4 = [8567,8567,8567,8567,8567,8567,8567,8567,8567,8567,8567,8567,8567,8567,8567,8567]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	(%rsp,%rax,2), %ymm5
	vmovdqu	1309(%rsi,%rax,2), %ymm6
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
	vpsllw	$14, %ymm5, %ymm5
	vpsllw	$6, %ymm7, %ymm7
	vpor	%ymm5, %ymm7, %ymm5
	vpsubw	%ymm8, %ymm6, %ymm6
	vpsraw	$2, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm8, %ymm6 # ymm6 = ymm8[0],ymm5[0],ymm8[1],ymm5[1],ymm8[2],ymm5[2],ymm8[3],ymm5[3],ymm8[8],ymm5[8],ymm8[9],ymm5[9],ymm8[10],ymm5[10],ymm8[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm8, %ymm5 # ymm5 = ymm8[4],ymm5[4],ymm8[5],ymm5[5],ymm8[6],ymm5[6],ymm8[7],ymm5[7],ymm8[12],ymm5[12],ymm8[13],ymm5[13],ymm8[14],ymm5[14],ymm8[15],ymm5[15]
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
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [65282,65282,65282,65282,65282,65282,65282,65282,65282,65282,65282,65282,65282,65282,65282,65282]
	movzwl	366(%rsp), %eax
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [54146,54146,54146,54146,54146,54146,54146,54146,54146,54146,54146,54146,54146,54146,54146,54146]
	movw	%ax, 844(%rsp)
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [1473,1473,1473,1473,1473,1473,1473,1473,1473,1473,1473,1473,1473,1473,1473,1473]
	movl	$103, %eax
	vpbroadcastw	.LCPI0_12(%rip), %ymm4 # ymm4 = [2625,2625,2625,2625,2625,2625,2625,2625,2625,2625,2625,2625,2625,2625,2625,2625]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	128(%rsp,%rax,2), %ymm5
	vpmovzxbw	1190(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$8, %ymm5, %ymm5
	vpor	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm5, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 368(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 400(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_13(%rip), %ymm1 # ymm1 = [280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280]
	vpbroadcastw	.LCPI0_14(%rip), %ymm2 # ymm2 = [38212,38212,38212,38212,38212,38212,38212,38212,38212,38212,38212,38212,38212,38212,38212,38212]
	movzwl	844(%rsp), %eax
	vpbroadcastw	.LCPI0_15(%rip), %ymm3 # ymm3 = [614,614,614,614,614,614,614,614,614,614,614,614,614,614,614,614]
	movw	%ax, 1800(%rsp)
	vpbroadcastw	.LCPI0_16(%rip), %ymm4 # ymm4 = [64922,64922,64922,64922,64922,64922,64922,64922,64922,64922,64922,64922,64922,64922,64922,64922]
	movl	$222, %eax
	vpbroadcastw	.LCPI0_17(%rip), %ymm5 # ymm5 = [57851,57851,57851,57851,57851,57851,57851,57851,57851,57851,57851,57851,57851,57851,57851,57851]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	368(%rsp,%rax,2), %ymm6
	vpmovzxbw	952(%rsi,%rax), %ymm7 # ymm7 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm6, %ymm8
	vpmullw	%ymm2, %ymm6, %ymm9
	vpmulhw	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm7, %ymm8, %ymm8
	vpsubw	%ymm9, %ymm8, %ymm8
	vpaddw	%ymm4, %ymm8, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsllw	$7, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm7, %ymm7
	vpsraw	$1, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpmullw	%ymm5, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm8, %ymm7 # ymm7 = ymm8[0],ymm6[0],ymm8[1],ymm6[1],ymm8[2],ymm6[2],ymm8[3],ymm6[3],ymm8[8],ymm6[8],ymm8[9],ymm6[9],ymm8[10],ymm6[10],ymm8[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm8, %ymm6 # ymm6 = ymm8[4],ymm6[4],ymm8[5],ymm6[5],ymm8[6],ymm6[6],ymm8[7],ymm6[7],ymm8[12],ymm6[12],ymm8[13],ymm6[13],ymm8[14],ymm6[14],ymm8[15],ymm6[15]
	vinserti128	$1, %xmm6, %ymm7, %ymm8
	vmovdqu	%ymm8, 848(%rsp,%rax,4)
	vperm2i128	$49, %ymm6, %ymm7, %ymm6 # ymm6 = ymm7[2,3],ymm6[2,3]
	vmovdqu	%ymm6, 880(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	movl	$-3171, %eax            # imm = 0xF39D
	addl	1800(%rsp), %eax
	vpbroadcastw	.LCPI0_18(%rip), %ymm1 # ymm1 = [65517,65517,65517,65517,65517,65517,65517,65517,65517,65517,65517,65517,65517,65517,65517,65517]
	vpbroadcastw	.LCPI0_19(%rip), %ymm2 # ymm2 = [62891,62891,62891,62891,62891,62891,62891,62891,62891,62891,62891,62891,62891,62891,62891,62891]
	vpbroadcastw	.LCPI0_20(%rip), %ymm3 # ymm3 = [6343,6343,6343,6343,6343,6343,6343,6343,6343,6343,6343,6343,6343,6343,6343,6343]
	movw	%ax, 1904(%rdi)
	vpbroadcastw	.LCPI0_21(%rip), %ymm4 # ymm4 = [10487,10487,10487,10487,10487,10487,10487,10487,10487,10487,10487,10487,10487,10487,10487,10487]
	movl	$460, %eax              # imm = 0x1CC
	vpbroadcastw	.LCPI0_22(%rip), %ymm5 # ymm5 = [62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365,62365]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	848(%rsp,%rax,2), %ymm6
	vmovdqu	(%rsi,%rax,2), %ymm7
	vpsrlw	$8, %ymm7, %ymm8
	vpmulhw	%ymm1, %ymm6, %ymm9
	vpmullw	%ymm2, %ymm6, %ymm6
	vpmulhw	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm9, %ymm6
	vpaddw	%ymm6, %ymm8, %ymm6
	vpmulhw	%ymm1, %ymm6, %ymm8
	vpmullw	%ymm2, %ymm6, %ymm6
	vpmulhw	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm8, %ymm6
	vpand	%ymm0, %ymm7, %ymm8
	vpaddw	%ymm6, %ymm8, %ymm6
	vpsraw	$15, %ymm6, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm8, %ymm6
	vpsubw	%ymm6, %ymm7, %ymm7
	vpmullw	%ymm4, %ymm7, %ymm7
	vpcmpgtw	%ymm7, %ymm3, %ymm8
	vpandn	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm5, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm7, %ymm7
	vpunpcklwd	%ymm7, %ymm6, %ymm8 # ymm8 = ymm6[0],ymm7[0],ymm6[1],ymm7[1],ymm6[2],ymm7[2],ymm6[3],ymm7[3],ymm6[8],ymm7[8],ymm6[9],ymm7[9],ymm6[10],ymm7[10],ymm6[11],ymm7[11]
	vpunpckhwd	%ymm7, %ymm6, %ymm6 # ymm6 = ymm6[4],ymm7[4],ymm6[5],ymm7[5],ymm6[6],ymm7[6],ymm6[7],ymm7[7],ymm6[12],ymm7[12],ymm6[13],ymm7[13],ymm6[14],ymm7[14],ymm6[15],ymm7[15]
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
	addq	$1808, %rsp             # imm = 0x710
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_953x6343), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_953x6343)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
