	.file	"core.c"
	.text
	.p2align 4,,15
	.globl	crypto_core_invhrss701_faster821
	.type	crypto_core_invhrss701_faster821, @function
crypto_core_invhrss701_faster821:
.LFB5187:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	xorl	%ecx, %ecx
	vpcmpeqd	%ymm7, %ymm7, %ymm7
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$5760, %rsp
	movzbl	1400(%rsi), %edi
	vmovdqa	.LC2(%rip), %ymm3
	vmovdqa	.LC3(%rip), %ymm6
	vmovdqa	.LC4(%rip), %ymm5
	notl	%edi
	vmovdqa	.LC5(%rip), %ymm4
	andl	$3, %edi
	movsbl	%dil, %eax
	subl	$3, %eax
	sarl	$5, %eax
	andl	%eax, %edi
	incl	%edi
	vmovd	%edi, %xmm8
	vpbroadcastb	%xmm8, %ymm8
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	32(%rsi,%rcx,2), %ymm3, %ymm0
	vpand	(%rsi,%rcx,2), %ymm3, %ymm1
	vpackuswb	%ymm0, %ymm1, %ymm1
	vpermq	$216, %ymm1, %ymm1
	vpand	%ymm1, %ymm6, %ymm1
	vpaddb	%ymm8, %ymm1, %ymm1
	vextracti128	$0x1, %ymm1, %xmm2
	vpmovsxbw	%xmm1, %ymm0
	vpmovsxbw	%xmm2, %ymm2
	vpsraw	$2, %ymm0, %ymm0
	vpsraw	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm3, %ymm0
	vpand	%ymm2, %ymm3, %ymm2
	vpackuswb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm2
	vpand	%ymm6, %ymm1, %ymm0
	vpaddb	%ymm2, %ymm0, %ymm0
	vpmovsxbw	%xmm0, %ymm1
	vextracti128	$0x1, %ymm0, %xmm9
	vpmovsxwd	%xmm1, %ymm2
	vextracti128	$0x1, %ymm1, %xmm1
	vpmovsxbw	%xmm9, %ymm9
	vpmovsxwd	%xmm1, %ymm1
	vpaddd	%ymm5, %ymm2, %ymm2
	vpaddd	%ymm5, %ymm1, %ymm1
	vpsrad	$5, %ymm2, %ymm2
	vpsrad	$5, %ymm1, %ymm1
	vpand	%ymm2, %ymm4, %ymm2
	vpand	%ymm1, %ymm4, %ymm1
	vpackusdw	%ymm1, %ymm2, %ymm1
	vpmovsxwd	%xmm9, %ymm2
	vextracti128	$0x1, %ymm9, %xmm9
	vpmovsxwd	%xmm9, %ymm9
	vpaddd	%ymm5, %ymm2, %ymm2
	vpermq	$216, %ymm1, %ymm1
	vpaddd	%ymm5, %ymm9, %ymm9
	vpsrad	$5, %ymm2, %ymm2
	vpand	%ymm1, %ymm3, %ymm1
	vpsrad	$5, %ymm9, %ymm9
	vpand	%ymm2, %ymm4, %ymm2
	vpand	%ymm9, %ymm4, %ymm9
	vpackusdw	%ymm9, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm2, %ymm3, %ymm2
	vpackuswb	%ymm2, %ymm1, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm2, %ymm0, %ymm0
	vpaddb	%ymm7, %ymm0, %ymm0
	vmovdqa	%ymm0, 1056(%rsp,%rcx)
	addq	$32, %rcx
	cmpq	$672, %rcx
	jne	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	movzbl	(%rsi,%rcx,2), %eax
	andl	$3, %eax
	addl	%edi, %eax
	movl	%eax, %edx
	sarb	$2, %al
	andl	$3, %edx
	addl	%edx, %eax
	movsbl	%al, %edx
	subl	$3, %edx
	sarl	$5, %edx
	andl	%edx, %eax
	decl	%eax
	movb	%al, 1056(%rsp,%rcx)
	incq	%rcx
	cmpq	$700, %rcx
	jne	.L3
	leaq	4896(%rsp), %rdx
	leaq	1792(%rsp), %rax
	leaq	768(%rdx), %rcx
	.p2align 4,,10
	.p2align 3
