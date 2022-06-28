	.file	"r3_recip.c"
	.text
	.p2align 4,,15
	.globl	r3_recip
	.type	r3_recip, @function
r3_recip:
.LFB5186:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	%esi, %r11d
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$4160, %rsp
	.cfi_offset 3, -24
	andl	$31, %r11d
	je	.L16
	leal	-1(%r11), %ecx
	addq	$2, %rcx
	movl	$1, %edx
	movl	$768, %r9d
	.p2align 4,,10
	.p2align 3
.L3:
	movq	%rdx, %rax
	negq	%rax
	movzbl	768(%rsi,%rax), %eax
	movl	%r9d, %r8d
	movb	%al, 3359(%rsp,%rdx)
	subl	%edx, %r8d
	movl	%edx, %eax
	incq	%rdx
	cmpq	%rdx, %rcx
	jne	.L3
.L2:
	movl	$768, %r10d
	movl	%r11d, %ecx
	leaq	3360(%rsp), %rbx
	subl	%r11d, %r10d
	leaq	(%rbx,%rcx), %rdx
	movl	%r10d, %r9d
	movq	%rsi, %rbx
	subq	%rcx, %rbx
	shrl	$5, %r9d
	movq	%rbx, %rcx
	salq	$5, %r9
	addq	$736, %rcx
	addq	%rdx, %r9
.L4:
	vmovdqa	(%rcx), %ymm0
	addq	$32, %rdx
	vperm2i128	$1, %ymm0, %ymm0, %ymm0
	vpshufb	.LC2(%rip), %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rdx)
	subq	$32, %rcx
	cmpq	%rdx, %r9
	jne	.L4
	movl	%r10d, %ecx
	andl	$-32, %ecx
	movl	%ecx, %r9d
	leal	(%rcx,%rax), %edx
	notl	%r9d
	cmpl	%r10d, %ecx
	je	.L5
	movl	$767, %ecx
	subl	%edx, %ecx
	movslq	%ecx, %rcx
	leaq	(%rsi,%rcx), %rax
	movslq	%edx, %rdx
	leaq	-1(%rsi,%rcx), %rsi
	leaq	3360(%rsp), %rbx
	leal	(%r8,%r9), %ecx
	addq	%rbx, %rdx
	subq	%rcx, %rsi
	.p2align 4,,10
	.p2align 3
.L6:
	movzbl	(%rax), %ecx
	decq	%rax
	movb	%cl, (%rdx)
	incq	%rdx
	cmpq	%rsi, %rax
	jne	.L6
.L5:
	xorl	%edx, %edx
	movl	$0, 4128(%rsp)
	movw	%dx, 4132(%rsp)
	movb	$0, 4134(%rsp)
	movl	$1, %eax
	leaq	3366(%rsp), %rsi
	.p2align 4,,10
	.p2align 3
.L7:
	movzbl	(%rsi,%rax), %edx
	movl	%edx, %ecx
	sarb	%dl
	andl	$1, %edx
	andl	$1, %ecx
	movb	%dl, 2591(%rsp,%rax)
	movb	%cl, 1823(%rsp,%rax)
	movl	%eax, %edx
	incq	%rax
	cmpq	$26, %rax
	jne	.L7
	vmovdqa	.LC3(%rip), %ymm2
	movl	$25, %eax
	leaq	3367(%rsp), %rcx
