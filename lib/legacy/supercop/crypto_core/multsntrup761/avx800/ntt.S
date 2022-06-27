	.file	"ntt.c"
	.text
	.p2align 4,,15
	.type	ntt512, @function
ntt512:
.LFB4755:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	movl	%esi, %r9d
	movslq	%esi, %r8
	pushq	-8(%r10)
	sall	$9, %r9d
	pushq	%rbp
	movslq	%r9d, %r9
	addq	%r9, %r9
	.cfi_escape 0x10,0x6,0x2,0x76,0
	movq	%rsp, %rbp
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x78,0x6
	movq	%r9, %r10
	negq	%r10
	testq	%r8, %r8
	jle	.L2
	movq	%rdi, %rax
	xorl	%ecx, %ecx
	.p2align 4,,10
	.p2align 3
.L3:
	vmovdqu	(%rax), %ymm1
	vmovdqu	768(%rax), %ymm2
	addq	$1, %rcx
	addq	$1024, %rax
	vmovdqu	-512(%rax), %ymm0
	vmovdqu	-768(%rax), %ymm5
	vmovdqu	-896(%rax), %ymm3
	vmovdqu	-640(%rax), %ymm4
	vpaddw	%ymm0, %ymm1, %ymm11
	vpsubw	%ymm0, %ymm1, %ymm0
	vmovdqa	3264(%rdx), %ymm12
	vpaddw	%ymm2, %ymm5, %ymm1
	vpsubw	%ymm2, %ymm5, %ymm5
	vmovdqu	-384(%rax), %ymm2
	vpmulhw	%ymm12, %ymm5, %ymm8
	vpaddw	%ymm2, %ymm3, %ymm9
	vpsubw	%ymm2, %ymm3, %ymm2
	vmovdqu	-128(%rax), %ymm3
	vpaddw	%ymm4, %ymm3, %ymm6
	vpsubw	%ymm3, %ymm4, %ymm4
	vpaddw	%ymm1, %ymm11, %ymm3
	vpsubw	%ymm1, %ymm11, %ymm11
	vmovdqa	3104(%rdx), %ymm1
	vpaddw	%ymm6, %ymm9, %ymm7
	vpsubw	%ymm6, %ymm9, %ymm9
	vmovdqa	1536(%rdx), %ymm6
	vpmullw	%ymm1, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm8, %ymm5
	vpmulhw	%ymm12, %ymm4, %ymm8
	vpaddw	%ymm5, %ymm0, %ymm10
	vpsubw	%ymm5, %ymm0, %ymm0
	vpmullw	%ymm4, %ymm1, %ymm5
	vpmullw	%ymm9, %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm5, %ymm8, %ymm4
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm4, %ymm2, %ymm5
	vpsubw	%ymm4, %ymm2, %ymm2
	vpmulhrsw	%ymm8, %ymm7, %ymm4
	vpmullw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm3, %ymm4, %ymm7
	vpsubw	%ymm4, %ymm3, %ymm3
	vpmulhw	%ymm12, %ymm9, %ymm4
	vpmulhrsw	%ymm8, %ymm7, %ymm8
	vpsubw	%ymm1, %ymm4, %ymm1
	vpmulhw	3328(%rdx), %ymm5, %ymm4
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmullw	%ymm6, %ymm8, %ymm8
	vpaddw	%ymm11, %ymm1, %ymm9
	vpsubw	%ymm1, %ymm11, %ymm1
	vpsubw	%ymm5, %ymm4, %ymm4
	vpsubw	%ymm8, %ymm7, %ymm7
	vpmulhw	512(%rdx), %ymm3, %ymm8
	vpaddw	%ymm10, %ymm4, %ymm5
	vpmullw	2080(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm4, %ymm10, %ymm4
	vpmulhw	3360(%rdx), %ymm2, %ymm10
	vpmullw	3200(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm3, %ymm8, %ymm3
	vpmulhw	768(%rdx), %ymm9, %ymm8
	vpmullw	2336(%rdx), %ymm9, %ymm9
	vpmulhw	%ymm6, %ymm9, %ymm9
	vpsubw	%ymm2, %ymm10, %ymm10
	vpaddw	%ymm0, %ymm10, %ymm2
	vpsubw	%ymm10, %ymm0, %ymm0
	vperm2i128	$32, %ymm3, %ymm7, %ymm10
	vperm2i128	$49, %ymm3, %ymm7, %ymm7
	vpsubw	%ymm9, %ymm8, %ymm8
	vpmulhw	896(%rdx), %ymm1, %ymm9
	vpmullw	2464(%rdx), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm9, %ymm1
	vpmulhw	1024(%rdx), %ymm5, %ymm9
	vpmullw	2592(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm9, %ymm5
	vpmulhw	1152(%rdx), %ymm4, %ymm9
	vpmullw	2720(%rdx), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm9, %ymm4
	vpmulhw	1408(%rdx), %ymm2, %ymm9
	vpmullw	2976(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm9, %ymm2
	vpmulhw	1280(%rdx), %ymm0, %ymm9
	vpmullw	2848(%rdx), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vperm2i128	$32, %ymm4, %ymm5, %ymm6
	vperm2i128	$49, %ymm4, %ymm5, %ymm5
	vmovdqu	%ymm10, -1024(%rax)
	vmovdqu	%ymm7, -896(%rax)
	vpsubw	%ymm0, %ymm9, %ymm0
	vperm2i128	$32, %ymm1, %ymm8, %ymm9
	vperm2i128	$49, %ymm1, %ymm8, %ymm1
	vperm2i128	$32, %ymm0, %ymm2, %ymm3
	vperm2i128	$49, %ymm0, %ymm2, %ymm2
	vmovdqu	%ymm9, -768(%rax)
	vmovdqu	%ymm1, -640(%rax)
	vmovdqu	%ymm6, -512(%rax)
	vmovdqu	%ymm5, -384(%rax)
	vmovdqu	%ymm3, -256(%rax)
	vmovdqu	%ymm2, -128(%rax)
	cmpq	%r8, %rcx
	jne	.L3
	salq	$10, %rcx
	movq	%rcx, %rax
	subq	%r9, %rax
	addq	%rax, %rdi
	leaq	(%rdi,%rcx), %rax
	.p2align 4,,10
	.p2align 3
.L5:
	vmovdqu	32(%rdi), %ymm1
	vmovdqu	800(%rdi), %ymm2
	addq	$1024, %rdi
	vmovdqu	-480(%rdi), %ymm0
	vmovdqu	-736(%rdi), %ymm5
	vmovdqu	-864(%rdi), %ymm3
	vmovdqu	-608(%rdi), %ymm4
	vpaddw	%ymm1, %ymm0, %ymm11
	vpsubw	%ymm0, %ymm1, %ymm0
	vmovdqa	3264(%rdx), %ymm12
	vpaddw	%ymm5, %ymm2, %ymm1
	vpsubw	%ymm2, %ymm5, %ymm5
	vmovdqu	-352(%rdi), %ymm2
	vpmulhw	%ymm12, %ymm5, %ymm8
	vpaddw	%ymm3, %ymm2, %ymm9
	vpsubw	%ymm2, %ymm3, %ymm2
	vmovdqu	-96(%rdi), %ymm3
	vpaddw	%ymm4, %ymm3, %ymm6
	vpsubw	%ymm3, %ymm4, %ymm4
	vpaddw	%ymm1, %ymm11, %ymm3
	vpsubw	%ymm1, %ymm11, %ymm11
	vmovdqa	3104(%rdx), %ymm1
	vpaddw	%ymm6, %ymm9, %ymm7
	vpsubw	%ymm6, %ymm9, %ymm9
	vmovdqa	1536(%rdx), %ymm6
	vpmullw	%ymm1, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm8, %ymm5
	vpmulhw	%ymm12, %ymm4, %ymm8
	vpaddw	%ymm5, %ymm0, %ymm10
	vpsubw	%ymm5, %ymm0, %ymm0
	vpmullw	%ymm4, %ymm1, %ymm5
	vpmullw	%ymm9, %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm5, %ymm8, %ymm4
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm4, %ymm2, %ymm5
	vpsubw	%ymm4, %ymm2, %ymm2
	vpmulhrsw	%ymm8, %ymm7, %ymm4
	vpmullw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm3, %ymm4, %ymm7
	vpsubw	%ymm4, %ymm3, %ymm3
	vpmulhw	%ymm12, %ymm9, %ymm4
	vpmulhrsw	%ymm8, %ymm7, %ymm8
	vpsubw	%ymm1, %ymm4, %ymm1
	vpmulhw	3328(%rdx), %ymm5, %ymm4
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmullw	%ymm6, %ymm8, %ymm8
	vpaddw	%ymm11, %ymm1, %ymm9
	vpsubw	%ymm1, %ymm11, %ymm1
	vpsubw	%ymm5, %ymm4, %ymm4
	vpsubw	%ymm8, %ymm7, %ymm7
	vpmulhw	544(%rdx), %ymm3, %ymm8
	vpaddw	%ymm10, %ymm4, %ymm5
	vpmullw	2112(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm4, %ymm10, %ymm4
	vpmulhw	3360(%rdx), %ymm2, %ymm10
	vpmullw	3200(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm3, %ymm8, %ymm3
	vpmulhw	800(%rdx), %ymm9, %ymm8
	vpmullw	2368(%rdx), %ymm9, %ymm9
	vpmulhw	%ymm6, %ymm9, %ymm9
	vpsubw	%ymm2, %ymm10, %ymm10
	vpaddw	%ymm0, %ymm10, %ymm2
	vpsubw	%ymm10, %ymm0, %ymm0
	vperm2i128	$32, %ymm3, %ymm7, %ymm10
	vperm2i128	$49, %ymm3, %ymm7, %ymm7
	vpsubw	%ymm9, %ymm8, %ymm8
	vpmulhw	928(%rdx), %ymm1, %ymm9
	vpmullw	2496(%rdx), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm9, %ymm1
	vpmulhw	1056(%rdx), %ymm5, %ymm9
	vpmullw	2624(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm9, %ymm5
	vpmulhw	1184(%rdx), %ymm4, %ymm9
	vpmullw	2752(%rdx), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm9, %ymm4
	vpmulhw	1440(%rdx), %ymm2, %ymm9
	vpmullw	3008(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm9, %ymm2
	vpmulhw	1312(%rdx), %ymm0, %ymm9
	vpmullw	2880(%rdx), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vperm2i128	$32, %ymm4, %ymm5, %ymm6
	vperm2i128	$49, %ymm4, %ymm5, %ymm5
	vmovdqu	%ymm10, -992(%rdi)
	vmovdqu	%ymm7, -864(%rdi)
	vpsubw	%ymm0, %ymm9, %ymm0
	vperm2i128	$32, %ymm1, %ymm8, %ymm9
	vperm2i128	$49, %ymm1, %ymm8, %ymm1
	vperm2i128	$32, %ymm0, %ymm2, %ymm3
	vperm2i128	$49, %ymm0, %ymm2, %ymm2
	vmovdqu	%ymm9, -736(%rdi)
	vmovdqu	%ymm1, -608(%rdi)
	vmovdqu	%ymm6, -480(%rdi)
	vmovdqu	%ymm5, -352(%rdi)
	vmovdqu	%ymm3, -224(%rdi)
	vmovdqu	%ymm2, -96(%rdi)
	cmpq	%rax, %rdi
	jne	.L5
	addq	%r10, %rdi
	leaq	(%rdi,%rcx), %rax
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	64(%rdi), %ymm1
	vmovdqu	832(%rdi), %ymm2
	addq	$1024, %rdi
	vmovdqu	-448(%rdi), %ymm0
	vmovdqu	-704(%rdi), %ymm5
	vmovdqu	-832(%rdi), %ymm3
	vmovdqu	-576(%rdi), %ymm4
	vpaddw	%ymm1, %ymm0, %ymm11
	vpsubw	%ymm0, %ymm1, %ymm0
	vmovdqa	3264(%rdx), %ymm12
	vpaddw	%ymm5, %ymm2, %ymm1
	vpsubw	%ymm2, %ymm5, %ymm5
	vmovdqu	-320(%rdi), %ymm2
	vpmulhw	%ymm12, %ymm5, %ymm8
	vpaddw	%ymm3, %ymm2, %ymm9
	vpsubw	%ymm2, %ymm3, %ymm2
	vmovdqu	-64(%rdi), %ymm3
	vpaddw	%ymm4, %ymm3, %ymm6
	vpsubw	%ymm3, %ymm4, %ymm4
	vpaddw	%ymm1, %ymm11, %ymm3
	vpsubw	%ymm1, %ymm11, %ymm11
	vmovdqa	3104(%rdx), %ymm1
	vpaddw	%ymm6, %ymm9, %ymm7
	vpsubw	%ymm6, %ymm9, %ymm9
	vmovdqa	1536(%rdx), %ymm6
	vpmullw	%ymm1, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm8, %ymm5
	vpmulhw	%ymm12, %ymm4, %ymm8
	vpaddw	%ymm5, %ymm0, %ymm10
	vpsubw	%ymm5, %ymm0, %ymm0
	vpmullw	%ymm4, %ymm1, %ymm5
	vpmullw	%ymm9, %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm5, %ymm8, %ymm4
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm4, %ymm2, %ymm5
	vpsubw	%ymm4, %ymm2, %ymm2
	vpmulhrsw	%ymm8, %ymm7, %ymm4
	vpmullw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm3, %ymm4, %ymm7
	vpsubw	%ymm4, %ymm3, %ymm3
	vpmulhw	%ymm12, %ymm9, %ymm4
	vpmulhrsw	%ymm8, %ymm7, %ymm8
	vpsubw	%ymm1, %ymm4, %ymm1
	vpmulhw	3328(%rdx), %ymm5, %ymm4
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmullw	%ymm6, %ymm8, %ymm8
	vpaddw	%ymm11, %ymm1, %ymm9
	vpsubw	%ymm1, %ymm11, %ymm1
	vpsubw	%ymm5, %ymm4, %ymm4
	vpsubw	%ymm8, %ymm7, %ymm7
	vpmulhw	576(%rdx), %ymm3, %ymm8
	vpaddw	%ymm10, %ymm4, %ymm5
	vpmullw	2144(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm4, %ymm10, %ymm4
	vpmulhw	3360(%rdx), %ymm2, %ymm10
	vpmullw	3200(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm3, %ymm8, %ymm3
	vpmulhw	832(%rdx), %ymm9, %ymm8
	vpmullw	2400(%rdx), %ymm9, %ymm9
	vpmulhw	%ymm6, %ymm9, %ymm9
	vpsubw	%ymm2, %ymm10, %ymm10
	vpaddw	%ymm0, %ymm10, %ymm2
	vpsubw	%ymm10, %ymm0, %ymm0
	vperm2i128	$32, %ymm3, %ymm7, %ymm10
	vperm2i128	$49, %ymm3, %ymm7, %ymm7
	vpsubw	%ymm9, %ymm8, %ymm8
	vpmulhw	960(%rdx), %ymm1, %ymm9
	vpmullw	2528(%rdx), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm9, %ymm1
	vpmulhw	1088(%rdx), %ymm5, %ymm9
	vpmullw	2656(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm9, %ymm5
	vpmulhw	1216(%rdx), %ymm4, %ymm9
	vpmullw	2784(%rdx), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm9, %ymm4
	vpmulhw	1472(%rdx), %ymm2, %ymm9
	vpmullw	3040(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm9, %ymm2
	vpmulhw	1344(%rdx), %ymm0, %ymm9
	vpmullw	2912(%rdx), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vperm2i128	$32, %ymm4, %ymm5, %ymm6
	vperm2i128	$49, %ymm4, %ymm5, %ymm5
	vmovdqu	%ymm10, -960(%rdi)
	vmovdqu	%ymm7, -832(%rdi)
	vpsubw	%ymm0, %ymm9, %ymm0
	vperm2i128	$32, %ymm1, %ymm8, %ymm9
	vperm2i128	$49, %ymm1, %ymm8, %ymm1
	vperm2i128	$32, %ymm0, %ymm2, %ymm3
	vperm2i128	$49, %ymm0, %ymm2, %ymm2
	vmovdqu	%ymm9, -704(%rdi)
	vmovdqu	%ymm1, -576(%rdi)
	vmovdqu	%ymm6, -448(%rdi)
	vmovdqu	%ymm5, -320(%rdi)
	vmovdqu	%ymm3, -192(%rdi)
	vmovdqu	%ymm2, -64(%rdi)
	cmpq	%rax, %rdi
	jne	.L7
	addq	%r10, %rdi
	addq	%rdi, %rcx
	.p2align 4,,10
	.p2align 3
.L9:
	vmovdqu	96(%rdi), %ymm1
	vmovdqu	864(%rdi), %ymm2
	addq	$1024, %rdi
	vmovdqu	-416(%rdi), %ymm0
	vmovdqu	-672(%rdi), %ymm5
	vmovdqu	-800(%rdi), %ymm3
	vmovdqu	-544(%rdi), %ymm4
	vpaddw	%ymm1, %ymm0, %ymm11
	vpsubw	%ymm0, %ymm1, %ymm0
	vmovdqa	3264(%rdx), %ymm12
	vpaddw	%ymm5, %ymm2, %ymm1
	vpsubw	%ymm2, %ymm5, %ymm5
	vmovdqu	-288(%rdi), %ymm2
	vpmulhw	%ymm12, %ymm5, %ymm8
	vpaddw	%ymm3, %ymm2, %ymm9
	vpsubw	%ymm2, %ymm3, %ymm2
	vmovdqu	-32(%rdi), %ymm3
	vpaddw	%ymm4, %ymm3, %ymm6
	vpsubw	%ymm3, %ymm4, %ymm4
	vpaddw	%ymm1, %ymm11, %ymm3
	vpsubw	%ymm1, %ymm11, %ymm11
	vmovdqa	3104(%rdx), %ymm1
	vpaddw	%ymm6, %ymm9, %ymm7
	vpsubw	%ymm6, %ymm9, %ymm9
	vmovdqa	1536(%rdx), %ymm6
	vpmullw	%ymm1, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm8, %ymm5
	vpmulhw	%ymm12, %ymm4, %ymm8
	vpaddw	%ymm5, %ymm0, %ymm10
	vpsubw	%ymm5, %ymm0, %ymm0
	vpmullw	%ymm4, %ymm1, %ymm5
	vpmullw	%ymm9, %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm5, %ymm8, %ymm4
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm4, %ymm2, %ymm5
	vpsubw	%ymm4, %ymm2, %ymm2
	vpmulhrsw	%ymm8, %ymm7, %ymm4
	vpmullw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm3, %ymm4, %ymm7
	vpsubw	%ymm4, %ymm3, %ymm3
	vpmulhw	%ymm12, %ymm9, %ymm4
	vpmulhrsw	%ymm8, %ymm7, %ymm8
	vpsubw	%ymm1, %ymm4, %ymm1
	vpmulhw	3328(%rdx), %ymm5, %ymm4
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpmullw	%ymm6, %ymm8, %ymm8
	vpaddw	%ymm11, %ymm1, %ymm9
	vpsubw	%ymm1, %ymm11, %ymm1
	vpsubw	%ymm5, %ymm4, %ymm4
	vpsubw	%ymm8, %ymm7, %ymm7
	vpmulhw	608(%rdx), %ymm3, %ymm8
	vpaddw	%ymm10, %ymm4, %ymm5
	vpmullw	2176(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm4, %ymm10, %ymm4
	vpmulhw	3360(%rdx), %ymm2, %ymm10
	vpmullw	3200(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm3, %ymm8, %ymm3
	vpmulhw	864(%rdx), %ymm9, %ymm8
	vpmullw	2432(%rdx), %ymm9, %ymm9
	vpmulhw	%ymm6, %ymm9, %ymm9
	vpsubw	%ymm2, %ymm10, %ymm10
	vpaddw	%ymm0, %ymm10, %ymm2
	vpsubw	%ymm10, %ymm0, %ymm0
	vperm2i128	$32, %ymm3, %ymm7, %ymm10
	vperm2i128	$49, %ymm3, %ymm7, %ymm7
	vpsubw	%ymm9, %ymm8, %ymm8
	vpmulhw	992(%rdx), %ymm1, %ymm9
	vpmullw	2560(%rdx), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm9, %ymm1
	vpmulhw	1120(%rdx), %ymm5, %ymm9
	vpmullw	2688(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm9, %ymm5
	vpmulhw	1248(%rdx), %ymm4, %ymm9
	vpmullw	2816(%rdx), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm9, %ymm4
	vpmulhw	1504(%rdx), %ymm2, %ymm9
	vpmullw	3072(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm9, %ymm2
	vpmulhw	1376(%rdx), %ymm0, %ymm9
	vpmullw	2944(%rdx), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vperm2i128	$32, %ymm4, %ymm5, %ymm6
	vperm2i128	$49, %ymm4, %ymm5, %ymm5
	vmovdqu	%ymm10, -928(%rdi)
	vmovdqu	%ymm7, -800(%rdi)
	vpsubw	%ymm0, %ymm9, %ymm0
	vperm2i128	$32, %ymm1, %ymm8, %ymm9
	vperm2i128	$49, %ymm1, %ymm8, %ymm1
	vperm2i128	$32, %ymm0, %ymm2, %ymm3
	vperm2i128	$49, %ymm0, %ymm2, %ymm2
	vmovdqu	%ymm9, -672(%rdi)
	vmovdqu	%ymm1, -544(%rdi)
	vmovdqu	%ymm6, -416(%rdi)
	vmovdqu	%ymm5, -288(%rdi)
	vmovdqu	%ymm3, -160(%rdi)
	vmovdqu	%ymm2, -32(%rdi)
	cmpq	%rcx, %rdi
	jne	.L9
.L14:
	addq	%r10, %rdi
	sall	$2, %esi
	xorl	%ecx, %ecx
	movslq	%esi, %r8
	movq	%rdi, %rax
	testl	%esi, %esi
	jle	.L27
	.p2align 4,,10
	.p2align 3
.L10:
	vmovdqu	64(%rax), %ymm0
	vmovdqu	(%rax), %ymm2
	addq	$1, %rcx
	addq	$256, %rax
	vmovdqu	-128(%rax), %ymm1
	vmovdqu	-224(%rax), %ymm6
	vpaddw	%ymm2, %ymm0, %ymm5
	vpsubw	%ymm0, %ymm2, %ymm2
	vmovdqu	-64(%rax), %ymm0
	vpsubw	%ymm0, %ymm1, %ymm9
	vpaddw	%ymm1, %ymm0, %ymm11
	vmovdqu	-160(%rax), %ymm0
	vmovdqu	-96(%rax), %ymm1
	vpaddw	%ymm6, %ymm0, %ymm3
	vpsubw	%ymm0, %ymm6, %ymm6
	vmovdqu	-32(%rax), %ymm0
	vpaddw	%ymm1, %ymm0, %ymm4
	vpsubw	%ymm0, %ymm1, %ymm10
	vmovdqa	3104(%rdx), %ymm1
	vpaddw	%ymm3, %ymm5, %ymm0
	vpsubw	%ymm3, %ymm5, %ymm5
	vmovdqa	3264(%rdx), %ymm3
	vpaddw	%ymm4, %ymm11, %ymm7
	vpsubw	%ymm4, %ymm11, %ymm11
	vmovdqa	1536(%rdx), %ymm4
	vpmulhw	%ymm3, %ymm6, %ymm8
	vpmullw	%ymm1, %ymm6, %ymm6
	vpmullw	%ymm10, %ymm1, %ymm1
	vpmulhw	%ymm3, %ymm10, %ymm3
	vpmulhw	%ymm4, %ymm6, %ymm6
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm1, %ymm3, %ymm1
	vpaddw	%ymm8, %ymm2, %ymm6
	vpmulhw	256(%rdx), %ymm5, %ymm3
	vpmullw	1824(%rdx), %ymm5, %ymm5
	vpaddw	%ymm1, %ymm9, %ymm10
	vpsubw	%ymm1, %ymm9, %ymm1
	vpmulhw	%ymm4, %ymm5, %ymm5
	vmovdqa	3232(%rdx), %ymm9
	vpsubw	%ymm8, %ymm2, %ymm2
	vpmulhw	288(%rdx), %ymm11, %ymm8
	vpmullw	1856(%rdx), %ymm11, %ymm11
	vpmulhw	%ymm4, %ymm11, %ymm15
	vpmulhrsw	%ymm9, %ymm0, %ymm14
	vpmulhrsw	%ymm9, %ymm7, %ymm9
	vpmulhw	448(%rdx), %ymm2, %ymm13
	vpmulhw	480(%rdx), %ymm1, %ymm11
	vpmulhw	416(%rdx), %ymm10, %ymm12
	vpmullw	2016(%rdx), %ymm2, %ymm2
	vpmullw	1984(%rdx), %ymm10, %ymm10
	vpmullw	2048(%rdx), %ymm1, %ymm1
	vpmulhw	%ymm4, %ymm10, %ymm10
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm5, %ymm3, %ymm3
	vmovdqa	%ymm15, -48(%rbp)
	vpmulhw	384(%rdx), %ymm6, %ymm15
	vpsubw	-48(%rbp), %ymm8, %ymm8
	vpmullw	1952(%rdx), %ymm6, %ymm6
	vpmullw	%ymm4, %ymm14, %ymm14
	vpmulhw	%ymm4, %ymm6, %ymm6
	vpmullw	%ymm4, %ymm9, %ymm4
	vpsubw	%ymm2, %ymm13, %ymm2
	vpsubw	%ymm10, %ymm12, %ymm10
	vpsubw	%ymm1, %ymm11, %ymm1
	vpunpckhwd	%ymm1, %ymm8, %ymm11
	vpsubw	%ymm14, %ymm0, %ymm0
	vpsubw	%ymm6, %ymm15, %ymm6
	vpunpcklwd	%ymm2, %ymm3, %ymm14
	vpsubw	%ymm4, %ymm7, %ymm7
	vpunpcklwd	%ymm6, %ymm0, %ymm15
	vpunpckhwd	%ymm6, %ymm0, %ymm6
	vpunpcklwd	%ymm10, %ymm7, %ymm13
	vpunpcklwd	%ymm1, %ymm8, %ymm0
	vpunpckhwd	%ymm2, %ymm3, %ymm3
	vpaddw	%ymm0, %ymm14, %ymm1
	vpunpckhwd	%ymm10, %ymm7, %ymm10
	vpaddw	%ymm13, %ymm15, %ymm2
	vpaddw	%ymm10, %ymm6, %ymm7
	vpsubw	%ymm0, %ymm14, %ymm0
	vpunpckldq	%ymm1, %ymm2, %ymm5
	vpsubw	%ymm13, %ymm15, %ymm14
	vpunpckhdq	%ymm1, %ymm2, %ymm2
	vpsubw	%ymm10, %ymm6, %ymm6
	vmovdqu	%ymm5, -256(%rax)
	vpaddw	%ymm11, %ymm3, %ymm1
	vpsubw	%ymm11, %ymm3, %ymm11
	vmovdqu	%ymm2, -224(%rax)
	vpunpckldq	%ymm1, %ymm7, %ymm4
	vpunpckhdq	%ymm1, %ymm7, %ymm1
	vpunpckldq	%ymm0, %ymm14, %ymm7
	vpunpckhdq	%ymm0, %ymm14, %ymm14
	vpunpckldq	%ymm11, %ymm6, %ymm0
	vpunpckhdq	%ymm11, %ymm6, %ymm6
	vmovdqu	%ymm4, -192(%rax)
	vmovdqu	%ymm1, -160(%rax)
	vmovdqu	%ymm7, -128(%rax)
	vmovdqu	%ymm14, -96(%rax)
	vmovdqu	%ymm0, -64(%rax)
	vmovdqu	%ymm6, -32(%rax)
	cmpq	%rcx, %r8
	jne	.L10
	salq	$8, %r8
	movq	%r8, %rax
	subq	%r9, %rax
	addq	%rax, %rdi
	addq	%rdi, %r8
	.p2align 4,,10
	.p2align 3
.L12:
	vmovdqu	64(%rdi), %ymm0
	vmovdqu	(%rdi), %ymm6
	addq	$256, %rdi
	vmovdqu	-224(%rdi), %ymm5
	vmovdqa	3104(%rdx), %ymm8
	vpaddw	%ymm6, %ymm0, %ymm1
	vpsubw	%ymm0, %ymm6, %ymm6
	vmovdqu	-64(%rdi), %ymm3
	vmovdqu	-160(%rdi), %ymm0
	vmovdqa	1536(%rdx), %ymm2
	vmovdqu	-32(%rdi), %ymm11
	vpaddw	%ymm5, %ymm0, %ymm9
	vpsubw	%ymm0, %ymm5, %ymm5
	vmovdqa	3264(%rdx), %ymm0
	vpmulhw	%ymm0, %ymm3, %ymm4
	vpmullw	%ymm3, %ymm8, %ymm3
	vpmulhw	%ymm2, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm4, %ymm3
	vmovdqu	-128(%rdi), %ymm4
	vpaddw	%ymm4, %ymm3, %ymm7
	vpsubw	%ymm3, %ymm4, %ymm4
	vpmulhw	%ymm0, %ymm11, %ymm3
	vpmulhw	192(%rdx), %ymm4, %ymm13
	vpmullw	%ymm11, %ymm8, %ymm11
	vpmullw	1760(%rdx), %ymm4, %ymm4
	vpmulhw	%ymm2, %ymm4, %ymm4
	vpmulhw	%ymm2, %ymm11, %ymm11
	vpsubw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm11, %ymm3, %ymm11
	vmovdqu	-96(%rdi), %ymm3
	vpaddw	%ymm3, %ymm11, %ymm12
	vpsubw	%ymm11, %ymm3, %ymm3
	vmovdqa	3232(%rdx), %ymm11
	vpmulhw	224(%rdx), %ymm3, %ymm4
	vpmullw	1792(%rdx), %ymm3, %ymm3
	vpmulhw	160(%rdx), %ymm12, %ymm14
	vpmulhrsw	%ymm11, %ymm1, %ymm10
	vpmulhrsw	%ymm11, %ymm9, %ymm11
	vpmullw	%ymm2, %ymm10, %ymm10
	vpmullw	%ymm2, %ymm11, %ymm11
	vpsubw	%ymm10, %ymm1, %ymm10
	vpsubw	%ymm11, %ymm9, %ymm9
	vpmulhw	(%rdx), %ymm6, %ymm1
	vpmullw	1600(%rdx), %ymm5, %ymm11
	vpmullw	1568(%rdx), %ymm6, %ymm6
	vpmulhw	%ymm2, %ymm11, %ymm11
	vpmulhw	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm1, %ymm6
	vpmulhw	32(%rdx), %ymm5, %ymm1
	vpsubw	%ymm11, %ymm1, %ymm11
	vpmulhw	128(%rdx), %ymm7, %ymm1
	vpmullw	1696(%rdx), %ymm7, %ymm7
	vpmulhw	%ymm2, %ymm7, %ymm7
	vpmullw	1728(%rdx), %ymm12, %ymm5
	vpmulhw	%ymm2, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm1, %ymm1
	vpmulhw	%ymm2, %ymm3, %ymm7
	vpsubw	%ymm5, %ymm14, %ymm14
	vpunpcklqdq	%ymm14, %ymm9, %ymm12
	vpunpcklqdq	%ymm1, %ymm10, %ymm5
	vpunpckhqdq	%ymm14, %ymm9, %ymm9
	vpunpckhqdq	%ymm1, %ymm10, %ymm1
	vpaddw	%ymm5, %ymm12, %ymm10
	vpsubw	%ymm12, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm4, %ymm7
	vpunpcklqdq	%ymm13, %ymm6, %ymm4
	vpunpckhqdq	%ymm13, %ymm6, %ymm6
	vpunpcklqdq	%ymm7, %ymm11, %ymm3
	vpunpckhqdq	%ymm7, %ymm11, %ymm7
	vpaddw	%ymm1, %ymm9, %ymm11
	vpsubw	%ymm9, %ymm1, %ymm1
	vpaddw	%ymm4, %ymm3, %ymm9
	vpsubw	%ymm3, %ymm4, %ymm4
	vpmulhw	%ymm0, %ymm1, %ymm3
	vpaddw	%ymm6, %ymm7, %ymm12
	vpmullw	%ymm1, %ymm8, %ymm1
	vpsubw	%ymm7, %ymm6, %ymm7
	vpmulhw	%ymm0, %ymm7, %ymm0
	vpmulhw	%ymm2, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm3, %ymm1
	vpmullw	%ymm7, %ymm8, %ymm3
	vpmulhw	%ymm2, %ymm3, %ymm2
	vpsubw	%ymm2, %ymm0, %ymm0
	vpaddw	%ymm10, %ymm11, %ymm2
	vmovdqu	%ymm2, -256(%rdi)
	vpaddw	%ymm5, %ymm1, %ymm2
	vpsubw	%ymm11, %ymm10, %ymm10
	vmovdqu	%ymm2, -224(%rdi)
	vpaddw	%ymm9, %ymm12, %ymm2
	vpsubw	%ymm1, %ymm5, %ymm5
	vmovdqu	%ymm2, -192(%rdi)
	vpsubw	%ymm12, %ymm9, %ymm9
	vpaddw	%ymm4, %ymm0, %ymm2
	vpsubw	%ymm0, %ymm4, %ymm0
	vmovdqu	%ymm2, -160(%rdi)
	vmovdqu	%ymm10, -128(%rdi)
	vmovdqu	%ymm5, -96(%rdi)
	vmovdqu	%ymm9, -64(%rdi)
	vmovdqu	%ymm0, -32(%rdi)
	cmpq	%r8, %rdi
	jne	.L12
.L27:
	vzeroupper
	popq	%r10
	.cfi_remember_state
	.cfi_def_cfa 10, 0
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L2:
	.cfi_restore_state
	movq	%r10, %rax
	subq	%r9, %rax
	subq	%r9, %rax
	addq	%rax, %rdi
	jmp	.L14
	.cfi_endproc
.LFE4755:
	.size	ntt512, .-ntt512
	.p2align 4,,15
	.type	invntt512, @function
invntt512:
.LFB4758:
	.cfi_startproc
	leal	0(,%rsi,4), %r9d
	sall	$9, %esi
	movslq	%esi, %r8
	movslq	%r9d, %r10
	addq	%r8, %r8
	movq	%r8, %r11
	negq	%r11
	testl	%r9d, %r9d
	jle	.L31
	movq	%rdi, %rcx
	xorl	%esi, %esi
	.p2align 4,,10
	.p2align 3
.L32:
	vmovdqu	96(%rcx), %ymm1
	vmovdqu	224(%rcx), %ymm0
	addq	$1, %rsi
	addq	$256, %rcx
	vmovdqa	3296(%rdx), %ymm6
	vmovdqa	3136(%rdx), %ymm4
	vpaddw	%ymm0, %ymm1, %ymm8
	vpsubw	%ymm0, %ymm1, %ymm1
	vmovdqu	-128(%rcx), %ymm7
	vmovdqa	1536(%rdx), %ymm0
	vpmulhw	%ymm6, %ymm1, %ymm5
	vpmullw	%ymm4, %ymm1, %ymm1
	vmovdqu	-224(%rcx), %ymm3
	vpmulhw	%ymm0, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm5, %ymm10
	vmovdqu	-96(%rcx), %ymm1
	vmovdqu	-192(%rcx), %ymm5
	vpaddw	%ymm1, %ymm3, %ymm9
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	-64(%rcx), %ymm1
	vpmulhw	%ymm6, %ymm3, %ymm11
	vpmullw	%ymm4, %ymm3, %ymm3
	vpaddw	%ymm1, %ymm5, %ymm2
	vpsubw	%ymm1, %ymm5, %ymm5
	vpmulhw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm11, %ymm11
	vmovdqu	-256(%rcx), %ymm3
	vpaddw	%ymm7, %ymm3, %ymm1
	vpsubw	%ymm7, %ymm3, %ymm3
	vpaddw	%ymm1, %ymm9, %ymm13
	vpaddw	%ymm10, %ymm5, %ymm7
	vpsubw	%ymm9, %ymm1, %ymm1
	vpsubw	%ymm10, %ymm5, %ymm5
	vpaddw	%ymm8, %ymm2, %ymm10
	vpsubw	%ymm8, %ymm2, %ymm2
	vpaddw	%ymm3, %ymm11, %ymm8
	vpsubw	%ymm11, %ymm3, %ymm3
	vpunpcklqdq	%ymm5, %ymm2, %ymm11
	vpunpckhqdq	%ymm5, %ymm2, %ymm2
	vpunpcklqdq	%ymm7, %ymm10, %ymm5
	vpunpckhqdq	%ymm7, %ymm10, %ymm10
	vpmulhw	128(%rdx), %ymm10, %ymm9
	vpunpcklqdq	%ymm3, %ymm1, %ymm7
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpmulhw	160(%rdx), %ymm2, %ymm3
	vpunpcklqdq	%ymm8, %ymm13, %ymm12
	vpmullw	1728(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpunpckhqdq	%ymm8, %ymm13, %ymm8
	vpsubw	%ymm2, %ymm3, %ymm2
	vpmullw	1696(%rdx), %ymm10, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vpmulhw	96(%rdx), %ymm11, %ymm10
	vpsubw	%ymm3, %ymm9, %ymm9
	vpmulhw	224(%rdx), %ymm1, %ymm3
	vpmullw	1792(%rdx), %ymm1, %ymm1
	vpmulhw	%ymm0, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm3, %ymm1
	vpmulhw	192(%rdx), %ymm8, %ymm3
	vpmullw	1760(%rdx), %ymm8, %ymm8
	vpmulhw	%ymm0, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm3, %ymm8
	vpmullw	1664(%rdx), %ymm11, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vpmulhw	64(%rdx), %ymm5, %ymm11
	vpmullw	1632(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vpsubw	%ymm9, %ymm8, %ymm13
	vpaddw	%ymm9, %ymm8, %ymm8
	vpsubw	%ymm3, %ymm10, %ymm10
	vpsubw	%ymm5, %ymm11, %ymm11
	vmovdqa	3232(%rdx), %ymm5
	vmovdqu	%ymm8, -128(%rcx)
	vpmulhrsw	%ymm5, %ymm7, %ymm3
	vpmullw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm7, %ymm7
	vpmulhrsw	%ymm5, %ymm12, %ymm3
	vpsubw	%ymm2, %ymm1, %ymm5
	vpaddw	%ymm2, %ymm1, %ymm1
	vmovdqu	%ymm1, -96(%rcx)
	vpmullw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm12, %ymm3
	vpmulhw	%ymm6, %ymm5, %ymm12
	vpmullw	%ymm5, %ymm4, %ymm5
	vpmullw	%ymm13, %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm13, %ymm6
	vpmulhw	%ymm0, %ymm5, %ymm5
	vpmulhw	%ymm0, %ymm4, %ymm0
	vpaddw	%ymm11, %ymm3, %ymm4
	vmovdqu	%ymm4, -256(%rcx)
	vpsubw	%ymm11, %ymm3, %ymm3
	vpaddw	%ymm10, %ymm7, %ymm4
	vpsubw	%ymm10, %ymm7, %ymm7
	vmovdqu	%ymm4, -224(%rcx)
	vmovdqu	%ymm3, -192(%rcx)
	vmovdqu	%ymm7, -160(%rcx)
	vpsubw	%ymm0, %ymm6, %ymm0
	vpsubw	%ymm5, %ymm12, %ymm5
	vmovdqu	%ymm0, -64(%rcx)
	vmovdqu	%ymm5, -32(%rcx)
	cmpq	%r10, %rsi
	jne	.L32
	salq	$8, %rsi
	movq	%rsi, %rax
	subq	%r8, %rax
	addq	%rdi, %rax
	addq	%rax, %rsi
	.p2align 4,,10
	.p2align 3
.L34:
	vmovdqu	224(%rax), %ymm5
	vmovdqu	192(%rax), %ymm0
	addq	$256, %rax
	vpunpckldq	%ymm5, %ymm0, %ymm1
	vpunpckhdq	%ymm5, %ymm0, %ymm0
	vpunpckldq	%ymm0, %ymm1, %ymm3
	vpunpckhdq	%ymm0, %ymm1, %ymm5
	vmovdqu	-128(%rax), %ymm1
	vmovdqu	-96(%rax), %ymm0
	vpunpckldq	%ymm0, %ymm1, %ymm4
	vpunpckhdq	%ymm0, %ymm1, %ymm0
	vmovdqu	-160(%rax), %ymm1
	vpunpckldq	%ymm0, %ymm4, %ymm2
	vpunpckhdq	%ymm0, %ymm4, %ymm0
	vmovdqu	-192(%rax), %ymm4
	vpunpckldq	%ymm1, %ymm4, %ymm9
	vpunpckhdq	%ymm1, %ymm4, %ymm1
	vmovdqu	-224(%rax), %ymm4
	vpunpckldq	%ymm1, %ymm9, %ymm13
	vpunpckhdq	%ymm1, %ymm9, %ymm9
	vmovdqu	-256(%rax), %ymm1
	vpunpckldq	%ymm4, %ymm1, %ymm7
	vpunpckhdq	%ymm4, %ymm1, %ymm1
	vpunpckldq	%ymm1, %ymm7, %ymm4
	vpunpckhdq	%ymm1, %ymm7, %ymm7
	vpsubw	%ymm5, %ymm9, %ymm1
	vpsubw	%ymm0, %ymm7, %ymm8
	vpsubw	%ymm2, %ymm4, %ymm12
	vpunpcklwd	%ymm1, %ymm8, %ymm6
	vpunpckhwd	%ymm1, %ymm8, %ymm8
	vpsubw	%ymm3, %ymm13, %ymm1
	vpunpcklwd	%ymm8, %ymm6, %ymm10
	vpunpckhwd	%ymm8, %ymm6, %ymm8
	vpaddw	%ymm7, %ymm0, %ymm0
	vpunpcklwd	%ymm8, %ymm10, %ymm6
	vpunpckhwd	%ymm8, %ymm10, %ymm8
	vpunpcklwd	%ymm1, %ymm12, %ymm10
	vpunpckhwd	%ymm1, %ymm12, %ymm12
	vpaddw	%ymm4, %ymm2, %ymm2
	vmovdqa	1536(%rdx), %ymm4
	vpunpckhwd	%ymm12, %ymm10, %ymm1
	vpunpcklwd	%ymm12, %ymm10, %ymm14
	vpunpcklwd	%ymm1, %ymm14, %ymm11
	vpunpckhwd	%ymm1, %ymm14, %ymm12
	vpaddw	%ymm9, %ymm5, %ymm1
	vpunpcklwd	%ymm1, %ymm0, %ymm7
	vpunpckhwd	%ymm1, %ymm0, %ymm0
	vpunpcklwd	%ymm0, %ymm7, %ymm9
	vpunpckhwd	%ymm0, %ymm7, %ymm7
	vpaddw	%ymm13, %ymm3, %ymm0
	vpunpcklwd	%ymm0, %ymm2, %ymm1
	vpunpckhwd	%ymm0, %ymm2, %ymm2
	vpunpcklwd	%ymm7, %ymm9, %ymm5
	vpunpcklwd	%ymm2, %ymm1, %ymm0
	vpunpckhwd	%ymm2, %ymm1, %ymm2
	vpmulhw	416(%rdx), %ymm8, %ymm1
	vpunpckhwd	%ymm7, %ymm9, %ymm7
	vpmullw	1984(%rdx), %ymm8, %ymm8
	vpmulhw	%ymm4, %ymm8, %ymm8
	vpunpcklwd	%ymm2, %ymm0, %ymm3
	vpunpckhwd	%ymm2, %ymm0, %ymm2
	vpmulhw	384(%rdx), %ymm7, %ymm0
	vpmullw	1952(%rdx), %ymm7, %ymm7
	vpmulhw	%ymm4, %ymm7, %ymm7
	vpsubw	%ymm8, %ymm1, %ymm10
	vpmullw	2048(%rdx), %ymm12, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpmulhw	352(%rdx), %ymm6, %ymm8
	vpsubw	%ymm7, %ymm0, %ymm9
	vpmulhw	480(%rdx), %ymm12, %ymm0
	vpmulhw	448(%rdx), %ymm2, %ymm7
	vpmullw	1920(%rdx), %ymm6, %ymm6
	vpmulhw	%ymm4, %ymm6, %ymm6
	vpsubw	%ymm1, %ymm0, %ymm1
	vpmullw	2016(%rdx), %ymm2, %ymm0
	vpmulhw	%ymm4, %ymm0, %ymm0
	vpsubw	%ymm6, %ymm8, %ymm8
	vpaddw	%ymm10, %ymm1, %ymm6
	vpsubw	%ymm10, %ymm1, %ymm1
	vpsubw	%ymm0, %ymm7, %ymm0
	vpmulhw	320(%rdx), %ymm5, %ymm7
	vpmullw	1888(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm4, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm7, %ymm7
	vmovdqa	3232(%rdx), %ymm5
	vpmulhrsw	%ymm5, %ymm11, %ymm2
	vpmulhrsw	%ymm5, %ymm3, %ymm5
	vpmullw	%ymm4, %ymm2, %ymm2
	vpmullw	%ymm4, %ymm5, %ymm5
	vpsubw	%ymm2, %ymm11, %ymm2
	vpsubw	%ymm5, %ymm3, %ymm3
	vmovdqa	3136(%rdx), %ymm11
	vmovdqa	3296(%rdx), %ymm5
	vpmulhw	%ymm5, %ymm1, %ymm10
	vpmullw	%ymm1, %ymm11, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm10, %ymm1
	vpaddw	%ymm9, %ymm0, %ymm10
	vpsubw	%ymm9, %ymm0, %ymm0
	vpmulhw	%ymm5, %ymm0, %ymm5
	vpmullw	%ymm11, %ymm0, %ymm0
	vpmulhw	%ymm4, %ymm0, %ymm0
	vpaddw	%ymm8, %ymm2, %ymm4
	vpsubw	%ymm8, %ymm2, %ymm2
	vpsubw	%ymm0, %ymm5, %ymm0
	vpaddw	%ymm7, %ymm3, %ymm5
	vpsubw	%ymm7, %ymm3, %ymm3
	vpaddw	%ymm10, %ymm5, %ymm7
	vmovdqu	%ymm7, -256(%rax)
	vpaddw	%ymm0, %ymm3, %ymm7
	vpsubw	%ymm0, %ymm3, %ymm3
	vpaddw	%ymm6, %ymm4, %ymm0
	vpsubw	%ymm10, %ymm5, %ymm10
	vmovdqu	%ymm7, -224(%rax)
	vmovdqu	%ymm0, -128(%rax)
	vpsubw	%ymm6, %ymm4, %ymm6
	vpaddw	%ymm1, %ymm2, %ymm0
	vpsubw	%ymm1, %ymm2, %ymm1
	vmovdqu	%ymm10, -192(%rax)
	vmovdqu	%ymm3, -160(%rax)
	vmovdqu	%ymm0, -96(%rax)
	vmovdqu	%ymm6, -64(%rax)
	vmovdqu	%ymm1, -32(%rax)
	cmpq	%rsi, %rax
	jne	.L34
.L43:
	leal	3(%r9), %ecx
	addq	%r11, %rax
	testl	%r9d, %r9d
	cmovns	%r9d, %ecx
	sarl	$2, %ecx
	movl	%ecx, %edi
	movslq	%ecx, %r8
	sall	$9, %edi
	movslq	%edi, %rdi
	addq	%rdi, %rdi
	movq	%rdi, %r9
	negq	%r9
	testl	%ecx, %ecx
	jle	.L54
	movq	%rax, %rcx
	xorl	%esi, %esi
	.p2align 4,,10
	.p2align 3
.L36:
	vmovdqa	1536(%rdx), %ymm0
	vmovdqu	992(%rcx), %ymm5
	addq	$1, %rsi
	addq	$1024, %rcx
	vmovdqu	-160(%rcx), %ymm3
	vmovdqu	-288(%rcx), %ymm6
	vmovdqu	-544(%rcx), %ymm7
	vmovdqu	-416(%rcx), %ymm2
	vperm2i128	$32, %ymm5, %ymm3, %ymm9
	vperm2i128	$49, %ymm5, %ymm3, %ymm3
	vpmulhw	1248(%rdx), %ymm3, %ymm5
	vpmullw	2816(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vmovdqu	-672(%rcx), %ymm11
	vperm2i128	$32, %ymm6, %ymm2, %ymm15
	vperm2i128	$49, %ymm6, %ymm2, %ymm2
	vpmulhw	1376(%rdx), %ymm2, %ymm6
	vpmullw	2944(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmulhw	1120(%rdx), %ymm9, %ymm12
	vperm2i128	$32, %ymm7, %ymm11, %ymm14
	vperm2i128	$49, %ymm7, %ymm11, %ymm11
	vpmulhw	864(%rdx), %ymm11, %ymm4
	vpmullw	2432(%rdx), %ymm11, %ymm11
	vpmulhw	%ymm0, %ymm11, %ymm11
	vmovdqu	-800(%rcx), %ymm10
	vmovdqu	-928(%rcx), %ymm8
	vpmulhw	1504(%rdx), %ymm15, %ymm1
	vpsubw	%ymm3, %ymm5, %ymm3
	vpmullw	2688(%rdx), %ymm9, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vperm2i128	$32, %ymm10, %ymm8, %ymm13
	vpsubw	%ymm2, %ymm6, %ymm9
	vpmullw	3072(%rdx), %ymm15, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vperm2i128	$49, %ymm10, %ymm8, %ymm8
	vpsubw	%ymm11, %ymm4, %ymm7
	vpmullw	2560(%rdx), %ymm14, %ymm4
	vpmulhw	%ymm0, %ymm4, %ymm4
	vmovdqa	3136(%rdx), %ymm11
	vpsubw	%ymm5, %ymm12, %ymm5
	vpaddw	%ymm3, %ymm5, %ymm2
	vpsubw	%ymm3, %ymm5, %ymm5
	vpmulhw	3328(%rdx), %ymm5, %ymm3
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vpsubw	%ymm6, %ymm1, %ymm6
	vpmulhw	992(%rdx), %ymm14, %ymm1
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	736(%rdx), %ymm8, %ymm1
	vpmullw	2304(%rdx), %ymm8, %ymm8
	vpmulhw	%ymm0, %ymm8, %ymm8
	vpsubw	%ymm5, %ymm3, %ymm12
	vpaddw	%ymm9, %ymm6, %ymm5
	vpsubw	%ymm9, %ymm6, %ymm6
	vpmulhw	3360(%rdx), %ymm6, %ymm3
	vpmullw	3200(%rdx), %ymm6, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm1, %ymm10
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm7, %ymm4, %ymm9
	vpsubw	%ymm7, %ymm4, %ymm4
	vpmulhrsw	%ymm8, %ymm13, %ymm1
	vpsubw	%ymm6, %ymm3, %ymm3
	vmovdqa	3296(%rdx), %ymm6
	vpmulhw	%ymm6, %ymm4, %ymm7
	vpmullw	%ymm4, %ymm11, %ymm4
	vpmullw	%ymm0, %ymm1, %ymm1
	vpmulhw	%ymm0, %ymm4, %ymm4
	vpsubw	%ymm1, %ymm13, %ymm1
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm10, %ymm1, %ymm7
	vpsubw	%ymm10, %ymm1, %ymm1
	vpaddw	%ymm12, %ymm3, %ymm10
	vpsubw	%ymm12, %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm12
	vpmullw	%ymm11, %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm12, %ymm3
	vpaddw	%ymm2, %ymm5, %ymm12
	vpsubw	%ymm2, %ymm5, %ymm2
	vpaddw	%ymm9, %ymm7, %ymm5
	vpmulhw	%ymm6, %ymm2, %ymm6
	vpmullw	%ymm11, %ymm2, %ymm2
	vpaddw	%ymm4, %ymm1, %ymm11
	vpsubw	%ymm4, %ymm1, %ymm1
	vpmulhrsw	%ymm8, %ymm12, %ymm4
	vpsubw	%ymm9, %ymm7, %ymm7
	vpmulhrsw	%ymm8, %ymm5, %ymm8
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmullw	%ymm0, %ymm4, %ymm4
	vpmullw	%ymm0, %ymm8, %ymm0
	vpsubw	%ymm2, %ymm6, %ymm2
	vpsubw	%ymm4, %ymm12, %ymm12
	vpsubw	%ymm0, %ymm5, %ymm0
	vpaddw	%ymm12, %ymm0, %ymm4
	vpsubw	%ymm12, %ymm0, %ymm0
	vmovdqu	%ymm4, -928(%rcx)
	vpaddw	%ymm10, %ymm11, %ymm4
	vpsubw	%ymm10, %ymm11, %ymm10
	vmovdqu	%ymm4, -800(%rcx)
	vpaddw	%ymm2, %ymm7, %ymm4
	vpsubw	%ymm2, %ymm7, %ymm2
	vmovdqu	%ymm4, -672(%rcx)
	vpaddw	%ymm3, %ymm1, %ymm4
	vpsubw	%ymm3, %ymm1, %ymm3
	vmovdqu	%ymm4, -544(%rcx)
	vmovdqu	%ymm0, -416(%rcx)
	vmovdqu	%ymm10, -288(%rcx)
	vmovdqu	%ymm2, -160(%rcx)
	vmovdqu	%ymm3, -32(%rcx)
	cmpq	%r8, %rsi
	jne	.L36
	salq	$10, %rsi
	movq	%rsi, %rcx
	subq	%rdi, %rcx
	addq	%rcx, %rax
	leaq	(%rax,%rsi), %rcx
	.p2align 4,,10
	.p2align 3
.L38:
	vmovdqa	1536(%rdx), %ymm0
	vmovdqu	960(%rax), %ymm5
	addq	$1024, %rax
	vmovdqu	-192(%rax), %ymm3
	vmovdqu	-320(%rax), %ymm6
	vmovdqu	-576(%rax), %ymm7
	vmovdqu	-448(%rax), %ymm2
	vperm2i128	$32, %ymm5, %ymm3, %ymm9
	vperm2i128	$49, %ymm5, %ymm3, %ymm3
	vpmulhw	1216(%rdx), %ymm3, %ymm5
	vpmullw	2784(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vmovdqu	-704(%rax), %ymm11
	vperm2i128	$32, %ymm6, %ymm2, %ymm15
	vperm2i128	$49, %ymm6, %ymm2, %ymm2
	vpmulhw	1344(%rdx), %ymm2, %ymm6
	vpmullw	2912(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmulhw	1088(%rdx), %ymm9, %ymm12
	vperm2i128	$32, %ymm7, %ymm11, %ymm14
	vperm2i128	$49, %ymm7, %ymm11, %ymm11
	vpmulhw	832(%rdx), %ymm11, %ymm4
	vpmullw	2400(%rdx), %ymm11, %ymm11
	vpmulhw	%ymm0, %ymm11, %ymm11
	vmovdqu	-832(%rax), %ymm10
	vmovdqu	-960(%rax), %ymm8
	vpmulhw	1472(%rdx), %ymm15, %ymm1
	vpsubw	%ymm3, %ymm5, %ymm3
	vpmullw	2656(%rdx), %ymm9, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vperm2i128	$32, %ymm10, %ymm8, %ymm13
	vpsubw	%ymm2, %ymm6, %ymm9
	vpmullw	3040(%rdx), %ymm15, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vperm2i128	$49, %ymm10, %ymm8, %ymm8
	vpsubw	%ymm11, %ymm4, %ymm7
	vpmullw	2528(%rdx), %ymm14, %ymm4
	vpmulhw	%ymm0, %ymm4, %ymm4
	vmovdqa	3136(%rdx), %ymm11
	vpsubw	%ymm5, %ymm12, %ymm5
	vpaddw	%ymm3, %ymm5, %ymm2
	vpsubw	%ymm3, %ymm5, %ymm5
	vpmulhw	3328(%rdx), %ymm5, %ymm3
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vpsubw	%ymm6, %ymm1, %ymm6
	vpmulhw	960(%rdx), %ymm14, %ymm1
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	704(%rdx), %ymm8, %ymm1
	vpmullw	2272(%rdx), %ymm8, %ymm8
	vpmulhw	%ymm0, %ymm8, %ymm8
	vpsubw	%ymm5, %ymm3, %ymm12
	vpaddw	%ymm9, %ymm6, %ymm5
	vpsubw	%ymm9, %ymm6, %ymm6
	vpmulhw	3360(%rdx), %ymm6, %ymm3
	vpmullw	3200(%rdx), %ymm6, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm1, %ymm10
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm7, %ymm4, %ymm9
	vpsubw	%ymm7, %ymm4, %ymm4
	vpmulhrsw	%ymm8, %ymm13, %ymm1
	vpsubw	%ymm6, %ymm3, %ymm3
	vmovdqa	3296(%rdx), %ymm6
	vpmulhw	%ymm6, %ymm4, %ymm7
	vpmullw	%ymm4, %ymm11, %ymm4
	vpmullw	%ymm0, %ymm1, %ymm1
	vpmulhw	%ymm0, %ymm4, %ymm4
	vpsubw	%ymm1, %ymm13, %ymm1
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm10, %ymm1, %ymm7
	vpsubw	%ymm10, %ymm1, %ymm1
	vpaddw	%ymm12, %ymm3, %ymm10
	vpsubw	%ymm12, %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm12
	vpmullw	%ymm11, %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm12, %ymm3
	vpaddw	%ymm2, %ymm5, %ymm12
	vpsubw	%ymm2, %ymm5, %ymm2
	vpaddw	%ymm9, %ymm7, %ymm5
	vpmulhw	%ymm6, %ymm2, %ymm6
	vpmullw	%ymm11, %ymm2, %ymm2
	vpaddw	%ymm4, %ymm1, %ymm11
	vpsubw	%ymm4, %ymm1, %ymm1
	vpmulhrsw	%ymm8, %ymm12, %ymm4
	vpsubw	%ymm9, %ymm7, %ymm7
	vpmulhrsw	%ymm8, %ymm5, %ymm8
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmullw	%ymm0, %ymm4, %ymm4
	vpmullw	%ymm0, %ymm8, %ymm0
	vpsubw	%ymm2, %ymm6, %ymm2
	vpsubw	%ymm4, %ymm12, %ymm12
	vpsubw	%ymm0, %ymm5, %ymm0
	vpaddw	%ymm12, %ymm0, %ymm4
	vpsubw	%ymm12, %ymm0, %ymm0
	vmovdqu	%ymm4, -960(%rax)
	vpaddw	%ymm10, %ymm11, %ymm4
	vpsubw	%ymm10, %ymm11, %ymm10
	vmovdqu	%ymm4, -832(%rax)
	vpaddw	%ymm2, %ymm7, %ymm4
	vpsubw	%ymm2, %ymm7, %ymm2
	vmovdqu	%ymm4, -704(%rax)
	vpaddw	%ymm3, %ymm1, %ymm4
	vpsubw	%ymm3, %ymm1, %ymm3
	vmovdqu	%ymm4, -576(%rax)
	vmovdqu	%ymm0, -448(%rax)
	vmovdqu	%ymm10, -320(%rax)
	vmovdqu	%ymm2, -192(%rax)
	vmovdqu	%ymm3, -64(%rax)
	cmpq	%rcx, %rax
	jne	.L38
	addq	%r9, %rax
	leaq	(%rax,%rsi), %rcx
	.p2align 4,,10
	.p2align 3
.L40:
	vmovdqa	1536(%rdx), %ymm0
	vmovdqu	928(%rax), %ymm5
	addq	$1024, %rax
	vmovdqu	-224(%rax), %ymm3
	vmovdqu	-352(%rax), %ymm6
	vmovdqu	-608(%rax), %ymm7
	vmovdqu	-480(%rax), %ymm2
	vperm2i128	$32, %ymm5, %ymm3, %ymm9
	vperm2i128	$49, %ymm5, %ymm3, %ymm3
	vpmulhw	1184(%rdx), %ymm3, %ymm5
	vpmullw	2752(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vmovdqu	-736(%rax), %ymm11
	vperm2i128	$32, %ymm6, %ymm2, %ymm15
	vperm2i128	$49, %ymm6, %ymm2, %ymm2
	vpmulhw	1312(%rdx), %ymm2, %ymm6
	vpmullw	2880(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmulhw	1056(%rdx), %ymm9, %ymm12
	vperm2i128	$32, %ymm7, %ymm11, %ymm14
	vperm2i128	$49, %ymm7, %ymm11, %ymm11
	vpmulhw	800(%rdx), %ymm11, %ymm4
	vpmullw	2368(%rdx), %ymm11, %ymm11
	vpmulhw	%ymm0, %ymm11, %ymm11
	vmovdqu	-864(%rax), %ymm10
	vmovdqu	-992(%rax), %ymm8
	vpmulhw	1440(%rdx), %ymm15, %ymm1
	vpsubw	%ymm3, %ymm5, %ymm3
	vpmullw	2624(%rdx), %ymm9, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vperm2i128	$32, %ymm10, %ymm8, %ymm13
	vpsubw	%ymm2, %ymm6, %ymm9
	vpmullw	3008(%rdx), %ymm15, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vperm2i128	$49, %ymm10, %ymm8, %ymm8
	vpsubw	%ymm11, %ymm4, %ymm7
	vpmullw	2496(%rdx), %ymm14, %ymm4
	vpmulhw	%ymm0, %ymm4, %ymm4
	vmovdqa	3136(%rdx), %ymm11
	vpsubw	%ymm5, %ymm12, %ymm5
	vpaddw	%ymm3, %ymm5, %ymm2
	vpsubw	%ymm3, %ymm5, %ymm5
	vpmulhw	3328(%rdx), %ymm5, %ymm3
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vpsubw	%ymm6, %ymm1, %ymm6
	vpmulhw	928(%rdx), %ymm14, %ymm1
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	672(%rdx), %ymm8, %ymm1
	vpmullw	2240(%rdx), %ymm8, %ymm8
	vpmulhw	%ymm0, %ymm8, %ymm8
	vpsubw	%ymm5, %ymm3, %ymm12
	vpaddw	%ymm9, %ymm6, %ymm5
	vpsubw	%ymm9, %ymm6, %ymm6
	vpmulhw	3360(%rdx), %ymm6, %ymm3
	vpmullw	3200(%rdx), %ymm6, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm1, %ymm10
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm7, %ymm4, %ymm9
	vpsubw	%ymm7, %ymm4, %ymm4
	vpmulhrsw	%ymm8, %ymm13, %ymm1
	vpsubw	%ymm6, %ymm3, %ymm3
	vmovdqa	3296(%rdx), %ymm6
	vpmulhw	%ymm6, %ymm4, %ymm7
	vpmullw	%ymm4, %ymm11, %ymm4
	vpmullw	%ymm0, %ymm1, %ymm1
	vpmulhw	%ymm0, %ymm4, %ymm4
	vpsubw	%ymm1, %ymm13, %ymm1
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm10, %ymm1, %ymm7
	vpsubw	%ymm10, %ymm1, %ymm1
	vpaddw	%ymm12, %ymm3, %ymm10
	vpsubw	%ymm12, %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm12
	vpmullw	%ymm11, %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm12, %ymm3
	vpaddw	%ymm2, %ymm5, %ymm12
	vpsubw	%ymm2, %ymm5, %ymm2
	vpaddw	%ymm9, %ymm7, %ymm5
	vpmulhw	%ymm6, %ymm2, %ymm6
	vpmullw	%ymm11, %ymm2, %ymm2
	vpaddw	%ymm4, %ymm1, %ymm11
	vpsubw	%ymm4, %ymm1, %ymm1
	vpmulhrsw	%ymm8, %ymm12, %ymm4
	vpsubw	%ymm9, %ymm7, %ymm7
	vpmulhrsw	%ymm8, %ymm5, %ymm8
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmullw	%ymm0, %ymm4, %ymm4
	vpmullw	%ymm0, %ymm8, %ymm0
	vpsubw	%ymm2, %ymm6, %ymm2
	vpsubw	%ymm4, %ymm12, %ymm12
	vpsubw	%ymm0, %ymm5, %ymm0
	vpaddw	%ymm12, %ymm0, %ymm4
	vpsubw	%ymm12, %ymm0, %ymm0
	vmovdqu	%ymm4, -992(%rax)
	vpaddw	%ymm10, %ymm11, %ymm4
	vpsubw	%ymm10, %ymm11, %ymm10
	vmovdqu	%ymm4, -864(%rax)
	vpaddw	%ymm2, %ymm7, %ymm4
	vpsubw	%ymm2, %ymm7, %ymm2
	vmovdqu	%ymm4, -736(%rax)
	vpaddw	%ymm3, %ymm1, %ymm4
	vpsubw	%ymm3, %ymm1, %ymm3
	vmovdqu	%ymm4, -608(%rax)
	vmovdqu	%ymm0, -480(%rax)
	vmovdqu	%ymm10, -352(%rax)
	vmovdqu	%ymm2, -224(%rax)
	vmovdqu	%ymm3, -96(%rax)
	cmpq	%rcx, %rax
	jne	.L40
	addq	%r9, %rax
	addq	%rax, %rsi
	.p2align 4,,10
	.p2align 3
.L42:
	vmovdqa	1536(%rdx), %ymm0
	vmovdqu	896(%rax), %ymm5
	addq	$1024, %rax
	vmovdqu	-256(%rax), %ymm3
	vmovdqu	-384(%rax), %ymm6
	vmovdqu	-640(%rax), %ymm7
	vmovdqu	-512(%rax), %ymm2
	vperm2i128	$32, %ymm5, %ymm3, %ymm9
	vperm2i128	$49, %ymm5, %ymm3, %ymm3
	vpmulhw	1152(%rdx), %ymm3, %ymm5
	vpmullw	2720(%rdx), %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vmovdqu	-768(%rax), %ymm11
	vperm2i128	$32, %ymm6, %ymm2, %ymm15
	vperm2i128	$49, %ymm6, %ymm2, %ymm2
	vpmulhw	1280(%rdx), %ymm2, %ymm6
	vpmullw	2848(%rdx), %ymm2, %ymm2
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmulhw	1024(%rdx), %ymm9, %ymm12
	vperm2i128	$32, %ymm7, %ymm11, %ymm14
	vperm2i128	$49, %ymm7, %ymm11, %ymm11
	vpmulhw	768(%rdx), %ymm11, %ymm4
	vpmullw	2336(%rdx), %ymm11, %ymm11
	vpmulhw	%ymm0, %ymm11, %ymm11
	vmovdqu	-896(%rax), %ymm10
	vmovdqu	-1024(%rax), %ymm8
	vpmulhw	1408(%rdx), %ymm15, %ymm1
	vpsubw	%ymm3, %ymm5, %ymm3
	vpmullw	2592(%rdx), %ymm9, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vperm2i128	$32, %ymm10, %ymm8, %ymm13
	vpsubw	%ymm2, %ymm6, %ymm9
	vpmullw	2976(%rdx), %ymm15, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vperm2i128	$49, %ymm10, %ymm8, %ymm8
	vpsubw	%ymm11, %ymm4, %ymm7
	vpmullw	2464(%rdx), %ymm14, %ymm4
	vpmulhw	%ymm0, %ymm4, %ymm4
	vmovdqa	3136(%rdx), %ymm11
	vpsubw	%ymm5, %ymm12, %ymm5
	vpaddw	%ymm3, %ymm5, %ymm2
	vpsubw	%ymm3, %ymm5, %ymm5
	vpmulhw	3328(%rdx), %ymm5, %ymm3
	vpmullw	3168(%rdx), %ymm5, %ymm5
	vpmulhw	%ymm0, %ymm5, %ymm5
	vpsubw	%ymm6, %ymm1, %ymm6
	vpmulhw	896(%rdx), %ymm14, %ymm1
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	640(%rdx), %ymm8, %ymm1
	vpmullw	2208(%rdx), %ymm8, %ymm8
	vpmulhw	%ymm0, %ymm8, %ymm8
	vpsubw	%ymm5, %ymm3, %ymm12
	vpaddw	%ymm9, %ymm6, %ymm5
	vpsubw	%ymm9, %ymm6, %ymm6
	vpmulhw	3360(%rdx), %ymm6, %ymm3
	vpmullw	3200(%rdx), %ymm6, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm1, %ymm10
	vmovdqa	3232(%rdx), %ymm8
	vpaddw	%ymm7, %ymm4, %ymm9
	vpsubw	%ymm7, %ymm4, %ymm4
	vpmulhrsw	%ymm8, %ymm13, %ymm1
	vpsubw	%ymm6, %ymm3, %ymm3
	vmovdqa	3296(%rdx), %ymm6
	vpmulhw	%ymm6, %ymm4, %ymm7
	vpmullw	%ymm4, %ymm11, %ymm4
	vpmullw	%ymm0, %ymm1, %ymm1
	vpmulhw	%ymm0, %ymm4, %ymm4
	vpsubw	%ymm1, %ymm13, %ymm1
	vpsubw	%ymm4, %ymm7, %ymm4
	vpaddw	%ymm10, %ymm1, %ymm7
	vpsubw	%ymm10, %ymm1, %ymm1
	vpaddw	%ymm12, %ymm3, %ymm10
	vpsubw	%ymm12, %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm12
	vpmullw	%ymm11, %ymm3, %ymm3
	vpmulhw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm12, %ymm3
	vpaddw	%ymm2, %ymm5, %ymm12
	vpsubw	%ymm2, %ymm5, %ymm2
	vpaddw	%ymm9, %ymm7, %ymm5
	vpmulhw	%ymm6, %ymm2, %ymm6
	vpmullw	%ymm11, %ymm2, %ymm2
	vpaddw	%ymm4, %ymm1, %ymm11
	vpsubw	%ymm4, %ymm1, %ymm1
	vpmulhrsw	%ymm8, %ymm12, %ymm4
	vpsubw	%ymm9, %ymm7, %ymm7
	vpmulhrsw	%ymm8, %ymm5, %ymm8
	vpmulhw	%ymm0, %ymm2, %ymm2
	vpmullw	%ymm0, %ymm4, %ymm4
	vpmullw	%ymm0, %ymm8, %ymm0
	vpsubw	%ymm2, %ymm6, %ymm2
	vpsubw	%ymm4, %ymm12, %ymm12
	vpsubw	%ymm0, %ymm5, %ymm0
	vpaddw	%ymm12, %ymm0, %ymm4
	vpsubw	%ymm12, %ymm0, %ymm0
	vmovdqu	%ymm4, -1024(%rax)
	vpaddw	%ymm10, %ymm11, %ymm4
	vpsubw	%ymm10, %ymm11, %ymm10
	vmovdqu	%ymm4, -896(%rax)
	vpaddw	%ymm2, %ymm7, %ymm4
	vpsubw	%ymm2, %ymm7, %ymm2
	vmovdqu	%ymm4, -768(%rax)
	vpaddw	%ymm3, %ymm1, %ymm4
	vpsubw	%ymm3, %ymm1, %ymm3
	vmovdqu	%ymm4, -640(%rax)
	vmovdqu	%ymm0, -512(%rax)
	vmovdqu	%ymm10, -384(%rax)
	vmovdqu	%ymm2, -256(%rax)
	vmovdqu	%ymm3, -128(%rax)
	cmpq	%rsi, %rax
	jne	.L42
.L54:
	vzeroupper
	ret
	.p2align 4,,10
	.p2align 3
.L31:
	leaq	(%rdi,%r11), %rax
	jmp	.L43
	.cfi_endproc
.LFE4758:
	.size	invntt512, .-invntt512
	.p2align 4,,15
	.globl	CRYPTO_NAMESPACE(ntt512_7681)
	.type	CRYPTO_NAMESPACE(ntt512_7681), @function
CRYPTO_NAMESPACE(ntt512_7681):
.LFB4756:
	.cfi_startproc
	leaq	qdata_7681(%rip), %rdx
	jmp	ntt512
	.cfi_endproc
.LFE4756:
	.size	CRYPTO_NAMESPACE(ntt512_7681), .-CRYPTO_NAMESPACE(ntt512_7681)
	.p2align 4,,15
	.globl	CRYPTO_NAMESPACE(ntt512_10753)
	.type	CRYPTO_NAMESPACE(ntt512_10753), @function
CRYPTO_NAMESPACE(ntt512_10753):
.LFB4757:
	.cfi_startproc
	leaq	qdata_10753(%rip), %rdx
	jmp	ntt512
	.cfi_endproc
.LFE4757:
	.size	CRYPTO_NAMESPACE(ntt512_10753), .-CRYPTO_NAMESPACE(ntt512_10753)
	.p2align 4,,15
	.globl	CRYPTO_NAMESPACE(invntt512_7681)
	.type	CRYPTO_NAMESPACE(invntt512_7681), @function
CRYPTO_NAMESPACE(invntt512_7681):
.LFB4759:
	.cfi_startproc
	leaq	qdata_7681(%rip), %rdx
	jmp	invntt512
	.cfi_endproc
.LFE4759:
	.size	CRYPTO_NAMESPACE(invntt512_7681), .-CRYPTO_NAMESPACE(invntt512_7681)
	.p2align 4,,15
	.globl	CRYPTO_NAMESPACE(invntt512_10753)
	.type	CRYPTO_NAMESPACE(invntt512_10753), @function
CRYPTO_NAMESPACE(invntt512_10753):
.LFB4760:
	.cfi_startproc
	leaq	qdata_10753(%rip), %rdx
	jmp	invntt512
	.cfi_endproc
.LFE4760:
	.size	CRYPTO_NAMESPACE(invntt512_10753), .-CRYPTO_NAMESPACE(invntt512_10753)
	.section	.rodata
	.align 32
	.type	qdata_10753, @object
	.size	qdata_10753, 3392
qdata_10753:
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-4188
	.value	-4188
	.value	-4188
	.value	-4188
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-4188
	.value	-4188
	.value	-4188
	.value	-4188
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	223
	.value	223
	.value	223
	.value	223
	.value	-3688
	.value	-3688
	.value	-3688
	.value	-3688
	.value	223
	.value	223
	.value	223
	.value	223
	.value	-3688
	.value	-3688
	.value	-3688
	.value	-3688
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	-376
	.value	-376
	.value	-376
	.value	-376
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	-376
	.value	-376
	.value	-376
	.value	-376
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	-3686
	.value	-3686
	.value	-3686
	.value	-3686
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	-3686
	.value	-3686
	.value	-3686
	.value	-3686
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	-2413
	.value	-2413
	.value	-2413
	.value	-2413
	.value	1018
	.value	1018
	.value	1018
	.value	1018
	.value	-2413
	.value	-2413
	.value	-2413
	.value	-2413
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	-357
	.value	-357
	.value	-357
	.value	-357
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	-357
	.value	-357
	.value	-357
	.value	-357
	.value	1018
	.value	-3364
	.value	-376
	.value	4855
	.value	3688
	.value	425
	.value	-3686
	.value	2695
	.value	1018
	.value	-3364
	.value	-376
	.value	4855
	.value	3688
	.value	425
	.value	-3686
	.value	2695
	.value	-223
	.value	-3784
	.value	357
	.value	-2236
	.value	-4188
	.value	4544
	.value	2413
	.value	730
	.value	-223
	.value	-3784
	.value	357
	.value	-2236
	.value	-4188
	.value	4544
	.value	2413
	.value	730
	.value	1018
	.value	-730
	.value	-2413
	.value	-4544
	.value	4188
	.value	2236
	.value	-357
	.value	3784
	.value	1018
	.value	-730
	.value	-2413
	.value	-4544
	.value	4188
	.value	2236
	.value	-357
	.value	3784
	.value	223
	.value	-2695
	.value	3686
	.value	-425
	.value	-3688
	.value	-4855
	.value	376
	.value	3364
	.value	223
	.value	-2695
	.value	3686
	.value	-425
	.value	-3688
	.value	-4855
	.value	376
	.value	3364
	.value	1018
	.value	-5175
	.value	-3364
	.value	2503
	.value	-376
	.value	1341
	.value	4855
	.value	-4875
	.value	1018
	.value	-5175
	.value	-3364
	.value	2503
	.value	-376
	.value	1341
	.value	4855
	.value	-4875
	.value	3688
	.value	-2629
	.value	425
	.value	-4347
	.value	-3686
	.value	3823
	.value	2695
	.value	-4035
	.value	3688
	.value	-2629
	.value	425
	.value	-4347
	.value	-3686
	.value	3823
	.value	2695
	.value	-4035
	.value	1018
	.value	5063
	.value	-730
	.value	341
	.value	-2413
	.value	-3012
	.value	-4544
	.value	-5213
	.value	1018
	.value	5063
	.value	-730
	.value	341
	.value	-2413
	.value	-3012
	.value	-4544
	.value	-5213
	.value	4188
	.value	1520
	.value	2236
	.value	1931
	.value	-357
	.value	918
	.value	3784
	.value	4095
	.value	4188
	.value	1520
	.value	2236
	.value	1931
	.value	-357
	.value	918
	.value	3784
	.value	4095
	.value	1018
	.value	3085
	.value	-5175
	.value	2982
	.value	-3364
	.value	-4744
	.value	2503
	.value	-4129
	.value	-376
	.value	-2576
	.value	1341
	.value	-193
	.value	4855
	.value	3062
	.value	-4875
	.value	4
	.value	3688
	.value	2388
	.value	-2629
	.value	-4513
	.value	425
	.value	4742
	.value	-4347
	.value	2935
	.value	-3686
	.value	-544
	.value	3823
	.value	-2178
	.value	2695
	.value	847
	.value	-4035
	.value	268
	.value	-223
	.value	-1299
	.value	-4095
	.value	-1287
	.value	-3784
	.value	-4876
	.value	-918
	.value	3091
	.value	357
	.value	-4189
	.value	-1931
	.value	4616
	.value	-2236
	.value	2984
	.value	-1520
	.value	-3550
	.value	-4188
	.value	-1009
	.value	5213
	.value	-205
	.value	4544
	.value	-4102
	.value	3012
	.value	2790
	.value	2413
	.value	-1085
	.value	-341
	.value	-2565
	.value	730
	.value	-4379
	.value	-5063
	.value	-1284
	.value	1018
	.value	1284
	.value	5063
	.value	4379
	.value	-730
	.value	2565
	.value	341
	.value	1085
	.value	-2413
	.value	-2790
	.value	-3012
	.value	4102
	.value	-4544
	.value	205
	.value	-5213
	.value	1009
	.value	4188
	.value	3550
	.value	1520
	.value	-2984
	.value	2236
	.value	-4616
	.value	1931
	.value	4189
	.value	-357
	.value	-3091
	.value	918
	.value	4876
	.value	3784
	.value	1287
	.value	4095
	.value	1299
	.value	223
	.value	-268
	.value	4035
	.value	-847
	.value	-2695
	.value	2178
	.value	-3823
	.value	544
	.value	3686
	.value	-2935
	.value	4347
	.value	-4742
	.value	-425
	.value	4513
	.value	2629
	.value	-2388
	.value	-3688
	.value	-4
	.value	4875
	.value	-3062
	.value	-4855
	.value	193
	.value	-1341
	.value	2576
	.value	376
	.value	4129
	.value	-2503
	.value	4744
	.value	3364
	.value	-2982
	.value	5175
	.value	-3085
	.value	1018
	.value	5116
	.value	3085
	.value	-3615
	.value	-5175
	.value	400
	.value	2982
	.value	3198
	.value	-3364
	.value	2234
	.value	-4744
	.value	-4828
	.value	2503
	.value	326
	.value	-4129
	.value	-512
	.value	-376
	.value	1068
	.value	-2576
	.value	-4580
	.value	1341
	.value	3169
	.value	-193
	.value	-2998
	.value	4855
	.value	-635
	.value	3062
	.value	-4808
	.value	-4875
	.value	-2740
	.value	4
	.value	675
	.value	3688
	.value	-1324
	.value	2388
	.value	5114
	.value	-2629
	.value	5294
	.value	-4513
	.value	-794
	.value	425
	.value	-864
	.value	4742
	.value	-886
	.value	-4347
	.value	336
	.value	2935
	.value	-2045
	.value	-3686
	.value	-3715
	.value	-544
	.value	4977
	.value	3823
	.value	-2737
	.value	-2178
	.value	3441
	.value	2695
	.value	467
	.value	847
	.value	454
	.value	-4035
	.value	-779
	.value	268
	.value	2213
	.value	1018
	.value	1615
	.value	1284
	.value	2206
	.value	5063
	.value	5064
	.value	4379
	.value	472
	.value	-730
	.value	-5341
	.value	2565
	.value	-4286
	.value	341
	.value	2981
	.value	1085
	.value	-1268
	.value	-2413
	.value	-3057
	.value	-2790
	.value	-2884
	.value	-3012
	.value	-1356
	.value	4102
	.value	-3337
	.value	-4544
	.value	5023
	.value	205
	.value	-636
	.value	-5213
	.value	909
	.value	1009
	.value	-2973
	.value	4188
	.value	2271
	.value	3550
	.value	-1572
	.value	1520
	.value	1841
	.value	-2984
	.value	970
	.value	2236
	.value	-4734
	.value	-4616
	.value	578
	.value	1931
	.value	-116
	.value	4189
	.value	1586
	.value	-357
	.value	-2774
	.value	-3091
	.value	-1006
	.value	918
	.value	-5156
	.value	4876
	.value	4123
	.value	3784
	.value	-567
	.value	1287
	.value	151
	.value	4095
	.value	1458
	.value	1299
	.value	2684
	.value	1018
	.value	-3260
	.value	5116
	.value	-1722
	.value	3085
	.value	5120
	.value	-3615
	.value	3760
	.value	-5175
	.value	73
	.value	400
	.value	4254
	.value	2982
	.value	2788
	.value	3198
	.value	-2657
	.value	-3364
	.value	569
	.value	2234
	.value	1930
	.value	-4744
	.value	-2279
	.value	-4828
	.value	5215
	.value	2503
	.value	-4403
	.value	326
	.value	1639
	.value	-4129
	.value	5068
	.value	-512
	.value	-5015
	.value	-376
	.value	-4859
	.value	1068
	.value	-40
	.value	-2576
	.value	4003
	.value	-4580
	.value	-4621
	.value	1341
	.value	2487
	.value	3169
	.value	-2374
	.value	-193
	.value	2625
	.value	-2998
	.value	4784
	.value	4855
	.value	825
	.value	-635
	.value	2118
	.value	3062
	.value	-2813
	.value	-4808
	.value	-4250
	.value	-4875
	.value	-2113
	.value	-2740
	.value	-4408
	.value	4
	.value	-1893
	.value	675
	.value	458
	.value	1018
	.value	5120
	.value	400
	.value	-2657
	.value	-4744
	.value	-4403
	.value	-512
	.value	-40
	.value	1341
	.value	2625
	.value	-635
	.value	-4250
	.value	4
	.value	-3360
	.value	5114
	.value	-5313
	.value	425
	.value	-2151
	.value	336
	.value	-2662
	.value	-544
	.value	5334
	.value	3441
	.value	2117
	.value	-4035
	.value	2205
	.value	-2684
	.value	-3570
	.value	-1287
	.value	-4973
	.value	5156
	.value	2419
	.value	357
	.value	1204
	.value	-578
	.value	1635
	.value	2984
	.value	-1111
	.value	-2271
	.value	4359
	.value	5213
	.value	-2449
	.value	3337
	.value	3453
	.value	2790
	.value	554
	.value	-2981
	.value	-1409
	.value	730
	.value	-279
	.value	-2206
	.value	3524
	.value	-3085
	.value	-73
	.value	-3198
	.value	-1930
	.value	-2503
	.value	-5068
	.value	-1068
	.value	4621
	.value	193
	.value	-825
	.value	4808
	.value	4408
	.value	1018
	.value	4428
	.value	5064
	.value	-4000
	.value	2565
	.value	573
	.value	-1268
	.value	3125
	.value	-3012
	.value	-4144
	.value	5023
	.value	1927
	.value	1009
	.value	-2139
	.value	-1572
	.value	3535
	.value	2236
	.value	663
	.value	-116
	.value	4967
	.value	-3091
	.value	-854
	.value	4123
	.value	1160
	.value	4095
	.value	-1349
	.value	-2213
	.value	1782
	.value	-847
	.value	2062
	.value	2737
	.value	624
	.value	3686
	.value	-2283
	.value	886
	.value	4889
	.value	4513
	.value	-4601
	.value	1324
	.value	1893
	.value	4875
	.value	-2118
	.value	2998
	.value	-2487
	.value	2576
	.value	5015
	.value	-326
	.value	2279
	.value	3364
	.value	-4254
	.value	3615
	.value	3260
	.value	-1284
	.value	-1381
	.value	-472
	.value	-3891
	.value	-341
	.value	2087
	.value	3057
	.value	4720
	.value	-4102
	.value	3410
	.value	636
	.value	1689
	.value	1018
	.value	-3524
	.value	1615
	.value	5268
	.value	1284
	.value	4428
	.value	2206
	.value	-834
	.value	5063
	.value	1381
	.value	5064
	.value	279
	.value	4379
	.value	2439
	.value	472
	.value	-4000
	.value	-730
	.value	-2015
	.value	-5341
	.value	3891
	.value	2565
	.value	1409
	.value	-4286
	.value	2605
	.value	341
	.value	573
	.value	2981
	.value	5356
	.value	1085
	.value	-2087
	.value	-1268
	.value	-554
	.value	-2413
	.value	3135
	.value	-3057
	.value	3125
	.value	-2790
	.value	-778
	.value	-2884
	.value	-4720
	.value	-3012
	.value	-3453
	.value	-1356
	.value	-355
	.value	4102
	.value	-4144
	.value	-3337
	.value	-152
	.value	-4544
	.value	-3410
	.value	5023
	.value	2449
	.value	205
	.value	-97
	.value	-636
	.value	1927
	.value	-5213
	.value	2624
	.value	909
	.value	-1689
	.value	1009
	.value	-4359
	.value	-2973
	.value	-3419
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	10753
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	1956
	.value	1956
	.value	1956
	.value	1956
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	1956
	.value	1956
	.value	1956
	.value	1956
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	408
	.value	408
	.value	408
	.value	408
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	408
	.value	408
	.value	408
	.value	408
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-20856
	.value	-20856
	.value	-20856
	.value	-20856
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-20856
	.value	-20856
	.value	-20856
	.value	-20856
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-21094
	.value	-21094
	.value	-21094
	.value	-21094
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-21094
	.value	-21094
	.value	-21094
	.value	-21094
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-10093
	.value	-10093
	.value	-10093
	.value	-10093
	.value	-6
	.value	-6
	.value	-6
	.value	-6
	.value	-10093
	.value	-10093
	.value	-10093
	.value	-10093
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-28517
	.value	-28517
	.value	-28517
	.value	-28517
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-28517
	.value	-28517
	.value	-28517
	.value	-28517
	.value	-6
	.value	-9508
	.value	-20856
	.value	-29449
	.value	-408
	.value	18345
	.value	-21094
	.value	-7033
	.value	-6
	.value	-9508
	.value	-20856
	.value	-29449
	.value	-408
	.value	18345
	.value	-21094
	.value	-7033
	.value	-27359
	.value	-16072
	.value	28517
	.value	-12476
	.value	1956
	.value	-28224
	.value	10093
	.value	16090
	.value	-27359
	.value	-16072
	.value	28517
	.value	-12476
	.value	1956
	.value	-28224
	.value	10093
	.value	16090
	.value	-6
	.value	-16090
	.value	-10093
	.value	28224
	.value	-1956
	.value	12476
	.value	-28517
	.value	16072
	.value	-6
	.value	-16090
	.value	-10093
	.value	28224
	.value	-1956
	.value	12476
	.value	-28517
	.value	16072
	.value	27359
	.value	7033
	.value	21094
	.value	-18345
	.value	408
	.value	29449
	.value	20856
	.value	9508
	.value	27359
	.value	7033
	.value	21094
	.value	-18345
	.value	408
	.value	29449
	.value	20856
	.value	9508
	.value	-6
	.value	-3639
	.value	-9508
	.value	25543
	.value	-20856
	.value	829
	.value	-29449
	.value	-17675
	.value	-6
	.value	-3639
	.value	-9508
	.value	25543
	.value	-20856
	.value	829
	.value	-29449
	.value	-17675
	.value	-408
	.value	18363
	.value	18345
	.value	7429
	.value	-21094
	.value	-10001
	.value	-7033
	.value	-4547
	.value	-408
	.value	18363
	.value	18345
	.value	7429
	.value	-21094
	.value	-10001
	.value	-7033
	.value	-4547
	.value	-6
	.value	28103
	.value	-16090
	.value	3925
	.value	-10093
	.value	7228
	.value	28224
	.value	11683
	.value	-6
	.value	28103
	.value	-16090
	.value	3925
	.value	-10093
	.value	7228
	.value	28224
	.value	11683
	.value	-1956
	.value	-23056
	.value	12476
	.value	14731
	.value	-28517
	.value	26518
	.value	16072
	.value	14847
	.value	-1956
	.value	-23056
	.value	12476
	.value	14731
	.value	-28517
	.value	26518
	.value	16072
	.value	14847
	.value	-6
	.value	-5619
	.value	-3639
	.value	-12378
	.value	-9508
	.value	15736
	.value	25543
	.value	23007
	.value	-20856
	.value	-27152
	.value	829
	.value	-22209
	.value	-29449
	.value	-20490
	.value	-17675
	.value	22532
	.value	-408
	.value	16724
	.value	18363
	.value	22623
	.value	18345
	.value	5766
	.value	7429
	.value	-31369
	.value	-21094
	.value	15840
	.value	-10001
	.value	19326
	.value	-7033
	.value	3407
	.value	-4547
	.value	2316
	.value	-27359
	.value	6381
	.value	-14847
	.value	8441
	.value	-16072
	.value	-6924
	.value	-26518
	.value	-4589
	.value	28517
	.value	12707
	.value	-14731
	.value	-15864
	.value	-12476
	.value	31656
	.value	23056
	.value	24098
	.value	1956
	.value	-31217
	.value	-11683
	.value	-24269
	.value	-28224
	.value	-5126
	.value	-7228
	.value	20198
	.value	10093
	.value	-573
	.value	-3925
	.value	-14341
	.value	16090
	.value	23781
	.value	-28103
	.value	-23812
	.value	-6
	.value	23812
	.value	28103
	.value	-23781
	.value	-16090
	.value	14341
	.value	3925
	.value	573
	.value	-10093
	.value	-20198
	.value	7228
	.value	5126
	.value	28224
	.value	24269
	.value	11683
	.value	31217
	.value	-1956
	.value	-24098
	.value	-23056
	.value	-31656
	.value	12476
	.value	15864
	.value	14731
	.value	-12707
	.value	-28517
	.value	4589
	.value	26518
	.value	6924
	.value	16072
	.value	-8441
	.value	14847
	.value	-6381
	.value	27359
	.value	-2316
	.value	4547
	.value	-3407
	.value	7033
	.value	-19326
	.value	10001
	.value	-15840
	.value	21094
	.value	31369
	.value	-7429
	.value	-5766
	.value	-18345
	.value	-22623
	.value	-18363
	.value	-16724
	.value	408
	.value	-22532
	.value	17675
	.value	20490
	.value	29449
	.value	22209
	.value	-829
	.value	27152
	.value	20856
	.value	-23007
	.value	-25543
	.value	-15736
	.value	9508
	.value	12378
	.value	3639
	.value	5619
	.value	-6
	.value	-17412
	.value	-5619
	.value	2017
	.value	-3639
	.value	24976
	.value	-12378
	.value	24702
	.value	-9508
	.value	-31558
	.value	15736
	.value	1316
	.value	25543
	.value	-31418
	.value	23007
	.value	-512
	.value	-20856
	.value	-13268
	.value	-27152
	.value	22044
	.value	829
	.value	8801
	.value	-22209
	.value	-12214
	.value	-29449
	.value	11141
	.value	-20490
	.value	-17096
	.value	-17675
	.value	32076
	.value	22532
	.value	17571
	.value	-408
	.value	13012
	.value	16724
	.value	4090
	.value	18363
	.value	-30546
	.value	22623
	.value	16614
	.value	18345
	.value	-17248
	.value	5766
	.value	22666
	.value	7429
	.value	-7856
	.value	-31369
	.value	31235
	.value	-21094
	.value	28541
	.value	15840
	.value	-30351
	.value	-10001
	.value	-177
	.value	19326
	.value	-31887
	.value	-7033
	.value	25555
	.value	3407
	.value	-31290
	.value	-4547
	.value	-13579
	.value	2316
	.value	-2395
	.value	-6
	.value	4175
	.value	23812
	.value	7326
	.value	28103
	.value	17352
	.value	-23781
	.value	-28200
	.value	-16090
	.value	11555
	.value	14341
	.value	6978
	.value	3925
	.value	-1627
	.value	573
	.value	780
	.value	-10093
	.value	32271
	.value	-20198
	.value	7356
	.value	7228
	.value	29364
	.value	5126
	.value	27895
	.value	28224
	.value	-609
	.value	24269
	.value	21892
	.value	11683
	.value	-7795
	.value	31217
	.value	-18845
	.value	-1956
	.value	29407
	.value	-24098
	.value	-7716
	.value	-23056
	.value	-719
	.value	-31656
	.value	-8246
	.value	12476
	.value	-26238
	.value	15864
	.value	11842
	.value	14731
	.value	1932
	.value	-12707
	.value	-11726
	.value	-28517
	.value	4394
	.value	4589
	.value	2066
	.value	26518
	.value	-11300
	.value	6924
	.value	-24037
	.value	16072
	.value	969
	.value	-8441
	.value	14999
	.value	14847
	.value	-11854
	.value	-6381
	.value	-19844
	.value	-6
	.value	-13500
	.value	-17412
	.value	32070
	.value	-5619
	.value	5120
	.value	2017
	.value	11952
	.value	-3639
	.value	1609
	.value	24976
	.value	9374
	.value	-12378
	.value	-23836
	.value	24702
	.value	-8289
	.value	-9508
	.value	-22471
	.value	-31558
	.value	25482
	.value	15736
	.value	-8935
	.value	1316
	.value	32351
	.value	25543
	.value	19661
	.value	-31418
	.value	8295
	.value	23007
	.value	-25652
	.value	-512
	.value	-19863
	.value	-20856
	.value	6917
	.value	-13268
	.value	-28712
	.value	-27152
	.value	20899
	.value	22044
	.value	4083
	.value	829
	.value	951
	.value	8801
	.value	29370
	.value	-22209
	.value	24641
	.value	-12214
	.value	12976
	.value	-29449
	.value	-22215
	.value	11141
	.value	-29626
	.value	-20490
	.value	30467
	.value	-17096
	.value	13158
	.value	-17675
	.value	-24129
	.value	32076
	.value	7880
	.value	22532
	.value	-30053
	.value	17571
	.value	-8758
	.value	-6
	.value	5120
	.value	24976
	.value	-8289
	.value	15736
	.value	19661
	.value	-512
	.value	-28712
	.value	829
	.value	24641
	.value	11141
	.value	13158
	.value	22532
	.value	13024
	.value	4090
	.value	-27329
	.value	18345
	.value	-8807
	.value	-7856
	.value	-20070
	.value	15840
	.value	-1834
	.value	-31887
	.value	-18875
	.value	-4547
	.value	18077
	.value	19844
	.value	-23026
	.value	8441
	.value	-12653
	.value	11300
	.value	11123
	.value	28517
	.value	31924
	.value	-11842
	.value	-14237
	.value	31656
	.value	16809
	.value	-29407
	.value	-5369
	.value	-11683
	.value	-16273
	.value	-27895
	.value	-29827
	.value	20198
	.value	7722
	.value	1627
	.value	9343
	.value	16090
	.value	-15127
	.value	-7326
	.value	-6716
	.value	5619
	.value	-1609
	.value	-24702
	.value	-25482
	.value	-25543
	.value	25652
	.value	13268
	.value	-4083
	.value	22209
	.value	22215
	.value	17096
	.value	-7880
	.value	-6
	.value	-26292
	.value	17352
	.value	12384
	.value	14341
	.value	61
	.value	780
	.value	23093
	.value	7228
	.value	-12336
	.value	-609
	.value	-7801
	.value	31217
	.value	-6747
	.value	-7716
	.value	6095
	.value	12476
	.value	15511
	.value	1932
	.value	11623
	.value	4589
	.value	6314
	.value	-24037
	.value	-19320
	.value	14847
	.value	19643
	.value	2395
	.value	-21770
	.value	-3407
	.value	-17394
	.value	177
	.value	-23952
	.value	21094
	.value	-31467
	.value	-22666
	.value	-1767
	.value	-22623
	.value	-14329
	.value	-13012
	.value	30053
	.value	17675
	.value	29626
	.value	12214
	.value	-951
	.value	27152
	.value	19863
	.value	31418
	.value	8935
	.value	9508
	.value	-9374
	.value	-2017
	.value	13500
	.value	-23812
	.value	-29541
	.value	28200
	.value	20173
	.value	-3925
	.value	-24025
	.value	-32271
	.value	-19856
	.value	-5126
	.value	-26286
	.value	-21892
	.value	-4967
	.value	-6
	.value	6716
	.value	4175
	.value	-13164
	.value	23812
	.value	-26292
	.value	7326
	.value	-12098
	.value	28103
	.value	29541
	.value	17352
	.value	15127
	.value	-23781
	.value	-7289
	.value	-28200
	.value	12384
	.value	-16090
	.value	-29151
	.value	11555
	.value	-20173
	.value	14341
	.value	-9343
	.value	6978
	.value	-22483
	.value	3925
	.value	61
	.value	-1627
	.value	23788
	.value	573
	.value	24025
	.value	780
	.value	-7722
	.value	-10093
	.value	-18881
	.value	32271
	.value	23093
	.value	-20198
	.value	-24330
	.value	7356
	.value	19856
	.value	7228
	.value	29827
	.value	29364
	.value	15517
	.value	5126
	.value	-12336
	.value	27895
	.value	-4248
	.value	28224
	.value	26286
	.value	-609
	.value	16273
	.value	24269
	.value	-5729
	.value	21892
	.value	-7801
	.value	11683
	.value	-30144
	.value	-7795
	.value	4967
	.value	31217
	.value	5369
	.value	-18845
	.value	-8027
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	-27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	27359
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-408
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	-1956
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	3
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	-223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	223
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	3688
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.value	4188
	.align 32
	.type	qdata_7681, @object
	.size	qdata_7681, 3392
qdata_7681:
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	3182
	.value	3182
	.value	3182
	.value	3182
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	3182
	.value	3182
	.value	3182
	.value	3182
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3625
	.value	3625
	.value	3625
	.value	3625
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3625
	.value	3625
	.value	3625
	.value	3625
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	2194
	.value	2194
	.value	2194
	.value	2194
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	2194
	.value	2194
	.value	2194
	.value	2194
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-1100
	.value	-1100
	.value	-1100
	.value	-1100
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-1100
	.value	-1100
	.value	-1100
	.value	-1100
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	3696
	.value	3696
	.value	3696
	.value	3696
	.value	-3593
	.value	-3593
	.value	-3593
	.value	-3593
	.value	3696
	.value	3696
	.value	3696
	.value	3696
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-2456
	.value	-2456
	.value	-2456
	.value	-2456
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-2456
	.value	-2456
	.value	-2456
	.value	-2456
	.value	-3593
	.value	1701
	.value	2194
	.value	834
	.value	-3625
	.value	2319
	.value	-1100
	.value	121
	.value	-3593
	.value	1701
	.value	2194
	.value	834
	.value	-3625
	.value	2319
	.value	-1100
	.value	121
	.value	-3777
	.value	1414
	.value	2456
	.value	2495
	.value	3182
	.value	2876
	.value	-3696
	.value	2250
	.value	-3777
	.value	1414
	.value	2456
	.value	2495
	.value	3182
	.value	2876
	.value	-3696
	.value	2250
	.value	-3593
	.value	-2250
	.value	3696
	.value	-2876
	.value	-3182
	.value	-2495
	.value	-2456
	.value	-1414
	.value	-3593
	.value	-2250
	.value	3696
	.value	-2876
	.value	-3182
	.value	-2495
	.value	-2456
	.value	-1414
	.value	3777
	.value	-121
	.value	1100
	.value	-2319
	.value	3625
	.value	-834
	.value	-2194
	.value	-1701
	.value	3777
	.value	-121
	.value	1100
	.value	-2319
	.value	3625
	.value	-834
	.value	-2194
	.value	-1701
	.value	-3593
	.value	3364
	.value	1701
	.value	-1599
	.value	2194
	.value	2557
	.value	834
	.value	-2816
	.value	-3593
	.value	3364
	.value	1701
	.value	-1599
	.value	2194
	.value	2557
	.value	834
	.value	-2816
	.value	-3625
	.value	617
	.value	2319
	.value	2006
	.value	-1100
	.value	-1296
	.value	121
	.value	1986
	.value	-3625
	.value	617
	.value	2319
	.value	2006
	.value	-1100
	.value	-1296
	.value	121
	.value	1986
	.value	-3593
	.value	2237
	.value	-2250
	.value	-1483
	.value	3696
	.value	3706
	.value	-2876
	.value	1921
	.value	-3593
	.value	2237
	.value	-2250
	.value	-1483
	.value	3696
	.value	3706
	.value	-2876
	.value	1921
	.value	-3182
	.value	2088
	.value	-2495
	.value	-1525
	.value	-2456
	.value	1993
	.value	-1414
	.value	2830
	.value	-3182
	.value	2088
	.value	-2495
	.value	-1525
	.value	-2456
	.value	1993
	.value	-1414
	.value	2830
	.value	-3593
	.value	514
	.value	3364
	.value	438
	.value	1701
	.value	2555
	.value	-1599
	.value	-1738
	.value	2194
	.value	103
	.value	2557
	.value	1881
	.value	834
	.value	-549
	.value	-2816
	.value	638
	.value	-3625
	.value	-1399
	.value	617
	.value	-1760
	.value	2319
	.value	2535
	.value	2006
	.value	3266
	.value	-1100
	.value	-1431
	.value	-1296
	.value	3174
	.value	121
	.value	3153
	.value	1986
	.value	-810
	.value	-3777
	.value	2956
	.value	-2830
	.value	-679
	.value	1414
	.value	2440
	.value	-1993
	.value	-3689
	.value	2456
	.value	2804
	.value	1525
	.value	3555
	.value	2495
	.value	1535
	.value	-2088
	.value	-7
	.value	3182
	.value	-1321
	.value	-1921
	.value	-1305
	.value	2876
	.value	-3772
	.value	-3706
	.value	3600
	.value	-3696
	.value	-2043
	.value	1483
	.value	-396
	.value	2250
	.value	-2310
	.value	-2237
	.value	1887
	.value	-3593
	.value	-1887
	.value	2237
	.value	2310
	.value	-2250
	.value	396
	.value	-1483
	.value	2043
	.value	3696
	.value	-3600
	.value	3706
	.value	3772
	.value	-2876
	.value	1305
	.value	1921
	.value	1321
	.value	-3182
	.value	7
	.value	2088
	.value	-1535
	.value	-2495
	.value	-3555
	.value	-1525
	.value	-2804
	.value	-2456
	.value	3689
	.value	1993
	.value	-2440
	.value	-1414
	.value	679
	.value	2830
	.value	-2956
	.value	3777
	.value	810
	.value	-1986
	.value	-3153
	.value	-121
	.value	-3174
	.value	1296
	.value	1431
	.value	1100
	.value	-3266
	.value	-2006
	.value	-2535
	.value	-2319
	.value	1760
	.value	-617
	.value	1399
	.value	3625
	.value	-638
	.value	2816
	.value	549
	.value	-834
	.value	-1881
	.value	-2557
	.value	-103
	.value	-2194
	.value	1738
	.value	1599
	.value	-2555
	.value	-1701
	.value	-438
	.value	-3364
	.value	-514
	.value	-3593
	.value	-1532
	.value	514
	.value	-373
	.value	3364
	.value	-3816
	.value	438
	.value	-3456
	.value	1701
	.value	783
	.value	2555
	.value	2883
	.value	-1599
	.value	727
	.value	-1738
	.value	-2385
	.value	2194
	.value	-2160
	.value	103
	.value	-2391
	.value	2557
	.value	2762
	.value	1881
	.value	-2426
	.value	834
	.value	3310
	.value	-549
	.value	-1350
	.value	-2816
	.value	1386
	.value	638
	.value	-194
	.value	-3625
	.value	404
	.value	-1399
	.value	-3692
	.value	617
	.value	-2764
	.value	-1760
	.value	-1054
	.value	2319
	.value	1799
	.value	2535
	.value	-3588
	.value	2006
	.value	1533
	.value	3266
	.value	2113
	.value	-1100
	.value	-2579
	.value	-1431
	.value	-1756
	.value	-1296
	.value	1598
	.value	3174
	.value	-2
	.value	121
	.value	-3480
	.value	3153
	.value	-2572
	.value	1986
	.value	2743
	.value	-810
	.value	2919
	.value	-3593
	.value	2789
	.value	-1887
	.value	-921
	.value	2237
	.value	-1497
	.value	2310
	.value	-2133
	.value	-2250
	.value	-915
	.value	396
	.value	1390
	.value	-1483
	.value	3135
	.value	2043
	.value	-859
	.value	3696
	.value	2732
	.value	-3600
	.value	-1464
	.value	3706
	.value	2224
	.value	3772
	.value	-2665
	.value	-2876
	.value	1698
	.value	1305
	.value	2835
	.value	1921
	.value	730
	.value	1321
	.value	486
	.value	-3182
	.value	3417
	.value	7
	.value	-3428
	.value	2088
	.value	-3145
	.value	-1535
	.value	1168
	.value	-2495
	.value	-3831
	.value	-3555
	.value	-3750
	.value	-1525
	.value	660
	.value	-2804
	.value	2649
	.value	-2456
	.value	3405
	.value	3689
	.value	-1521
	.value	1993
	.value	1681
	.value	-2440
	.value	1056
	.value	-1414
	.value	1166
	.value	679
	.value	-2233
	.value	2830
	.value	2175
	.value	-2956
	.value	-1919
	.value	-3593
	.value	-1404
	.value	-1532
	.value	451
	.value	514
	.value	-402
	.value	-373
	.value	1278
	.value	3364
	.value	-509
	.value	-3816
	.value	-3770
	.value	438
	.value	-2345
	.value	-3456
	.value	-226
	.value	1701
	.value	-1689
	.value	783
	.value	-1509
	.value	2555
	.value	2963
	.value	2883
	.value	1242
	.value	-1599
	.value	1669
	.value	727
	.value	2719
	.value	-1738
	.value	642
	.value	-2385
	.value	-436
	.value	2194
	.value	3335
	.value	-2160
	.value	1779
	.value	103
	.value	3745
	.value	-2391
	.value	17
	.value	2557
	.value	2812
	.value	2762
	.value	-1144
	.value	1881
	.value	83
	.value	-2426
	.value	-1181
	.value	834
	.value	-1519
	.value	3310
	.value	3568
	.value	-549
	.value	-796
	.value	-1350
	.value	2072
	.value	-2816
	.value	-2460
	.value	1386
	.value	2891
	.value	638
	.value	-2083
	.value	-194
	.value	-715
	.value	-3593
	.value	-402
	.value	-3816
	.value	-226
	.value	2555
	.value	1669
	.value	-2385
	.value	1779
	.value	2557
	.value	83
	.value	3310
	.value	2072
	.value	638
	.value	1012
	.value	-3692
	.value	1295
	.value	2319
	.value	-3208
	.value	1533
	.value	-2071
	.value	-1431
	.value	-2005
	.value	-2
	.value	1586
	.value	1986
	.value	-293
	.value	1919
	.value	-929
	.value	-679
	.value	777
	.value	-1681
	.value	-3461
	.value	2456
	.value	3366
	.value	3750
	.value	-1203
	.value	1535
	.value	-3657
	.value	-3417
	.value	-1712
	.value	-1921
	.value	2515
	.value	2665
	.value	-1070
	.value	3600
	.value	2532
	.value	-3135
	.value	-2589
	.value	2250
	.value	-2258
	.value	921
	.value	-658
	.value	-514
	.value	509
	.value	3456
	.value	1509
	.value	1599
	.value	-642
	.value	2160
	.value	-17
	.value	-1881
	.value	1519
	.value	1350
	.value	-2891
	.value	-3593
	.value	-3434
	.value	-1497
	.value	893
	.value	396
	.value	-2422
	.value	-859
	.value	2965
	.value	3706
	.value	-2339
	.value	1698
	.value	-2937
	.value	1321
	.value	-670
	.value	-3428
	.value	-3163
	.value	-2495
	.value	-1072
	.value	660
	.value	1084
	.value	3689
	.value	-179
	.value	1056
	.value	-1338
	.value	2830
	.value	2786
	.value	-2919
	.value	-3677
	.value	-3153
	.value	-151
	.value	-1598
	.value	3334
	.value	1100
	.value	-3314
	.value	3588
	.value	2262
	.value	1760
	.value	-2230
	.value	-404
	.value	2083
	.value	2816
	.value	-3568
	.value	2426
	.value	-2812
	.value	-103
	.value	436
	.value	-727
	.value	-2963
	.value	-1701
	.value	3770
	.value	373
	.value	1404
	.value	1887
	.value	-1649
	.value	2133
	.value	-826
	.value	1483
	.value	434
	.value	-2732
	.value	3287
	.value	-3772
	.value	-2378
	.value	-2835
	.value	3723
	.value	-3593
	.value	658
	.value	2789
	.value	370
	.value	-1887
	.value	-3434
	.value	-921
	.value	-3752
	.value	2237
	.value	1649
	.value	-1497
	.value	2258
	.value	2310
	.value	3581
	.value	-2133
	.value	893
	.value	-2250
	.value	3794
	.value	-915
	.value	826
	.value	396
	.value	2589
	.value	1390
	.value	592
	.value	-1483
	.value	-2422
	.value	3135
	.value	3214
	.value	2043
	.value	-434
	.value	-859
	.value	-2532
	.value	3696
	.value	1121
	.value	2732
	.value	2965
	.value	-3600
	.value	2998
	.value	-1464
	.value	-3287
	.value	3706
	.value	1070
	.value	2224
	.value	-589
	.value	3772
	.value	-2339
	.value	-2665
	.value	2070
	.value	-2876
	.value	2378
	.value	1698
	.value	-2515
	.value	1305
	.value	-2815
	.value	2835
	.value	-2937
	.value	1921
	.value	-1348
	.value	730
	.value	-3723
	.value	1321
	.value	1712
	.value	486
	.value	2130
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	7681
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	10350
	.value	10350
	.value	10350
	.value	10350
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	10350
	.value	10350
	.value	10350
	.value	10350
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	16425
	.value	16425
	.value	16425
	.value	16425
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	16425
	.value	16425
	.value	16425
	.value	16425
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-4974
	.value	-4974
	.value	-4974
	.value	-4974
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-4974
	.value	-4974
	.value	-4974
	.value	-4974
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-7244
	.value	-7244
	.value	-7244
	.value	-7244
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-7244
	.value	-7244
	.value	-7244
	.value	-7244
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-4496
	.value	-4496
	.value	-4496
	.value	-4496
	.value	-9
	.value	-9
	.value	-9
	.value	-9
	.value	-4496
	.value	-4496
	.value	-4496
	.value	-4496
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-14744
	.value	-14744
	.value	-14744
	.value	-14744
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-14744
	.value	-14744
	.value	-14744
	.value	-14744
	.value	-9
	.value	-20315
	.value	-4974
	.value	18242
	.value	-16425
	.value	18191
	.value	-7244
	.value	-11655
	.value	-9
	.value	-20315
	.value	-4974
	.value	18242
	.value	-16425
	.value	18191
	.value	-7244
	.value	-11655
	.value	-28865
	.value	20870
	.value	14744
	.value	-22593
	.value	10350
	.value	828
	.value	4496
	.value	23754
	.value	-28865
	.value	20870
	.value	14744
	.value	-22593
	.value	10350
	.value	828
	.value	4496
	.value	23754
	.value	-9
	.value	-23754
	.value	-4496
	.value	-828
	.value	-10350
	.value	22593
	.value	-14744
	.value	-20870
	.value	-9
	.value	-23754
	.value	-4496
	.value	-828
	.value	-10350
	.value	22593
	.value	-14744
	.value	-20870
	.value	28865
	.value	11655
	.value	7244
	.value	-18191
	.value	16425
	.value	-18242
	.value	4974
	.value	20315
	.value	28865
	.value	11655
	.value	7244
	.value	-18191
	.value	16425
	.value	-18242
	.value	4974
	.value	20315
	.value	-9
	.value	-10972
	.value	-20315
	.value	23489
	.value	-4974
	.value	25597
	.value	18242
	.value	-2816
	.value	-9
	.value	-10972
	.value	-20315
	.value	23489
	.value	-4974
	.value	25597
	.value	18242
	.value	-2816
	.value	-16425
	.value	-19351
	.value	18191
	.value	-3114
	.value	-7244
	.value	-9488
	.value	-11655
	.value	19394
	.value	-16425
	.value	-19351
	.value	18191
	.value	-3114
	.value	-7244
	.value	-9488
	.value	-11655
	.value	19394
	.value	-9
	.value	-7491
	.value	-23754
	.value	-15307
	.value	-4496
	.value	-15750
	.value	-828
	.value	-5759
	.value	-9
	.value	-7491
	.value	-23754
	.value	-15307
	.value	-4496
	.value	-15750
	.value	-828
	.value	-5759
	.value	-10350
	.value	22568
	.value	22593
	.value	-20469
	.value	-14744
	.value	31177
	.value	-20870
	.value	26382
	.value	-10350
	.value	22568
	.value	22593
	.value	-20469
	.value	-14744
	.value	31177
	.value	-20870
	.value	26382
	.value	-9
	.value	-14846
	.value	-10972
	.value	-21066
	.value	-20315
	.value	-24581
	.value	23489
	.value	-23242
	.value	-4974
	.value	-4505
	.value	25597
	.value	-26279
	.value	18242
	.value	21467
	.value	-2816
	.value	15998
	.value	-16425
	.value	-4983
	.value	-19351
	.value	14624
	.value	18191
	.value	-2073
	.value	-3114
	.value	20674
	.value	-7244
	.value	-21399
	.value	-9488
	.value	6246
	.value	-11655
	.value	-29103
	.value	19394
	.value	-5930
	.value	-28865
	.value	-23668
	.value	-26382
	.value	-28839
	.value	20870
	.value	6536
	.value	-31177
	.value	16279
	.value	14744
	.value	29428
	.value	20469
	.value	29667
	.value	-22593
	.value	9215
	.value	-22568
	.value	-11783
	.value	10350
	.value	-14121
	.value	5759
	.value	-5913
	.value	828
	.value	-1724
	.value	15750
	.value	11792
	.value	4496
	.value	25093
	.value	15307
	.value	26228
	.value	23754
	.value	-21766
	.value	7491
	.value	-6817
	.value	-9
	.value	6817
	.value	-7491
	.value	21766
	.value	-23754
	.value	-26228
	.value	-15307
	.value	-25093
	.value	-4496
	.value	-11792
	.value	-15750
	.value	1724
	.value	-828
	.value	5913
	.value	-5759
	.value	14121
	.value	-10350
	.value	11783
	.value	22568
	.value	-9215
	.value	22593
	.value	-29667
	.value	-20469
	.value	-29428
	.value	-14744
	.value	-16279
	.value	31177
	.value	-6536
	.value	-20870
	.value	28839
	.value	26382
	.value	23668
	.value	28865
	.value	5930
	.value	-19394
	.value	29103
	.value	11655
	.value	-6246
	.value	9488
	.value	21399
	.value	7244
	.value	-20674
	.value	3114
	.value	2073
	.value	-18191
	.value	-14624
	.value	19351
	.value	4983
	.value	16425
	.value	-15998
	.value	2816
	.value	-21467
	.value	-18242
	.value	26279
	.value	-25597
	.value	4505
	.value	4974
	.value	23242
	.value	-23489
	.value	24581
	.value	20315
	.value	21066
	.value	10972
	.value	14846
	.value	-9
	.value	-32252
	.value	-14846
	.value	-19317
	.value	-10972
	.value	8472
	.value	-21066
	.value	-3456
	.value	-20315
	.value	16655
	.value	-24581
	.value	12611
	.value	23489
	.value	-12073
	.value	-23242
	.value	29871
	.value	-4974
	.value	6032
	.value	-4505
	.value	10409
	.value	25597
	.value	24266
	.value	-26279
	.value	17030
	.value	18242
	.value	10478
	.value	21467
	.value	11962
	.value	-2816
	.value	-26262
	.value	15998
	.value	-17602
	.value	-16425
	.value	-22124
	.value	-4983
	.value	-26220
	.value	-19351
	.value	-8908
	.value	14624
	.value	32738
	.value	18191
	.value	13575
	.value	-2073
	.value	27132
	.value	-3114
	.value	24573
	.value	20674
	.value	27201
	.value	-7244
	.value	12269
	.value	-21399
	.value	-16092
	.value	-9488
	.value	-15810
	.value	6246
	.value	15358
	.value	-11655
	.value	-15768
	.value	-29103
	.value	24052
	.value	19394
	.value	-26441
	.value	-5930
	.value	-1689
	.value	-9
	.value	13541
	.value	6817
	.value	-5529
	.value	-7491
	.value	26663
	.value	21766
	.value	-4693
	.value	-23754
	.value	13933
	.value	-26228
	.value	8558
	.value	-15307
	.value	-21953
	.value	-25093
	.value	-22875
	.value	-4496
	.value	-7508
	.value	-11792
	.value	-30136
	.value	-15750
	.value	26800
	.value	1724
	.value	17303
	.value	-828
	.value	2722
	.value	5913
	.value	-12013
	.value	-5759
	.value	30426
	.value	14121
	.value	3558
	.value	-10350
	.value	-24743
	.value	11783
	.value	-21860
	.value	22568
	.value	-32329
	.value	-9215
	.value	9360
	.value	22593
	.value	-7415
	.value	-29667
	.value	25946
	.value	-20469
	.value	-21868
	.value	-29428
	.value	-25511
	.value	-14744
	.value	1869
	.value	-16279
	.value	14351
	.value	31177
	.value	2193
	.value	-6536
	.value	17440
	.value	-20870
	.value	24718
	.value	28839
	.value	-23225
	.value	26382
	.value	9855
	.value	23668
	.value	-9599
	.value	-9
	.value	-32124
	.value	-32252
	.value	10179
	.value	-14846
	.value	6766
	.value	-19317
	.value	16638
	.value	-10972
	.value	-23549
	.value	8472
	.value	-17082
	.value	-21066
	.value	-15145
	.value	-3456
	.value	31518
	.value	-20315
	.value	-6297
	.value	16655
	.value	-12261
	.value	-24581
	.value	-11885
	.value	12611
	.value	30938
	.value	23489
	.value	28805
	.value	-12073
	.value	26783
	.value	-23242
	.value	-14718
	.value	29871
	.value	5708
	.value	-4974
	.value	15111
	.value	6032
	.value	-29453
	.value	-4505
	.value	12449
	.value	10409
	.value	529
	.value	25597
	.value	-32004
	.value	24266
	.value	2952
	.value	-26279
	.value	18003
	.value	17030
	.value	24931
	.value	18242
	.value	-1007
	.value	10478
	.value	-4624
	.value	21467
	.value	17636
	.value	11962
	.value	14360
	.value	-2816
	.value	15972
	.value	-26262
	.value	16715
	.value	15998
	.value	4573
	.value	-17602
	.value	-14539
	.value	-9
	.value	6766
	.value	8472
	.value	31518
	.value	-24581
	.value	28805
	.value	29871
	.value	-29453
	.value	25597
	.value	18003
	.value	10478
	.value	14360
	.value	15998
	.value	27636
	.value	-26220
	.value	17167
	.value	18191
	.value	-7304
	.value	24573
	.value	-22039
	.value	-21399
	.value	-4565
	.value	15358
	.value	10802
	.value	19394
	.value	21723
	.value	9599
	.value	-9633
	.value	-28839
	.value	-2807
	.value	-2193
	.value	-30597
	.value	14744
	.value	-26330
	.value	-25946
	.value	-2739
	.value	9215
	.value	32695
	.value	24743
	.value	-26288
	.value	5759
	.value	20435
	.value	-17303
	.value	24530
	.value	11792
	.value	20964
	.value	21953
	.value	23523
	.value	23754
	.value	-27858
	.value	5529
	.value	6510
	.value	14846
	.value	23549
	.value	3456
	.value	12261
	.value	-23489
	.value	14718
	.value	-6032
	.value	-529
	.value	26279
	.value	1007
	.value	-11962
	.value	-16715
	.value	-9
	.value	24214
	.value	26663
	.value	23933
	.value	-26228
	.value	-13686
	.value	-22875
	.value	-27243
	.value	-15750
	.value	4317
	.value	2722
	.value	8839
	.value	14121
	.value	-32414
	.value	-21860
	.value	-25179
	.value	22593
	.value	-25648
	.value	-21868
	.value	-964
	.value	-16279
	.value	-1715
	.value	17440
	.value	-14650
	.value	26382
	.value	-28958
	.value	1689
	.value	-10333
	.value	29103
	.value	-20119
	.value	15810
	.value	22790
	.value	7244
	.value	20238
	.value	-27132
	.value	-2858
	.value	-14624
	.value	19274
	.value	22124
	.value	-4573
	.value	2816
	.value	4624
	.value	-17030
	.value	32004
	.value	4505
	.value	-5708
	.value	12073
	.value	11885
	.value	20315
	.value	17082
	.value	19317
	.value	32124
	.value	-6817
	.value	14223
	.value	4693
	.value	-14138
	.value	15307
	.value	9650
	.value	7508
	.value	-9513
	.value	-1724
	.value	-23882
	.value	12013
	.value	-15221
	.value	-9
	.value	-6510
	.value	13541
	.value	-23182
	.value	6817
	.value	24214
	.value	-5529
	.value	-24232
	.value	-7491
	.value	-14223
	.value	26663
	.value	27858
	.value	21766
	.value	26621
	.value	-4693
	.value	23933
	.value	-23754
	.value	29394
	.value	13933
	.value	14138
	.value	-26228
	.value	-23523
	.value	8558
	.value	-23984
	.value	-15307
	.value	-13686
	.value	-21953
	.value	26766
	.value	-25093
	.value	-9650
	.value	-22875
	.value	-20964
	.value	-4496
	.value	-22943
	.value	-7508
	.value	-27243
	.value	-11792
	.value	-18506
	.value	-30136
	.value	9513
	.value	-15750
	.value	-24530
	.value	26800
	.value	947
	.value	1724
	.value	4317
	.value	17303
	.value	29718
	.value	-828
	.value	23882
	.value	2722
	.value	-20435
	.value	5913
	.value	-10495
	.value	-12013
	.value	8839
	.value	-5759
	.value	-3396
	.value	30426
	.value	15221
	.value	14121
	.value	26288
	.value	3558
	.value	27730
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	-28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	28865
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-16425
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	-10350
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	4
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	-3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	3777
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3625
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.value	-3182
	.ident	"GCC: (Debian 7.4.0-6) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
