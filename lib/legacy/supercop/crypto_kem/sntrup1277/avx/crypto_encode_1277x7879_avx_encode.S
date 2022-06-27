	.file	"encode.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(crypto_encode_1277x7879)
	.type	CRYPTO_NAMESPACE(crypto_encode_1277x7879), @function
CRYPTO_NAMESPACE(crypto_encode_1277x7879):
.LFB5347:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r9
	movq	%rsi, %rdx
	movl	$39, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%rbx
	andq	$-32, %rsp
	subq	$1160, %rsp
	.cfi_offset 14, -24
	.cfi_offset 3, -32
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
	vpand	%ymm8, %ymm3, %ymm8
	vpsrld	$16, %ymm0, %ymm0
	vpsrld	$16, %ymm1, %ymm1
	vpand	%ymm7, %ymm3, %ymm7
	vpmulld	%ymm4, %ymm0, %ymm0
	vpmulld	%ymm4, %ymm1, %ymm1
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
	je	.L24
	decq	%rcx
	je	.L3
	addq	$64, %rdx
	addq	$32, %rsi
	movq	%rax, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	addq	$56, %rdx
	addq	$28, %rsi
	addq	$28, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L24:
	movzwl	2552(%r9), %edx
	vmovdqa	.LC5(%rip), %ymm3
	movl	$39, %r14d
	movq	%r8, %rdi
	vmovdqa	.LC6(%rip), %ymm5
	vmovdqa	.LC7(%rip), %ymm4
	movq	%r8, %rsi
	addw	$3939, %dx
	andw	$16383, %dx
	movw	%dx, 1156(%rsp)
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
	testq	%r14, %r14
	je	.L25
	decq	%r14
	je	.L7
	addq	$32, %rsi
	addq	$16, %rdi
	movq	%rcx, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L7:
	addq	$28, %rsi
	addq	$14, %rdi
	addq	$7, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L25:
	movzwl	1156(%rsp), %esi
	vmovdqa	.LC8(%rip), %ymm6
	movq	%rcx, %r9
	movq	%r8, %rdi
	leaq	520(%rsp), %r10
	movw	%si, 518(%rsp)
	movq	%r8, %rsi
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rsi), %ymm5
	addq	$64, %rsi
	addq	$32, %rdi
	addq	$32, %r9
	vpsrld	$16, %ymm5, %ymm0
	vmovdqu	-32(%rsi), %ymm5
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm5, %ymm1
	vpand	-64(%rsi), %ymm3, %ymm5
	vpmulld	%ymm6, %ymm1, %ymm1
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rsi), %ymm3, %ymm5
	vpshufb	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm1, %ymm1
	vpermq	$216, %ymm1, %ymm1
	vperm2i128	$49, %ymm1, %ymm0, %ymm5
	vinserti128	$1, %xmm1, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%rdi)
	vmovdqu	%ymm0, -32(%r9)
	cmpq	%r10, %rsi
	jne	.L10
	xorl	%esi, %esi
	.p2align 4,,10
	.p2align 3
.L11:
	vmovdqu	(%r8,%rsi,4), %ymm2
	vpand	(%r8,%rsi,4), %ymm3, %ymm1
	vpsrld	$16, %ymm2, %ymm2
	vpslld	$1, %ymm2, %ymm0
	vpaddd	%ymm2, %ymm0, %ymm2
	vpslld	$6, %ymm2, %ymm0
	vpsubd	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%r8,%rsi,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, 320(%rcx,%rsi)
	vpextrd	$2, %xmm0, 324(%rcx,%rsi)
	addq	$8, %rsi
	cmpq	$80, %rsi
	jne	.L11
	xorl	%esi, %esi
.L12:
	vmovdqu	(%r8,%rsi,4), %ymm6
	vpand	(%r8,%rsi,4), %ymm3, %ymm1
	vpsrld	$16, %ymm6, %ymm2
	vpslld	$3, %ymm2, %ymm0
	vpaddd	%ymm2, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm0
	vpsubd	%ymm2, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm4, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%r8,%rsi,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, 400(%rcx,%rsi)
	vpextrd	$2, %xmm0, 404(%rcx,%rsi)
	addq	$8, %rsi
	cmpq	$40, %rsi
	jne	.L12
	movq	%r8, %r9
	leaq	-118(%rsp), %rdi
	leaq	-80(%rsp), %r11
	.p2align 4,,10
	.p2align 3
.L13:
	movzwl	(%rdi), %esi
	movzwl	-2(%rdi), %r10d
	addq	$2, %r9
	addq	$4, %rdi
	imull	$77, %esi, %esi
	addl	%r10d, %esi
	movw	%si, -2(%r9)
	cmpq	%r9, %r11
	jne	.L13
	xorl	%esi, %esi
	.p2align 4,,10
	.p2align 3
.L14:
	movzwl	2(%r8,%rsi,4), %ebx
	movzwl	(%r8,%rsi,4), %edi
	imull	$5929, %ebx, %ebx
	addl	%edi, %ebx
	movb	%bl, 440(%rcx,%rsi,2)
	movb	%bh, 441(%rcx,%rsi,2)
	shrl	$16, %ebx
	movw	%bx, (%r8,%rsi,2)
	incq	%rsi
	cmpq	$10, %rsi
	jne	.L14
.L15:
	movzwl	2(%r8,%r14,4), %esi
	movzwl	(%r8,%r14,4), %edi
	imull	$537, %esi, %esi
	addl	%edi, %esi
	movb	%sil, 460(%rcx,%r14)
	shrl	$8, %esi
	movw	%si, (%r8,%r14,2)
	incq	%r14
	cmpq	$5, %r14
	jne	.L15
	movzwl	-118(%rsp), %edx
	movzwl	-120(%rsp), %ecx
	movzwl	-116(%rsp), %esi
	imull	$1127, %edx, %edx
	addl	%ecx, %edx
	movzwl	-114(%rsp), %ecx
	movb	%dl, 473(%rax)
	shrl	$8, %edx
	imull	$1127, %ecx, %ecx
	addl	%esi, %ecx
	movzwl	%dx, %esi
	movb	%cl, 474(%rax)
	shrl	$8, %ecx
	movzwl	%cx, %edx
	movzwl	-112(%rsp), %ecx
	imull	$4962, %edx, %edx
	imull	$376, %ecx, %ecx
	addl	%esi, %edx
	movw	%dx, 475(%rax)
	shrl	$16, %edx
	addl	%ecx, %edx
	movw	%dx, 477(%rax)
	shrl	$8, %edx
	movb	%dh, 479(%rax)
	vzeroupper
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5347:
	.size	CRYPTO_NAMESPACE(crypto_encode_1277x7879), .-CRYPTO_NAMESPACE(crypto_encode_1277x7879)
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
	.long	7879
	.long	7879
	.long	7879
	.long	7879
	.long	7879
	.long	7879
	.long	7879
	.long	7879
	.align 32
.LC3:
	.quad	70364449226751
	.quad	70364449226751
	.quad	70364449226751
	.quad	70364449226751
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
	.long	948
	.long	948
	.long	948
	.long	948
	.long	948
	.long	948
	.long	948
	.long	948
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
	.long	3511
	.long	3511
	.long	3511
	.long	3511
	.long	3511
	.long	3511
	.long	3511
	.long	3511
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
