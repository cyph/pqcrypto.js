	.file	"encode.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(crypto_encode_761x4591)
	.type	CRYPTO_NAMESPACE(crypto_encode_761x4591), @function
CRYPTO_NAMESPACE(crypto_encode_761x4591):
.LFB5347:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r9
	movq	%rsi, %rdx
	movl	$23, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$648, %rsp
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
	movzwl	1520(%r9), %edx
	vmovdqa	.LC5(%rip), %ymm1
	movq	%r8, %rdi
	movq	%r8, %rsi
	vmovdqa	.LC6(%rip), %ymm2
	addw	$2295, %dx
	andw	$16383, %dx
	movw	%dx, 640(%rsp)
	movl	$23, %edx
	.p2align 4,,10
	.p2align 3
.L6:
	vmovdqu	(%rsi), %ymm5
	vpand	(%rsi), %ymm1, %ymm3
	leaq	8(%rax), %rcx
	vpsrld	$16, %ymm5, %ymm4
	vpslld	$2, %ymm4, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpslld	$5, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpslld	$1, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
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
	movzwl	640(%rsp), %eax
	vmovdqa	.LC7(%rip), %ymm4
	movq	%r8, %rsi
	movq	%r8, %rdx
	movw	%ax, 260(%rsp)
	movl	$11, %eax
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rdx), %ymm6
	vpand	(%rdx), %ymm1, %ymm3
	leaq	8(%rcx), %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
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
	movq	%rdi, %rcx
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
	movzwl	260(%rsp), %eax
	movw	%ax, 70(%rsp)
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L14:
	vmovdqu	(%r8,%rax,4), %ymm6
	vpand	(%r8,%rax,4), %ymm1, %ymm3
	vpsrld	$16, %ymm6, %ymm4
	vpslld	$2, %ymm4, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpslld	$5, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%r8,%rax,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rdi,%rax)
	vpextrd	$2, %xmm0, 4(%rdi,%rax)
	addq	$8, %rax
	cmpq	$48, %rax
	jne	.L14
	addq	$56, %rcx
	movl	$2, %esi
	movq	%r8, %rdx
	movq	%r8, %rax
	vmovdqa	.LC8(%rip), %ymm4
.L21:
	vmovdqu	(%rax), %ymm6
	vpand	(%rax), %ymm1, %ymm3
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rdx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rcx)
	vpextrd	$2, %xmm0, 4(%rcx)
	testq	%rsi, %rsi
	je	.L33
	decq	%rsi
	jne	.L34
	addq	$28, %rax
	addq	$14, %rdx
	addq	$7, %rcx
	jmp	.L21
	.p2align 4,,10
	.p2align 3
.L33:
	movzwl	-26(%rsp), %eax
	movzwl	-28(%rsp), %edx
	imull	$1621, %eax, %eax
	addl	%edx, %eax
	xorl	%edx, %edx
	movw	%ax, 8(%rcx)
	shrl	$16, %eax
	movl	%eax, %r9d
	.p2align 4,,10
	.p2align 3
.L19:
	movzwl	2(%r8,%rdx,4), %eax
	movzwl	(%r8,%rdx,4), %edi
	imull	$10265, %eax, %eax
	addl	%edi, %eax
	movb	%al, 10(%rcx,%rdx,2)
	movb	%ah, 11(%rcx,%rdx,2)
	shrl	$16, %eax
	movw	%ax, (%r8,%rdx,2)
	incq	%rdx
	cmpq	$11, %rdx
	jne	.L19
	imull	$10265, %r9d, %eax
	movzwl	-76(%rsp), %edx
	addl	%edx, %eax
	movb	%al, 32(%rcx)
	shrl	$8, %eax
	movl	%eax, %edi
.L20:
	movzwl	2(%r8,%rsi,4), %eax
	movzwl	(%r8,%rsi,4), %edx
	imull	$1608, %eax, %eax
	addl	%edx, %eax
	movb	%al, 33(%rcx,%rsi)
	shrl	$8, %eax
	movw	%ax, (%r8,%rsi,2)
	incq	%rsi
	cmpq	$5, %rsi
	jne	.L20
	imull	$1608, %edi, %eax
	movzwl	-100(%rsp), %edx
	movzwl	-120(%rsp), %esi
	movzwl	-116(%rsp), %edi
	addl	%eax, %edx
	movzwl	-118(%rsp), %eax
	movw	%dx, 38(%rcx)
	shrl	$16, %edx
	imull	$10101, %eax, %eax
	addl	%esi, %eax
	movw	%ax, 40(%rcx)
	shrl	$16, %eax
	movl	%eax, %esi
	movzwl	-114(%rsp), %eax
	imull	$10101, %eax, %eax
	addl	%edi, %eax
	imull	$10101, %edx, %edi
	movzwl	-112(%rsp), %edx
	movw	%ax, 42(%rcx)
	shrl	$16, %eax
	imull	$1557, %eax, %eax
	addl	%edi, %edx
	movb	%dl, 44(%rcx)
	shrl	$8, %edx
	imull	$9470, %edx, %edx
	addl	%esi, %eax
	movb	%al, 45(%rcx)
	shrl	$8, %eax
	addl	%edx, %eax
	movl	%eax, 46(%rcx)
	vzeroupper
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L34:
	.cfi_restore_state
	addq	$32, %rax
	addq	$16, %rdx
	addq	$8, %rcx
	jmp	.L21
	.cfi_endproc
.LFE5347:
	.size	CRYPTO_NAMESPACE(crypto_encode_761x4591), .-CRYPTO_NAMESPACE(crypto_encode_761x4591)
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
	.quad	70364449226751
	.quad	70364449226751
	.quad	70364449226751
	.quad	70364449226751
	.align 32
.LC3:
	.long	4591
	.long	4591
	.long	4591
	.long	4591
	.long	4591
	.long	4591
	.long	4591
	.long	4591
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
	.long	406
	.long	406
	.long	406
	.long	406
	.long	406
	.long	406
	.long	406
	.long	406
	.align 32
.LC8:
	.long	1621
	.long	1621
	.long	1621
	.long	1621
	.long	1621
	.long	1621
	.long	1621
	.long	1621
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
