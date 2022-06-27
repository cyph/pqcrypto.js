	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_1277x2627)
	.type	CRYPTO_NAMESPACE(crypto_encode_1277x2627), @function
CRYPTO_NAMESPACE(crypto_encode_1277x2627):
.LFB5342:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r9
	movq	%rsi, %r8
	movl	$40, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$1160, %rsp
	.cfi_offset 3, -24
	leaq	-118(%rsp), %rdx
	movq	%rdx, %rax
	vmovdqa	.LC0(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm6
	vmovdqa	.LC2(%rip), %ymm5
	vmovdqa	.LC3(%rip), %ymm1
	vmovdqa	.LC4(%rip), %ymm4
	vmovdqa	.LC5(%rip), %ymm2
.L3:
	movq	%rdi, %rsi
	decq	%rcx
	jne	.L2
	subq	$8, %r8
	subq	$4, %rdx
	subq	$4, %rsi
.L2:
	vpaddw	(%r8), %ymm7, %ymm9
	addq	$32, %rdx
	leaq	32(%rsi), %rdi
	addq	$64, %r8
	vpaddw	-32(%r8), %ymm7, %ymm8
	vpand	%ymm6, %ymm9, %ymm9
	vpand	%ymm6, %ymm8, %ymm8
	vpmulhw	%ymm5, %ymm9, %ymm9
	vpmulhw	%ymm5, %ymm8, %ymm8
	vpsrld	$16, %ymm9, %ymm0
	vpand	%ymm1, %ymm9, %ymm9
	vpsrld	$16, %ymm8, %ymm3
	vpmulld	%ymm4, %ymm0, %ymm0
	vpand	%ymm1, %ymm8, %ymm8
	vpmulld	%ymm4, %ymm3, %ymm3
	vpaddd	%ymm9, %ymm0, %ymm0
	vpaddd	%ymm8, %ymm3, %ymm3
	vpshufb	%ymm2, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm3, %ymm3
	vpermq	$216, %ymm0, %ymm0
	vpermq	$216, %ymm3, %ymm3
	vperm2i128	$49, %ymm3, %ymm0, %ymm8
	vinserti128	$1, %xmm3, %ymm0, %ymm0
	vmovdqu	%ymm8, -32(%rdx)
	vmovdqu	%ymm0, (%rsi)
	testq	%rcx, %rcx
	jne	.L3
	movswl	2552(%r9), %edx
	addl	$3939, %edx
	andl	$16383, %edx
	imull	$10923, %edx, %edx
	sarl	$15, %edx
	movw	%dx, 1158(%rsp)
.L4:
	movzwl	2(%rax,%rcx,4), %r8d
	movzwl	(%rax,%rcx,4), %r9d
	imull	$106, %r8d, %r8d
	addl	%r9d, %r8d
	movw	%r8w, (%rax,%rcx,2)
	incq	%rcx
	cmpq	$319, %rcx
	jne	.L4
	vmovdqa	.LC6(%rip), %ymm5
	movw	%dx, 520(%rsp)
	xorl	%edx, %edx
.L5:
	vmovdqu	(%rax,%rdx,2), %ymm6
	vmovdqu	32(%rax,%rdx,2), %ymm7
	vpand	(%rax,%rdx,2), %ymm1, %ymm4
	vpsrld	$16, %ymm6, %ymm0
	vpsrld	$16, %ymm7, %ymm3
	vpmulld	%ymm5, %ymm0, %ymm0
	vpmulld	%ymm5, %ymm3, %ymm3
	vpaddd	%ymm4, %ymm0, %ymm0
	vpand	32(%rax,%rdx,2), %ymm1, %ymm4
	vpshufb	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm3, %ymm3
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm3, %ymm3
	vpermq	$216, %ymm3, %ymm3
	vperm2i128	$49, %ymm3, %ymm0, %ymm4
	vinserti128	$1, %xmm3, %ymm0, %ymm0
	vmovdqu	%ymm4, (%rax,%rdx)
	vmovdqu	%ymm0, (%rdi,%rdx)
	addq	$32, %rdx
	cmpq	$320, %rdx
	jne	.L5
	vmovdqa	.LC7(%rip), %ymm5
	vmovdqa	.LC8(%rip), %ymm4
	xorl	%edx, %edx
.L6:
	vmovdqu	(%rax,%rdx,4), %ymm6
	vpand	(%rax,%rdx,4), %ymm1, %ymm3
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rax,%rdx,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, 320(%rdi,%rdx)
	vpextrd	$2, %xmm0, 324(%rdi,%rdx)
	addq	$8, %rdx
	cmpq	$80, %rdx
	jne	.L6
	vmovdqa	.LC9(%rip), %ymm5
	addq	$432, %rsi
	movq	%rax, %r8
	movq	%rax, %rdx
	movl	$3, %edi
.L8:
	movq	%rsi, %rcx
	decq	%rdi
	jne	.L7
	subq	$32, %rdx
	subq	$16, %r8
	subq	$16, %rcx
.L7:
	vmovdqu	(%rdx), %ymm7
	vpand	(%rdx), %ymm1, %ymm4
	addq	$32, %r8
	addq	$64, %rdx
	vmovdqu	-32(%rdx), %ymm6
	leaq	32(%rcx), %rsi
	vpsrld	$16, %ymm7, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpsrld	$16, %ymm6, %ymm3
	vpmulld	%ymm5, %ymm3, %ymm3
	vpaddd	%ymm4, %ymm0, %ymm0
	vpand	-32(%rdx), %ymm1, %ymm4
	vpshufb	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm3, %ymm3
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm3, %ymm3
	vpermq	$216, %ymm3, %ymm3
	vperm2i128	$49, %ymm3, %ymm0, %ymm4
	vinserti128	$1, %xmm3, %ymm0, %ymm0
	vmovdqu	%ymm4, -32(%r8)
	vmovdqu	%ymm0, (%rcx)
	testq	%rdi, %rdi
	jne	.L8
	xorl	%r8d, %r8d
.L9:
	movzwl	2(%rax,%r8,4), %edx
	movzwl	(%rax,%r8,4), %r9d
	imull	$3211, %edx, %edx
	addl	%r9d, %edx
	movb	%dl, (%rsi,%r8,2)
	movzbl	%dh, %ebx
	shrl	$16, %edx
	movb	%bl, 1(%rsi,%r8,2)
	movw	%dx, (%rax,%r8,2)
	incq	%r8
	cmpq	$19, %r8
	jne	.L9
	movzwl	-40(%rsp), %edx
	movzwl	-42(%rsp), %r8d
	imull	$3211, %edx, %edx
	addl	%r8d, %edx
	xorl	%r8d, %r8d
	movb	%dl, 70(%rcx)
	shrl	$8, %edx
	movw	%dx, -80(%rsp)
.L10:
	movzwl	2(%rax,%r8,4), %edx
	movzwl	(%rax,%r8,4), %r9d
	imull	$158, %edx, %edx
	addl	%r9d, %edx
	movb	%dl, 39(%rsi,%r8)
	shrl	$8, %edx
	movw	%dx, (%rax,%r8,2)
	incq	%r8
	cmpq	$10, %r8
	jne	.L10
.L11:
	movzwl	2(%rax,%rdi,4), %edx
	movzwl	(%rax,%rdi,4), %esi
	imull	$98, %edx, %edx
	addl	%esi, %edx
	movw	%dx, (%rax,%rdi,2)
	incq	%rdi
	cmpq	$4, %rdi
	jne	.L11
	movzwl	-100(%rsp), %edx
	movzwl	-102(%rsp), %eax
	movzwl	-118(%rsp), %esi
	movzwl	-114(%rsp), %edi
	imull	$98, %edx, %edx
	addl	%eax, %edx
	movzwl	-116(%rsp), %eax
	movb	%dl, 81(%rcx)
	shrl	$8, %edx
	imull	$9604, %eax, %eax
	imull	$7744, %edx, %edx
	addl	%esi, %eax
	movw	%ax, 82(%rcx)
	shrl	$16, %eax
	movl	%eax, %esi
	movzwl	-112(%rsp), %eax
	imull	$9604, %eax, %eax
	addl	%edi, %eax
	movw	%ax, 84(%rcx)
	shrl	$16, %eax
	imull	$1408, %eax, %eax
	addl	%esi, %eax
	movb	%al, 86(%rcx)
	shrl	$8, %eax
	addl	%edx, %eax
	movl	%eax, 87(%rcx)
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_1277x2627), .-CRYPTO_NAMESPACE(crypto_encode_1277x2627)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.value	3939
	.align 32
.LC1:
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.align 32
.LC2:
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.align 32
.LC3:
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.align 32
.LC4:
	.long	2627
	.long	2627
	.long	2627
	.long	2627
	.long	2627
	.long	2627
	.long	2627
	.long	2627
	.align 32
.LC5:
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
.LC6:
	.long	11236
	.long	11236
	.long	11236
	.long	11236
	.long	11236
	.long	11236
	.long	11236
	.long	11236
	.align 32
.LC7:
	.long	1927
	.long	1927
	.long	1927
	.long	1927
	.long	1927
	.long	1927
	.long	1927
	.long	1927
	.align 32
.LC8:
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
.LC9:
	.long	14506
	.long	14506
	.long	14506
	.long	14506
	.long	14506
	.long	14506
	.long	14506
	.long	14506
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
