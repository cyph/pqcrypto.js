	.file	"swap.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(swap)
	.type	CRYPTO_NAMESPACE(swap), @function
CRYPTO_NAMESPACE(swap):
.LFB5279:
	.cfi_startproc
	vmovd	%ecx, %xmm1
	movl	%ecx, %r9d
	vpbroadcastd	%xmm1, %ymm1
	cmpl	$31, %edx
	jle	.L11
	subl	$32, %edx
	movq	%rsi, %rax
	movl	%edx, %r11d
	shrl	$5, %r11d
	movl	%r11d, %r10d
	incq	%r10
	salq	$5, %r10
	leaq	(%rdi,%r10), %r8
	.p2align 4,,10
	.p2align 3
.L3:
	vmovdqu	(%rdi), %ymm4
	vmovdqu	(%rax), %ymm5
	addq	$32, %rdi
	addq	$32, %rax
	vpblendvb	%ymm1, -32(%rax), %ymm4, %ymm2
	vpblendvb	%ymm1, %ymm4, %ymm5, %ymm0
	vmovdqu	%ymm2, -32(%rdi)
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%r8, %rdi
	jne	.L3
	sall	$5, %r11d
	addq	%r10, %rsi
	subl	%r11d, %edx
.L2:
	testl	%edx, %edx
	jle	.L62
	leaq	15(%r8), %rax
	subq	%rsi, %rax
	cmpq	$30, %rax
	leal	-1(%rdx), %eax
	jbe	.L5
	cmpl	$14, %eax
	jbe	.L5
	vmovdqu	(%r8), %xmm2
	vmovdqu	(%rsi), %xmm0
	vmovd	%r9d, %xmm3
	movl	%edx, %eax
	vpbroadcastb	%xmm3, %xmm3
	andl	$-16, %eax
	vpxor	%xmm0, %xmm2, %xmm1
	movl	%eax, %edi
	vpand	%xmm3, %xmm1, %xmm1
	vpxor	%xmm1, %xmm2, %xmm2
	vpxor	%xmm1, %xmm0, %xmm0
	vmovups	%xmm2, (%r8)
	addq	%rdi, %r8
	vmovups	%xmm0, (%rsi)
	addq	%rdi, %rsi
	movl	%edx, %edi
	subl	%eax, %edi
	cmpl	%eax, %edx
	je	.L62
	movzbl	(%rsi), %eax
	movzbl	(%r8), %edx
	movzbl	(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, (%r8)
	movb	%al, (%rsi)
	cmpl	$1, %edi
	je	.L62
	movzbl	1(%rsi), %eax
	movzbl	1(%r8), %edx
	movzbl	1(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 1(%r8)
	movb	%al, 1(%rsi)
	cmpl	$2, %edi
	je	.L62
	movzbl	2(%rsi), %eax
	movzbl	2(%r8), %edx
	movzbl	2(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 2(%r8)
	movb	%al, 2(%rsi)
	cmpl	$3, %edi
	je	.L62
	movzbl	3(%rsi), %eax
	movzbl	3(%r8), %edx
	movzbl	3(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 3(%r8)
	movb	%al, 3(%rsi)
	cmpl	$4, %edi
	je	.L62
	movzbl	4(%rsi), %eax
	movzbl	4(%r8), %edx
	movzbl	4(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 4(%r8)
	movb	%al, 4(%rsi)
	cmpl	$5, %edi
	je	.L62
	movzbl	5(%rsi), %eax
	movzbl	5(%r8), %edx
	movzbl	5(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 5(%r8)
	movb	%al, 5(%rsi)
	cmpl	$6, %edi
	je	.L62
	movzbl	6(%rsi), %eax
	movzbl	6(%r8), %edx
	movzbl	6(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 6(%r8)
	movb	%al, 6(%rsi)
	cmpl	$7, %edi
	je	.L62
	movzbl	7(%rsi), %eax
	movzbl	7(%r8), %edx
	movzbl	7(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 7(%r8)
	movb	%al, 7(%rsi)
	cmpl	$8, %edi
	je	.L62
	movzbl	8(%rsi), %eax
	movzbl	8(%r8), %edx
	movzbl	8(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 8(%r8)
	movb	%al, 8(%rsi)
	cmpl	$9, %edi
	je	.L62
	movzbl	9(%rsi), %eax
	movzbl	9(%r8), %edx
	movzbl	9(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 9(%r8)
	movb	%al, 9(%rsi)
	cmpl	$10, %edi
	je	.L62
	movzbl	10(%rsi), %eax
	movzbl	10(%r8), %edx
	movzbl	10(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 10(%r8)
	movb	%al, 10(%rsi)
	cmpl	$11, %edi
	je	.L62
	movzbl	11(%rsi), %eax
	movzbl	11(%r8), %edx
	movzbl	11(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 11(%r8)
	movb	%al, 11(%rsi)
	cmpl	$12, %edi
	je	.L62
	movzbl	12(%rsi), %eax
	movzbl	12(%r8), %edx
	movzbl	12(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 12(%r8)
	movb	%al, 12(%rsi)
	cmpl	$13, %edi
	je	.L62
	movzbl	13(%rsi), %eax
	movzbl	13(%r8), %edx
	movzbl	13(%r8), %r9d
	xorl	%eax, %edx
	andl	%ecx, %edx
	xorl	%edx, %r9d
	xorl	%edx, %eax
	movb	%r9b, 13(%r8)
	movb	%al, 13(%rsi)
	cmpl	$14, %edi
	je	.L62
	movzbl	14(%rsi), %eax
	movzbl	14(%r8), %edx
	xorl	%eax, %edx
	andl	%edx, %ecx
	movzbl	14(%r8), %edx
	xorl	%ecx, %eax
	xorl	%ecx, %edx
	movb	%dl, 14(%r8)
	movb	%al, 14(%rsi)
.L62:
	vzeroupper
	ret
	.p2align 4,,10
	.p2align 3
.L5:
	leaq	1(%rsi,%rax), %rdi
	.p2align 4,,10
	.p2align 3
.L9:
	movzbl	(%rsi), %eax
	movzbl	(%r8), %edx
	incq	%rsi
	incq	%r8
	movzbl	-1(%r8), %ecx
	xorl	%eax, %edx
	andl	%r9d, %edx
	xorl	%edx, %ecx
	xorl	%edx, %eax
	movb	%cl, -1(%r8)
	movb	%al, -1(%rsi)
	cmpq	%rdi, %rsi
	jne	.L9
	vzeroupper
	ret
	.p2align 4,,10
	.p2align 3
.L11:
	movq	%rdi, %r8
	jmp	.L2
	.cfi_endproc
.LFE5279:
	.size	CRYPTO_NAMESPACE(swap), .-CRYPTO_NAMESPACE(swap)
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
