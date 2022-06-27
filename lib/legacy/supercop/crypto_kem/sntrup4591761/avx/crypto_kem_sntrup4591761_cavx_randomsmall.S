	.file	"randomsmall.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(small_random)
	.type	CRYPTO_NAMESPACE(small_random), @function
CRYPTO_NAMESPACE(small_random):
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	$3044, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$3072, %rsp
	movq	%rsp, %r12
	movq	%r12, %rdi
	call	randombytes@PLT
	movq	%r12, %rax
	movq	%rbx, %rdx
	vpcmpeqd	%ymm5, %ymm5, %ymm5
	vmovdqa	.LC0(%rip), %ymm3
	vmovdqa	.LC1(%rip), %ymm2
	leaq	2944(%rsp), %rcx
	vmovdqa	.LC2(%rip), %ymm4
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	(%rax), %ymm3, %ymm0
	vpand	32(%rax), %ymm3, %ymm6
	subq	$-128, %rax
	addq	$32, %rdx
	vpand	-32(%rax), %ymm3, %ymm7
	vpslld	$1, %ymm0, %ymm1
	vpaddd	%ymm0, %ymm1, %ymm1
	vpslld	$1, %ymm6, %ymm0
	vpaddd	%ymm6, %ymm0, %ymm0
	vpsrld	$30, %ymm1, %ymm1
	vpand	-64(%rax), %ymm3, %ymm6
	vpsrld	$30, %ymm0, %ymm0
	vpand	%ymm1, %ymm2, %ymm1
	vpand	%ymm0, %ymm2, %ymm0
	vpackusdw	%ymm0, %ymm1, %ymm1
	vpslld	$1, %ymm6, %ymm0
	vpaddd	%ymm6, %ymm0, %ymm0
	vpslld	$1, %ymm7, %ymm6
	vpermq	$216, %ymm1, %ymm1
	vpaddd	%ymm7, %ymm6, %ymm6
	vpsrld	$30, %ymm0, %ymm0
	vpand	%ymm1, %ymm4, %ymm1
	vpsrld	$30, %ymm6, %ymm6
	vpand	%ymm0, %ymm2, %ymm0
	vpand	%ymm6, %ymm2, %ymm6
	vpackusdw	%ymm6, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm4, %ymm0
	vpackuswb	%ymm0, %ymm1, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm5, %ymm0
	vmovdqu	%ymm0, -32(%rdx)
	cmpq	%rcx, %rax
	jne	.L2
	movl	$736, %edx
	.p2align 4,,10
	.p2align 3
.L3:
	movl	(%r12,%rdx,4), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, (%rbx,%rdx)
	incq	%rdx
	cmpq	$761, %rdx
	jne	.L3
	xorl	%eax, %eax
	movb	$0, 767(%rbx)
	movl	$0, 761(%rbx)
	movw	%ax, 765(%rbx)
	vzeroupper
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	CRYPTO_NAMESPACE(small_random), .-CRYPTO_NAMESPACE(small_random)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.align 32
.LC1:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC2:
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
