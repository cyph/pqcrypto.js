	.file	"core.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACETOP
	.type	CRYPTO_NAMESPACETOP, @function
CRYPTO_NAMESPACETOP:
.LFB5296:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	xorl	%ecx, %ecx
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	.cfi_offset 3, -40
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$5568, %rsp
	movzbl	1400(%rsi), %eax
	vmovdqa	.LC2(%rip), %ymm1
	vmovdqa	.LC3(%rip), %ymm3
	vmovdqa	.LC4(%rip), %ymm5
	leaq	864(%rsp), %rdi
	notl	%eax
	movl	%eax, %r8d
	andl	$3, %eax
	leal	-3(%rax), %edx
	andl	$3, %r8d
	movl	%edx, %eax
	sarl	$5, %eax
	andl	%r8d, %eax
	leal	1(%rax), %r8d
	vmovd	%r8d, %xmm6
	vpbroadcastb	%xmm6, %ymm6
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	32(%rsi,%rcx,2), %ymm1, %ymm2
	vpand	(%rsi,%rcx,2), %ymm1, %ymm0
	vpackuswb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm3, %ymm0
	vpaddb	%ymm6, %ymm0, %ymm0
	vextracti128	$0x1, %ymm0, %xmm7
	vpmovsxbw	%xmm0, %ymm2
	vpand	%ymm3, %ymm0, %ymm0
	vpmovsxbw	%xmm7, %ymm7
	vpsraw	$2, %ymm2, %ymm2
	vpsraw	$2, %ymm7, %ymm7
	vpand	%ymm2, %ymm1, %ymm2
	vpand	%ymm7, %ymm1, %ymm7
	vpackuswb	%ymm7, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm0, %ymm0
	vpaddb	%ymm5, %ymm0, %ymm7
	vpmovsxbw	%xmm7, %ymm2
	vextracti128	$0x1, %ymm7, %xmm7
	vpmovsxbw	%xmm7, %ymm7
	vpsraw	$5, %ymm2, %ymm2
	vpsraw	$5, %ymm7, %ymm7
	vpand	%ymm2, %ymm1, %ymm2
	vpand	%ymm7, %ymm1, %ymm7
	vpackuswb	%ymm7, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm2, %ymm0, %ymm0
	vpaddb	%ymm4, %ymm0, %ymm0
	vmovdqa	%ymm0, (%rdi,%rcx)
	addq	$32, %rcx
	cmpq	$672, %rcx
	jne	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	movzbl	(%rsi,%rcx,2), %eax
	andl	$3, %eax
	addl	%r8d, %eax
	movl	%eax, %edx
	sarb	$2, %al
	andl	$3, %edx
	addl	%edx, %eax
	movsbl	%al, %edx
	subl	$3, %edx
	sarl	$5, %edx
	andl	%edx, %eax
	decl	%eax
	movb	%al, (%rdi,%rcx)
	incq	%rcx
	cmpq	$700, %rcx
	jne	.L3
	leaq	4704(%rsp), %rcx
	leaq	1600(%rsp), %rdx
	movq	%rcx, %rax
	leaq	5472(%rsp), %rsi
	.p2align 4,,10
	.p2align 3
