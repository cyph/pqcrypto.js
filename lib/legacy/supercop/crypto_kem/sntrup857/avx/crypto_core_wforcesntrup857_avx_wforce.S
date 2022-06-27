	.file	"wforce.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(crypto_core_wforcesntrup857)
	.type	CRYPTO_NAMESPACE(crypto_core_wforcesntrup857), @function
CRYPTO_NAMESPACE(crypto_core_wforcesntrup857):
.LFB5347:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	.cfi_offset 3, -40
	movq	%rsi, %rbx
	andq	$-32, %rsp
	subq	$32, %rsp
	leaq	30(%rsp), %r13
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_weightsntrup857)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_int16)@PLT
	movzwl	30(%rsp), %ecx
	movl	$322, %edx
	vmovdqa	.LC0(%rip), %ymm1
	vpxor	(%rbx), %ymm1, %ymm0
	subl	%ecx, %edx
	leal	-322(%rcx), %eax
	movl	$258, %ecx
	orl	%edx, %eax
	leaq	32(%r12), %rdx
	sarw	$15, %ax
	notl	%eax
	vmovd	%eax, %xmm3
	leaq	32(%rbx), %rax
	vpbroadcastw	%xmm3, %ymm3
	vpand	%ymm3, %ymm0, %ymm0
	vmovdqa	%ymm3, %ymm2
	vpxor	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, (%r12)
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	movq	%rdi, %rax
	movq	%rsi, %rdx
.L2:
	vpxor	(%rax), %ymm1, %ymm0
	leaq	32(%rax), %rdi
	leaq	32(%rdx), %rsi
	vpand	%ymm0, %ymm2, %ymm0
	vpxor	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, (%rdx)
	subl	$32, %ecx
	jns	.L3
	cmpl	$-31, %ecx
	jl	.L4
	movslq	%ecx, %rax
	addq	%rax, %rdi
	addq	%rax, %rsi
	jmp	.L3
.L4:
	vpand	32(%rax), %ymm3, %ymm3
	addq	$64, %rdx
	addq	$64, %rax
	movl	$471, %ecx
	vmovdqu	%ymm3, -32(%rdx)
.L7:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rdx
	addq	$32, %rax
	vmovdqu	%ymm0, -32(%rdx)
	subl	$32, %ecx
	jns	.L7
	cmpl	$-31, %ecx
	jl	.L8
	movslq	%ecx, %rsi
	addq	%rsi, %rax
	addq	%rsi, %rdx
	jmp	.L7
.L8:
	xorl	%eax, %eax
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5347:
	.size	CRYPTO_NAMESPACE(crypto_core_wforcesntrup857), .-CRYPTO_NAMESPACE(crypto_core_wforcesntrup857)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
