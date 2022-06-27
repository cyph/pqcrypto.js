	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_857x1723)
	.type	CRYPTO_NAMESPACE(crypto_encode_857x1723), @function
CRYPTO_NAMESPACE(crypto_encode_857x1723):
.LFB5342:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rcx
	movl	$54, %r8d
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	subq	$744, %rsp
	leaq	-114(%rsp), %rdx
	movq	%rdx, %rax
	vmovdqa	.LC0(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm6
	vmovdqa	.LC2(%rip), %ymm5
	vmovdqa	.LC3(%rip), %ymm4
	vmovdqa	.LC4(%rip), %ymm1
	vmovdqa	.LC5(%rip), %ymm2
.L3:
	decq	%r8
	jne	.L2
	subq	$16, %rcx
	subq	$8, %rdx
	subq	$4, %rdi
.L2:
	vpaddw	(%rcx), %ymm7, %ymm3
	addq	$16, %rdx
	addq	$32, %rcx
	addq	$8, %rdi
	vpand	%ymm6, %ymm3, %ymm3
	vpmulhw	%ymm5, %ymm3, %ymm3
	vpsrld	$16, %ymm3, %ymm0
	vpand	%ymm1, %ymm3, %ymm3
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rdx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%r8, %r8
	jne	.L3
	movswl	1712(%rsi), %edx
	vmovdqa	.LC6(%rip), %ymm6
	movq	%rax, %rcx
	movl	$14, %esi
	vmovdqa	.LC7(%rip), %ymm3
	addl	$2583, %edx
	andl	$16383, %edx
	imull	$10923, %edx, %edx
	sarl	$15, %edx
	movw	%dx, 742(%rsp)
	movq	%rax, %rdx
.L5:
	decq	%rsi
	jne	.L4
	subq	$40, %rdx
	subq	$20, %rcx
	subq	$20, %rdi
.L4:
	vmovdqu	(%rdx), %ymm7
	vpand	(%rdx), %ymm1, %ymm5
	addq	$32, %rcx
	addq	$64, %rdx
	addq	$32, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rdx), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rdx), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%rcx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rsi, %rsi
	jne	.L5
	movw	742(%rsp), %dx
	vmovdqa	.LC8(%rip), %ymm6
	movq	%rax, %r8
	movq	%rax, %rcx
	movw	%dx, 314(%rsp)
	movl	$7, %edx
.L7:
	movq	%rdi, %rsi
	decq	%rdx
	jne	.L6
	subq	$20, %rcx
	subq	$10, %r8
	subq	$10, %rsi
.L6:
	vmovdqu	(%rcx), %ymm7
	vpand	(%rcx), %ymm1, %ymm5
	addq	$32, %r8
	addq	$64, %rcx
	leaq	32(%rsi), %rdi
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rcx), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rcx), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%r8)
	vmovdqu	%ymm0, (%rsi)
	testq	%rdx, %rdx
	jne	.L7
	movw	314(%rsp), %di
	xorl	%ecx, %ecx
	movw	%di, 100(%rsp)
.L8:
	movzwl	2(%rax,%rcx,4), %r8d
	movzwl	(%rax,%rcx,4), %r9d
	imull	$65, %r8d, %r8d
	addl	%r9d, %r8d
	movw	%r8w, (%rax,%rcx,2)
	incq	%rcx
	cmpq	$53, %rcx
	jne	.L8
	movzwl	%di, %edi
	movzwl	98(%rsp), %ecx
	leaq	33(%rsi), %r10
	vmovdqa	.LC9(%rip), %ymm6
	imull	$65, %edi, %edi
	movl	$2, %r8d
	movl	$2, %r9d
	addl	%edi, %ecx
	movq	%rax, %rdi
	movb	%cl, 32(%rsi)
	shrl	$8, %ecx
	movw	%cx, -8(%rsp)
	movq	%rax, %rcx
.L10:
	decq	%r9
	movq	%r10, %rsi
	jne	.L9
	subq	$24, %rcx
	subq	$12, %rdi
	subq	$12, %rsi
