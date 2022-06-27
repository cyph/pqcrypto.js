	.file	"r3_recip.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(r3_recip)
	.type	CRYPTO_NAMESPACE(r3_recip), @function
CRYPTO_NAMESPACE(r3_recip):
.LFB5295:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	736(%rsi), %rcx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$4000, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	leaq	3200(%rsp), %rdx
	leaq	3968(%rsp), %rsi
	movq	%rdx, %rax
.L2:
	vmovdqu	(%rcx), %ymm6
	addq	$32, %rax
	subq	$32, %rcx
	vperm2i128	$1, %ymm6, %ymm6, %ymm0
	vpshufb	.LC2(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rsi, %rax
	jne	.L2
	movl	$0, 3968(%rsp)
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	vmovdqa	.LC3(%rip), %ymm3
	movb	$0, 3974(%rsp)
	vmovdqa	.LC4(%rip), %ymm2
	leaq	3207(%rsp), %rsi
	leaq	1664(%rsp), %r8
	movw	%cx, 3972(%rsp)
	leaq	2432(%rsp), %rcx
.L3:
	vmovdqu	(%rsi,%rax), %ymm1
	vpand	%ymm3, %ymm1, %ymm0
	vmovdqa	%ymm0, (%r8,%rax)
	vpmovsxbw	%xmm1, %ymm0
	vextracti128	$0x1, %ymm1, %xmm1
	vpmovsxbw	%xmm1, %ymm1
	vpsraw	$1, %ymm0, %ymm0
	vpsraw	$1, %ymm1, %ymm1
	vpand	%ymm0, %ymm2, %ymm0
	vpand	%ymm1, %ymm2, %ymm1
	vpackuswb	%ymm1, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm3, %ymm0
	vmovdqa	%ymm0, (%rcx,%rax)
	addq	$32, %rax
	cmpq	$768, %rax
	jne	.L3
	vmovdqa	1664(%rsp), %ymm6
	movl	$256, %r10d
	movl	$-1, %ebx
	vpunpckhdq	1696(%rsp), %ymm6, %ymm1
	vmovdqa	1728(%rsp), %ymm4
	vmovdqa	1856(%rsp), %ymm2
	vpunpckhdq	1760(%rsp), %ymm4, %ymm3
	vpslld	$2, %ymm1, %ymm1
	vmovdqa	1792(%rsp), %ymm4
	vpunpckhdq	1888(%rsp), %ymm2, %ymm2
	vpunpckhdq	1824(%rsp), %ymm4, %ymm0
	vpunpckldq	1696(%rsp), %ymm6, %ymm7
	vpslld	$2, %ymm3, %ymm3
	vpunpckldq	1824(%rsp), %ymm4, %ymm5
	vmovdqa	1728(%rsp), %ymm6
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm2, %ymm2
	vpunpckldq	1760(%rsp), %ymm6, %ymm6
	vmovdqa	1856(%rsp), %ymm4
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm5, %ymm0, %ymm0
	vpunpckldq	1888(%rsp), %ymm4, %ymm4
	vpor	%ymm6, %ymm3, %ymm3
	vpor	%ymm4, %ymm2, %ymm2
	vpunpcklqdq	%ymm3, %ymm1, %ymm4
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpunpcklqdq	%ymm2, %ymm0, %ymm3
	vpslld	$1, %ymm1, %ymm1
	vpunpckhqdq	%ymm2, %ymm0, %ymm0
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm4, %ymm1
	vmovdqa	1984(%rsp), %ymm4
	vpunpckhdq	2016(%rsp), %ymm4, %ymm2
	vmovdqa	2048(%rsp), %ymm4
	vpor	%ymm0, %ymm3, %ymm0
	vpunpckldq	2080(%rsp), %ymm4, %ymm5
	vperm2i128	$32, %ymm0, %ymm1, %ymm15
	vpslld	$2, %ymm2, %ymm2
	vperm2i128	$49, %ymm0, %ymm1, %ymm1
	vpslld	$4, %ymm1, %ymm0
	vpor	%ymm0, %ymm15, %ymm15
	vpunpckhdq	2080(%rsp), %ymm4, %ymm0
	vmovdqa	2112(%rsp), %ymm4
	vpshufb	.LC5(%rip), %ymm15, %ymm15
	vpunpckldq	2144(%rsp), %ymm4, %ymm3
	vpunpckhdq	2144(%rsp), %ymm4, %ymm4
	vpermq	$216, %ymm15, %ymm15
	vpslld	$2, %ymm0, %ymm0
	vpshufd	$216, %ymm15, %ymm6
	vpslld	$2, %ymm4, %ymm4
	vpor	%ymm5, %ymm0, %ymm0
	vmovdqa	%ymm6, %ymm15
	vpor	%ymm3, %ymm4, %ymm4
	vmovdqa	1920(%rsp), %ymm6
	vpunpckhdq	1952(%rsp), %ymm6, %ymm1
	vpunpckldq	1952(%rsp), %ymm6, %ymm7
	vmovdqa	1984(%rsp), %ymm6
	vpunpckldq	2016(%rsp), %ymm6, %ymm6
	vpslld	$2, %ymm1, %ymm1
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm6, %ymm2, %ymm2
	vpunpcklqdq	%ymm2, %ymm1, %ymm3
	vpunpckhqdq	%ymm2, %ymm1, %ymm1
	vpunpcklqdq	%ymm4, %ymm0, %ymm2
	vpunpckhqdq	%ymm4, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm3, %ymm1
	vpor	%ymm0, %ymm2, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm14
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm14, %ymm14
	vpshufb	.LC5(%rip), %ymm14, %ymm14
	vpermq	$216, %ymm14, %ymm14
	vpshufd	$216, %ymm14, %ymm6
	vmovdqa	%ymm6, %ymm13
	vmovdqa	2176(%rsp), %ymm6
	vpunpckldq	2208(%rsp), %ymm6, %ymm7
	vpunpckhdq	2208(%rsp), %ymm6, %ymm2
	vmovdqa	2240(%rsp), %ymm6
	vpunpckldq	2272(%rsp), %ymm6, %ymm5
	vpunpckhdq	2272(%rsp), %ymm6, %ymm3
	vmovdqa	2304(%rsp), %ymm6
	vpslld	$2, %ymm2, %ymm2
	vpunpckldq	2336(%rsp), %ymm6, %ymm1
	vpunpckhdq	2336(%rsp), %ymm6, %ymm0
	vpslld	$2, %ymm3, %ymm3
	vpor	%ymm2, %ymm7, %ymm2
	vmovdqa	2368(%rsp), %ymm6
	vpunpckldq	2400(%rsp), %ymm6, %ymm4
	vpor	%ymm3, %ymm5, %ymm3
	vpunpckhdq	2400(%rsp), %ymm6, %ymm6
	vpslld	$2, %ymm0, %ymm0
	vpslld	$2, %ymm6, %ymm6
	vpor	%ymm0, %ymm1, %ymm1
	vpunpcklqdq	%ymm3, %ymm2, %ymm0
	vpor	%ymm6, %ymm4, %ymm4
	vpunpckhqdq	%ymm3, %ymm2, %ymm2
	vpunpcklqdq	%ymm4, %ymm1, %ymm3
	vpslld	$1, %ymm2, %ymm2
	vpunpckhqdq	%ymm4, %ymm1, %ymm1
	vmovdqa	2496(%rsp), %ymm4
	vpslld	$1, %ymm1, %ymm1
	vpor	%ymm2, %ymm0, %ymm0
	vpunpckhdq	2528(%rsp), %ymm4, %ymm2
	vmovdqa	2560(%rsp), %ymm4
	vpunpckldq	2592(%rsp), %ymm4, %ymm5
	vpor	%ymm1, %ymm3, %ymm1
	vperm2i128	$32, %ymm1, %ymm0, %ymm3
	vpslld	$2, %ymm2, %ymm2
	vperm2i128	$49, %ymm1, %ymm0, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm3, %ymm3
	vpunpckhdq	2592(%rsp), %ymm4, %ymm0
	vmovdqa	2624(%rsp), %ymm4
	vpshufb	.LC5(%rip), %ymm3, %ymm3
	vpermq	$216, %ymm3, %ymm3
	vpslld	$2, %ymm0, %ymm0
	vpshufd	$216, %ymm3, %ymm6
	vpor	%ymm5, %ymm0, %ymm0
	vpunpckldq	2656(%rsp), %ymm4, %ymm3
	vpunpckhdq	2656(%rsp), %ymm4, %ymm4
	vmovdqa	%ymm6, %ymm14
	vmovdqa	2432(%rsp), %ymm6
	vpunpckhdq	2464(%rsp), %ymm6, %ymm1
	vpunpckldq	2464(%rsp), %ymm6, %ymm7
	vmovdqa	2496(%rsp), %ymm6
	vpslld	$2, %ymm4, %ymm4
	vpunpckldq	2528(%rsp), %ymm6, %ymm6
	vpslld	$2, %ymm1, %ymm1
	vpor	%ymm3, %ymm4, %ymm4
	vpor	%ymm6, %ymm2, %ymm2
	vpor	%ymm7, %ymm1, %ymm1
	vpunpcklqdq	%ymm2, %ymm1, %ymm3
	vpunpckhqdq	%ymm2, %ymm1, %ymm1
	vpunpcklqdq	%ymm4, %ymm0, %ymm2
	vpunpckhqdq	%ymm4, %ymm0, %ymm0
	vpslld	$1, %ymm1, %ymm1
	vmovdqa	2752(%rsp), %ymm4
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm3, %ymm1
	vpunpckhdq	2784(%rsp), %ymm4, %ymm3
	vmovdqa	2816(%rsp), %ymm4
	vpor	%ymm0, %ymm2, %ymm0
	vperm2i128	$32, %ymm0, %ymm1, %ymm8
	vpslld	$2, %ymm3, %ymm3
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm8, %ymm8
	vpshufb	.LC5(%rip), %ymm8, %ymm8
	vpermq	$216, %ymm8, %ymm8
	vpshufd	$216, %ymm8, %ymm6
	vmovdqa	%ymm6, %ymm12
	vmovdqa	2688(%rsp), %ymm6
	vpunpckldq	2720(%rsp), %ymm6, %ymm7
	vpunpckhdq	2720(%rsp), %ymm6, %ymm1
	vmovdqa	2752(%rsp), %ymm6
	vpunpckldq	2784(%rsp), %ymm6, %ymm6
	vpunpckldq	2848(%rsp), %ymm4, %ymm5
	vpunpckhdq	2848(%rsp), %ymm4, %ymm0
	vpslld	$2, %ymm1, %ymm1
	vmovdqa	2880(%rsp), %ymm4
	vpunpckldq	2912(%rsp), %ymm4, %ymm2
	vpor	%ymm6, %ymm3, %ymm3
	vpor	%ymm7, %ymm1, %ymm1
	vpunpckhdq	2912(%rsp), %ymm4, %ymm4
	vpslld	$2, %ymm0, %ymm0
	vpor	%ymm5, %ymm0, %ymm0
	vpslld	$2, %ymm4, %ymm4
	vpor	%ymm2, %ymm4, %ymm4
	vpunpcklqdq	%ymm3, %ymm1, %ymm2
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpunpcklqdq	%ymm4, %ymm0, %ymm3
	vpslld	$1, %ymm1, %ymm1
	vpunpckhqdq	%ymm4, %ymm0, %ymm0
	vmovdqa	3072(%rsp), %ymm4
	vpslld	$1, %ymm0, %ymm0
	vpor	%ymm1, %ymm2, %ymm1
	vpunpckhdq	3104(%rsp), %ymm4, %ymm4
	vpor	%ymm0, %ymm3, %ymm0
	vmovdqa	3136(%rsp), %ymm3
	vperm2i128	$32, %ymm0, %ymm1, %ymm7
	vperm2i128	$49, %ymm0, %ymm1, %ymm0
	vpunpckldq	3168(%rsp), %ymm3, %ymm1
	vpunpckhdq	3168(%rsp), %ymm3, %ymm3
	vpslld	$4, %ymm0, %ymm0
	vpor	%ymm0, %ymm7, %ymm7
	vpshufb	.LC5(%rip), %ymm7, %ymm7
	vpermq	$216, %ymm7, %ymm7
	vpshufd	$216, %ymm7, %ymm6
	vmovdqa	%ymm6, %ymm11
	vmovdqa	2944(%rsp), %ymm6
	vpunpckhdq	2976(%rsp), %ymm6, %ymm0
	vpunpckldq	2976(%rsp), %ymm6, %ymm2
	vmovdqa	3008(%rsp), %ymm6
	vpunpckhdq	3040(%rsp), %ymm6, %ymm7
	vpslld	$2, %ymm0, %ymm0
	vpunpckldq	3040(%rsp), %ymm6, %ymm5
	vmovdqa	3072(%rsp), %ymm6
	vpor	%ymm0, %ymm2, %ymm2
	vpunpckldq	3104(%rsp), %ymm6, %ymm6
	vpslld	$2, %ymm7, %ymm0
	vmovdqa	.LC0(%rip), %ymm7
	vpor	%ymm0, %ymm5, %ymm5
	vpslld	$2, %ymm4, %ymm0
	vmovdqa	%ymm7, 640(%rsp)
	vpor	%ymm0, %ymm6, %ymm4
	vpslld	$2, %ymm3, %ymm0
	vmovdqa	%ymm7, 576(%rsp)
	vpor	%ymm0, %ymm1, %ymm1
	vpunpcklqdq	%ymm5, %ymm2, %ymm0
	vpunpckhqdq	%ymm5, %ymm2, %ymm2
	vpunpcklqdq	%ymm1, %ymm4, %ymm3
	vpunpckhqdq	%ymm1, %ymm4, %ymm1
	vmovdqa	.LC1(%rip), %ymm4
	vpxor	%xmm5, %xmm5, %xmm5
	vpslld	$1, %ymm2, %ymm2
	vpslld	$1, %ymm1, %ymm1
	vmovdqa	%ymm5, 512(%rsp)
	vmovdqa	%ymm4, 256(%rsp)
	vpor	%ymm2, %ymm0, %ymm0
	vpor	%ymm1, %ymm3, %ymm2
	vmovdqa	%ymm5, 608(%rsp)
	vperm2i128	$32, %ymm2, %ymm0, %ymm1
	vperm2i128	$49, %ymm2, %ymm0, %ymm0
	vmovdqa	%ymm5, 544(%rsp)
	vpslld	$4, %ymm0, %ymm0
	vmovdqa	%ymm4, 480(%rsp)
	vpor	%ymm0, %ymm1, %ymm0
	vmovdqa	%ymm12, %ymm1
	vpshufb	.LC5(%rip), %ymm0, %ymm0
	vmovdqa	%ymm5, 416(%rsp)
	vpermq	$216, %ymm0, %ymm0
	vpshufd	$216, %ymm0, %ymm6
	vmovdqa	%ymm6, %ymm10
	vmovdqa	%ymm5, %ymm6
.L4:
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm5, %ymm5
	movl	%ebx, %r11d
	vmovdqa	480(%rsp), %ymm2
	vmovq	%xmm6, %rax
	sarl	$31, %r11d
	addq	%rax, %rax
	vpxor	%ymm2, %ymm14, %ymm9
	vmovq	%rax, %xmm0
	vmovq	%xmm5, %rax
	addq	%rax, %rax
	vpblendd	$3, %ymm0, %ymm6, %ymm6
	vmovdqa	%ymm6, 352(%rsp)
	vmovq	%rax, %xmm0
	vmovd	%xmm15, %eax
	vmovdqa	576(%rsp), %ymm6
	andl	$1, %eax
	vpblendd	$3, %ymm0, %ymm5, %ymm7
	vmovdqa	416(%rsp), %ymm0
	negl	%eax
	vpxor	%ymm6, %ymm15, %ymm4
	vmovd	%xmm6, %r9d
	vmovdqa	%ymm7, 320(%rsp)
	vmovdqa	544(%rsp), %ymm7
	andl	%eax, %r11d
	andl	$1, %r9d
	vmovd	%r11d, %xmm3
	negl	%r9d
	vpbroadcastd	%xmm3, %ymm3
	vmovd	%xmm7, %r12d
	andl	%eax, %r9d
	vpand	%ymm3, %ymm4, %ymm4
	vmovd	%xmm1, %eax
	vpand	%ymm3, %ymm9, %ymm9
	andl	$1, %r12d
	vpxor	%ymm6, %ymm4, %ymm5
	vpxor	%ymm0, %ymm13, %ymm6
	andl	$1, %eax
	negl	%r12d
	vpand	%ymm3, %ymm6, %ymm6
	vpxor	%ymm2, %ymm9, %ymm2
	vpxor	%ymm4, %ymm15, %ymm15
	negl	%eax
	vpxor	%ymm0, %ymm6, %ymm0
	vpxor	%ymm6, %ymm13, %ymm6
	vpxor	%ymm9, %ymm14, %ymm9
	xorl	%r12d, %eax
	vmovdqa	%ymm0, 416(%rsp)
	vpxor	%ymm7, %ymm1, %ymm0
	andl	%r9d, %eax
	movl	%ebx, %r12d
	vmovdqa	%ymm2, 480(%rsp)
	vpand	%ymm3, %ymm0, %ymm0
	vmovd	%r9d, %xmm2
	negl	%r12d
	vpxor	%ymm7, %ymm0, %ymm12
	vpxor	%ymm0, %ymm1, %ymm1
	vpbroadcastd	%xmm2, %ymm2
	xorl	%ebx, %r12d
	vmovdqa	%ymm12, 544(%rsp)
	vmovdqa	%ymm12, %ymm7
	andl	%r11d, %r12d
	vmovdqa	608(%rsp), %ymm12
	vmovdqa	%ymm5, 576(%rsp)
	xorl	%r12d, %ebx
	vpxor	%ymm12, %ymm11, %ymm8
	decl	%ebx
	vpand	%ymm3, %ymm8, %ymm8
	vpxor	%ymm12, %ymm8, %ymm12
	vpxor	%ymm8, %ymm11, %ymm8
	vmovdqa	256(%rsp), %ymm11
	vmovdqa	%ymm12, 608(%rsp)
	vpand	%ymm5, %ymm2, %ymm12
	vpxor	%ymm11, %ymm10, %ymm0
	vpand	%ymm3, %ymm0, %ymm0
	vpxor	%ymm11, %ymm0, %ymm11
	vpxor	%ymm0, %ymm10, %ymm10
	vmovd	%eax, %xmm0
	vmovdqa	%ymm10, 448(%rsp)
	vpbroadcastd	%xmm0, %ymm0
	vpand	480(%rsp), %ymm2, %ymm10
	vpxor	608(%rsp), %ymm0, %ymm4
	vmovdqa	%ymm11, 256(%rsp)
	vpxor	%ymm7, %ymm0, %ymm5
	vpxor	256(%rsp), %ymm0, %ymm13
	vpand	416(%rsp), %ymm2, %ymm11
	vpxor	%ymm12, %ymm15, %ymm7
	vpand	%ymm5, %ymm12, %ymm5
	vpand	%ymm13, %ymm10, %ymm15
	vpand	%ymm4, %ymm11, %ymm4
	vpxor	%ymm11, %ymm6, %ymm6
	vmovdqa	%ymm15, %ymm13
	vpxor	%ymm10, %ymm9, %ymm15
	vpxor	%ymm8, %ymm4, %ymm14
	vmovdqa	448(%rsp), %ymm9
	vpor	%ymm6, %ymm14, %ymm14
	vpxor	%ymm6, %ymm4, %ymm4
	vpxor	%ymm8, %ymm11, %ymm8
	vmovdqa	%ymm15, 672(%rsp)
	vpxor	%ymm1, %ymm5, %ymm15
	vmovq	%xmm14, %rax
	vpxor	%ymm13, %ymm9, %ymm9
	vmovdqa	%ymm13, 384(%rsp)
	vpor	%ymm7, %ymm15, %ymm15
	salq	$63, %rax
	vpxor	%ymm7, %ymm5, %ymm5
	vpxor	%ymm1, %ymm12, %ymm1
	vmovq	%xmm14, %r12
	vpand	%ymm4, %ymm8, %ymm8
	vpand	%ymm5, %ymm1, %ymm1
	vmovaps	%xmm15, 288(%rsp)
	movq	288(%rsp), %r9
	shrq	%r12
	vpor	672(%rsp), %ymm9, %ymm9
	vpxor	448(%rsp), %ymm10, %ymm10
	vmovdqa	672(%rsp), %ymm4
	vpxor	384(%rsp), %ymm4, %ymm4
	shrq	%r9
	vmovdqa	320(%rsp), %ymm7
	vmovdqa	352(%rsp), %ymm6
	orq	%r9, %rax
	vmovq	%xmm1, %r9
	vmovdqa	640(%rsp), %ymm12
	vmovq	%rax, %xmm13
	vmovq	%xmm9, %rax
	shrq	%r9
	vpand	%ymm4, %ymm10, %ymm10
	salq	$63, %rax
	vpblendd	$3, %ymm13, %ymm15, %ymm15
	orq	%r12, %rax
	vmovq	%xmm8, %r12
	vpermq	$57, %ymm15, %ymm15
	vmovq	%rax, %xmm13
	vmovq	%xmm9, %rax
	shrq	%r12
	shrq	%rax
	vpblendd	$3, %ymm13, %ymm14, %ymm14
	vmovq	%rax, %xmm13
	vmovq	%xmm8, %rax
	salq	$63, %rax
	vpblendd	$3, %ymm13, %ymm9, %ymm9
	vpermq	$57, %ymm14, %ymm13
	orq	%r9, %rax
	vpermq	$57, %ymm9, %ymm14
	vmovq	%rax, %xmm4
	vmovq	%xmm10, %rax
	salq	$63, %rax
	vpblendd	$3, %ymm4, %ymm1, %ymm1
	orq	%r12, %rax
	vpermq	$57, %ymm1, %ymm1
	vmovq	%rax, %xmm4
	vmovq	%xmm10, %rax
	vpblendd	$3, %ymm4, %ymm8, %ymm8
	shrq	%rax
	vpermq	$57, %ymm8, %ymm11
	vmovq	%rax, %xmm4
	vmovdqa	512(%rsp), %ymm8
	vpblendd	$3, %ymm4, %ymm10, %ymm10
	vpxor	%ymm12, %ymm6, %ymm4
	vpxor	%ymm8, %ymm7, %ymm5
	vpand	%ymm3, %ymm4, %ymm4
	vpermq	$57, %ymm10, %ymm10
	vpand	%ymm3, %ymm5, %ymm3
	vpxor	%ymm6, %ymm4, %ymm6
	vpxor	%ymm12, %ymm4, %ymm4
	vpxor	%ymm7, %ymm3, %ymm5
	vpand	%ymm2, %ymm6, %ymm2
	vpxor	%ymm8, %ymm3, %ymm3
	vpxor	%ymm0, %ymm5, %ymm0
	vpxor	%ymm2, %ymm4, %ymm4
	vpand	%ymm0, %ymm2, %ymm0
	vpxor	%ymm2, %ymm3, %ymm2
	vpxor	%ymm0, %ymm3, %ymm7
	vpxor	%ymm4, %ymm0, %ymm0
	vpor	%ymm4, %ymm7, %ymm7
	vmovdqa	%ymm7, 640(%rsp)
	vpand	%ymm0, %ymm2, %ymm7
	vmovdqa	%ymm7, 512(%rsp)
	decl	%r10d
	jne	.L4
	vpxor	%xmm7, %xmm7, %xmm7
	vmovdqa	%ymm11, %ymm0
	vmovdqa	%ymm15, %ymm3
	vmovdqa	%ymm14, 320(%rsp)
	vmovdqa	%ymm5, %ymm12
	movl	$256, %r10d
	vmovdqa	%ymm13, 352(%rsp)
	vmovdqa	%ymm7, %ymm5
	vmovdqa	%ymm1, %ymm11
	vmovdqa	%ymm0, %ymm15
	vmovdqa	%ymm10, %ymm9
	vmovdqa	%ymm7, 384(%rsp)
	vmovdqa	%ymm7, 672(%rsp)
	vmovdqa	%ymm3, %ymm14
	.p2align 4,,10
	.p2align 3
.L5:
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm12, %ymm12
	vmovdqa	416(%rsp), %ymm2
	vmovq	%xmm6, %rax
	vmovq	%xmm5, %r9
	vpermq	$147, %ymm7, %ymm7
	vmovdqa	544(%rsp), %ymm3
	addq	%r9, %r9
	leaq	(%rax,%rax), %r11
	shrq	$63, %rax
	orq	%r9, %rax
	vmovq	%r11, %xmm0
	vmovq	%xmm7, %r9
	vpblendd	$3, %ymm0, %ymm6, %ymm6
	vmovq	%rax, %xmm0
	vmovq	%xmm12, %rax
	addq	%r9, %r9
	leaq	(%rax,%rax), %r11
	shrq	$63, %rax
	vpblendd	$3, %ymm0, %ymm5, %ymm5
	vmovdqa	%ymm6, 192(%rsp)
	orq	%r9, %rax
	vmovd	%xmm14, %r9d
	vmovq	%r11, %xmm0
	movl	%ebx, %r11d
	andl	$1, %r9d
	vpblendd	$3, %ymm0, %ymm12, %ymm1
	sarl	$31, %r11d
	vmovd	%xmm3, %r12d
	vmovq	%rax, %xmm0
	movl	%r9d, %eax
	andl	$1, %r12d
	vmovdqa	%ymm1, 128(%rsp)
	vpblendd	$3, %ymm0, %ymm7, %ymm4
	negl	%eax
	vpxor	%ymm3, %ymm11, %ymm0
	vmovdqa	352(%rsp), %ymm12
	andl	%eax, %r11d
	vmovdqa	%ymm4, 96(%rsp)
	negl	%r12d
	vmovdqa	576(%rsp), %ymm4
	vmovd	%r11d, %xmm1
	vpxor	%ymm2, %ymm12, %ymm6
	vmovdqa	%ymm5, 160(%rsp)
	vpbroadcastd	%xmm1, %ymm1
	vpxor	%ymm4, %ymm14, %ymm7
	vmovd	%xmm4, %r9d
	vpand	%ymm1, %ymm7, %ymm7
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm0, %ymm0
	andl	$1, %r9d
	vpxor	%ymm4, %ymm7, %ymm13
	vpxor	%ymm2, %ymm6, %ymm8
	vpxor	%ymm3, %ymm0, %ymm3
	negl	%r9d
	vmovdqa	608(%rsp), %ymm4
	andl	%eax, %r9d
	vmovd	%xmm11, %eax
	vmovdqa	480(%rsp), %ymm2
	vpxor	%ymm11, %ymm0, %ymm11
	vpxor	%ymm7, %ymm14, %ymm14
	vpxor	%ymm12, %ymm6, %ymm6
	andl	$1, %eax
	vpxor	320(%rsp), %ymm2, %ymm5
	vpxor	%ymm4, %ymm15, %ymm10
	negl	%eax
	vmovdqa	%ymm11, 448(%rsp)
	vmovdqa	%ymm3, 544(%rsp)
	vpand	%ymm1, %ymm10, %ymm10
	xorl	%r12d, %eax
	movl	%ebx, %r12d
	vpxor	%ymm10, %ymm15, %ymm15
	vpxor	%ymm4, %ymm10, %ymm4
	vpand	%ymm1, %ymm5, %ymm5
	andl	%r9d, %eax
	vmovdqa	256(%rsp), %ymm10
	vpxor	%ymm2, %ymm5, %ymm2
	negl	%r12d
	vmovdqa	%ymm4, 608(%rsp)
	vmovdqa	%ymm2, 480(%rsp)
	vmovd	%eax, %xmm2
	xorl	%ebx, %r12d
	vpxor	320(%rsp), %ymm5, %ymm5
	vpxor	%ymm10, %ymm9, %ymm0
	vpbroadcastd	%xmm2, %ymm2
	andl	%r11d, %r12d
	vmovdqa	%ymm13, 576(%rsp)
	vpand	%ymm1, %ymm0, %ymm0
	vpxor	%ymm3, %ymm2, %ymm4
	xorl	%r12d, %ebx
	vmovdqa	%ymm2, 224(%rsp)
	vpxor	%ymm10, %ymm0, %ymm10
	vpxor	%ymm0, %ymm9, %ymm9
	decl	%ebx
	vmovdqa	%ymm8, 416(%rsp)
	vpxor	608(%rsp), %ymm2, %ymm3
	vmovd	%r9d, %xmm0
	vmovdqa	%ymm10, 256(%rsp)
	vpxor	256(%rsp), %ymm2, %ymm2
	vmovdqa	%ymm9, 288(%rsp)
	vpbroadcastd	%xmm0, %ymm0
	vpand	480(%rsp), %ymm0, %ymm9
	vpand	%ymm13, %ymm0, %ymm11
	vpand	%ymm8, %ymm0, %ymm10
	vpxor	%ymm11, %ymm14, %ymm7
	vpand	%ymm4, %ymm11, %ymm4
	vpand	%ymm3, %ymm10, %ymm3
	vpxor	%ymm10, %ymm6, %ymm6
	vpxor	%ymm15, %ymm3, %ymm13
	vpand	%ymm2, %ymm9, %ymm2
	vpor	%ymm6, %ymm13, %ymm13
	vpxor	%ymm9, %ymm5, %ymm5
	vpxor	%ymm6, %ymm3, %ymm3
	vmovq	%xmm13, %rax
	vmovq	%xmm13, %r12
	vpxor	%ymm15, %ymm10, %ymm10
	vpxor	448(%rsp), %ymm4, %ymm14
	salq	$63, %rax
	vpxor	%ymm7, %ymm4, %ymm4
	vpand	%ymm3, %ymm10, %ymm10
	vpxor	288(%rsp), %ymm2, %ymm8
	shrq	%r12
	vpxor	%ymm5, %ymm2, %ymm2
	vpxor	448(%rsp), %ymm11, %ymm11
	vpor	%ymm7, %ymm14, %ymm14
	vmovaps	%xmm14, 352(%rsp)
	movq	352(%rsp), %r9
	vpor	%ymm5, %ymm8, %ymm8
	vpand	%ymm4, %ymm11, %ymm11
	shrq	%r9
	orq	%r9, %rax
	vmovq	%xmm11, %r9
	vmovq	%rax, %xmm12
	vmovq	%xmm8, %rax
	shrq	%r9
	salq	$63, %rax
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	orq	%r12, %rax
	vmovq	%xmm10, %r12
	vpermq	$57, %ymm14, %ymm14
	vmovq	%rax, %xmm12
	vmovq	%xmm8, %rax
	shrq	%rax
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	shrq	%r12
	vmovq	%rax, %xmm12
	vpermq	$57, %ymm13, %ymm13
	vmovq	%xmm10, %rax
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	salq	$63, %rax
	vmovdqa	%ymm13, 352(%rsp)
	vpermq	$57, %ymm8, %ymm8
	orq	%r9, %rax
	vmovdqa	%ymm8, 320(%rsp)
	vmovq	%rax, %xmm4
	vpxor	288(%rsp), %ymm9, %ymm9
	vmovdqa	160(%rsp), %ymm7
	vmovdqa	512(%rsp), %ymm13
	vpblendd	$3, %ymm4, %ymm11, %ymm11
	vmovdqa	192(%rsp), %ymm6
	vpand	%ymm2, %ymm9, %ymm9
	vpermq	$57, %ymm11, %ymm11
	vpxor	640(%rsp), %ymm6, %ymm4
	vmovq	%xmm9, %rax
	salq	$63, %rax
	vpand	%ymm1, %ymm4, %ymm4
	orq	%r12, %rax
	vpxor	%ymm6, %ymm4, %ymm6
	vmovq	%rax, %xmm15
	vmovq	%xmm9, %rax
	shrq	%rax
	vpblendd	$3, %ymm15, %ymm10, %ymm10
	vmovq	%rax, %xmm2
	vpermq	$57, %ymm10, %ymm15
	vpblendd	$3, %ymm2, %ymm9, %ymm9
	vpxor	672(%rsp), %ymm7, %ymm2
	vpermq	$57, %ymm9, %ymm9
	vpand	%ymm1, %ymm2, %ymm2
	vpxor	%ymm7, %ymm2, %ymm5
	vmovdqa	128(%rsp), %ymm7
	vpxor	672(%rsp), %ymm2, %ymm2
	vpxor	%ymm13, %ymm7, %ymm8
	vpand	%ymm1, %ymm8, %ymm8
	vpxor	%ymm7, %ymm8, %ymm12
	vpxor	%ymm13, %ymm8, %ymm8
	vmovdqa	96(%rsp), %ymm7
	vmovdqa	384(%rsp), %ymm13
	vpxor	224(%rsp), %ymm12, %ymm10
	vpxor	%ymm13, %ymm7, %ymm3
	vpand	%ymm1, %ymm3, %ymm1
	vpxor	640(%rsp), %ymm4, %ymm3
	vpxor	%ymm7, %ymm1, %ymm7
	vpxor	%ymm13, %ymm1, %ymm1
	vpand	%ymm0, %ymm6, %ymm13
	vpand	%ymm10, %ymm13, %ymm10
	vpxor	%ymm13, %ymm3, %ymm3
	vpand	%ymm0, %ymm5, %ymm0
	vpxor	%ymm8, %ymm10, %ymm4
	vpxor	%ymm8, %ymm13, %ymm8
	vpxor	%ymm0, %ymm2, %ymm2
	vpor	%ymm3, %ymm4, %ymm4
	vpxor	%ymm3, %ymm10, %ymm3
	vmovdqa	%ymm4, 640(%rsp)
	vpand	%ymm3, %ymm8, %ymm3
	vmovdqa	%ymm3, 512(%rsp)
	vpxor	224(%rsp), %ymm7, %ymm3
	vpand	%ymm3, %ymm0, %ymm3
	vpxor	%ymm1, %ymm3, %ymm4
	vpxor	%ymm1, %ymm0, %ymm1
	vpor	%ymm2, %ymm4, %ymm4
	vpxor	%ymm2, %ymm3, %ymm2
	vmovdqa	%ymm4, 672(%rsp)
	vpand	%ymm2, %ymm1, %ymm1
	vmovdqa	%ymm1, 384(%rsp)
	decl	%r10d
	jne	.L5
	vpxor	%xmm3, %xmm3, %xmm3
	vmovdqa	%ymm9, 64(%rsp)
	movl	$497, %r10d
	vmovdqa	%ymm11, 192(%rsp)
	vmovdqa	%ymm3, %ymm4
	vmovdqa	%ymm15, 160(%rsp)
	vmovdqa	%ymm14, 96(%rsp)
	vmovdqa	%ymm12, 288(%rsp)
	vmovdqa	%ymm3, 224(%rsp)
	vmovdqa	%ymm3, 448(%rsp)
	.p2align 4,,10
	.p2align 3
.L6:
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm4, %ymm4
	vmovdqa	192(%rsp), %ymm2
	vmovq	%xmm6, %r9
	vmovq	%xmm5, %rax
	vpermq	$147, 288(%rsp), %ymm15
	vmovdqa	%ymm6, 704(%rsp)
	leaq	(%r9,%r9), %r12
	vpermq	$147, %ymm7, %ymm7
	shrq	$63, %r9
	vpermq	$147, %ymm3, %ymm3
	movq	%r12, 704(%rsp)
	vmovq	%xmm4, %r11
	leaq	(%rax,%rax), %r12
	shrq	$63, %rax
	vmovdqa	%ymm5, 736(%rsp)
	orq	%r12, %r9
	addq	%r11, %r11
	vmovdqa	608(%rsp), %ymm9
	orq	%r11, %rax
	vmovq	%xmm3, %r11
	movq	%r9, 736(%rsp)
	vmovq	%xmm15, %r9
	vmovq	%rax, %xmm0
	leaq	(%r9,%r9), %r12
	vmovq	%xmm7, %rax
	addq	%r11, %r11
	vmovdqa	%ymm15, 800(%rsp)
	vpblendd	$3, %ymm0, %ymm4, %ymm6
	shrq	$63, %r9
	vmovdqa	544(%rsp), %ymm4
	vmovdqa	352(%rsp), %ymm12
	vmovdqa	%ymm6, 32(%rsp)
	vmovdqa	416(%rsp), %ymm5
	movq	%r12, 800(%rsp)
	leaq	(%rax,%rax), %r12
	shrq	$63, %rax
	vpxor	%ymm2, %ymm4, %ymm11
	orq	%r11, %rax
	orq	%r12, %r9
	vmovd	%xmm4, %r12d
	movl	%ebx, %r11d
	vmovq	%rax, %xmm0
	sarl	$31, %r11d
	vpxor	%ymm5, %ymm12, %ymm6
	andl	$1, %r12d
	vpblendd	$3, %ymm0, %ymm3, %ymm14
	vmovdqa	96(%rsp), %ymm3
	negl	%r12d
	vmovdqa	%ymm7, 832(%rsp)
	vmovdqa	576(%rsp), %ymm7
	vmovdqa	%ymm14, (%rsp)
	vmovdqa	480(%rsp), %ymm0
	movq	%r9, 832(%rsp)
	vmovd	%xmm3, %r9d
	andl	$1, %r9d
	vpxor	%ymm3, %ymm7, %ymm14
	movl	%r9d, %eax
	vmovd	%xmm7, %r9d
	negl	%eax
	andl	$1, %r9d
	andl	%eax, %r11d
	negl	%r9d
	vmovd	%r11d, %xmm1
	andl	%eax, %r9d
	vmovd	%xmm2, %eax
	vpbroadcastd	%xmm1, %ymm1
	andl	$1, %eax
	vpand	%ymm1, %ymm11, %ymm11
	vpand	%ymm1, %ymm14, %ymm14
	vpand	%ymm1, %ymm6, %ymm6
	negl	%eax
	vpxor	%ymm2, %ymm11, %ymm15
	vpxor	%ymm4, %ymm11, %ymm4
	vpxor	%ymm7, %ymm14, %ymm13
	xorl	%r12d, %eax
	vpxor	%ymm5, %ymm6, %ymm8
	andl	%r9d, %eax
	vpxor	%ymm3, %ymm14, %ymm14
	movl	%ebx, %r12d
	vmovdqa	160(%rsp), %ymm11
	vpxor	%ymm12, %ymm6, %ymm6
	negl	%r12d
	vmovdqa	256(%rsp), %ymm7
	vmovdqa	320(%rsp), %ymm5
	xorl	%ebx, %r12d
	vmovdqa	%ymm15, 288(%rsp)
	vmovd	%eax, %xmm15
	vpxor	%ymm9, %ymm11, %ymm10
	vpbroadcastd	%xmm15, %ymm15
	andl	%r11d, %r12d
	vmovdqa	%ymm4, 544(%rsp)
	vpand	%ymm1, %ymm10, %ymm10
	vpxor	%ymm0, %ymm5, %ymm5
	vpxor	%ymm4, %ymm15, %ymm4
	xorl	%r12d, %ebx
	vpxor	%ymm9, %ymm10, %ymm2
	vpxor	%ymm11, %ymm10, %ymm11
	vmovdqa	64(%rsp), %ymm10
	decl	%ebx
	vpand	%ymm1, %ymm5, %ymm5
	vpxor	%ymm2, %ymm15, %ymm3
	vmovdqa	%ymm11, 192(%rsp)
	vpxor	%ymm7, %ymm10, %ymm9
	vpxor	%ymm0, %ymm5, %ymm0
	vmovdqa	%ymm13, 576(%rsp)
	vmovdqa	%ymm0, 480(%rsp)
	vpand	%ymm1, %ymm9, %ymm9
	vmovdqa	%ymm8, 416(%rsp)
	vpxor	%ymm7, %ymm9, %ymm7
	vpxor	%ymm10, %ymm9, %ymm9
	vmovd	%r9d, %xmm10
	vpbroadcastd	%xmm10, %ymm10
	vmovdqa	%ymm7, %ymm0
	vmovdqa	%ymm7, 256(%rsp)
	vmovdqa	%ymm2, 608(%rsp)
	vpand	%ymm10, %ymm13, %ymm11
	vpxor	%ymm0, %ymm15, %ymm2
	vpxor	%ymm11, %ymm14, %ymm7
	vmovdqa	%ymm10, %ymm14
	vpand	%ymm4, %ymm11, %ymm4
	vmovdqa	%ymm9, 352(%rsp)
	vmovdqa	%ymm14, 128(%rsp)
	vpand	%ymm10, %ymm8, %ymm10
	vpand	480(%rsp), %ymm14, %ymm9
	vpxor	288(%rsp), %ymm4, %ymm14
	vpand	%ymm3, %ymm10, %ymm3
	vpxor	%ymm10, %ymm6, %ymm6
	vpxor	192(%rsp), %ymm3, %ymm13
	vmovdqa	352(%rsp), %ymm0
	vpor	%ymm7, %ymm14, %ymm14
	vpand	%ymm2, %ymm9, %ymm2
	vpxor	320(%rsp), %ymm5, %ymm5
	vpor	%ymm6, %ymm13, %ymm13
	vpxor	%ymm0, %ymm2, %ymm8
	vmovaps	%xmm14, 320(%rsp)
	movq	320(%rsp), %r9
	vmovq	%xmm13, %rax
	vpxor	%ymm9, %ymm5, %ymm5
	vmovq	%xmm13, %r12
	shrq	%r9
	salq	$63, %rax
	vpor	%ymm5, %ymm8, %ymm8
	vpxor	%ymm7, %ymm4, %ymm4
	orq	%r9, %rax
	shrq	%r12
	vpxor	%ymm6, %ymm3, %ymm3
	vpxor	%ymm5, %ymm2, %ymm2
	vmovq	%rax, %xmm12
	vmovq	%xmm8, %rax
	vpxor	%ymm0, %ymm9, %ymm9
	vmovdqa	32(%rsp), %ymm7
	salq	$63, %rax
	vpblendd	$3, %ymm12, %ymm14, %ymm14
	vpand	%ymm2, %ymm9, %ymm9
	vpxor	288(%rsp), %ymm11, %ymm11
	orq	%r12, %rax
	vpermq	$57, %ymm14, %ymm14
	vpxor	192(%rsp), %ymm10, %ymm10
	vmovq	%rax, %xmm12
	vmovq	%xmm8, %rax
	vmovdqa	%ymm14, 96(%rsp)
	vpand	%ymm4, %ymm11, %ymm11
	shrq	%rax
	vpblendd	$3, %ymm12, %ymm13, %ymm13
	vpand	%ymm3, %ymm10, %ymm10
	vmovq	%rax, %xmm12
	vmovq	%xmm11, %r9
	vmovq	%xmm10, %rax
	shrq	%r9
	vmovq	%xmm10, %r12
	vpblendd	$3, %ymm12, %ymm8, %ymm8
	salq	$63, %rax
	shrq	%r12
	vpermq	$57, %ymm13, %ymm14
	vmovdqa	%ymm14, 352(%rsp)
	orq	%r9, %rax
	vpermq	$57, %ymm8, %ymm14
	vmovdqa	%ymm14, 320(%rsp)
	vmovq	%rax, %xmm4
	vmovq	%xmm9, %rax
	vmovdqa	(%rsp), %ymm14
	salq	$63, %rax
	vpblendd	$3, %ymm4, %ymm11, %ymm11
	orq	%r12, %rax
	vpermq	$57, %ymm11, %ymm5
	vmovdqa	%ymm5, 192(%rsp)
	vmovq	%rax, %xmm3
	vmovq	%xmm9, %rax
	shrq	%rax
	vpblendd	$3, %ymm3, %ymm10, %ymm10
	vmovdqa	384(%rsp), %ymm3
	vpxor	832(%rsp), %ymm3, %ymm11
	vmovq	%rax, %xmm2
	vpermq	$57, %ymm10, %ymm5
	vpblendd	$3, %ymm2, %ymm9, %ymm9
	vpand	%ymm1, %ymm11, %ymm11
	vpxor	448(%rsp), %ymm7, %ymm2
	vmovdqa	%ymm5, 160(%rsp)
	vpermq	$57, %ymm9, %ymm5
	vmovdqa	640(%rsp), %ymm9
	vpxor	704(%rsp), %ymm9, %ymm10
	vpand	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm5, 64(%rsp)
	vmovdqa	672(%rsp), %ymm5
	vpxor	736(%rsp), %ymm5, %ymm8
	vpxor	%ymm7, %ymm2, %ymm4
	vpand	%ymm1, %ymm10, %ymm10
	vmovdqa	512(%rsp), %ymm7
	vpxor	800(%rsp), %ymm7, %ymm12
	vpand	%ymm1, %ymm8, %ymm8
	vpxor	%ymm9, %ymm10, %ymm9
	vpxor	704(%rsp), %ymm10, %ymm6
	vpxor	736(%rsp), %ymm8, %ymm5
	vpand	%ymm1, %ymm12, %ymm12
	vpxor	800(%rsp), %ymm12, %ymm13
	vpxor	%ymm7, %ymm12, %ymm12
	vpxor	832(%rsp), %ymm11, %ymm7
	vpxor	%ymm3, %ymm11, %ymm11
	vpxor	224(%rsp), %ymm14, %ymm3
	vmovdqa	%ymm13, 288(%rsp)
	vpxor	%ymm13, %ymm15, %ymm13
	vpand	%ymm1, %ymm3, %ymm1
	vpxor	%ymm14, %ymm1, %ymm3
	vpxor	224(%rsp), %ymm1, %ymm1
	vpand	128(%rsp), %ymm6, %ymm14
	vpxor	672(%rsp), %ymm8, %ymm8
	vpxor	448(%rsp), %ymm2, %ymm2
	vpand	%ymm13, %ymm14, %ymm13
	vpxor	%ymm14, %ymm9, %ymm9
	vpxor	%ymm12, %ymm13, %ymm10
	vpxor	%ymm12, %ymm14, %ymm12
	vmovdqa	128(%rsp), %ymm14
	vpor	%ymm9, %ymm10, %ymm0
	vpxor	%ymm9, %ymm13, %ymm9
	vmovdqa	%ymm0, 640(%rsp)
	vpand	%ymm14, %ymm5, %ymm10
	vpand	%ymm9, %ymm12, %ymm0
	vpxor	%ymm15, %ymm7, %ymm9
	vpand	%ymm9, %ymm10, %ymm9
	vpxor	%ymm10, %ymm8, %ymm8
	vmovdqa	%ymm0, 512(%rsp)
	vpxor	%ymm11, %ymm9, %ymm12
	vpxor	%ymm11, %ymm10, %ymm11
	vpor	%ymm8, %ymm12, %ymm0
	vpxor	%ymm8, %ymm9, %ymm8
	vmovdqa	%ymm0, 672(%rsp)
	vpand	%ymm8, %ymm11, %ymm0
	vpxor	%ymm15, %ymm3, %ymm8
	vmovdqa	%ymm0, 384(%rsp)
	vpand	%ymm14, %ymm4, %ymm0
	vpand	%ymm8, %ymm0, %ymm8
	vpxor	%ymm0, %ymm2, %ymm2
	vpxor	%ymm1, %ymm8, %ymm9
	vpxor	%ymm1, %ymm0, %ymm1
	vpor	%ymm2, %ymm9, %ymm15
	vpxor	%ymm2, %ymm8, %ymm2
	vmovdqa	%ymm15, 448(%rsp)
	vpand	%ymm2, %ymm1, %ymm1
	vmovdqa	%ymm1, 224(%rsp)
	decl	%r10d
	jne	.L6
	vmovdqa	96(%rsp), %ymm14
	movl	$256, %r11d
	.p2align 4,,10
	.p2align 3
.L7:
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm4, %ymm4
	vmovdqa	544(%rsp), %ymm1
	vmovq	%xmm6, %r9
	vmovq	%xmm5, %rax
	vmovq	%xmm4, %r10
	vmovdqa	%ymm6, 704(%rsp)
	leaq	(%r9,%r9), %r12
	addq	%r10, %r10
	shrq	$63, %r9
	vpermq	$147, %ymm7, %ymm7
	movq	%r12, 704(%rsp)
	leaq	(%rax,%rax), %r12
	shrq	$63, %rax
	vpermq	$147, %ymm3, %ymm3
	vmovdqa	%ymm5, 736(%rsp)
	orq	%r10, %rax
	orq	%r12, %r9
	vmovq	%xmm3, %r10
	vmovq	%rax, %xmm0
	movq	%r9, 736(%rsp)
	vmovq	%xmm7, %rax
	addq	%r10, %r10
	vmovdqa	%ymm7, 832(%rsp)
	vpblendd	$3, %ymm0, %ymm4, %ymm4
	vpermq	$147, 288(%rsp), %ymm0
	vmovdqa	576(%rsp), %ymm5
	vmovdqa	192(%rsp), %ymm2
	vmovdqa	352(%rsp), %ymm15
	vmovq	%xmm0, %r9
	vpxor	%ymm5, %ymm14, %ymm7
	vmovdqa	%ymm0, 800(%rsp)
	leaq	(%r9,%r9), %r12
	shrq	$63, %r9
	vpxor	%ymm2, %ymm1, %ymm9
	movq	%r12, 800(%rsp)
	leaq	(%rax,%rax), %r12
	shrq	$63, %rax
	orq	%r12, %r9
	orq	%r10, %rax
	vmovd	%xmm1, %r12d
	movl	%ebx, %r10d
	movq	%r9, 832(%rsp)
	vmovd	%xmm14, %r9d
	sarl	$31, %r10d
	andl	$1, %r12d
	andl	$1, %r9d
	vmovq	%rax, %xmm0
	negl	%r12d
	movl	%r9d, %eax
	vpblendd	$3, %ymm0, %ymm3, %ymm13
	vmovd	%xmm5, %r9d
	vmovdqa	416(%rsp), %ymm0
	vmovdqa	%ymm13, 480(%rsp)
	negl	%eax
	andl	$1, %r9d
	andl	%eax, %r10d
	negl	%r9d
	vpxor	%ymm15, %ymm0, %ymm6
	vmovd	%r10d, %xmm3
	andl	%eax, %r9d
	vmovd	%xmm2, %eax
	vpbroadcastd	%xmm3, %ymm3
	andl	$1, %eax
	vpand	%ymm3, %ymm9, %ymm9
	vpand	%ymm3, %ymm6, %ymm6
	vpand	%ymm3, %ymm7, %ymm7
	negl	%eax
	vpxor	%ymm1, %ymm9, %ymm10
	vpxor	%ymm2, %ymm9, %ymm9
	vpxor	%ymm0, %ymm6, %ymm12
	xorl	%r12d, %eax
	andl	%r9d, %eax
	vpxor	%ymm5, %ymm7, %ymm5
	vpxor	%ymm15, %ymm6, %ymm6
	movl	%ebx, %r12d
	vmovdqa	160(%rsp), %ymm2
	vmovd	%eax, %xmm0
	vpxor	%ymm14, %ymm7, %ymm7
	vmovdqa	608(%rsp), %ymm1
	vpbroadcastd	%xmm0, %ymm0
	negl	%r12d
	vmovdqa	%ymm5, 576(%rsp)
	vmovdqa	%ymm10, 544(%rsp)
	vpxor	%ymm2, %ymm1, %ymm8
	xorl	%ebx, %r12d
	vmovdqa	%ymm12, 416(%rsp)
	vpand	%ymm3, %ymm8, %ymm8
	andl	%r10d, %r12d
	vpxor	%ymm1, %ymm8, %ymm1
	vpxor	%ymm2, %ymm8, %ymm8
	xorl	%r12d, %ebx
	vmovdqa	%ymm1, 608(%rsp)
	vmovdqa	%ymm1, %ymm13
	decl	%ebx
	vmovd	%r9d, %xmm1
	vpbroadcastd	%xmm1, %ymm1
	vpxor	%ymm13, %ymm0, %ymm2
	vpand	%ymm5, %ymm1, %ymm11
	vpxor	%ymm10, %ymm0, %ymm5
	vpand	%ymm12, %ymm1, %ymm10
	vpand	%ymm5, %ymm11, %ymm5
	vpand	%ymm2, %ymm10, %ymm2
	vpxor	%ymm11, %ymm7, %ymm7
	vpxor	%ymm10, %ymm6, %ymm6
	vpxor	%ymm9, %ymm5, %ymm14
	vpxor	%ymm8, %ymm2, %ymm12
	vpor	%ymm6, %ymm12, %ymm12
	vpor	%ymm7, %ymm14, %ymm14
	vpxor	%ymm7, %ymm5, %ymm5
	vmovq	%xmm14, %r9
	vmovq	%xmm12, %r10
	vpxor	%ymm6, %ymm2, %ymm2
	salq	$63, %r10
	shrq	%r9
	vpxor	%ymm9, %ymm11, %ymm9
	vpxor	%ymm8, %ymm10, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpand	%ymm5, %ymm9, %ymm9
	vmovq	%xmm12, %rax
	orq	%r10, %r9
	vmovq	%r9, %xmm13
	vmovq	%xmm8, %r10
	shrq	%rax
	vmovq	%xmm9, %r9
	shrq	%r9
	salq	$63, %r10
	vpblendd	$3, %ymm13, %ymm14, %ymm14
	vmovq	%rax, %xmm13
	orq	%r10, %r9
	vmovq	%xmm8, %rax
	vpblendd	$3, %ymm13, %ymm12, %ymm12
	vmovdqa	672(%rsp), %ymm5
	vmovq	%r9, %xmm11
	shrq	%rax
	vpermq	$57, %ymm12, %ymm15
	vpermq	$57, %ymm14, %ymm14
	vpblendd	$3, %ymm11, %ymm9, %ymm9
	vmovq	%rax, %xmm10
	vmovdqa	%ymm15, 352(%rsp)
	vpxor	448(%rsp), %ymm4, %ymm2
	vpblendd	$3, %ymm10, %ymm8, %ymm8
	vpermq	$57, %ymm9, %ymm6
	vmovdqa	512(%rsp), %ymm11
	vmovdqa	%ymm6, 192(%rsp)
	vpermq	$57, %ymm8, %ymm6
	vpxor	736(%rsp), %ymm5, %ymm8
	vpand	%ymm3, %ymm2, %ymm2
	vmovdqa	%ymm6, 160(%rsp)
	vpxor	%ymm4, %ymm2, %ymm4
	vmovdqa	640(%rsp), %ymm6
	vpxor	704(%rsp), %ymm6, %ymm15
	vpand	%ymm3, %ymm8, %ymm8
	vpxor	736(%rsp), %ymm8, %ymm5
	vpand	%ymm3, %ymm15, %ymm15
	vpxor	704(%rsp), %ymm15, %ymm6
	vpxor	800(%rsp), %ymm11, %ymm12
	vmovdqa	384(%rsp), %ymm9
	vmovdqa	480(%rsp), %ymm13
	vpxor	640(%rsp), %ymm15, %ymm15
	vpand	%ymm3, %ymm12, %ymm12
	vpxor	800(%rsp), %ymm12, %ymm10
	vpxor	672(%rsp), %ymm8, %ymm8
	vpxor	%ymm11, %ymm12, %ymm12
	vpxor	832(%rsp), %ymm9, %ymm11
	vmovdqa	%ymm10, 288(%rsp)
	vpxor	448(%rsp), %ymm2, %ymm2
	vpand	%ymm3, %ymm11, %ymm11
	vpxor	832(%rsp), %ymm11, %ymm7
	vpxor	%ymm9, %ymm11, %ymm11
	vmovdqa	224(%rsp), %ymm9
	vpxor	%ymm9, %ymm13, %ymm10
	vpand	%ymm3, %ymm10, %ymm10
	vpxor	%ymm13, %ymm10, %ymm3
	vpxor	%ymm9, %ymm10, %ymm10
	vpand	%ymm1, %ymm6, %ymm9
	vpxor	288(%rsp), %ymm0, %ymm13
	vpxor	%ymm9, %ymm15, %ymm15
	vmovdqa	%ymm15, 512(%rsp)
	vpand	%ymm13, %ymm9, %ymm13
	vpxor	%ymm12, %ymm13, %ymm15
	vpxor	%ymm12, %ymm9, %ymm12
	vpxor	512(%rsp), %ymm13, %ymm9
	vpor	512(%rsp), %ymm15, %ymm15
	vpand	%ymm9, %ymm12, %ymm12
	vpxor	%ymm0, %ymm7, %ymm9
	vpxor	%ymm0, %ymm3, %ymm0
	vmovdqa	%ymm12, 512(%rsp)
	vpand	%ymm1, %ymm5, %ymm12
	vpand	%ymm1, %ymm4, %ymm1
	vpand	%ymm9, %ymm12, %ymm9
	vpxor	%ymm12, %ymm8, %ymm8
	vpand	%ymm0, %ymm1, %ymm0
	vmovdqa	%ymm15, 640(%rsp)
	vpxor	%ymm11, %ymm9, %ymm13
	vpxor	%ymm1, %ymm2, %ymm2
	vpxor	%ymm11, %ymm12, %ymm11
	vpor	%ymm8, %ymm13, %ymm13
	vpxor	%ymm8, %ymm9, %ymm8
	vmovdqa	%ymm13, 672(%rsp)
	vpand	%ymm8, %ymm11, %ymm11
	vpxor	%ymm10, %ymm0, %ymm8
	vpxor	%ymm10, %ymm1, %ymm10
	vpxor	%ymm2, %ymm0, %ymm0
	vpor	%ymm2, %ymm8, %ymm15
	vmovdqa	%ymm11, 384(%rsp)
	vmovdqa	%ymm15, 448(%rsp)
	vpand	%ymm0, %ymm10, %ymm1
	vmovdqa	%ymm1, 224(%rsp)
	decl	%r11d
	jne	.L7
	vmovdqa	%ymm14, 96(%rsp)
	movl	$256, %r10d
	vmovdqa	%ymm1, %ymm14
	vmovdqa	%ymm11, %ymm13
	vmovdqa	640(%rsp), %ymm15
.L8:
	vpermq	$147, %ymm6, %ymm6
	vpermq	$147, %ymm5, %ymm5
	vpermq	$147, %ymm4, %ymm4
	vmovdqa	576(%rsp), %ymm1
	vmovq	%xmm6, %r9
	vmovq	%xmm5, %rax
	vmovq	%xmm4, %r11
	vmovdqa	%ymm6, 704(%rsp)
	leaq	(%r9,%r9), %r12
	addq	%r11, %r11
	shrq	$63, %r9
	vpermq	$147, %ymm7, %ymm7
	movq	%r12, 704(%rsp)
	leaq	(%rax,%rax), %r12
	shrq	$63, %rax
	vpermq	$147, %ymm3, %ymm3
	vmovdqa	%ymm5, 736(%rsp)
	orq	%r11, %rax
	orq	%r12, %r9
	vmovq	%xmm3, %r11
	vmovq	%rax, %xmm0
	movq	%r9, 736(%rsp)
	vmovq	%xmm7, %rax
	addq	%r11, %r11
	vmovdqa	%ymm7, 832(%rsp)
	vpblendd	$3, %ymm0, %ymm4, %ymm4
	vpermq	$147, 288(%rsp), %ymm0
	vmovdqa	96(%rsp), %ymm7
	vmovdqa	544(%rsp), %ymm11
	vmovdqa	192(%rsp), %ymm5
	vmovq	%xmm0, %r9
	vpxor	%ymm7, %ymm1, %ymm6
	vmovdqa	%ymm0, 800(%rsp)
	leaq	(%r9,%r9), %r12
	shrq	$63, %r9
	movq	%r12, 800(%rsp)
	leaq	(%rax,%rax), %r12
	shrq	$63, %rax
	orq	%r11, %rax
	orq	%r12, %r9
	vmovd	%xmm1, %r11d
	vmovq	%rax, %xmm0
	vmovd	%xmm7, %eax
	vmovd	%xmm11, %r12d
	andl	$1, %r11d
	andl	$1, %eax
	negl	%r11d
	vpblendd	$3, %ymm0, %ymm3, %ymm3
	andl	$1, %r12d
	movq	%r9, 832(%rsp)
	movl	%ebx, %r9d
	negl	%eax
	negl	%r12d
	sarl	$31, %r9d
	andl	%eax, %r11d
	andl	%eax, %r9d
	vmovd	%xmm5, %eax
	vmovd	%r9d, %xmm10
	andl	$1, %eax
	vpbroadcastd	%xmm10, %ymm10
	negl	%eax
	vpand	%ymm10, %ymm6, %ymm6
	xorl	%r12d, %eax
	movl	%ebx, %r12d
	vpxor	%ymm1, %ymm6, %ymm2
	vmovdqa	%ymm5, %ymm1
	vpxor	%ymm11, %ymm5, %ymm5
	andl	%r11d, %eax
	vpand	%ymm10, %ymm5, %ymm5
	vmovd	%eax, %xmm0
	vpxor	%ymm7, %ymm6, %ymm6
	negl	%r12d
	vpxor	%ymm11, %ymm5, %ymm11
	vpxor	%ymm1, %ymm5, %ymm5
	vmovd	%r11d, %xmm1
	xorl	%ebx, %r12d
	vpbroadcastd	%xmm1, %ymm1
	vpbroadcastd	%xmm0, %ymm0
	andl	%r9d, %r12d
	vmovdqa	%ymm2, 576(%rsp)
	vmovdqa	%ymm11, 544(%rsp)
	vpand	%ymm2, %ymm1, %ymm8
	xorl	%r12d, %ebx
	vpxor	%ymm11, %ymm0, %ymm2
	vpand	%ymm2, %ymm8, %ymm2
	vpxor	%ymm8, %ymm6, %ymm6
	decl	%ebx
	vmovdqa	512(%rsp), %ymm11
	vpxor	800(%rsp), %ymm11, %ymm12
	vpxor	%ymm2, %ymm5, %ymm7
	vpxor	%ymm6, %ymm2, %ymm2
	vpor	%ymm6, %ymm7, %ymm7
	vpxor	%ymm8, %ymm5, %ymm5
	vmovq	%xmm7, %rax
	vpand	%ymm2, %ymm5, %ymm5
	vpand	%ymm10, %ymm12, %ymm12
	shrq	%rax
	vmovq	%rax, %xmm9
	vmovq	%xmm5, %rax
	vpblendd	$3, %ymm9, %ymm7, %ymm7
	shrq	%rax
	vpxor	704(%rsp), %ymm15, %ymm9
	vpermq	$57, %ymm7, %ymm7
	vmovq	%rax, %xmm2
	vpblendd	$3, %ymm2, %ymm5, %ymm5
	vmovdqa	%ymm7, 96(%rsp)
	vpand	%ymm10, %ymm9, %ymm9
	vmovdqa	672(%rsp), %ymm7
	vpxor	736(%rsp), %ymm7, %ymm8
	vpermq	$57, %ymm5, %ymm6
	vpxor	800(%rsp), %ymm12, %ymm7
	vpxor	448(%rsp), %ymm4, %ymm2
	vpxor	%ymm11, %ymm12, %ymm12
	vpxor	832(%rsp), %ymm13, %ymm11
	vmovdqa	%ymm6, 192(%rsp)
	vmovdqa	%ymm7, 288(%rsp)
	vpand	%ymm10, %ymm8, %ymm8
	vpxor	704(%rsp), %ymm9, %ymm6
	vpxor	736(%rsp), %ymm8, %ymm5
	vpand	%ymm10, %ymm11, %ymm11
	vpand	%ymm10, %ymm2, %ymm2
	vpxor	%ymm9, %ymm15, %ymm9
	vmovdqa	%ymm7, 800(%rsp)
	vpxor	832(%rsp), %ymm11, %ymm7
	vpxor	%ymm11, %ymm13, %ymm11
	vpxor	%ymm3, %ymm14, %ymm13
	vmovdqa	%ymm6, 704(%rsp)
	vpand	%ymm10, %ymm13, %ymm10
	vpxor	%ymm4, %ymm2, %ymm4
	vmovdqa	%ymm5, 736(%rsp)
	vmovdqa	%ymm7, 832(%rsp)
	vpxor	%ymm3, %ymm10, %ymm3
	vpxor	288(%rsp), %ymm0, %ymm13
	vpxor	%ymm10, %ymm14, %ymm10
	vpxor	672(%rsp), %ymm8, %ymm8
	vpand	%ymm1, %ymm6, %ymm14
	vpxor	448(%rsp), %ymm2, %ymm2
	vpand	%ymm13, %ymm14, %ymm13
	vpxor	%ymm14, %ymm9, %ymm9
	vpxor	%ymm12, %ymm13, %ymm15
	vpxor	%ymm12, %ymm14, %ymm12
	vpor	%ymm9, %ymm15, %ymm15
	vpxor	%ymm9, %ymm13, %ymm9
	vpand	%ymm9, %ymm12, %ymm9
	vpand	%ymm1, %ymm5, %ymm12
	vpand	%ymm1, %ymm4, %ymm1
	vmovdqa	%ymm9, 512(%rsp)
	vpxor	%ymm0, %ymm7, %ymm9
	vpxor	%ymm12, %ymm8, %ymm8
	vpxor	%ymm0, %ymm3, %ymm0
	vpand	%ymm9, %ymm12, %ymm9
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	%ymm1, %ymm2, %ymm2
	vpxor	%ymm11, %ymm9, %ymm13
	vpxor	%ymm11, %ymm12, %ymm11
	vpxor	%ymm10, %ymm1, %ymm1
	vpor	%ymm8, %ymm13, %ymm14
	vpxor	%ymm8, %ymm9, %ymm8
	vmovdqa	%ymm14, 672(%rsp)
	vpand	%ymm8, %ymm11, %ymm13
	vpxor	%ymm10, %ymm0, %ymm8
	vpxor	%ymm2, %ymm0, %ymm0
	vpor	%ymm2, %ymm8, %ymm14
	vmovdqa	%ymm14, 448(%rsp)
	vpand	%ymm0, %ymm1, %ymm14
	decl	%r10d
	jne	.L8
	vmovdqa	576(%rsp), %xmm0
	leaq	896(%rsp), %r9
	vmovd	%xmm0, %eax
	vmovdqa	544(%rsp), %xmm0
	andl	$1, %eax
	negl	%eax
	vmovd	%eax, %xmm1
	vmovd	%xmm0, %eax
	andl	$1, %eax
	vpbroadcastd	%xmm1, %ymm1
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm5, %ymm5
	vpand	%ymm1, %ymm4, %ymm4
	negl	%eax
	vmovd	%eax, %xmm0
	xorl	%eax, %eax
	vpbroadcastd	%xmm0, %ymm0
	vpxor	288(%rsp), %ymm0, %ymm15
	vpxor	%ymm0, %ymm3, %ymm1
	vpxor	%ymm0, %ymm7, %ymm7
	vpshufd	$216, %ymm6, %ymm0
	vmovdqa	%ymm7, 672(%rsp)
	vpermq	$216, %ymm0, %ymm0
	vmovdqa	.LC6(%rip), %ymm7
	vpshufb	.LC5(%rip), %ymm0, %ymm0
	vmovdqa	%ymm1, 640(%rsp)
	vmovdqa	.LC7(%rip), %ymm1
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
	vmovdqa	.LC8(%rip), %ymm0
	vpand	%ymm0, %ymm9, %ymm12
	vpand	%ymm0, %ymm3, %ymm13
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
	vpunpcklqdq	%ymm9, %ymm12, %ymm14
	vpunpckhqdq	%ymm9, %ymm12, %ymm9
	vpunpcklqdq	%ymm3, %ymm13, %ymm12
	vmovdqa	%ymm9, 928(%rsp)
	vpunpckhqdq	%ymm3, %ymm13, %ymm3
	vpunpcklqdq	%ymm8, %ymm11, %ymm13
	vpunpckhqdq	%ymm8, %ymm11, %ymm8
	vmovdqa	%ymm8, 1056(%rsp)
	vpunpcklqdq	%ymm2, %ymm10, %ymm11
	vpunpckhqdq	%ymm2, %ymm10, %ymm2
	vmovdqa	%ymm2, 1120(%rsp)
	vpshufd	$216, %ymm5, %ymm2
	vmovdqa	%ymm3, 992(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC5(%rip), %ymm2, %ymm2
	vmovdqa	%ymm12, 960(%rsp)
	vmovdqa	%ymm13, 1024(%rsp)
	vpand	%ymm7, %ymm2, %ymm8
	vpsrld	$4, %ymm2, %ymm2
	vmovdqa	%ymm11, 1088(%rsp)
	vpand	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm14, 896(%rsp)
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
	vpand	%ymm0, %ymm9, %ymm12
	vpand	%ymm0, %ymm3, %ymm13
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
	vpunpcklqdq	%ymm9, %ymm12, %ymm14
	vpunpckhqdq	%ymm9, %ymm12, %ymm9
	vmovdqa	%ymm9, 1184(%rsp)
	vpunpcklqdq	%ymm3, %ymm13, %ymm12
	vpunpckhqdq	%ymm3, %ymm13, %ymm3
	vpunpcklqdq	%ymm8, %ymm11, %ymm13
	vmovdqa	%ymm3, 1248(%rsp)
	vpunpckhqdq	%ymm8, %ymm11, %ymm8
	vpunpcklqdq	%ymm2, %ymm10, %ymm11
	vpunpckhqdq	%ymm2, %ymm10, %ymm2
	vmovdqa	%ymm2, 1376(%rsp)
	vpshufd	$216, %ymm4, %ymm2
	vmovdqa	%ymm8, 1312(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC5(%rip), %ymm2, %ymm2
	vmovdqa	%ymm12, 1216(%rsp)
	vmovdqa	%ymm13, 1280(%rsp)
	vpand	%ymm7, %ymm2, %ymm8
	vpsrld	$4, %ymm2, %ymm2
	vmovdqa	%ymm11, 1344(%rsp)
	vpand	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm14, 1152(%rsp)
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
	vpand	%ymm0, %ymm9, %ymm12
	vpand	%ymm0, %ymm3, %ymm13
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
	vpunpcklqdq	%ymm9, %ymm12, %ymm14
	vpunpckhqdq	%ymm9, %ymm12, %ymm9
	vmovdqa	%ymm9, 1440(%rsp)
	vpunpcklqdq	%ymm3, %ymm13, %ymm12
	vpunpckhqdq	%ymm3, %ymm13, %ymm3
	vpunpcklqdq	%ymm8, %ymm11, %ymm13
	vmovdqa	%ymm12, 1472(%rsp)
	vpunpckhqdq	%ymm8, %ymm11, %ymm8
	vpunpcklqdq	%ymm2, %ymm10, %ymm11
	vpunpckhqdq	%ymm2, %ymm10, %ymm2
	vmovdqa	%ymm3, 1504(%rsp)
	vmovdqa	%ymm13, 1536(%rsp)
	vmovdqa	%ymm14, 1408(%rsp)
	vmovdqa	%ymm8, 1568(%rsp)
	vmovdqa	%ymm2, 1632(%rsp)
	vpand	%ymm15, %ymm6, %ymm2
	vmovdqa	%ymm11, 1600(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC5(%rip), %ymm2, %ymm2
	vpand	%ymm7, %ymm2, %ymm6
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm7, %ymm2, %ymm2
	vperm2i128	$32, %ymm2, %ymm6, %ymm3
	vperm2i128	$49, %ymm2, %ymm6, %ymm2
	vpsrld	$1, %ymm2, %ymm9
	vpsrld	$1, %ymm3, %ymm6
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm6, %ymm6
	vpand	%ymm1, %ymm9, %ymm9
	vpand	%ymm1, %ymm3, %ymm3
	vpunpckldq	%ymm6, %ymm3, %ymm8
	vpunpckhdq	%ymm6, %ymm3, %ymm3
	vpunpckldq	%ymm9, %ymm2, %ymm6
	vpunpckhdq	%ymm9, %ymm2, %ymm2
	vpand	%ymm0, %ymm8, %ymm13
	vpand	%ymm0, %ymm3, %ymm11
	vpand	%ymm0, %ymm6, %ymm10
	vpsrld	$2, %ymm8, %ymm8
	vpand	%ymm0, %ymm2, %ymm9
	vpsrld	$2, %ymm3, %ymm3
	vpsrld	$2, %ymm6, %ymm6
	vpand	%ymm0, %ymm8, %ymm8
	vpsrld	$2, %ymm2, %ymm2
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm2, %ymm2
	vpunpcklqdq	%ymm8, %ymm13, %ymm12
	vpunpckhqdq	%ymm8, %ymm13, %ymm8
	vmovdqa	%ymm12, 1664(%rsp)
	vpunpcklqdq	%ymm3, %ymm11, %ymm13
	vpunpckhqdq	%ymm3, %ymm11, %ymm3
	vpunpcklqdq	%ymm6, %ymm10, %ymm11
	vmovdqa	%ymm3, 1760(%rsp)
	vpunpckhqdq	%ymm6, %ymm10, %ymm6
	vpunpcklqdq	%ymm2, %ymm9, %ymm10
	vpunpckhqdq	%ymm2, %ymm9, %ymm2
	vmovdqa	%ymm2, 1888(%rsp)
	vpand	672(%rsp), %ymm5, %ymm2
	vmovdqa	%ymm6, 1824(%rsp)
	vmovdqa	%ymm10, 1856(%rsp)
	vpshufd	$216, %ymm2, %ymm2
	vmovdqa	%ymm8, 1696(%rsp)
	vpermq	$216, %ymm2, %ymm2
	vpshufb	.LC5(%rip), %ymm2, %ymm2
	vmovdqa	%ymm11, 1792(%rsp)
	vmovdqa	%ymm13, 1728(%rsp)
	vpand	%ymm7, %ymm2, %ymm3
	vpsrld	$4, %ymm2, %ymm2
	vpand	%ymm7, %ymm2, %ymm2
	vperm2i128	$32, %ymm2, %ymm3, %ymm5
	vperm2i128	$49, %ymm2, %ymm3, %ymm2
	vpsrld	$1, %ymm5, %ymm3
	vpsrld	$1, %ymm2, %ymm8
	vpand	%ymm1, %ymm2, %ymm2
	vpand	%ymm1, %ymm3, %ymm9
	vpand	%ymm1, %ymm5, %ymm3
	vpand	%ymm1, %ymm8, %ymm8
	vpunpckldq	%ymm9, %ymm3, %ymm6
	vpunpckhdq	%ymm9, %ymm3, %ymm3
	vpunpckldq	%ymm8, %ymm2, %ymm5
	vpand	%ymm0, %ymm6, %ymm10
	vpand	%ymm0, %ymm3, %ymm9
	vpunpckhdq	%ymm8, %ymm2, %ymm2
	vpsrld	$2, %ymm6, %ymm6
	vpsrld	$2, %ymm3, %ymm3
	vpand	%ymm0, %ymm5, %ymm11
	vpand	%ymm0, %ymm6, %ymm6
	vpand	%ymm0, %ymm3, %ymm3
	vpand	%ymm0, %ymm2, %ymm8
	vpsrld	$2, %ymm5, %ymm5
	vpsrld	$2, %ymm2, %ymm2
	vpunpcklqdq	%ymm6, %ymm10, %ymm12
	vpunpckhqdq	%ymm6, %ymm10, %ymm6
	vpunpcklqdq	%ymm3, %ymm9, %ymm10
	vpunpckhqdq	%ymm3, %ymm9, %ymm3
	vmovdqa	%ymm12, 1920(%rsp)
	vmovdqa	%ymm3, 2016(%rsp)
	vpand	%ymm0, %ymm5, %ymm5
	vpand	640(%rsp), %ymm4, %ymm3
	vpand	%ymm0, %ymm2, %ymm2
	vmovdqa	%ymm6, 1952(%rsp)
	vpunpcklqdq	%ymm5, %ymm11, %ymm9
	vpunpckhqdq	%ymm5, %ymm11, %ymm5
	vpunpcklqdq	%ymm2, %ymm8, %ymm11
	vpshufd	$216, %ymm3, %ymm3
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vmovdqa	%ymm5, 2080(%rsp)
	vmovdqa	%ymm2, 2144(%rsp)
	vpermq	$216, %ymm3, %ymm3
	vpshufb	.LC5(%rip), %ymm3, %ymm3
	vmovdqa	%ymm9, 2048(%rsp)
	vmovdqa	%ymm10, 1984(%rsp)
	vpand	%ymm7, %ymm3, %ymm2
	vpsrld	$4, %ymm3, %ymm3
	vmovdqa	%ymm11, 2112(%rsp)
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
	vpunpckhdq	%ymm6, %ymm1, %ymm1
	vpand	%ymm0, %ymm5, %ymm7
	vpand	%ymm0, %ymm4, %ymm9
	vpand	%ymm0, %ymm1, %ymm6
	vpsrld	$2, %ymm3, %ymm2
	vpand	%ymm0, %ymm3, %ymm8
	vpsrld	$2, %ymm1, %ymm1
	vpsrld	$2, %ymm5, %ymm5
	vpand	%ymm0, %ymm2, %ymm2
	vpsrld	$2, %ymm4, %ymm4
	vpand	%ymm0, %ymm5, %ymm5
	vpand	%ymm0, %ymm4, %ymm4
	vpand	%ymm0, %ymm1, %ymm0
	vpunpcklqdq	%ymm5, %ymm7, %ymm10
	vpunpckhqdq	%ymm5, %ymm7, %ymm3
	vpunpckhqdq	%ymm4, %ymm9, %ymm1
	vpunpcklqdq	%ymm2, %ymm8, %ymm7
	vmovdqa	%ymm10, 2176(%rsp)
	vpunpcklqdq	%ymm4, %ymm9, %ymm5
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vpunpcklqdq	%ymm0, %ymm6, %ymm4
	vmovdqa	%ymm1, 2336(%rsp)
	vpunpckhqdq	%ymm0, %ymm6, %ymm0
	vpxor	%xmm1, %xmm1, %xmm1
	vmovdqa	%ymm3, 2208(%rsp)
	vmovdqa	%ymm7, 2240(%rsp)
	vmovdqa	%ymm2, 2272(%rsp)
	vmovdqa	%ymm5, 2304(%rsp)
	vmovdqa	%ymm4, 2368(%rsp)
	vmovdqa	%ymm0, 2400(%rsp)
.L9:
	vmovdqa	(%r8,%rax), %ymm6
	vpand	(%r9,%rax), %ymm6, %ymm0
	vpsubb	%ymm0, %ymm1, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm6, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	(%r9,%rax), %ymm0, %ymm0
	vmovdqa	%ymm0, (%rcx,%rax)
	addq	$32, %rax
	cmpq	$768, %rax
	jne	.L9
	leaq	3168(%rsp), %rax
.L10:
	vmovdqa	(%rax), %ymm6
	movq	%rax, %r8
	addq	$32, %rdx
	subq	$32, %rax
	vperm2i128	$1, %ymm6, %ymm6, %ymm0
	vpshufb	.LC2(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%r8, %rcx
	jne	.L10
	xorl	%eax, %eax
	movl	$768, %edx
	movl	$0, 3968(%rsp)
	movw	%ax, 3972(%rsp)
	movb	$0, 3974(%rsp)
	vzeroupper
	call	memcpy@PLT
	leaq	-16(%rbp), %rsp
	movl	%ebx, %eax
	sarl	$31, %eax
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5295:
	.size	CRYPTO_NAMESPACE(r3_recip), .-CRYPTO_NAMESPACE(r3_recip)
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
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
