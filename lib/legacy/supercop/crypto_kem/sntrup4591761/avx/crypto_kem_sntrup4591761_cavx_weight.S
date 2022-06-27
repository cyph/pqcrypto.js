	.file	"weight.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(r3_weightw_mask)
	.type	CRYPTO_NAMESPACE(r3_weightw_mask), @function
CRYPTO_NAMESPACE(r3_weightw_mask):
.LFB5279:
	.cfi_startproc
	vmovdqa	.LC0(%rip), %ymm2
	leaq	768(%rdi), %rax
	vpxor	%xmm1, %xmm1, %xmm1
	.p2align 4,,10
	.p2align 3
.L2:
	vpmovsxbw	(%rdi), %ymm0
	addq	$16, %rdi
	vpand	%ymm0, %ymm2, %ymm0
	vpaddw	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, %ymm1
	cmpq	%rdi, %rax
	jne	.L2
	vphaddw	%ymm0, %ymm0, %ymm0
	vphaddw	%ymm0, %ymm0, %ymm0
	vphaddw	%ymm0, %ymm0, %ymm0
	vmovdqa	%xmm0, %xmm1
	vextracti128	$0x1, %ymm0, %xmm0
	vpextrw	$0, %xmm0, %eax
	vpextrw	$0, %xmm1, %edx
	leal	-286(%rax,%rdx), %eax
	movzwl	%ax, %eax
	negl	%eax
	sarl	$30, %eax
	vzeroupper
	ret
	.cfi_endproc
.LFE5279:
	.size	CRYPTO_NAMESPACE(r3_weightw_mask), .-CRYPTO_NAMESPACE(r3_weightw_mask)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.quad	281479271743489
	.quad	281479271743489
	.quad	281479271743489
	.quad	281479271743489
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
