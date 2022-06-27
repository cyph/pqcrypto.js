	.file	"encode.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(crypto_encode_1013x7177)
	.type	CRYPTO_NAMESPACE(crypto_encode_1013x7177), @function
CRYPTO_NAMESPACE(crypto_encode_1013x7177):
.LFB5347:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r9
	movq	%rsi, %rdx
	movl	$31, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$904, %rsp
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
	je	.L26
	decq	%rcx
	je	.L3
	addq	$64, %rdx
	addq	$32, %rsi
	movq	%rax, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	addq	$40, %rdx
	addq	$20, %rsi
	addq	$20, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L26:
	movzwl	2024(%r9), %edx
	movq	%r8, %rsi
	movq	%r8, %rcx
	vmovdqa	.LC5(%rip), %ymm3
	vmovdqa	.LC6(%rip), %ymm5
	vmovdqa	.LC7(%rip), %ymm4
	addw	$3588, %dx
	andw	$16383, %dx
	movw	%dx, 892(%rsp)
	movl	$31, %edx
	.p2align 4,,10
	.p2align 3
.L6:
	vmovdqu	(%rcx), %ymm6
	vpand	(%rcx), %ymm3, %ymm1
	leaq	8(%rax), %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rsi)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rax)
	vpextrd	$2, %xmm0, 4(%rax)
	testq	%rdx, %rdx
	je	.L27
	decq	%rdx
	je	.L7
	addq	$32, %rcx
	addq	$16, %rsi
	movq	%rdi, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L7:
	addq	$20, %rcx
	addq	$10, %rsi
	addq	$5, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L27:
	movzwl	892(%rsp), %eax
	vmovdqa	.LC8(%rip), %ymm6
	movl	$7, %r9d
	movq	%r8, %rdx
	movw	%ax, 386(%rsp)
	movq	%r8, %rax
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rax), %ymm5
	leaq	32(%rdi), %r10
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
	vmovdqu	%ymm5, (%rdx)
	vmovdqu	%ymm0, (%rdi)
	testq	%r9, %r9
	je	.L28
	decq	%r9
	je	.L11
	addq	$64, %rax
	addq	$32, %rdx
	movq	%r10, %rdi
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L11:
	addq	$60, %rax
	addq	$30, %rdx
	addq	$30, %rdi
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L28:
	movq	%r8, %rcx
	leaq	-118(%rsp), %rdx
	leaq	6(%rsp), %rdi
	.p2align 4,,10
	.p2align 3
.L14:
	movzwl	(%rdx), %eax
	movzwl	-2(%rdx), %esi
	addq	$2, %rcx
	addq	$4, %rdx
	imull	$89, %eax, %eax
	addl	%esi, %eax
	movw	%ax, -2(%rcx)
	cmpq	%rcx, %rdi
	jne	.L14
	movzwl	132(%rsp), %eax
	vmovdqa	.LC9(%rip), %ymm6
	movw	%ax, 6(%rsp)
	xorl	%eax, %eax
.L15:
	vmovdqu	(%r8,%rax,2), %ymm5
	leaq	(%r10,%rax), %rdx
	vpsrld	$16, %ymm5, %ymm0
	vmovdqu	32(%r8,%rax,2), %ymm5
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm5, %ymm1
	vpand	(%r8,%rax,2), %ymm3, %ymm5
	vpmulld	%ymm6, %ymm1, %ymm1
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	32(%r8,%rax,2), %ymm3, %ymm5
	vpshufb	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm1, %ymm1
	vpermq	$216, %ymm1, %ymm1
	vperm2i128	$49, %ymm1, %ymm0, %ymm5
	vinserti128	$1, %xmm1, %ymm0, %ymm0
	vmovdqu	%ymm5, (%r8,%rax)
	vmovdqu	%ymm0, (%r10,%rax)
	addq	$30, %rax
	cmpq	$60, %rax
	jne	.L15
	movzwl	6(%rsp), %eax
	movzwl	4(%rsp), %ecx
	imull	$7921, %eax, %eax
	addl	%ecx, %eax
	movb	%al, 32(%rdx)
	shrl	$8, %eax
	movw	%ax, -58(%rsp)
	movl	$33, %eax
