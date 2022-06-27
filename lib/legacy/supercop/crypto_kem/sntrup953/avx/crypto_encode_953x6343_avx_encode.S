	.file	"encode.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(crypto_encode_953x6343)
	.type	CRYPTO_NAMESPACE(crypto_encode_953x6343), @function
CRYPTO_NAMESPACE(crypto_encode_953x6343):
.LFB5347:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r9
	movq	%rsi, %rdx
	movl	$29, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$840, %rsp
	.cfi_offset 3, -24
	leaq	-120(%rsp), %r8
	movq	%r8, %rsi
	vmovdqa	.LC0(%rip), %ymm6
	vmovdqa	.LC1(%rip), %ymm5
	vmovdqa	.LC2(%rip), %ymm4
	vmovdqa	.LC3(%rip), %ymm3
	vmovdqa	.LC4(%rip), %ymm2
	.p2align 4,,10
	.p2align 3
.L2:
	vpaddw	(%rdx), %ymm6, %ymm8
	vpaddw	32(%rdx), %ymm6, %ymm7
	leaq	32(%rdi), %rax
	vpand	%ymm8, %ymm5, %ymm0
	vpand	%ymm7, %ymm5, %ymm1
	vpand	%ymm8, %ymm4, %ymm8
	vpsrld	$16, %ymm0, %ymm0
	vpsrld	$16, %ymm1, %ymm1
	vpand	%ymm7, %ymm4, %ymm7
	vpmulld	%ymm3, %ymm0, %ymm0
	vpmulld	%ymm3, %ymm1, %ymm1
	vpaddd	%ymm8, %ymm0, %ymm0
	vpaddd	%ymm7, %ymm1, %ymm1
	vpshufb	%ymm2, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpermq	$216, %ymm1, %ymm1
	vperm2i128	$49, %ymm1, %ymm0, %ymm7
	vinserti128	$1, %xmm1, %ymm0, %ymm0
	vmovdqu	%ymm7, (%rsi)
	vmovdqu	%ymm0, (%rdi)
	testq	%rcx, %rcx
	je	.L30
	decq	%rcx
	je	.L3
	addq	$64, %rdx
	addq	$32, %rsi
	movq	%rax, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	addq	$48, %rdx
	addq	$24, %rsi
	addq	$24, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L30:
	movzwl	1904(%r9), %edx
	movq	%r8, %rdi
	movq	%r8, %rsi
	vmovdqa	.LC5(%rip), %ymm3
	vmovdqa	.LC6(%rip), %ymm5
	vmovdqa	.LC7(%rip), %ymm4
	addw	$3171, %dx
	andw	$16383, %dx
	movw	%dx, 832(%rsp)
	movl	$29, %edx
	.p2align 4,,10
	.p2align 3
.L6:
	vmovdqu	(%rsi), %ymm6
	vpand	(%rsi), %ymm3, %ymm1
	leaq	8(%rax), %rcx
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rdi)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rax)
	vpextrd	$2, %xmm0, 4(%rax)
	testq	%rdx, %rdx
	je	.L31
	decq	%rdx
	je	.L7
	addq	$32, %rsi
	addq	$16, %rdi
	movq	%rcx, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L7:
	addq	$24, %rsi
	addq	$12, %rdi
	addq	$6, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L31:
	movzwl	832(%rsp), %eax
	vmovdqa	.LC8(%rip), %ymm5
	movq	%r8, %rsi
	movq	%r8, %rdx
	movw	%ax, 356(%rsp)
	movl	$14, %eax
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rdx), %ymm6
	vpand	(%rdx), %ymm3, %ymm1
	leaq	8(%rcx), %r9
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rsi)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rcx)
	vpextrd	$2, %xmm0, 4(%rcx)
	testq	%rax, %rax
	je	.L32
	decq	%rax
	je	.L11
	addq	$32, %rdx
	addq	$16, %rsi
	movq	%r9, %rcx
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L11:
	addq	$28, %rdx
	addq	$14, %rsi
	addq	$7, %rcx
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L32:
	movzwl	356(%rsp), %eax
	vmovdqa	.LC9(%rip), %ymm6
	movl	$3, %edx
	movq	%r8, %rcx
	movw	%ax, 118(%rsp)
	movq	%r8, %rax
.L14:
	vmovdqu	(%rax), %ymm5
	leaq	32(%r9), %rdi
	vpsrld	$16, %ymm5, %ymm0
	vmovdqu	32(%rax), %ymm5
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm5, %ymm1
	vpand	(%rax), %ymm3, %ymm5
	vpmulld	%ymm6, %ymm1, %ymm1
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	32(%rax), %ymm3, %ymm5
	vpshufb	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm1, %ymm1
	vpermq	$216, %ymm1, %ymm1
	vperm2i128	$49, %ymm1, %ymm0, %ymm5
	vinserti128	$1, %xmm1, %ymm0, %ymm0
	vmovdqu	%ymm5, (%rcx)
	vmovdqu	%ymm0, (%r9)
	testq	%rdx, %rdx
	je	.L33
	decq	%rdx
	je	.L15
	addq	$64, %rax
	addq	$32, %rcx
	movq	%rdi, %r9
	jmp	.L14
	.p2align 4,,10
	.p2align 3
