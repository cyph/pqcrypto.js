	.file	"rq_recip3.c"
	.text
	.p2align 4,,15
	.globl	rq_recip3
	.type	rq_recip3, @function
rq_recip3:
.LFB5184:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	$1538, %edx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	.cfi_offset 13, -24
	movq	%rsi, %r13
	xorl	%esi, %esi
	pushq	%r12
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$6240, %rsp
	.cfi_offset 3, -40
	leaq	3136(%rsp), %rdi
	call	memset
	movl	$1, %ecx
	leaq	729(%r13), %r8
	movw	%cx, 3136(%rsp)
	movl	$-1, 4656(%rsp)
	andl	$31, %r8d
	je	.L14
	leal	-1(%r8), %ecx
	addq	$2, %rcx
	movl	$1, %eax
	movq	%rsp, %rbx
	movl	$761, %esi
	.p2align 4,,10
	.p2align 3
.L3:
	movq	%rax, %rdx
	negq	%rdx
	movsbw	761(%r13,%rdx), %dx
	movl	%esi, %edi
	leal	(%rdx,%rdx,2), %edx
	movw	%dx, -2(%rbx,%rax,2)
	subl	%eax, %edi
	movl	%eax, %edx
	incq	%rax
	cmpq	%rax, %rcx
	jne	.L3
.L2:
	movl	$761, %eax
	movl	%r8d, %ecx
	subl	%r8d, %eax
	movq	%r13, %rsi
	movl	%eax, %r8d
	subq	%rcx, %rsi
	leaq	(%rbx,%rcx,2), %rax
	movq	%rsi, %rcx
	movl	%r8d, %esi
	shrl	$5, %esi
	vmovdqa	.LC1(%rip), %xmm2
	salq	$6, %rsi
	vmovdqa	.LC0(%rip), %ymm3
	addq	$729, %rcx
	addq	%rax, %rsi
	vpmovsxbw	%xmm2, %ymm2
	.p2align 4,,10
	.p2align 3
.L4:
	vmovdqa	(%rcx), %ymm0
	addq	$64, %rax
	vperm2i128	$1, %ymm0, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm0, %ymm0
	vpmovsxbw	%xmm0, %ymm1
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovsxbw	%xmm0, %ymm0
	vpmullw	%ymm2, %ymm1, %ymm1
	vpmullw	%ymm2, %ymm0, %ymm0
	subq	$32, %rcx
	vmovdqu	%ymm1, -64(%rax)
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rax, %rsi
	jne	.L4
	movl	%r8d, %eax
	andl	$-32, %eax
	movl	%eax, %ecx
	addl	%eax, %edx
	notl	%ecx
	cmpl	%r8d, %eax
	je	.L5
	movl	$760, %eax
	subl	%edx, %eax
	cltq
	leaq	-1(%r13,%rax), %rsi
	addl	%edi, %ecx
	addq	%r13, %rax
	subq	%rcx, %rsi
	leal	(%rdx,%rax), %edi
	.p2align 4,,10
	.p2align 3
.L6:
	movsbw	(%rax), %dx
	movl	%edi, %ecx
	subl	%eax, %ecx
	movslq	%ecx, %rcx
	leal	(%rdx,%rdx,2), %edx
	decq	%rax
	movw	%dx, (%rsp,%rcx,2)
	cmpq	%rax, %rsi
	jne	.L6
.L5:
	vpxor	%xmm0, %xmm0, %xmm0
	movl	$1538, %edx
	xorl	%esi, %esi
	leaq	4688(%rsp), %rdi
	vmovups	%xmm0, 1522(%rsp)
	vzeroupper
	call	memset
	leaq	1568(%rsp), %rcx
	movl	$1, %edx
	movq	%rcx, %rdi
	movw	%dx, 4688(%rsp)
	xorl	%esi, %esi
	movl	$1538, %edx
	call	memset
	vmovdqa	.LC2(%rip), %ymm4
	vmovdqa	.LC3(%rip), %ymm3
	movq	%rax, %rcx
	movl	$1, %edx
	movl	$1, %edi
	xorl	%esi, %esi
	leaq	4674(%rsp), %r8
	.p2align 4,,10
	.p2align 3
