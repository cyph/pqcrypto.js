	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_953x2115round)
	.type	CRYPTO_NAMESPACE(crypto_encode_953x2115round), @function
CRYPTO_NAMESPACE(crypto_encode_953x2115round):
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
	leaq	-114(%rsp), %rax
	movq	%rax, %rdx
	vmovdqa	.LC0(%rip), %ymm8
	vmovdqa	.LC1(%rip), %ymm7
	vmovdqa	.LC2(%rip), %ymm6
	vmovdqa	.LC3(%rip), %ymm5
	vmovdqa	.LC4(%rip), %ymm1
	vmovdqa	.LC5(%rip), %ymm4
	vmovdqa	.LC6(%rip), %ymm3
.L3:
	decq	%rcx
	jne	.L2
	subq	$16, %rsi
	subq	$8, %rax
	subq	$8, %rdi
.L2:
	vpmulhrsw	(%rsi), %ymm8, %ymm0
	vpaddw	%ymm0, %ymm0, %ymm9
	addq	$64, %rsi
	addq	$32, %rax
	vpaddw	%ymm7, %ymm9, %ymm9
	vpmulhrsw	-32(%rsi), %ymm8, %ymm10
	vpaddw	%ymm10, %ymm10, %ymm2
	addq	$32, %rdi
	vpaddw	%ymm0, %ymm9, %ymm9
	vpaddw	%ymm7, %ymm2, %ymm2
	vpand	%ymm6, %ymm9, %ymm9
	vpaddw	%ymm10, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm9, %ymm9
	vpand	%ymm6, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm2, %ymm2
	vpsrld	$16, %ymm9, %ymm0
	vpand	%ymm1, %ymm9, %ymm9
	vpmulld	%ymm4, %ymm0, %ymm0
	vpsrld	$16, %ymm2, %ymm10
	vpand	%ymm1, %ymm2, %ymm2
	vpaddd	%ymm9, %ymm0, %ymm0
	vpmulld	%ymm4, %ymm10, %ymm9
	vpshufb	%ymm3, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddd	%ymm2, %ymm9, %ymm2
	vpshufb	%ymm3, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vperm2i128	$49, %ymm2, %ymm0, %ymm9
	vinserti128	$1, %xmm2, %ymm0, %ymm0
	vmovdqu	%ymm9, -32(%rax)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rcx, %rcx
	jne	.L3
	movswl	1904(%r8), %eax
	imull	$10923, %eax, %eax
	addl	$16384, %eax
	sarl	$15, %eax
	leal	(%rax,%rax,2), %eax
	addl	$3171, %eax
	andl	$16383, %eax
	imull	$10923, %eax, %eax
	sarl	$15, %eax
	movw	%ax, 838(%rsp)
.L4:
	movzwl	2(%rdx,%rcx,4), %esi
	movzwl	(%rdx,%rcx,4), %r8d
	imull	$69, %esi, %esi
	addl	%r8d, %esi
	movw	%si, (%rdx,%rcx,2)
	incq	%rcx
	cmpq	$238, %rcx
	jne	.L4
	vmovdqa	.LC7(%rip), %ymm5
	movw	%ax, 362(%rsp)
	movq	%rdx, %rcx
	movq	%rdx, %rax
	movl	$8, %esi
.L6:
	decq	%rsi
	jne	.L5
	subq	$36, %rax
	subq	$18, %rcx
	subq	$18, %rdi
.L5:
	vmovdqu	(%rax), %ymm6
	vpand	(%rax), %ymm1, %ymm4
	addq	$32, %rcx
	addq	$64, %rax
	vmovdqu	-32(%rax), %ymm7
	addq	$32, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm2
	vpmulld	%ymm5, %ymm2, %ymm2
	vpaddd	%ymm4, %ymm0, %ymm0
	vpand	-32(%rax), %ymm1, %ymm4
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
	movw	362(%rsp), %ax
	vmovdqa	.LC8(%rip), %ymm4
	movq	%rdx, %rcx
	movl	$8, %esi
	vmovdqa	.LC9(%rip), %ymm2
	movw	%ax, 124(%rsp)
	movq	%rdx, %rax
