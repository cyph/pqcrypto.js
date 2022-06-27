	.file	"encode.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(crypto_encode_857x5167)
	.type	CRYPTO_NAMESPACE(crypto_encode_857x5167), @function
CRYPTO_NAMESPACE(crypto_encode_857x5167):
.LFB5347:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r9
	movq	%rsi, %rdx
	movl	$26, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$744, %rsp
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
	je	.L28
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
.L28:
	movzwl	1712(%r9), %edx
	vmovdqa	.LC5(%rip), %ymm3
	movq	%r8, %rdi
	movq	%r8, %rsi
	vmovdqa	.LC6(%rip), %ymm4
	addw	$2583, %dx
	andw	$16383, %dx
	movw	%dx, 736(%rsp)
	movl	$26, %edx
	.p2align 4,,10
	.p2align 3
.L6:
	vmovdqu	(%rsi), %ymm6
	vpand	(%rsi), %ymm3, %ymm5
	leaq	8(%rax), %rcx
	vpsrld	$16, %ymm6, %ymm0
	vpslld	$1, %ymm0, %ymm1
	vpaddd	%ymm0, %ymm1, %ymm1
	vpslld	$4, %ymm1, %ymm0
	vpaddd	%ymm0, %ymm1, %ymm0
	vpslld	$3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rdi)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rax)
	vpextrd	$2, %xmm0, 4(%rax)
	testq	%rdx, %rdx
	je	.L29
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
.L29:
	movzwl	736(%rsp), %eax
	vmovdqa	.LC7(%rip), %ymm5
	movq	%r8, %rdi
	movq	%r8, %rsi
	movw	%ax, 308(%rsp)
	movl	$13, %eax
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rsi), %ymm6
	vpand	(%rsi), %ymm3, %ymm1
	leaq	8(%rcx), %rdx
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rdi)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rcx)
	vpextrd	$2, %xmm0, 4(%rcx)
	testq	%rax, %rax
	je	.L30
	decq	%rax
	je	.L11
	addq	$32, %rsi
	addq	$16, %rdi
	movq	%rdx, %rcx
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L11:
	addq	$12, %rsi
	addq	$6, %rdi
	addq	$3, %rcx
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L30:
	movzwl	308(%rsp), %eax
	vmovdqa	.LC8(%rip), %ymm5
	movl	$6, %ecx
	movq	%r8, %rsi
	movw	%ax, 94(%rsp)
	movq	%r8, %rax
	.p2align 4,,10
	.p2align 3
.L14:
	vmovdqu	(%rax), %ymm6
	vpand	(%rax), %ymm3, %ymm1
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rsi)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rdx)
	vpextrd	$2, %xmm0, 4(%rdx)
	testq	%rcx, %rcx
	je	.L31
	decq	%rcx
	je	.L15
	addq	$32, %rax
	addq	$16, %rsi
	addq	$8, %rdx
	jmp	.L14
	.p2align 4,,10
	.p2align 3
.L15:
	addq	$20, %rax
	addq	$10, %rsi
	addq	$5, %rdx
	jmp	.L14
	.p2align 4,,10
	.p2align 3
.L31:
	movzwl	94(%rsp), %eax
	movzwl	92(%rsp), %esi
	vmovdqa	.LC9(%rip), %ymm6
	imull	$1656, %eax, %eax
	addl	%esi, %eax
	leaq	-130(%rsp), %rsi
	movw	%ax, 8(%rdx)
	shrl	$16, %eax
	movw	%ax, -14(%rsp)
	movl	$10, %eax
