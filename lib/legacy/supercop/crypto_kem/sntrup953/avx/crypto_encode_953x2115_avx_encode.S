	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_953x2115)
	.type	CRYPTO_NAMESPACE(crypto_encode_953x2115), @function
CRYPTO_NAMESPACE(crypto_encode_953x2115):
.LFB5342:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r8
	movl	$30, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$840, %rsp
	.cfi_offset 3, -24
	leaq	-114(%rsp), %rdx
	movq	%rdx, %rax
	vmovdqa	.LC0(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm6
	vmovdqa	.LC2(%rip), %ymm5
	vmovdqa	.LC3(%rip), %ymm1
	vmovdqa	.LC4(%rip), %ymm4
	vmovdqa	.LC5(%rip), %ymm3
.L3:
	decq	%rcx
	jne	.L2
	subq	$16, %rsi
	subq	$8, %rdx
	subq	$8, %rdi
.L2:
	vpaddw	(%rsi), %ymm7, %ymm9
	addq	$32, %rdx
	addq	$64, %rsi
	addq	$32, %rdi
	vpaddw	-32(%rsi), %ymm7, %ymm8
	vpand	%ymm6, %ymm9, %ymm9
	vpand	%ymm6, %ymm8, %ymm8
	vpmulhw	%ymm5, %ymm9, %ymm9
	vpmulhw	%ymm5, %ymm8, %ymm8
	vpsrld	$16, %ymm9, %ymm0
	vpand	%ymm1, %ymm9, %ymm9
	vpsrld	$16, %ymm8, %ymm2
	vpmulld	%ymm4, %ymm0, %ymm0
	vpand	%ymm1, %ymm8, %ymm8
	vpmulld	%ymm4, %ymm2, %ymm2
	vpaddd	%ymm9, %ymm0, %ymm0
	vpaddd	%ymm8, %ymm2, %ymm2
	vpshufb	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpermq	$216, %ymm2, %ymm2
	vperm2i128	$49, %ymm2, %ymm0, %ymm8
	vinserti128	$1, %xmm2, %ymm0, %ymm0
	vmovdqu	%ymm8, -32(%rdx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rcx, %rcx
	jne	.L3
	movswl	1904(%r8), %edx
	addl	$3171, %edx
	andl	$16383, %edx
	imull	$10923, %edx, %edx
	sarl	$15, %edx
	movw	%dx, 838(%rsp)
.L4:
	movzwl	2(%rax,%rcx,4), %esi
	movzwl	(%rax,%rcx,4), %r8d
	imull	$69, %esi, %esi
	addl	%r8d, %esi
	movw	%si, (%rax,%rcx,2)
	incq	%rcx
	cmpq	$238, %rcx
	jne	.L4
	vmovdqa	.LC6(%rip), %ymm5
	movw	%dx, 362(%rsp)
	movq	%rax, %rcx
	movq	%rax, %rdx
	movl	$8, %esi
.L6:
	decq	%rsi
	jne	.L5
	subq	$36, %rdx
	subq	$18, %rcx
	subq	$18, %rdi
.L5:
	vmovdqu	(%rdx), %ymm6
	vpand	(%rdx), %ymm1, %ymm4
	addq	$32, %rcx
	addq	$64, %rdx
	vmovdqu	-32(%rdx), %ymm7
	addq	$32, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm2
	vpmulld	%ymm5, %ymm2, %ymm2
	vpaddd	%ymm4, %ymm0, %ymm0
	vpand	-32(%rdx), %ymm1, %ymm4
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vperm2i128	$49, %ymm2, %ymm0, %ymm4
	vinserti128	$1, %xmm2, %ymm0, %ymm0
	vmovdqu	%ymm4, -32(%rcx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rsi, %rsi
	jne	.L6
	movw	362(%rsp), %dx
	vmovdqa	.LC7(%rip), %ymm4
	movq	%rax, %rcx
	movl	$8, %esi
	vmovdqa	.LC8(%rip), %ymm2
	movw	%dx, 124(%rsp)
	movq	%rax, %rdx
.L8:
	decq	%rsi
	jne	.L7
	subq	$16, %rcx
	subq	$8, %rdx
	subq	$4, %rdi
.L7:
	vmovdqu	(%rcx), %ymm6
	vpand	(%rcx), %ymm1, %ymm3
	addq	$16, %rdx
	addq	$32, %rcx
	addq	$8, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rdx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%rsi, %rsi
	jne	.L8
	vmovdqa	.LC9(%rip), %ymm4
	movq	%rax, %rdx
	movq	%rax, %rsi
	movl	$4, %r11d
.L10:
	decq	%r11
	jne	.L9
	subq	$8, %rsi
	subq	$4, %rdx
	subq	$2, %rdi
.L9:
	vmovdqu	(%rsi), %ymm7
	vpand	(%rsi), %ymm1, %ymm3
	addq	$16, %rdx
	addq	$32, %rsi
	addq	$8, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rdx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%r11, %r11
	jne	.L10
	vmovdqa	.LC10(%rip), %ymm4
	movl	$2, %r9d
	movq	%rax, %rsi
	movq	%rax, %r8
	movl	$2, %r10d
.L12:
	decq	%r10
	movq	%rdi, %rdx
	jne	.L11
	subq	$8, %r8
	subq	$4, %rsi
	subq	$2, %rdx
.L11:
	vmovdqu	(%r8), %ymm5
	addq	$16, %rsi
	addq	$32, %r8
	leaq	8(%rdx), %rdi
	vpand	-32(%r8), %ymm1, %ymm3
	movl	$1, %r10d
	vpsrld	$16, %ymm5, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rsi)
	vextracti128	$0x1, %ymm0, %xmm0
	decl	%r9d
	vmovd	%xmm0, (%rdx)
	vpextrd	$2, %xmm0, 4(%rdx)
	je	.L26
	movl	$1, %r9d
	jmp	.L12
.L26:
	movzwl	-56(%rsp), %esi
	movzwl	-58(%rsp), %edi
	imull	$856, %esi, %esi
	addl	%edi, %esi
	xorl	%edi, %edi
	movw	%si, 8(%rdx)
	shrl	$16, %esi
.L13:
	movzwl	2(%rax,%rdi,4), %ebx
	movzwl	(%rax,%rdi,4), %r8d
	imull	$2863, %ebx, %ebx
	addl	%r8d, %ebx
	movb	%bl, 10(%rdx,%rdi,2)
	movb	%bh, 11(%rdx,%rdi,2)
	shrl	$16, %ebx
	movw	%bx, (%rax,%rdi,2)
	incq	%rdi
	cmpq	$7, %rdi
	jne	.L13
	movw	%si, -100(%rsp)
.L14:
	movzwl	2(%rax,%r11,4), %esi
	movzwl	(%rax,%r11,4), %edi
	imull	$126, %esi, %esi
	addl	%edi, %esi
	movw	%si, (%rax,%r11,2)
	incq	%r11
	cmpq	$4, %r11
	jne	.L14
	movzwl	-112(%rsp), %eax
	movzwl	-114(%rsp), %ecx
	movzwl	-110(%rsp), %esi
	imull	$15876, %eax, %eax
	addl	%ecx, %eax
	movw	%ax, 24(%rdx)
	shrl	$16, %eax
	movl	%eax, %ecx
	movzwl	-108(%rsp), %eax
	imull	$15876, %eax, %eax
	addl	%esi, %eax
	movw	%ax, 26(%rdx)
	shrl	$16, %eax
	imull	$3846, %eax, %eax
	addl	%ecx, %eax
	movw	%ax, 28(%rdx)
	shrl	$16, %eax
	movb	%al, 30(%rdx)
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_953x2115), .-CRYPTO_NAMESPACE(crypto_encode_953x2115)
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
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
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
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.align 32
.LC7:
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
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
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.align 32
.LC10:
	.long	856
	.long	856
	.long	856
	.long	856
	.long	856
	.long	856
	.long	856
	.long	856
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
