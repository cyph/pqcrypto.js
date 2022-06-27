	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_653x1541round)
	.type	CRYPTO_NAMESPACE(crypto_encode_653x1541round), @function
CRYPTO_NAMESPACE(crypto_encode_653x1541round):
.LFB5342:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rdx
	movl	$41, %r8d
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$552, %rsp
	.cfi_offset 3, -24
	leaq	-102(%rsp), %rax
	movq	%rax, %rcx
	vmovdqa	.LC0(%rip), %ymm8
	vmovdqa	.LC1(%rip), %ymm7
	vmovdqa	.LC2(%rip), %ymm6
	vmovdqa	.LC3(%rip), %ymm5
	vmovdqa	.LC4(%rip), %ymm4
	vmovdqa	.LC5(%rip), %ymm1
	vmovdqa	.LC6(%rip), %ymm2
.L3:
	decq	%r8
	jne	.L2
	subq	$8, %rdx
	subq	$4, %rax
	subq	$2, %rdi
.L2:
	vpmulhrsw	(%rdx), %ymm8, %ymm0
	vpaddw	%ymm0, %ymm0, %ymm3
	addq	$32, %rdx
	addq	$16, %rax
	vpaddw	%ymm7, %ymm3, %ymm3
	addq	$8, %rdi
	vpaddw	%ymm0, %ymm3, %ymm3
	vpand	%ymm6, %ymm3, %ymm3
	vpmulhw	%ymm5, %ymm3, %ymm3
	vpsrld	$16, %ymm3, %ymm0
	vpand	%ymm1, %ymm3, %ymm3
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rax)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%r8, %r8
	jne	.L3
	movswl	1304(%rsi), %eax
	vmovdqa	.LC7(%rip), %ymm6
	movq	%rcx, %rdx
	movl	$11, %esi
	vmovdqa	.LC8(%rip), %ymm3
	imull	$10923, %eax, %eax
	addl	$16384, %eax
	sarl	$15, %eax
	leal	(%rax,%rax,2), %eax
	addl	$2310, %eax
	andl	$16383, %eax
	imull	$10923, %eax, %eax
	sarl	$15, %eax
	movw	%ax, 550(%rsp)
	movq	%rcx, %rax
.L5:
	decq	%rsi
	jne	.L4
	subq	$52, %rax
	subq	$26, %rdx
	subq	$26, %rdi
.L4:
	vmovdqu	(%rax), %ymm7
	vpand	(%rax), %ymm1, %ymm5
	addq	$32, %rdx
	addq	$64, %rax
	addq	$32, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rax), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rax), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%rdx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rsi, %rsi
	jne	.L5
	movw	550(%rsp), %ax
	vmovdqa	.LC9(%rip), %ymm5
	movq	%rcx, %rdx
	movl	$11, %esi
	movw	%ax, 224(%rsp)
	movq	%rcx, %rax
.L7:
	decq	%rsi
	jne	.L6
	subq	$24, %rdx
	subq	$12, %rax
	subq	$6, %rdi
.L6:
	vmovdqu	(%rdx), %ymm7
	vpand	(%rdx), %ymm1, %ymm4
	addq	$16, %rax
	addq	$32, %rdx
	addq	$8, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rax)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%rsi, %rsi
	jne	.L7
	vmovdqa	.LC10(%rip), %ymm6
	movq	%rcx, %rdx
	movq	%rcx, %rax
	movl	$3, %esi
.L9:
	decq	%rsi
	jne	.L8
	subq	$28, %rax
	subq	$14, %rdx
	subq	$14, %rdi
.L8:
	vmovdqu	(%rax), %ymm7
	vpand	(%rax), %ymm1, %ymm5
	addq	$32, %rdx
	addq	$64, %rax
	addq	$32, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rax), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rax), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%rdx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rsi, %rsi
	jne	.L9
	vmovdqa	.LC11(%rip), %ymm4
	movq	%rcx, %rdx
	movq	%rcx, %r8
	movl	$3, %eax
.L11:
	movq	%rdi, %rsi
	decq	%rax
	jne	.L10
	subq	$16, %r8
	subq	$8, %rdx
	subq	$4, %rsi
.L10:
	vmovdqu	(%r8), %ymm6
	addq	$16, %rdx
	addq	$32, %r8
	leaq	8(%rsi), %rdi
	vpand	-32(%r8), %ymm1, %ymm3
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rdx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rsi)
	vpextrd	$2, %xmm0, 4(%rsi)
	testq	%rax, %rax
	jne	.L11
	movw	-22(%rsp), %r8w
	xorl	%r9d, %r9d
.L12:
	movzwl	2(%rcx,%r9,4), %edx
	movzwl	(%rcx,%r9,4), %r10d
	imull	$1887, %edx, %edx
	addl	%r10d, %edx
	movb	%dl, (%rdi,%r9)
	shrl	$8, %edx
	movw	%dx, (%rcx,%r9,2)
	incq	%r9
	cmpq	$10, %r9
	jne	.L12
.L13:
	movzwl	2(%rcx,%rax,4), %ebx
	movzwl	(%rcx,%rax,4), %edx
	imull	$13910, %ebx, %ebx
	addl	%edx, %ebx
	movb	%bl, 10(%rdi,%rax,2)
	movb	%bh, 11(%rdi,%rax,2)
	shrl	$16, %ebx
	movw	%bx, (%rcx,%rax,2)
	incq	%rax
	cmpq	$5, %rax
	jne	.L13
	movzwl	-100(%rsp), %eax
	movzwl	-102(%rsp), %edx
	movzwl	%r8w, %r8d
	imull	$2953, %r8d, %r8d
	imull	$2953, %eax, %eax
	addl	%edx, %eax
	movzwl	-98(%rsp), %edx
	movw	%ax, 28(%rsi)
	shrl	$16, %eax
	movl	%eax, %ecx
	movzwl	-96(%rsp), %eax
	imull	$2953, %eax, %eax
	addl	%edx, %eax
	movzwl	-94(%rsp), %edx
	movw	%ax, 30(%rsi)
	shrl	$16, %eax
	imull	$134, %eax, %eax
	addl	%r8d, %edx
	movb	%dl, 32(%rsi)
	shrl	$8, %edx
	addl	%ecx, %eax
	movb	%al, 33(%rsi)
	shrl	$8, %eax
	movl	%eax, %ecx
	movzwl	%dx, %eax
	imull	$71, %eax, %eax
	addl	%ecx, %eax
	movw	%ax, 34(%rsi)
	shrl	$8, %eax
	movb	%ah, 36(%rsi)
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_653x1541round), .-CRYPTO_NAMESPACE(crypto_encode_653x1541round)
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
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
	.value	2310
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
	.long	1541
	.long	1541
	.long	1541
	.long	1541
	.long	1541
	.long	1541
	.long	1541
	.long	1541
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
	.long	9277
	.long	9277
	.long	9277
	.long	9277
	.long	9277
	.long	9277
	.long	9277
	.long	9277
	.align 32
.LC8:
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
.LC9:
	.long	1314
	.long	1314
	.long	1314
	.long	1314
	.long	1314
	.long	1314
	.long	1314
	.long	1314
	.align 32
.LC10:
	.long	6745
	.long	6745
	.long	6745
	.long	6745
	.long	6745
	.long	6745
	.long	6745
	.long	6745
	.align 32
.LC11:
	.long	695
	.long	695
	.long	695
	.long	695
	.long	695
	.long	695
	.long	695
	.long	695
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