.L9:
	movswl	(%rsp), %r9d
	movl	%edi, %r10d
	imull	$58470, %r9d, %eax
	negl	%r10d
	addl	$134217728, %eax
	sarl	$28, %eax
	imull	$-4591, %eax, %eax
	addl	%r9d, %eax
	movzwl	%ax, %r9d
	negl	%r9d
	andl	%r10d, %r9d
	sarl	$31, %r9d
	xorl	%edi, %r10d
	andl	%r9d, %r10d
	xorl	%r10d, %edi
	movl	%edx, %r10d
	xorl	%eax, %r10d
	movswl	%r10w, %r10d
	andl	%r9d, %r10d
	xorl	%r10d, %edx
	xorl	%r10d, %eax
	vmovd	%edx, %xmm5
	vmovd	%eax, %xmm0
	vpbroadcastw	%xmm5, %ymm5
	vpbroadcastw	%xmm0, %ymm0
	vpmullw	%ymm4, %ymm0, %ymm6
	vpmullw	%ymm4, %ymm5, %ymm7
	vmovd	%r9d, %xmm1
	movw	%dx, 3136(%rsp)
	incl	%edi
	vpbroadcastd	%xmm1, %ymm1
	leaq	2(%rbx), %rdx
	leaq	3138(%rsp), %rax
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	(%rax), %ymm8
	vmovdqu	(%rdx), %ymm2
	addq	$32, %rax
	vpblendvb	%ymm1, %ymm2, %ymm8, %ymm10
	vpblendvb	%ymm1, %ymm8, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm2, %ymm11
	vpmullw	%ymm10, %ymm6, %ymm8
	vpmullw	%ymm2, %ymm7, %ymm2
	vpmulhw	%ymm0, %ymm10, %ymm9
	vmovdqu	%ymm10, -32(%rax)
	addq	$32, %rdx
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpmulhw	%ymm3, %ymm2, %ymm2
	vpaddw	%ymm11, %ymm8, %ymm8
	vpaddw	%ymm2, %ymm9, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vmovdqu	%ymm2, -34(%rdx)
	cmpq	%r8, %rax
	jne	.L7
	leal	1(%rsi), %r9d
	movl	%r9d, %eax
	negl	%eax
	andl	$15, %eax
	addl	%r9d, %eax
	cltq
	addq	%rax, %rax
	leaq	4688(%rsp), %rdx
	shrl	$4, %esi
	leaq	(%rcx,%rax), %r10
	addq	%rdx, %rax
	movl	%esi, %edx
	notq	%rdx
	salq	$5, %rdx
	addq	%r10, %rdx
	.p2align 4,,10
	.p2align 3
.L8:
	subq	$32, %r10
	subq	$32, %rax
	vmovdqu	(%r10), %ymm8
	vmovdqu	(%rax), %ymm2
	vpblendvb	%ymm1, %ymm2, %ymm8, %ymm10
	vpblendvb	%ymm1, %ymm8, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm2, %ymm11
	vpmullw	%ymm10, %ymm6, %ymm8
	vpmullw	%ymm2, %ymm7, %ymm2
	vpmulhw	%ymm0, %ymm10, %ymm9
	vmovdqu	%ymm10, 2(%r10)
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpmulhw	%ymm3, %ymm2, %ymm2
	vpaddw	%ymm11, %ymm8, %ymm8
	vpaddw	%ymm2, %ymm9, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vmovdqu	%ymm2, (%rax)
	cmpq	%rdx, %r10
	jne	.L8
	movzwl	3136(%rsp), %edx
	movl	%r9d, %esi
	cmpl	$761, %r9d
	jne	.L9
	movl	$760, %r10d
	.p2align 4,,10
	.p2align 3
