	.file	"rq_mod3.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(rq_mod3)
	.type	CRYPTO_NAMESPACE(rq_mod3), @function
CRYPTO_NAMESPACE(rq_mod3):
.LFB5299:
	.cfi_startproc
	vmovdqa	.LC0(%rip), %ymm6
	vmovdqa	.LC1(%rip), %ymm2
	xorl	%eax, %eax
	vmovdqa	.LC2(%rip), %ymm5
	vmovdqa	.LC3(%rip), %ymm4
	vmovdqa	.LC4(%rip), %ymm3
	.p2align 4,,10
	.p2align 3
.L2:
	vmovdqu	(%rsi,%rax,2), %ymm7
	vpsllw	$1, %ymm7, %ymm1
	vpaddw	%ymm7, %ymm1, %ymm1
	vpmulhrsw	%ymm6, %ymm1, %ymm0
	vpmullw	%ymm2, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm1
	vpsraw	$15, %ymm1, %ymm7
	vpaddw	%ymm2, %ymm1, %ymm0
	vpblendvb	%ymm7, %ymm0, %ymm1, %ymm0
	vpaddw	%ymm0, %ymm5, %ymm7
	vpaddw	%ymm0, %ymm4, %ymm1
	vpsraw	$15, %ymm7, %ymm7
	vpblendvb	%ymm7, %ymm0, %ymm1, %ymm1
	vpmulhrsw	%ymm3, %ymm1, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm1
	vpaddw	%ymm0, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm0
	vpacksswb	%ymm0, %ymm0, %ymm0
	vmovdqa	%xmm0, %xmm1
	vextracti128	$0x1, %ymm0, %xmm0
	vpunpcklqdq	%xmm0, %xmm1, %xmm0
	vmovups	%xmm0, (%rdi,%rax)
	addq	$16, %rax
	cmpq	$768, %rax
	jne	.L2
	vzeroupper
	ret
	.cfi_endproc
.LFE5299:
	.size	CRYPTO_NAMESPACE(rq_mod3), .-CRYPTO_NAMESPACE(rq_mod3)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.align 32
.LC1:
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.align 32
.LC2:
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.align 32
.LC3:
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.align 32
.LC4:
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