.L4:
	vmovdqa	(%rax), %ymm1
	addq	$32, %rdx
	subq	$32, %rax
	vperm2i128	$1, %ymm1, %ymm1, %ymm0
	vpshufb	.LC6(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%rdx, %rcx
	jne	.L4
	vpxor	%xmm0, %xmm0, %xmm0
	vmovdqa	.LC7(%rip), %ymm2
	xorl	%eax, %eax
	movl	$0, 5728(%rsp)
	vmovaps	%xmm0, 5664(%rsp)
	leaq	4964(%rsp), %rdx
	vmovaps	%xmm0, 5680(%rsp)
	vmovaps	%xmm0, 5696(%rsp)
	vmovaps	%xmm0, 5712(%rsp)
	.p2align 4,,10
	.p2align 3
.L5:
	vmovdqu	(%rdx,%rax), %ymm1
	vpand	%ymm2, %ymm1, %ymm0
	vmovdqa	%ymm0, 3360(%rsp,%rax)
	vpmovsxbw	%xmm1, %ymm0
	vextracti128	$0x1, %ymm1, %xmm1
	vpmovsxbw	%xmm1, %ymm1
	vpsraw	$1, %ymm0, %ymm0
	vpsraw	$1, %ymm1, %ymm1
	vpand	%ymm0, %ymm3, %ymm0
	vpand	%ymm1, %ymm3, %ymm1
	vpackuswb	%ymm1, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm2, %ymm0
	vmovdqa	%ymm0, 4128(%rsp,%rax)
	addq	$32, %rax
	cmpq	$768, %rax
	jne	.L5
	vmovdqa	3360(%rsp), %ymm1
	movl	$256, %esi
	vpunpckldq	3392(%rsp), %ymm1, %ymm7
	movl	$-1, %eax
	vmovdqa	3424(%rsp), %ymm4
	vpunpckhdq	3392(%rsp), %ymm1, %ymm1
	vpunpckhdq	3456(%rsp), %ymm4, %ymm3
	vmovdqa	3552(%rsp), %ymm2
	vmovdqa	3488(%rsp), %ymm4
	vpslld	$2, %ymm1, %ymm1
	vpunpckhdq	3520(%rsp), %ymm4, %ymm0
	vpunpckhdq	3584(%rsp), %ymm2, %ymm2
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm7, %ymm1, %ymm1
	vmovdqa	3424(%rsp), %ymm6
	vpunpckldq	3520(%rsp), %ymm4, %ymm5
	vpslld	$2, %ymm0, %ymm0
	vpunpckldq	3456(%rsp), %ymm6, %ymm6
	vmovdqa	3552(%rsp), %ymm4
	vpslld	$2, %ymm2, %ymm2
	vpunpckldq	3584(%rsp), %ymm4, %ymm4
	vpor	%ymm6, %ymm3, %ymm3
	vpor	%ymm5, %ymm0, %ymm0
	vmovdqa	3680(%rsp), %ymm6
	vpunpckldq	3712(%rsp), %ymm6, %ymm6
	vpor	%ymm4, %ymm2, %ymm2
	vpunpcklqdq	%ymm3, %ymm1, %ymm4
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpunpcklqdq	%ymm2, %ymm0, %ymm3
	vpslld	$1, %ymm1, %ymm1
	vpunpckhqdq	%ymm2, %ymm0, %ymm0
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm4, %ymm1
	vmovdqa	3680(%rsp), %ymm4
	vpunpckhdq	3712(%rsp), %ymm4, %ymm2
	vmovdqa	3744(%rsp), %ymm4
	vpor	%ymm0, %ymm3, %ymm0
	vpunpckldq	3776(%rsp), %ymm4, %ymm5
	vperm2i128	$32, %ymm0, %ymm1, %ymm15
	vpslld	$2, %ymm2, %ymm2
	vperm2i128	$49, %ymm0, %ymm1, %ymm1
	vpslld	$4, %ymm1, %ymm0
	vpor	%ymm6, %ymm2, %ymm2
	vpor	%ymm0, %ymm15, %ymm15
	vpunpckhdq	3776(%rsp), %ymm4, %ymm0
	vmovdqa	3808(%rsp), %ymm4
	vpshufb	.LC8(%rip), %ymm15, %ymm15
	vpunpckldq	3840(%rsp), %ymm4, %ymm3
	vpunpckhdq	3840(%rsp), %ymm4, %ymm4
	vpermq	$216, %ymm15, %ymm15
	vpslld	$2, %ymm0, %ymm0
	vpshufd	$216, %ymm15, %ymm1
	vpslld	$2, %ymm4, %ymm4
	vpor	%ymm5, %ymm0, %ymm0
	vmovdqa	%ymm1, 384(%rsp)
	vpor	%ymm3, %ymm4, %ymm4
	vmovdqa	3616(%rsp), %ymm1
	vpunpckldq	3648(%rsp), %ymm1, %ymm7
	vpunpckhdq	3648(%rsp), %ymm1, %ymm1
	vpslld	$2, %ymm1, %ymm1
	vpor	%ymm7, %ymm1, %ymm1
	vpunpcklqdq	%ymm2, %ymm1, %ymm3
	vpunpckhqdq	%ymm2, %ymm1, %ymm1
	vpunpcklqdq	%ymm4, %ymm0, %ymm2
	vpunpckhqdq	%ymm4, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm3, %ymm1
	vpor	%ymm0, %ymm2, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm6
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm6, %ymm6
	vpshufb	.LC8(%rip), %ymm6, %ymm6
	vpermq	$216, %ymm6, %ymm6
	vpshufd	$216, %ymm6, %ymm1
	vmovdqa	3872(%rsp), %ymm6
	vpunpckldq	3904(%rsp), %ymm6, %ymm7
	vpunpckhdq	3904(%rsp), %ymm6, %ymm2
	vmovdqa	%ymm1, %ymm11
	vmovdqa	4000(%rsp), %ymm4
	vmovdqa	3936(%rsp), %ymm1
	vpunpckhdq	4032(%rsp), %ymm4, %ymm0
	vpunpckhdq	3968(%rsp), %ymm1, %ymm3
	vpslld	$2, %ymm2, %ymm2
	vmovdqa	4064(%rsp), %ymm4
	vpunpckldq	4096(%rsp), %ymm4, %ymm5
	vpslld	$2, %ymm0, %ymm0
	vpor	%ymm2, %ymm7, %ymm2
	vmovdqa	3936(%rsp), %ymm6
	vpunpckhdq	4096(%rsp), %ymm4, %ymm4
	vpslld	$2, %ymm3, %ymm3
	vpunpckldq	3968(%rsp), %ymm6, %ymm6
	vmovdqa	4000(%rsp), %ymm1
	vpunpckldq	4032(%rsp), %ymm1, %ymm1
	vpslld	$2, %ymm4, %ymm4
	vpor	%ymm3, %ymm6, %ymm3
	vpor	%ymm4, %ymm5, %ymm4
	vpor	%ymm0, %ymm1, %ymm1
	vpunpcklqdq	%ymm3, %ymm2, %ymm0
	vmovdqa	4256(%rsp), %ymm5
	vpunpckhqdq	%ymm3, %ymm2, %ymm2
	vpunpcklqdq	%ymm4, %ymm1, %ymm3
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vpslld	$1, %ymm2, %ymm2
	vpslld	$1, %ymm1, %ymm1
	vpor	%ymm1, %ymm3, %ymm1
	vpor	%ymm2, %ymm0, %ymm0
	vmovdqa	4320(%rsp), %ymm3
	vpunpckhdq	4352(%rsp), %ymm3, %ymm3
	vperm2i128	$32, %ymm1, %ymm0, %ymm13
	vperm2i128	$49, %ymm1, %ymm0, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm0, %ymm13, %ymm13
	vpunpckhdq	4288(%rsp), %ymm5, %ymm0
	vmovdqa	4320(%rsp), %ymm5
	vpshufb	.LC8(%rip), %ymm13, %ymm13
	vpunpckldq	4352(%rsp), %ymm5, %ymm5
	vpermq	$216, %ymm13, %ymm13
	vpslld	$2, %ymm0, %ymm0
	vpshufd	$216, %ymm13, %ymm6
	vpor	%ymm5, %ymm3, %ymm3
	vmovdqa	%ymm11, %ymm13
	vmovdqa	%ymm6, %ymm15
	vmovdqa	4128(%rsp), %ymm6
	vpunpckldq	4160(%rsp), %ymm6, %ymm4
	vpunpckhdq	4160(%rsp), %ymm6, %ymm2
	vmovdqa	4192(%rsp), %ymm6
	vpunpckhdq	4224(%rsp), %ymm6, %ymm1
	vpunpckldq	4224(%rsp), %ymm6, %ymm7
	vmovdqa	4256(%rsp), %ymm6
	vpslld	$2, %ymm2, %ymm2
	vpunpckldq	4288(%rsp), %ymm6, %ymm6
	vpslld	$2, %ymm1, %ymm1
	vpor	%ymm4, %ymm2, %ymm4
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm6, %ymm0, %ymm0
	vmovdqa	4448(%rsp), %ymm6
	vpunpcklqdq	%ymm1, %ymm4, %ymm2
	vpunpckhqdq	%ymm1, %ymm4, %ymm1
	vpunpcklqdq	%ymm3, %ymm0, %ymm4
	vpunpckhqdq	%ymm3, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm2, %ymm1
	vpor	%ymm0, %ymm4, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm9
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vmovdqa	4384(%rsp), %ymm1
	vpunpckldq	4416(%rsp), %ymm1, %ymm7
	vpunpckhdq	4416(%rsp), %ymm1, %ymm1
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm9, %ymm9
	vpshufb	.LC8(%rip), %ymm9, %ymm9
	vpslld	$2, %ymm1, %ymm1
	vpunpckldq	4480(%rsp), %ymm6, %ymm6
	vmovdqa	4448(%rsp), %ymm4
	vpunpckhdq	4480(%rsp), %ymm4, %ymm2
	vpor	%ymm7, %ymm1, %ymm1
	vpermq	$216, %ymm9, %ymm9
	vmovdqa	4512(%rsp), %ymm4
	vpunpckldq	4544(%rsp), %ymm4, %ymm5
	vpshufd	$216, %ymm9, %ymm14
	vpunpckhdq	4544(%rsp), %ymm4, %ymm0
	vmovdqa	4576(%rsp), %ymm4
	vpslld	$2, %ymm2, %ymm2
	vmovdqa	%ymm14, %ymm11
	vpunpckldq	4608(%rsp), %ymm4, %ymm3
	vpunpckhdq	4608(%rsp), %ymm4, %ymm4
	vpslld	$2, %ymm0, %ymm0
	vpor	%ymm6, %ymm2, %ymm2
	vmovdqa	4768(%rsp), %ymm6
	vpunpckhdq	4800(%rsp), %ymm6, %ymm6
	vpor	%ymm5, %ymm0, %ymm0
	vpslld	$2, %ymm4, %ymm4
	vpor	%ymm3, %ymm4, %ymm4
	vpunpcklqdq	%ymm2, %ymm1, %ymm3
	vpunpckhqdq	%ymm2, %ymm1, %ymm1
	vpunpcklqdq	%ymm4, %ymm0, %ymm2
	vpslld	$1, %ymm1, %ymm1
	vpunpckhqdq	%ymm4, %ymm0, %ymm0
	vmovdqa	4832(%rsp), %ymm4
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm3, %ymm1
	vmovdqa	4832(%rsp), %ymm3
	vpunpckhdq	4864(%rsp), %ymm3, %ymm3
	vpunpckldq	4864(%rsp), %ymm4, %ymm4
	vpor	%ymm0, %ymm2, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm8
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm8, %ymm8
	vpshufb	.LC8(%rip), %ymm8, %ymm8
	vpermq	$216, %ymm8, %ymm8
	vpshufd	$216, %ymm8, %ymm1
	vmovdqa	%ymm15, %ymm8
	vmovdqa	%ymm1, %ymm10
	vmovdqa	4640(%rsp), %ymm1
	vpunpckhdq	4672(%rsp), %ymm1, %ymm0
	vpunpckldq	4672(%rsp), %ymm1, %ymm2
	vmovdqa	4704(%rsp), %ymm1
	vpunpckhdq	4736(%rsp), %ymm1, %ymm7
	vpslld	$2, %ymm0, %ymm0
	vpunpckldq	4736(%rsp), %ymm1, %ymm5
	vmovdqa	4768(%rsp), %ymm1
	vpor	%ymm0, %ymm2, %ymm2
	vpunpckldq	4800(%rsp), %ymm1, %ymm1
	vpslld	$2, %ymm7, %ymm0
	vpxor	%xmm7, %xmm7, %xmm7
	vmovdqa	%ymm7, 352(%rsp)
	vpor	%ymm0, %ymm5, %ymm5
	vpslld	$2, %ymm6, %ymm0
	vmovdqa	.LC0(%rip), %ymm6
	vmovdqa	%ymm7, 448(%rsp)
	vpor	%ymm0, %ymm1, %ymm1
	vpslld	$2, %ymm3, %ymm0
	vmovdqa	%ymm6, 640(%rsp)
	vpor	%ymm0, %ymm4, %ymm4
	vpunpcklqdq	%ymm5, %ymm2, %ymm0
	vpunpckhqdq	%ymm5, %ymm2, %ymm2
	vpunpcklqdq	%ymm4, %ymm1, %ymm3
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vmovdqa	.LC1(%rip), %ymm6
	vmovdqa	%ymm7, 480(%rsp)
	vpslld	$1, %ymm2, %ymm2
	vpslld	$1, %ymm1, %ymm1
	vmovdqa	%ymm7, %ymm4
	vmovdqa	%ymm7, 544(%rsp)
	vpor	%ymm1, %ymm3, %ymm1
	vpor	%ymm2, %ymm0, %ymm0
	vperm2i128	$32, %ymm1, %ymm0, %ymm12
	vperm2i128	$49, %ymm1, %ymm0, %ymm0
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm12, %ymm12
	vpshufb	.LC8(%rip), %ymm12, %ymm12
	vmovdqa	%ymm6, 416(%rsp)
	vmovdqa	384(%rsp), %ymm14
	vmovdqa	%ymm1, 512(%rsp)
	vpermq	$216, %ymm12, %ymm12
	vmovdqa	%ymm1, 576(%rsp)
	vpshufd	$216, %ymm12, %ymm12
	.p2align 4,,10
	.p2align 3
.L6:
	vpermq	$147, %ymm4, %ymm4
	vpermq	$147, %ymm7, %ymm7
	vmovd	%xmm14, %ecx
	movl	%eax, %r8d
	vmovq	%xmm4, %rdx
	andl	$1, %ecx
	sarl	$31, %r8d
	vmovd	%xmm11, %edi
	addq	%rdx, %rdx
	negl	%ecx
	andl	$1, %edi
	vmovdqa	512(%rsp), %ymm3
	vmovq	%rdx, %xmm0
	vmovq	%xmm7, %rdx
	andl	%ecx, %r8d
	negl	%edi
	addq	%rdx, %rdx
	vpblendd	$3, %ymm0, %ymm4, %ymm1
	vmovdqa	544(%rsp), %ymm4
	vmovdqa	480(%rsp), %ymm15
	vmovdqa	%ymm1, 288(%rsp)
	vmovq	%rdx, %xmm0
	movl	%ecx, %edx
	vmovd	%r8d, %xmm1
	vpblendd	$3, %ymm0, %ymm7, %ymm6
	vpbroadcastd	%xmm1, %ymm1
	vmovdqa	%ymm6, 256(%rsp)
	vmovdqa	576(%rsp), %ymm6
	vpxor	%ymm6, %ymm14, %ymm7
	vmovd	%xmm6, %ecx
	vpand	%ymm1, %ymm7, %ymm7
	andl	$1, %ecx
	vpxor	%ymm6, %ymm7, %ymm5
	vpxor	%ymm3, %ymm13, %ymm6
	vpxor	%ymm7, %ymm14, %ymm14
	negl	%ecx
	vmovdqa	%ymm5, 576(%rsp)
	vpand	%ymm1, %ymm6, %ymm6
	andl	%edx, %ecx
	vmovd	%xmm4, %edx
	vpxor	%ymm3, %ymm6, %ymm2
	andl	$1, %edx
	vpxor	%ymm6, %ymm13, %ymm13
	vmovdqa	416(%rsp), %ymm3
	vmovdqa	%ymm2, 512(%rsp)
	vmovdqa	%ymm4, %ymm2
	negl	%edx
	vpxor	%ymm4, %ymm11, %ymm4
	vpand	%ymm1, %ymm4, %ymm4
	vpxor	%ymm3, %ymm8, %ymm5
	xorl	%edi, %edx
	movl	%eax, %edi
	vpxor	%ymm2, %ymm4, %ymm0
	vpand	%ymm1, %ymm5, %ymm5
	andl	%ecx, %edx
	negl	%edi
	vpxor	%ymm3, %ymm5, %ymm3
	vmovdqa	%ymm0, %ymm2
	vpxor	%ymm5, %ymm8, %ymm5
	xorl	%eax, %edi
	vmovdqa	%ymm0, 544(%rsp)
	vpxor	%ymm4, %ymm11, %ymm0
	andl	%r8d, %edi
	vmovdqa	448(%rsp), %ymm11
	vmovdqa	%ymm3, 416(%rsp)
	vpxor	%ymm15, %ymm10, %ymm3
	xorl	%edi, %eax
	vpand	%ymm1, %ymm3, %ymm3
	vpxor	%ymm11, %ymm12, %ymm9
	decl	%eax
	vmovdqa	%ymm0, 608(%rsp)
	vpxor	%ymm15, %ymm3, %ymm15
	vpand	%ymm1, %ymm9, %ymm9
	vmovd	%ecx, %xmm0
	vpxor	%ymm11, %ymm9, %ymm11
	vpbroadcastd	%xmm0, %ymm0
	vpxor	%ymm3, %ymm10, %ymm10
	vmovdqa	%ymm15, 480(%rsp)
	vmovd	%edx, %xmm15
	vpxor	%ymm9, %ymm12, %ymm9
	vmovdqa	%ymm11, 448(%rsp)
	vpand	576(%rsp), %ymm0, %ymm11
	vpbroadcastd	%xmm15, %ymm15
	vmovdqa	%ymm9, %ymm12
	vmovdqa	%ymm10, 384(%rsp)
	vpxor	480(%rsp), %ymm15, %ymm3
	vpand	512(%rsp), %ymm0, %ymm10
	vpxor	%ymm2, %ymm15, %ymm4
	vpxor	%ymm11, %ymm14, %ymm7
	vmovdqa	%ymm12, 320(%rsp)
	vpand	416(%rsp), %ymm0, %ymm9
	vpand	%ymm4, %ymm11, %ymm4
	vpxor	608(%rsp), %ymm4, %ymm14
	vpxor	%ymm10, %ymm13, %ymm6
	vpand	%ymm3, %ymm10, %ymm3
	vpxor	384(%rsp), %ymm3, %ymm13
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	%ymm9, %ymm5, %ymm5
	vpxor	448(%rsp), %ymm15, %ymm2
	vpor	%ymm6, %ymm13, %ymm13
	vpxor	%ymm7, %ymm4, %ymm4
	vmovaps	%xmm14, 224(%rsp)
	movq	224(%rsp), %rdi
	vpand	%ymm2, %ymm9, %ymm2
	vmovq	%xmm13, %rdx
	vmovq	%xmm13, %r8
	vmovdqa	288(%rsp), %ymm7
	shrq	%rdi
	vpxor	%ymm12, %ymm2, %ymm8
	salq	$63, %rdx
	vpxor	%ymm6, %ymm3, %ymm3
	vpor	%ymm5, %ymm8, %ymm8
	orq	%rdi, %rdx
	shrq	%r8
	vpxor	%ymm5, %ymm2, %ymm2
	vmovq	%rdx, %xmm12
	vmovq	%xmm8, %rdx
	vpxor	608(%rsp), %ymm11, %ymm11
	vmovdqa	640(%rsp), %ymm5
	salq	$63, %rdx
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	vpxor	384(%rsp), %ymm10, %ymm10
	vmovdqa	256(%rsp), %ymm6
	vpand	%ymm4, %ymm11, %ymm11
	vmovq	%xmm8, %rcx
	vpermq	$57, %ymm14, %ymm14
	orq	%r8, %rdx
	vpand	%ymm3, %ymm10, %ymm10
	vmovq	%rdx, %xmm12
	vmovq	%xmm11, %rdi
	shrq	%rcx
	vmovq	%xmm10, %rdx
	shrq	%rdi
	vmovq	%xmm10, %r8
	vpxor	320(%rsp), %ymm9, %ymm9
	salq	$63, %rdx
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	shrq	%r8
	vmovq	%rcx, %xmm12
	vpand	%ymm2, %ymm9, %ymm9
	vpxor	%ymm5, %ymm7, %ymm2
	vpermq	$57, %ymm13, %ymm13
	orq	%rdi, %rdx
	vmovq	%rdx, %xmm4
	vmovq	%xmm9, %rdx
	vmovq	%xmm9, %rcx
	salq	$63, %rdx
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vpand	%ymm1, %ymm2, %ymm2
	orq	%r8, %rdx
	shrq	%rcx
	vpblendd	$3, %ymm4, %ymm11, %ymm11
	vpxor	%ymm7, %ymm2, %ymm4
	vmovq	%rcx, %xmm12
	vmovq	%rdx, %xmm3
	vpand	%ymm0, %ymm4, %ymm0
	vpblendd	$3, %ymm12, %ymm9, %ymm9
	vpblendd	$3, %ymm3, %ymm10, %ymm10
	vpxor	%ymm5, %ymm2, %ymm2
	vpermq	$57, %ymm9, %ymm12
	vpxor	%ymm0, %ymm2, %ymm2
	vpermq	$57, %ymm8, %ymm8
	vmovdqa	352(%rsp), %ymm9
	vpermq	$57, %ymm11, %ymm11
	vpermq	$57, %ymm10, %ymm10
	vpxor	%ymm9, %ymm6, %ymm3
	vpand	%ymm1, %ymm3, %ymm1
	vpxor	%ymm6, %ymm1, %ymm7
	vpxor	%ymm9, %ymm1, %ymm1
	vpxor	%ymm15, %ymm7, %ymm3
	vpand	%ymm3, %ymm0, %ymm3
	vpxor	%ymm3, %ymm1, %ymm5
	vpxor	%ymm0, %ymm1, %ymm1
	vpor	%ymm2, %ymm5, %ymm6
	vpxor	%ymm2, %ymm3, %ymm2
	vmovdqa	%ymm6, 640(%rsp)
	vpand	%ymm2, %ymm1, %ymm1
	vmovdqa	%ymm1, 352(%rsp)
	decl	%esi
	jne	.L6
	vpxor	%xmm5, %xmm5, %xmm5
	movl	$256, %esi
	vmovdqa	%ymm14, 384(%rsp)
	vmovdqa	%ymm11, %ymm14
	vmovdqa	%ymm13, 320(%rsp)
	vmovdqa	%ymm5, %ymm6
	vmovdqa	%ymm8, 224(%rsp)
	vmovdqa	%ymm5, 288(%rsp)
	vmovdqa	%ymm5, 608(%rsp)
	.p2align 4,,10
	.p2align 3
.L7:
	vpermq	$147, %ymm4, %ymm4
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm7, %ymm7
	movl	%eax, %r8d
	vmovq	%xmm4, %rdx
	vmovq	%xmm6, %rcx
	vpermq	$147, %ymm5, %ymm5
	vmovdqa	384(%rsp), %ymm2
	addq	%rcx, %rcx
	leaq	(%rdx,%rdx), %rdi
	shrq	$63, %rdx
	vmovdqa	320(%rsp), %ymm8
	orq	%rcx, %rdx
	vmovq	%rdi, %xmm0
	vmovq	%xmm5, %rcx
	sarl	$31, %r8d
	vpblendd	$3, %ymm0, %ymm4, %ymm1
	vmovq	%rdx, %xmm0
	addq	%rcx, %rcx
	vmovdqa	416(%rsp), %ymm11
	vmovq	%xmm7, %rdx
	vpblendd	$3, %ymm0, %ymm6, %ymm4
	vmovdqa	224(%rsp), %ymm15
	vmovdqa	%ymm1, 128(%rsp)
	leaq	(%rdx,%rdx), %rdi
	shrq	$63, %rdx
	vmovdqa	%ymm4, 96(%rsp)
	vmovdqa	576(%rsp), %ymm4
	orq	%rcx, %rdx
	vmovd	%xmm2, %ecx
	vmovq	%rdi, %xmm0
	vmovdqa	544(%rsp), %ymm3
	andl	$1, %ecx
	vpblendd	$3, %ymm0, %ymm7, %ymm6
	vmovq	%rdx, %xmm0
	negl	%ecx
	vmovdqa	%ymm6, 64(%rsp)
	vpxor	%ymm2, %ymm4, %ymm7
	vmovd	%xmm14, %edi
	andl	%ecx, %r8d
	vpblendd	$3, %ymm0, %ymm5, %ymm5
	movl	%ecx, %edx
	andl	$1, %edi
	vmovd	%r8d, %xmm1
	vmovdqa	%ymm5, 32(%rsp)
	vmovd	%xmm4, %ecx
	negl	%edi
	vmovdqa	512(%rsp), %ymm0
	vpbroadcastd	%xmm1, %ymm1
	vpxor	%ymm11, %ymm15, %ymm5
	andl	$1, %ecx
	vpand	%ymm1, %ymm5, %ymm5
	negl	%ecx
	vpand	%ymm1, %ymm7, %ymm7
	vpxor	%ymm0, %ymm8, %ymm6
	vpxor	%ymm11, %ymm5, %ymm9
	vpxor	%ymm3, %ymm14, %ymm11
	andl	%edx, %ecx
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm11, %ymm11
	vmovd	%xmm3, %edx
	vmovdqa	%ymm9, 416(%rsp)
	vpxor	%ymm0, %ymm6, %ymm0
	vpxor	%ymm4, %ymm7, %ymm13
	andl	$1, %edx
	vpxor	%ymm3, %ymm11, %ymm4
	vpxor	%ymm11, %ymm14, %ymm11
	vpxor	%ymm2, %ymm7, %ymm7
	negl	%edx
	vmovdqa	%ymm0, 512(%rsp)
	xorl	%edi, %edx
	movl	%eax, %edi
	vmovdqa	480(%rsp), %ymm0
	andl	%ecx, %edx
	negl	%edi
	vpxor	%ymm8, %ymm6, %ymm6
	vmovdqa	%ymm11, 256(%rsp)
	vpxor	%ymm0, %ymm10, %ymm3
	vmovd	%edx, %xmm15
	xorl	%eax, %edi
	vmovdqa	%ymm4, 544(%rsp)
	vpand	%ymm1, %ymm3, %ymm3
	vpbroadcastd	%xmm15, %ymm15
	andl	%r8d, %edi
	vmovdqa	%ymm13, 576(%rsp)
	vpxor	%ymm0, %ymm3, %ymm0
	vpxor	%ymm3, %ymm10, %ymm10
	vpxor	%ymm4, %ymm15, %ymm4
	xorl	%edi, %eax
	vmovdqa	448(%rsp), %ymm3
	vmovdqa	%ymm0, %ymm14
	decl	%eax
	vmovdqa	%ymm0, 480(%rsp)
	vmovdqa	%ymm10, 192(%rsp)
	vmovd	%ecx, %xmm0
	vpxor	224(%rsp), %ymm5, %ymm5
	vpxor	%ymm3, %ymm12, %ymm9
	vpbroadcastd	%xmm0, %ymm0
	vpand	512(%rsp), %ymm0, %ymm10
	vpand	%ymm1, %ymm9, %ymm9
	vpand	%ymm13, %ymm0, %ymm11
	vpxor	%ymm3, %ymm9, %ymm3
	vpand	%ymm4, %ymm11, %ymm4
	vpxor	%ymm11, %ymm7, %ymm7
	vmovdqa	%ymm3, 448(%rsp)
	vpxor	%ymm14, %ymm15, %ymm3
	vpxor	%ymm9, %ymm12, %ymm12
	vpxor	%ymm10, %ymm6, %ymm6
	vpxor	256(%rsp), %ymm4, %ymm14
	vpand	%ymm3, %ymm10, %ymm3
	vpxor	192(%rsp), %ymm3, %ymm13
	vmovdqa	%ymm12, 160(%rsp)
	vpand	416(%rsp), %ymm0, %ymm9
	vpxor	%ymm7, %ymm4, %ymm4
	vpxor	%ymm6, %ymm3, %ymm3
	vpor	%ymm7, %ymm14, %ymm14
	vpor	%ymm6, %ymm13, %ymm13
	vpxor	448(%rsp), %ymm15, %ymm2
	vmovq	%xmm13, %rdx
	vpxor	%ymm9, %ymm5, %ymm5
	vmovq	%xmm13, %r8
	vmovaps	%xmm14, 384(%rsp)
	movq	384(%rsp), %rdi
	salq	$63, %rdx
	vpand	%ymm2, %ymm9, %ymm2
	shrq	%r8
	vpxor	%ymm12, %ymm2, %ymm8
	vpxor	%ymm5, %ymm2, %ymm2
	shrq	%rdi
	vpor	%ymm5, %ymm8, %ymm8
	orq	%rdi, %rdx
	vmovq	%xmm8, %rcx
	vmovq	%rdx, %xmm12
	vmovq	%xmm8, %rdx
	shrq	%rcx
	salq	$63, %rdx
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	orq	%r8, %rdx
	vpermq	$57, %ymm14, %ymm14
	vmovdqa	%ymm14, 384(%rsp)
	vmovq	%rdx, %xmm12
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vmovq	%rcx, %xmm12
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vpermq	$57, %ymm13, %ymm14
	vmovdqa	%ymm14, 320(%rsp)
	vpermq	$57, %ymm8, %ymm8
	vmovdqa	%ymm8, 224(%rsp)
	vmovdqa	32(%rsp), %ymm5
	vpxor	256(%rsp), %ymm11, %ymm11
	vpxor	192(%rsp), %ymm10, %ymm10
	vmovdqa	128(%rsp), %ymm7
	vpxor	160(%rsp), %ymm9, %ymm9
	vpand	%ymm4, %ymm11, %ymm11
	vmovdqa	608(%rsp), %ymm8
	vpand	%ymm3, %ymm10, %ymm10
	vmovq	%xmm11, %rdi
	vmovq	%xmm10, %rdx
	vmovq	%xmm10, %r8
	shrq	%rdi
	vpand	%ymm2, %ymm9, %ymm9
	shrq	%r8
	salq	$63, %rdx
	vmovq	%xmm9, %rcx
	orq	%rdi, %rdx
	shrq	%rcx
	vmovq	%rdx, %xmm14
	vmovq	%rcx, %xmm12
	vmovq	%xmm9, %rdx
	vpblendd	$3, %ymm14, %ymm11, %ymm11
	vpblendd	$3, %ymm12, %ymm9, %ymm9
	salq	$63, %rdx
	vpermq	$57, %ymm11, %ymm14
	vpermq	$57, %ymm9, %ymm12
	orq	%r8, %rdx
	vmovdqa	640(%rsp), %ymm11
	vmovdqa	352(%rsp), %ymm9
	vmovq	%rdx, %xmm3
	vpxor	%ymm11, %ymm7, %ymm13
	vpblendd	$3, %ymm3, %ymm10, %ymm10
	vpand	%ymm1, %ymm13, %ymm13
	vpermq	$57, %ymm10, %ymm10
	vpxor	%ymm7, %ymm13, %ymm4
	vmovdqa	96(%rsp), %ymm7
	vpxor	%ymm11, %ymm13, %ymm13
	vpxor	%ymm7, %ymm8, %ymm8
	vpand	%ymm1, %ymm8, %ymm8
	vpxor	%ymm7, %ymm8, %ymm6
	vmovdqa	64(%rsp), %ymm7
	vpxor	608(%rsp), %ymm8, %ymm8
	vpxor	%ymm9, %ymm7, %ymm2
	vpand	%ymm1, %ymm2, %ymm2
	vpxor	%ymm7, %ymm2, %ymm7
	vpxor	%ymm9, %ymm2, %ymm2
	vmovdqa	288(%rsp), %ymm9
	vpxor	%ymm9, %ymm5, %ymm3
	vpand	%ymm1, %ymm3, %ymm1
	vpxor	%ymm15, %ymm7, %ymm3
	vpxor	%ymm5, %ymm1, %ymm5
	vpxor	%ymm9, %ymm1, %ymm1
	vpand	%ymm0, %ymm4, %ymm9
	vpand	%ymm3, %ymm9, %ymm3
	vpxor	%ymm9, %ymm13, %ymm11
	vpand	%ymm0, %ymm6, %ymm0
	vpxor	%ymm2, %ymm3, %ymm13
	vpxor	%ymm2, %ymm9, %ymm2
	vpxor	%ymm0, %ymm8, %ymm8
	vpor	%ymm11, %ymm13, %ymm13
	vpxor	%ymm11, %ymm3, %ymm11
	vmovdqa	%ymm13, 640(%rsp)
	vpand	%ymm11, %ymm2, %ymm9
	vpxor	%ymm15, %ymm5, %ymm2
	vmovdqa	%ymm9, 352(%rsp)
	vpand	%ymm2, %ymm0, %ymm2
	vpxor	%ymm1, %ymm2, %ymm3
	vpxor	%ymm1, %ymm0, %ymm1
	vpor	%ymm8, %ymm3, %ymm3
	vpxor	%ymm8, %ymm2, %ymm8
	vmovdqa	%ymm3, 608(%rsp)
	vpand	%ymm8, %ymm1, %ymm1
	vmovdqa	%ymm1, 288(%rsp)
	decl	%esi
	jne	.L7
	vpxor	%xmm13, %xmm13, %xmm13
	vmovdqa	%ymm12, 96(%rsp)
	movl	$375, %esi
	vmovdqa	%ymm5, %ymm2
	vmovdqa	%ymm10, 160(%rsp)
	vmovdqa	%ymm13, %ymm12
	vmovdqa	%ymm13, 192(%rsp)
	vmovdqa	%ymm13, 256(%rsp)
	vmovdqa	%ymm7, 128(%rsp)
	vmovdqa	%ymm14, 64(%rsp)
	.p2align 4,,10
	.p2align 3
.L8:
	vpermq	$147, %ymm4, %ymm4
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm12, %ymm12
	vmovdqa	544(%rsp), %ymm3
	vmovq	%xmm4, %rcx
	vmovq	%xmm6, %rdx
	vmovq	%xmm12, %rdi
	vmovdqa	%ymm4, 864(%rsp)
	leaq	(%rcx,%rcx), %r8
	addq	%rdi, %rdi
	shrq	$63, %rcx
	vpermq	$147, %ymm2, %ymm2
	vpermq	$147, 128(%rsp), %ymm15
	vpermq	$147, %ymm13, %ymm13
	movq	%r8, 864(%rsp)
	leaq	(%rdx,%rdx), %r8
	shrq	$63, %rdx
	orq	%r8, %rcx
	vmovdqa	%ymm6, 896(%rsp)
	vmovdqa	576(%rsp), %ymm6
	orq	%rdi, %rdx
	vmovq	%xmm13, %rdi
	movq	%rcx, 896(%rsp)
	vmovq	%xmm15, %rcx
	vmovq	%rdx, %xmm0
	leaq	(%rcx,%rcx), %r8
	vmovq	%xmm2, %rdx
	vmovdqa	%ymm15, 960(%rsp)
	vpblendd	$3, %ymm0, %ymm12, %ymm7
	shrq	$63, %rcx
	movq	%r8, 960(%rsp)
	leaq	(%rdx,%rdx), %r8
	vmovdqa	%ymm7, 32(%rsp)
	orq	%r8, %rcx
	movl	%eax, %r8d
	addq	%rdi, %rdi
	shrq	$63, %rdx
	sarl	$31, %r8d
	vmovdqa	64(%rsp), %ymm4
	vmovdqa	384(%rsp), %ymm7
	vmovdqa	%ymm2, 992(%rsp)
	orq	%rdi, %rdx
	vmovdqa	512(%rsp), %ymm5
	vmovdqa	480(%rsp), %ymm15
	movq	%rcx, 992(%rsp)
	vmovd	%xmm7, %ecx
	vpxor	%ymm3, %ymm4, %ymm11
	vmovd	%xmm4, %edi
	andl	$1, %ecx
	vmovq	%rdx, %xmm0
	andl	$1, %edi
	vmovdqa	224(%rsp), %ymm8
	negl	%ecx
	vpblendd	$3, %ymm0, %ymm13, %ymm14
	negl	%edi
	vmovdqa	320(%rsp), %ymm13
	andl	%ecx, %r8d
	movl	%ecx, %edx
	vmovd	%xmm6, %ecx
	vmovdqa	%ymm14, (%rsp)
	vmovd	%r8d, %xmm1
	andl	$1, %ecx
	vpxor	%ymm6, %ymm7, %ymm14
	vmovdqa	416(%rsp), %ymm0
	vpbroadcastd	%xmm1, %ymm1
	negl	%ecx
	vpand	%ymm1, %ymm11, %ymm11
	andl	%edx, %ecx
	vmovd	%xmm3, %edx
	vpxor	%ymm4, %ymm11, %ymm4
	vpxor	%ymm3, %ymm11, %ymm3
	vpand	%ymm1, %ymm14, %ymm14
	andl	$1, %edx
	vmovdqa	160(%rsp), %ymm11
	vpxor	%ymm6, %ymm14, %ymm2
	negl	%edx
	vpxor	%ymm5, %ymm13, %ymm6
	vpand	%ymm1, %ymm6, %ymm6
	xorl	%edi, %edx
	vpxor	%ymm7, %ymm14, %ymm14
	movl	%eax, %edi
	vpxor	%ymm11, %ymm15, %ymm10
	vpxor	%ymm5, %ymm6, %ymm12
	vpxor	%ymm0, %ymm8, %ymm5
	andl	%ecx, %edx
	vpand	%ymm1, %ymm10, %ymm10
	vpand	%ymm1, %ymm5, %ymm5
	vpxor	%ymm13, %ymm6, %ymm6
	negl	%edi
	vpxor	%ymm15, %ymm10, %ymm15
	vpxor	%ymm11, %ymm10, %ymm10
	vpxor	%ymm0, %ymm5, %ymm0
	xorl	%eax, %edi
	vmovdqa	448(%rsp), %ymm11
	andl	%r8d, %edi
	vpxor	%ymm8, %ymm5, %ymm5
	vmovdqa	%ymm4, 128(%rsp)
	vmovdqa	96(%rsp), %ymm4
	xorl	%edi, %eax
	vmovdqa	%ymm0, 416(%rsp)
	vmovd	%ecx, %xmm0
	vmovdqa	%ymm15, 480(%rsp)
	vmovd	%edx, %xmm15
	vpbroadcastd	%xmm0, %ymm0
	decl	%eax
	vpxor	%ymm4, %ymm11, %ymm9
	vpbroadcastd	%xmm15, %ymm15
	vmovdqa	%ymm2, 576(%rsp)
	vmovdqa	%ymm12, 512(%rsp)
	vpand	%ymm1, %ymm9, %ymm9
	vmovdqa	%ymm10, 160(%rsp)
	vpxor	%ymm11, %ymm9, %ymm11
	vpxor	%ymm4, %ymm9, %ymm9
	vpxor	%ymm3, %ymm15, %ymm4
	vmovdqa	%ymm11, 448(%rsp)
	vpand	%ymm2, %ymm0, %ymm11
	vpand	%ymm12, %ymm0, %ymm10
	vmovdqa	%ymm9, 96(%rsp)
	vpxor	%ymm11, %ymm14, %ymm7
	vpand	%ymm4, %ymm11, %ymm4
	vpxor	%ymm10, %ymm6, %ymm6
	vmovdqa	%ymm3, 544(%rsp)
	vpxor	480(%rsp), %ymm15, %ymm3
	vpxor	128(%rsp), %ymm4, %ymm14
	vpxor	%ymm7, %ymm4, %ymm4
	vpand	416(%rsp), %ymm0, %ymm9
	vpxor	448(%rsp), %ymm15, %ymm2
	vpand	%ymm3, %ymm10, %ymm3
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	160(%rsp), %ymm3, %ymm13
	vmovdqa	32(%rsp), %ymm7
	vpand	%ymm2, %ymm9, %ymm2
	vpxor	96(%rsp), %ymm2, %ymm8
	vmovaps	%xmm14, 384(%rsp)
	movq	384(%rsp), %rdi
	vpor	%ymm6, %ymm13, %ymm13
	vpxor	%ymm9, %ymm5, %ymm5
	vpxor	128(%rsp), %ymm11, %ymm11
	vmovq	%xmm13, %rdx
	shrq	%rdi
	vpor	%ymm5, %ymm8, %ymm8
	vpxor	%ymm6, %ymm3, %ymm3
	salq	$63, %rdx
	vmovq	%xmm13, %r8
	vpand	%ymm4, %ymm11, %ymm11
	orq	%rdi, %rdx
	shrq	%r8
	vpxor	96(%rsp), %ymm9, %ymm9
	vpxor	%ymm5, %ymm2, %ymm2
	vmovq	%rdx, %xmm12
	vmovq	%xmm8, %rdx
	vmovq	%xmm11, %rdi
	vmovdqa	608(%rsp), %ymm5
	salq	$63, %rdx
	shrq	%rdi
	vpand	%ymm2, %ymm9, %ymm9
	vmovq	%xmm8, %rcx
	vpxor	160(%rsp), %ymm10, %ymm10
	orq	%r8, %rdx
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	shrq	%rcx
	vmovq	%rdx, %xmm12
	vpermq	$57, %ymm14, %ymm14
	vpxor	896(%rsp), %ymm5, %ymm5
	vpand	%ymm3, %ymm10, %ymm10
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vmovq	%rcx, %xmm12
	vmovdqa	%ymm14, 384(%rsp)
	vmovq	%xmm10, %rdx
	vmovq	%xmm10, %r8
	vmovq	%xmm9, %rcx
	salq	$63, %rdx
	shrq	%r8
	vpermq	$57, %ymm13, %ymm14
	vpand	%ymm1, %ymm5, %ymm5
	orq	%rdi, %rdx
	shrq	%rcx
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vmovdqa	352(%rsp), %ymm13
	vmovq	%rdx, %xmm4
	vmovq	%xmm9, %rdx
	vmovq	%rcx, %xmm2
	vmovdqa	%ymm14, 320(%rsp)
	salq	$63, %rdx
	vpblendd	$3, %ymm4, %ymm11, %ymm11
	vpblendd	$3, %ymm2, %ymm9, %ymm9
	orq	%r8, %rdx
	vpermq	$57, %ymm11, %ymm6
	vpermq	$57, %ymm8, %ymm14
	vmovq	%rdx, %xmm3
	vmovdqa	%ymm6, 64(%rsp)
	vmovdqa	%ymm14, 224(%rsp)
	vpblendd	$3, %ymm3, %ymm10, %ymm10
	vmovdqa	256(%rsp), %ymm3
	vpermq	$57, %ymm10, %ymm6
	vpxor	960(%rsp), %ymm13, %ymm10
	vmovdqa	%ymm6, 160(%rsp)
	vpermq	$57, %ymm9, %ymm6
	vpxor	%ymm7, %ymm3, %ymm3
	vpand	%ymm1, %ymm10, %ymm10
	vmovdqa	%ymm6, 96(%rsp)
	vpand	%ymm1, %ymm3, %ymm3
	vpxor	960(%rsp), %ymm10, %ymm11
	vmovdqa	640(%rsp), %ymm6
	vpxor	%ymm13, %ymm10, %ymm10
	vpxor	%ymm7, %ymm3, %ymm12
	vmovdqa	288(%rsp), %ymm13
	vpxor	864(%rsp), %ymm6, %ymm8
	vpxor	992(%rsp), %ymm13, %ymm9
	vmovdqa	%ymm11, 128(%rsp)
	vpxor	%ymm11, %ymm15, %ymm11
	vpxor	896(%rsp), %ymm5, %ymm6
	vpand	%ymm1, %ymm8, %ymm8
	vpand	%ymm1, %ymm9, %ymm9
	vpxor	864(%rsp), %ymm8, %ymm4
	vpxor	992(%rsp), %ymm9, %ymm2
	vmovdqa	(%rsp), %ymm14
	vpxor	%ymm13, %ymm9, %ymm9
	vmovdqa	192(%rsp), %ymm7
	vpxor	640(%rsp), %ymm8, %ymm8
	vpxor	608(%rsp), %ymm5, %ymm5
	vpxor	256(%rsp), %ymm3, %ymm3
	vpxor	%ymm14, %ymm7, %ymm7
	vpand	%ymm1, %ymm7, %ymm1
	vpxor	%ymm14, %ymm1, %ymm13
	vpand	%ymm0, %ymm4, %ymm14
	vpxor	192(%rsp), %ymm1, %ymm1
	vpand	%ymm11, %ymm14, %ymm11
	vpxor	%ymm14, %ymm8, %ymm7
	vpxor	%ymm10, %ymm11, %ymm8
	vpxor	%ymm10, %ymm14, %ymm10
	vpor	%ymm7, %ymm8, %ymm8
	vpxor	%ymm7, %ymm11, %ymm7
	vmovdqa	%ymm8, 640(%rsp)
	vpand	%ymm7, %ymm10, %ymm10
	vpand	%ymm0, %ymm6, %ymm8
	vpxor	%ymm15, %ymm2, %ymm7
	vpand	%ymm7, %ymm8, %ymm7
	vpxor	%ymm8, %ymm5, %ymm5
	vpand	%ymm0, %ymm12, %ymm0
	vmovdqa	%ymm10, 352(%rsp)
	vpxor	%ymm9, %ymm7, %ymm10
	vpxor	%ymm9, %ymm8, %ymm9
	vpxor	%ymm0, %ymm3, %ymm3
	vpor	%ymm5, %ymm10, %ymm10
	vpxor	%ymm5, %ymm7, %ymm5
	vmovdqa	%ymm10, 608(%rsp)
	vpand	%ymm5, %ymm9, %ymm5
	vmovdqa	%ymm5, 288(%rsp)
	vpxor	%ymm15, %ymm13, %ymm5
	vpand	%ymm5, %ymm0, %ymm5
	vpxor	%ymm1, %ymm5, %ymm7
	vpxor	%ymm1, %ymm0, %ymm1
	vpor	%ymm3, %ymm7, %ymm7
	vpxor	%ymm3, %ymm5, %ymm3
	vmovdqa	%ymm7, 256(%rsp)
	vpand	%ymm3, %ymm1, %ymm1
	vmovdqa	%ymm1, 192(%rsp)
	decl	%esi
	jne	.L8
	vmovdqa	64(%rsp), %ymm14
	vmovdqa	%ymm13, %ymm9
	movl	$256, %esi
	vmovdqa	%ymm12, 448(%rsp)
	vmovdqa	128(%rsp), %ymm7
	vmovdqa	96(%rsp), %ymm12
	vmovdqa	%ymm2, 416(%rsp)
	vmovdqa	224(%rsp), %ymm1
	vmovdqa	%ymm12, 832(%rsp)
	vmovdqa	%ymm7, %ymm2
	vmovdqa	%ymm1, 736(%rsp)
	.p2align 4,,10
	.p2align 3
.L9:
	vpermq	$147, %ymm4, %ymm4
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, 448(%rsp), %ymm8
	vmovdqa	576(%rsp), %ymm1
	vmovq	%xmm4, %rcx
	vmovq	%xmm6, %rdx
	vmovdqa	%ymm4, 864(%rsp)
	vpermq	$147, %ymm9, %ymm9
	leaq	(%rcx,%rcx), %r8
	vmovq	%xmm8, %rdi
	shrq	$63, %rcx
	vmovdqa	%ymm6, 896(%rsp)
	movq	%r8, 864(%rsp)
	addq	%rdi, %rdi
	leaq	(%rdx,%rdx), %r8
	shrq	$63, %rdx
	vmovdqa	320(%rsp), %ymm15
	orq	%rdi, %rdx
	orq	%r8, %rcx
	vmovq	%xmm9, %rdi
	vmovq	%rdx, %xmm0
	movq	%rcx, 896(%rsp)
	addq	%rdi, %rdi
	vpblendd	$3, %ymm0, %ymm8, %ymm7
	vpermq	$147, %ymm2, %ymm0
	vpermq	$147, 416(%rsp), %ymm2
	vmovdqa	%ymm0, 960(%rsp)
	vmovq	%xmm0, %rcx
	vmovq	%xmm2, %rdx
	leaq	(%rcx,%rcx), %r8
	vmovdqa	%ymm2, 992(%rsp)
	shrq	$63, %rcx
	vmovdqa	384(%rsp), %ymm2
	movq	%r8, 960(%rsp)
	leaq	(%rdx,%rdx), %r8
	shrq	$63, %rdx
	vmovdqa	%ymm7, 448(%rsp)
	orq	%r8, %rcx
	movl	%eax, %r8d
	orq	%rdi, %rdx
	movq	%rcx, 992(%rsp)
	vmovd	%xmm2, %ecx
	sarl	$31, %r8d
	vpxor	%ymm1, %ymm2, %ymm5
	andl	$1, %ecx
	vmovq	%rdx, %xmm0
	vmovd	%xmm14, %edi
	vmovdqa	544(%rsp), %ymm7
	negl	%ecx
	vpblendd	$3, %ymm0, %ymm9, %ymm9
	andl	$1, %edi
	vmovdqa	160(%rsp), %ymm0
	andl	%ecx, %r8d
	movl	%ecx, %edx
	vmovd	%xmm1, %ecx
	negl	%edi
	vmovd	%r8d, %xmm10
	andl	$1, %ecx
	vpbroadcastd	%xmm10, %ymm10
	negl	%ecx
	vpand	%ymm10, %ymm5, %ymm5
	andl	%edx, %ecx
	vmovd	%xmm7, %edx
	vpxor	%ymm1, %ymm5, %ymm3
	andl	$1, %edx
	vpxor	%ymm2, %ymm5, %ymm5
	vmovdqa	512(%rsp), %ymm1
	vmovdqa	%ymm3, 576(%rsp)
	negl	%edx
	vpxor	%ymm1, %ymm15, %ymm4
	xorl	%edi, %edx
	movl	%eax, %edi
	vpand	%ymm10, %ymm4, %ymm4
	andl	%ecx, %edx
	negl	%edi
	vpxor	%ymm1, %ymm4, %ymm12
	vmovdqa	%ymm7, %ymm1
	vpxor	%ymm7, %ymm14, %ymm7
	xorl	%eax, %edi
	vpand	%ymm10, %ymm7, %ymm7
	vpxor	%ymm15, %ymm4, %ymm4
	andl	%r8d, %edi
	vmovdqa	%ymm12, 512(%rsp)
	vpxor	%ymm1, %ymm7, %ymm13
	vpxor	%ymm7, %ymm14, %ymm7
	xorl	%edi, %eax
	vmovdqa	480(%rsp), %ymm1
	vmovdqa	%ymm13, 544(%rsp)
	decl	%eax
	vpxor	%ymm0, %ymm1, %ymm6
	vpand	%ymm10, %ymm6, %ymm6
	vpxor	%ymm1, %ymm6, %ymm14
	vpxor	%ymm0, %ymm6, %ymm6
	vmovd	%ecx, %xmm1
	vmovd	%edx, %xmm0
	vpbroadcastd	%xmm1, %ymm1
	vmovdqa	%ymm14, 480(%rsp)
	vpbroadcastd	%xmm0, %ymm0
	vpand	%ymm3, %ymm1, %ymm11
	vpand	%ymm12, %ymm1, %ymm8
	vpxor	%ymm13, %ymm0, %ymm3
	vpxor	%ymm14, %ymm0, %ymm2
	vpxor	%ymm11, %ymm5, %ymm5
	vpand	%ymm3, %ymm11, %ymm3
	vpand	%ymm2, %ymm8, %ymm2
	vpxor	%ymm8, %ymm4, %ymm4
	vpxor	%ymm7, %ymm3, %ymm14
	vpxor	%ymm6, %ymm2, %ymm12
	vpxor	%ymm5, %ymm3, %ymm3
	vpor	%ymm4, %ymm12, %ymm12
	vpor	%ymm5, %ymm14, %ymm14
	vpxor	%ymm4, %ymm2, %ymm2
	vmovdqa	608(%rsp), %ymm5
	vmovq	%xmm14, %rcx
	vmovq	%xmm12, %rdi
	vmovq	%xmm12, %rdx
	salq	$63, %rdi
	shrq	%rcx
	vpxor	%ymm7, %ymm11, %ymm7
	vpxor	%ymm6, %ymm8, %ymm6
	orq	%rdi, %rcx
	vpand	%ymm2, %ymm6, %ymm6
	shrq	%rdx
	vpand	%ymm3, %ymm7, %ymm7
	vmovq	%rcx, %xmm13
	vmovq	%xmm7, %rcx
	vmovq	%xmm6, %rdi
	vpblendd	$3, %ymm13, %ymm14, %ymm14
	vmovq	%rdx, %xmm13
	shrq	%rcx
	vmovq	%xmm6, %rdx
	shrq	%rdx
	vpblendd	$3, %ymm13, %ymm12, %ymm12
	vpermq	$57, %ymm14, %ymm15
	salq	$63, %rdi
	vmovq	%rdx, %xmm8
	orq	%rdi, %rcx
	vmovdqa	%ymm15, 384(%rsp)
	vpermq	$57, %ymm12, %ymm15
	vpblendd	$3, %ymm8, %ymm6, %ymm6
	vmovq	%rcx, %xmm11
	vmovdqa	%ymm15, 320(%rsp)
	vpxor	896(%rsp), %ymm5, %ymm5
	vpermq	$57, %ymm6, %ymm6
	vpblendd	$3, %ymm11, %ymm7, %ymm7
	vmovdqa	%ymm6, 160(%rsp)
	vpand	%ymm10, %ymm5, %ymm5
	vpermq	$57, %ymm7, %ymm14
	vmovdqa	640(%rsp), %ymm6
	vpxor	864(%rsp), %ymm6, %ymm15
	vmovdqa	448(%rsp), %ymm7
	vpxor	896(%rsp), %ymm5, %ymm6
	vpand	%ymm10, %ymm15, %ymm15
	vpxor	864(%rsp), %ymm15, %ymm4
	vmovdqa	256(%rsp), %ymm3
	vmovdqa	352(%rsp), %ymm13
	vpxor	960(%rsp), %ymm13, %ymm12
	vpxor	640(%rsp), %ymm15, %ymm15
	vpxor	%ymm7, %ymm3, %ymm3
	vpxor	608(%rsp), %ymm5, %ymm5
	vpand	%ymm10, %ymm12, %ymm12
	vpand	%ymm10, %ymm3, %ymm3
	vpxor	960(%rsp), %ymm12, %ymm2
	vpxor	%ymm13, %ymm12, %ymm12
	vpxor	%ymm7, %ymm3, %ymm8
	vmovdqa	288(%rsp), %ymm13
	vpxor	992(%rsp), %ymm13, %ymm11
	vmovdqa	%ymm8, 448(%rsp)
	vpand	%ymm1, %ymm4, %ymm8
	vpxor	256(%rsp), %ymm3, %ymm3
	vpand	%ymm10, %ymm11, %ymm11
	vpxor	992(%rsp), %ymm11, %ymm7
	vpxor	%ymm13, %ymm11, %ymm11
	vmovdqa	192(%rsp), %ymm13
	vmovdqa	%ymm7, 416(%rsp)
	vpxor	%ymm13, %ymm9, %ymm7
	vpand	%ymm10, %ymm7, %ymm10
	vpxor	%ymm8, %ymm15, %ymm7
	vpxor	%ymm9, %ymm10, %ymm9
	vpxor	%ymm13, %ymm10, %ymm10
	vpxor	%ymm0, %ymm2, %ymm13
	vpand	%ymm13, %ymm8, %ymm13
	vpxor	%ymm12, %ymm13, %ymm15
	vpxor	%ymm12, %ymm8, %ymm12
	vpor	%ymm7, %ymm15, %ymm15
	vpxor	%ymm7, %ymm13, %ymm7
	vmovdqa	%ymm15, 640(%rsp)
	vpand	%ymm7, %ymm12, %ymm12
	vpxor	416(%rsp), %ymm0, %ymm7
	vpxor	%ymm0, %ymm9, %ymm0
	vmovdqa	%ymm12, 352(%rsp)
	vpand	%ymm1, %ymm6, %ymm12
	vpand	448(%rsp), %ymm1, %ymm1
	vpand	%ymm7, %ymm12, %ymm7
	vpxor	%ymm12, %ymm5, %ymm5
	vpxor	%ymm11, %ymm7, %ymm13
	vpxor	%ymm11, %ymm12, %ymm11
	vpand	%ymm0, %ymm1, %ymm0
	vpor	%ymm5, %ymm13, %ymm15
	vpxor	%ymm5, %ymm7, %ymm5
	vpxor	%ymm1, %ymm3, %ymm3
	vpand	%ymm5, %ymm11, %ymm5
	vpxor	%ymm10, %ymm1, %ymm1
	vmovdqa	%ymm15, 608(%rsp)
	vmovdqa	%ymm5, 288(%rsp)
	vpxor	%ymm10, %ymm0, %ymm5
	vpxor	%ymm3, %ymm0, %ymm10
	vpor	%ymm3, %ymm5, %ymm7
	vpand	%ymm10, %ymm1, %ymm1
	vmovdqa	%ymm7, 256(%rsp)
	vmovdqa	%ymm1, 192(%rsp)
	decl	%esi
	jne	.L9
	vmovdqa	448(%rsp), %ymm1
	vmovdqa	%ymm7, %ymm15
	vmovdqa	%ymm14, 768(%rsp)
	vmovdqa	288(%rsp), %ymm13
	vmovdqa	%ymm14, 480(%rsp)
	vmovdqa	%ymm2, %ymm7
	vmovdqa	352(%rsp), %ymm10
	vmovdqa	192(%rsp), %ymm14
	vmovdqa	%ymm1, 928(%rsp)
	movl	$256, %esi
	vmovdqa	%ymm4, %ymm2
	vmovdqa	384(%rsp), %ymm1
	vmovdqa	%ymm9, 512(%rsp)
	vmovdqa	%ymm1, 672(%rsp)
	vmovdqa	320(%rsp), %ymm1
	vmovdqa	%ymm9, 1024(%rsp)
	vmovdqa	%ymm1, 704(%rsp)
	vmovdqa	160(%rsp), %ymm1
	vmovdqa	%ymm1, 800(%rsp)
	.p2align 4,,10
	.p2align 3
.L10:
	vpermq	$147, %ymm2, %ymm0
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm7, %ymm7
	vmovdqa	480(%rsp), %ymm5
	vpermq	$147, 448(%rsp), %ymm9
	vmovq	%xmm0, %rcx
	vmovq	%xmm6, %rdx
	vmovdqa	%ymm0, 864(%rsp)
	vpermq	$147, 416(%rsp), %ymm2
	leaq	(%rcx,%rcx), %r8
	shrq	$63, %rcx
	vmovdqa	%ymm6, 896(%rsp)
	movq	%r8, 864(%rsp)
	vmovq	%xmm9, %rdi
	leaq	(%rdx,%rdx), %r8
	shrq	$63, %rdx
	vmovdqa	%ymm7, 960(%rsp)
	orq	%r8, %rcx
	addq	%rdi, %rdi
	vpermq	$147, 512(%rsp), %ymm12
	movq	%rcx, 896(%rsp)
	orq	%rdi, %rdx
	vmovq	%xmm7, %rcx
	vmovdqa	384(%rsp), %ymm7
	vmovq	%rdx, %xmm0
	leaq	(%rcx,%rcx), %r8
	vmovq	%xmm2, %rdx
	shrq	$63, %rcx
	movq	%r8, 960(%rsp)
	leaq	(%rdx,%rdx), %r8
	vmovq	%xmm12, %rdi
	vmovdqa	576(%rsp), %ymm6
	orq	%r8, %rcx
	addq	%rdi, %rdi
	movl	%eax, %r8d
	shrq	$63, %rdx
	vmovdqa	%ymm2, 992(%rsp)
	orq	%rdi, %rdx
	sarl	$31, %r8d
	vpblendd	$3, %ymm0, %ymm9, %ymm9
	movq	%rcx, 992(%rsp)
	vmovd	%xmm7, %ecx
	vmovd	%xmm5, %edi
	vpxor	%ymm6, %ymm7, %ymm4
	andl	$1, %ecx
	vmovq	%rdx, %xmm0
	andl	$1, %edi
	vmovdqa	544(%rsp), %ymm2
	negl	%ecx
	negl	%edi
	vpblendd	$3, %ymm0, %ymm12, %ymm12
	movl	%ecx, %edx
	andl	%ecx, %r8d
	vmovd	%xmm6, %ecx
	andl	$1, %ecx
	vmovd	%r8d, %xmm8
	vpxor	%ymm2, %ymm5, %ymm3
	negl	%ecx
	vpbroadcastd	%xmm8, %ymm8
	andl	%edx, %ecx
	vmovd	%xmm2, %edx
	vpand	%ymm8, %ymm4, %ymm4
	vpand	%ymm8, %ymm3, %ymm3
	vmovd	%ecx, %xmm1
	vpxor	%ymm6, %ymm4, %ymm6
	andl	$1, %edx
	vpbroadcastd	%xmm1, %ymm1
	vpxor	%ymm2, %ymm3, %ymm2
	vpxor	%ymm7, %ymm4, %ymm4
	negl	%edx
	vmovdqa	%ymm6, 576(%rsp)
	xorl	%edi, %edx
	vpand	%ymm6, %ymm1, %ymm6
	vpxor	%ymm5, %ymm3, %ymm3
	andl	%ecx, %edx
	vpxor	%ymm6, %ymm4, %ymm4
	movl	%eax, %edi
	vmovdqa	%ymm2, 544(%rsp)
	vmovd	%edx, %xmm0
	negl	%edi
	vpbroadcastd	%xmm0, %ymm0
	xorl	%eax, %edi
	vpxor	%ymm2, %ymm0, %ymm2
	andl	%r8d, %edi
	vpand	%ymm2, %ymm6, %ymm2
	xorl	%edi, %eax
	vpxor	%ymm2, %ymm3, %ymm5
	vpxor	%ymm4, %ymm2, %ymm2
	vpxor	%ymm6, %ymm3, %ymm3
	decl	%eax
	vpor	%ymm4, %ymm5, %ymm5
	vpand	%ymm2, %ymm3, %ymm3
	vmovdqa	608(%rsp), %ymm4
	vpxor	896(%rsp), %ymm4, %ymm4
	vmovq	%xmm5, %rdx
	shrq	%rdx
	vpand	%ymm8, %ymm4, %ymm4
	vmovq	%rdx, %xmm7
	vmovq	%xmm3, %rdx
	shrq	%rdx
	vpblendd	$3, %ymm7, %ymm5, %ymm5
	vmovdqa	640(%rsp), %ymm7
	vpxor	864(%rsp), %ymm7, %ymm11
	vmovq	%rdx, %xmm6
	vpermq	$57, %ymm5, %ymm5
	vpblendd	$3, %ymm6, %ymm3, %ymm3
	vpand	%ymm8, %ymm11, %ymm11
	vmovdqa	%ymm5, 384(%rsp)
	vpxor	864(%rsp), %ymm11, %ymm2
	vpermq	$57, %ymm3, %ymm6
	vpxor	%ymm9, %ymm15, %ymm3
	vmovdqa	%ymm6, 480(%rsp)
	vpand	%ymm8, %ymm3, %ymm3
	vpxor	896(%rsp), %ymm4, %ymm6
	vmovdqa	%ymm2, 864(%rsp)
	vpxor	%ymm9, %ymm3, %ymm5
	vpxor	992(%rsp), %ymm13, %ymm9
	vpxor	%ymm3, %ymm15, %ymm3
	vmovdqa	%ymm5, 448(%rsp)
	vpxor	960(%rsp), %ymm10, %ymm5
	vmovdqa	%ymm6, 896(%rsp)
	vpand	%ymm8, %ymm9, %ymm9
	vpand	%ymm8, %ymm5, %ymm5
	vpxor	960(%rsp), %ymm5, %ymm7
	vpxor	%ymm5, %ymm10, %ymm10
	vmovdqa	%ymm7, 960(%rsp)
	vpxor	992(%rsp), %ymm9, %ymm5
	vpxor	%ymm9, %ymm13, %ymm9
	vpxor	608(%rsp), %ymm4, %ymm4
	vpand	%ymm1, %ymm6, %ymm13
	vmovdqa	%ymm5, 416(%rsp)
	vpxor	%ymm13, %ymm4, %ymm4
	vmovdqa	%ymm5, 992(%rsp)
	vpxor	%ymm12, %ymm14, %ymm5
	vpand	%ymm8, %ymm5, %ymm8
	vpxor	640(%rsp), %ymm11, %ymm5
	vpxor	416(%rsp), %ymm0, %ymm11
	vpxor	%ymm12, %ymm8, %ymm12
	vpxor	%ymm8, %ymm14, %ymm8
	vpand	%ymm1, %ymm2, %ymm14
	vmovdqa	%ymm12, 512(%rsp)
	vpxor	%ymm0, %ymm7, %ymm12
	vpxor	%ymm14, %ymm5, %ymm5
	vpand	448(%rsp), %ymm1, %ymm1
	vpand	%ymm12, %ymm14, %ymm12
	vpand	%ymm11, %ymm13, %ymm11
	vpxor	512(%rsp), %ymm0, %ymm0
	vpxor	%ymm10, %ymm12, %ymm15
	vpxor	%ymm1, %ymm3, %ymm3
	vpxor	%ymm10, %ymm14, %ymm10
	vpor	%ymm5, %ymm15, %ymm15
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	%ymm5, %ymm12, %ymm5
	vmovdqa	%ymm15, 640(%rsp)
	vpxor	%ymm9, %ymm11, %ymm15
	vpxor	%ymm9, %ymm13, %ymm9
	vpxor	%ymm4, %ymm11, %ymm13
	vpor	%ymm4, %ymm15, %ymm15
	vpand	%ymm5, %ymm10, %ymm10
	vpand	%ymm13, %ymm9, %ymm13
	vmovdqa	%ymm15, 608(%rsp)
	vpxor	%ymm8, %ymm0, %ymm15
	vpxor	%ymm8, %ymm1, %ymm8
	vpxor	%ymm3, %ymm0, %ymm0
	vpor	%ymm3, %ymm15, %ymm15
	vpand	%ymm0, %ymm8, %ymm14
	decl	%esi
	jne	.L10
	vmovdqa	576(%rsp), %xmm0
	vmovd	%xmm0, %eax
	vmovdqa	544(%rsp), %xmm0
	andl	$1, %eax
	negl	%eax
	vmovd	%eax, %xmm5
	vmovd	%xmm0, %eax
	andl	$1, %eax
	vpbroadcastd	%xmm5, %ymm5
	negl	%eax
	vpand	%ymm5, %ymm2, %ymm4
	vpand	%ymm5, %ymm6, %ymm6
	vpand	448(%rsp), %ymm5, %ymm5
	vmovd	%eax, %xmm0
	xorl	%eax, %eax
	vpbroadcastd	%xmm0, %ymm0
	vpxor	416(%rsp), %ymm0, %ymm1
	vpxor	%ymm0, %ymm7, %ymm15
	vpxor	512(%rsp), %ymm0, %ymm7
	vpshufd	$216, %ymm4, %ymm0
	vmovdqa	%ymm1, 640(%rsp)
	vpermq	$216, %ymm0, %ymm0
	vmovdqa	.LC10(%rip), %ymm1
	vmovdqa	%ymm7, 608(%rsp)
	vmovdqa	.LC9(%rip), %ymm7
	vpshufb	.LC8(%rip), %ymm0, %ymm0
	vpand	%ymm7, %ymm0, %ymm2
	vpsrld	$4, %ymm0, %ymm0
	vpand	%ymm7, %ymm0, %ymm0
	vperm2i128	$32, %ymm0, %ymm2, %ymm3
	vperm2i128	$49, %ymm0, %ymm2, %ymm2
	vpsrld	$1, %ymm3, %ymm8
	vpsrld	$1, %ymm2, %ymm0
	vpand	%ymm1, %ymm3, %ymm3
	vpand	%ymm1, %ymm8, %ymm8
	vpand	%ymm1, %ymm0, %ymm0
	vpand	%ymm1, %ymm2, %ymm2
	vpunpckldq	%ymm8, %ymm3, %ymm9
	vpunpckhdq	%ymm8, %ymm3, %ymm3
	vpunpckldq	%ymm0, %ymm2, %ymm8
	vpunpckhdq	%ymm0, %ymm2, %ymm2
	vmovdqa	.LC11(%rip), %ymm0
	vpand	%ymm0, %ymm9, %ymm14
	vpand	%ymm0, %ymm3, %ymm12
	vpand	%ymm0, %ymm8, %ymm11
	vpsrld	$2, %ymm9, %ymm9
	vpsrld	$2, %ymm3, %ymm3
	vpand	%ymm0, %ymm2, %ymm10
	vpsrld	$2, %ymm8, %ymm8
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm9, %ymm9
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm8, %ymm8
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm9, %ymm14, %ymm13
	vpunpckhqdq	%ymm9, %ymm14, %ymm9
	vpunpcklqdq	%ymm3, %ymm12, %ymm14
	vmovdqa	%ymm9, 2624(%rsp)
	vpunpckhqdq	%ymm3, %ymm12, %ymm3
	vpunpcklqdq	%ymm8, %ymm11, %ymm12
	vpunpckhqdq	%ymm8, %ymm11, %ymm8
	vmovdqa	%ymm8, 2752(%rsp)
	vpunpcklqdq	%ymm2, %ymm10, %ymm11
	vpunpckhqdq	%ymm2, %ymm10, %ymm2
	vmovdqa	%ymm2, 2816(%rsp)
	vpshufd	$216, %ymm6, %ymm2
	vmovdqa	%ymm3, 2688(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC8(%rip), %ymm2, %ymm2
	vmovdqa	%ymm14, 2656(%rsp)
	vmovdqa	%ymm12, 2720(%rsp)
	vpand	%ymm7, %ymm2, %ymm8
	vpsrld	$4, %ymm2, %ymm2
	vmovdqa	%ymm11, 2784(%rsp)
	vpand	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm13, 2592(%rsp)
	vperm2i128	$32, %ymm2, %ymm8, %ymm3
	vperm2i128	$49, %ymm2, %ymm8, %ymm2
	vpsrld	$1, %ymm2, %ymm10
	vpsrld	$1, %ymm3, %ymm8
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm8, %ymm8
	vpand	%ymm1, %ymm10, %ymm10
	vpand	%ymm1, %ymm3, %ymm3
	vpunpckldq	%ymm8, %ymm3, %ymm9
	vpunpckhdq	%ymm8, %ymm3, %ymm3
	vpunpckldq	%ymm10, %ymm2, %ymm8
	vpunpckhdq	%ymm10, %ymm2, %ymm2
	vpand	%ymm0, %ymm9, %ymm14
	vpand	%ymm0, %ymm3, %ymm12
	vpand	%ymm0, %ymm8, %ymm11
	vpsrld	$2, %ymm9, %ymm9
	vpand	%ymm0, %ymm2, %ymm10
	vpsrld	$2, %ymm3, %ymm3
	vpsrld	$2, %ymm8, %ymm8
	vpand	%ymm0, %ymm9, %ymm9
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm8, %ymm8
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm9, %ymm14, %ymm13
	vpunpckhqdq	%ymm9, %ymm14, %ymm9
	vmovdqa	%ymm9, 2880(%rsp)
	vpunpcklqdq	%ymm3, %ymm12, %ymm14
	vpunpckhqdq	%ymm3, %ymm12, %ymm3
	vpunpcklqdq	%ymm8, %ymm11, %ymm12
	vmovdqa	%ymm3, 2944(%rsp)
	vpunpckhqdq	%ymm8, %ymm11, %ymm8
	vpunpcklqdq	%ymm2, %ymm10, %ymm11
	vpunpckhqdq	%ymm2, %ymm10, %ymm2
	vmovdqa	%ymm2, 3072(%rsp)
	vpshufd	$216, %ymm5, %ymm2
	vmovdqa	%ymm8, 3008(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC8(%rip), %ymm2, %ymm2
	vmovdqa	%ymm14, 2912(%rsp)
	vmovdqa	%ymm12, 2976(%rsp)
	vpand	%ymm7, %ymm2, %ymm8
	vpsrld	$4, %ymm2, %ymm2
	vmovdqa	%ymm11, 3040(%rsp)
	vpand	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm13, 2848(%rsp)
	vperm2i128	$32, %ymm2, %ymm8, %ymm3
	vperm2i128	$49, %ymm2, %ymm8, %ymm2
	vpsrld	$1, %ymm2, %ymm10
	vpsrld	$1, %ymm3, %ymm8
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm8, %ymm8
	vpand	%ymm1, %ymm10, %ymm10
	vpand	%ymm1, %ymm3, %ymm3
	vpunpckldq	%ymm8, %ymm3, %ymm9
	vpunpckhdq	%ymm8, %ymm3, %ymm3
	vpunpckldq	%ymm10, %ymm2, %ymm8
	vpunpckhdq	%ymm10, %ymm2, %ymm2
	vpand	%ymm0, %ymm9, %ymm14
	vpand	%ymm0, %ymm3, %ymm12
	vpand	%ymm0, %ymm8, %ymm11
	vpsrld	$2, %ymm9, %ymm9
	vpand	%ymm0, %ymm2, %ymm10
	vpsrld	$2, %ymm3, %ymm3
	vpsrld	$2, %ymm8, %ymm8
	vpand	%ymm0, %ymm9, %ymm9
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm8, %ymm8
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm9, %ymm14, %ymm13
	vpunpckhqdq	%ymm9, %ymm14, %ymm9
	vmovdqa	%ymm13, 3104(%rsp)
	vpunpcklqdq	%ymm3, %ymm12, %ymm14
	vpunpckhqdq	%ymm3, %ymm12, %ymm3
	vpunpcklqdq	%ymm8, %ymm11, %ymm12
	vmovdqa	%ymm9, 3136(%rsp)
	vpunpckhqdq	%ymm8, %ymm11, %ymm8
	vpunpcklqdq	%ymm2, %ymm10, %ymm11
	vpunpckhqdq	%ymm2, %ymm10, %ymm2
	vmovdqa	%ymm14, 3168(%rsp)
	vmovdqa	%ymm3, 3200(%rsp)
	vmovdqa	%ymm2, 3328(%rsp)
	vpand	%ymm15, %ymm4, %ymm2
	vmovdqa	%ymm8, 3264(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vmovdqa	%ymm11, 3296(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC8(%rip), %ymm2, %ymm2
	vmovdqa	%ymm12, 3232(%rsp)
	vpand	%ymm7, %ymm2, %ymm4
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm7, %ymm2, %ymm2
	vperm2i128	$32, %ymm2, %ymm4, %ymm3
	vperm2i128	$49, %ymm2, %ymm4, %ymm2
	vpsrld	$1, %ymm2, %ymm9
	vpsrld	$1, %ymm3, %ymm4
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm9, %ymm9
	vpand	%ymm1, %ymm3, %ymm3
	vpunpckldq	%ymm4, %ymm3, %ymm8
	vpunpckhdq	%ymm4, %ymm3, %ymm3
	vpunpckldq	%ymm9, %ymm2, %ymm4
	vpunpckhdq	%ymm9, %ymm2, %ymm2
	vpand	%ymm0, %ymm8, %ymm13
	vpand	%ymm0, %ymm3, %ymm11
	vpand	%ymm0, %ymm4, %ymm10
	vpsrld	$2, %ymm8, %ymm8
	vpand	%ymm0, %ymm2, %ymm9
	vpsrld	$2, %ymm3, %ymm3
	vpsrld	$2, %ymm4, %ymm4
	vpand	%ymm0, %ymm8, %ymm8
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm8, %ymm13, %ymm12
	vpunpckhqdq	%ymm8, %ymm13, %ymm8
	vmovdqa	%ymm12, 3360(%rsp)
	vpunpcklqdq	%ymm3, %ymm11, %ymm13
	vpunpckhqdq	%ymm3, %ymm11, %ymm3
	vpunpcklqdq	%ymm4, %ymm10, %ymm11
	vmovdqa	%ymm3, 3456(%rsp)
	vpunpckhqdq	%ymm4, %ymm10, %ymm4
	vpunpcklqdq	%ymm2, %ymm9, %ymm10
	vpunpckhqdq	%ymm2, %ymm9, %ymm2
	vmovdqa	%ymm2, 3584(%rsp)
	vpand	640(%rsp), %ymm6, %ymm2
	vmovdqa	%ymm4, 3520(%rsp)
	vmovdqa	%ymm10, 3552(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vmovdqa	%ymm8, 3392(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC8(%rip), %ymm2, %ymm2
	vmovdqa	%ymm11, 3488(%rsp)
	vmovdqa	%ymm13, 3424(%rsp)
	vpand	%ymm7, %ymm2, %ymm3
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm7, %ymm2, %ymm2
	vperm2i128	$32, %ymm2, %ymm3, %ymm4
	vperm2i128	$49, %ymm2, %ymm3, %ymm2
	vpsrld	$1, %ymm4, %ymm3
	vpsrld	$1, %ymm2, %ymm8
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm3, %ymm9
	vpand	%ymm1, %ymm4, %ymm3
	vpand	%ymm1, %ymm8, %ymm8
	vpunpckldq	%ymm9, %ymm3, %ymm6
	vpunpckhdq	%ymm9, %ymm3, %ymm3
	vpunpckldq	%ymm8, %ymm2, %ymm4
	vpand	%ymm0, %ymm6, %ymm10
	vpand	%ymm0, %ymm3, %ymm9
	vpunpckhdq	%ymm8, %ymm2, %ymm2
	vpsrld	$2, %ymm6, %ymm6
	vpsrld	$2, %ymm3, %ymm3
	vpand	%ymm0, %ymm4, %ymm11
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm2, %ymm8
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm2, %ymm2
	vpunpcklqdq	%ymm6, %ymm10, %ymm12
	vpunpckhqdq	%ymm6, %ymm10, %ymm6
	vpunpcklqdq	%ymm3, %ymm9, %ymm10
	vpunpckhqdq	%ymm3, %ymm9, %ymm3
	vmovdqa	%ymm12, 3616(%rsp)
	vmovdqa	%ymm3, 3712(%rsp)
	vpand	%ymm0, %ymm4, %ymm4
	vpand	608(%rsp), %ymm5, %ymm3
	vpand	%ymm0, %ymm2, %ymm2
	vmovdqa	%ymm6, 3648(%rsp)
	vpunpcklqdq	%ymm4, %ymm11, %ymm9
	vpunpckhqdq	%ymm4, %ymm11, %ymm4
	vpunpcklqdq	%ymm2, %ymm8, %ymm11
	vpshufd	$216, %ymm3, %ymm3
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vmovdqa	%ymm4, 3776(%rsp)
	vmovdqa	%ymm2, 3840(%rsp)
	vpermq	$216, %ymm3, %ymm3
	vpshufb	.LC8(%rip), %ymm3, %ymm3
	vmovdqa	%ymm10, 3680(%rsp)
	vmovdqa	%ymm9, 3744(%rsp)
	vpand	%ymm7, %ymm3, %ymm2
	vpsrld	$4, %ymm3, %ymm3
	vmovdqa	%ymm11, 3808(%rsp)
	vpand	%ymm7, %ymm3, %ymm7
	vperm2i128	$32, %ymm7, %ymm2, %ymm3
	vperm2i128	$49, %ymm7, %ymm2, %ymm2
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
	vmovdqa	%ymm9, 3872(%rsp)
	vpunpckhqdq	%ymm2, %ymm7, %ymm2
	vpunpckhqdq	%ymm3, %ymm8, %ymm3
	vpunpckhqdq	%ymm0, %ymm4, %ymm0
	vmovdqa	%ymm1, 3904(%rsp)
	vmovdqa	%ymm10, 3936(%rsp)
	vmovdqa	%ymm2, 3968(%rsp)
	vmovdqa	%ymm6, 4000(%rsp)
	vmovdqa	%ymm3, 4032(%rsp)
	vmovdqa	%ymm5, 4064(%rsp)
	vmovdqa	%ymm0, 4096(%rsp)
	.p2align 4,,10
	.p2align 3
.L11:
	vmovdqa	3360(%rsp,%rax), %ymm1
	vpaddb	2592(%rsp,%rax), %ymm1, %ymm0
	vmovdqa	%ymm0, 4128(%rsp,%rax)
	addq	$32, %rax
	vmovdqa	%ymm1, 640(%rsp)
	cmpq	$768, %rax
	jne	.L11
	leaq	4896(%rsp), %rdx
	leaq	4864(%rsp), %rax
	leaq	768(%rdx), %rcx
	.p2align 4,,10
	.p2align 3
.L12:
	vmovdqa	(%rax), %ymm1
	addq	$32, %rdx
	subq	$32, %rax
	vperm2i128	$1, %ymm1, %ymm1, %ymm0
	vpshufb	.LC6(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%rcx, %rdx
	jne	.L12
	vpxor	%xmm0, %xmm0, %xmm0
	movl	$768, %edx
	leaq	4964(%rsp), %rsi
	movl	$0, 5728(%rsp)
	leaq	1824(%rsp), %rdi
	vmovaps	%xmm0, 5664(%rsp)
	vmovaps	%xmm0, 5680(%rsp)
	vmovaps	%xmm0, 5696(%rsp)
	vmovaps	%xmm0, 5712(%rsp)
	vzeroupper
	call	memcpy
	xorl	%eax, %eax
	vpxor	%xmm2, %xmm2, %xmm2
	.p2align 4,,10
	.p2align 3
.L13:
	vmovdqa	1824(%rsp,%rax), %ymm1
	vmovdqa	%ymm1, 640(%rsp)
	vpunpcklbw	%ymm2, %ymm1, %ymm1
	vmovdqa	640(%rsp), %ymm6
	vpunpckhbw	%ymm2, %ymm6, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm3
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vmovdqu	%ymm3, (%rbx,%rax,2)
	vmovdqu	%ymm0, 32(%rbx,%rax,2)
	addq	$32, %rax
	cmpq	$704, %rax
	jne	.L13
	vzeroupper
	xorl	%eax, %eax
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5187:
	.size	crypto_core_invhrss701_faster821, .-crypto_core_invhrss701_faster821
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
	.long	-3
	.long	-3
	.long	-3
	.long	-3
	.long	-3
	.long	-3
	.long	-3
	.long	-3
	.align 32
.LC5:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC6:
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
.LC7:
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
.LC8:
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
.LC9:
	.quad	1085102592571150095
	.quad	1085102592571150095
	.quad	1085102592571150095
	.quad	1085102592571150095
	.align 32
.LC10:
	.quad	361700864190383365
	.quad	361700864190383365
	.quad	361700864190383365
	.quad	361700864190383365
	.align 32
.LC11:
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.ident	"GCC: (GNU) 8.2.1 20181105 (Red Hat 8.2.1-5)"
	.section	.note.GNU-stack,"",@progbits