.L12:
	movswl	(%rsp), %eax
	movl	%r10d, %r9d
	imull	$58470, %eax, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%eax, %esi
	movzwl	%si, %r8d
	movl	%edi, %eax
	negl	%eax
	negl	%r8d
	andl	%eax, %r8d
	xorl	%edi, %eax
	movl	%eax, %r11d
	movl	%edx, %eax
	xorl	%esi, %eax
	sarl	$31, %r8d
	cwtl
	andl	%r8d, %eax
	xorl	%eax, %edx
	xorl	%eax, %esi
	vmovd	%edx, %xmm5
	vmovd	%esi, %xmm0
	vpbroadcastw	%xmm5, %ymm5
	vpbroadcastw	%xmm0, %ymm0
	vpmullw	%ymm4, %ymm0, %ymm7
	vpmullw	%ymm4, %ymm5, %ymm6
	andl	%r8d, %r11d
	xorl	%edi, %r11d
	vmovd	%r8d, %xmm1
	movw	%dx, 3136(%rsp)
	leal	1(%r11), %edi
	vpbroadcastd	%xmm1, %ymm1
	leaq	2(%rbx), %rdx
	leaq	3138(%rsp), %rax
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	(%rax), %ymm8
	vmovdqu	(%rdx), %ymm2
	subl	$16, %r9d
	vpblendvb	%ymm1, %ymm2, %ymm8, %ymm10
	vpblendvb	%ymm1, %ymm8, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm2, %ymm11
	vpmullw	%ymm10, %ymm7, %ymm8
	vpmullw	%ymm2, %ymm6, %ymm2
	vpmulhw	%ymm0, %ymm10, %ymm9
	vmovdqu	%ymm10, (%rax)
	addq	$32, %rdx
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpmulhw	%ymm3, %ymm2, %ymm2
	addq	$32, %rax
	vpaddw	%ymm11, %ymm8, %ymm8
	vpaddw	%ymm2, %ymm9, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vmovdqu	%ymm2, -34(%rdx)
	testl	%r9d, %r9d
	jg	.L10
	leaq	6224(%rsp), %rdx
	leaq	1536(%rcx), %rax
	.p2align 4,,10
	.p2align 3
