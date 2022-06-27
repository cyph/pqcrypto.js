	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_761x1531)
	.type	CRYPTO_NAMESPACE(crypto_encode_761x1531), @function
CRYPTO_NAMESPACE(crypto_encode_761x1531):
.LFB5342:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rcx
	movl	$48, %r8d
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$648, %rsp
	.cfi_offset 3, -24
	leaq	-114(%rsp), %rdx
	movq	%rdx, %rax
	vmovdqa	.LC0(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm6
	vmovdqa	.LC2(%rip), %ymm5
	vmovdqa	.LC3(%rip), %ymm1
	vmovdqa	.LC4(%rip), %ymm4
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
	movswl	1520(%rsi), %edx
	vmovdqa	.LC6(%rip), %ymm6
	movq	%rax, %rcx
	movl	$12, %esi
	vmovdqa	.LC7(%rip), %ymm3
	addl	$2295, %edx
	andl	$16383, %edx
	imull	$10923, %edx, %edx
	sarl	$15, %edx
	movw	%dx, 646(%rsp)
	movq	%rax, %rdx
.L5:
	decq	%rsi
	jne	.L4
	subq	$8, %rdx
	subq	$4, %rcx
	subq	$4, %rdi
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
	movw	646(%rsp), %dx
	vmovdqa	.LC8(%rip), %ymm5
	movq	%rax, %rcx
	movq	%rax, %r8
	movw	%dx, 266(%rsp)
	movl	$12, %edx
.L7:
	movq	%rdi, %rsi
	decq	%rdx
	jne	.L6
	subq	$4, %r8
	subq	$2, %rcx
	decq	%rsi
.L6:
	vmovdqu	(%r8), %ymm7
	addq	$16, %rcx
	addq	$32, %r8
	leaq	8(%rsi), %rdi
	vpand	-32(%r8), %ymm1, %ymm4
	vpsrld	$16, %ymm7, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rcx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rsi)
	vpextrd	$2, %xmm0, 4(%rsi)
	testq	%rdx, %rdx
	jne	.L7
	movw	266(%rsp), %cx
	vmovdqa	.LC9(%rip), %ymm6
	movw	%cx, 76(%rsp)
	xorl	%ecx, %ecx
.L8:
	vmovdqu	(%rax,%rcx,2), %ymm7
	vpand	(%rax,%rcx,2), %ymm1, %ymm5
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	32(%rax,%rcx,2), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	32(%rax,%rcx,2), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, (%rax,%rcx)
	vmovdqu	%ymm0, (%rdi,%rcx)
	addq	$32, %rcx
	cmpq	$96, %rcx
	jne	.L8
	vmovdqa	.LC10(%rip), %ymm4
	xorl	%ecx, %ecx
.L9:
	vmovdqu	(%rax,%rcx,4), %ymm6
	vpand	(%rax,%rcx,4), %ymm1, %ymm3
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rax,%rcx,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, 96(%rdi,%rcx)
	vpextrd	$2, %xmm0, 100(%rdi,%rcx)
	addq	$8, %rcx
	cmpq	$24, %rcx
	jne	.L9
	vmovdqa	.LC11(%rip), %ymm4
	leaq	128(%rsi), %r8
	movq	%rax, %rcx
	movq	%rax, %rdi
	movl	$2, %r9d
	movl	$2, %r10d
.L11:
	decq	%r10
	movq	%r8, %rsi
	jne	.L10
	subq	$16, %rdi
	subq	$8, %rcx
	subq	$4, %rsi
.L10:
	vmovdqu	(%rdi), %ymm6
	vpand	(%rdi), %ymm1, %ymm3
	addq	$16, %rcx
	addq	$32, %rdi
	leaq	8(%rsi), %r8
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
	je	.L26
	movl	$1, %r9d
	jmp	.L11
.L26:
	xorl	%edi, %edi
.L12:
	movzwl	2(%rax,%rdi,4), %ecx
	movzwl	(%rax,%rdi,4), %r9d
	imull	$9097, %ecx, %ecx
	addl	%r9d, %ecx
	movb	%cl, (%r8,%rdi,2)
	movzbl	%ch, %ebx
	shrl	$16, %ecx
	movb	%bl, 1(%r8,%rdi,2)
	movw	%cx, (%rax,%rdi,2)
	incq	%rdi
	cmpq	$6, %rdi
	jne	.L12
.L13:
	movzwl	2(%rax,%rdx,4), %ecx
	movzwl	(%rax,%rdx,4), %edi
	imull	$1263, %ecx, %ecx
	addl	%edi, %ecx
	movb	%cl, 12(%r8,%rdx)
	shrl	$8, %ecx
	movw	%cx, (%rax,%rdx,2)
	incq	%rdx
	cmpq	$3, %rdx
	jne	.L13
	movzwl	-112(%rsp), %eax
	movzwl	-114(%rsp), %edx
	imull	$6232, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 23(%rsi)
	shrl	$16, %eax
	movl	%eax, %edx
	movzwl	-110(%rsp), %eax
	imull	$593, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 25(%rsi)
	shrl	$8, %eax
	movb	%ah, 27(%rsi)
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_761x1531), .-CRYPTO_NAMESPACE(crypto_encode_761x1531)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
	.value	2295
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
	.long	1531
	.long	1531
	.long	1531
	.long	1531
	.long	1531
	.long	1531
	.long	1531
	.long	1531
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
	.long	9157
	.long	9157
	.long	9157
	.long	9157
	.long	9157
	.long	9157
	.long	9157
	.long	9157
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
	.long	1280
	.long	1280
	.long	1280
	.long	1280
	.long	1280
	.long	1280
	.long	1280
	.long	1280
	.align 32
.LC9:
	.long	6400
	.long	6400
	.long	6400
	.long	6400
	.long	6400
	.long	6400
	.long	6400
	.long	6400
	.align 32
.LC10:
	.long	625
	.long	625
	.long	625
	.long	625
	.long	625
	.long	625
	.long	625
	.long	625
	.align 32
.LC11:
	.long	1526
	.long	1526
	.long	1526
	.long	1526
	.long	1526
	.long	1526
	.long	1526
	.long	1526
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