.L8:
	vmovdqa	(%rcx,%rax), %ymm1
	vpand	%ymm2, %ymm1, %ymm0
	vmovdqu	%ymm0, 1824(%rsp,%rax)
	vpmovsxbw	%xmm1, %ymm0
	vextracti128	$0x1, %ymm1, %xmm1
	vpmovsxbw	%xmm1, %ymm1
	vpsraw	$1, %ymm0, %ymm0
	vpsraw	$1, %ymm1, %ymm1
	vpand	.LC4(%rip), %ymm0, %ymm0
	vpand	.LC4(%rip), %ymm1, %ymm1
	vpackuswb	%ymm1, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm2, %ymm0
	vmovdqu	%ymm0, 2592(%rsp,%rax)
	addq	$32, %rax
	cmpq	$761, %rax
	jne	.L8
	leal	743(%rdx), %eax
	cltq
	movzbl	3360(%rsp,%rax), %eax
	leal	736(%rdx), %ecx
	movl	%eax, %esi
	sarb	%al
	movslq	%ecx, %rcx
	andl	$1, %eax
	movb	%al, 2592(%rsp,%rcx)
	leal	744(%rdx), %eax
	cltq
	movzbl	3360(%rsp,%rax), %eax
	andl	$1, %esi
	movb	%sil, 1824(%rsp,%rcx)
	leal	737(%rdx), %ecx
	movl	%eax, %esi
	sarb	%al
	movslq	%ecx, %rcx
	andl	$1, %eax
	movb	%al, 2592(%rsp,%rcx)
	leal	745(%rdx), %eax
	cltq
	movzbl	3360(%rsp,%rax), %eax
	andl	$1, %esi
	movb	%sil, 1824(%rsp,%rcx)
	leal	738(%rdx), %ecx
	movl	%eax, %esi
	sarb	%al
	movslq	%ecx, %rcx
	andl	$1, %eax
	movb	%al, 2592(%rsp,%rcx)
	leal	746(%rdx), %eax
	cltq
	movzbl	3360(%rsp,%rax), %eax
	andl	$1, %esi
	movb	%sil, 1824(%rsp,%rcx)
	leal	739(%rdx), %ecx
	movl	%eax, %esi
	sarb	%al
	movslq	%ecx, %rcx
	andl	$1, %eax
	movb	%al, 2592(%rsp,%rcx)
	leal	747(%rdx), %eax
	cltq
	movzbl	3360(%rsp,%rax), %eax
	andl	$1, %esi
	movb	%sil, 1824(%rsp,%rcx)
	leal	740(%rdx), %ecx
	movl	%eax, %esi
	sarb	%al
	movslq	%ecx, %rcx
	andl	$1, %eax
	movb	%al, 2592(%rsp,%rcx)
	leal	748(%rdx), %eax
	cltq
	movzbl	3360(%rsp,%rax), %eax
	andl	$1, %esi
	movb	%sil, 1824(%rsp,%rcx)
	leal	741(%rdx), %ecx
	movl	%eax, %esi
	sarb	%al
	movslq	%ecx, %rcx
	andl	$1, %eax
	movb	%al, 2592(%rsp,%rcx)
	leal	749(%rdx), %eax
	cltq
	movzbl	3360(%rsp,%rax), %eax
	andl	$1, %esi
	movb	%sil, 1824(%rsp,%rcx)
	leal	742(%rdx), %ecx
	movslq	%ecx, %rdx
	movl	%eax, %ecx
	andl	$1, %ecx
	movb	%cl, 1824(%rsp,%rdx)
	sarb	%al
	andl	$1, %eax
	vmovdqa	1824(%rsp), %ymm0
	vmovdqa	1856(%rsp), %ymm1
	vmovdqa	1920(%rsp), %ymm3
	vpunpckldq	%ymm1, %ymm0, %ymm8
	vpunpckhdq	%ymm1, %ymm0, %ymm1
	vmovdqa	1888(%rsp), %ymm0
	vmovdqa	1984(%rsp), %ymm2
	vpunpckldq	%ymm3, %ymm0, %ymm7
	vpunpckhdq	%ymm3, %ymm0, %ymm3
	vmovdqa	1952(%rsp), %ymm0
	vmovdqa	2048(%rsp), %ymm4
	vpunpckldq	%ymm2, %ymm0, %ymm6
	vpunpckhdq	%ymm2, %ymm0, %ymm0
	vmovdqa	2016(%rsp), %ymm2
	vpslld	$2, %ymm1, %ymm1
	vpunpckldq	%ymm4, %ymm2, %ymm5
	vpunpckhdq	%ymm4, %ymm2, %ymm2
	vpslld	$2, %ymm3, %ymm3
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm2, %ymm2
	vpor	%ymm7, %ymm3, %ymm3
	vpor	%ymm5, %ymm2, %ymm2
	vpor	%ymm8, %ymm1, %ymm1
	vpor	%ymm6, %ymm0, %ymm0
	vpunpcklqdq	%ymm3, %ymm1, %ymm4
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpunpcklqdq	%ymm2, %ymm0, %ymm3
	vpunpckhqdq	%ymm2, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm0, %ymm3, %ymm0
	vpor	%ymm1, %ymm4, %ymm1
	vperm2i128	$32, %ymm0, %ymm1, %ymm11
	vperm2i128	$49, %ymm0, %ymm1, %ymm1
	vpslld	$4, %ymm1, %ymm0
	vpor	%ymm0, %ymm11, %ymm11
	vpshufb	.LC5(%rip), %ymm11, %ymm11
	vpermq	$216, %ymm11, %ymm11
	vpshufd	$216, %ymm11, %ymm7
	vmovdqa	2112(%rsp), %ymm0
	vmovdqa	%ymm7, 384(%rsp)
	movb	%al, 2592(%rsp,%rdx)
	vmovdqa	2080(%rsp), %ymm1
	vmovdqa	2144(%rsp), %ymm4
	vpunpckldq	%ymm0, %ymm1, %ymm8
	vpunpckhdq	%ymm0, %ymm1, %ymm1
	vmovdqa	2176(%rsp), %ymm0
	vmovdqa	2208(%rsp), %ymm2
	vpunpckldq	%ymm0, %ymm4, %ymm7
	vpunpckhdq	%ymm0, %ymm4, %ymm4
	vmovdqa	2240(%rsp), %ymm0
	vmovdqa	2272(%rsp), %ymm3
	vpunpckldq	%ymm0, %ymm2, %ymm6
	vpunpckhdq	%ymm0, %ymm2, %ymm0
	vmovdqa	2304(%rsp), %ymm2
	vpslld	$2, %ymm1, %ymm1
	vpunpckldq	%ymm2, %ymm3, %ymm5
	vpunpckhdq	%ymm2, %ymm3, %ymm3
	vpslld	$2, %ymm4, %ymm4
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm7, %ymm4, %ymm4
	vpor	%ymm5, %ymm3, %ymm3
	vpor	%ymm8, %ymm1, %ymm1
	vpor	%ymm6, %ymm0, %ymm0
	vpunpcklqdq	%ymm4, %ymm1, %ymm2
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vpunpcklqdq	%ymm3, %ymm0, %ymm4
	vpunpckhqdq	%ymm3, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm2, %ymm1
	vpor	%ymm0, %ymm4, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm3
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vmovdqa	2336(%rsp), %ymm1
	vpor	%ymm0, %ymm3, %ymm3
	vmovdqa	2368(%rsp), %ymm0
	vmovdqa	2400(%rsp), %ymm5
	vpunpckldq	%ymm0, %ymm1, %ymm9
	vpshufb	.LC5(%rip), %ymm3, %ymm3
	vpunpckhdq	%ymm0, %ymm1, %ymm1
	vmovdqa	2432(%rsp), %ymm0
	vpermq	$216, %ymm3, %ymm3
	vpshufd	$216, %ymm3, %ymm7
	vpunpckldq	%ymm0, %ymm5, %ymm2
	vmovdqa	2464(%rsp), %ymm3
	vpunpckhdq	%ymm0, %ymm5, %ymm5
	vmovdqa	2496(%rsp), %ymm0
	vmovdqa	2528(%rsp), %ymm8
	vmovdqa	%ymm7, %ymm10
	vpunpckldq	%ymm0, %ymm3, %ymm7
	vpunpckhdq	%ymm0, %ymm3, %ymm3
	vmovdqa	2560(%rsp), %ymm0
	vpslld	$2, %ymm1, %ymm6
	vpunpckldq	%ymm0, %ymm8, %ymm4
	vpunpckhdq	%ymm0, %ymm8, %ymm0
	vpslld	$2, %ymm5, %ymm5
	vpslld	$2, %ymm3, %ymm3
	vpslld	$2, %ymm0, %ymm0
	vpor	%ymm6, %ymm9, %ymm6
	vpor	%ymm0, %ymm4, %ymm4
	vpor	%ymm5, %ymm2, %ymm2
	vpor	%ymm3, %ymm7, %ymm1
	vpunpcklqdq	%ymm4, %ymm1, %ymm3
	vpunpcklqdq	%ymm2, %ymm6, %ymm0
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vpunpckhqdq	%ymm2, %ymm6, %ymm2
	vpslld	$1, %ymm2, %ymm2
	vpslld	$1, %ymm1, %ymm1
	vpor	%ymm1, %ymm3, %ymm1
	vpor	%ymm2, %ymm0, %ymm0
	vperm2i128	$32, %ymm1, %ymm0, %ymm13
	vperm2i128	$49, %ymm1, %ymm0, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm13, %ymm13
	vpshufb	.LC5(%rip), %ymm13, %ymm13
	vmovdqa	2624(%rsp), %ymm0
	vmovdqa	2592(%rsp), %ymm1
	vpermq	$216, %ymm13, %ymm13
	vpshufd	$216, %ymm13, %ymm7
	vmovdqa	2656(%rsp), %ymm2
	vmovdqa	%ymm7, %ymm15
	vpunpckldq	%ymm0, %ymm1, %ymm7
	vpunpckhdq	%ymm0, %ymm1, %ymm1
	vmovdqa	2688(%rsp), %ymm0
	vmovdqa	2720(%rsp), %ymm3
	vpunpckldq	%ymm0, %ymm2, %ymm4
	vpunpckhdq	%ymm0, %ymm2, %ymm2
	vmovdqa	2752(%rsp), %ymm0
	vmovdqa	2784(%rsp), %ymm8
	vpunpckldq	%ymm0, %ymm3, %ymm6
	vpunpckhdq	%ymm0, %ymm3, %ymm0
	vmovdqa	2816(%rsp), %ymm3
	vpslld	$2, %ymm2, %ymm2
	vpunpckldq	%ymm3, %ymm8, %ymm5
	vpunpckhdq	%ymm3, %ymm8, %ymm3
	vpslld	$2, %ymm1, %ymm1
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm5, %ymm3, %ymm3
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm4, %ymm2, %ymm4
	vpor	%ymm6, %ymm0, %ymm0
	vpunpcklqdq	%ymm4, %ymm1, %ymm2
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vpunpcklqdq	%ymm3, %ymm0, %ymm4
	vpunpckhqdq	%ymm3, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm2, %ymm1
	vpor	%ymm0, %ymm4, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm9
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vmovdqa	2848(%rsp), %ymm2
	vpor	%ymm0, %ymm9, %ymm9
	vmovdqa	2880(%rsp), %ymm0
	vmovdqa	2912(%rsp), %ymm1
	vpunpckldq	%ymm0, %ymm2, %ymm7
	vpunpckhdq	%ymm0, %ymm2, %ymm2
	vmovdqa	2944(%rsp), %ymm0
	vmovdqa	2976(%rsp), %ymm3
	vpunpckldq	%ymm0, %ymm1, %ymm4
	vpunpckhdq	%ymm0, %ymm1, %ymm1
	vmovdqa	3008(%rsp), %ymm0
	vpshufb	.LC5(%rip), %ymm9, %ymm9
	vpunpckldq	%ymm0, %ymm3, %ymm6
	vpunpckhdq	%ymm0, %ymm3, %ymm0
	vmovdqa	3072(%rsp), %ymm3
	vmovdqa	3040(%rsp), %ymm8
	vpslld	$2, %ymm1, %ymm1
	vpunpckldq	%ymm3, %ymm8, %ymm5
	vpunpckhdq	%ymm3, %ymm8, %ymm3
	vpslld	$2, %ymm2, %ymm2
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm5, %ymm3, %ymm3
	vpor	%ymm7, %ymm2, %ymm2
	vpor	%ymm4, %ymm1, %ymm4
	vpor	%ymm6, %ymm0, %ymm0
	vpunpcklqdq	%ymm4, %ymm2, %ymm1
	vpunpckhqdq	%ymm4, %ymm2, %ymm2
	vpunpcklqdq	%ymm3, %ymm0, %ymm4
	vpunpckhqdq	%ymm3, %ymm0, %ymm0
	vpslld	$1, %ymm2, %ymm2
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm2, %ymm1, %ymm1
	vpor	%ymm0, %ymm4, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm8
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm8, %ymm8
	vmovdqa	3104(%rsp), %ymm1
	vmovdqa	3136(%rsp), %ymm0
	vmovdqa	3168(%rsp), %ymm2
	vpunpckldq	%ymm0, %ymm1, %ymm7
	vpunpckhdq	%ymm0, %ymm1, %ymm1
	vmovdqa	3200(%rsp), %ymm0
	vmovdqa	3232(%rsp), %ymm3
	vpunpckldq	%ymm0, %ymm2, %ymm6
	vpshufb	.LC5(%rip), %ymm8, %ymm8
	vpunpckhdq	%ymm0, %ymm2, %ymm2
	vmovdqa	3264(%rsp), %ymm0
	vpermq	$216, %ymm8, %ymm8
	vpunpckldq	%ymm0, %ymm3, %ymm5
	vpshufd	$216, %ymm8, %ymm12
	vpunpckhdq	%ymm0, %ymm3, %ymm0
	vmovdqa	3328(%rsp), %ymm8
	vmovdqa	3296(%rsp), %ymm3
	vpslld	$2, %ymm1, %ymm1
	vpunpckldq	%ymm8, %ymm3, %ymm4
	vpunpckhdq	%ymm8, %ymm3, %ymm3
	vpor	%ymm1, %ymm7, %ymm7
	vpslld	$2, %ymm2, %ymm2
	vpslld	$2, %ymm0, %ymm1
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm3, %ymm4, %ymm4
	vpor	%ymm2, %ymm6, %ymm6
	vpor	%ymm1, %ymm5, %ymm1
	vpunpcklqdq	%ymm4, %ymm1, %ymm3
	vpunpckhqdq	%ymm6, %ymm7, %ymm0
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vpunpcklqdq	%ymm6, %ymm7, %ymm2
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm0, %ymm2, %ymm0
	vpor	%ymm1, %ymm3, %ymm2
	vperm2i128	$32, %ymm2, %ymm0, %ymm1
	vperm2i128	$49, %ymm2, %ymm0, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm1, %ymm0
	vpshufb	.LC5(%rip), %ymm0, %ymm0
	vpermq	$216, %ymm9, %ymm9
	vpermq	$216, %ymm0, %ymm0
	vpshufd	$216, %ymm9, %ymm14
	vpshufd	$216, %ymm0, %ymm11
	vmovdqa	.LC0(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm1
	vpxor	%xmm4, %xmm4, %xmm4
	vmovdqa	%ymm11, %ymm9
	vmovdqa	%ymm14, %ymm11
	vmovdqa	384(%rsp), %ymm14
	vmovdqa	%ymm10, %ymm13
	movl	$256, %ecx
	vmovdqa	%ymm4, %ymm3
	vmovdqa	%ymm4, 320(%rsp)
	vmovdqa	%ymm7, 640(%rsp)
	vmovdqa	%ymm1, 480(%rsp)
	vmovdqa	%ymm4, 512(%rsp)
	vmovdqa	%ymm4, 608(%rsp)
	vmovdqa	%ymm1, 416(%rsp)
	vmovdqa	%ymm4, 448(%rsp)
	vmovdqa	%ymm7, 576(%rsp)
	movl	$-1, %ebx
	vmovdqa	%ymm15, %ymm8
	vmovdqa	%ymm12, %ymm10
	.p2align 4,,10
	.p2align 3
.L9:
	vpermq	$147, %ymm3, %ymm3
	vmovq	%xmm3, %rax
	addq	%rax, %rax
	vpermq	$147, %ymm4, %ymm4
	vmovq	%rax, %xmm0
	vmovd	%xmm14, %edx
	vmovq	%xmm4, %rax
	addq	%rax, %rax
	andl	$1, %edx
	vpblendd	$3, %ymm0, %ymm3, %ymm7
	movl	%ebx, %r8d
	vmovq	%rax, %xmm0
	movl	%edx, %eax
	negl	%eax
	sarl	$31, %r8d
	vmovdqa	576(%rsp), %ymm6
	andl	%eax, %r8d
	vpblendd	$3, %ymm0, %ymm4, %ymm1
	vmovdqa	%ymm1, 256(%rsp)
	vmovd	%r8d, %xmm1
	vpbroadcastd	%xmm1, %ymm1
	vmovdqa	448(%rsp), %ymm5
	vmovdqa	%ymm7, 288(%rsp)
	vpxor	%ymm6, %ymm14, %ymm7
	vpand	%ymm1, %ymm7, %ymm7
	vpxor	%ymm6, %ymm7, %ymm12
	vmovd	%xmm6, %edx
	vpxor	%ymm5, %ymm13, %ymm6
	vmovdqa	608(%rsp), %ymm3
	vmovdqa	416(%rsp), %ymm0
	vpand	%ymm1, %ymm6, %ymm6
	andl	$1, %edx
	vpxor	%ymm5, %ymm6, %ymm5
	negl	%edx
	andl	%eax, %edx
	vmovdqa	%ymm5, 448(%rsp)
	vpxor	%ymm3, %ymm11, %ymm4
	vpxor	%ymm0, %ymm8, %ymm5
	vmovd	%xmm3, %esi
	vmovd	%xmm11, %eax
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm5, %ymm5
	andl	$1, %eax
	andl	$1, %esi
	vpxor	%ymm0, %ymm5, %ymm2
	vpxor	%ymm4, %ymm11, %ymm15
	vpxor	%ymm3, %ymm4, %ymm0
	negl	%esi
	vmovdqa	480(%rsp), %ymm4
	negl	%eax
	vmovdqa	512(%rsp), %ymm11
	xorl	%esi, %eax
	andl	%edx, %eax
	vmovdqa	%ymm2, 416(%rsp)
	vpxor	%ymm4, %ymm9, %ymm2
	vmovdqa	%ymm15, 544(%rsp)
	vpand	%ymm1, %ymm2, %ymm2
	vpxor	%ymm11, %ymm10, %ymm3
	vmovd	%eax, %xmm15
	vpand	%ymm1, %ymm3, %ymm3
	vpbroadcastd	%xmm15, %ymm15
	vmovdqa	%ymm0, 608(%rsp)
	vpxor	%ymm4, %ymm2, %ymm4
	vmovd	%edx, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpxor	%ymm11, %ymm3, %ymm11
	vmovdqa	%ymm4, 480(%rsp)
	vpxor	608(%rsp), %ymm15, %ymm4
	vmovdqa	%ymm11, 512(%rsp)
	vpxor	%ymm3, %ymm10, %ymm10
	vpand	%ymm12, %ymm0, %ymm11
	vmovdqa	%ymm10, 384(%rsp)
	vpand	%ymm4, %ymm11, %ymm4
	vpand	448(%rsp), %ymm0, %ymm10
	vpxor	%ymm7, %ymm14, %ymm14
	vpxor	512(%rsp), %ymm15, %ymm3
	vpxor	%ymm11, %ymm14, %ymm7
	movl	%ebx, %esi
	vpxor	544(%rsp), %ymm4, %ymm14
	vpxor	%ymm2, %ymm9, %ymm9
	vpand	%ymm3, %ymm10, %ymm3
	negl	%esi
	vpxor	%ymm6, %ymm13, %ymm13
	vpxor	%ymm10, %ymm13, %ymm6
	vmovdqa	%ymm9, 352(%rsp)
	xorl	%ebx, %esi
	vpand	416(%rsp), %ymm0, %ymm9
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	384(%rsp), %ymm3, %ymm13
	vpxor	480(%rsp), %ymm15, %ymm2
	andl	%r8d, %esi
	vmovaps	%xmm14, 224(%rsp)
	vpor	%ymm6, %ymm13, %ymm13
	xorl	%esi, %ebx
	vpand	%ymm2, %ymm9, %ymm2
	movq	224(%rsp), %rsi
	vpxor	%ymm5, %ymm8, %ymm5
	vmovq	%xmm13, %rax
	vpxor	352(%rsp), %ymm2, %ymm8
	vpxor	%ymm9, %ymm5, %ymm5
	shrq	%rsi
	salq	$63, %rax
	vpor	%ymm5, %ymm8, %ymm8
	orq	%rsi, %rax
	vmovdqa	%ymm12, 576(%rsp)
	vmovq	%xmm13, %r8
	vmovq	%rax, %xmm12
	vpxor	544(%rsp), %ymm11, %ymm11
	vmovq	%xmm8, %rax
	vpxor	384(%rsp), %ymm10, %ymm10
	shrq	%r8
	vpxor	%ymm7, %ymm4, %ymm4
	vpxor	%ymm6, %ymm3, %ymm3
	salq	$63, %rax
	orq	%r8, %rax
	vpand	%ymm3, %ymm10, %ymm10
	vpand	%ymm4, %ymm11, %ymm11
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	vpxor	352(%rsp), %ymm9, %ymm9
	vmovq	%rax, %xmm12
	vmovq	%xmm11, %rsi
	vmovq	%xmm10, %rax
	vpxor	%ymm5, %ymm2, %ymm2
	shrq	%rsi
	salq	$63, %rax
	vpand	%ymm2, %ymm9, %ymm9
	orq	%rsi, %rax
	vmovq	%xmm8, %rdx
	shrq	%rdx
	vmovq	%rax, %xmm4
	vmovq	%xmm10, %r8
	vmovq	%xmm9, %rax
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	shrq	%r8
	vmovq	%rdx, %xmm12
	salq	$63, %rax
	vmovq	%xmm9, %rdx
	vmovdqa	640(%rsp), %ymm6
	vmovdqa	288(%rsp), %ymm7
	decl	%ebx
	orq	%r8, %rax
	shrq	%rdx
	vmovq	%rdx, %xmm2
	vpblendd	$3, %ymm2, %ymm9, %ymm9
	vpxor	%ymm6, %ymm7, %ymm2
	vpand	%ymm1, %ymm2, %ymm2
	vmovq	%rax, %xmm3
	vpblendd	$3, %ymm3, %ymm10, %ymm10
	vpxor	%ymm7, %ymm2, %ymm3
	vmovdqa	256(%rsp), %ymm7
	vmovdqa	320(%rsp), %ymm5
	vpblendd	$3, %ymm4, %ymm11, %ymm11
	vpxor	%ymm5, %ymm7, %ymm4
	vpand	%ymm1, %ymm4, %ymm1
	vpxor	%ymm7, %ymm1, %ymm4
	vpand	%ymm0, %ymm3, %ymm0
	vpxor	%ymm5, %ymm1, %ymm1
	vpxor	%ymm15, %ymm4, %ymm5
	vpand	%ymm5, %ymm0, %ymm5
	vpxor	%ymm6, %ymm2, %ymm2
	vpxor	%ymm0, %ymm2, %ymm2
	vpxor	%ymm5, %ymm1, %ymm6
	vpor	%ymm2, %ymm6, %ymm7
	vpxor	%ymm0, %ymm1, %ymm1
	vpxor	%ymm2, %ymm5, %ymm2
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vpand	%ymm2, %ymm1, %ymm1
	vpermq	$57, %ymm14, %ymm14
	vpermq	$57, %ymm13, %ymm13
	vpermq	$57, %ymm8, %ymm8
	vpermq	$57, %ymm11, %ymm11
	vpermq	$57, %ymm10, %ymm10
	vpermq	$57, %ymm9, %ymm9
	vmovdqa	%ymm7, 640(%rsp)
	vmovdqa	%ymm1, 320(%rsp)
	decl	%ecx
	jne	.L9
	vmovdqa	%ymm10, %ymm12
	vpxor	%xmm10, %xmm10, %xmm10
	vmovdqa	%ymm14, 384(%rsp)
	vmovdqa	%ymm8, 224(%rsp)
	vmovdqa	%ymm11, %ymm14
	vmovdqa	%ymm13, 352(%rsp)
	vmovdqa	%ymm9, %ymm11
	movl	$256, %ecx
	vmovdqa	%ymm10, %ymm5
	vmovdqa	%ymm10, 288(%rsp)
	vmovdqa	%ymm10, 544(%rsp)
	.p2align 4,,10
	.p2align 3
.L10:
	vpermq	$147, %ymm3, %ymm3
	vpermq	$147, %ymm5, %ymm5
	vmovq	%xmm3, %rax
	vmovq	%xmm5, %rdx
	addq	%rdx, %rdx
	leaq	(%rax,%rax), %rsi
	shrq	$63, %rax
	orq	%rdx, %rax
	vmovq	%rsi, %xmm0
	vpermq	$147, %ymm4, %ymm4
	vpermq	$147, %ymm10, %ymm10
	vmovdqa	384(%rsp), %ymm2
	vpblendd	$3, %ymm0, %ymm3, %ymm7
	vmovq	%xmm10, %rdx
	vmovq	%rax, %xmm0
	vmovq	%xmm4, %rax
	addq	%rdx, %rdx
	leaq	(%rax,%rax), %rsi
	shrq	$63, %rax
	orq	%rdx, %rax
	vmovd	%xmm2, %edx
	vpblendd	$3, %ymm0, %ymm5, %ymm1
	andl	$1, %edx
	vmovq	%rsi, %xmm0
	vpblendd	$3, %ymm0, %ymm4, %ymm6
	movl	%ebx, %r8d
	vmovq	%rax, %xmm0
	movl	%edx, %eax
	negl	%eax
	sarl	$31, %r8d
	andl	%eax, %r8d
	vmovdqa	352(%rsp), %ymm8
	vpblendd	$3, %ymm0, %ymm10, %ymm13
	vmovdqa	448(%rsp), %ymm0
	vmovdqa	%ymm1, 96(%rsp)
	vmovd	%r8d, %xmm1
	vpbroadcastd	%xmm1, %ymm1
	vmovdqa	%ymm6, 64(%rsp)
	vpxor	%ymm0, %ymm8, %ymm6
	vpand	%ymm1, %ymm6, %ymm6
	vpxor	%ymm0, %ymm6, %ymm0
	vmovdqa	608(%rsp), %ymm4
	vmovdqa	416(%rsp), %ymm15
	vmovdqa	%ymm13, 32(%rsp)
	vmovdqa	%ymm0, 448(%rsp)
	vmovdqa	%ymm0, %ymm13
	vmovdqa	224(%rsp), %ymm0
	vmovdqa	576(%rsp), %ymm3
	vpxor	%ymm15, %ymm0, %ymm5
	vpxor	%ymm4, %ymm14, %ymm0
	vpand	%ymm1, %ymm0, %ymm0
	vmovdqa	512(%rsp), %ymm9
	vmovd	%xmm3, %edx
	vpxor	%ymm0, %ymm14, %ymm10
	andl	$1, %edx
	vmovdqa	%ymm10, 256(%rsp)
	negl	%edx
	vpxor	%ymm9, %ymm12, %ymm10
	andl	%eax, %edx
	vpand	%ymm1, %ymm10, %ymm10
	vmovd	%xmm4, %esi
	vmovd	%xmm14, %eax
	vpxor	%ymm4, %ymm0, %ymm4
	vmovdqa	480(%rsp), %ymm0
	vpxor	%ymm9, %ymm10, %ymm9
	andl	$1, %eax
	andl	$1, %esi
	negl	%esi
	vmovdqa	%ymm9, %ymm14
	negl	%eax
	vmovdqa	%ymm9, 512(%rsp)
	vpxor	%ymm0, %ymm11, %ymm9
	vpand	%ymm1, %ymm9, %ymm9
	xorl	%esi, %eax
	vpand	%ymm1, %ymm5, %ymm5
	andl	%edx, %eax
	vpxor	%ymm15, %ymm5, %ymm15
	vpxor	%ymm0, %ymm9, %ymm0
	vmovdqa	%ymm7, 128(%rsp)
	vpxor	%ymm2, %ymm3, %ymm7
	vpand	%ymm1, %ymm7, %ymm7
	vmovdqa	%ymm15, 416(%rsp)
	vmovdqa	%ymm0, 480(%rsp)
	vmovd	%eax, %xmm15
	vmovd	%edx, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpbroadcastd	%xmm15, %ymm15
	vpxor	%ymm3, %ymm7, %ymm3
	vpxor	%ymm9, %ymm11, %ymm11
	vmovdqa	%ymm11, 192(%rsp)
	vmovdqa	%ymm4, 608(%rsp)
	vpand	%ymm3, %ymm0, %ymm11
	vpxor	%ymm4, %ymm15, %ymm4
	vpand	%ymm4, %ymm11, %ymm4
	movl	%ebx, %esi
	vmovdqa	%ymm3, 576(%rsp)
	vpxor	%ymm2, %ymm7, %ymm7
	vpxor	%ymm14, %ymm15, %ymm3
	vpxor	256(%rsp), %ymm4, %ymm14
	vpxor	%ymm10, %ymm12, %ymm12
	vpxor	%ymm11, %ymm7, %ymm7
	vpand	%ymm13, %ymm0, %ymm10
	negl	%esi
	vpand	416(%rsp), %ymm0, %ymm9
	xorl	%ebx, %esi
	vpand	%ymm3, %ymm10, %ymm3
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	%ymm8, %ymm6, %ymm6
	vpxor	480(%rsp), %ymm15, %ymm2
	andl	%r8d, %esi
	vpxor	%ymm10, %ymm6, %ymm6
	vmovaps	%xmm14, 384(%rsp)
	vpxor	%ymm12, %ymm3, %ymm13
	vpor	%ymm6, %ymm13, %ymm13
	xorl	%esi, %ebx
	vpand	%ymm2, %ymm9, %ymm2
	vpxor	224(%rsp), %ymm5, %ymm5
	movq	384(%rsp), %rsi
	vpxor	192(%rsp), %ymm2, %ymm8
	vmovq	%xmm13, %rax
	vpxor	%ymm9, %ymm5, %ymm5
	shrq	%rsi
	salq	$63, %rax
	vpor	%ymm5, %ymm8, %ymm8
	orq	%rsi, %rax
	vmovdqa	%ymm12, 160(%rsp)
	vmovq	%xmm13, %r8
	vmovq	%rax, %xmm12
	vmovq	%xmm8, %rax
	shrq	%r8
	salq	$63, %rax
	orq	%r8, %rax
	vmovq	%xmm8, %rdx
	shrq	%rdx
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	vmovq	%rax, %xmm12
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vpermq	$57, %ymm14, %ymm14
	vmovq	%rdx, %xmm12
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vmovdqa	%ymm14, 384(%rsp)
	vpermq	$57, %ymm13, %ymm14
	vmovdqa	%ymm14, 352(%rsp)
	vpermq	$57, %ymm8, %ymm14
	vmovdqa	%ymm14, 224(%rsp)
	vpxor	256(%rsp), %ymm11, %ymm11
	vpxor	160(%rsp), %ymm10, %ymm10
	vpxor	%ymm7, %ymm4, %ymm4
	vpxor	%ymm6, %ymm3, %ymm3
	vpand	%ymm3, %ymm10, %ymm10
	vpand	%ymm4, %ymm11, %ymm11
	vmovq	%xmm10, %rax
	vmovq	%xmm11, %rsi
	vmovq	%xmm10, %r8
	vpxor	192(%rsp), %ymm9, %ymm9
	vpxor	%ymm5, %ymm2, %ymm2
	shrq	%rsi
	decl	%ebx
	shrq	%r8
	salq	$63, %rax
	vpand	%ymm2, %ymm9, %ymm9
	orq	%rsi, %rax
	vmovq	%rax, %xmm14
	vmovq	%xmm9, %rax
	salq	$63, %rax
	vmovdqa	128(%rsp), %ymm5
	orq	%r8, %rax
	vmovq	%xmm9, %rdx
	vmovq	%rax, %xmm12
	vpxor	640(%rsp), %ymm5, %ymm7
	vpblendd	$3, %ymm12, %ymm10, %ymm10
	shrq	%rdx
	vmovdqa	64(%rsp), %ymm6
	vmovq	%rdx, %xmm2
	vpermq	$57, %ymm10, %ymm12
	vmovdqa	320(%rsp), %ymm10
	vpand	%ymm1, %ymm7, %ymm7
	vpblendd	$3, %ymm2, %ymm9, %ymm9
	vpblendd	$3, %ymm14, %ymm11, %ymm11
	vpxor	%ymm5, %ymm7, %ymm3
	vpxor	%ymm10, %ymm6, %ymm8
	vmovdqa	32(%rsp), %ymm13
	vmovdqa	96(%rsp), %ymm5
	vmovdqa	544(%rsp), %ymm2
	vpermq	$57, %ymm11, %ymm14
	vpermq	$57, %ymm9, %ymm11
	vmovdqa	288(%rsp), %ymm9
	vpand	%ymm1, %ymm8, %ymm8
	vpxor	%ymm6, %ymm8, %ymm4
	vpxor	%ymm5, %ymm2, %ymm2
	vpxor	%ymm9, %ymm13, %ymm6
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm6, %ymm1
	vpxor	%ymm10, %ymm8, %ymm8
	vpxor	640(%rsp), %ymm7, %ymm6
	vpxor	%ymm13, %ymm1, %ymm10
	vpand	%ymm0, %ymm3, %ymm13
	vpxor	%ymm9, %ymm1, %ymm1
	vpxor	%ymm15, %ymm4, %ymm9
	vpand	%ymm9, %ymm13, %ymm9
	vpxor	%ymm13, %ymm6, %ymm6
	vpxor	%ymm8, %ymm9, %ymm7
	vpor	%ymm6, %ymm7, %ymm7
	vpxor	%ymm8, %ymm13, %ymm8
	vpxor	%ymm6, %ymm9, %ymm6
	vpxor	%ymm5, %ymm2, %ymm5
	vpand	%ymm6, %ymm8, %ymm6
	vpand	%ymm0, %ymm5, %ymm0
	vpxor	544(%rsp), %ymm2, %ymm2
	vmovdqa	%ymm6, 320(%rsp)
	vpxor	%ymm15, %ymm10, %ymm6
	vpand	%ymm6, %ymm0, %ymm6
	vpxor	%ymm0, %ymm2, %ymm2
	vmovdqa	%ymm7, 640(%rsp)
	vpxor	%ymm1, %ymm6, %ymm7
	vpor	%ymm2, %ymm7, %ymm7
	vpxor	%ymm1, %ymm0, %ymm1
	vpxor	%ymm2, %ymm6, %ymm2
	vmovdqa	%ymm7, 544(%rsp)
	vpand	%ymm2, %ymm1, %ymm7
	vmovdqa	%ymm7, 288(%rsp)
	decl	%ecx
	jne	.L10
	vpxor	%xmm7, %xmm7, %xmm7
	vmovdqa	%ymm11, 64(%rsp)
	vmovdqa	%ymm12, 160(%rsp)
	vmovdqa	%ymm14, %ymm15
	movl	$497, %ecx
	vmovdqa	%ymm7, 192(%rsp)
	vmovdqa	%ymm7, 256(%rsp)
	vmovdqa	%ymm7, %ymm11
	vmovdqa	%ymm7, %ymm12
	vmovdqa	%ymm10, %ymm14
	.p2align 4,,10
	.p2align 3
.L11:
	vpermq	$147, %ymm3, %ymm3
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm12, %ymm12
	vmovq	%xmm3, %rdx
	vmovq	%xmm5, %rax
	leaq	(%rdx,%rdx), %r8
	vmovq	%xmm12, %rsi
	vmovdqa	%ymm3, 864(%rsp)
	addq	%rsi, %rsi
	movq	%r8, 864(%rsp)
	shrq	$63, %rdx
	leaq	(%rax,%rax), %r8
	shrq	$63, %rax
	orq	%r8, %rdx
	orq	%rsi, %rax
	vpermq	$147, %ymm4, %ymm4
	vmovdqa	%ymm5, 896(%rsp)
	vmovq	%rax, %xmm0
	vpermq	$147, %ymm14, %ymm14
	movq	%rdx, 896(%rsp)
	vmovq	%xmm4, %rdx
	vpblendd	$3, %ymm0, %ymm12, %ymm6
	vmovq	%xmm14, %rax
	leaq	(%rdx,%rdx), %r8
	vmovdqa	%ymm4, 960(%rsp)
	vmovdqa	%ymm6, 32(%rsp)
	vpermq	$147, %ymm11, %ymm11
	vmovdqa	384(%rsp), %ymm6
	movq	%r8, 960(%rsp)
	shrq	$63, %rdx
	leaq	(%rax,%rax), %r8
	orq	%r8, %rdx
	vmovq	%xmm11, %rsi
	vmovdqa	%ymm14, 992(%rsp)
	addq	%rsi, %rsi
	movq	%rdx, 992(%rsp)
	shrq	$63, %rax
	vmovd	%xmm6, %edx
	orq	%rsi, %rax
	andl	$1, %edx
	vmovq	%rax, %xmm0
	movl	%ebx, %r8d
	movl	%edx, %eax
	negl	%eax
	sarl	$31, %r8d
	vmovdqa	576(%rsp), %ymm7
	andl	%eax, %r8d
	vpblendd	$3, %ymm0, %ymm11, %ymm11
	vmovd	%r8d, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpxor	%ymm7, %ymm6, %ymm14
	vpand	%ymm0, %ymm14, %ymm14
	vpxor	%ymm7, %ymm14, %ymm3
	vmovdqa	352(%rsp), %ymm12
	vmovd	%xmm7, %edx
	vmovdqa	448(%rsp), %ymm7
	vmovdqa	224(%rsp), %ymm8
	vpxor	%ymm7, %ymm12, %ymm5
	vpand	%ymm0, %ymm5, %ymm5
	vpxor	%ymm7, %ymm5, %ymm2
	vmovdqa	416(%rsp), %ymm7
	vmovdqa	608(%rsp), %ymm1
	vpxor	%ymm8, %ymm7, %ymm4
	vpand	%ymm0, %ymm4, %ymm4
	vpxor	%ymm7, %ymm4, %ymm7
	vmovdqa	%ymm7, 416(%rsp)
	andl	$1, %edx
	vpxor	%ymm1, %ymm15, %ymm7
	vpand	%ymm0, %ymm7, %ymm7
	negl	%edx
	andl	%eax, %edx
	vmovdqa	160(%rsp), %ymm13
	vmovd	%xmm1, %esi
	vmovd	%xmm15, %eax
	vpxor	%ymm1, %ymm7, %ymm1
	vpxor	%ymm7, %ymm15, %ymm7
	vmovdqa	512(%rsp), %ymm15
	vmovdqa	%ymm1, 608(%rsp)
	vpxor	%ymm13, %ymm15, %ymm10
	vpand	%ymm0, %ymm10, %ymm10
	vmovdqa	64(%rsp), %ymm1
	vpxor	%ymm15, %ymm10, %ymm15
	vpxor	%ymm13, %ymm10, %ymm13
	andl	$1, %eax
	vmovdqa	480(%rsp), %ymm10
	andl	$1, %esi
	negl	%esi
	negl	%eax
	xorl	%esi, %eax
	vpxor	%ymm1, %ymm10, %ymm9
	andl	%edx, %eax
	vpand	%ymm0, %ymm9, %ymm9
	vmovdqa	%ymm15, 512(%rsp)
	vmovdqa	%ymm13, 160(%rsp)
	vpxor	%ymm1, %ymm9, %ymm15
	vmovd	%edx, %xmm13
	vmovd	%eax, %xmm1
	vpbroadcastd	%xmm13, %ymm13
	vpbroadcastd	%xmm1, %ymm1
	vpxor	%ymm10, %ymm9, %ymm10
	vmovdqa	%ymm11, (%rsp)
	vmovdqa	%ymm3, 576(%rsp)
	vpand	%ymm3, %ymm13, %ymm11
	vpxor	608(%rsp), %ymm1, %ymm3
	vmovdqa	%ymm2, 448(%rsp)
	vmovdqa	%ymm10, 480(%rsp)
	vpand	%ymm2, %ymm13, %ymm10
	vpxor	512(%rsp), %ymm1, %ymm2
	vpand	%ymm3, %ymm11, %ymm3
	movl	%ebx, %esi
	vpxor	%ymm6, %ymm14, %ymm14
	vmovdqa	416(%rsp), %ymm9
	vpxor	%ymm11, %ymm14, %ymm6
	vpand	%ymm2, %ymm10, %ymm2
	vpxor	%ymm7, %ymm3, %ymm14
	negl	%esi
	vpand	%ymm13, %ymm9, %ymm9
	xorl	%ebx, %esi
	vpor	%ymm6, %ymm14, %ymm14
	vpxor	%ymm12, %ymm5, %ymm5
	vmovdqa	%ymm13, 128(%rsp)
	vmovdqa	%ymm1, 96(%rsp)
	vpxor	160(%rsp), %ymm2, %ymm13
	andl	%r8d, %esi
	vpxor	%ymm10, %ymm5, %ymm5
	vmovaps	%xmm14, 384(%rsp)
	vpxor	480(%rsp), %ymm1, %ymm1
	vpor	%ymm5, %ymm13, %ymm13
	xorl	%esi, %ebx
	movq	384(%rsp), %rsi
	vpand	%ymm1, %ymm9, %ymm1
	vpxor	%ymm8, %ymm4, %ymm4
	vmovq	%xmm13, %rax
	vpxor	%ymm9, %ymm4, %ymm4
	vpxor	%ymm15, %ymm1, %ymm8
	shrq	%rsi
	salq	$63, %rax
	vpor	%ymm4, %ymm8, %ymm8
	orq	%rsi, %rax
	vmovq	%rax, %xmm12
	vpxor	160(%rsp), %ymm10, %ymm10
	vmovq	%xmm13, %r8
	vmovq	%xmm8, %rax
	shrq	%r8
	vpxor	%ymm6, %ymm3, %ymm3
	vpxor	%ymm5, %ymm2, %ymm2
	vpxor	%ymm7, %ymm11, %ymm7
	decl	%ebx
	salq	$63, %rax
	vpand	%ymm2, %ymm10, %ymm10
	orq	%r8, %rax
	vpand	%ymm3, %ymm7, %ymm7
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	vmovq	%xmm7, %rsi
	vmovq	%rax, %xmm12
	vmovq	%xmm10, %rax
	vpxor	%ymm4, %ymm1, %ymm1
	vpxor	%ymm15, %ymm9, %ymm9
	shrq	%rsi
	salq	$63, %rax
	vpand	%ymm1, %ymm9, %ymm9
	orq	%rsi, %rax
	vmovq	%rax, %xmm15
	vmovq	%xmm8, %rdx
	vmovq	%xmm10, %r8
	vmovq	%xmm9, %rax
	shrq	%rdx
	shrq	%r8
	salq	$63, %rax
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vpermq	$57, %ymm14, %ymm14
	vmovq	%rdx, %xmm12
	orq	%r8, %rax
	vmovq	%xmm9, %rdx
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	vmovdqa	%ymm14, 384(%rsp)
	vmovq	%rax, %xmm2
	vpermq	$57, %ymm13, %ymm14
	shrq	%rdx
	vpblendd	$3, %ymm2, %ymm10, %ymm10
	vmovdqa	960(%rsp), %ymm4
	vmovdqa	%ymm14, 352(%rsp)
	vpblendd	$3, %ymm15, %ymm7, %ymm7
	vpermq	$57, %ymm8, %ymm14
	vmovq	%rdx, %xmm1
	vmovdqa	320(%rsp), %ymm8
	vpblendd	$3, %ymm1, %ymm9, %ymm9
	vpermq	$57, %ymm7, %ymm15
	vpermq	$57, %ymm10, %ymm7
	vmovdqa	%ymm14, 224(%rsp)
	vmovdqa	288(%rsp), %ymm10
	vmovdqa	992(%rsp), %ymm14
	vmovdqa	%ymm7, 160(%rsp)
	vpermq	$57, %ymm9, %ymm7
	vpxor	%ymm8, %ymm4, %ymm9
	vmovdqa	32(%rsp), %ymm6
	vpand	%ymm0, %ymm9, %ymm9
	vmovdqa	864(%rsp), %ymm3
	vpxor	%ymm9, %ymm4, %ymm4
	vpxor	%ymm8, %ymm9, %ymm9
	vpxor	%ymm10, %ymm14, %ymm8
	vmovdqa	896(%rsp), %ymm5
	vpxor	256(%rsp), %ymm6, %ymm1
	vpand	%ymm0, %ymm8, %ymm8
	vmovdqa	(%rsp), %ymm11
	vmovdqa	%ymm7, 64(%rsp)
	vpxor	%ymm8, %ymm14, %ymm14
	vpxor	640(%rsp), %ymm3, %ymm7
	vpxor	%ymm10, %ymm8, %ymm8
	vmovdqa	192(%rsp), %ymm10
	vpxor	544(%rsp), %ymm5, %ymm2
	vpand	%ymm0, %ymm1, %ymm1
	vpand	%ymm0, %ymm7, %ymm7
	vpxor	%ymm6, %ymm1, %ymm12
	vpxor	%ymm10, %ymm11, %ymm6
	vpand	%ymm0, %ymm2, %ymm2
	vpxor	%ymm7, %ymm3, %ymm3
	vpand	%ymm0, %ymm6, %ymm0
	vpand	128(%rsp), %ymm3, %ymm13
	vpxor	%ymm11, %ymm0, %ymm11
	vpxor	%ymm10, %ymm0, %ymm0
	vpxor	96(%rsp), %ymm4, %ymm10
	vpxor	640(%rsp), %ymm7, %ymm6
	vpand	%ymm10, %ymm13, %ymm10
	vpxor	%ymm13, %ymm6, %ymm6
	vpxor	%ymm9, %ymm10, %ymm7
	vpor	%ymm6, %ymm7, %ymm7
	vpxor	%ymm9, %ymm13, %ymm9
	vpxor	%ymm6, %ymm10, %ymm6
	vmovdqa	128(%rsp), %ymm13
	vmovdqa	96(%rsp), %ymm10
	vpand	%ymm6, %ymm9, %ymm6
	vpxor	%ymm2, %ymm5, %ymm5
	vmovdqa	%ymm7, 640(%rsp)
	vpxor	544(%rsp), %ymm2, %ymm2
	vpand	%ymm13, %ymm5, %ymm7
	vmovdqa	%ymm6, 320(%rsp)
	vpxor	%ymm10, %ymm14, %ymm6
	vpand	%ymm6, %ymm7, %ymm6
	vpxor	%ymm7, %ymm2, %ymm2
	vpxor	%ymm8, %ymm6, %ymm9
	vpor	%ymm2, %ymm9, %ymm9
	vpxor	%ymm8, %ymm7, %ymm8
	vpxor	%ymm2, %ymm6, %ymm2
	vpand	%ymm2, %ymm8, %ymm6
	vmovdqa	%ymm6, 288(%rsp)
	vmovdqa	%ymm9, 544(%rsp)
	vpand	%ymm13, %ymm12, %ymm6
	vpxor	256(%rsp), %ymm1, %ymm1
	vpxor	%ymm10, %ymm11, %ymm2
	vpand	%ymm2, %ymm6, %ymm2
	vpxor	%ymm6, %ymm1, %ymm1
	vpxor	%ymm0, %ymm2, %ymm7
	vpor	%ymm1, %ymm7, %ymm7
	vpxor	%ymm0, %ymm6, %ymm0
	vpxor	%ymm1, %ymm2, %ymm1
	vmovdqa	%ymm7, 256(%rsp)
	vpand	%ymm1, %ymm0, %ymm7
	vmovdqa	%ymm7, 192(%rsp)
	decl	%ecx
	jne	.L11
	vmovdqa	224(%rsp), %ymm7
	vmovdqa	%ymm11, %ymm8
	vmovdqa	%ymm12, 480(%rsp)
	vmovdqa	64(%rsp), %ymm11
	vmovdqa	160(%rsp), %ymm12
	vmovdqa	%ymm14, 416(%rsp)
	vmovdqa	%ymm7, 736(%rsp)
	vmovdqa	%ymm15, %ymm14
	vmovdqa	%ymm11, 832(%rsp)
	movl	$256, %ecx
	vmovdqa	%ymm12, 224(%rsp)
	.p2align 4,,10
	.p2align 3
.L12:
	vpermq	$147, 480(%rsp), %ymm9
	vpermq	$147, %ymm3, %ymm3
	vpermq	$147, %ymm5, %ymm5
	vmovq	%xmm3, %rdx
	vmovq	%xmm5, %rax
	leaq	(%rdx,%rdx), %r8
	vmovq	%xmm9, %rsi
	vmovdqa	%ymm3, 864(%rsp)
	addq	%rsi, %rsi
	movq	%r8, 864(%rsp)
	leaq	(%rax,%rax), %r8
	shrq	$63, %rax
	orq	%rsi, %rax
	vmovq	%rax, %xmm0
	shrq	$63, %rdx
	orq	%r8, %rdx
	vpblendd	$3, %ymm0, %ymm9, %ymm7
	vpermq	$147, %ymm4, %ymm4
	vpermq	$147, 416(%rsp), %ymm0
	vmovdqa	%ymm5, 896(%rsp)
	movq	%rdx, 896(%rsp)
	vmovq	%xmm4, %rdx
	vmovq	%xmm0, %rax
	leaq	(%rdx,%rdx), %r8
	vmovdqa	384(%rsp), %ymm2
	vmovdqa	%ymm4, 960(%rsp)
	vpermq	$147, %ymm8, %ymm8
	movq	%r8, 960(%rsp)
	shrq	$63, %rdx
	leaq	(%rax,%rax), %r8
	orq	%r8, %rdx
	vmovq	%xmm8, %rsi
	vmovdqa	%ymm0, 992(%rsp)
	addq	%rsi, %rsi
	movq	%rdx, 992(%rsp)
	shrq	$63, %rax
	vmovd	%xmm2, %edx
	orq	%rsi, %rax
	andl	$1, %edx
	vmovq	%rax, %xmm0
	movl	%ebx, %r8d
	movl	%edx, %eax
	negl	%eax
	sarl	$31, %r8d
	vmovdqa	%ymm7, 480(%rsp)
	andl	%eax, %r8d
	vmovdqa	576(%rsp), %ymm7
	vmovd	%r8d, %xmm10
	vpbroadcastd	%xmm10, %ymm10
	vmovdqa	608(%rsp), %ymm1
	vpxor	%ymm7, %ymm2, %ymm5
	vmovd	%xmm7, %edx
	vpand	%ymm10, %ymm5, %ymm5
	andl	$1, %edx
	vpxor	%ymm7, %ymm5, %ymm3
	vmovdqa	352(%rsp), %ymm15
	vmovdqa	448(%rsp), %ymm6
	vpxor	%ymm1, %ymm14, %ymm7
	negl	%edx
	andl	%eax, %edx
	vpand	%ymm10, %ymm7, %ymm7
	vmovd	%xmm1, %esi
	vmovd	%xmm14, %eax
	vpxor	%ymm1, %ymm7, %ymm12
	vpblendd	$3, %ymm0, %ymm8, %ymm8
	vmovdqa	224(%rsp), %ymm1
	vmovdqa	512(%rsp), %ymm0
	vpxor	%ymm6, %ymm15, %ymm4
	andl	$1, %eax
	andl	$1, %esi
	negl	%esi
	vpand	%ymm10, %ymm4, %ymm4
	negl	%eax
	vpxor	%ymm6, %ymm4, %ymm13
	xorl	%esi, %eax
	vpxor	%ymm0, %ymm1, %ymm6
	andl	%edx, %eax
	vpand	%ymm10, %ymm6, %ymm6
	vpxor	%ymm7, %ymm14, %ymm7
	vpxor	%ymm0, %ymm6, %ymm14
	vpxor	%ymm1, %ymm6, %ymm6
	vmovd	%eax, %xmm0
	vmovd	%edx, %xmm1
	vpbroadcastd	%xmm1, %ymm1
	vpbroadcastd	%xmm0, %ymm0
	vpand	%ymm3, %ymm1, %ymm11
	vpand	%ymm13, %ymm1, %ymm9
	movl	%ebx, %esi
	vmovdqa	%ymm3, 576(%rsp)
	vpxor	%ymm2, %ymm5, %ymm5
	vpxor	%ymm12, %ymm0, %ymm3
	vpxor	%ymm14, %ymm0, %ymm2
	vpand	%ymm3, %ymm11, %ymm3
	vpand	%ymm2, %ymm9, %ymm2
	negl	%esi
	vpxor	%ymm15, %ymm4, %ymm4
	vpxor	%ymm11, %ymm5, %ymm5
	vpxor	%ymm9, %ymm4, %ymm4
	xorl	%ebx, %esi
	vmovdqa	%ymm12, 608(%rsp)
	vmovdqa	%ymm14, 512(%rsp)
	vpxor	%ymm6, %ymm2, %ymm12
	vpxor	%ymm7, %ymm3, %ymm14
	vpor	%ymm4, %ymm12, %ymm12
	andl	%r8d, %esi
	vpor	%ymm5, %ymm14, %ymm14
	xorl	%esi, %ebx
	vmovq	%xmm14, %rdx
	vmovq	%xmm12, %rsi
	salq	$63, %rsi
	vpxor	%ymm5, %ymm3, %ymm3
	vpxor	%ymm4, %ymm2, %ymm2
	shrq	%rdx
	vpxor	%ymm7, %ymm11, %ymm7
	vpxor	%ymm6, %ymm9, %ymm6
	orq	%rsi, %rdx
	vpand	%ymm2, %ymm6, %ymm6
	vpand	%ymm3, %ymm7, %ymm7
	vmovdqa	%ymm13, 448(%rsp)
	vmovq	%xmm12, %rax
	vmovq	%rdx, %xmm13
	vmovq	%xmm6, %rsi
	vmovq	%xmm7, %rdx
	shrq	%rax
	decl	%ebx
	shrq	%rdx
	salq	$63, %rsi
	orq	%rsi, %rdx
	vmovq	%rdx, %xmm11
	vpblendd	$3, %ymm13, %ymm14, %ymm14
	vpblendd	$3, %ymm11, %ymm7, %ymm7
	vmovq	%rax, %xmm13
	vmovq	%xmm6, %rax
	vpermq	$57, %ymm14, %ymm15
	shrq	%rax
	vpermq	$57, %ymm7, %ymm14
	vmovdqa	480(%rsp), %ymm7
	vmovq	%rax, %xmm9
	vpblendd	$3, %ymm9, %ymm6, %ymm6
	vpxor	256(%rsp), %ymm7, %ymm2
	vpermq	$57, %ymm6, %ymm5
	vpblendd	$3, %ymm13, %ymm12, %ymm12
	vmovdqa	864(%rsp), %ymm3
	vpand	%ymm10, %ymm2, %ymm2
	vmovdqa	%ymm5, 224(%rsp)
	vmovdqa	896(%rsp), %ymm5
	vpxor	%ymm7, %ymm2, %ymm13
	vmovdqa	%ymm15, 384(%rsp)
	vpermq	$57, %ymm12, %ymm15
	vpxor	544(%rsp), %ymm5, %ymm6
	vmovdqa	%ymm15, 352(%rsp)
	vmovdqa	%ymm13, 480(%rsp)
	vpxor	640(%rsp), %ymm3, %ymm15
	vmovdqa	960(%rsp), %ymm4
	vmovdqa	320(%rsp), %ymm11
	vmovdqa	288(%rsp), %ymm9
	vpxor	%ymm11, %ymm4, %ymm12
	vmovdqa	992(%rsp), %ymm7
	vpand	%ymm10, %ymm12, %ymm12
	vpxor	%ymm12, %ymm4, %ymm4
	vpxor	%ymm11, %ymm12, %ymm12
	vpxor	%ymm9, %ymm7, %ymm11
	vpand	%ymm10, %ymm11, %ymm11
	vpxor	%ymm11, %ymm7, %ymm7
	vpxor	%ymm9, %ymm11, %ymm11
	vmovdqa	192(%rsp), %ymm9
	vpand	%ymm10, %ymm15, %ymm15
	vmovdqa	%ymm7, 416(%rsp)
	vpxor	%ymm9, %ymm8, %ymm7
	vpand	%ymm10, %ymm6, %ymm6
	vpxor	%ymm15, %ymm3, %ymm3
	vpand	%ymm10, %ymm7, %ymm10
	vpxor	640(%rsp), %ymm15, %ymm15
	vpxor	%ymm8, %ymm10, %ymm8
	vpxor	%ymm0, %ymm4, %ymm13
	vpxor	%ymm9, %ymm10, %ymm10
	vpand	%ymm1, %ymm3, %ymm9
	vpand	%ymm13, %ymm9, %ymm13
	vpxor	%ymm9, %ymm15, %ymm7
	vpxor	%ymm12, %ymm13, %ymm15
	vpor	%ymm7, %ymm15, %ymm15
	vpxor	%ymm12, %ymm9, %ymm12
	vpxor	%ymm7, %ymm13, %ymm7
	vpand	%ymm7, %ymm12, %ymm12
	vpxor	%ymm6, %ymm5, %ymm5
	vpxor	416(%rsp), %ymm0, %ymm7
	vpxor	544(%rsp), %ymm6, %ymm6
	vmovdqa	%ymm12, 320(%rsp)
	vpand	%ymm1, %ymm5, %ymm12
	vpand	%ymm7, %ymm12, %ymm7
	vpxor	%ymm11, %ymm7, %ymm13
	vpand	480(%rsp), %ymm1, %ymm1
	vpxor	%ymm12, %ymm6, %ymm6
	vpxor	256(%rsp), %ymm2, %ymm2
	vmovdqa	%ymm15, 640(%rsp)
	vpxor	%ymm11, %ymm12, %ymm11
	vpor	%ymm6, %ymm13, %ymm15
	vpxor	%ymm0, %ymm8, %ymm0
	vpxor	%ymm6, %ymm7, %ymm6
	vpand	%ymm0, %ymm1, %ymm0
	vpand	%ymm6, %ymm11, %ymm6
	vpxor	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm6, 288(%rsp)
	vpxor	%ymm10, %ymm0, %ymm6
	vpor	%ymm2, %ymm6, %ymm7
	vpxor	%ymm10, %ymm1, %ymm1
	vpxor	%ymm2, %ymm0, %ymm0
	vmovdqa	%ymm7, 256(%rsp)
	vpand	%ymm0, %ymm1, %ymm7
	vmovdqa	%ymm15, 544(%rsp)
	vmovdqa	%ymm7, 192(%rsp)
	decl	%ecx
	jne	.L12
	vmovdqa	480(%rsp), %ymm7
	vmovdqa	224(%rsp), %ymm12
	vmovdqa	%ymm7, 928(%rsp)
	vmovdqa	384(%rsp), %ymm7
	vmovdqa	%ymm14, 768(%rsp)
	vmovdqa	%ymm7, 672(%rsp)
	vmovdqa	352(%rsp), %ymm7
	vmovdqa	416(%rsp), %ymm2
	vmovdqa	%ymm7, 704(%rsp)
	vmovdqa	%ymm14, 416(%rsp)
	vmovdqa	320(%rsp), %ymm7
	vmovdqa	256(%rsp), %ymm15
	vmovdqa	288(%rsp), %ymm13
	vmovdqa	192(%rsp), %ymm14
	vmovdqa	%ymm8, 512(%rsp)
	vmovdqa	%ymm8, 1024(%rsp)
	vmovdqa	%ymm12, 800(%rsp)
	movl	$256, %ecx
	vmovdqa	%ymm4, 448(%rsp)
	.p2align 4,,10
	.p2align 3
.L13:
	vpermq	$147, 480(%rsp), %ymm9
	vpermq	$147, %ymm3, %ymm3
	vpermq	$147, %ymm5, %ymm5
	vmovq	%xmm3, %rdx
	vmovq	%xmm5, %rax
	leaq	(%rdx,%rdx), %r8
	vmovq	%xmm9, %rsi
	vmovdqa	%ymm3, 864(%rsp)
	addq	%rsi, %rsi
	movq	%r8, 864(%rsp)
	leaq	(%rax,%rax), %r8
	shrq	$63, %rax
	orq	%rsi, %rax
	vmovq	%rax, %xmm0
	vpblendd	$3, %ymm0, %ymm9, %ymm9
	shrq	$63, %rdx
	vpermq	$147, 448(%rsp), %ymm0
	orq	%r8, %rdx
	vmovdqa	%ymm5, 896(%rsp)
	vpermq	$147, %ymm2, %ymm2
	movq	%rdx, 896(%rsp)
	vmovq	%xmm0, %rdx
	vpermq	$147, 512(%rsp), %ymm12
	vmovq	%xmm2, %rax
	leaq	(%rdx,%rdx), %r8
	vmovdqa	384(%rsp), %ymm5
	vmovdqa	%ymm0, 960(%rsp)
	shrq	$63, %rdx
	movq	%r8, 960(%rsp)
	leaq	(%rax,%rax), %r8
	orq	%r8, %rdx
	vmovq	%xmm12, %rsi
	vmovdqa	%ymm2, 992(%rsp)
	vmovdqa	576(%rsp), %ymm6
	movq	%rdx, 992(%rsp)
	addq	%rsi, %rsi
	vmovd	%xmm5, %edx
	shrq	$63, %rax
	orq	%rsi, %rax
	andl	$1, %edx
	vmovq	%rax, %xmm0
	movl	%edx, %eax
	vmovd	%xmm6, %edx
	vmovdqa	416(%rsp), %ymm1
	vmovdqa	608(%rsp), %ymm2
	movl	%ebx, %r8d
	andl	$1, %edx
	negl	%eax
	sarl	$31, %r8d
	negl	%edx
	andl	%eax, %r8d
	andl	%eax, %edx
	vmovd	%xmm2, %esi
	vmovd	%xmm1, %eax
	andl	$1, %eax
	andl	$1, %esi
	negl	%esi
	negl	%eax
	vmovd	%r8d, %xmm8
	vpbroadcastd	%xmm8, %ymm8
	vpxor	%ymm1, %ymm2, %ymm3
	xorl	%esi, %eax
	vpxor	%ymm6, %ymm5, %ymm4
	vpand	%ymm8, %ymm3, %ymm3
	andl	%edx, %eax
	vpblendd	$3, %ymm0, %ymm12, %ymm12
	vpand	%ymm8, %ymm4, %ymm4
	vpxor	%ymm2, %ymm3, %ymm2
	vmovd	%eax, %xmm0
	vpxor	%ymm1, %ymm3, %ymm3
	vmovd	%edx, %xmm1
	vpbroadcastd	%xmm1, %ymm1
	vpbroadcastd	%xmm0, %ymm0
	vpxor	%ymm6, %ymm4, %ymm6
	vmovdqa	%ymm6, 576(%rsp)
	vmovdqa	%ymm2, 608(%rsp)
	vpand	%ymm6, %ymm1, %ymm6
	vpxor	%ymm2, %ymm0, %ymm2
	vpand	%ymm2, %ymm6, %ymm2
	vpxor	%ymm5, %ymm4, %ymm4
	vpxor	%ymm6, %ymm4, %ymm4
	vpxor	%ymm2, %ymm3, %ymm5
	vpor	%ymm4, %ymm5, %ymm5
	vpxor	%ymm4, %ymm2, %ymm2
	vmovq	%xmm5, %rax
	vpxor	%ymm6, %ymm3, %ymm3
	vpand	%ymm2, %ymm3, %ymm3
	shrq	%rax
	vmovq	%rax, %xmm10
	vmovq	%xmm3, %rax
	shrq	%rax
	vmovq	%rax, %xmm6
	vpblendd	$3, %ymm6, %ymm3, %ymm3
	vpermq	$57, %ymm3, %ymm4
	vmovdqa	%ymm4, 416(%rsp)
	vpxor	%ymm9, %ymm15, %ymm4
	vpand	%ymm8, %ymm4, %ymm4
	vpxor	%ymm9, %ymm4, %ymm2
	vmovdqa	%ymm2, 480(%rsp)
	vmovdqa	960(%rsp), %ymm2
	vpblendd	$3, %ymm10, %ymm5, %ymm5
	vpxor	%ymm2, %ymm7, %ymm10
	vpand	%ymm8, %ymm10, %ymm10
	vpermq	$57, %ymm5, %ymm5
	vmovdqa	864(%rsp), %ymm3
	vpxor	%ymm10, %ymm2, %ymm2
	vmovdqa	%ymm5, 384(%rsp)
	vmovdqa	896(%rsp), %ymm5
	vmovdqa	%ymm2, 448(%rsp)
	vmovdqa	%ymm2, 960(%rsp)
	vmovdqa	992(%rsp), %ymm2
	vpxor	640(%rsp), %ymm3, %ymm11
	vpxor	544(%rsp), %ymm5, %ymm6
	vpxor	%ymm10, %ymm7, %ymm10
	vpxor	%ymm2, %ymm13, %ymm9
	vpxor	%ymm12, %ymm14, %ymm7
	vpand	%ymm8, %ymm11, %ymm11
	vpand	%ymm8, %ymm6, %ymm6
	vpand	%ymm8, %ymm9, %ymm9
	vpand	%ymm8, %ymm7, %ymm8
	vpxor	%ymm11, %ymm3, %ymm3
	vpxor	%ymm6, %ymm5, %ymm5
	vpxor	%ymm9, %ymm2, %ymm2
	vpxor	%ymm9, %ymm13, %ymm9
	vpxor	%ymm12, %ymm8, %ymm13
	vmovdqa	%ymm13, 512(%rsp)
	vmovdqa	%ymm3, 864(%rsp)
	vmovdqa	%ymm5, 896(%rsp)
	vmovdqa	%ymm2, 992(%rsp)
	vpxor	448(%rsp), %ymm0, %ymm12
	vpxor	%ymm8, %ymm14, %ymm8
	vpxor	640(%rsp), %ymm11, %ymm7
	vpand	%ymm1, %ymm3, %ymm14
	vpand	%ymm12, %ymm14, %ymm12
	vpand	%ymm1, %ymm5, %ymm13
	vpxor	%ymm14, %ymm7, %ymm7
	movl	%ebx, %esi
	vpxor	%ymm0, %ymm2, %ymm11
	vpxor	544(%rsp), %ymm6, %ymm6
	vpxor	%ymm4, %ymm15, %ymm4
	vpxor	%ymm10, %ymm12, %ymm15
	vpand	%ymm11, %ymm13, %ymm11
	vpand	480(%rsp), %ymm1, %ymm1
	vpor	%ymm7, %ymm15, %ymm15
	negl	%esi
	vpxor	512(%rsp), %ymm0, %ymm0
	vpxor	%ymm13, %ymm6, %ymm6
	xorl	%ebx, %esi
	vmovdqa	%ymm15, 640(%rsp)
	vpxor	%ymm9, %ymm11, %ymm15
	vpor	%ymm6, %ymm15, %ymm15
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	%ymm1, %ymm4, %ymm4
	andl	%r8d, %esi
	vpxor	%ymm10, %ymm14, %ymm10
	xorl	%esi, %ebx
	vmovdqa	%ymm15, 544(%rsp)
	vpxor	%ymm7, %ymm12, %ymm7
	vpxor	%ymm8, %ymm0, %ymm15
	vpxor	%ymm9, %ymm13, %ymm9
	vpxor	%ymm6, %ymm11, %ymm6
	vpxor	%ymm8, %ymm1, %ymm8
	vpxor	%ymm4, %ymm0, %ymm0
	decl	%ebx
	vpor	%ymm4, %ymm15, %ymm15
	vpand	%ymm7, %ymm10, %ymm7
	vpand	%ymm6, %ymm9, %ymm13
	vpand	%ymm0, %ymm8, %ymm14
	decl	%ecx
	jne	.L13
	vmovdqa	576(%rsp), %xmm0
	vmovdqa	448(%rsp), %ymm4
	vmovd	%xmm0, %eax
	andl	$1, %eax
	vmovdqa	608(%rsp), %xmm0
	negl	%eax
	vmovd	%eax, %xmm1
	vmovd	%xmm0, %eax
	andl	$1, %eax
	negl	%eax
	vpbroadcastd	%xmm1, %ymm1
	vmovd	%eax, %xmm0
	vpbroadcastd	%xmm0, %ymm0
	vpand	%ymm1, %ymm3, %ymm3
	vpand	%ymm1, %ymm5, %ymm13
	vpxor	%ymm2, %ymm0, %ymm7
	vpand	480(%rsp), %ymm1, %ymm14
	vpxor	%ymm0, %ymm4, %ymm15
	vpxor	512(%rsp), %ymm0, %ymm1
	vpshufd	$216, %ymm3, %ymm0
	vmovdqa	.LC6(%rip), %ymm5
	vpermq	$216, %ymm0, %ymm0
	vpshufb	.LC5(%rip), %ymm0, %ymm0
	vpand	%ymm5, %ymm0, %ymm2
	vpsrld	$4, %ymm0, %ymm0
	vpand	%ymm5, %ymm0, %ymm0
	vperm2i128	$32, %ymm0, %ymm2, %ymm4
	vmovdqa	%ymm1, 608(%rsp)
	vperm2i128	$49, %ymm0, %ymm2, %ymm2
	vmovdqa	.LC7(%rip), %ymm1
	vpsrld	$1, %ymm4, %ymm6
	vpsrld	$1, %ymm2, %ymm0
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm0, %ymm0
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm7, 640(%rsp)
	vpunpckldq	%ymm6, %ymm4, %ymm7
	vpunpckhdq	%ymm6, %ymm4, %ymm4
	vpunpckldq	%ymm0, %ymm2, %ymm6
	vpunpckhdq	%ymm0, %ymm2, %ymm2
	vmovdqa	.LC8(%rip), %ymm0
	xorl	%eax, %eax
	vpand	%ymm0, %ymm7, %ymm12
	vpand	%ymm0, %ymm4, %ymm10
	vpsrld	$2, %ymm7, %ymm7
	vpsrld	$2, %ymm4, %ymm4
	vpand	%ymm0, %ymm7, %ymm7
	vpand	%ymm0, %ymm4, %ymm4
	vpunpcklqdq	%ymm7, %ymm12, %ymm11
	vpunpckhqdq	%ymm7, %ymm12, %ymm7
	vpunpcklqdq	%ymm4, %ymm10, %ymm12
	vpunpckhqdq	%ymm4, %ymm10, %ymm4
	vpand	%ymm0, %ymm6, %ymm9
	vpand	%ymm0, %ymm2, %ymm8
	vmovdqa	%ymm4, 1152(%rsp)
	vpsrld	$2, %ymm6, %ymm6
	vpsrld	$2, %ymm2, %ymm2
	vpshufd	$216, %ymm13, %ymm4
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm2, %ymm2
	vpermq	$216, %ymm4, %ymm4
	vpunpcklqdq	%ymm6, %ymm9, %ymm10
	vpshufb	.LC5(%rip), %ymm4, %ymm4
	vpunpckhqdq	%ymm6, %ymm9, %ymm6
	vpunpcklqdq	%ymm2, %ymm8, %ymm9
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vmovdqa	%ymm2, 1280(%rsp)
	vpsrld	$4, %ymm4, %ymm2
	vpand	%ymm5, %ymm2, %ymm2
	vmovdqa	%ymm6, 1216(%rsp)
	vpand	%ymm5, %ymm4, %ymm6
	vperm2i128	$32, %ymm2, %ymm6, %ymm4
	vperm2i128	$49, %ymm2, %ymm6, %ymm2
	vpsrld	$1, %ymm4, %ymm6
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm4, %ymm4
	vmovdqa	%ymm7, 1088(%rsp)
	vpunpckldq	%ymm6, %ymm4, %ymm7
	vpunpckhdq	%ymm6, %ymm4, %ymm4
	vmovdqa	%ymm12, 1120(%rsp)
	vmovdqa	%ymm10, 1184(%rsp)
	vpand	%ymm0, %ymm7, %ymm12
	vpand	%ymm0, %ymm4, %ymm10
	vpsrld	$1, %ymm2, %ymm8
	vpsrld	$2, %ymm7, %ymm7
	vpsrld	$2, %ymm4, %ymm4
	vpand	%ymm1, %ymm8, %ymm8
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm0, %ymm7, %ymm7
	vpand	%ymm0, %ymm4, %ymm4
	vpunpckldq	%ymm8, %ymm2, %ymm6
	vmovdqa	%ymm11, 1056(%rsp)
	vpunpckhdq	%ymm8, %ymm2, %ymm2
	vpunpcklqdq	%ymm7, %ymm12, %ymm11
	vpunpckhqdq	%ymm7, %ymm12, %ymm7
	vpunpcklqdq	%ymm4, %ymm10, %ymm12
	vpunpckhqdq	%ymm4, %ymm10, %ymm4
	vpand	%ymm0, %ymm2, %ymm8
	vmovdqa	%ymm9, 1248(%rsp)
	vmovdqa	%ymm4, 1408(%rsp)
	vpand	%ymm0, %ymm6, %ymm9
	vpsrld	$2, %ymm2, %ymm2
	vpsrld	$2, %ymm6, %ymm6
	vpshufd	$216, %ymm14, %ymm4
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm2, %ymm2
	vpermq	$216, %ymm4, %ymm4
	vpunpcklqdq	%ymm6, %ymm9, %ymm10
	vpshufb	.LC5(%rip), %ymm4, %ymm4
	vpunpckhqdq	%ymm6, %ymm9, %ymm6
	vpunpcklqdq	%ymm2, %ymm8, %ymm9
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vmovdqa	%ymm2, 1536(%rsp)
	vpsrld	$4, %ymm4, %ymm2
	vpand	%ymm5, %ymm2, %ymm2
	vmovdqa	%ymm6, 1472(%rsp)
	vpand	%ymm5, %ymm4, %ymm6
	vperm2i128	$32, %ymm2, %ymm6, %ymm4
	vperm2i128	$49, %ymm2, %ymm6, %ymm2
	vpsrld	$1, %ymm2, %ymm8
	vpsrld	$1, %ymm4, %ymm6
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm8, %ymm8
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm7, 1344(%rsp)
	vpunpckldq	%ymm6, %ymm4, %ymm7
	vpunpckhdq	%ymm6, %ymm4, %ymm4
	vpunpckldq	%ymm8, %ymm2, %ymm6
	vpunpckhdq	%ymm8, %ymm2, %ymm2
	vpand	%ymm0, %ymm2, %ymm8
	vmovdqa	%ymm12, 1376(%rsp)
	vmovdqa	%ymm10, 1440(%rsp)
	vpand	%ymm0, %ymm7, %ymm12
	vpand	%ymm0, %ymm4, %ymm10
	vmovdqa	%ymm9, 1504(%rsp)
	vpsrld	$2, %ymm7, %ymm7
	vpand	%ymm0, %ymm6, %ymm9
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm6, %ymm6
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm7, %ymm7
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm2, %ymm2
	vmovdqa	%ymm11, 1312(%rsp)
	vpunpcklqdq	%ymm7, %ymm12, %ymm11
	vpunpckhqdq	%ymm7, %ymm12, %ymm7
	vpunpcklqdq	%ymm4, %ymm10, %ymm12
	vpunpckhqdq	%ymm4, %ymm10, %ymm4
	vpunpcklqdq	%ymm6, %ymm9, %ymm10
	vpunpckhqdq	%ymm6, %ymm9, %ymm6
	vpunpcklqdq	%ymm2, %ymm8, %ymm9
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vmovdqa	%ymm11, 1568(%rsp)
	vmovdqa	%ymm7, 1600(%rsp)
	vmovdqa	%ymm12, 1632(%rsp)
	vmovdqa	%ymm4, 1664(%rsp)
	vmovdqa	%ymm2, 1792(%rsp)
	vpand	%ymm15, %ymm3, %ymm2
	vpshufd	$216, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC5(%rip), %ymm2, %ymm2
	vpand	%ymm5, %ymm2, %ymm4
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm5, %ymm2, %ymm2
	vperm2i128	$32, %ymm2, %ymm4, %ymm3
	vperm2i128	$49, %ymm2, %ymm4, %ymm2
	vpsrld	$1, %ymm2, %ymm7
	vpsrld	$1, %ymm3, %ymm4
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm7, %ymm7
	vpand	%ymm1, %ymm3, %ymm3
	vpand	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm6, 1728(%rsp)
	vpunpckldq	%ymm4, %ymm3, %ymm6
	vpunpckhdq	%ymm4, %ymm3, %ymm3
	vpunpckldq	%ymm7, %ymm2, %ymm4
	vpunpckhdq	%ymm7, %ymm2, %ymm2
	vpand	%ymm0, %ymm6, %ymm11
	vpand	%ymm0, %ymm4, %ymm8
	vpand	%ymm0, %ymm2, %ymm7
	vmovdqa	%ymm9, 1760(%rsp)
	vpsrld	$2, %ymm6, %ymm6
	vpand	%ymm0, %ymm3, %ymm9
	vpsrld	$2, %ymm4, %ymm4
	vpsrld	$2, %ymm3, %ymm3
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm2, %ymm2
	vmovdqa	%ymm10, 1696(%rsp)
	vpunpcklqdq	%ymm6, %ymm11, %ymm10
	vpunpckhqdq	%ymm6, %ymm11, %ymm6
	vpunpcklqdq	%ymm3, %ymm9, %ymm11
	vpunpckhqdq	%ymm3, %ymm9, %ymm3
	vpunpcklqdq	%ymm4, %ymm8, %ymm9
	vpunpckhqdq	%ymm4, %ymm8, %ymm4
	vpunpcklqdq	%ymm2, %ymm7, %ymm8
	vpunpckhqdq	%ymm2, %ymm7, %ymm2
	vmovdqa	%ymm2, 2048(%rsp)
	vpand	640(%rsp), %ymm13, %ymm2
	vmovdqa	%ymm4, 1984(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC5(%rip), %ymm2, %ymm2
	vpand	%ymm5, %ymm2, %ymm4
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm5, %ymm2, %ymm2
	vmovdqa	%ymm3, 1920(%rsp)
	vperm2i128	$32, %ymm2, %ymm4, %ymm3
	vperm2i128	$49, %ymm2, %ymm4, %ymm2
	vmovdqa	%ymm6, 1856(%rsp)
	vpsrld	$1, %ymm2, %ymm7
	vpsrld	$1, %ymm3, %ymm6
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm7, %ymm7
	vpand	%ymm1, %ymm3, %ymm3
	vpand	%ymm1, %ymm2, %ymm2
	vpunpckldq	%ymm6, %ymm3, %ymm4
	vpunpckhdq	%ymm6, %ymm3, %ymm3
	vpunpckldq	%ymm7, %ymm2, %ymm6
	vpunpckhdq	%ymm7, %ymm2, %ymm2
	vpand	%ymm0, %ymm2, %ymm7
	vmovdqa	%ymm11, 1888(%rsp)
	vmovdqa	%ymm9, 1952(%rsp)
	vpand	%ymm0, %ymm4, %ymm11
	vpand	%ymm0, %ymm3, %ymm9
	vmovdqa	%ymm8, 2016(%rsp)
	vpsrld	$2, %ymm4, %ymm4
	vpand	%ymm0, %ymm6, %ymm8
	vpsrld	$2, %ymm3, %ymm3
	vpsrld	$2, %ymm6, %ymm6
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm2, %ymm2
	vmovdqa	%ymm10, 1824(%rsp)
	vpunpcklqdq	%ymm4, %ymm11, %ymm10
	vpunpckhqdq	%ymm4, %ymm11, %ymm4
	vpunpcklqdq	%ymm3, %ymm9, %ymm11
	vpunpckhqdq	%ymm3, %ymm9, %ymm3
	vpunpcklqdq	%ymm6, %ymm8, %ymm9
	vpunpckhqdq	%ymm6, %ymm8, %ymm6
	vpunpcklqdq	%ymm2, %ymm7, %ymm8
	vpunpckhqdq	%ymm2, %ymm7, %ymm2
	vmovdqa	%ymm2, 2304(%rsp)
	vpand	608(%rsp), %ymm14, %ymm2
	vmovdqa	%ymm3, 2176(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC5(%rip), %ymm2, %ymm2
	vpand	%ymm5, %ymm2, %ymm3
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm5, %ymm2, %ymm5
	vperm2i128	$32, %ymm5, %ymm3, %ymm2
	vperm2i128	$49, %ymm5, %ymm3, %ymm5
	vmovdqa	%ymm4, 2112(%rsp)
	vpsrld	$1, %ymm5, %ymm3
	vpsrld	$1, %ymm2, %ymm4
	vpand	%ymm1, %ymm4, %ymm4
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm3, %ymm3
	vpand	%ymm1, %ymm5, %ymm1
	vmovdqa	%ymm6, 2240(%rsp)
	vpunpckldq	%ymm4, %ymm2, %ymm6
	vpunpckhdq	%ymm4, %ymm2, %ymm2
	vpunpckldq	%ymm3, %ymm1, %ymm4
	vpunpckhdq	%ymm3, %ymm1, %ymm3
	vmovdqa	%ymm9, 2208(%rsp)
	vmovdqa	%ymm8, 2272(%rsp)
	vpand	%ymm0, %ymm3, %ymm9
	vpand	%ymm0, %ymm6, %ymm8
	vpsrld	$2, %ymm6, %ymm5
	vpsrld	$2, %ymm2, %ymm1
	vpand	%ymm0, %ymm4, %ymm6
	vpsrld	$2, %ymm3, %ymm3
	vpsrld	$2, %ymm4, %ymm4
	vpand	%ymm0, %ymm5, %ymm5
	vpand	%ymm0, %ymm1, %ymm1
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm2, %ymm7
	vpand	%ymm0, %ymm3, %ymm0
	vpunpckhqdq	%ymm1, %ymm7, %ymm2
	vpunpckhqdq	%ymm5, %ymm8, %ymm3
	vmovdqa	%ymm10, 2080(%rsp)
	vpunpcklqdq	%ymm5, %ymm8, %ymm10
	vpunpcklqdq	%ymm1, %ymm7, %ymm8
	vpunpcklqdq	%ymm4, %ymm6, %ymm5
	vpunpckhqdq	%ymm4, %ymm6, %ymm1
	vpunpcklqdq	%ymm0, %ymm9, %ymm4
	vpunpckhqdq	%ymm0, %ymm9, %ymm0
	vmovdqa	%ymm3, 2368(%rsp)
	vmovdqa	%ymm2, 2432(%rsp)
	vmovdqa	%ymm11, 2144(%rsp)
	vmovdqa	%ymm10, 2336(%rsp)
	vmovdqa	%ymm8, 2400(%rsp)
	vmovdqa	%ymm5, 2464(%rsp)
	vmovdqa	%ymm1, 2496(%rsp)
	vmovdqa	%ymm4, 2528(%rsp)
	vmovdqa	%ymm0, 2560(%rsp)
	vmovdqa	.LC9(%rip), %ymm3
	vmovdqa	.LC10(%rip), %ymm2
.L14:
	vmovdqa	1824(%rsp,%rax), %ymm1
	vmovdqa	1056(%rsp,%rax), %ymm4
	vextracti128	$0x1, %ymm1, %xmm5
	vpand	%ymm4, %ymm1, %ymm0
	vpmovsxbw	%xmm0, %ymm7
	vpmullw	%ymm3, %ymm7, %ymm8
	vpmulhw	%ymm3, %ymm7, %ymm7
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovsxbw	%xmm0, %ymm6
	vpmovsxbw	%xmm1, %ymm0
	vpmovsxwd	%xmm0, %ymm1
	vpunpcklwd	%ymm7, %ymm8, %ymm9
	vextracti128	$0x1, %ymm0, %xmm0
	vpunpckhwd	%ymm7, %ymm8, %ymm7
	vperm2i128	$32, %ymm7, %ymm9, %ymm8
	vpmovsxwd	%xmm0, %ymm0
	vperm2i128	$49, %ymm7, %ymm9, %ymm7
	vpaddd	%ymm7, %ymm0, %ymm0
	vpmullw	%ymm3, %ymm6, %ymm7
	vpmulhw	%ymm3, %ymm6, %ymm6
	vpaddd	%ymm8, %ymm1, %ymm1
	vpmovsxbw	%xmm5, %ymm5
	vpand	%ymm0, %ymm2, %ymm0
	vpand	%ymm1, %ymm2, %ymm1
	vpunpcklwd	%ymm6, %ymm7, %ymm8
	vpackusdw	%ymm0, %ymm1, %ymm1
	vpunpckhwd	%ymm6, %ymm7, %ymm6
	vpmovsxwd	%xmm5, %ymm0
	vextracti128	$0x1, %ymm5, %xmm5
	vperm2i128	$32, %ymm6, %ymm8, %ymm7
	vpmovsxwd	%xmm5, %ymm5
	vperm2i128	$49, %ymm6, %ymm8, %ymm6
	vpaddd	%ymm7, %ymm0, %ymm0
	vpaddd	%ymm6, %ymm5, %ymm5
	vpand	%ymm0, %ymm2, %ymm0
	vpand	%ymm5, %ymm2, %ymm5
	vpackusdw	%ymm5, %ymm0, %ymm0
	vpermq	$216, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpand	.LC4(%rip), %ymm1, %ymm1
	vpand	.LC4(%rip), %ymm0, %ymm0
	vpackuswb	%ymm0, %ymm1, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm4, %ymm0, %ymm0
	vmovdqa	%ymm0, 2592(%rsp,%rax)
	addq	$32, %rax
	cmpq	$768, %rax
	jne	.L14
	leaq	3360(%rsp), %rdx
	movq	%rdx, %rcx
	leaq	3328(%rsp), %rax
	addq	$768, %rcx
.L15:
	vmovdqa	(%rax), %ymm0
	addq	$32, %rdx
	vperm2i128	$1, %ymm0, %ymm0, %ymm0
	vpshufb	.LC2(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	subq	$32, %rax
	cmpq	%rcx, %rdx
	jne	.L15
	xorl	%eax, %eax
	leaq	3367(%rsp), %rsi
	movl	$768, %edx
	movl	$0, 4128(%rsp)
	movw	%ax, 4132(%rsp)
	movb	$0, 4134(%rsp)
	vzeroupper
	call	memcpy
	movl	%ebx, %eax
	sarl	$31, %eax
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L16:
	.cfi_restore_state
	movl	$768, %r8d
	xorl	%eax, %eax
	jmp	.L2
	.cfi_endproc
.LFE5186:
	.size	r3_recip, .-r3_recip
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.quad	1
	.quad	0
	.quad	0
	.quad	0
	.align 32
.LC1:
	.quad	4611686018427387904
	.quad	4611686018427387904
	.quad	0
	.quad	0
	.align 32
.LC2:
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
.LC3:
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
.LC4:
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
.LC5:
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
.LC6:
	.quad	1085102592571150095
	.quad	1085102592571150095
	.quad	1085102592571150095
	.quad	1085102592571150095
	.align 32
.LC7:
	.quad	361700864190383365
	.quad	361700864190383365
	.quad	361700864190383365
	.quad	361700864190383365
	.align 32
.LC8:
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.quad	72340172838076673
	.align 32
.LC9:
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.value	-2
	.align 32
.LC10:
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