.L11:
	subq	$32, %rax
	subq	$32, %rdx
	vmovdqu	(%rax), %ymm8
	vmovdqu	(%rdx), %ymm2
	vpblendvb	%ymm1, %ymm2, %ymm8, %ymm10
	vpblendvb	%ymm1, %ymm8, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm2, %ymm11
	vpmullw	%ymm10, %ymm7, %ymm8
	vpmullw	%ymm2, %ymm6, %ymm2
	vpmulhw	%ymm0, %ymm10, %ymm9
	vmovdqu	%ymm10, 2(%rax)
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpmulhw	%ymm3, %ymm2, %ymm2
	vpaddw	%ymm11, %ymm8, %ymm8
	vpaddw	%ymm2, %ymm9, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vmovdqu	%ymm2, (%rdx)
	cmpq	%rcx, %rax
	jne	.L11
	movswl	3136(%rsp), %edx
	decl	%r10d
	jne	.L12
	imull	$228, %edx, %ecx
	vmovdqa	.LC4(%rip), %ymm7
	vmovdqa	.LC5(%rip), %ymm6
	sarl	$20, %ecx
	imull	$-4591, %ecx, %ecx
	vmovdqa	.LC6(%rip), %ymm1
	vmovdqa	.LC7(%rip), %ymm4
	addl	%ecx, %edx
	imull	$58470, %edx, %ecx
	vmovdqa	.LC8(%rip), %ymm3
	vmovdqa	.LC9(%rip), %ymm2
	addl	$134217728, %ecx
	sarl	$28, %ecx
	imull	$-4591, %ecx, %ecx
	addl	%ecx, %edx
	movswl	%dx, %ecx
	movl	%ecx, %esi
	imull	%ecx, %esi
	movl	%ecx, %edi
	imull	$228, %esi, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%esi, %edx
	imull	$58470, %edx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %edx
	movswl	%dx, %edx
	imull	%edx, %edi
	imull	%edx, %edx
	imull	$228, %edi, %esi
	imull	$228, %edx, %r8d
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	sarl	$20, %r8d
	imull	$-4591, %r8d, %r8d
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%r8d, %edx
	imull	$58470, %edx, %r8d
	addl	$134217728, %r8d
	sarl	$28, %r8d
	imull	$-4591, %r8d, %r8d
	addl	%r8d, %edx
	movswl	%dx, %edx
	imull	%edx, %edx
	movl	%edx, %r8d
	imull	$228, %edx, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%r8d, %edx
	imull	$58470, %edx, %r8d
	addl	$134217728, %r8d
	sarl	$28, %r8d
	imull	$-4591, %r8d, %r8d
	addl	%r8d, %edx
	movswl	%dx, %edx
	imull	%edx, %edx
	movl	%edx, %r8d
	imull	$228, %edx, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%r8d, %edx
	imull	$58470, %edx, %r8d
	addl	$134217728, %r8d
	sarl	$28, %r8d
	imull	$-4591, %r8d, %r8d
	addl	%r8d, %edx
	movswl	%dx, %edx
	imull	%edx, %edx
	imull	$228, %edx, %r8d
	sarl	$20, %r8d
	imull	$-4591, %r8d, %r8d
	addl	%edx, %r8d
	imull	$58470, %r8d, %r9d
	leal	(%rsi,%rdi), %edx
	movswl	%dx, %edx
	addl	$134217728, %r9d
	sarl	$28, %r9d
	imull	$-4591, %r9d, %r9d
	leal	(%r8,%r9), %esi
	movswl	%si, %esi
	movl	%esi, %edi
	imull	%edx, %edi
	imull	$228, %edi, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	movswl	%si, %esi
	movl	%esi, %edi
	imull	%esi, %edi
	imull	$228, %edi, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	movswl	%si, %esi
	imull	%esi, %esi
	imull	$228, %esi, %edi
	sarl	$20, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	movswl	%si, %esi
	movl	%esi, %edi
	imull	%edx, %edi
	imull	$228, %edi, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	movswl	%si, %esi
	movl	%esi, %edi
	imull	%esi, %edi
	imull	$228, %edi, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	movswl	%si, %esi
	movl	%esi, %edi
	imull	%esi, %edi
	imull	$228, %edi, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	movswl	%si, %esi
	imull	%esi, %esi
	imull	$228, %esi, %edi
	sarl	$20, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	imull	$58470, %esi, %edi
	addl	$134217728, %edi
	sarl	$28, %edi
	imull	$-4591, %edi, %edi
	addl	%edi, %esi
	movswl	%si, %esi
	imull	%edx, %esi
	imull	$228, %esi, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%esi, %edx
	imull	$58470, %edx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %edx
	movswl	%dx, %edx
	movl	%edx, %esi
	imull	%edx, %esi
	imull	$228, %esi, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%esi, %edx
	imull	$58470, %edx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %edx
	movswl	%dx, %edx
	imull	%edx, %edx
	imull	$228, %edx, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addq	$1492, %rax
	addl	%esi, %edx
	imull	$58470, %edx, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %edx
	movswl	%dx, %edx
	imull	%edx, %ecx
	leaq	1556(%rsp), %rsi
	imull	$228, %ecx, %edx
	sarl	$20, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %ecx
	imull	$58470, %ecx, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	addl	%ecx, %edx
	vmovd	%edx, %xmm5
	movswl	%dx, %ecx
	vpbroadcastw	%xmm5, %ymm5
	movq	%r12, %rdx
	.p2align 4,,10
	.p2align 3
