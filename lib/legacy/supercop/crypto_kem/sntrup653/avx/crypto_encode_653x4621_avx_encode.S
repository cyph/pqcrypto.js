	.file	"encode.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(crypto_encode_653x4621)
	.type	CRYPTO_NAMESPACE(crypto_encode_653x4621), @function
CRYPTO_NAMESPACE(crypto_encode_653x4621):
.LFB5347:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r9
	movq	%rsi, %rdx
	movl	$20, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$552, %rsp
	.cfi_offset 3, -24
	leaq	-104(%rsp), %rsi
	movq	%rsi, %r8
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
	vmovdqu	%ymm7, (%r8)
	vmovdqu	%ymm0, (%rdi)
	testq	%rcx, %rcx
	je	.L26
	decq	%rcx
	je	.L3
	addq	$64, %rdx
	addq	$32, %r8
	movq	%rax, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	addq	$24, %rdx
	addq	$12, %r8
	addq	$12, %rdi
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L26:
	movzwl	1304(%r9), %edx
	movl	$20, %ecx
	movq	%rsi, %r8
	movq	%rsi, %rdi
	vmovdqa	.LC5(%rip), %ymm2
	vmovdqa	.LC6(%rip), %ymm3
	addw	$2310, %dx
	andw	$16383, %dx
	movw	%dx, 548(%rsp)
	.p2align 4,,10
	.p2align 3
.L6:
	vmovdqu	(%rdi), %ymm6
	vpand	(%rdi), %ymm2, %ymm4
	leaq	8(%rax), %rdx
	vpsrld	$16, %ymm6, %ymm5
	vpslld	$2, %ymm5, %ymm1
	vpaddd	%ymm5, %ymm1, %ymm1
	vpslld	$6, %ymm1, %ymm0
	vpaddd	%ymm0, %ymm1, %ymm0
	vpaddd	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%r8)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rax)
	vpextrd	$2, %xmm0, 4(%rax)
	testq	%rcx, %rcx
	je	.L27
	decq	%rcx
	je	.L7
	addq	$32, %rdi
	addq	$16, %r8
	movq	%rdx, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L7:
	addq	$12, %rdi
	addq	$6, %r8
	addq	$3, %rax
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L27:
	movzwl	548(%rsp), %eax
	movq	%rsi, %rdi
	movq	%rsi, %rcx
	movw	%ax, 222(%rsp)
	movl	$10, %eax
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rcx), %ymm6
	vpand	(%rcx), %ymm2, %ymm1
	leaq	8(%rdx), %r8
	vpsrld	$16, %ymm6, %ymm4
	vpslld	$1, %ymm4, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpslld	$5, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rdi)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%rdx)
	vpextrd	$2, %xmm0, 4(%rdx)
	testq	%rax, %rax
	je	.L28
	decq	%rax
	je	.L11
	addq	$32, %rcx
	addq	$16, %rdi
	movq	%r8, %rdx
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L11:
	addq	$8, %rcx
	addq	$4, %rdi
	addq	$2, %rdx
	jmp	.L10
	.p2align 4,,10
	.p2align 3
.L28:
	vmovdqa	.LC7(%rip), %ymm4
.L14:
	vmovdqu	(%rsi,%rax,4), %ymm6
	vpand	(%rsi,%rax,4), %ymm2, %ymm1
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rsi,%rax,2)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%r8,%rax)
	vpextrd	$2, %xmm0, 4(%r8,%rax)
	addq	$8, %rax
	cmpq	$40, %rax
	jne	.L14
	movzwl	58(%rsp), %eax
	movzwl	56(%rsp), %ecx
	leaq	50(%rdx), %r8
	imull	$676, %eax, %eax
	addl	%ecx, %eax
	movl	$2, %ecx
	movw	%ax, 48(%rdx)
	shrl	$16, %eax
	movq	%rsi, %rdx
	movw	%ax, -24(%rsp)
	movq	%rsi, %rax
.L15:
	vmovdqu	(%rax), %ymm6
	vpand	(%rax), %ymm2, %ymm1
	leaq	8(%r8), %rdi
	vpsrld	$16, %ymm6, %ymm5
	vpslld	$3, %ymm5, %ymm0
	vpsubd	%ymm5, %ymm0, %ymm4
	vpslld	$8, %ymm4, %ymm0
	vpsubd	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%rdx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, (%r8)
	vpextrd	$2, %xmm0, 4(%r8)
	testq	%rcx, %rcx
	je	.L29
	decq	%rcx
	jne	.L30
	addq	$16, %rax
	addq	$8, %rdx
	addq	$4, %r8
	jmp	.L15
	.p2align 4,,10
	.p2align 3
.L29:
	movzwl	-24(%rsp), %r9d
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L19:
	movzwl	2(%rsi,%rdx,4), %eax
	movzwl	(%rsi,%rdx,4), %r10d
	imull	$12461, %eax, %eax
	addl	%r10d, %eax
	movb	%al, (%rdi,%rdx,2)
	movb	%ah, 1(%rdi,%rdx,2)
	shrl	$16, %eax
	movw	%ax, (%rsi,%rdx,2)
	incq	%rdx
	cmpq	$10, %rdx
	jne	.L19
.L20:
	movzwl	2(%rsi,%rcx,4), %eax
	movzwl	(%rsi,%rcx,4), %edx
	imull	$2370, %eax, %eax
	addl	%edx, %eax
	movb	%al, 20(%rdi,%rcx,2)
	movb	%ah, 21(%rdi,%rcx,2)
	shrl	$16, %eax
	movw	%ax, (%rsi,%rcx,2)
	incq	%rcx
	cmpq	$5, %rcx
	jne	.L20
	movzwl	%r9w, %r9d
	imulw	$86, -102(%rsp), %dx
	addw	-104(%rsp), %dx
	imull	$86, %r9d, %r9d
	movzwl	%dx, %ecx
	movzwl	-96(%rsp), %eax
	imulw	$86, -98(%rsp), %dx
	addw	-100(%rsp), %dx
	movzwl	%dx, %edx
	imull	$7396, %edx, %edx
	addl	%r9d, %eax
	movzwl	%ax, %eax
	imull	$835, %eax, %eax
	addl	%ecx, %edx
	movw	%dx, 38(%r8)
	shrl	$16, %edx
	addl	%edx, %eax
	movw	%ax, 40(%r8)
	shrl	$16, %eax
	movb	%al, 42(%r8)
	vzeroupper
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L30:
	.cfi_restore_state
	addq	$32, %rax
	addq	$16, %rdx
	movq	%rdi, %r8
	jmp	.L15
	.cfi_endproc
.LFE5347:
	.size	CRYPTO_NAMESPACE(crypto_encode_653x4621), .-CRYPTO_NAMESPACE(crypto_encode_653x4621)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
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
	.long	4621
	.long	4621
	.long	4621
	.long	4621
	.long	4621
	.long	4621
	.long	4621
	.long	4621
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
	.long	676
	.long	676
	.long	676
	.long	676
	.long	676
	.long	676
	.long	676
	.long	676
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
