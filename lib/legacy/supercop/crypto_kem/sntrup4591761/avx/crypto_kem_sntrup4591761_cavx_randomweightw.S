	.file	"randomweightw.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(small_random_weightw)
	.type	CRYPTO_NAMESPACE(small_random_weightw), @function
CRYPTO_NAMESPACE(small_random_weightw):
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	$3044, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$3072, %rsp
	.cfi_offset 3, -40
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	movq	%rbx, %r13
	call	randombytes@PLT
	vmovdqa	.LC0(%rip), %ymm1
	leaq	1120(%rsp), %rdx
	movq	%rbx, %rax
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	(%rax), %ymm1, %ymm0
	addq	$32, %rax
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rdx, %rax
	jne	.L2
	vmovdqa	.LC1(%rip), %ymm2
	vmovdqa	.LC2(%rip), %ymm1
	movabsq	$-4294967298, %rax
	leaq	3032(%rbx), %rdx
	andq	%rax, 1120(%rsp)
	andq	%rax, 1128(%rsp)
	andq	%rax, 1136(%rsp)
	leaq	1144(%rsp), %rax
	.p2align 4,,10
	.p2align 3
.L3:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rax
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L3
	movl	3032(%rsp), %eax
	movl	$761, %esi
	movq	%rbx, %rdi
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 3032(%rsp)
	movl	3036(%rsp), %eax
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 3036(%rsp)
	movl	3040(%rsp), %eax
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 3040(%rsp)
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_sort_int32)@PLT
	vmovdqa	.LC3(%rip), %ymm2
	movq	%r12, %rax
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vmovdqa	.LC4(%rip), %ymm3
	vmovdqa	.LC5(%rip), %ymm5
	leaq	2944(%rbx), %rdx
	.p2align 4,,10
	.p2align 3
.L4:
	vpand	32(%r13), %ymm2, %ymm1
	vpand	0(%r13), %ymm2, %ymm0
	subq	$-128, %r13
	addq	$32, %rax
	vpand	-32(%r13), %ymm2, %ymm6
	vpackusdw	%ymm1, %ymm0, %ymm0
	vpand	-64(%r13), %ymm2, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpackusdw	%ymm6, %ymm1, %ymm1
	vpand	%ymm0, %ymm3, %ymm0
	vpermq	$216, %ymm1, %ymm1
	vpand	%ymm1, %ymm3, %ymm1
	vpackuswb	%ymm1, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm5, %ymm0
	vpaddb	%ymm4, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rdx, %r13
	jne	.L4
	movl	$736, %edx
	.p2align 4,,10
	.p2align 3
.L5:
	movl	(%rbx,%rdx,4), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, (%r12,%rdx)
	incq	%rdx
	cmpq	$761, %rdx
	jne	.L5
	movl	$0, 761(%r12)
	xorl	%eax, %eax
	movw	%ax, 765(%r12)
	movb	$0, 767(%r12)
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	CRYPTO_NAMESPACE(small_random_weightw), .-CRYPTO_NAMESPACE(small_random_weightw)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.align 32
.LC1:
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.align 32
.LC2:
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.align 32
.LC3:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC4:
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
	.align 32
.LC5:
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