.L15:
	addq	$48, %rax
	addq	$24, %rcx
	addq	$24, %r9
	jmp	.L14
	.p2align 4,,10
	.p2align 3
.L33:
	vmovdqa	.LC10(%rip), %ymm2
	movl	$3, %esi
	movq	%r8, %rdx
	movq	%r8, %rax
.L18:
	vmovdqu	(%rax), %ymm5
	vpand	(%rax), %ymm3, %ymm1
	leaq	8(%rdi), %rcx
	vpsrld	$16, %ymm5, %ymm0
	vpmulld	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rdx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rdi)
	vpextrd	$2, %xmm0, 4(%rdi)
	testq	%rsi, %rsi
	je	.L34
	decq	%rsi
	je	.L19
	addq	$32, %rax
	addq	$16, %rdx
	movq	%rcx, %rdi
	jmp	.L18
	.p2align 4,,10
	.p2align 3
.L19:
	addq	$24, %rax
	addq	$12, %rdx
	addq	$6, %rdi
	jmp	.L18
	.p2align 4,,10
	.p2align 3
.L34:
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L22:
	movzwl	2(%r8,%rdx,4), %eax
	movzwl	(%r8,%rdx,4), %r9d
	imull	$4701, %eax, %eax
	addl	%r9d, %eax
	movb	%al, (%rcx,%rdx,2)
	movb	%ah, 1(%rcx,%rdx,2)
	shrl	$16, %eax
	movw	%ax, (%r8,%rdx,2)
	incq	%rdx
	cmpq	$15, %rdx
	jne	.L22
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L23:
	movzwl	2(%r8,%rdx,4), %eax
	movzwl	(%r8,%rdx,4), %r9d
	imull	$338, %eax, %eax
	addl	%r9d, %eax
	movb	%al, 30(%rcx,%rdx)
	shrl	$8, %eax
	movw	%ax, (%r8,%rdx,2)
	incq	%rdx
	cmpq	$7, %rdx
	jne	.L23
	movzwl	-92(%rsp), %eax
	movw	%ax, -106(%rsp)
.L24:
	movzwl	2(%r8,%rsi,4), %eax
	movzwl	(%r8,%rsi,4), %edx
	imull	$447, %eax, %eax
	addl	%edx, %eax
	movb	%al, 37(%rcx,%rsi)
	shrl	$8, %eax
	movw	%ax, (%r8,%rsi,2)
	incq	%rsi
	cmpq	$4, %rsi
	jne	.L24
	movzwl	-118(%rsp), %eax
	movzwl	-120(%rsp), %edx
	movzwl	-116(%rsp), %ecx
	imull	$781, %eax, %eax
	addl	%edx, %eax
	movzwl	-114(%rsp), %edx
	movb	%al, 49(%rdi)
	shrl	$8, %eax
	imull	$781, %edx, %edx
	addl	%ecx, %edx
	movzwl	%ax, %ecx
	movb	%dl, 50(%rdi)
	shrl	$8, %edx
	movzwl	%dx, %eax
	imull	$2383, %eax, %eax
	addl	%ecx, %eax
	movw	%ax, 51(%rdi)
	shrl	$8, %eax
	movb	%ah, 53(%rdi)
	vzeroupper
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5347:
	.size	CRYPTO_NAMESPACE(crypto_encode_953x6343), .-CRYPTO_NAMESPACE(crypto_encode_953x6343)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.value	3171
	.align 32
.LC1:
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.align 32
.LC2:
	.quad	70364449226751
	.quad	70364449226751
	.quad	70364449226751
	.quad	70364449226751
	.align 32
.LC3:
	.long	6343
	.long	6343
	.long	6343
	.long	6343
	.long	6343
	.long	6343
	.long	6343
	.long	6343
	.align 32
.LC4:
	.byte	0
	.byte	1
	.byte	4
	.byte	5
	.byte	8
	.byte	9
	.byte	12
	.byte	13
	.byte	2
	.byte	3
	.byte	6
	.byte	7
	.byte	10
	.byte	11
	.byte	14
	.byte	15
	.byte	0
	.byte	1
	.byte	4
	.byte	5
	.byte	8
	.byte	9
	.byte	12
	.byte	13
	.byte	2
	.byte	3
	.byte	6
	.byte	7
	.byte	10
	.byte	11
	.byte	14
	.byte	15
	.align 32
.LC5:
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.align 32
.LC6:
	.long	614
	.long	614
	.long	614
	.long	614
	.long	614
	.long	614
	.long	614
	.long	614
	.align 32
.LC7:
	.byte	1
	.byte	2
	.byte	5
	.byte	6
	.byte	9
	.byte	10
	.byte	13
	.byte	14
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	1
	.byte	2
	.byte	5
	.byte	6
	.byte	9
	.byte	10
	.byte	13
	.byte	14
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.align 32
.LC8:
	.long	1473
	.long	1473
	.long	1473
	.long	1473
	.long	1473
	.long	1473
	.long	1473
	.long	1473
	.align 32
.LC9:
	.long	8476
	.long	8476
	.long	8476
	.long	8476
	.long	8476
	.long	8476
	.long	8476
	.long	8476
	.align 32
.LC10:
	.long	1097
	.long	1097
	.long	1097
	.long	1097
	.long	1097
	.long	1097
	.long	1097
	.long	1097
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
