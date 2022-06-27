	.text
	.file	"weight.c"
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5               # -- Begin function CRYPTO_NAMESPACE(crypto_core_weightsntrup1013)
.LCPI0_0:
	.quad	0                       # 0x0
	.quad	72340172838010880       # 0x101010101000000
	.quad	72340172838076673       # 0x101010101010101
	.quad	72340172838076673       # 0x101010101010101
.LCPI0_2:
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
	.section	.rodata,"a",@progbits
.LCPI0_1:
	.byte	1                       # 0x1
	.text
	.globl	CRYPTO_NAMESPACE(crypto_core_weightsntrup1013)
	.type	CRYPTO_NAMESPACE(crypto_core_weightsntrup1013),@function
CRYPTO_NAMESPACE(crypto_core_weightsntrup1013): # @CRYPTO_NAMESPACE(crypto_core_weightsntrup1013)
	.cfi_startproc
# %bb.0:
	pushq	%rax
	.cfi_def_cfa_offset 16
	vmovdqu	981(%rsi), %ymm0
	vpand	.LCPI0_0(%rip), %ymm0, %ymm0
	movl	$1013, %eax             # imm = 0x3F5
	vpbroadcastb	.LCPI0_1(%rip), %ymm1 # ymm1 = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	vpand	(%rsi), %ymm1, %ymm2
	vpaddb	%ymm0, %ymm2, %ymm0
	addq	$32, %rsi
	addl	$-32, %eax
	cmpl	$31, %eax
	ja	.LBB0_1
# %bb.2:
	vpsrlw	$8, %ymm0, %ymm1
	vpand	.LCPI0_2(%rip), %ymm0, %ymm0
	vpaddw	%ymm0, %ymm1, %ymm0
	vphaddw	%ymm0, %ymm0, %ymm0
	vphaddw	%ymm0, %ymm0, %ymm0
	vphaddw	%ymm0, %ymm0, %ymm0
	vmovd	%xmm0, %eax
	vextracti128	$1, %ymm0, %xmm0
	vmovd	%xmm0, %ecx
	addl	%eax, %ecx
	leaq	6(%rsp), %rsi
	movw	%cx, (%rsi)
	vzeroupper
	callq	CRYPTO_NAMESPACE(crypto_encode_int16)@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_core_weightsntrup1013), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_core_weightsntrup1013)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