.L13:
	vmovdqu	(%rax), %ymm8
	subq	$32, %rax
	vperm2i128	$1, %ymm8, %ymm8, %ymm8
	vpshufb	%ymm7, %ymm8, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm0
	vpmullw	%ymm8, %ymm6, %ymm11
	vpmovsxwd	%xmm8, %ymm10
	vextracti128	$0x1, %ymm8, %xmm8
	vpmovsxwd	%xmm8, %ymm8
	addq	$32, %rdx
	vpunpcklwd	%ymm0, %ymm11, %ymm9
	vpunpckhwd	%ymm0, %ymm11, %ymm11
	vperm2i128	$32, %ymm11, %ymm9, %ymm0
	vperm2i128	$49, %ymm11, %ymm9, %ymm9
	vpsrad	$20, %ymm0, %ymm0
	vpsrad	$20, %ymm9, %ymm9
	vpmulld	%ymm1, %ymm0, %ymm0
	vpmulld	%ymm1, %ymm9, %ymm9
	vpaddd	%ymm10, %ymm0, %ymm10
	vpaddd	%ymm8, %ymm9, %ymm9
	vpmulld	%ymm4, %ymm10, %ymm0
	vpmulld	%ymm4, %ymm9, %ymm8
	vpaddd	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm8, %ymm8
	vpsrad	$28, %ymm0, %ymm0
	vpsrad	$28, %ymm8, %ymm8
	vpmulld	%ymm1, %ymm0, %ymm0
	vpmulld	%ymm1, %ymm8, %ymm8
	vpaddd	%ymm10, %ymm0, %ymm0
	vpaddd	%ymm9, %ymm8, %ymm8
	vpand	%ymm8, %ymm2, %ymm8
	vpand	%ymm0, %ymm2, %ymm0
	vpackusdw	%ymm8, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpmullw	%ymm0, %ymm5, %ymm10
	vpmulhw	%ymm0, %ymm5, %ymm0
	vpunpcklwd	%ymm0, %ymm10, %ymm8
	vpunpckhwd	%ymm0, %ymm10, %ymm0
	vperm2i128	$32, %ymm0, %ymm8, %ymm9
	vperm2i128	$49, %ymm0, %ymm8, %ymm10
	vpslld	$3, %ymm9, %ymm8
	vpsubd	%ymm9, %ymm8, %ymm8
	vpslld	$3, %ymm8, %ymm8
	vpaddd	%ymm9, %ymm8, %ymm8
	vpslld	$2, %ymm8, %ymm8
	vpsrad	$20, %ymm8, %ymm8
	vpmulld	%ymm1, %ymm8, %ymm8
	vpslld	$3, %ymm10, %ymm0
	vpsubd	%ymm10, %ymm0, %ymm0
	vpslld	$3, %ymm0, %ymm0
	vpaddd	%ymm10, %ymm0, %ymm0
	vpaddd	%ymm9, %ymm8, %ymm9
	vpmulld	%ymm4, %ymm9, %ymm8
	vpslld	$2, %ymm0, %ymm0
	vpsrad	$20, %ymm0, %ymm0
	vpmulld	%ymm1, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm8, %ymm8
	vpsrad	$28, %ymm8, %ymm8
	vpmulld	%ymm1, %ymm8, %ymm8
	vpaddd	%ymm10, %ymm0, %ymm0
	vpaddd	%ymm9, %ymm8, %ymm8
	vpmulld	%ymm4, %ymm0, %ymm9
	vpaddd	%ymm3, %ymm9, %ymm9
	vpsrad	$28, %ymm9, %ymm9
	vpmulld	%ymm1, %ymm9, %ymm9
	vpaddd	%ymm0, %ymm9, %ymm9
	vpand	%ymm8, %ymm2, %ymm0
	vpand	%ymm9, %ymm2, %ymm8
	vpackusdw	%ymm8, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rdx)
	cmpq	%rsi, %rax
	jne	.L13
	movswl	1586(%rsp), %edx
	movq	$0, 1522(%r12)
	imull	$228, %edx, %eax
	movl	$0, 1530(%r12)
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
	movw	%ax, 1504(%r12)
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
	movw	%ax, 1506(%r12)
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
	movw	%ax, 1508(%r12)
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
	movw	%ax, 1510(%r12)
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
	movw	%ax, 1512(%r12)
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
	movw	%ax, 1514(%r12)
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
	movw	%ax, 1516(%r12)
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
	movw	%ax, 1518(%r12)
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
	movw	%ax, 1520(%r12)
	xorl	%eax, %eax
	movw	%ax, 1534(%r12)
	movl	%r11d, %eax
	notl	%eax
	sarl	$31, %eax
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L14:
	.cfi_restore_state
	movl	$761, %edi
	xorl	%edx, %edx
	movq	%rsp, %rbx
	jmp	.L2
	.cfi_endproc
.LFE5184:
	.size	rq_recip3, .-rq_recip3
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
	.ident	"GCC: (GNU) 8.2.1 20181105 (Red Hat 8.2.1-5)"
	.section	.note.GNU-stack,"",@progbits
