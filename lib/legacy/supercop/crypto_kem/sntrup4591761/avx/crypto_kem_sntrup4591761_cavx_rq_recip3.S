	.file	"rq_recip3.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(rq_recip3)
	.type	CRYPTO_NAMESPACE(rq_recip3), @function
CRYPTO_NAMESPACE(rq_recip3):
.LFB5293:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	$1538, %edx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	movq	%rsi, %r12
	xorl	%esi, %esi
	pushq	%rbx
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$6240, %rsp
	leaq	3136(%rsp), %rdi
	call	memset@PLT
	vmovdqa	.LC1(%rip), %xmm2
	movq	%rsp, %rsi
	movl	$-1, 4656(%rsp)
	vmovdqa	.LC0(%rip), %ymm3
	leaq	729(%r12), %rcx
	leaq	1472(%rsp), %rdx
	movq	%rsi, %rax
	vpmovsxbw	%xmm2, %ymm2
	.p2align 4,,10
	.p2align 3
.L2:
	vmovdqu	(%rcx), %ymm4
	addq	$64, %rax
	subq	$32, %rcx
	vperm2i128	$1, %ymm4, %ymm4, %ymm0
	vpshufb	%ymm3, %ymm0, %ymm0
	vpmovsxbw	%xmm0, %ymm1
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovsxbw	%xmm0, %ymm0
	vpmullw	%ymm2, %ymm1, %ymm1
	vpmullw	%ymm2, %ymm0, %ymm0
	vmovdqa	%ymm1, -64(%rax)
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L2
	leaq	24(%r12), %rcx
	addq	$1522, %rsi
	.p2align 4,,10
	.p2align 3
.L3:
	movsbw	(%rcx), %ax
	addq	$2, %rdx
	decq	%rcx
	leal	(%rax,%rax,2), %eax
	movw	%ax, -2(%rdx)
	cmpq	%rdx, %rsi
	jne	.L3
	leaq	4688(%rsp), %r12
	vpxor	%xmm0, %xmm0, %xmm0
	movl	$1538, %edx
	xorl	%esi, %esi
	vmovups	%xmm0, 1522(%rsp)
	movq	%r12, %rdi
	vzeroupper
	call	memset@PLT
	movl	$1, %edx
	xorl	%esi, %esi
	movw	%dx, 4688(%rsp)
	leaq	1568(%rsp), %rdi
	movl	$1538, %edx
	call	memset@PLT
	vmovdqa	.LC2(%rip), %ymm4
	movl	$1, %edx
	vmovdqa	.LC3(%rip), %ymm3
	movq	%rax, %rdi
	movl	$1, %esi
	leaq	2(%rsp), %r9
	xorl	%ecx, %ecx
	leaq	3138(%rsp), %r10
	leaq	4674(%rsp), %r11
	.p2align 4,,10
	.p2align 3
.L6:
	movswl	(%rsp), %r8d
	movl	%esi, %r13d
	negl	%r13d
	imull	$58470, %r8d, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%r8d, %eax
	movzwl	%ax, %r8d
	negl	%r8d
	andl	%r13d, %r8d
	xorl	%esi, %r13d
	sarl	$31, %r8d
	andl	%r8d, %r13d
	vmovd	%r8d, %xmm6
	xorl	%r13d, %esi
	movl	%edx, %r13d
	vpbroadcastd	%xmm6, %ymm6
	xorl	%eax, %r13d
	incl	%esi
	movswl	%r13w, %r13d
	andl	%r8d, %r13d
	xorl	%r13d, %edx
	xorl	%r13d, %eax
	vmovd	%edx, %xmm5
	vmovd	%eax, %xmm1
	movw	%dx, 3136(%rsp)
	movq	%r10, %rax
	vpbroadcastw	%xmm5, %ymm5
	vpbroadcastw	%xmm1, %ymm1
	movq	%r9, %rdx
	vpmullw	%ymm4, %ymm1, %ymm7
	vpmullw	%ymm4, %ymm5, %ymm8
	.p2align 4,,10
	.p2align 3
