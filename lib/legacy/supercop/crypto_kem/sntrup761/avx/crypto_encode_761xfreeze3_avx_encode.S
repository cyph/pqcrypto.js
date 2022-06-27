	.text
	.file	"encode.c"
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5               # -- Begin function CRYPTO_NAMESPACE(crypto_encode_761xfreeze3)
.LCPI0_0:
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.short	10923                   # 0x2aab
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_761xfreeze3)
	.p2align	4, 0x90
	.type	CRYPTO_NAMESPACE(crypto_encode_761xfreeze3),@function
CRYPTO_NAMESPACE(crypto_encode_761xfreeze3): # @CRYPTO_NAMESPACE(crypto_encode_761xfreeze3)
	.cfi_startproc
# %bb.0:
	movl	$745, %eax              # imm = 0x2E9
	vmovdqa	.LCPI0_0(%rip), %ymm0   # ymm0 = [10923,10923,10923,10923,10923,10923,10923,10923,10923,10923,10923,10923,10923,10923,10923,10923]
	.p2align	4, 0x90
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movl	%eax, %ecx
	vmovdqu	(%rsi), %ymm1
	vpmulhrsw	%ymm0, %ymm1, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vpaddw	%ymm2, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vextracti128	$1, %ymm1, %xmm2
	vpacksswb	%xmm2, %xmm1, %xmm1
	vmovdqu	%xmm1, (%rdi)
	leal	-16(%rcx), %eax
	addq	$32, %rsi
	addq	$16, %rdi
	cmpl	$15, %ecx
	jg	.LBB0_1
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	testl	%ecx, %ecx
	jle	.LBB0_4
# %bb.3:                                #   in Loop: Header=BB0_1 Depth=1
	movslq	%eax, %rcx
	leaq	(%rsi,%rcx,2), %rsi
	addq	%rcx, %rdi
	jmp	.LBB0_1
.LBB0_4:
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_encode_761xfreeze3), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_encode_761xfreeze3)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
