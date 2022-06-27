	.text
	.file	"encode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_encode_1013x3)
.LCPI0_0:
	.short	255                     # 0xff
.LCPI0_1:
	.byte	252                     # 0xfc
.LCPI0_2:
	.byte	240                     # 0xf0
.LCPI0_4:
	.byte	85                      # 0x55
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_3:
	.long	0                       # 0x0
	.long	4                       # 0x4
	.long	1                       # 0x1
	.long	5                       # 0x5
	.long	2                       # 0x2
	.long	6                       # 0x6
	.long	3                       # 0x3
	.long	7                       # 0x7
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_1013x3)
	.type	CRYPTO_NAMESPACE(crypto_encode_1013x3),@function
CRYPTO_NAMESPACE(crypto_encode_1013x3): # @CRYPTO_NAMESPACE(crypto_encode_1013x3)
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	leaq	116(%rsi), %r11
	leaq	29(%rdi), %rax
	movl	$9, %ecx
	vpbroadcastw	.LCPI0_0(%rip), %ymm0 # ymm0 = [255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255]
	movq	$-3, %rbx
	vpbroadcastb	.LCPI0_1(%rip), %ymm1 # ymm1 = [252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252]
	vpbroadcastb	.LCPI0_2(%rip), %ymm2 # ymm2 = [240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240]
	vmovdqa	.LCPI0_3(%rip), %ymm3   # ymm3 = [0,4,1,5,2,6,3,7]
	vpbroadcastb	.LCPI0_4(%rip), %ymm4 # ymm4 = [85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85]
	movl	$32, %r8d
	movq	%rdi, %r9
	movq	%rsi, %rdx
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movq	%rbx, %r10
	vmovdqu	(%rdx), %ymm7
	vmovdqu	32(%rdx), %ymm8
	vmovdqu	64(%rdx), %ymm5
	vmovdqu	96(%rdx), %ymm6
	movq	%r11, %rdx
	leaq	128(%r11), %r11
	vpand	%ymm0, %ymm7, %ymm9
	vpand	%ymm0, %ymm8, %ymm10
	vpackuswb	%ymm10, %ymm9, %ymm9
	vpsrlw	$8, %ymm7, %ymm7
	vpsrlw	$8, %ymm8, %ymm8
	vpackuswb	%ymm8, %ymm7, %ymm7
	vpand	%ymm0, %ymm5, %ymm8
	vpand	%ymm0, %ymm6, %ymm10
	vpackuswb	%ymm10, %ymm8, %ymm8
	vpsrlw	$8, %ymm5, %ymm5
	vpsrlw	$8, %ymm6, %ymm6
	vpackuswb	%ymm6, %ymm5, %ymm5
	vpsllw	$2, %ymm7, %ymm6
	vpand	%ymm1, %ymm6, %ymm6
	vpaddb	%ymm6, %ymm9, %ymm6
	vpsllw	$2, %ymm5, %ymm5
	vpand	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm5, %ymm8, %ymm5
	vpand	%ymm0, %ymm6, %ymm7
	vpand	%ymm0, %ymm5, %ymm8
	vpackuswb	%ymm8, %ymm7, %ymm7
	vpsrlw	$8, %ymm6, %ymm6
	vpsrlw	$8, %ymm5, %ymm5
	vpackuswb	%ymm5, %ymm6, %ymm5
	vpsllw	$4, %ymm5, %ymm5
	vpand	%ymm2, %ymm5, %ymm5
	vpaddb	%ymm7, %ymm5, %ymm5
	vpermd	%ymm5, %ymm3, %ymm5
	vpaddb	%ymm4, %ymm5, %ymm5
	vmovdqu	%ymm5, (%r9)
	decl	%ecx
	addq	%r8, %rbx
	movq	%rax, %r9
	addq	%r8, %rax
	cmpl	$1, %ecx
	ja	.LBB0_1
# %bb.2:
	movb	128(%rsi,%r10,4), %al
	incb	%al
	movb	%al, 32(%rdi,%r10)
	popq	%rbx
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_encode_1013x3), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_encode_1013x3)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
