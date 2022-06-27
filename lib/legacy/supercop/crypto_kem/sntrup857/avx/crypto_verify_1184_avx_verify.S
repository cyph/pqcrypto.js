	.file	"verify.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_verify_1184)
	.type	CRYPTO_NAMESPACE(crypto_verify_1184), @function
CRYPTO_NAMESPACE(crypto_verify_1184):
.LFB5342:
	.cfi_startproc
	movl	$1152, %eax
	vpxor	%xmm0, %xmm0, %xmm0
.L2:
	vmovdqu	(%rdi), %ymm2
	vpxor	(%rsi), %ymm2, %ymm1
	addq	$32, %rdi
	addq	$32, %rsi
	vpor	%ymm1, %ymm0, %ymm0
	subl	$32, %eax
	jns	.L2
	cmpl	$-31, %eax
	jl	.L4
	movslq	%eax, %rdx
	addq	%rdx, %rdi
	addq	%rdx, %rsi
	jmp	.L2
.L4:
	vpsrlw	$8, %ymm0, %ymm1
	vpor	%ymm1, %ymm0, %ymm0
	vpsrld	$16, %ymm0, %ymm1
	vpor	%ymm1, %ymm0, %ymm0
	vpsrlq	$32, %ymm0, %ymm1
	vpor	%ymm1, %ymm0, %ymm0
	vpextrb	$0, %xmm0, %esi
	vpextrb	$8, %xmm0, %eax
	vextracti128	$0x1, %ymm0, %xmm0
	vpextrb	$0, %xmm0, %ecx
	orl	%esi, %eax
	vpextrb	$8, %xmm0, %edx
	orl	%ecx, %eax
	orl	%edx, %eax
	movzbl	%al, %eax
	decl	%eax
	shrl	$31, %eax
	decl	%eax
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_verify_1184), .-CRYPTO_NAMESPACE(crypto_verify_1184)
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