.L4:
	vmovdqa	(%rdx), %ymm5
	addq	$32, %rax
	subq	$32, %rdx
	vperm2i128	$1, %ymm5, %ymm5, %ymm0
	vpshufb	.LC5(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rsi, %rax
	jne	.L4
	vpxor	%xmm0, %xmm0, %xmm0
	vmovdqa	.LC6(%rip), %ymm3
	xorl	%eax, %eax
	movl	$0, 5536(%rsp)
	vmovaps	%xmm0, 5472(%rsp)
	leaq	4772(%rsp), %r8
	leaq	3168(%rsp), %rdi
	vmovaps	%xmm0, 5488(%rsp)
	leaq	3936(%rsp), %r9
	vmovaps	%xmm0, 5504(%rsp)
	vmovaps	%xmm0, 5520(%rsp)
	.p2align 4,,10
	.p2align 3
.L5:
	vmovdqu	(%r8,%rax), %ymm2
	vpand	%ymm3, %ymm2, %ymm0
	vmovdqa	%ymm0, (%rdi,%rax)
	vpmovsxbw	%xmm2, %ymm0
	vextracti128	$0x1, %ymm2, %xmm2
	vpmovsxbw	%xmm2, %ymm2
	vpsraw	$1, %ymm0, %ymm0
	vpsraw	$1, %ymm2, %ymm2
	vpand	%ymm0, %ymm1, %ymm0
	vpand	%ymm2, %ymm1, %ymm2
	vpackuswb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm3, %ymm0
	vmovdqa	%ymm0, (%r9,%rax)
	addq	$32, %rax
	cmpq	$768, %rax
	jne	.L5
	vmovdqa	3168(%rsp), %ymm5
	movl	$256, %r12d
	movl	$-1, %eax
	vpunpckldq	3200(%rsp), %ymm5, %ymm7
	vpunpckhdq	3200(%rsp), %ymm5, %ymm1
	vmovdqa	3296(%rsp), %ymm4
	vmovdqa	3232(%rsp), %ymm5
	vpunpckhdq	3328(%rsp), %ymm4, %ymm0
	vpunpckhdq	3264(%rsp), %ymm5, %ymm3
	vpslld	$2, %ymm1, %ymm1
	vmovdqa	3360(%rsp), %ymm2
	vpunpckhdq	3392(%rsp), %ymm2, %ymm2
	vpslld	$2, %ymm0, %ymm0
	vpor	%ymm7, %ymm1, %ymm1
	vmovdqa	3360(%rsp), %ymm4
	vpunpckldq	3264(%rsp), %ymm5, %ymm6
	vpslld	$2, %ymm3, %ymm3
	vpunpckldq	3392(%rsp), %ymm4, %ymm4
	vmovdqa	3296(%rsp), %ymm5
	vpslld	$2, %ymm2, %ymm2
	vpunpckldq	3328(%rsp), %ymm5, %ymm5
	vpor	%ymm6, %ymm3, %ymm3
	vpor	%ymm4, %ymm2, %ymm2
	vpor	%ymm5, %ymm0, %ymm0
	vpunpcklqdq	%ymm3, %ymm1, %ymm4
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpunpcklqdq	%ymm2, %ymm0, %ymm3
	vpslld	$1, %ymm1, %ymm1
	vpunpckhqdq	%ymm2, %ymm0, %ymm0
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm4, %ymm1
	vpor	%ymm0, %ymm3, %ymm0
	vmovdqa	3552(%rsp), %ymm3
	vperm2i128	$32, %ymm0, %ymm1, %ymm4
	vperm2i128	$49, %ymm0, %ymm1, %ymm1
	vpslld	$4, %ymm1, %ymm0
	vpor	%ymm0, %ymm4, %ymm4
	vpunpckhdq	3584(%rsp), %ymm3, %ymm0
	vmovdqa	3616(%rsp), %ymm3
	vpshufb	.LC7(%rip), %ymm4, %ymm4
	vpunpckldq	3648(%rsp), %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpslld	$2, %ymm0, %ymm0
	vpshufd	$216, %ymm4, %ymm5
	vmovdqa	3616(%rsp), %ymm4
	vpunpckhdq	3648(%rsp), %ymm4, %ymm4
	vmovdqa	%ymm5, %ymm14
	vmovdqa	3424(%rsp), %ymm5
	vpunpckldq	3456(%rsp), %ymm5, %ymm7
	vpunpckhdq	3456(%rsp), %ymm5, %ymm1
	vpslld	$2, %ymm4, %ymm4
	vmovdqa	3488(%rsp), %ymm5
	vpunpckhdq	3520(%rsp), %ymm5, %ymm2
	vpor	%ymm3, %ymm4, %ymm4
	vpunpckldq	3520(%rsp), %ymm5, %ymm6
	vmovdqa	3552(%rsp), %ymm5
	vpslld	$2, %ymm1, %ymm1
	vpunpckldq	3584(%rsp), %ymm5, %ymm5
	vpslld	$2, %ymm2, %ymm2
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm6, %ymm2, %ymm2
	vpor	%ymm5, %ymm0, %ymm0
	vpunpcklqdq	%ymm2, %ymm1, %ymm3
	vpunpckhqdq	%ymm2, %ymm1, %ymm1
	vpunpcklqdq	%ymm4, %ymm0, %ymm2
	vpunpckhqdq	%ymm4, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm3, %ymm1
	vpor	%ymm0, %ymm2, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm5
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm5, %ymm5
	vpshufb	.LC7(%rip), %ymm5, %ymm5
	vpermq	$216, %ymm5, %ymm5
	vpshufd	$216, %ymm5, %ymm1
	vmovdqa	%ymm1, %ymm13
	vmovdqa	3680(%rsp), %ymm1
	vpunpckldq	3712(%rsp), %ymm1, %ymm7
	vpunpckhdq	3712(%rsp), %ymm1, %ymm2
	vmovdqa	3744(%rsp), %ymm5
	vmovdqa	3872(%rsp), %ymm4
	vpunpckldq	3776(%rsp), %ymm5, %ymm6
	vpunpckhdq	3776(%rsp), %ymm5, %ymm3
	vpslld	$2, %ymm2, %ymm2
	vmovdqa	3808(%rsp), %ymm5
	vpunpckhdq	3904(%rsp), %ymm4, %ymm4
	vpor	%ymm2, %ymm7, %ymm2
	vpunpckhdq	3840(%rsp), %ymm5, %ymm0
	vpunpckldq	3840(%rsp), %ymm5, %ymm1
	vpslld	$2, %ymm3, %ymm3
	vmovdqa	3872(%rsp), %ymm5
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm4, %ymm4
	vpor	%ymm3, %ymm6, %ymm3
	vpunpckldq	3904(%rsp), %ymm5, %ymm5
	vpor	%ymm0, %ymm1, %ymm1
	vpunpcklqdq	%ymm3, %ymm2, %ymm0
	vpunpckhqdq	%ymm3, %ymm2, %ymm2
	vpor	%ymm4, %ymm5, %ymm4
	vpslld	$1, %ymm2, %ymm2
	vmovdqa	4000(%rsp), %ymm5
	vpunpckldq	4032(%rsp), %ymm5, %ymm7
	vpunpcklqdq	%ymm4, %ymm1, %ymm3
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vpor	%ymm2, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpor	%ymm1, %ymm3, %ymm1
	vmovdqa	4128(%rsp), %ymm3
	vpunpckhdq	4160(%rsp), %ymm3, %ymm3
	vperm2i128	$32, %ymm1, %ymm0, %ymm12
	vperm2i128	$49, %ymm1, %ymm0, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm0, %ymm12, %ymm12
	vpshufb	.LC7(%rip), %ymm12, %ymm12
	vpermq	$216, %ymm12, %ymm12
	vpshufd	$216, %ymm12, %ymm1
	vmovdqa	%ymm1, %ymm12
	vmovdqa	3936(%rsp), %ymm1
	vpunpckldq	3968(%rsp), %ymm1, %ymm4
	vpunpckhdq	3968(%rsp), %ymm1, %ymm2
	vpunpckhdq	4032(%rsp), %ymm5, %ymm1
	vmovdqa	4064(%rsp), %ymm5
	vpunpckhdq	4096(%rsp), %ymm5, %ymm0
	vpunpckldq	4096(%rsp), %ymm5, %ymm6
	vpslld	$2, %ymm2, %ymm2
	vmovdqa	4128(%rsp), %ymm5
	vpslld	$2, %ymm1, %ymm1
	vpslld	$2, %ymm0, %ymm0
	vpor	%ymm4, %ymm2, %ymm4
	vpunpckldq	4160(%rsp), %ymm5, %ymm5
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm6, %ymm0, %ymm0
	vpunpcklqdq	%ymm1, %ymm4, %ymm2
	vpunpckhqdq	%ymm1, %ymm4, %ymm1
	vpor	%ymm5, %ymm3, %ymm3
	vpslld	$1, %ymm1, %ymm1
	vmovdqa	4256(%rsp), %ymm5
	vpunpckldq	4288(%rsp), %ymm5, %ymm6
	vpunpcklqdq	%ymm3, %ymm0, %ymm4
	vpunpckhqdq	%ymm3, %ymm0, %ymm0
	vpor	%ymm1, %ymm2, %ymm1
	vpunpckhdq	4288(%rsp), %ymm5, %ymm3
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm0, %ymm4, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm8
	vpslld	$2, %ymm3, %ymm3
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vmovdqa	4192(%rsp), %ymm1
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm6, %ymm3, %ymm3
	vpunpckldq	4224(%rsp), %ymm1, %ymm7
	vpunpckhdq	4224(%rsp), %ymm1, %ymm1
	vpor	%ymm0, %ymm8, %ymm8
	vpshufb	.LC7(%rip), %ymm8, %ymm8
	vmovdqa	4320(%rsp), %ymm5
	vmovdqa	4320(%rsp), %ymm4
	vpunpckhdq	4352(%rsp), %ymm4, %ymm0
	vpslld	$2, %ymm1, %ymm1
	vpermq	$216, %ymm8, %ymm8
	vmovdqa	4384(%rsp), %ymm4
	vpunpckldq	4416(%rsp), %ymm4, %ymm2
	vpor	%ymm7, %ymm1, %ymm1
	vpshufd	$216, %ymm8, %ymm15
	vpunpckhdq	4416(%rsp), %ymm4, %ymm4
	vpslld	$2, %ymm0, %ymm0
	vpunpckldq	4352(%rsp), %ymm5, %ymm5
	vpslld	$2, %ymm4, %ymm4
	vpor	%ymm5, %ymm0, %ymm0
	vpor	%ymm2, %ymm4, %ymm4
	vpunpcklqdq	%ymm3, %ymm1, %ymm2
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpunpcklqdq	%ymm4, %ymm0, %ymm3
	vpslld	$1, %ymm1, %ymm1
	vpunpckhqdq	%ymm4, %ymm0, %ymm0
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm2, %ymm1
	vpor	%ymm0, %ymm3, %ymm0
	vmovdqa	4640(%rsp), %ymm3
	vpunpckhdq	4672(%rsp), %ymm3, %ymm3
	vperm2i128	$32, %ymm0, %ymm1, %ymm7
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm7, %ymm7
	vpshufb	.LC7(%rip), %ymm7, %ymm7
	vpermq	$216, %ymm7, %ymm7
	vpshufd	$216, %ymm7, %ymm1
	vmovdqa	%ymm1, %ymm10
	vmovdqa	4448(%rsp), %ymm1
	vpunpckhdq	4480(%rsp), %ymm1, %ymm0
	vpunpckldq	4480(%rsp), %ymm1, %ymm2
	vmovdqa	4512(%rsp), %ymm1
	vpunpckhdq	4544(%rsp), %ymm1, %ymm7
	vpslld	$2, %ymm0, %ymm0
	vpunpckldq	4544(%rsp), %ymm1, %ymm5
	vmovdqa	4576(%rsp), %ymm1
	vpor	%ymm0, %ymm2, %ymm2
	vpunpckhdq	4608(%rsp), %ymm1, %ymm4
	vpunpckldq	4608(%rsp), %ymm1, %ymm6
	vpslld	$2, %ymm7, %ymm0
	vmovdqa	4640(%rsp), %ymm1
	vpunpckldq	4672(%rsp), %ymm1, %ymm1
	vpor	%ymm0, %ymm5, %ymm5
	vpslld	$2, %ymm4, %ymm0
	vpor	%ymm0, %ymm6, %ymm4
	vpslld	$2, %ymm3, %ymm0
	vpxor	%xmm6, %xmm6, %xmm6
	vmovdqa	%ymm6, 480(%rsp)
	vpor	%ymm0, %ymm1, %ymm1
	vpunpcklqdq	%ymm5, %ymm2, %ymm0
	vpunpckhqdq	%ymm5, %ymm2, %ymm2
	vpunpcklqdq	%ymm1, %ymm4, %ymm3
	vpunpckhqdq	%ymm1, %ymm4, %ymm1
	vmovdqa	%ymm6, 416(%rsp)
	vpslld	$1, %ymm2, %ymm2
	vpslld	$1, %ymm1, %ymm1
	vmovdqa	%ymm6, 576(%rsp)
	vmovdqa	%ymm6, 544(%rsp)
	vpor	%ymm2, %ymm0, %ymm0
	vpor	%ymm1, %ymm3, %ymm2
	vperm2i128	$32, %ymm2, %ymm0, %ymm1
	vperm2i128	$49, %ymm2, %ymm0, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm1, %ymm0
	vmovdqa	.LC0(%rip), %ymm1
	vpshufb	.LC7(%rip), %ymm0, %ymm0
	vmovdqa	%ymm1, 608(%rsp)
	vpermq	$216, %ymm0, %ymm0
	vmovdqa	.LC1(%rip), %ymm1
	vpshufd	$216, %ymm0, %ymm5
	vmovdqa	%ymm1, 384(%rsp)
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	vmovdqa	%ymm5, %ymm9
	vmovdqa	%ymm6, %ymm5
	vmovdqa	%ymm1, 352(%rsp)
	vmovdqa	%ymm1, 512(%rsp)
.L6:
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm6, %ymm1
	vmovd	%xmm14, %r10d
	movl	%eax, %r11d
	vmovq	%xmm5, %rdx
	andl	$1, %r10d
	sarl	$31, %r11d
	vmovdqa	544(%rsp), %ymm4
	vmovdqa	576(%rsp), %ymm2
	addq	%rdx, %rdx
	vmovd	%xmm15, %r13d
	vmovq	%rdx, %xmm0
	vmovq	%xmm1, %rdx
	andl	$1, %r13d
	addq	%rdx, %rdx
	vpblendd	$3, %ymm0, %ymm5, %ymm5
	negl	%r13d
	vmovdqa	%ymm5, 288(%rsp)
	vmovq	%rdx, %xmm0
	movl	%r10d, %edx
	negl	%edx
	vpblendd	$3, %ymm0, %ymm1, %ymm6
	vpxor	%ymm2, %ymm10, %ymm0
	vmovdqa	512(%rsp), %ymm1
	vmovdqa	%ymm6, 256(%rsp)
	andl	%edx, %r11d
	vmovd	%r11d, %xmm3
	vpxor	%ymm1, %ymm14, %ymm7
	vmovd	%xmm1, %r10d
	vpbroadcastd	%xmm3, %ymm3
	andl	$1, %r10d
	vpand	%ymm3, %ymm7, %ymm7
	vpand	%ymm3, %ymm0, %ymm0
	negl	%r10d
	vpxor	%ymm1, %ymm7, %ymm11
	andl	%edx, %r10d
	vmovd	%xmm4, %edx
	vmovdqa	352(%rsp), %ymm1
	vmovdqa	%ymm11, 512(%rsp)
	andl	$1, %edx
	vpxor	%ymm7, %ymm14, %ymm7
	vpxor	%ymm1, %ymm13, %ymm6
	negl	%edx
	vpand	%ymm3, %ymm6, %ymm6
	xorl	%r13d, %edx
	movl	%eax, %r13d
	vpxor	%ymm1, %ymm6, %ymm8
	andl	%r10d, %edx
	vpxor	%ymm6, %ymm13, %ymm6
	negl	%r13d
	vmovdqa	384(%rsp), %ymm1
	xorl	%eax, %r13d
	vmovdqa	%ymm8, 352(%rsp)
	andl	%r11d, %r13d
	vpxor	%ymm1, %ymm12, %ymm5
	xorl	%r13d, %eax
	vpand	%ymm3, %ymm5, %ymm5
	decl	%eax
	vpxor	%ymm1, %ymm5, %ymm1
	vpxor	%ymm5, %ymm12, %ymm5
	vmovdqa	%ymm1, 384(%rsp)
	vpxor	%ymm4, %ymm15, %ymm1
	vpand	%ymm3, %ymm1, %ymm1
	vpxor	%ymm4, %ymm1, %ymm4
	vpxor	%ymm1, %ymm15, %ymm1
	vpxor	%ymm2, %ymm0, %ymm15
	vmovdqa	%ymm4, 544(%rsp)
	vpxor	%ymm0, %ymm10, %ymm2
	vmovdqa	416(%rsp), %ymm10
	vmovdqa	%ymm2, 448(%rsp)
	vmovd	%r10d, %xmm2
	vpxor	%ymm10, %ymm9, %ymm0
	vpbroadcastd	%xmm2, %ymm2
	vmovdqa	%ymm15, 576(%rsp)
	vpand	%ymm3, %ymm0, %ymm0
	vpand	%ymm11, %ymm2, %ymm11
	vpxor	%ymm10, %ymm0, %ymm10
	vpxor	%ymm0, %ymm9, %ymm0
	vpxor	%ymm11, %ymm7, %ymm7
	vmovdqa	%ymm0, 320(%rsp)
	vmovd	%edx, %xmm0
	vpand	384(%rsp), %ymm2, %ymm9
	vmovdqa	%ymm10, 416(%rsp)
	vpbroadcastd	%xmm0, %ymm0
	vpand	%ymm8, %ymm2, %ymm10
	vpxor	%ymm4, %ymm0, %ymm4
	vpxor	%ymm15, %ymm0, %ymm8
	vpxor	%ymm10, %ymm6, %ymm6
	vpand	%ymm8, %ymm10, %ymm14
	vpand	%ymm4, %ymm11, %ymm4
	vpxor	%ymm9, %ymm5, %ymm5
	vmovdqa	%ymm14, 640(%rsp)
	vpxor	%ymm1, %ymm4, %ymm14
	vmovdqa	640(%rsp), %ymm13
	vpxor	448(%rsp), %ymm13, %ymm13
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	%ymm7, %ymm4, %ymm4
	vpxor	%ymm1, %ymm11, %ymm1
	vmovdqa	608(%rsp), %ymm7
	vpxor	416(%rsp), %ymm0, %ymm8
	vpor	%ymm6, %ymm13, %ymm13
	vmovaps	%xmm14, 224(%rsp)
	movq	224(%rsp), %r10
	vmovq	%xmm13, %rdx
	vmovq	%xmm13, %r13
	vpxor	640(%rsp), %ymm6, %ymm6
	vmovdqa	480(%rsp), %ymm11
	shrq	%r10
	vpand	%ymm8, %ymm9, %ymm15
	salq	$63, %rdx
	vpand	%ymm4, %ymm1, %ymm1
	vpxor	320(%rsp), %ymm15, %ymm8
	orq	%r10, %rdx
	shrq	%r13
	vpxor	448(%rsp), %ymm10, %ymm10
	vmovq	%rdx, %xmm12
	vmovq	%xmm1, %r10
	vpxor	320(%rsp), %ymm9, %ymm9
	vpor	%ymm5, %ymm8, %ymm8
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	vpand	%ymm6, %ymm10, %ymm10
	shrq	%r10
	vmovq	%xmm8, %rdx
	vpxor	%ymm5, %ymm15, %ymm5
	vpermq	$57, %ymm14, %ymm14
	vmovdqa	256(%rsp), %ymm6
	salq	$63, %rdx
	vpand	%ymm5, %ymm9, %ymm9
	vmovdqa	288(%rsp), %ymm5
	orq	%r13, %rdx
	vmovq	%xmm10, %r13
	vmovq	%rdx, %xmm12
	vmovq	%xmm8, %rdx
	shrq	%r13
	shrq	%rdx
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vmovq	%rdx, %xmm12
	vmovq	%xmm10, %rdx
	vpermq	$57, %ymm13, %ymm13
	salq	$63, %rdx
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	orq	%r10, %rdx
	vpermq	$57, %ymm8, %ymm12
	vmovq	%rdx, %xmm15
	vmovq	%xmm9, %rdx
	salq	$63, %rdx
	vpblendd	$3, %ymm15, %ymm1, %ymm1
	orq	%r13, %rdx
	vpermq	$57, %ymm1, %ymm15
	vpxor	%ymm11, %ymm6, %ymm1
	vmovq	%rdx, %xmm4
	vmovq	%xmm9, %rdx
	vpand	%ymm3, %ymm1, %ymm1
	shrq	%rdx
	vpblendd	$3, %ymm4, %ymm10, %ymm10
	vpxor	%ymm6, %ymm1, %ymm6
	vmovq	%rdx, %xmm4
	vpxor	%ymm0, %ymm6, %ymm0
	vpxor	%ymm11, %ymm1, %ymm1
	vpblendd	$3, %ymm4, %ymm9, %ymm9
	vpxor	%ymm7, %ymm5, %ymm4
	vpermq	$57, %ymm10, %ymm10
	vpand	%ymm3, %ymm4, %ymm4
	vpermq	$57, %ymm9, %ymm9
	vpxor	%ymm5, %ymm4, %ymm5
	vpxor	%ymm7, %ymm4, %ymm3
	vpand	%ymm2, %ymm5, %ymm2
	vpand	%ymm0, %ymm2, %ymm0
	vpxor	%ymm2, %ymm3, %ymm3
	vpxor	%ymm0, %ymm1, %ymm4
	vpxor	%ymm2, %ymm1, %ymm1
	vpxor	%ymm3, %ymm0, %ymm0
	vpor	%ymm3, %ymm4, %ymm4
	vpand	%ymm0, %ymm1, %ymm1
	vmovdqa	%ymm4, 608(%rsp)
	vmovdqa	%ymm1, 480(%rsp)
	decl	%r12d
	jne	.L6
	vpxor	%xmm4, %xmm4, %xmm4
	vmovdqa	%ymm15, %ymm11
	vmovdqa	%ymm13, 288(%rsp)
	movl	$256, %r12d
	vmovdqa	%ymm4, %ymm3
	vmovdqa	%ymm14, %ymm15
	vmovdqa	%ymm12, 256(%rsp)
	vmovdqa	%ymm6, %ymm13
	vmovdqa	%ymm4, 320(%rsp)
	vmovdqa	%ymm4, 640(%rsp)
	.p2align 4,,10
	.p2align 3
.L7:
	vpermq	$147, %ymm3, %ymm3
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm13, %ymm13
	vmovdqa	288(%rsp), %ymm8
	vmovq	%xmm5, %rdx
	vmovq	%xmm3, %r10
	vpermq	$147, %ymm4, %ymm4
	addq	%r10, %r10
	leaq	(%rdx,%rdx), %r11
	shrq	$63, %rdx
	vmovd	%xmm11, %r13d
	orq	%r10, %rdx
	vmovq	%r11, %xmm0
	vmovq	%xmm4, %r10
	andl	$1, %r13d
	vpblendd	$3, %ymm0, %ymm5, %ymm5
	vmovq	%rdx, %xmm0
	addq	%r10, %r10
	negl	%r13d
	vmovq	%xmm13, %rdx
	vpblendd	$3, %ymm0, %ymm3, %ymm1
	vmovdqa	%ymm5, 128(%rsp)
	vmovdqa	512(%rsp), %ymm5
	leaq	(%rdx,%rdx), %r11
	shrq	$63, %rdx
	vmovdqa	%ymm1, 96(%rsp)
	vmovdqa	544(%rsp), %ymm3
	orq	%r10, %rdx
	vmovd	%xmm15, %r10d
	vmovq	%r11, %xmm0
	movl	%eax, %r11d
	andl	$1, %r10d
	vpblendd	$3, %ymm0, %ymm13, %ymm6
	sarl	$31, %r11d
	vpxor	%ymm5, %ymm15, %ymm14
	vmovq	%rdx, %xmm0
	movl	%r10d, %edx
	vmovd	%xmm5, %r10d
	vmovdqa	%ymm6, 64(%rsp)
	negl	%edx
	vpblendd	$3, %ymm0, %ymm4, %ymm7
	vpxor	%ymm3, %ymm11, %ymm4
	andl	$1, %r10d
	andl	%edx, %r11d
	negl	%r10d
	vmovdqa	%ymm7, 32(%rsp)
	vmovdqa	352(%rsp), %ymm0
	vmovd	%r11d, %xmm1
	andl	%edx, %r10d
	vmovd	%xmm3, %edx
	vpbroadcastd	%xmm1, %ymm1
	vpxor	%ymm8, %ymm0, %ymm6
	andl	$1, %edx
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm14, %ymm14
	vpand	%ymm1, %ymm6, %ymm6
	negl	%edx
	vpxor	%ymm3, %ymm4, %ymm3
	vpxor	%ymm5, %ymm14, %ymm13
	vpxor	%ymm0, %ymm6, %ymm12
	xorl	%r13d, %edx
	vmovdqa	%ymm3, %ymm7
	andl	%r10d, %edx
	vpxor	%ymm14, %ymm15, %ymm14
	movl	%eax, %r13d
	vmovdqa	384(%rsp), %ymm0
	vpxor	%ymm8, %ymm6, %ymm6
	negl	%r13d
	vpxor	256(%rsp), %ymm0, %ymm5
	vmovdqa	%ymm3, 544(%rsp)
	vpxor	%ymm11, %ymm4, %ymm3
	xorl	%eax, %r13d
	vmovdqa	576(%rsp), %ymm11
	vmovdqa	416(%rsp), %ymm4
	vpand	%ymm1, %ymm5, %ymm5
	andl	%r11d, %r13d
	vmovdqa	%ymm3, 448(%rsp)
	vpxor	%ymm11, %ymm10, %ymm3
	vpxor	%ymm0, %ymm5, %ymm0
	xorl	%r13d, %eax
	vmovdqa	%ymm13, 512(%rsp)
	vpxor	%ymm4, %ymm9, %ymm2
	vpand	%ymm1, %ymm3, %ymm3
	decl	%eax
	vmovdqa	%ymm0, 384(%rsp)
	vpand	%ymm1, %ymm2, %ymm2
	vpxor	%ymm11, %ymm3, %ymm11
	vpxor	%ymm3, %ymm10, %ymm10
	vmovdqa	%ymm12, 352(%rsp)
	vpxor	%ymm2, %ymm9, %ymm3
	vpxor	%ymm4, %ymm2, %ymm4
	vmovd	%r10d, %xmm0
	vmovdqa	%ymm11, 576(%rsp)
	vmovd	%edx, %xmm2
	vpbroadcastd	%xmm0, %ymm0
	vmovdqa	%ymm3, 192(%rsp)
	vpand	384(%rsp), %ymm0, %ymm9
	vpbroadcastd	%xmm2, %ymm2
	vpand	%ymm13, %ymm0, %ymm11
	vpxor	576(%rsp), %ymm2, %ymm3
	vmovdqa	%ymm10, 224(%rsp)
	vmovdqa	%ymm4, 416(%rsp)
	vpand	%ymm12, %ymm0, %ymm10
	vpxor	%ymm7, %ymm2, %ymm4
	vpxor	%ymm11, %ymm14, %ymm7
	vpand	%ymm4, %ymm11, %ymm4
	vpand	%ymm3, %ymm10, %ymm3
	vpxor	448(%rsp), %ymm4, %ymm14
	vmovdqa	%ymm2, 160(%rsp)
	vpxor	224(%rsp), %ymm3, %ymm13
	vpxor	%ymm10, %ymm6, %ymm6
	vpxor	416(%rsp), %ymm2, %ymm2
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	%ymm7, %ymm4, %ymm4
	vpxor	%ymm6, %ymm3, %ymm3
	vpor	%ymm6, %ymm13, %ymm13
	vpand	%ymm2, %ymm9, %ymm2
	vmovq	%xmm14, %r10
	vmovq	%xmm13, %rdx
	shrq	%r10
	vmovq	%xmm13, %r13
	vpxor	256(%rsp), %ymm5, %ymm5
	vpxor	192(%rsp), %ymm2, %ymm8
	salq	$63, %rdx
	vpxor	448(%rsp), %ymm11, %ymm11
	vpxor	%ymm9, %ymm5, %ymm5
	orq	%r10, %rdx
	shrq	%r13
	vpor	%ymm5, %ymm8, %ymm8
	vmovq	%rdx, %xmm12
	vpand	%ymm4, %ymm11, %ymm11
	vmovq	%xmm8, %rdx
	vmovq	%xmm8, %r11
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	salq	$63, %rdx
	shrq	%r11
	vpermq	$57, %ymm14, %ymm15
	vmovq	%xmm11, %r10
	orq	%r13, %rdx
	vpxor	%ymm5, %ymm2, %ymm2
	shrq	%r10
	vmovq	%rdx, %xmm12
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vmovq	%r11, %xmm12
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vpermq	$57, %ymm13, %ymm14
	vmovdqa	%ymm14, 288(%rsp)
	vpermq	$57, %ymm8, %ymm14
	vmovdqa	%ymm14, 256(%rsp)
	vpxor	224(%rsp), %ymm10, %ymm10
	vmovdqa	64(%rsp), %ymm6
	vpxor	192(%rsp), %ymm9, %ymm9
	vmovdqa	608(%rsp), %ymm8
	vmovdqa	320(%rsp), %ymm14
	vpand	%ymm3, %ymm10, %ymm10
	vmovdqa	128(%rsp), %ymm5
	vmovq	%xmm10, %rdx
	vmovq	%xmm10, %r13
	vpand	%ymm2, %ymm9, %ymm9
	shrq	%r13
	salq	$63, %rdx
	vmovq	%xmm9, %r11
	vpxor	%ymm8, %ymm5, %ymm12
	orq	%r10, %rdx
	shrq	%r11
	vpand	%ymm1, %ymm12, %ymm12
	vmovq	%rdx, %xmm4
	vmovq	%xmm9, %rdx
	vmovq	%r11, %xmm2
	vpblendd	$3, %ymm4, %ymm11, %ymm11
	salq	$63, %rdx
	vmovdqa	96(%rsp), %ymm4
	vpxor	%ymm5, %ymm12, %ymm5
	vpxor	640(%rsp), %ymm4, %ymm7
	vpxor	%ymm8, %ymm12, %ymm12
	vpermq	$57, %ymm11, %ymm11
	orq	%r13, %rdx
	vmovq	%rdx, %xmm3
	vpblendd	$3, %ymm2, %ymm9, %ymm9
	vpand	%ymm1, %ymm7, %ymm7
	vpblendd	$3, %ymm3, %ymm10, %ymm10
	vpermq	$57, %ymm9, %ymm9
	vpxor	%ymm4, %ymm7, %ymm3
	vpermq	$57, %ymm10, %ymm10
	vmovdqa	480(%rsp), %ymm4
	vpxor	640(%rsp), %ymm7, %ymm7
	vpxor	%ymm4, %ymm6, %ymm2
	vpand	%ymm1, %ymm2, %ymm2
	vpxor	%ymm6, %ymm2, %ymm13
	vmovdqa	32(%rsp), %ymm6
	vpxor	%ymm4, %ymm2, %ymm2
	vpxor	%ymm14, %ymm6, %ymm4
	vpand	%ymm1, %ymm4, %ymm1
	vpxor	%ymm6, %ymm1, %ymm4
	vpxor	%ymm14, %ymm1, %ymm1
	vpand	%ymm0, %ymm5, %ymm14
	vpxor	160(%rsp), %ymm13, %ymm6
	vpxor	%ymm14, %ymm12, %ymm8
	vpand	%ymm0, %ymm3, %ymm0
	vpxor	%ymm0, %ymm7, %ymm7
	vpand	%ymm6, %ymm14, %ymm6
	vpxor	%ymm2, %ymm6, %ymm12
	vpxor	%ymm2, %ymm14, %ymm2
	vpor	%ymm8, %ymm12, %ymm12
	vpxor	%ymm8, %ymm6, %ymm8
	vmovdqa	%ymm12, 608(%rsp)
	vpand	%ymm8, %ymm2, %ymm8
	vpxor	160(%rsp), %ymm4, %ymm2
	vmovdqa	%ymm8, 480(%rsp)
	vpand	%ymm2, %ymm0, %ymm2
	vpxor	%ymm1, %ymm2, %ymm6
	vpxor	%ymm1, %ymm0, %ymm1
	vpor	%ymm7, %ymm6, %ymm6
	vpxor	%ymm7, %ymm2, %ymm7
	vmovdqa	%ymm6, 640(%rsp)
	vpand	%ymm7, %ymm1, %ymm1
	vmovdqa	%ymm1, 320(%rsp)
	decl	%r12d
	jne	.L7
	vmovdqa	%ymm9, 64(%rsp)
	vpxor	%xmm9, %xmm9, %xmm9
	movl	$375, %r12d
	vmovdqa	%ymm10, 128(%rsp)
	vmovdqa	%ymm9, %ymm10
	vmovdqa	%ymm15, 160(%rsp)
	vmovdqa	%ymm13, 224(%rsp)
	vmovdqa	%ymm9, 192(%rsp)
	vmovdqa	%ymm9, 448(%rsp)
	vmovdqa	%ymm11, 96(%rsp)
	.p2align 4,,10
	.p2align 3
.L8:
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm3, %ymm3
	vpermq	$147, %ymm10, %ymm10
	vmovdqa	288(%rsp), %ymm12
	vmovq	%xmm5, %r10
	vmovq	%xmm3, %rdx
	vpermq	$147, 224(%rsp), %ymm15
	vmovdqa	%ymm5, 672(%rsp)
	leaq	(%r10,%r10), %r13
	vpermq	$147, %ymm4, %ymm4
	shrq	$63, %r10
	vpermq	$147, %ymm9, %ymm9
	movq	%r13, 672(%rsp)
	vmovq	%xmm10, %r11
	leaq	(%rdx,%rdx), %r13
	shrq	$63, %rdx
	orq	%r13, %r10
	addq	%r11, %r11
	vmovdqa	%ymm3, 704(%rsp)
	vmovdqa	160(%rsp), %ymm3
	orq	%r11, %rdx
	vmovq	%xmm9, %r11
	movq	%r10, 704(%rsp)
	vmovq	%xmm15, %r10
	vmovq	%rdx, %xmm0
	leaq	(%r10,%r10), %r13
	vmovq	%xmm4, %rdx
	vmovdqa	%ymm15, 768(%rsp)
	shrq	$63, %r10
	movq	%r13, 768(%rsp)
	leaq	(%rdx,%rdx), %r13
	addq	%r11, %r11
	orq	%r13, %r10
	shrq	$63, %rdx
	vmovdqa	%ymm4, 800(%rsp)
	vpblendd	$3, %ymm0, %ymm10, %ymm7
	orq	%r11, %rdx
	movl	%eax, %r11d
	movq	%r10, 800(%rsp)
	vmovd	%xmm3, %r10d
	andl	$1, %r10d
	vmovq	%rdx, %xmm0
	sarl	$31, %r11d
	vmovdqa	352(%rsp), %ymm5
	movl	%r10d, %edx
	vmovdqa	%ymm7, 32(%rsp)
	vmovdqa	512(%rsp), %ymm7
	vmovdqa	96(%rsp), %ymm2
	negl	%edx
	vpxor	%ymm5, %ymm12, %ymm6
	vpblendd	$3, %ymm0, %ymm9, %ymm9
	vmovdqa	544(%rsp), %ymm4
	andl	%edx, %r11d
	vmovd	%xmm7, %r10d
	vmovd	%xmm2, %r13d
	vmovdqa	%ymm9, (%rsp)
	vmovd	%r11d, %xmm1
	vpxor	%ymm2, %ymm4, %ymm11
	andl	$1, %r10d
	andl	$1, %r13d
	vpbroadcastd	%xmm1, %ymm1
	negl	%r10d
	vpxor	%ymm3, %ymm7, %ymm14
	negl	%r13d
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm11, %ymm11
	vpand	%ymm1, %ymm14, %ymm14
	andl	%edx, %r10d
	vpxor	%ymm5, %ymm6, %ymm8
	vpxor	%ymm2, %ymm11, %ymm15
	vmovd	%xmm4, %edx
	vmovdqa	384(%rsp), %ymm0
	vpxor	%ymm4, %ymm11, %ymm4
	vpxor	%ymm7, %ymm14, %ymm13
	vpxor	%ymm3, %ymm14, %ymm14
	andl	$1, %edx
	vmovdqa	256(%rsp), %ymm5
	negl	%edx
	vpxor	%ymm12, %ymm6, %ymm6
	vmovdqa	128(%rsp), %ymm2
	vmovdqa	576(%rsp), %ymm11
	xorl	%r13d, %edx
	movl	%eax, %r13d
	vmovdqa	%ymm15, 224(%rsp)
	vpxor	%ymm0, %ymm5, %ymm5
	andl	%r10d, %edx
	negl	%r13d
	vmovdqa	%ymm13, 512(%rsp)
	vpand	%ymm1, %ymm5, %ymm5
	vpxor	%ymm2, %ymm11, %ymm10
	xorl	%eax, %r13d
	vmovdqa	%ymm8, 352(%rsp)
	vpxor	%ymm0, %ymm5, %ymm0
	vpand	%ymm1, %ymm10, %ymm10
	andl	%r11d, %r13d
	vmovdqa	%ymm4, 544(%rsp)
	vmovdqa	%ymm0, 384(%rsp)
	vpxor	%ymm11, %ymm10, %ymm11
	xorl	%r13d, %eax
	vmovdqa	416(%rsp), %ymm0
	vmovdqa	%ymm11, 576(%rsp)
	vpxor	%ymm2, %ymm10, %ymm10
	decl	%eax
	vmovdqa	64(%rsp), %ymm2
	vmovdqa	%ymm10, 128(%rsp)
	vpxor	%ymm0, %ymm2, %ymm9
	vpand	%ymm1, %ymm9, %ymm9
	vpxor	%ymm2, %ymm9, %ymm15
	vpxor	%ymm0, %ymm9, %ymm0
	vmovdqa	%ymm0, 416(%rsp)
	vmovd	%r10d, %xmm0
	vmovdqa	%ymm15, 96(%rsp)
	vmovd	%edx, %xmm15
	vpbroadcastd	%xmm0, %ymm0
	vpbroadcastd	%xmm15, %ymm15
	vpand	%ymm13, %ymm0, %ymm11
	vpand	%ymm8, %ymm0, %ymm10
	vpxor	%ymm4, %ymm15, %ymm4
	vpxor	%ymm11, %ymm14, %ymm7
	vpxor	%ymm10, %ymm6, %ymm6
	vpxor	576(%rsp), %ymm15, %ymm3
	vpand	%ymm4, %ymm11, %ymm4
	vpxor	224(%rsp), %ymm4, %ymm14
	vpand	384(%rsp), %ymm0, %ymm9
	vpxor	%ymm7, %ymm4, %ymm4
	vpxor	416(%rsp), %ymm15, %ymm2
	vpand	%ymm3, %ymm10, %ymm3
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	128(%rsp), %ymm3, %ymm13
	vmovdqa	32(%rsp), %ymm7
	vmovaps	%xmm14, 288(%rsp)
	vpand	%ymm2, %ymm9, %ymm2
	vpxor	256(%rsp), %ymm5, %ymm5
	movq	288(%rsp), %r10
	vpor	%ymm6, %ymm13, %ymm13
	vpxor	96(%rsp), %ymm2, %ymm8
	vpxor	224(%rsp), %ymm11, %ymm11
	vmovq	%xmm13, %rdx
	vpxor	%ymm9, %ymm5, %ymm5
	shrq	%r10
	vpxor	%ymm6, %ymm3, %ymm3
	vpor	%ymm5, %ymm8, %ymm8
	salq	$63, %rdx
	vpand	%ymm4, %ymm11, %ymm11
	vpxor	%ymm5, %ymm2, %ymm2
	orq	%r10, %rdx
	vmovq	%xmm13, %r13
	vmovq	%xmm8, %r11
	vmovdqa	640(%rsp), %ymm6
	vmovq	%rdx, %xmm12
	vmovq	%xmm8, %rdx
	shrq	%r13
	vpxor	128(%rsp), %ymm10, %ymm10
	salq	$63, %rdx
	vpxor	96(%rsp), %ymm9, %ymm9
	vmovq	%xmm11, %r10
	shrq	%r11
	vpand	%ymm3, %ymm10, %ymm10
	orq	%r13, %rdx
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	shrq	%r10
	vmovq	%rdx, %xmm12
	vmovq	%xmm10, %rdx
	vpand	%ymm2, %ymm9, %ymm9
	salq	$63, %rdx
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vmovq	%r11, %xmm12
	orq	%r10, %rdx
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vmovq	%xmm10, %r13
	vpermq	$57, %ymm14, %ymm12
	vmovq	%rdx, %xmm4
	shrq	%r13
	vpermq	$57, %ymm8, %ymm8
	vmovq	%xmm9, %rdx
	vmovq	%xmm9, %r11
	vmovdqa	%ymm12, 160(%rsp)
	vpermq	$57, %ymm13, %ymm12
	salq	$63, %rdx
	vpblendd	$3, %ymm4, %ymm11, %ymm11
	shrq	%r11
	vmovdqa	480(%rsp), %ymm13
	vmovdqa	%ymm12, 288(%rsp)
	orq	%r13, %rdx
	vpermq	$57, %ymm11, %ymm5
	vpxor	768(%rsp), %ymm13, %ymm12
	vmovq	%rdx, %xmm3
	vmovq	%r11, %xmm2
	vmovdqa	%ymm5, 96(%rsp)
	vmovdqa	320(%rsp), %ymm14
	vpblendd	$3, %ymm3, %ymm10, %ymm10
	vpand	%ymm1, %ymm12, %ymm12
	vpxor	768(%rsp), %ymm12, %ymm11
	vmovdqa	%ymm8, 256(%rsp)
	vpblendd	$3, %ymm2, %ymm9, %ymm9
	vpermq	$57, %ymm10, %ymm5
	vpxor	%ymm13, %ymm12, %ymm12
	vmovdqa	448(%rsp), %ymm2
	vmovdqa	%ymm5, 128(%rsp)
	vpermq	$57, %ymm9, %ymm5
	vpxor	704(%rsp), %ymm6, %ymm6
	vmovdqa	%ymm11, 224(%rsp)
	vpxor	%ymm7, %ymm2, %ymm2
	vpxor	800(%rsp), %ymm14, %ymm11
	vmovdqa	%ymm5, 64(%rsp)
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm2, %ymm2
	vmovdqa	608(%rsp), %ymm5
	vpxor	672(%rsp), %ymm5, %ymm8
	vpand	%ymm1, %ymm11, %ymm11
	vpxor	%ymm7, %ymm2, %ymm10
	vpxor	704(%rsp), %ymm6, %ymm3
	vpand	%ymm1, %ymm8, %ymm8
	vpxor	672(%rsp), %ymm8, %ymm5
	vpxor	800(%rsp), %ymm11, %ymm4
	vpxor	%ymm14, %ymm11, %ymm11
	vmovdqa	(%rsp), %ymm9
	vmovdqa	192(%rsp), %ymm14
	vpxor	224(%rsp), %ymm15, %ymm13
	vpxor	608(%rsp), %ymm8, %ymm8
	vpxor	640(%rsp), %ymm6, %ymm6
	vpxor	%ymm14, %ymm9, %ymm7
	vpxor	448(%rsp), %ymm2, %ymm2
	vpand	%ymm1, %ymm7, %ymm1
	vpxor	%ymm9, %ymm1, %ymm9
	vpxor	%ymm14, %ymm1, %ymm1
	vpand	%ymm0, %ymm5, %ymm14
	vpand	%ymm13, %ymm14, %ymm13
	vpxor	%ymm14, %ymm8, %ymm7
	vpxor	%ymm12, %ymm13, %ymm8
	vpxor	%ymm12, %ymm14, %ymm12
	vpor	%ymm7, %ymm8, %ymm8
	vpxor	%ymm7, %ymm13, %ymm7
	vmovdqa	%ymm8, 608(%rsp)
	vpand	%ymm7, %ymm12, %ymm12
	vpand	%ymm0, %ymm3, %ymm8
	vpxor	%ymm15, %ymm4, %ymm7
	vpand	%ymm7, %ymm8, %ymm7
	vpxor	%ymm8, %ymm6, %ymm6
	vpand	%ymm0, %ymm10, %ymm0
	vmovdqa	%ymm12, 480(%rsp)
	vpxor	%ymm11, %ymm7, %ymm12
	vpxor	%ymm11, %ymm8, %ymm11
	vpxor	%ymm0, %ymm2, %ymm2
	vpor	%ymm6, %ymm12, %ymm14
	vpxor	%ymm6, %ymm7, %ymm6
	vmovdqa	%ymm14, 640(%rsp)
	vpand	%ymm6, %ymm11, %ymm6
	vmovdqa	%ymm6, 320(%rsp)
	vpxor	%ymm15, %ymm9, %ymm6
	vpand	%ymm6, %ymm0, %ymm6
	vpxor	%ymm1, %ymm6, %ymm7
	vpxor	%ymm1, %ymm0, %ymm1
	vpor	%ymm2, %ymm7, %ymm7
	vpxor	%ymm2, %ymm6, %ymm2
	vmovdqa	%ymm7, 448(%rsp)
	vpand	%ymm2, %ymm1, %ymm1
	vmovdqa	%ymm1, 192(%rsp)
	decl	%r12d
	jne	.L8
	vmovdqa	96(%rsp), %ymm15
	movl	$256, %r12d
	vmovdqa	%ymm15, %ymm14
	.p2align 4,,10
	.p2align 3
.L9:
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm3, %ymm3
	vpermq	$147, %ymm10, %ymm10
	vmovdqa	160(%rsp), %ymm2
	vmovq	%xmm5, %r10
	vmovq	%xmm3, %rdx
	vmovq	%xmm10, %r11
	vmovdqa	%ymm5, 672(%rsp)
	leaq	(%r10,%r10), %r13
	addq	%r11, %r11
	shrq	$63, %r10
	vpermq	$147, %ymm4, %ymm4
	movq	%r13, 672(%rsp)
	leaq	(%rdx,%rdx), %r13
	shrq	$63, %rdx
	vpermq	$147, %ymm9, %ymm9
	vmovdqa	%ymm3, 704(%rsp)
	orq	%r11, %rdx
	orq	%r13, %r10
	vmovq	%xmm9, %r11
	vmovq	%rdx, %xmm0
	movq	%r10, 704(%rsp)
	vmovq	%xmm4, %rdx
	addq	%r11, %r11
	vmovdqa	%ymm4, 800(%rsp)
	vpblendd	$3, %ymm0, %ymm10, %ymm7
	vpermq	$147, 224(%rsp), %ymm0
	vmovdqa	512(%rsp), %ymm1
	vmovdqa	288(%rsp), %ymm15
	vmovdqa	%ymm7, 416(%rsp)
	vmovdqa	544(%rsp), %ymm7
	vmovq	%xmm0, %r10
	vpxor	%ymm2, %ymm1, %ymm5
	vmovdqa	%ymm0, 768(%rsp)
	leaq	(%r10,%r10), %r13
	shrq	$63, %r10
	movq	%r13, 768(%rsp)
	leaq	(%rdx,%rdx), %r13
	shrq	$63, %rdx
	orq	%r13, %r10
	orq	%r11, %rdx
	vmovd	%xmm14, %r13d
	movl	%eax, %r11d
	movq	%r10, 800(%rsp)
	vmovd	%xmm2, %r10d
	sarl	$31, %r11d
	andl	$1, %r13d
	andl	$1, %r10d
	vmovq	%rdx, %xmm0
	negl	%r13d
	movl	%r10d, %edx
	vmovd	%xmm1, %r10d
	vpblendd	$3, %ymm0, %ymm9, %ymm9
	vmovdqa	128(%rsp), %ymm0
	negl	%edx
	andl	$1, %r10d
	andl	%edx, %r11d
	negl	%r10d
	vmovd	%r11d, %xmm8
	andl	%edx, %r10d
	vmovd	%xmm7, %edx
	vpbroadcastd	%xmm8, %ymm8
	andl	$1, %edx
	vpand	%ymm8, %ymm5, %ymm5
	negl	%edx
	vpxor	%ymm1, %ymm5, %ymm3
	xorl	%r13d, %edx
	vpxor	%ymm2, %ymm5, %ymm5
	movl	%eax, %r13d
	vmovdqa	352(%rsp), %ymm1
	andl	%r10d, %edx
	negl	%r13d
	vmovdqa	%ymm3, 512(%rsp)
	xorl	%eax, %r13d
	vpxor	%ymm15, %ymm1, %ymm4
	andl	%r11d, %r13d
	vpand	%ymm8, %ymm4, %ymm4
	xorl	%r13d, %eax
	vpxor	%ymm1, %ymm4, %ymm10
	vmovdqa	%ymm7, %ymm1
	vpxor	%ymm7, %ymm14, %ymm7
	decl	%eax
	vpand	%ymm8, %ymm7, %ymm7
	vpxor	%ymm15, %ymm4, %ymm4
	vmovdqa	%ymm10, 352(%rsp)
	vpxor	%ymm1, %ymm7, %ymm13
	vpxor	%ymm14, %ymm7, %ymm7
	vmovdqa	576(%rsp), %ymm1
	vmovdqa	%ymm13, 544(%rsp)
	vpxor	%ymm0, %ymm1, %ymm6
	vpand	%ymm8, %ymm6, %ymm6
	vpxor	%ymm1, %ymm6, %ymm14
	vpxor	%ymm0, %ymm6, %ymm6
	vmovd	%r10d, %xmm1
	vmovd	%edx, %xmm0
	vpbroadcastd	%xmm1, %ymm1
	vmovdqa	%ymm14, 576(%rsp)
	vpbroadcastd	%xmm0, %ymm0
	vpand	%ymm3, %ymm1, %ymm11
	vpand	%ymm10, %ymm1, %ymm10
	vpxor	%ymm13, %ymm0, %ymm3
	vpxor	%ymm14, %ymm0, %ymm2
	vpxor	%ymm11, %ymm5, %ymm5
	vpand	%ymm3, %ymm11, %ymm3
	vpand	%ymm2, %ymm10, %ymm2
	vpxor	%ymm10, %ymm4, %ymm4
	vpxor	%ymm7, %ymm3, %ymm14
	vpxor	%ymm6, %ymm2, %ymm12
	vpxor	%ymm5, %ymm3, %ymm3
	vpor	%ymm4, %ymm12, %ymm12
	vpor	%ymm5, %ymm14, %ymm14
	vpxor	%ymm4, %ymm2, %ymm2
	vmovq	%xmm14, %r10
	vmovq	%xmm12, %r11
	vpxor	%ymm7, %ymm11, %ymm7
	salq	$63, %r11
	shrq	%r10
	vpxor	%ymm6, %ymm10, %ymm6
	vpand	%ymm3, %ymm7, %ymm7
	orq	%r11, %r10
	vpand	%ymm2, %ymm6, %ymm6
	vmovq	%xmm12, %rdx
	vmovdqa	448(%rsp), %ymm2
	vmovq	%r10, %xmm13
	vmovq	%xmm6, %r11
	vmovq	%xmm7, %r10
	shrq	%rdx
	shrq	%r10
	salq	$63, %r11
	vpblendd	$3, %ymm13, %ymm14, %ymm14
	vmovq	%rdx, %xmm13
	orq	%r11, %r10
	vmovq	%xmm6, %rdx
	vpblendd	$3, %ymm13, %ymm12, %ymm12
	vmovq	%r10, %xmm11
	shrq	%rdx
	vpermq	$57, %ymm14, %ymm15
	vpblendd	$3, %ymm11, %ymm7, %ymm7
	vmovq	%rdx, %xmm10
	vmovdqa	%ymm15, 160(%rsp)
	vpermq	$57, %ymm12, %ymm15
	vpblendd	$3, %ymm10, %ymm6, %ymm6
	vpermq	$57, %ymm7, %ymm14
	vmovdqa	416(%rsp), %ymm7
	vmovdqa	%ymm15, 288(%rsp)
	vpermq	$57, %ymm6, %ymm5
	vmovdqa	640(%rsp), %ymm6
	vpxor	704(%rsp), %ymm6, %ymm6
	vmovdqa	%ymm5, 128(%rsp)
	vpxor	%ymm7, %ymm2, %ymm2
	vmovdqa	608(%rsp), %ymm5
	vpxor	672(%rsp), %ymm5, %ymm15
	vpand	%ymm8, %ymm2, %ymm2
	vpand	%ymm8, %ymm6, %ymm6
	vpxor	704(%rsp), %ymm6, %ymm3
	vpand	%ymm8, %ymm15, %ymm15
	vpxor	%ymm7, %ymm2, %ymm10
	vpxor	672(%rsp), %ymm15, %ymm5
	vmovdqa	480(%rsp), %ymm7
	vpxor	768(%rsp), %ymm7, %ymm12
	vpxor	608(%rsp), %ymm15, %ymm15
	vpxor	640(%rsp), %ymm6, %ymm6
	vpxor	448(%rsp), %ymm2, %ymm2
	vpand	%ymm8, %ymm12, %ymm12
	vpxor	768(%rsp), %ymm12, %ymm13
	vpxor	%ymm7, %ymm12, %ymm12
	vmovdqa	320(%rsp), %ymm7
	vpxor	800(%rsp), %ymm7, %ymm11
	vmovdqa	%ymm13, 224(%rsp)
	vpxor	%ymm13, %ymm0, %ymm13
	vpand	%ymm8, %ymm11, %ymm11
	vpxor	800(%rsp), %ymm11, %ymm4
	vpxor	%ymm7, %ymm11, %ymm11
	vpxor	192(%rsp), %ymm9, %ymm7
	vpand	%ymm8, %ymm7, %ymm8
	vpand	%ymm1, %ymm5, %ymm7
	vpand	%ymm13, %ymm7, %ymm13
	vpxor	%ymm7, %ymm15, %ymm15
	vpxor	%ymm9, %ymm8, %ymm9
	vmovdqa	%ymm15, 480(%rsp)
	vpxor	%ymm12, %ymm13, %ymm15
	vpxor	%ymm12, %ymm7, %ymm12
	vpxor	480(%rsp), %ymm13, %ymm7
	vpor	480(%rsp), %ymm15, %ymm15
	vpxor	192(%rsp), %ymm8, %ymm8
	vpand	%ymm7, %ymm12, %ymm7
	vpand	%ymm1, %ymm3, %ymm12
	vpand	%ymm1, %ymm10, %ymm1
	vmovdqa	%ymm7, 480(%rsp)
	vpxor	%ymm0, %ymm4, %ymm7
	vpxor	%ymm12, %ymm6, %ymm6
	vpxor	%ymm0, %ymm9, %ymm0
	vpand	%ymm7, %ymm12, %ymm7
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm15, 608(%rsp)
	vpxor	%ymm11, %ymm7, %ymm13
	vpxor	%ymm11, %ymm12, %ymm11
	vpxor	%ymm8, %ymm1, %ymm1
	vpor	%ymm6, %ymm13, %ymm15
	vpxor	%ymm6, %ymm7, %ymm6
	vmovdqa	%ymm15, 640(%rsp)
	vpand	%ymm6, %ymm11, %ymm7
	vpxor	%ymm8, %ymm0, %ymm6
	vpxor	%ymm2, %ymm0, %ymm8
	vpor	%ymm2, %ymm6, %ymm6
	vpand	%ymm8, %ymm1, %ymm1
	vmovdqa	%ymm7, 320(%rsp)
	vmovdqa	%ymm6, 448(%rsp)
	vmovdqa	%ymm1, 192(%rsp)
	decl	%r12d
	jne	.L9
	vmovdqa	%ymm14, %ymm15
	movl	$256, %r10d
	vmovdqa	%ymm1, %ymm13
	vmovdqa	608(%rsp), %ymm14
	vmovdqa	%ymm7, %ymm12
.L10:
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm10, %ymm10
	vpermq	$147, %ymm3, %ymm3
	vmovdqa	160(%rsp), %ymm6
	vmovq	%xmm5, %r11
	vmovq	%xmm10, %rdx
	vmovq	%xmm3, %r12
	vmovdqa	%ymm5, 672(%rsp)
	leaq	(%rdx,%rdx), %r13
	leaq	(%r11,%r11), %rdx
	vpermq	$147, %ymm4, %ymm4
	shrq	$63, %r11
	movq	%rdx, 672(%rsp)
	leaq	(%r12,%r12), %rdx
	vpermq	$147, %ymm9, %ymm9
	vmovdqa	512(%rsp), %ymm1
	orq	%rdx, %r11
	vmovq	%xmm3, %rdx
	vmovq	%xmm9, %r12
	vmovdqa	%ymm3, 704(%rsp)
	shrq	$63, %rdx
	movq	%r11, 704(%rsp)
	vmovq	%xmm4, %r11
	addq	%r12, %r12
	vmovdqa	%ymm4, 800(%rsp)
	orq	%r13, %rdx
	vpxor	%ymm6, %ymm1, %ymm3
	vmovq	%rdx, %xmm0
	vpblendd	$3, %ymm0, %ymm10, %ymm10
	vpermq	$147, 224(%rsp), %ymm0
	vmovdqa	%ymm0, 768(%rsp)
	vmovq	%xmm0, %rdx
	leaq	(%rdx,%rdx), %r13
	shrq	$63, %rdx
	movq	%r13, 768(%rsp)
	leaq	(%r11,%r11), %r13
	movl	%eax, %r11d
	orq	%r13, %rdx
	sarl	$31, %r11d
	vmovd	%xmm15, %r13d
	movq	%rdx, 800(%rsp)
	vmovq	%xmm4, %rdx
	andl	$1, %r13d
	vmovdqa	544(%rsp), %ymm4
	shrq	$63, %rdx
	negl	%r13d
	orq	%r12, %rdx
	vmovd	%xmm1, %r12d
	vmovq	%rdx, %xmm0
	vmovd	%xmm6, %edx
	andl	$1, %r12d
	andl	$1, %edx
	negl	%r12d
	vpblendd	$3, %ymm0, %ymm9, %ymm9
	negl	%edx
	vpxor	%ymm4, %ymm15, %ymm0
	andl	%edx, %r11d
	andl	%edx, %r12d
	vmovd	%xmm4, %edx
	andl	$1, %edx
	vmovd	%r11d, %xmm7
	negl	%edx
	vpbroadcastd	%xmm7, %ymm7
	xorl	%r13d, %edx
	vpand	%ymm7, %ymm0, %ymm0
	vpand	%ymm7, %ymm3, %ymm3
	movl	%eax, %r13d
	andl	%r12d, %edx
	vpxor	%ymm1, %ymm3, %ymm5
	vpxor	%ymm4, %ymm0, %ymm4
	negl	%r13d
	vpxor	%ymm15, %ymm0, %ymm15
	vmovd	%r12d, %xmm1
	vmovd	%edx, %xmm0
	xorl	%eax, %r13d
	vpbroadcastd	%xmm1, %ymm1
	vpbroadcastd	%xmm0, %ymm0
	vpxor	%ymm6, %ymm3, %ymm3
	andl	%r11d, %r13d
	vmovdqa	%ymm5, 512(%rsp)
	vpxor	%ymm4, %ymm0, %ymm2
	vpand	%ymm5, %ymm1, %ymm5
	xorl	%r13d, %eax
	vpand	%ymm2, %ymm5, %ymm2
	vpxor	%ymm5, %ymm3, %ymm3
	decl	%eax
	vmovdqa	%ymm4, 544(%rsp)
	vpxor	%ymm2, %ymm15, %ymm4
	vpxor	%ymm3, %ymm2, %ymm2
	vpxor	%ymm5, %ymm15, %ymm15
	vpor	%ymm3, %ymm4, %ymm4
	vpand	%ymm2, %ymm15, %ymm15
	vmovdqa	640(%rsp), %ymm3
	vmovq	%xmm4, %rdx
	shrq	%rdx
	vmovq	%rdx, %xmm6
	vmovq	%xmm15, %rdx
	vpblendd	$3, %ymm6, %ymm4, %ymm4
	shrq	%rdx
	vpermq	$57, %ymm4, %ymm6
	vmovq	%rdx, %xmm2
	vpxor	448(%rsp), %ymm10, %ymm4
	vmovdqa	%ymm6, 160(%rsp)
	vpblendd	$3, %ymm2, %ymm15, %ymm15
	vpxor	672(%rsp), %ymm14, %ymm6
	vpxor	704(%rsp), %ymm3, %ymm2
	vpand	%ymm7, %ymm4, %ymm8
	vpermq	$57, %ymm15, %ymm15
	vpxor	%ymm8, %ymm10, %ymm10
	vpand	%ymm7, %ymm6, %ymm6
	vpand	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm8, 608(%rsp)
	vmovdqa	480(%rsp), %ymm8
	vpxor	768(%rsp), %ymm8, %ymm11
	vpxor	672(%rsp), %ymm6, %ymm5
	vpxor	%ymm6, %ymm14, %ymm6
	vpxor	704(%rsp), %ymm2, %ymm3
	vpand	%ymm7, %ymm11, %ymm11
	vpxor	768(%rsp), %ymm11, %ymm4
	vmovdqa	%ymm5, 672(%rsp)
	vpxor	%ymm8, %ymm11, %ymm11
	vpxor	800(%rsp), %ymm12, %ymm8
	vmovdqa	%ymm4, 224(%rsp)
	vmovdqa	%ymm4, 768(%rsp)
	vpand	%ymm7, %ymm8, %ymm8
	vpxor	800(%rsp), %ymm8, %ymm4
	vmovdqa	%ymm3, 704(%rsp)
	vpxor	%ymm8, %ymm12, %ymm8
	vpxor	%ymm9, %ymm13, %ymm12
	vmovdqa	%ymm4, 800(%rsp)
	vpand	%ymm7, %ymm12, %ymm7
	vpxor	224(%rsp), %ymm0, %ymm12
	vpxor	640(%rsp), %ymm2, %ymm2
	vpxor	%ymm9, %ymm7, %ymm9
	vpxor	%ymm7, %ymm13, %ymm7
	vpand	%ymm1, %ymm5, %ymm13
	vpand	%ymm12, %ymm13, %ymm12
	vpxor	%ymm13, %ymm6, %ymm6
	vpxor	%ymm11, %ymm12, %ymm14
	vpxor	%ymm11, %ymm13, %ymm11
	vpor	%ymm6, %ymm14, %ymm14
	vpxor	%ymm6, %ymm12, %ymm6
	vpand	%ymm6, %ymm11, %ymm6
	vpand	%ymm1, %ymm3, %ymm11
	vpand	%ymm1, %ymm10, %ymm1
	vmovdqa	%ymm6, 480(%rsp)
	vpxor	%ymm0, %ymm4, %ymm6
	vpxor	%ymm11, %ymm2, %ymm2
	vpxor	%ymm0, %ymm9, %ymm0
	vpand	%ymm6, %ymm11, %ymm6
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	%ymm8, %ymm6, %ymm12
	vpxor	%ymm8, %ymm11, %ymm8
	vpor	%ymm2, %ymm12, %ymm13
	vpxor	%ymm2, %ymm6, %ymm2
	vmovdqa	608(%rsp), %ymm6
	vmovdqa	%ymm13, 640(%rsp)
	vpand	%ymm2, %ymm8, %ymm12
	vpxor	448(%rsp), %ymm6, %ymm2
	vpxor	%ymm7, %ymm0, %ymm6
	vpxor	%ymm1, %ymm2, %ymm2
	vpxor	%ymm7, %ymm1, %ymm1
	vpor	%ymm2, %ymm6, %ymm6
	vpxor	%ymm2, %ymm0, %ymm0
	vmovdqa	%ymm6, 448(%rsp)
	vpand	%ymm0, %ymm1, %ymm13
	decl	%r10d
	jne	.L10
	vmovdqa	512(%rsp), %xmm0
	leaq	2400(%rsp), %rdx
	vmovd	%xmm0, %eax
	vmovdqa	544(%rsp), %xmm0
	andl	$1, %eax
	negl	%eax
	vmovd	%eax, %xmm1
	vmovd	%xmm0, %eax
	andl	$1, %eax
	vpbroadcastd	%xmm1, %ymm1
	vpand	%ymm1, %ymm5, %ymm5
	vpand	%ymm1, %ymm3, %ymm3
	vpand	%ymm1, %ymm10, %ymm10
	negl	%eax
	vmovd	%eax, %xmm0
	xorl	%eax, %eax
	vpbroadcastd	%xmm0, %ymm0
	vpxor	224(%rsp), %ymm0, %ymm15
	vpxor	%ymm0, %ymm4, %ymm6
	vpxor	%ymm0, %ymm9, %ymm1
	vpshufd	$216, %ymm5, %ymm0
	vmovdqa	%ymm6, 640(%rsp)
	vpermq	$216, %ymm0, %ymm0
	vmovdqa	.LC8(%rip), %ymm6
	vpshufb	.LC7(%rip), %ymm0, %ymm0
	vmovdqa	%ymm1, 608(%rsp)
	vmovdqa	.LC9(%rip), %ymm1
	vpand	%ymm6, %ymm0, %ymm2
	vpsrld	$4, %ymm0, %ymm0
	vpand	%ymm6, %ymm0, %ymm0
	vperm2i128	$32, %ymm0, %ymm2, %ymm4
	vperm2i128	$49, %ymm0, %ymm2, %ymm2
	vpsrld	$1, %ymm4, %ymm7
	vpsrld	$1, %ymm2, %ymm0
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm7, %ymm7
	vpand	%ymm1, %ymm0, %ymm0
	vpand	%ymm1, %ymm2, %ymm2
	vpunpckldq	%ymm7, %ymm4, %ymm8
	vpunpckhdq	%ymm7, %ymm4, %ymm4
	vpunpckldq	%ymm0, %ymm2, %ymm7
	vpunpckhdq	%ymm0, %ymm2, %ymm2
	vmovdqa	.LC10(%rip), %ymm0
	vpand	%ymm0, %ymm8, %ymm14
	vpand	%ymm0, %ymm4, %ymm12
	vpand	%ymm0, %ymm7, %ymm11
	vpsrld	$2, %ymm8, %ymm8
	vpsrld	$2, %ymm4, %ymm4
	vpand	%ymm0, %ymm2, %ymm9
	vpsrld	$2, %ymm7, %ymm7
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm8, %ymm8
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm7, %ymm7
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm8, %ymm14, %ymm13
	vpunpckhqdq	%ymm8, %ymm14, %ymm8
	vpunpcklqdq	%ymm4, %ymm12, %ymm14
	vmovdqa	%ymm8, 2432(%rsp)
	vpunpckhqdq	%ymm4, %ymm12, %ymm4
	vpunpcklqdq	%ymm7, %ymm11, %ymm12
	vpunpckhqdq	%ymm7, %ymm11, %ymm7
	vmovdqa	%ymm7, 2560(%rsp)
	vpunpcklqdq	%ymm2, %ymm9, %ymm11
	vpunpckhqdq	%ymm2, %ymm9, %ymm2
	vmovdqa	%ymm2, 2624(%rsp)
	vpshufd	$216, %ymm3, %ymm2
	vmovdqa	%ymm4, 2496(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC7(%rip), %ymm2, %ymm2
	vmovdqa	%ymm14, 2464(%rsp)
	vmovdqa	%ymm12, 2528(%rsp)
	vpand	%ymm6, %ymm2, %ymm7
	vpsrld	$4, %ymm2, %ymm2
	vmovdqa	%ymm11, 2592(%rsp)
	vpand	%ymm6, %ymm2, %ymm2
	vmovdqa	%ymm13, 2400(%rsp)
	vperm2i128	$32, %ymm2, %ymm7, %ymm4
	vperm2i128	$49, %ymm2, %ymm7, %ymm2
	vpsrld	$1, %ymm2, %ymm9
	vpsrld	$1, %ymm4, %ymm7
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm7, %ymm7
	vpand	%ymm1, %ymm9, %ymm9
	vpand	%ymm1, %ymm4, %ymm4
	vpunpckldq	%ymm7, %ymm4, %ymm8
	vpunpckhdq	%ymm7, %ymm4, %ymm4
	vpunpckldq	%ymm9, %ymm2, %ymm7
	vpunpckhdq	%ymm9, %ymm2, %ymm2
	vpand	%ymm0, %ymm8, %ymm14
	vpand	%ymm0, %ymm4, %ymm12
	vpand	%ymm0, %ymm7, %ymm11
	vpsrld	$2, %ymm8, %ymm8
	vpand	%ymm0, %ymm2, %ymm9
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm7, %ymm7
	vpand	%ymm0, %ymm8, %ymm8
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm7, %ymm7
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm8, %ymm14, %ymm13
	vpunpckhqdq	%ymm8, %ymm14, %ymm8
	vmovdqa	%ymm8, 2688(%rsp)
	vpunpcklqdq	%ymm4, %ymm12, %ymm14
	vpunpckhqdq	%ymm4, %ymm12, %ymm4
	vpunpcklqdq	%ymm7, %ymm11, %ymm12
	vmovdqa	%ymm4, 2752(%rsp)
	vpunpckhqdq	%ymm7, %ymm11, %ymm7
	vpunpcklqdq	%ymm2, %ymm9, %ymm11
	vpunpckhqdq	%ymm2, %ymm9, %ymm2
	vmovdqa	%ymm2, 2880(%rsp)
	vpshufd	$216, %ymm10, %ymm2
	vmovdqa	%ymm7, 2816(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC7(%rip), %ymm2, %ymm2
	vmovdqa	%ymm14, 2720(%rsp)
	vmovdqa	%ymm12, 2784(%rsp)
	vpand	%ymm6, %ymm2, %ymm7
	vpsrld	$4, %ymm2, %ymm2
	vmovdqa	%ymm11, 2848(%rsp)
	vpand	%ymm6, %ymm2, %ymm2
	vmovdqa	%ymm13, 2656(%rsp)
	vperm2i128	$32, %ymm2, %ymm7, %ymm4
	vperm2i128	$49, %ymm2, %ymm7, %ymm2
	vpsrld	$1, %ymm2, %ymm9
	vpsrld	$1, %ymm4, %ymm7
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm7, %ymm7
	vpand	%ymm1, %ymm9, %ymm9
	vpand	%ymm1, %ymm4, %ymm4
	vpunpckldq	%ymm7, %ymm4, %ymm8
	vpunpckhdq	%ymm7, %ymm4, %ymm4
	vpunpckldq	%ymm9, %ymm2, %ymm7
	vpunpckhdq	%ymm9, %ymm2, %ymm2
	vpand	%ymm0, %ymm8, %ymm14
	vpand	%ymm0, %ymm4, %ymm12
	vpand	%ymm0, %ymm7, %ymm11
	vpsrld	$2, %ymm8, %ymm8
	vpand	%ymm0, %ymm2, %ymm9
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm7, %ymm7
	vpand	%ymm0, %ymm8, %ymm8
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm7, %ymm7
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm8, %ymm14, %ymm13
	vpunpckhqdq	%ymm8, %ymm14, %ymm8
	vmovdqa	%ymm13, 2912(%rsp)
	vpunpcklqdq	%ymm4, %ymm12, %ymm14
	vpunpckhqdq	%ymm4, %ymm12, %ymm4
	vpunpcklqdq	%ymm7, %ymm11, %ymm12
	vmovdqa	%ymm8, 2944(%rsp)
	vpunpckhqdq	%ymm7, %ymm11, %ymm7
	vpunpcklqdq	%ymm2, %ymm9, %ymm11
	vpunpckhqdq	%ymm2, %ymm9, %ymm2
	vmovdqa	%ymm4, 3008(%rsp)
	vmovdqa	%ymm12, 3040(%rsp)
	vmovdqa	%ymm14, 2976(%rsp)
	vmovdqa	%ymm7, 3072(%rsp)
	vmovdqa	%ymm2, 3136(%rsp)
	vpand	%ymm15, %ymm5, %ymm2
	vmovdqa	%ymm11, 3104(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC7(%rip), %ymm2, %ymm2
	vpand	%ymm6, %ymm2, %ymm5
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm6, %ymm2, %ymm2
	vperm2i128	$32, %ymm2, %ymm5, %ymm4
	vperm2i128	$49, %ymm2, %ymm5, %ymm2
	vpsrld	$1, %ymm2, %ymm8
	vpsrld	$1, %ymm4, %ymm5
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm5, %ymm5
	vpand	%ymm1, %ymm8, %ymm8
	vpand	%ymm1, %ymm4, %ymm4
	vpunpckldq	%ymm5, %ymm4, %ymm7
	vpunpckhdq	%ymm5, %ymm4, %ymm4
	vpunpckldq	%ymm8, %ymm2, %ymm5
	vpunpckhdq	%ymm8, %ymm2, %ymm2
	vpand	%ymm0, %ymm7, %ymm13
	vpand	%ymm0, %ymm4, %ymm11
	vpand	%ymm0, %ymm5, %ymm9
	vpsrld	$2, %ymm7, %ymm7
	vpand	%ymm0, %ymm2, %ymm8
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm5, %ymm5
	vpand	%ymm0, %ymm7, %ymm7
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm5, %ymm5
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm7, %ymm13, %ymm12
	vpunpckhqdq	%ymm7, %ymm13, %ymm7
	vmovdqa	%ymm12, 3168(%rsp)
	vpunpcklqdq	%ymm4, %ymm11, %ymm13
	vpunpckhqdq	%ymm4, %ymm11, %ymm4
	vpunpcklqdq	%ymm5, %ymm9, %ymm11
	vmovdqa	%ymm4, 3264(%rsp)
	vpunpckhqdq	%ymm5, %ymm9, %ymm5
	vpunpcklqdq	%ymm2, %ymm8, %ymm9
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vmovdqa	%ymm2, 3392(%rsp)
	vpand	640(%rsp), %ymm3, %ymm2
	vmovdqa	%ymm5, 3328(%rsp)
	vmovdqa	%ymm9, 3360(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vmovdqa	%ymm7, 3200(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC7(%rip), %ymm2, %ymm2
	vmovdqa	%ymm11, 3296(%rsp)
	vmovdqa	%ymm13, 3232(%rsp)
	vpand	%ymm6, %ymm2, %ymm3
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm6, %ymm2, %ymm2
	vperm2i128	$32, %ymm2, %ymm3, %ymm4
	vperm2i128	$49, %ymm2, %ymm3, %ymm2
	vpsrld	$1, %ymm4, %ymm3
	vpsrld	$1, %ymm2, %ymm7
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm3, %ymm8
	vpand	%ymm1, %ymm4, %ymm3
	vpand	%ymm1, %ymm7, %ymm7
	vpunpckldq	%ymm8, %ymm3, %ymm5
	vpunpckhdq	%ymm8, %ymm3, %ymm3
	vpunpckldq	%ymm7, %ymm2, %ymm4
	vpand	%ymm0, %ymm5, %ymm9
	vpand	%ymm0, %ymm3, %ymm8
	vpunpckhdq	%ymm7, %ymm2, %ymm2
	vpsrld	$2, %ymm5, %ymm5
	vpsrld	$2, %ymm3, %ymm3
	vpand	%ymm0, %ymm4, %ymm11
	vpand	%ymm0, %ymm5, %ymm5
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm2, %ymm7
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm2, %ymm2
	vpunpcklqdq	%ymm5, %ymm9, %ymm12
	vpunpckhqdq	%ymm5, %ymm9, %ymm5
	vpunpcklqdq	%ymm3, %ymm8, %ymm9
	vpunpckhqdq	%ymm3, %ymm8, %ymm3
	vmovdqa	%ymm12, 3424(%rsp)
	vmovdqa	%ymm3, 3520(%rsp)
	vpand	%ymm0, %ymm4, %ymm4
	vpand	608(%rsp), %ymm10, %ymm3
	vpand	%ymm0, %ymm2, %ymm2
	vmovdqa	%ymm5, 3456(%rsp)
	vpunpcklqdq	%ymm4, %ymm11, %ymm8
	vpunpckhqdq	%ymm4, %ymm11, %ymm4
	vpunpcklqdq	%ymm2, %ymm7, %ymm11
	vpshufd	$216, %ymm3, %ymm3
	vpunpckhqdq	%ymm2, %ymm7, %ymm2
	vmovdqa	%ymm4, 3584(%rsp)
	vmovdqa	%ymm2, 3648(%rsp)
	vpermq	$216, %ymm3, %ymm3
	vpshufb	.LC7(%rip), %ymm3, %ymm3
	vmovdqa	%ymm8, 3552(%rsp)
	vmovdqa	%ymm9, 3488(%rsp)
	vpand	%ymm6, %ymm3, %ymm2
	vpsrld	$4, %ymm3, %ymm3
	vmovdqa	%ymm11, 3616(%rsp)
	vpand	%ymm6, %ymm3, %ymm6
	vperm2i128	$32, %ymm6, %ymm2, %ymm3
	vperm2i128	$49, %ymm6, %ymm2, %ymm2
	vpsrld	$1, %ymm3, %ymm4
	vpsrld	$1, %ymm2, %ymm6
	vpand	%ymm1, %ymm3, %ymm3
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm2, %ymm1
	vpunpckldq	%ymm4, %ymm3, %ymm5
	vpunpckhdq	%ymm4, %ymm3, %ymm3
	vpunpckldq	%ymm6, %ymm1, %ymm4
	vpand	%ymm0, %ymm4, %ymm8
	vpunpckhdq	%ymm6, %ymm1, %ymm1
	vpand	%ymm0, %ymm3, %ymm7
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm3, %ymm2
	vpand	%ymm0, %ymm5, %ymm6
	vpand	%ymm0, %ymm4, %ymm3
	vpsrld	$2, %ymm5, %ymm5
	vpand	%ymm0, %ymm1, %ymm4
	vpsrld	$2, %ymm1, %ymm1
	vpand	%ymm0, %ymm5, %ymm5
	vpand	%ymm0, %ymm2, %ymm2
	vpand	%ymm0, %ymm1, %ymm0
	vpunpcklqdq	%ymm5, %ymm6, %ymm9
	vpunpckhqdq	%ymm5, %ymm6, %ymm1
	vpunpcklqdq	%ymm2, %ymm7, %ymm10
	vpunpcklqdq	%ymm3, %ymm8, %ymm6
	vpunpcklqdq	%ymm0, %ymm4, %ymm5
	vmovdqa	%ymm9, 3680(%rsp)
	vpunpckhqdq	%ymm2, %ymm7, %ymm2
	vpunpckhqdq	%ymm3, %ymm8, %ymm3
	vpunpckhqdq	%ymm0, %ymm4, %ymm0
	vmovdqa	%ymm1, 3712(%rsp)
	vmovdqa	%ymm10, 3744(%rsp)
	vmovdqa	%ymm2, 3776(%rsp)
	vmovdqa	%ymm6, 3808(%rsp)
	vmovdqa	%ymm3, 3840(%rsp)
	vmovdqa	%ymm5, 3872(%rsp)
	vmovdqa	%ymm0, 3904(%rsp)
	.p2align 4,,10
	.p2align 3
.L11:
	vmovdqa	(%rdi,%rax), %ymm1
	vpaddb	(%rdx,%rax), %ymm1, %ymm0
	vmovdqa	%ymm0, (%r9,%rax)
	addq	$32, %rax
	cmpq	$768, %rax
	jne	.L11
	leaq	4672(%rsp), %rax
	.p2align 4,,10
	.p2align 3
.L12:
	vmovdqa	(%rax), %ymm5
	addq	$32, %rcx
	subq	$32, %rax
	vperm2i128	$1, %ymm5, %ymm5, %ymm0
	vpshufb	.LC5(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rcx)
	cmpq	%rsi, %rcx
	jne	.L12
	leaq	1632(%rsp), %rcx
	vpxor	%xmm0, %xmm0, %xmm0
	movl	$768, %edx
	movq	%r8, %rsi
	movl	$0, 5536(%rsp)
	movq	%rcx, %rdi
	vmovaps	%xmm0, 5472(%rsp)
	vmovaps	%xmm0, 5488(%rsp)
	vmovaps	%xmm0, 5504(%rsp)
	vmovaps	%xmm0, 5520(%rsp)
	vzeroupper
	call	memcpy@PLT
	vpxor	%xmm2, %xmm2, %xmm2
	movq	%rax, %rcx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L13:
	vmovdqa	(%rcx,%rax), %ymm1
	vmovdqa	(%rcx,%rax), %ymm5
	vpunpcklbw	%ymm2, %ymm1, %ymm1
	vpunpckhbw	%ymm2, %ymm5, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm3
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vmovdqu	%ymm3, (%rbx,%rax,2)
	vmovdqu	%ymm0, 32(%rbx,%rax,2)
	addq	$32, %rax
	cmpq	$704, %rax
	jne	.L13
	xorl	%eax, %eax
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5296:
	.size	CRYPTO_NAMESPACETOP, .-CRYPTO_NAMESPACETOP
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.quad	1
	.quad	0
	.quad	0
	.quad	0
	.align 32
.LC1:
	.quad	281474976710655
	.quad	140737488355327
	.quad	140737488355327
	.quad	140737488355327
	.align 32
.LC2:
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.align 32
.LC3:
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
.LC4:
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.align 32
.LC5:
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
.LC6:
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.align 32
.LC7:
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	1
	.byte	5
	.byte	9
	.byte	13
	.byte	2
	.byte	6
	.byte	10
	.byte	14
	.byte	3
	.byte	7
	.byte	11
	.byte	15
	.byte	16
	.byte	20
	.byte	24
	.byte	28
	.byte	17
	.byte	21
	.byte	25
	.byte	29
	.byte	18
	.byte	22
	.byte	26
	.byte	30
	.byte	19
	.byte	23
	.byte	27
	.byte	31
	.align 32
.LC8:
	.quad	1085102592571150095
	.quad	1085102592571150095
	.quad	1085102592571150095
	.quad	1085102592571150095
	.align 32
.LC9:
	.quad	361700864190383365
	.quad	361700864190383365
	.quad	361700864190383365
	.quad	361700864190383365
	.align 32
.LC10:
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