.L18:
	vmovdqu	-20(%r8,%rax,2), %ymm5
	leaq	(%rdx,%rax), %rdi
	vpsrld	$16, %ymm5, %ymm0
	vmovdqu	12(%r8,%rax,2), %ymm5
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm5, %ymm1
	vpand	-20(%r8,%rax,2), %ymm3, %ymm5
	vpmulld	%ymm6, %ymm1, %ymm1
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	12(%r8,%rax,2), %ymm3, %ymm5
	vpshufb	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm1, %ymm1
	vpermq	$216, %ymm1, %ymm1
	vperm2i128	$49, %ymm1, %ymm0, %ymm5
	vinserti128	$1, %xmm1, %ymm0, %ymm0
	vmovdqu	%ymm5, (%rsi,%rax)
	vmovdqu	%ymm0, (%rdx,%rax)
	addq	$20, %rax
	cmpq	$50, %rax
	jne	.L18
	movzwl	-14(%rsp), %eax
	movzwl	-16(%rsp), %edx
	vmovdqa	.LC10(%rip), %ymm2
	imull	$10713, %eax, %eax
	addl	%edx, %eax
	movb	%al, 32(%rdi)
	shrl	$8, %eax
	movw	%ax, -68(%rsp)
	movl	$33, %eax
.L19:
	vmovdqu	-132(%r8,%rax,4), %ymm5
	leaq	(%rdi,%rax), %rsi
	vpand	-132(%r8,%rax,4), %ymm3, %ymm1
	vpsrld	$16, %ymm5, %ymm0
	vpmulld	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -66(%r8,%rax,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rdi,%rax)
	vpextrd	$2, %xmm0, 4(%rdi,%rax)
	addq	$5, %rax
	cmpq	$43, %rax
	jne	.L19
	movzwl	-68(%rsp), %eax
	xorl	%edx, %edx
	movw	%ax, -94(%rsp)
	.p2align 4,,10
	.p2align 3
.L20:
	movzwl	2(%r8,%rdx,4), %eax
	movzwl	(%r8,%rdx,4), %edi
	imull	$11991, %eax, %eax
	addl	%edi, %eax
	movb	%al, 8(%rsi,%rdx,2)
	movb	%ah, 9(%rsi,%rdx,2)
	shrl	$16, %eax
	movw	%ax, (%r8,%rdx,2)
	incq	%rdx
	cmpq	$7, %rdx
	jne	.L20
.L21:
	movzwl	2(%r8,%rcx,4), %eax
	movzwl	(%r8,%rcx,4), %edx
	imull	$2194, %eax, %eax
	addl	%edx, %eax
	movb	%al, 22(%rsi,%rcx,2)
	movb	%ah, 23(%rsi,%rcx,2)
	shrl	$16, %eax
	movw	%ax, (%r8,%rcx,2)
	incq	%rcx
	cmpq	$3, %rcx
	jne	.L21
	movzwl	-118(%rsp), %edx
	movzwl	-120(%rsp), %eax
	movzwl	-116(%rsp), %ecx
	imull	$74, %edx, %edx
	addl	%eax, %edx
	movzwl	-108(%rsp), %eax
	movzwl	%dx, %edx
	imull	$74, %eax, %eax
	addl	%ecx, %eax
	movb	%al, 28(%rsi)
	shrl	$8, %eax
	imull	$5476, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 29(%rsi)
	shrl	$8, %eax
	movb	%ah, 31(%rsi)
	vzeroupper
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5347:
	.size	CRYPTO_NAMESPACE(crypto_encode_857x5167), .-CRYPTO_NAMESPACE(crypto_encode_857x5167)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
	.value	2583
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
	.long	5167
	.long	5167
	.long	5167
	.long	5167
	.long	5167
	.long	5167
	.long	5167
	.long	5167
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
.LC7:
	.long	651
	.long	651
	.long	651
	.long	651
	.long	651
	.long	651
	.long	651
	.long	651
	.align 32
.LC8:
	.long	1656
	.long	1656
	.long	1656
	.long	1656
	.long	1656
	.long	1656
	.long	1656
	.long	1656
	.align 32
.LC9:
	.long	10713
	.long	10713
	.long	10713
	.long	10713
	.long	10713
	.long	10713
	.long	10713
	.long	10713
	.align 32
.LC10:
	.long	1752
	.long	1752
	.long	1752
	.long	1752
	.long	1752
	.long	1752
	.long	1752
	.long	1752
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