.L16:
	vmovdqu	-132(%r8,%rax,4), %ymm2
	leaq	(%rdx,%rax), %rcx
	vpand	-132(%r8,%rax,4), %ymm3, %ymm1
	vpsrld	$16, %ymm2, %ymm2
	vpslld	$4, %ymm2, %ymm0
	vpsubd	%ymm2, %ymm0, %ymm0
	vpslld	$5, %ymm0, %ymm0
	vpsubd	%ymm2, %ymm0, %ymm0
	vpslld	$1, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -66(%r8,%rax,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rdx,%rax)
	vpextrd	$2, %xmm0, 4(%rdx,%rax)
	addq	$7, %rax
	cmpq	$47, %rax
	jne	.L16
	movzwl	-58(%rsp), %eax
	movzwl	-60(%rsp), %edx
	imull	$958, %eax, %eax
	addl	%edx, %eax
	xorl	%edx, %edx
	movw	%ax, 8(%rcx)
	shrl	$16, %eax
	movl	%eax, %edi
	.p2align 4,,10
	.p2align 3
.L17:
	movzwl	2(%r8,%rdx,4), %eax
	movzwl	(%r8,%rdx,4), %esi
	imull	$3586, %eax, %eax
	addl	%esi, %eax
	movb	%al, 10(%rcx,%rdx,2)
	movb	%ah, 11(%rcx,%rdx,2)
	shrl	$16, %eax
	movw	%ax, (%r8,%rdx,2)
	incq	%rdx
	cmpq	$7, %rdx
	jne	.L17
	imull	$3586, %edi, %eax
	movzwl	-92(%rsp), %edx
	addl	%edx, %eax
	movb	%al, 24(%rcx)
	shrl	$8, %eax
	movw	%ax, -106(%rsp)
.L18:
	movzwl	2(%r8,%r9,4), %eax
	movzwl	(%r8,%r9,4), %edx
	imull	$197, %eax, %eax
	addl	%edx, %eax
	movb	%al, 25(%rcx,%r9)
	shrl	$8, %eax
	movw	%ax, (%r8,%r9,2)
	incq	%r9
	cmpq	$4, %r9
	jne	.L18
	movzwl	-118(%rsp), %eax
	movzwl	-120(%rsp), %edx
	movzwl	-116(%rsp), %esi
	imull	$152, %eax, %eax
	addl	%edx, %eax
	movb	%al, 29(%rcx)
	shrl	$8, %eax
	movl	%eax, %edx
	movzwl	-114(%rsp), %eax
	imull	$152, %eax, %eax
	addl	%esi, %eax
	movb	%al, 30(%rcx)
	shrl	$8, %eax
	imull	$91, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 31(%rcx)
	shrl	$8, %eax
	movb	%ah, 33(%rcx)
	vzeroupper
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5347:
	.size	CRYPTO_NAMESPACE(crypto_encode_1013x7177), .-CRYPTO_NAMESPACE(crypto_encode_1013x7177)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
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
	.long	7177
	.long	7177
	.long	7177
	.long	7177
	.long	7177
	.long	7177
	.long	7177
	.long	7177
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
	.long	786
	.long	786
	.long	786
	.long	786
	.long	786
	.long	786
	.long	786
	.long	786
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
	.long	2414
	.long	2414
	.long	2414
	.long	2414
	.long	2414
	.long	2414
	.long	2414
	.long	2414
	.align 32
.LC9:
	.long	7921
	.long	7921
	.long	7921
	.long	7921
	.long	7921
	.long	7921
	.long	7921
	.long	7921
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
