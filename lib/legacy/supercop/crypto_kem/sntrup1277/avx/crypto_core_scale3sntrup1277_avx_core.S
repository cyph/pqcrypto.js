	.text
	.file	"core.c"
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5               # -- Begin function CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277)
.LCPI0_0:
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
	.short	3                       # 0x3
.LCPI0_1:
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
	.short	61596                   # 0xf09c
.LCPI0_2:
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
	.short	7879                    # 0x1ec7
.LCPI0_3:
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.short	61597                   # 0xf09d
	.text
	.globl	CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277)
	.p2align	4, 0x90
	.type	CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277),@function
CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277): # @CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277)
	.cfi_startproc
# %bb.0:
	vmovups	2522(%rsi), %ymm0
	movl	$1261, %eax             # imm = 0x4ED
	movl	$2490, %ecx             # imm = 0x9BA
	vmovdqa	.LCPI0_0(%rip), %ymm1   # ymm1 = [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
	vmovdqa	.LCPI0_1(%rip), %ymm2   # ymm2 = [61596,61596,61596,61596,61596,61596,61596,61596,61596,61596,61596,61596,61596,61596,61596,61596]
	vmovdqa	.LCPI0_2(%rip), %ymm3   # ymm3 = [7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879]
	vmovdqa	.LCPI0_3(%rip), %ymm4   # ymm4 = [61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597]
	jmp	.LBB0_1
	.p2align	4, 0x90
.LBB0_2:                                #   in Loop: Header=BB0_1 Depth=1
	addl	$-16, %eax
	addl	$-32, %ecx
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	vpmullw	(%rsi), %ymm1, %ymm5
	vpaddw	%ymm2, %ymm5, %ymm5
	vpsraw	$15, %ymm5, %ymm6
	vpand	%ymm3, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpsraw	$15, %ymm5, %ymm6
	vpand	%ymm3, %ymm6, %ymm6
	vpaddw	%ymm6, %ymm5, %ymm5
	vpaddw	%ymm4, %ymm5, %ymm5
	vmovdqu	%ymm5, (%rdi)
	addq	$32, %rsi
	addq	$32, %rdi
	cmpl	$16, %eax
	jge	.LBB0_2
# %bb.3:                                #   in Loop: Header=BB0_1 Depth=1
	testl	%eax, %eax
	jle	.LBB0_5
# %bb.4:                                #   in Loop: Header=BB0_1 Depth=1
	movslq	%ecx, %rdx
	addq	%rdx, %rsi
	vmovups	%ymm0, (%rdi,%rdx)
	addq	%rdx, %rdi
	jmp	.LBB0_2
.LBB0_5:
	xorl	%eax, %eax
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