.L4:
	vmovdqu	(%rax), %ymm0
	addq	$32, %rax
	addq	$32, %rdx
	vpblendvb	%ymm6, -32(%rdx), %ymm0, %ymm10
	vmovdqu	-32(%rdx), %ymm0
	vpmullw	%ymm10, %ymm7, %ymm2
	vpmulhw	%ymm1, %ymm10, %ymm9
	vpblendvb	%ymm6, -32(%rax), %ymm0, %ymm0
	vmovdqu	%ymm10, -32(%rax)
	vpmulhw	%ymm5, %ymm0, %ymm11
	vpmullw	%ymm0, %ymm8, %ymm0
	vpmulhw	%ymm3, %ymm2, %ymm2
	vpmulhw	%ymm3, %ymm0, %ymm0
	vpaddw	%ymm11, %ymm2, %ymm2
	vpaddw	%ymm0, %ymm9, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm0
	vmovdqu	%ymm0, -34(%rdx)
	cmpq	%r11, %rax
	jne	.L4
	movl	%ecx, %eax
	leal	1(%rcx), %r8d
	shrl	$4, %ecx
	notl	%eax
	movl	%ecx, %edx
	andl	$15, %eax
	notq	%rdx
	addl	%r8d, %eax
	salq	$5, %rdx
	cltq
	addq	%rax, %rax
	leaq	(%rdi,%rax), %r13
	addq	%r12, %rax
	addq	%r13, %rdx
	.p2align 4,,10
	.p2align 3
.L5:
	vmovdqu	-32(%r13), %ymm2
	vmovdqu	-32(%rax), %ymm0
	subq	$32, %r13
	subq	$32, %rax
	vpblendvb	%ymm6, (%rax), %ymm2, %ymm10
	vpblendvb	%ymm6, %ymm2, %ymm0, %ymm0
	vpmulhw	%ymm5, %ymm0, %ymm11
	vpmullw	%ymm10, %ymm7, %ymm2
	vmovdqu	%ymm10, 2(%r13)
	vpmullw	%ymm0, %ymm8, %ymm0
	vpmulhw	%ymm1, %ymm10, %ymm9
	vpmulhw	%ymm3, %ymm2, %ymm2
	vpmulhw	%ymm3, %ymm0, %ymm0
	vpaddw	%ymm11, %ymm2, %ymm2
	vpaddw	%ymm0, %ymm9, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm0
	vmovdqu	%ymm0, (%rax)
	cmpq	%rdx, %r13
	jne	.L5
	movzwl	3136(%rsp), %edx
	cmpl	$761, %r8d
	je	.L22
	movl	%r8d, %ecx
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L22:
	movl	$760, %r12d
	leaq	6224(%rsp), %rax
	leaq	3104(%rsp), %r13
	.p2align 4,,10
	.p2align 3
.L9:
	movswl	(%rsp), %r8d
	movl	%esi, %r14d
	movl	%r12d, %r11d
	negl	%r14d
	imull	$58470, %r8d, %ecx
	movl	%r14d, %r15d
	xorl	%esi, %r15d
	addl	$134217728, %ecx
	sarl	$28, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%r8d, %ecx
	movzwl	%cx, %r8d
	negl	%r8d
	andl	%r14d, %r8d
	movl	%edx, %r14d
	xorl	%ecx, %r14d
	sarl	$31, %r8d
	movswl	%r14w, %r14d
	andl	%r8d, %r15d
	vmovd	%r8d, %xmm1
	andl	%r8d, %r14d
	xorl	%esi, %r15d
	vpbroadcastd	%xmm1, %ymm1
	xorl	%r14d, %edx
	xorl	%r14d, %ecx
	leal	1(%r15), %esi
	vmovd	%edx, %xmm6
	vmovd	%ecx, %xmm0
	movw	%dx, 3136(%rsp)
	movq	%r9, %rcx
	vpbroadcastw	%xmm6, %ymm6
	vpbroadcastw	%xmm0, %ymm0
	movq	%r10, %rdx
	vpmullw	%ymm4, %ymm0, %ymm7
	vpmullw	%ymm4, %ymm6, %ymm8
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	(%rdx), %ymm5
	subl	$16, %r11d
	addq	$32, %rdx
	addq	$32, %rcx
	vpblendvb	%ymm1, -32(%rcx), %ymm5, %ymm10
	vmovdqu	-32(%rcx), %ymm5
	vpmulhw	%ymm0, %ymm10, %ymm9
	vpblendvb	%ymm1, -32(%rdx), %ymm5, %ymm2
	vpmullw	%ymm10, %ymm7, %ymm5
	vmovdqu	%ymm10, -32(%rdx)
	vpmulhw	%ymm6, %ymm2, %ymm11
	vpmullw	%ymm2, %ymm8, %ymm2
	vpmulhw	%ymm3, %ymm5, %ymm5
	vpmulhw	%ymm3, %ymm2, %ymm2
	vpaddw	%ymm11, %ymm5, %ymm5
	vpaddw	%ymm2, %ymm9, %ymm2
	vpsubw	%ymm2, %ymm5, %ymm2
	vmovdqu	%ymm2, -34(%rcx)
	testl	%r11d, %r11d
	jg	.L7
	movq	%rax, %rcx
	movq	%r13, %rdx
	.p2align 4,,10
	.p2align 3
