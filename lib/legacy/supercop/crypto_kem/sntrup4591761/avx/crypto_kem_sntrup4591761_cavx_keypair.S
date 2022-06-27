	.file	"keypair.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(keypair)
	.type	CRYPTO_NAMESPACE(keypair), @function
CRYPTO_NAMESPACE(keypair):
.LFB10:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%rdi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rsi, %rbx
	subq	$5400, %rsp
	.cfi_def_cfa_offset 5456
	leaq	16(%rsp), %r12
	leaq	784(%rsp), %rbp
	.p2align 4,,10
	.p2align 3
.L2:
	movq	%r12, %rdi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(small_random)@PLT
	xorl	%eax, %eax
	movq	%r12, %rsi
	movq	%rbp, %rdi
	call	CRYPTO_NAMESPACE(r3_recip)@PLT
	testl	%eax, %eax
	jne	.L2
	leaq	1552(%rsp), %r14
	leaq	3856(%rsp), %r15
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(small_random_weightw)@PLT
	leaq	2320(%rsp), %r8
	movq	%r14, %rsi
	xorl	%eax, %eax
	movq	%r8, %rdi
	movq	%r8, 8(%rsp)
	call	CRYPTO_NAMESPACE(rq_recip3)@PLT
	movq	8(%rsp), %r8
	movq	%r12, %rdx
	movq	%r15, %rdi
	xorl	%eax, %eax
	movq	%r8, %rsi
	call	CRYPTO_NAMESPACE(rq_mult)@PLT
	movq	%r15, %rsi
	movq	%r13, %rdi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(rq_encode)@PLT
	movq	%r14, %rsi
	movq	%rbx, %rdi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(small_encode)@PLT
	leaq	191(%rbx), %rdi
	movq	%rbp, %rsi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(small_encode)@PLT
	leaq	382(%rbx), %rdi
	movq	%r13, %rsi
	movl	$1218, %edx
	call	memcpy@PLT
	addq	$5400, %rsp
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE10:
	.size	CRYPTO_NAMESPACE(keypair), .-CRYPTO_NAMESPACE(keypair)
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