.L8:
	decq	%rsi
	jne	.L7
	subq	$16, %rcx
	subq	$8, %rax
	subq	$4, %rdi
.L7:
	vmovdqu	(%rcx), %ymm6
	vpand	(%rcx), %ymm1, %ymm3
	addq	$16, %rax
	addq	$32, %rcx
	addq	$8, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rax)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%rsi, %rsi
	jne	.L8
	vmovdqa	.LC10(%rip), %ymm4
	movq	%rdx, %rcx
	movq	%rdx, %rsi
	movl	$4, %r11d
.L10:
	decq	%r11
	jne	.L9
	subq	$8, %rsi
	subq	$4, %rcx
	subq	$2, %rdi
.L9:
	vmovdqu	(%rsi), %ymm7
	vpand	(%rsi), %ymm1, %ymm3
	addq	$16, %rcx
	addq	$32, %rsi
	addq	$8, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rcx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%r11, %r11
	jne	.L10
	vmovdqa	.LC11(%rip), %ymm4
	movl	$2, %r9d
	movq	%rdx, %rsi
	movq	%rdx, %r8
	movl	$2, %r10d
.L12:
	decq	%r10
	movq	%rdi, %rcx
	jne	.L11
	subq	$8, %r8
	subq	$4, %rsi
	subq	$2, %rcx
.L11:
	vmovdqu	(%r8), %ymm5
	addq	$16, %rsi
	addq	$32, %r8
	leaq	8(%rcx), %rdi
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
	vmovd	%xmm0, (%rcx)
	vpextrd	$2, %xmm0, 4(%rcx)
	je	.L26
	movl	$1, %r9d
	jmp	.L12
.L26:
	movzwl	-56(%rsp), %esi
	movzwl	-58(%rsp), %edi
	imull	$856, %esi, %esi
	addl	%edi, %esi
	xorl	%edi, %edi
	movw	%si, 8(%rcx)
	shrl	$16, %esi
.L13:
	movzwl	2(%rdx,%rdi,4), %ebx
	movzwl	(%rdx,%rdi,4), %r8d
	imull	$2863, %ebx, %ebx
	addl	%r8d, %ebx
	movb	%bl, 10(%rcx,%rdi,2)
	movb	%bh, 11(%rcx,%rdi,2)
	shrl	$16, %ebx
	movw	%bx, (%rdx,%rdi,2)
	incq	%rdi
	cmpq	$7, %rdi
	jne	.L13
	movw	%si, -100(%rsp)
.L14:
	movzwl	2(%rdx,%r11,4), %esi
	movzwl	(%rdx,%r11,4), %edi
	imull	$126, %esi, %esi
	addl	%edi, %esi
	movw	%si, (%rdx,%r11,2)
	incq	%r11
	cmpq	$4, %r11
	jne	.L14
	movzwl	-112(%rsp), %eax
	movzwl	-114(%rsp), %edx
	movzwl	-110(%rsp), %esi
	imull	$15876, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 24(%rcx)
	shrl	$16, %eax
	movl	%eax, %edx
	movzwl	-108(%rsp), %eax
	imull	$15876, %eax, %eax
	addl	%esi, %eax
	movw	%ax, 26(%rcx)
	shrl	$16, %eax
	imull	$3846, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 28(%rcx)
	shrl	$16, %eax
	movb	%al, 30(%rcx)
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_953x2115round), .-CRYPTO_NAMESPACE(crypto_encode_953x2115round)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.align 32
.LC1:
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
.LC2:
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.align 32
.LC3:
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
.LC4:
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.align 32
.LC5:
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.long	2115
	.align 32
.LC6:
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
.LC7:
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.long	4761
	.align 32
.LC8:
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
	.long	346
	.align 32
.LC9:
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
.LC10:
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.long	468
	.align 32
.LC11:
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