.L8:
	vmovdqu	-32(%rdx), %ymm5
	subq	$32, %rdx
	subq	$32, %rcx
	vpblendvb	%ymm1, (%rcx), %ymm5, %ymm10
	vmovdqu	(%rcx), %ymm5
	vpmulhw	%ymm0, %ymm10, %ymm9
	vpblendvb	%ymm1, (%rdx), %ymm5, %ymm2
	vpmullw	%ymm10, %ymm7, %ymm5
	vmovdqu	%ymm10, 2(%rdx)
	vpmulhw	%ymm6, %ymm2, %ymm11
	vpmullw	%ymm2, %ymm8, %ymm2
	vpmulhw	%ymm3, %ymm5, %ymm5
	vpmulhw	%ymm3, %ymm2, %ymm2
	vpaddw	%ymm11, %ymm5, %ymm5
	vpaddw	%ymm2, %ymm9, %ymm2
	vpsubw	%ymm2, %ymm5, %ymm2
	vmovdqu	%ymm2, (%rcx)
	cmpq	%rdi, %rdx
	jne	.L8
	movswl	3136(%rsp), %edx
	decl	%r12d
	jne	.L9
	imull	$228, %edx, %eax
	vmovdqa	.LC4(%rip), %ymm8
	vmovdqa	.LC5(%rip), %ymm7
	vmovdqa	.LC6(%rip), %ymm4
	vmovdqa	.LC7(%rip), %ymm3
	vmovdqa	.LC8(%rip), %ymm2
	vmovdqa	.LC9(%rip), %ymm1
	sarl	$20, %eax
	vmovdqa	.LC10(%rip), %ymm6
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	movswl	%dx, %edx
	movl	%edx, %ecx
	movl	%edx, %esi
	imull	%edx, %ecx
	imull	$228, %ecx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%ecx, %eax
	imull	$58470, %eax, %ecx
	addl	$134217728, %ecx
	sarl	$28, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%ecx, %eax
	cwtl
	imull	%eax, %esi
	imull	%eax, %eax
	imull	$228, %esi, %ecx
	imull	$228, %eax, %edi
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	sarl	$20, %edi
	imull	$-4591, %edi, %edi
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%edi, %eax
	imull	$58470, %eax, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %eax
	cwtl
	imull	%eax, %eax
	movl	%eax, %edi
	imull	$228, %eax, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%edi, %eax
	imull	$58470, %eax, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %eax
	cwtl
	imull	%eax, %eax
	movl	%eax, %edi
	imull	$228, %eax, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%edi, %eax
	imull	$58470, %eax, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %eax
	cwtl
	imull	%eax, %eax
	imull	$228, %eax, %edi
	sarl	$20, %edi
	imull	$-4591, %edi, %edi
	addl	%eax, %edi
	leal	(%rcx,%rsi), %eax
	imull	$58470, %edi, %r8d
	cwtl
	addl	$134217728, %r8d
	sarl	$28, %r8d
	imull	$-4591, %r8d, %r8d
	leal	(%rdi,%r8), %ecx
	movswl	%cx, %ecx
	movl	%ecx, %esi
	imull	%eax, %esi
	imull	$228, %esi, %ecx
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	movswl	%cx, %ecx
	movl	%ecx, %esi
	imull	%ecx, %esi
	imull	$228, %esi, %ecx
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	movswl	%cx, %ecx
	imull	%ecx, %ecx
	imull	$228, %ecx, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	movswl	%cx, %ecx
	movl	%ecx, %esi
	imull	%eax, %esi
	imull	$228, %esi, %ecx
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	movswl	%cx, %ecx
	movl	%ecx, %esi
	imull	%ecx, %esi
	imull	$228, %esi, %ecx
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	movswl	%cx, %ecx
	movl	%ecx, %esi
	imull	%ecx, %esi
	imull	$228, %esi, %ecx
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	movswl	%cx, %ecx
	imull	%ecx, %ecx
	imull	$228, %ecx, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	imull	$58470, %ecx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %ecx
	leaq	1556(%rsp), %rsi
	movswl	%cx, %ecx
	imull	%eax, %ecx
	imull	$228, %ecx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%ecx, %eax
	imull	$58470, %eax, %ecx
	addl	$134217728, %ecx
	sarl	$28, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%ecx, %eax
	cwtl
	imull	%eax, %eax
	movl	%eax, %ecx
	imull	$228, %eax, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%ecx, %eax
	imull	$58470, %eax, %ecx
	addl	$134217728, %ecx
	sarl	$28, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%ecx, %eax
	cwtl
	imull	%eax, %eax
	imull	$228, %eax, %ecx
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%ecx, %eax
	imull	$58470, %eax, %ecx
	addl	$134217728, %ecx
	sarl	$28, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%ecx, %eax
	cwtl
	imull	%eax, %edx
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movq	%rbx, %rdx
	vmovd	%eax, %xmm5
	movswl	%ax, %ecx
	leaq	3060(%rsp), %rax
	vpbroadcastw	%xmm5, %ymm5
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rax), %ymm0
	subq	$32, %rax
	addq	$32, %rdx
	vperm2i128	$1, %ymm0, %ymm0, %ymm0
	vpshufb	%ymm8, %ymm0, %ymm0
	vpmulhw	%ymm7, %ymm0, %ymm9
	vpmullw	%ymm0, %ymm7, %ymm11
	vpmovsxwd	%xmm0, %ymm12
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovsxwd	%xmm0, %ymm0
	vpunpcklwd	%ymm9, %ymm11, %ymm10
	vpunpckhwd	%ymm9, %ymm11, %ymm11
	vperm2i128	$32, %ymm11, %ymm10, %ymm9
	vperm2i128	$49, %ymm11, %ymm10, %ymm10
	vpsrad	$20, %ymm9, %ymm9
	vpsrad	$20, %ymm10, %ymm10
	vpmulld	%ymm4, %ymm9, %ymm9
	vpmulld	%ymm4, %ymm10, %ymm10
	vpaddd	%ymm12, %ymm9, %ymm9
	vpaddd	%ymm0, %ymm10, %ymm10
	vpmulld	%ymm3, %ymm10, %ymm11
	vpmulld	%ymm3, %ymm9, %ymm0
	vpand	%ymm10, %ymm1, %ymm10
	vpand	%ymm9, %ymm1, %ymm9
	vpackusdw	%ymm10, %ymm9, %ymm9
	vpermq	$216, %ymm9, %ymm9
	vpaddd	%ymm2, %ymm0, %ymm0
	vpaddd	%ymm2, %ymm11, %ymm11
	vpsrad	$28, %ymm0, %ymm0
	vpsrad	$28, %ymm11, %ymm11
	vpand	%ymm11, %ymm1, %ymm11
	vpand	%ymm0, %ymm1, %ymm0
	vpackusdw	%ymm11, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpmullw	%ymm0, %ymm6, %ymm0
	vpaddw	%ymm9, %ymm0, %ymm0
	vpmullw	%ymm0, %ymm5, %ymm10
	vpmulhw	%ymm0, %ymm5, %ymm0
	vpunpcklwd	%ymm0, %ymm10, %ymm9
	vpunpckhwd	%ymm0, %ymm10, %ymm0
	vperm2i128	$32, %ymm0, %ymm9, %ymm11
	vperm2i128	$49, %ymm0, %ymm9, %ymm10
	vpslld	$3, %ymm10, %ymm9
	vpslld	$3, %ymm11, %ymm0
	vpsubd	%ymm11, %ymm0, %ymm0
	vpsubd	%ymm10, %ymm9, %ymm9
	vpslld	$3, %ymm0, %ymm0
	vpslld	$3, %ymm9, %ymm9
	vpaddd	%ymm11, %ymm0, %ymm0
	vpaddd	%ymm10, %ymm9, %ymm9
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm9, %ymm9
	vpsrad	$20, %ymm0, %ymm0
	vpsrad	$20, %ymm9, %ymm9
	vpmulld	%ymm4, %ymm0, %ymm0
	vpmulld	%ymm4, %ymm9, %ymm9
	vpaddd	%ymm11, %ymm0, %ymm0
	vpaddd	%ymm10, %ymm9, %ymm9
	vpmulld	%ymm3, %ymm9, %ymm11
	vpmulld	%ymm3, %ymm0, %ymm10
	vpand	%ymm9, %ymm1, %ymm9
	vpand	%ymm0, %ymm1, %ymm0
	vpackusdw	%ymm9, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddd	%ymm2, %ymm10, %ymm10
	vpaddd	%ymm2, %ymm11, %ymm11
	vpsrad	$28, %ymm10, %ymm10
	vpsrad	$28, %ymm11, %ymm11
	vpand	%ymm10, %ymm1, %ymm10
	vpand	%ymm11, %ymm1, %ymm11
	vpackusdw	%ymm11, %ymm10, %ymm10
	vpermq	$216, %ymm10, %ymm10
	vpmullw	%ymm10, %ymm6, %ymm10
	vpaddw	%ymm0, %ymm10, %ymm0
	vmovdqu	%ymm0, -32(%rdx)
	cmpq	%rsi, %rax
	jne	.L10
	movswl	1586(%rsp), %edx
	movq	$0, 1522(%rbx)
	movl	$0, 1530(%rbx)
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	cwtl
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	imull	$58470, %eax, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	movw	%ax, 1504(%rbx)
	movswl	1584(%rsp), %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	movswl	%dx, %edx
	imull	%ecx, %edx
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movswl	1582(%rsp), %edx
	movw	%ax, 1506(%rbx)
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	cwtl
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movswl	1580(%rsp), %edx
	movw	%ax, 1508(%rbx)
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	cwtl
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movswl	1578(%rsp), %edx
	movw	%ax, 1510(%rbx)
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	cwtl
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movswl	1576(%rsp), %edx
	movw	%ax, 1512(%rbx)
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	cwtl
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movswl	1574(%rsp), %edx
	movw	%ax, 1514(%rbx)
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	cwtl
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movswl	1572(%rsp), %edx
	movw	%ax, 1516(%rbx)
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	cwtl
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 1518(%rbx)
	movswl	1570(%rsp), %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%eax, %edx
	movswl	%dx, %eax
	imull	%ecx, %eax
	imull	$228, %eax, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%eax, %edx
	imull	$58470, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	movw	%ax, 1520(%rbx)
	xorl	%eax, %eax
	movw	%ax, 1534(%rbx)
	movl	%r15d, %eax
	notl	%eax
	sarl	$31, %eax
	vzeroupper
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5293:
	.size	CRYPTO_NAMESPACE(rq_recip3), .-CRYPTO_NAMESPACE(rq_recip3)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.byte	15
	.byte	14
	.byte	13
	.byte	12
	.byte	11
	.byte	10
	.byte	9
	.byte	8
	.byte	7
	.byte	6
	.byte	5
	.byte	4
	.byte	3
	.byte	2
	.byte	1
	.byte	0
	.byte	15
	.byte	14
	.byte	13
	.byte	12
	.byte	11
	.byte	10
	.byte	9
	.byte	8
	.byte	7
	.byte	6
	.byte	5
	.byte	4
	.byte	3
	.byte	2
	.byte	1
	.byte	0
	.align 32
.LC1:
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.align 32
.LC2:
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.value	15631
	.align 32
.LC3:
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.align 32
.LC4:
	.byte	14
	.byte	15
	.byte	12
	.byte	13
	.byte	10
	.byte	11
	.byte	8
	.byte	9
	.byte	6
	.byte	7
	.byte	4
	.byte	5
	.byte	2
	.byte	3
	.byte	0
	.byte	1
	.byte	14
	.byte	15
	.byte	12
	.byte	13
	.byte	10
	.byte	11
	.byte	8
	.byte	9
	.byte	6
	.byte	7
	.byte	4
	.byte	5
	.byte	2
	.byte	3
	.byte	0
	.byte	1
	.align 32
.LC5:
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.value	228
	.align 32
.LC6:
	.long	-4591
	.long	-4591
	.long	-4591
	.long	-4591
	.long	-4591
	.long	-4591
	.long	-4591
	.long	-4591
	.align 32
.LC7:
	.long	58470
	.long	58470
	.long	58470
	.long	58470
	.long	58470
	.long	58470
	.long	58470
	.long	58470
	.align 32
.LC8:
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.align 32
.LC9:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC10:
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