.L9:
	vmovdqu	(%rcx), %ymm7
	vpand	(%rcx), %ymm1, %ymm5
	addq	$32, %rdi
	addq	$64, %rcx
	leaq	32(%rsi), %r10
	movl	$1, %r9d
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rcx), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rcx), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%rdi)
	decl	%r8d
	vmovdqu	%ymm0, (%rsi)
	je	.L28
	movl	$1, %r8d
	jmp	.L10
.L28:
	movzwl	-8(%rsp), %ecx
	movzwl	-10(%rsp), %edi
	movl	$2, %r9d
	movq	%rax, %r8
	vmovdqa	.LC10(%rip), %ymm4
	movl	$2, %r10d
	imull	$4225, %ecx, %ecx
	addl	%edi, %ecx
	leaq	33(%rsi), %rdi
	movb	%cl, 32(%rsi)
	shrl	$8, %ecx
	movw	%cx, -62(%rsp)
	movq	%rax, %rcx
.L12:
	decq	%r10
	movq	%rdi, %rsi
	jne	.L11
	subq	$12, %r8
	subq	$6, %rcx
	subq	$3, %rsi
.L11:
	vmovdqu	(%r8), %ymm6
	addq	$16, %rcx
	addq	$32, %r8
	leaq	8(%rsi), %rdi
	vpand	-32(%r8), %ymm1, %ymm3
	movl	$1, %r10d
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rcx)
	vextracti128	$0x1, %ymm0, %xmm0
	decl	%r9d
	vmovd	%xmm0, (%rsi)
	vpextrd	$2, %xmm0, 4(%rsi)
	je	.L29
	movl	$1, %r9d
	jmp	.L12
.L29:
	movw	-62(%rsp), %cx
	xorl	%r8d, %r8d
	movw	%cx, -88(%rsp)
.L13:
	movzwl	2(%rax,%r8,4), %ecx
	movzwl	(%rax,%r8,4), %r9d
	imull	$292, %ecx, %ecx
	addl	%r9d, %ecx
	movb	%cl, (%rdi,%r8)
	shrl	$8, %ecx
	movw	%cx, (%rax,%r8,2)
	incq	%r8
	cmpq	$7, %r8
	jne	.L13
.L14:
	movzwl	2(%rax,%rdx,4), %ecx
	movzwl	(%rax,%rdx,4), %r8d
	imull	$334, %ecx, %ecx
	addl	%r8d, %ecx
	movb	%cl, 7(%rdi,%rdx)
	shrl	$8, %ecx
	movw	%cx, (%rax,%rdx,2)
	incq	%rdx
	cmpq	$3, %rdx
	jne	.L14
	movzwl	-112(%rsp), %eax
	movzwl	-102(%rsp), %ecx
	movzwl	-114(%rsp), %edx
	imull	$436, %eax, %eax
	imull	$436, %ecx, %ecx
	addl	%edx, %eax
	movzwl	-110(%rsp), %edx
	movb	%al, 18(%rsi)
	shrl	$8, %eax
	addl	%ecx, %edx
	movzwl	%ax, %ecx
	movb	%dl, 19(%rsi)
	shrl	$8, %edx
	movzwl	%dx, %eax
	imull	$743, %eax, %eax
	addl	%ecx, %eax
	movw	%ax, 20(%rsi)
	shrl	$16, %eax
	movb	%al, 22(%rsi)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_857x1723), .-CRYPTO_NAMESPACE(crypto_encode_857x1723)
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
	.long	1723
	.long	1723
	.long	1723
	.long	1723
	.long	1723
	.long	1723
	.long	1723
	.long	1723
	.align 32
.LC4:
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.align 32
.LC5:
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
.LC6:
	.long	11597
	.long	11597
	.long	11597
	.long	11597
	.long	11597
	.long	11597
	.long	11597
	.long	11597
	.align 32
.LC7:
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
.LC8:
	.long	2053
	.long	2053
	.long	2053
	.long	2053
	.long	2053
	.long	2053
	.long	2053
	.long	2053
	.align 32
.LC9:
	.long	4225
	.long	4225
	.long	4225
	.long	4225
	.long	4225
	.long	4225
	.long	4225
	.long	4225
	.align 32
.LC10:
	.long	273
	.long	273
	.long	273
	.long	273
	.long	273
	.long	273
	.long	273
	.long	273
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
