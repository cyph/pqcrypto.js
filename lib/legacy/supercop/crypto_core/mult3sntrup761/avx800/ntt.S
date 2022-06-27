	.file	"ntt.c"
	.text
	.p2align 4,,15
	.globl	CRYPTO_NAMESPACE(ntt512_7681)
	.type	CRYPTO_NAMESPACE(ntt512_7681), @function
CRYPTO_NAMESPACE(ntt512_7681):
.LFB4756:
	.cfi_startproc
	movl	%esi, %r8d
	movslq	%esi, %rcx
	sall	$9, %r8d
	movslq	%r8d, %r8
	addq	%r8, %r8
	movq	%r8, %r9
	negq	%r9
	testq	%rcx, %rcx
	jle	.L2
	vmovdqa	1536+qdata_7681(%rip), %ymm11
	vmovdqa	3264+qdata_7681(%rip), %ymm9
	movq	%rdi, %rax
	xorl	%edx, %edx
	vmovdqa	3104+qdata_7681(%rip), %ymm10
	vmovdqa	3232+qdata_7681(%rip), %ymm12
	vmovdqa	3328+qdata_7681(%rip), %ymm13
	vmovdqa	3168+qdata_7681(%rip), %ymm14
	vmovdqa	%ymm11, %ymm6
	.p2align 4,,10
	.p2align 3
.L3:
	vmovdqu	512(%rax), %ymm0
	vmovdqu	(%rax), %ymm2
	addq	$1, %rdx
	addq	$1024, %rax
	vmovdqu	-256(%rax), %ymm1
	vmovdqu	-768(%rax), %ymm3
	vpaddw	%ymm2, %ymm0, %ymm7
	vmovdqu	-896(%rax), %ymm4
	vpsubw	%ymm0, %ymm2, %ymm2
	vmovdqu	-640(%rax), %ymm5
	vpaddw	%ymm3, %ymm1, %ymm0
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	-384(%rax), %ymm1
	vpaddw	%ymm4, %ymm1, %ymm15
	vpsubw	%ymm1, %ymm4, %ymm1
	vmovdqu	-128(%rax), %ymm4
	vpaddw	%ymm5, %ymm4, %ymm8
	vpsubw	%ymm4, %ymm5, %ymm5
	vpaddw	%ymm0, %ymm7, %ymm4
	vpsubw	%ymm0, %ymm7, %ymm0
	vpaddw	%ymm8, %ymm15, %ymm7
	vpsubw	%ymm8, %ymm15, %ymm8
	vpmulhw	%ymm9, %ymm3, %ymm15
	vpmullw	%ymm10, %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm2, %ymm3
	vpsubw	%ymm15, %ymm2, %ymm2
	vpmulhw	%ymm9, %ymm5, %ymm15
	vpmullw	%ymm10, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm1, %ymm5
	vpsubw	%ymm15, %ymm1, %ymm1
	vpmulhrsw	%ymm12, %ymm7, %ymm15
	vpmullw	%ymm11, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm7, %ymm15
	vpaddw	%ymm15, %ymm4, %ymm7
	vpsubw	%ymm15, %ymm4, %ymm4
	vpmulhw	%ymm9, %ymm8, %ymm15
	vpmullw	%ymm10, %ymm8, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm0, %ymm8
	vpsubw	%ymm15, %ymm0, %ymm0
	vpmulhw	%ymm13, %ymm5, %ymm15
	vpmullw	%ymm14, %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm3, %ymm5
	vpsubw	%ymm15, %ymm3, %ymm3
	vpmulhw	3360+qdata_7681(%rip), %ymm1, %ymm15
	vpmullw	3200+qdata_7681(%rip), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm15, %ymm1
	vpaddw	%ymm1, %ymm2, %ymm15
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmulhrsw	%ymm12, %ymm7, %ymm1
	vpmullw	%ymm11, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm7, %ymm7
	vpmulhw	512+qdata_7681(%rip), %ymm4, %ymm1
	vpmullw	2080+qdata_7681(%rip), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	768+qdata_7681(%rip), %ymm8, %ymm1
	vpmullw	2336+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm1, %ymm8
	vpmulhw	896+qdata_7681(%rip), %ymm0, %ymm1
	vpmullw	2464+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm0
	vpmulhw	1024+qdata_7681(%rip), %ymm5, %ymm1
	vpmullw	2592+qdata_7681(%rip), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm1, %ymm5
	vpmulhw	1152+qdata_7681(%rip), %ymm3, %ymm1
	vpmullw	2720+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm1, %ymm3
	vpmulhw	1408+qdata_7681(%rip), %ymm15, %ymm1
	vpmullw	2976+qdata_7681(%rip), %ymm15, %ymm15
	vpmulhw	%ymm6, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm1, %ymm15
	vpmulhw	1280+qdata_7681(%rip), %ymm2, %ymm1
	vpmullw	2848+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vperm2i128	$32, %ymm4, %ymm7, %ymm2
	vperm2i128	$49, %ymm4, %ymm7, %ymm4
	vperm2i128	$32, %ymm0, %ymm8, %ymm7
	vperm2i128	$49, %ymm0, %ymm8, %ymm0
	vperm2i128	$32, %ymm3, %ymm5, %ymm8
	vmovdqu	%ymm2, -1024(%rax)
	vperm2i128	$49, %ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm1, %ymm15, %ymm5
	vperm2i128	$49, %ymm1, %ymm15, %ymm1
	vmovdqu	%ymm4, -896(%rax)
	vmovdqu	%ymm7, -768(%rax)
	vmovdqu	%ymm0, -640(%rax)
	vmovdqu	%ymm8, -512(%rax)
	vmovdqu	%ymm3, -384(%rax)
	vmovdqu	%ymm5, -256(%rax)
	vmovdqu	%ymm1, -128(%rax)
	cmpq	%rcx, %rdx
	jne	.L3
	salq	$10, %rdx
	movq	%rdx, %rax
	subq	%r8, %rax
	addq	%rax, %rdi
	leaq	(%rdi,%rdx), %rax
	.p2align 4,,10
	.p2align 3
.L5:
	vmovdqu	544(%rdi), %ymm0
	vmovdqu	800(%rdi), %ymm1
	addq	$1024, %rdi
	vmovdqu	-992(%rdi), %ymm2
	vmovdqu	-736(%rdi), %ymm3
	vmovdqu	-864(%rdi), %ymm4
	vmovdqu	-608(%rdi), %ymm5
	vpaddw	%ymm2, %ymm0, %ymm7
	vpsubw	%ymm0, %ymm2, %ymm2
	vpaddw	%ymm3, %ymm1, %ymm0
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	-352(%rdi), %ymm1
	vpaddw	%ymm4, %ymm1, %ymm15
	vpsubw	%ymm1, %ymm4, %ymm1
	vmovdqu	-96(%rdi), %ymm4
	vpaddw	%ymm5, %ymm4, %ymm8
	vpsubw	%ymm4, %ymm5, %ymm5
	vpaddw	%ymm0, %ymm7, %ymm4
	vpsubw	%ymm0, %ymm7, %ymm0
	vpaddw	%ymm8, %ymm15, %ymm7
	vpsubw	%ymm8, %ymm15, %ymm8
	vpmulhw	%ymm9, %ymm3, %ymm15
	vpmullw	%ymm3, %ymm10, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm2, %ymm3
	vpsubw	%ymm15, %ymm2, %ymm2
	vpmulhw	%ymm9, %ymm5, %ymm15
	vpmullw	%ymm5, %ymm10, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm1, %ymm5
	vpsubw	%ymm15, %ymm1, %ymm1
	vpmulhrsw	%ymm12, %ymm7, %ymm15
	vpmullw	%ymm11, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm7, %ymm15
	vpaddw	%ymm15, %ymm4, %ymm7
	vpsubw	%ymm15, %ymm4, %ymm4
	vpmulhw	%ymm9, %ymm8, %ymm15
	vpmullw	%ymm8, %ymm10, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm0, %ymm8
	vpsubw	%ymm15, %ymm0, %ymm0
	vpmulhw	%ymm13, %ymm5, %ymm15
	vpmullw	%ymm5, %ymm14, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm3, %ymm5
	vpsubw	%ymm15, %ymm3, %ymm3
	vpmulhw	3360+qdata_7681(%rip), %ymm1, %ymm15
	vpmullw	3200+qdata_7681(%rip), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm15, %ymm1
	vpaddw	%ymm1, %ymm2, %ymm15
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmulhrsw	%ymm12, %ymm7, %ymm1
	vpmullw	%ymm11, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm7, %ymm7
	vpmulhw	544+qdata_7681(%rip), %ymm4, %ymm1
	vpmullw	2112+qdata_7681(%rip), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	800+qdata_7681(%rip), %ymm8, %ymm1
	vpmullw	2368+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm1, %ymm8
	vpmulhw	928+qdata_7681(%rip), %ymm0, %ymm1
	vpmullw	2496+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm0
	vpmulhw	1056+qdata_7681(%rip), %ymm5, %ymm1
	vpmullw	2624+qdata_7681(%rip), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm1, %ymm5
	vpmulhw	1184+qdata_7681(%rip), %ymm3, %ymm1
	vpmullw	2752+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm1, %ymm3
	vpmulhw	1440+qdata_7681(%rip), %ymm15, %ymm1
	vpmullw	3008+qdata_7681(%rip), %ymm15, %ymm15
	vpmulhw	%ymm6, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm1, %ymm15
	vpmulhw	1312+qdata_7681(%rip), %ymm2, %ymm1
	vpmullw	2880+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vperm2i128	$32, %ymm4, %ymm7, %ymm2
	vperm2i128	$49, %ymm4, %ymm7, %ymm4
	vperm2i128	$32, %ymm0, %ymm8, %ymm7
	vperm2i128	$49, %ymm0, %ymm8, %ymm0
	vperm2i128	$32, %ymm3, %ymm5, %ymm8
	vmovdqu	%ymm2, -992(%rdi)
	vperm2i128	$49, %ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm1, %ymm15, %ymm5
	vperm2i128	$49, %ymm1, %ymm15, %ymm1
	vmovdqu	%ymm4, -864(%rdi)
	vmovdqu	%ymm7, -736(%rdi)
	vmovdqu	%ymm0, -608(%rdi)
	vmovdqu	%ymm8, -480(%rdi)
	vmovdqu	%ymm3, -352(%rdi)
	vmovdqu	%ymm5, -224(%rdi)
	vmovdqu	%ymm1, -96(%rdi)
	cmpq	%rax, %rdi
	jne	.L5
	addq	%r9, %rdi
	leaq	(%rdi,%rdx), %rax
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	576(%rdi), %ymm0
	vmovdqu	832(%rdi), %ymm1
	addq	$1024, %rdi
	vmovdqu	-960(%rdi), %ymm2
	vmovdqu	-704(%rdi), %ymm3
	vmovdqu	-832(%rdi), %ymm4
	vmovdqu	-576(%rdi), %ymm5
	vpaddw	%ymm2, %ymm0, %ymm7
	vpsubw	%ymm0, %ymm2, %ymm2
	vpaddw	%ymm3, %ymm1, %ymm0
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	-320(%rdi), %ymm1
	vpaddw	%ymm4, %ymm1, %ymm15
	vpsubw	%ymm1, %ymm4, %ymm1
	vmovdqu	-64(%rdi), %ymm4
	vpaddw	%ymm5, %ymm4, %ymm8
	vpsubw	%ymm4, %ymm5, %ymm5
	vpaddw	%ymm0, %ymm7, %ymm4
	vpsubw	%ymm0, %ymm7, %ymm0
	vpaddw	%ymm8, %ymm15, %ymm7
	vpsubw	%ymm8, %ymm15, %ymm8
	vpmulhw	%ymm9, %ymm3, %ymm15
	vpmullw	%ymm3, %ymm10, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm2, %ymm3
	vpsubw	%ymm15, %ymm2, %ymm2
	vpmulhw	%ymm9, %ymm5, %ymm15
	vpmullw	%ymm5, %ymm10, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm1, %ymm5
	vpsubw	%ymm15, %ymm1, %ymm1
	vpmulhrsw	%ymm12, %ymm7, %ymm15
	vpmullw	%ymm11, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm7, %ymm15
	vpaddw	%ymm15, %ymm4, %ymm7
	vpsubw	%ymm15, %ymm4, %ymm4
	vpmulhw	%ymm9, %ymm8, %ymm15
	vpmullw	%ymm8, %ymm10, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm0, %ymm8
	vpsubw	%ymm15, %ymm0, %ymm0
	vpmulhw	%ymm13, %ymm5, %ymm15
	vpmullw	%ymm5, %ymm14, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm3, %ymm5
	vpsubw	%ymm15, %ymm3, %ymm3
	vpmulhw	3360+qdata_7681(%rip), %ymm1, %ymm15
	vpmullw	3200+qdata_7681(%rip), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm15, %ymm1
	vpaddw	%ymm1, %ymm2, %ymm15
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmulhrsw	%ymm12, %ymm7, %ymm1
	vpmullw	%ymm11, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm7, %ymm7
	vpmulhw	576+qdata_7681(%rip), %ymm4, %ymm1
	vpmullw	2144+qdata_7681(%rip), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	832+qdata_7681(%rip), %ymm8, %ymm1
	vpmullw	2400+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm1, %ymm8
	vpmulhw	960+qdata_7681(%rip), %ymm0, %ymm1
	vpmullw	2528+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm0
	vpmulhw	1088+qdata_7681(%rip), %ymm5, %ymm1
	vpmullw	2656+qdata_7681(%rip), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm1, %ymm5
	vpmulhw	1216+qdata_7681(%rip), %ymm3, %ymm1
	vpmullw	2784+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm1, %ymm3
	vpmulhw	1472+qdata_7681(%rip), %ymm15, %ymm1
	vpmullw	3040+qdata_7681(%rip), %ymm15, %ymm15
	vpmulhw	%ymm6, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm1, %ymm15
	vpmulhw	1344+qdata_7681(%rip), %ymm2, %ymm1
	vpmullw	2912+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vperm2i128	$32, %ymm4, %ymm7, %ymm2
	vperm2i128	$49, %ymm4, %ymm7, %ymm4
	vperm2i128	$32, %ymm0, %ymm8, %ymm7
	vperm2i128	$49, %ymm0, %ymm8, %ymm0
	vperm2i128	$32, %ymm3, %ymm5, %ymm8
	vmovdqu	%ymm2, -960(%rdi)
	vperm2i128	$49, %ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm1, %ymm15, %ymm5
	vperm2i128	$49, %ymm1, %ymm15, %ymm1
	vmovdqu	%ymm4, -832(%rdi)
	vmovdqu	%ymm7, -704(%rdi)
	vmovdqu	%ymm0, -576(%rdi)
	vmovdqu	%ymm8, -448(%rdi)
	vmovdqu	%ymm3, -320(%rdi)
	vmovdqu	%ymm5, -192(%rdi)
	vmovdqu	%ymm1, -64(%rdi)
	cmpq	%rax, %rdi
	jne	.L7
	addq	%r9, %rdi
	addq	%rdi, %rdx
	.p2align 4,,10
	.p2align 3
.L9:
	vmovdqu	608(%rdi), %ymm0
	vmovdqu	864(%rdi), %ymm1
	addq	$1024, %rdi
	vmovdqu	-928(%rdi), %ymm2
	vmovdqu	-672(%rdi), %ymm3
	vmovdqu	-800(%rdi), %ymm4
	vmovdqu	-544(%rdi), %ymm5
	vpaddw	%ymm2, %ymm0, %ymm7
	vpsubw	%ymm0, %ymm2, %ymm2
	vpaddw	%ymm3, %ymm1, %ymm0
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	-288(%rdi), %ymm1
	vpaddw	%ymm4, %ymm1, %ymm15
	vpsubw	%ymm1, %ymm4, %ymm1
	vmovdqu	-32(%rdi), %ymm4
	vpaddw	%ymm5, %ymm4, %ymm8
	vpsubw	%ymm4, %ymm5, %ymm5
	vpaddw	%ymm0, %ymm7, %ymm4
	vpsubw	%ymm0, %ymm7, %ymm0
	vpaddw	%ymm8, %ymm15, %ymm7
	vpsubw	%ymm8, %ymm15, %ymm8
	vpmulhw	%ymm9, %ymm3, %ymm15
	vpmullw	%ymm3, %ymm10, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm2, %ymm3
	vpsubw	%ymm15, %ymm2, %ymm2
	vpmulhw	%ymm9, %ymm5, %ymm15
	vpmullw	%ymm5, %ymm10, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm1, %ymm5
	vpsubw	%ymm15, %ymm1, %ymm1
	vpmulhrsw	%ymm12, %ymm7, %ymm15
	vpmullw	%ymm11, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm7, %ymm15
	vpaddw	%ymm15, %ymm4, %ymm7
	vpsubw	%ymm15, %ymm4, %ymm4
	vpmulhw	%ymm9, %ymm8, %ymm15
	vpmullw	%ymm8, %ymm10, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm0, %ymm8
	vpsubw	%ymm15, %ymm0, %ymm0
	vpmulhw	%ymm13, %ymm5, %ymm15
	vpmullw	%ymm5, %ymm14, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm15, %ymm15
	vpaddw	%ymm15, %ymm3, %ymm5
	vpsubw	%ymm15, %ymm3, %ymm3
	vpmulhw	3360+qdata_7681(%rip), %ymm1, %ymm15
	vpmullw	3200+qdata_7681(%rip), %ymm1, %ymm1
	vpmulhw	%ymm6, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm15, %ymm1
	vpaddw	%ymm1, %ymm2, %ymm15
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmulhrsw	%ymm12, %ymm7, %ymm1
	vpmullw	%ymm11, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm7, %ymm7
	vpmulhw	608+qdata_7681(%rip), %ymm4, %ymm1
	vpmullw	2176+qdata_7681(%rip), %ymm4, %ymm4
	vpmulhw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm1, %ymm4
	vpmulhw	864+qdata_7681(%rip), %ymm8, %ymm1
	vpmullw	2432+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm6, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm1, %ymm8
	vpmulhw	992+qdata_7681(%rip), %ymm0, %ymm1
	vpmullw	2560+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm6, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm0
	vpmulhw	1120+qdata_7681(%rip), %ymm5, %ymm1
	vpmullw	2688+qdata_7681(%rip), %ymm5, %ymm5
	vpmulhw	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm1, %ymm5
	vpmulhw	1248+qdata_7681(%rip), %ymm3, %ymm1
	vpmullw	2816+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm1, %ymm3
	vpmulhw	1504+qdata_7681(%rip), %ymm15, %ymm1
	vpmullw	3072+qdata_7681(%rip), %ymm15, %ymm15
	vpmulhw	%ymm6, %ymm15, %ymm15
	vpsubw	%ymm15, %ymm1, %ymm15
	vpmulhw	1376+qdata_7681(%rip), %ymm2, %ymm1
	vpmullw	2944+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm6, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vperm2i128	$32, %ymm4, %ymm7, %ymm2
	vperm2i128	$49, %ymm4, %ymm7, %ymm4
	vperm2i128	$32, %ymm0, %ymm8, %ymm7
	vperm2i128	$49, %ymm0, %ymm8, %ymm0
	vperm2i128	$32, %ymm3, %ymm5, %ymm8
	vmovdqu	%ymm2, -928(%rdi)
	vperm2i128	$49, %ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm1, %ymm15, %ymm5
	vperm2i128	$49, %ymm1, %ymm15, %ymm1
	vmovdqu	%ymm4, -800(%rdi)
	vmovdqu	%ymm7, -672(%rdi)
	vmovdqu	%ymm0, -544(%rdi)
	vmovdqu	%ymm8, -416(%rdi)
	vmovdqu	%ymm3, -288(%rdi)
	vmovdqu	%ymm5, -160(%rdi)
	vmovdqu	%ymm1, -32(%rdi)
	cmpq	%rdx, %rdi
	jne	.L9
	sall	$2, %esi
	addq	%r9, %rdi
	movslq	%esi, %rcx
	testl	%esi, %esi
	jle	.L28
.L32:
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	movq	%rdi, %rax
	xorl	%edx, %edx
	pushq	-8(%r10)
	pushq	%rbp
	.cfi_escape 0x10,0x6,0x2,0x76,0
	movq	%rsp, %rbp
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x78,0x6
	vmovdqa	1536+qdata_7681(%rip), %ymm0
	vmovdqa	%ymm0, %ymm10
	vmovdqa	%ymm0, -48(%rbp)
	.p2align 4,,10
	.p2align 3
.L11:
	vmovdqu	(%rax), %ymm1
	vmovdqu	64(%rax), %ymm0
	addq	$1, %rdx
	addq	$256, %rax
	vmovdqu	-128(%rax), %ymm2
	vmovdqu	-224(%rax), %ymm6
	vpaddw	%ymm1, %ymm0, %ymm5
	vpsubw	%ymm0, %ymm1, %ymm0
	vmovdqu	-64(%rax), %ymm1
	vmovdqu	-96(%rax), %ymm7
	vpaddw	%ymm2, %ymm1, %ymm8
	vpsubw	%ymm1, %ymm2, %ymm2
	vmovdqu	-160(%rax), %ymm1
	vpaddw	%ymm6, %ymm1, %ymm3
	vpsubw	%ymm1, %ymm6, %ymm6
	vmovdqu	-32(%rax), %ymm1
	vpaddw	%ymm3, %ymm5, %ymm13
	vpsubw	%ymm3, %ymm5, %ymm5
	vpmulhw	3264+qdata_7681(%rip), %ymm6, %ymm3
	vpmullw	3104+qdata_7681(%rip), %ymm6, %ymm6
	vpmulhw	%ymm10, %ymm6, %ymm6
	vpaddw	%ymm7, %ymm1, %ymm4
	vpsubw	%ymm1, %ymm7, %ymm1
	vpaddw	%ymm4, %ymm8, %ymm7
	vpsubw	%ymm4, %ymm8, %ymm4
	vpmulhw	288+qdata_7681(%rip), %ymm4, %ymm12
	vpmullw	1856+qdata_7681(%rip), %ymm4, %ymm4
	vpmulhw	%ymm10, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm3, %ymm3
	vpaddw	%ymm3, %ymm0, %ymm6
	vpsubw	%ymm3, %ymm0, %ymm0
	vpmulhw	3264+qdata_7681(%rip), %ymm1, %ymm3
	vpmullw	3104+qdata_7681(%rip), %ymm1, %ymm1
	vpmulhw	%ymm10, %ymm1, %ymm1
	vpmulhw	384+qdata_7681(%rip), %ymm6, %ymm15
	vpmullw	1952+qdata_7681(%rip), %ymm6, %ymm6
	vpmulhw	%ymm10, %ymm6, %ymm6
	vpmulhw	448+qdata_7681(%rip), %ymm0, %ymm14
	vpmullw	2016+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm10, %ymm0, %ymm0
	vpsubw	%ymm4, %ymm12, %ymm4
	vpsubw	%ymm1, %ymm3, %ymm1
	vpsubw	%ymm6, %ymm15, %ymm6
	vpmulhrsw	3232+qdata_7681(%rip), %ymm7, %ymm3
	vmovdqa	%ymm3, -80(%rbp)
	vpaddw	%ymm1, %ymm2, %ymm8
	vpmulhw	256+qdata_7681(%rip), %ymm5, %ymm3
	vpmulhw	416+qdata_7681(%rip), %ymm8, %ymm11
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmullw	1824+qdata_7681(%rip), %ymm5, %ymm5
	vpmulhw	%ymm10, %ymm5, %ymm5
	vpmulhw	480+qdata_7681(%rip), %ymm2, %ymm9
	vpmullw	1984+qdata_7681(%rip), %ymm8, %ymm8
	vpmullw	2048+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm10, %ymm8, %ymm8
	vpmulhw	%ymm10, %ymm2, %ymm2
	vpsubw	%ymm0, %ymm14, %ymm0
	vpmulhrsw	3232+qdata_7681(%rip), %ymm13, %ymm1
	vpmullw	-48(%rbp), %ymm1, %ymm1
	vpsubw	%ymm1, %ymm13, %ymm13
	vpunpcklwd	%ymm6, %ymm13, %ymm1
	vpunpckhwd	%ymm6, %ymm13, %ymm6
	vpsubw	%ymm5, %ymm3, %ymm3
	vmovdqa	-48(%rbp), %ymm5
	vpmullw	-80(%rbp), %ymm5, %ymm13
	vpsubw	%ymm13, %ymm7, %ymm7
	vpsubw	%ymm8, %ymm11, %ymm8
	vpsubw	%ymm2, %ymm9, %ymm2
	vpunpcklwd	%ymm0, %ymm3, %ymm15
	vpunpcklwd	%ymm2, %ymm4, %ymm5
	vpunpckhwd	%ymm0, %ymm3, %ymm3
	vpunpcklwd	%ymm8, %ymm7, %ymm0
	vpunpckhwd	%ymm8, %ymm7, %ymm13
	vpunpckhwd	%ymm2, %ymm4, %ymm9
	vpaddw	%ymm0, %ymm1, %ymm8
	vpaddw	%ymm5, %ymm15, %ymm2
	vpaddw	%ymm13, %ymm6, %ymm7
	vpunpckldq	%ymm2, %ymm8, %ymm12
	vpsubw	%ymm5, %ymm15, %ymm15
	vpunpckhdq	%ymm2, %ymm8, %ymm8
	vpsubw	%ymm0, %ymm1, %ymm1
	vpaddw	%ymm9, %ymm3, %ymm2
	vmovdqu	%ymm12, -256(%rax)
	vpsubw	%ymm9, %ymm3, %ymm3
	vpsubw	%ymm13, %ymm6, %ymm6
	vpunpckldq	%ymm2, %ymm7, %ymm11
	vmovdqu	%ymm8, -224(%rax)
	vpunpckhdq	%ymm2, %ymm7, %ymm7
	vpunpckldq	%ymm15, %ymm1, %ymm0
	vpunpckldq	%ymm3, %ymm6, %ymm2
	vmovdqu	%ymm11, -192(%rax)
	vpunpckhdq	%ymm15, %ymm1, %ymm1
	vpunpckhdq	%ymm3, %ymm6, %ymm6
	vmovdqu	%ymm0, -128(%rax)
	vmovdqu	%ymm7, -160(%rax)
	vmovdqu	%ymm1, -96(%rax)
	vmovdqu	%ymm2, -64(%rax)
	vmovdqu	%ymm6, -32(%rax)
	cmpq	%rdx, %rcx
	jne	.L11
	salq	$8, %rcx
	vmovdqa	1568+qdata_7681(%rip), %ymm9
	vmovdqa	-48(%rbp), %ymm7
	movq	%rcx, %rax
	subq	%r8, %rax
	addq	%rax, %rdi
	addq	%rdi, %rcx
	.p2align 4,,10
	.p2align 3
.L13:
	vmovdqu	64(%rdi), %ymm0
	vmovdqu	(%rdi), %ymm5
	addq	$256, %rdi
	vmovdqu	-224(%rdi), %ymm4
	vmovdqu	-96(%rdi), %ymm8
	vpaddw	%ymm5, %ymm0, %ymm1
	vpsubw	%ymm0, %ymm5, %ymm5
	vmovdqu	-160(%rdi), %ymm0
	vpaddw	%ymm4, %ymm0, %ymm6
	vpsubw	%ymm0, %ymm4, %ymm4
	vmovdqu	-64(%rdi), %ymm0
	vpmulhw	3264+qdata_7681(%rip), %ymm0, %ymm3
	vpmullw	3104+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm10, %ymm0, %ymm0
	vpmullw	1600+qdata_7681(%rip), %ymm4, %ymm11
	vpmulhw	%ymm10, %ymm11, %ymm11
	vpsubw	%ymm0, %ymm3, %ymm0
	vmovdqu	-128(%rdi), %ymm3
	vpaddw	%ymm3, %ymm0, %ymm12
	vpsubw	%ymm0, %ymm3, %ymm3
	vmovdqu	-32(%rdi), %ymm0
	vpmulhw	3264+qdata_7681(%rip), %ymm0, %ymm2
	vpmullw	3104+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm10, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm2
	vpaddw	%ymm8, %ymm2, %ymm0
	vpsubw	%ymm2, %ymm8, %ymm2
	vpmulhw	160+qdata_7681(%rip), %ymm0, %ymm13
	vpmulhrsw	3232+qdata_7681(%rip), %ymm1, %ymm8
	vpmullw	%ymm7, %ymm8, %ymm8
	vpmullw	1728+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm10, %ymm0, %ymm0
	vpsubw	%ymm8, %ymm1, %ymm8
	vpsubw	%ymm0, %ymm13, %ymm13
	vpmulhw	192+qdata_7681(%rip), %ymm3, %ymm0
	vpmulhrsw	3232+qdata_7681(%rip), %ymm6, %ymm1
	vpmullw	%ymm7, %ymm1, %ymm1
	vpmullw	1760+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm10, %ymm3, %ymm3
	vpsubw	%ymm1, %ymm6, %ymm6
	vpmulhw	qdata_7681(%rip), %ymm5, %ymm1
	vpmullw	%ymm9, %ymm5, %ymm5
	vpsubw	%ymm3, %ymm0, %ymm0
	vpmullw	1792+qdata_7681(%rip), %ymm2, %ymm3
	vpmulhw	%ymm10, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm1, %ymm5
	vpmulhw	32+qdata_7681(%rip), %ymm4, %ymm1
	vpsubw	%ymm11, %ymm1, %ymm11
	vpmullw	1696+qdata_7681(%rip), %ymm12, %ymm4
	vpmulhw	%ymm10, %ymm4, %ymm4
	vpmulhw	128+qdata_7681(%rip), %ymm12, %ymm1
	vpunpcklqdq	%ymm13, %ymm6, %ymm12
	vpunpckhqdq	%ymm13, %ymm6, %ymm6
	vpsubw	%ymm4, %ymm1, %ymm1
	vpmulhw	224+qdata_7681(%rip), %ymm2, %ymm4
	vpmulhw	%ymm10, %ymm3, %ymm2
	vpunpcklqdq	%ymm0, %ymm5, %ymm3
	vpunpckhqdq	%ymm0, %ymm5, %ymm5
	vpsubw	%ymm2, %ymm4, %ymm2
	vpunpcklqdq	%ymm1, %ymm8, %ymm4
	vpunpckhqdq	%ymm1, %ymm8, %ymm1
	vpunpcklqdq	%ymm2, %ymm11, %ymm0
	vpunpckhqdq	%ymm2, %ymm11, %ymm2
	vpaddw	%ymm4, %ymm12, %ymm8
	vpaddw	%ymm1, %ymm6, %ymm11
	vpsubw	%ymm12, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm1, %ymm1
	vpaddw	%ymm5, %ymm2, %ymm12
	vpaddw	%ymm3, %ymm0, %ymm6
	vpsubw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm2, %ymm5, %ymm0
	vpmulhw	3264+qdata_7681(%rip), %ymm1, %ymm2
	vpmullw	3104+qdata_7681(%rip), %ymm1, %ymm1
	vpmulhw	%ymm10, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm2, %ymm1
	vpmulhw	3264+qdata_7681(%rip), %ymm0, %ymm2
	vpmullw	3104+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm10, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm0
	vpaddw	%ymm11, %ymm8, %ymm2
	vmovdqu	%ymm2, -256(%rdi)
	vpaddw	%ymm1, %ymm4, %ymm2
	vpsubw	%ymm11, %ymm8, %ymm8
	vmovdqu	%ymm2, -224(%rdi)
	vpaddw	%ymm12, %ymm6, %ymm2
	vpsubw	%ymm1, %ymm4, %ymm4
	vmovdqu	%ymm2, -192(%rdi)
	vpsubw	%ymm12, %ymm6, %ymm6
	vpaddw	%ymm0, %ymm3, %ymm2
	vpsubw	%ymm0, %ymm3, %ymm3
	vmovdqu	%ymm2, -160(%rdi)
	vmovdqu	%ymm8, -128(%rdi)
	vmovdqu	%ymm4, -96(%rdi)
	vmovdqu	%ymm6, -64(%rdi)
	vmovdqu	%ymm3, -32(%rdi)
	cmpq	%rcx, %rdi
	jne	.L13
	vzeroupper
	popq	%r10
	.cfi_def_cfa 10, 0
	popq	%rbp
	leaq	-8(%r10), %rsp
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L2:
	.cfi_restore 6
	movq	%r9, %rax
	sall	$2, %esi
	subq	%r8, %rax
	movslq	%esi, %rcx
	subq	%r8, %rax
	addq	%rax, %rdi
	addq	%r9, %rdi
	testl	%esi, %esi
	jg	.L32
.L28:
	vzeroupper
	ret
	.cfi_endproc
.LFE4756:
	.size	CRYPTO_NAMESPACE(ntt512_7681), .-CRYPTO_NAMESPACE(ntt512_7681)
	.p2align 4,,15
	.globl	CRYPTO_NAMESPACE(invntt512_7681)
	.type	CRYPTO_NAMESPACE(invntt512_7681), @function
CRYPTO_NAMESPACE(invntt512_7681):
.LFB4758:
	.cfi_startproc
	leaq	8(%rsp), %r10
	.cfi_def_cfa 10, 0
	andq	$-32, %rsp
	leal	0(,%rsi,4), %r8d
	sall	$9, %esi
	pushq	-8(%r10)
	movslq	%esi, %rsi
	pushq	%rbp
	addq	%rsi, %rsi
	movq	%rsi, %r9
	negq	%r9
	.cfi_escape 0x10,0x6,0x2,0x76,0
	movq	%rsp, %rbp
	pushq	%r10
	.cfi_escape 0xf,0x3,0x76,0x78,0x6
	testl	%r8d, %r8d
	jle	.L34
	vmovdqa	1536+qdata_7681(%rip), %ymm0
	movslq	%r8d, %rcx
	movq	%rdi, %rax
	xorl	%edx, %edx
	vmovdqa	3296+qdata_7681(%rip), %ymm14
	vmovdqa	3136+qdata_7681(%rip), %ymm15
	vmovdqa	%ymm0, %ymm13
	vmovdqa	%ymm0, -48(%rbp)
	.p2align 4,,10
	.p2align 3
.L35:
	vmovdqu	224(%rax), %ymm1
	vmovdqu	96(%rax), %ymm0
	addq	$1, %rdx
	addq	$256, %rax
	vmovdqu	-256(%rax), %ymm10
	vpaddw	%ymm0, %ymm1, %ymm7
	vpsubw	%ymm1, %ymm0, %ymm0
	vpmulhw	%ymm14, %ymm0, %ymm1
	vpmullw	%ymm15, %ymm0, %ymm0
	vpmulhw	%ymm13, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm4
	vmovdqu	-96(%rax), %ymm1
	vmovdqu	-224(%rax), %ymm0
	vpaddw	%ymm0, %ymm1, %ymm9
	vpsubw	%ymm1, %ymm0, %ymm0
	vmovdqu	-192(%rax), %ymm1
	vpmulhw	%ymm14, %ymm0, %ymm2
	vpmullw	%ymm0, %ymm15, %ymm0
	vpmulhw	%ymm13, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm2
	vmovdqu	-64(%rax), %ymm0
	vpaddw	%ymm1, %ymm0, %ymm3
	vpsubw	%ymm0, %ymm1, %ymm1
	vmovdqu	-128(%rax), %ymm0
	vpaddw	%ymm1, %ymm4, %ymm5
	vpaddw	%ymm3, %ymm7, %ymm12
	vpaddw	%ymm10, %ymm0, %ymm6
	vpsubw	%ymm4, %ymm1, %ymm1
	vpunpcklqdq	%ymm5, %ymm12, %ymm11
	vpsubw	%ymm7, %ymm3, %ymm3
	vpaddw	%ymm6, %ymm9, %ymm8
	vpunpckhqdq	%ymm5, %ymm12, %ymm12
	vpsubw	%ymm9, %ymm6, %ymm6
	vpunpcklqdq	%ymm1, %ymm3, %ymm9
	vpunpckhqdq	%ymm1, %ymm3, %ymm3
	vpmulhw	160+qdata_7681(%rip), %ymm3, %ymm5
	vpmullw	1728+qdata_7681(%rip), %ymm3, %ymm4
	vpmulhw	%ymm13, %ymm4, %ymm4
	vpsubw	%ymm0, %ymm10, %ymm10
	vpmullw	1696+qdata_7681(%rip), %ymm12, %ymm3
	vpmulhw	%ymm13, %ymm3, %ymm3
	vpaddw	%ymm10, %ymm2, %ymm7
	vpsubw	%ymm2, %ymm10, %ymm10
	vpunpcklqdq	%ymm10, %ymm6, %ymm0
	vpunpckhqdq	%ymm10, %ymm6, %ymm2
	vpmullw	1792+qdata_7681(%rip), %ymm2, %ymm6
	vpunpcklqdq	%ymm7, %ymm8, %ymm1
	vpmulhw	%ymm13, %ymm6, %ymm6
	vpunpckhqdq	%ymm7, %ymm8, %ymm8
	vpmulhw	192+qdata_7681(%rip), %ymm8, %ymm7
	vpsubw	%ymm4, %ymm5, %ymm4
	vpmulhw	128+qdata_7681(%rip), %ymm12, %ymm5
	vpsubw	%ymm3, %ymm5, %ymm5
	vpmulhw	224+qdata_7681(%rip), %ymm2, %ymm3
	vpmullw	1760+qdata_7681(%rip), %ymm8, %ymm2
	vpmulhw	%ymm13, %ymm2, %ymm2
	vpmulhw	96+qdata_7681(%rip), %ymm9, %ymm8
	vpmullw	1664+qdata_7681(%rip), %ymm9, %ymm9
	vpmulhw	%ymm13, %ymm9, %ymm9
	vpsubw	%ymm6, %ymm3, %ymm6
	vmovdqa	-48(%rbp), %ymm3
	vpsubw	%ymm2, %ymm7, %ymm7
	vpmulhrsw	3232+qdata_7681(%rip), %ymm0, %ymm2
	vpmullw	%ymm3, %ymm2, %ymm2
	vpsubw	%ymm9, %ymm8, %ymm8
	vpmulhw	64+qdata_7681(%rip), %ymm11, %ymm9
	vpmullw	1632+qdata_7681(%rip), %ymm11, %ymm11
	vpmulhw	%ymm13, %ymm11, %ymm11
	vpsubw	%ymm2, %ymm0, %ymm0
	vpmulhrsw	3232+qdata_7681(%rip), %ymm1, %ymm2
	vpmullw	%ymm3, %ymm2, %ymm2
	vpsubw	%ymm5, %ymm7, %ymm3
	vpsubw	%ymm11, %ymm9, %ymm9
	vpmulhw	%ymm14, %ymm3, %ymm11
	vpaddw	%ymm7, %ymm5, %ymm5
	vpmullw	%ymm3, %ymm15, %ymm3
	vmovdqu	%ymm5, -128(%rax)
	vpsubw	%ymm2, %ymm1, %ymm1
	vpsubw	%ymm4, %ymm6, %ymm2
	vpmulhw	%ymm14, %ymm2, %ymm10
	vpmullw	%ymm2, %ymm15, %ymm2
	vpaddw	%ymm1, %ymm9, %ymm12
	vpmulhw	%ymm13, %ymm3, %ymm3
	vmovdqu	%ymm12, -256(%rax)
	vpsubw	%ymm9, %ymm1, %ymm1
	vpaddw	%ymm0, %ymm8, %ymm12
	vpaddw	%ymm6, %ymm4, %ymm4
	vpsubw	%ymm8, %ymm0, %ymm0
	vmovdqu	%ymm12, -224(%rax)
	vmovdqu	%ymm1, -192(%rax)
	vmovdqu	%ymm0, -160(%rax)
	vpmulhw	%ymm13, %ymm2, %ymm2
	vmovdqu	%ymm4, -96(%rax)
	vpsubw	%ymm3, %ymm11, %ymm3
	vmovdqu	%ymm3, -64(%rax)
	vpsubw	%ymm2, %ymm10, %ymm2
	vmovdqu	%ymm2, -32(%rax)
	cmpq	%rcx, %rdx
	jne	.L35
	salq	$8, %rdx
	movq	%rdx, %rax
	subq	%rsi, %rax
	addq	%rax, %rdi
	addq	%rdi, %rdx
	.p2align 4,,10
	.p2align 3
.L37:
	vmovdqu	224(%rdi), %ymm4
	vmovdqu	192(%rdi), %ymm0
	addq	$256, %rdi
	vmovdqu	-96(%rdi), %ymm2
	vpunpckldq	%ymm4, %ymm0, %ymm8
	vpunpckhdq	%ymm4, %ymm0, %ymm0
	vpunpckldq	%ymm0, %ymm8, %ymm6
	vpunpckhdq	%ymm0, %ymm8, %ymm8
	vmovdqu	-128(%rdi), %ymm0
	vpunpckldq	%ymm2, %ymm0, %ymm1
	vpunpckhdq	%ymm2, %ymm0, %ymm0
	vmovdqu	-160(%rdi), %ymm2
	vpunpckldq	%ymm0, %ymm1, %ymm3
	vpunpckhdq	%ymm0, %ymm1, %ymm1
	vmovdqu	-192(%rdi), %ymm0
	vpunpckldq	%ymm2, %ymm0, %ymm4
	vpunpckhdq	%ymm2, %ymm0, %ymm0
	vmovdqu	-224(%rdi), %ymm2
	vpunpckldq	%ymm0, %ymm4, %ymm10
	vpunpckhdq	%ymm0, %ymm4, %ymm4
	vmovdqu	-256(%rdi), %ymm0
	vpunpckldq	%ymm2, %ymm0, %ymm7
	vpunpckhdq	%ymm2, %ymm0, %ymm0
	vpsubw	%ymm8, %ymm4, %ymm2
	vpunpckldq	%ymm0, %ymm7, %ymm9
	vpunpckhdq	%ymm0, %ymm7, %ymm7
	vpaddw	%ymm4, %ymm8, %ymm4
	vpsubw	%ymm1, %ymm7, %ymm5
	vpsubw	%ymm3, %ymm9, %ymm11
	vpunpcklwd	%ymm2, %ymm5, %ymm0
	vpunpckhwd	%ymm2, %ymm5, %ymm5
	vpaddw	%ymm7, %ymm1, %ymm1
	vpunpcklwd	%ymm5, %ymm0, %ymm2
	vpunpckhwd	%ymm5, %ymm0, %ymm0
	vpunpcklwd	%ymm4, %ymm1, %ymm7
	vpunpcklwd	%ymm0, %ymm2, %ymm5
	vpunpckhwd	%ymm0, %ymm2, %ymm2
	vpsubw	%ymm6, %ymm10, %ymm0
	vpunpcklwd	%ymm0, %ymm11, %ymm12
	vpunpckhwd	%ymm4, %ymm1, %ymm4
	vpunpckhwd	%ymm0, %ymm11, %ymm0
	vpmulhw	416+qdata_7681(%rip), %ymm2, %ymm1
	vpmullw	1984+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm13, %ymm2, %ymm2
	vpunpcklwd	%ymm0, %ymm12, %ymm11
	vpunpckhwd	%ymm0, %ymm12, %ymm0
	vpunpcklwd	%ymm0, %ymm11, %ymm12
	vpunpckhwd	%ymm0, %ymm11, %ymm0
	vpmulhw	480+qdata_7681(%rip), %ymm0, %ymm11
	vpunpcklwd	%ymm4, %ymm7, %ymm8
	vpmullw	2048+qdata_7681(%rip), %ymm0, %ymm0
	vpmulhw	%ymm13, %ymm0, %ymm0
	vpunpckhwd	%ymm4, %ymm7, %ymm7
	vpaddw	%ymm10, %ymm6, %ymm6
	vpunpcklwd	%ymm7, %ymm8, %ymm4
	vpunpckhwd	%ymm7, %ymm8, %ymm8
	vpaddw	%ymm9, %ymm3, %ymm3
	vpunpcklwd	%ymm6, %ymm3, %ymm7
	vpunpckhwd	%ymm6, %ymm3, %ymm3
	vpsubw	%ymm2, %ymm1, %ymm2
	vpmulhw	384+qdata_7681(%rip), %ymm8, %ymm1
	vpunpcklwd	%ymm3, %ymm7, %ymm6
	vpmullw	1952+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm13, %ymm8, %ymm8
	vpunpckhwd	%ymm3, %ymm7, %ymm3
	vpsubw	%ymm0, %ymm11, %ymm11
	vpmulhw	352+qdata_7681(%rip), %ymm5, %ymm0
	vpunpcklwd	%ymm3, %ymm6, %ymm7
	vpmullw	1920+qdata_7681(%rip), %ymm5, %ymm5
	vpmulhw	%ymm13, %ymm5, %ymm5
	vpunpckhwd	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm1, %ymm3
	vpmulhw	448+qdata_7681(%rip), %ymm6, %ymm1
	vpmullw	2016+qdata_7681(%rip), %ymm6, %ymm6
	vpmulhw	%ymm13, %ymm6, %ymm6
	vpsubw	%ymm5, %ymm0, %ymm5
	vpmulhw	320+qdata_7681(%rip), %ymm4, %ymm0
	vpmullw	1888+qdata_7681(%rip), %ymm4, %ymm4
	vpmulhw	%ymm13, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm1, %ymm1
	vmovdqa	-48(%rbp), %ymm6
	vpsubw	%ymm4, %ymm0, %ymm4
	vpmulhrsw	3232+qdata_7681(%rip), %ymm12, %ymm0
	vpmullw	%ymm6, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm12, %ymm12
	vpmulhrsw	3232+qdata_7681(%rip), %ymm7, %ymm0
	vpmullw	%ymm6, %ymm0, %ymm0
	vpaddw	%ymm11, %ymm2, %ymm6
	vpsubw	%ymm2, %ymm11, %ymm2
	vpmulhw	%ymm14, %ymm2, %ymm11
	vpmullw	%ymm2, %ymm15, %ymm2
	vpsubw	%ymm0, %ymm7, %ymm0
	vpaddw	%ymm1, %ymm3, %ymm7
	vpsubw	%ymm3, %ymm1, %ymm1
	vpmulhw	%ymm14, %ymm1, %ymm3
	vpmullw	%ymm1, %ymm15, %ymm1
	vpmulhw	%ymm13, %ymm2, %ymm2
	vpmulhw	%ymm13, %ymm1, %ymm1
	vpsubw	%ymm2, %ymm11, %ymm2
	vpsubw	%ymm1, %ymm3, %ymm1
	vpaddw	%ymm12, %ymm5, %ymm3
	vpsubw	%ymm5, %ymm12, %ymm12
	vpaddw	%ymm0, %ymm4, %ymm5
	vpsubw	%ymm4, %ymm0, %ymm0
	vpaddw	%ymm5, %ymm7, %ymm4
	vmovdqu	%ymm4, -256(%rdi)
	vpaddw	%ymm0, %ymm1, %ymm4
	vpsubw	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, -160(%rdi)
	vpaddw	%ymm3, %ymm6, %ymm0
	vpsubw	%ymm7, %ymm5, %ymm7
	vmovdqu	%ymm0, -128(%rdi)
	vpsubw	%ymm6, %ymm3, %ymm6
	vpaddw	%ymm12, %ymm2, %ymm0
	vpsubw	%ymm2, %ymm12, %ymm12
	vmovdqu	%ymm4, -224(%rdi)
	vmovdqu	%ymm7, -192(%rdi)
	vmovdqu	%ymm0, -96(%rdi)
	vmovdqu	%ymm6, -64(%rdi)
	vmovdqu	%ymm12, -32(%rdi)
	cmpq	%rdx, %rdi
	jne	.L37
.L46:
	leal	3(%r8), %eax
	addq	%r9, %rdi
	testl	%r8d, %r8d
	cmovns	%r8d, %eax
	sarl	$2, %eax
	movl	%eax, %ecx
	movslq	%eax, %rsi
	sall	$9, %ecx
	movslq	%ecx, %rcx
	addq	%rcx, %rcx
	movq	%rcx, %r8
	negq	%r8
	testl	%eax, %eax
	jle	.L57
	vmovdqa	1536+qdata_7681(%rip), %ymm9
	movq	%rdi, %rax
	xorl	%edx, %edx
	vmovdqa	3232+qdata_7681(%rip), %ymm10
	vmovdqa	3296+qdata_7681(%rip), %ymm11
	vmovdqa	3136+qdata_7681(%rip), %ymm12
	vmovdqa	%ymm9, %ymm4
	.p2align 4,,10
	.p2align 3
.L39:
	vmovdqu	480(%rax), %ymm0
	vmovdqu	224(%rax), %ymm1
	addq	$1, %rdx
	addq	$1024, %rax
	vmovdqu	-32(%rax), %ymm8
	vmovdqu	-672(%rax), %ymm15
	vmovdqu	-928(%rax), %ymm13
	vmovdqu	-160(%rax), %ymm5
	vperm2i128	$32, %ymm0, %ymm15, %ymm6
	vperm2i128	$49, %ymm0, %ymm15, %ymm15
	vmovdqu	-288(%rax), %ymm14
	vmovdqu	-416(%rax), %ymm2
	vperm2i128	$32, %ymm8, %ymm5, %ymm7
	vperm2i128	$32, %ymm1, %ymm13, %ymm0
	vperm2i128	$49, %ymm8, %ymm5, %ymm5
	vperm2i128	$49, %ymm1, %ymm13, %ymm13
	vpmulhw	1248+qdata_7681(%rip), %ymm5, %ymm1
	vperm2i128	$32, %ymm14, %ymm2, %ymm3
	vpmullw	2816+qdata_7681(%rip), %ymm5, %ymm5
	vpmulhw	%ymm4, %ymm5, %ymm5
	vperm2i128	$49, %ymm14, %ymm2, %ymm2
	vpsubw	%ymm5, %ymm1, %ymm8
	vpmulhw	1120+qdata_7681(%rip), %ymm7, %ymm5
	vpmullw	2688+qdata_7681(%rip), %ymm7, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpmulhw	1376+qdata_7681(%rip), %ymm2, %ymm7
	vpmullw	2944+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm1, %ymm5, %ymm1
	vpsubw	%ymm2, %ymm7, %ymm14
	vpmulhw	1504+qdata_7681(%rip), %ymm3, %ymm2
	vpmullw	3072+qdata_7681(%rip), %ymm3, %ymm7
	vpmulhw	%ymm4, %ymm7, %ymm7
	vpmulhw	864+qdata_7681(%rip), %ymm15, %ymm3
	vpmullw	2432+qdata_7681(%rip), %ymm15, %ymm15
	vpmulhw	%ymm4, %ymm15, %ymm15
	vpsubw	%ymm7, %ymm2, %ymm7
	vpmulhrsw	%ymm10, %ymm0, %ymm2
	vpsubw	%ymm15, %ymm3, %ymm5
	vpmulhw	992+qdata_7681(%rip), %ymm6, %ymm3
	vpmullw	2560+qdata_7681(%rip), %ymm6, %ymm6
	vpmulhw	%ymm4, %ymm6, %ymm6
	vpmullw	%ymm9, %ymm2, %ymm2
	vpsubw	%ymm6, %ymm3, %ymm3
	vpmulhw	736+qdata_7681(%rip), %ymm13, %ymm6
	vpmullw	2304+qdata_7681(%rip), %ymm13, %ymm13
	vpmulhw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm2, %ymm0, %ymm0
	vpsubw	%ymm13, %ymm6, %ymm6
	vpaddw	%ymm1, %ymm8, %ymm13
	vpsubw	%ymm8, %ymm1, %ymm1
	vpmulhw	3328+qdata_7681(%rip), %ymm1, %ymm2
	vpmullw	3168+qdata_7681(%rip), %ymm1, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm2, %ymm8
	vpaddw	%ymm7, %ymm14, %ymm1
	vpsubw	%ymm14, %ymm7, %ymm7
	vpmulhw	3360+qdata_7681(%rip), %ymm7, %ymm2
	vpmullw	3200+qdata_7681(%rip), %ymm7, %ymm7
	vpmulhw	%ymm4, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm2, %ymm2
	vpaddw	%ymm3, %ymm5, %ymm7
	vpsubw	%ymm5, %ymm3, %ymm3
	vpmulhw	%ymm11, %ymm3, %ymm5
	vpmullw	%ymm12, %ymm3, %ymm3
	vpmulhw	%ymm4, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm5, %ymm5
	vpaddw	%ymm0, %ymm6, %ymm3
	vpsubw	%ymm6, %ymm0, %ymm0
	vpaddw	%ymm2, %ymm8, %ymm6
	vpsubw	%ymm8, %ymm2, %ymm2
	vpaddw	%ymm0, %ymm5, %ymm14
	vpmulhw	%ymm11, %ymm2, %ymm8
	vpmullw	%ymm2, %ymm12, %ymm2
	vpsubw	%ymm5, %ymm0, %ymm0
	vpaddw	%ymm3, %ymm7, %ymm5
	vpsubw	%ymm7, %ymm3, %ymm3
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vpaddw	%ymm1, %ymm13, %ymm8
	vpsubw	%ymm13, %ymm1, %ymm1
	vpmulhw	%ymm11, %ymm1, %ymm13
	vpmullw	%ymm1, %ymm12, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm13, %ymm1
	vpmulhrsw	%ymm10, %ymm8, %ymm13
	vpmullw	%ymm9, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm8, %ymm8
	vpmulhrsw	%ymm10, %ymm5, %ymm13
	vpmullw	%ymm9, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm5, %ymm13
	vpaddw	%ymm13, %ymm8, %ymm5
	vpsubw	%ymm8, %ymm13, %ymm13
	vmovdqu	%ymm5, -928(%rax)
	vpaddw	%ymm14, %ymm6, %ymm5
	vpsubw	%ymm6, %ymm14, %ymm6
	vmovdqu	%ymm5, -800(%rax)
	vpaddw	%ymm3, %ymm1, %ymm5
	vpsubw	%ymm1, %ymm3, %ymm1
	vmovdqu	%ymm5, -672(%rax)
	vpaddw	%ymm0, %ymm2, %ymm5
	vpsubw	%ymm2, %ymm0, %ymm0
	vmovdqu	%ymm5, -544(%rax)
	vmovdqu	%ymm13, -416(%rax)
	vmovdqu	%ymm6, -288(%rax)
	vmovdqu	%ymm1, -160(%rax)
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rsi, %rdx
	jne	.L39
	salq	$10, %rdx
	vmovdqa	704+qdata_7681(%rip), %ymm15
	vmovdqa	2272+qdata_7681(%rip), %ymm14
	movq	%rdx, %rax
	subq	%rcx, %rax
	addq	%rax, %rdi
	leaq	(%rdi,%rdx), %rax
	.p2align 4,,10
	.p2align 3
.L41:
	vmovdqu	960(%rdi), %ymm5
	vmovdqu	704(%rdi), %ymm0
	addq	$1024, %rdi
	vmovdqu	-192(%rdi), %ymm8
	vmovdqu	-448(%rdi), %ymm6
	vmovdqu	-704(%rdi), %ymm7
	vmovdqu	-960(%rdi), %ymm1
	vperm2i128	$32, %ymm5, %ymm8, %ymm2
	vperm2i128	$32, %ymm0, %ymm6, %ymm13
	vperm2i128	$49, %ymm5, %ymm8, %ymm8
	vmovdqu	-832(%rdi), %ymm5
	vperm2i128	$49, %ymm0, %ymm6, %ymm6
	vmovdqu	-576(%rdi), %ymm0
	vperm2i128	$32, %ymm0, %ymm7, %ymm3
	vperm2i128	$49, %ymm0, %ymm7, %ymm7
	vperm2i128	$32, %ymm5, %ymm1, %ymm0
	vperm2i128	$49, %ymm5, %ymm1, %ymm1
	vpmulhw	1216+qdata_7681(%rip), %ymm8, %ymm5
	vpmullw	2784+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm4, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm5, %ymm5
	vpmulhw	1088+qdata_7681(%rip), %ymm2, %ymm8
	vpmullw	2656+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vpmulhw	1344+qdata_7681(%rip), %ymm6, %ymm8
	vpmullw	2912+qdata_7681(%rip), %ymm6, %ymm6
	vpmulhw	%ymm4, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm8, %ymm8
	vpmulhw	1472+qdata_7681(%rip), %ymm13, %ymm6
	vpmullw	3040+qdata_7681(%rip), %ymm13, %ymm13
	vpmulhw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm6, %ymm13
	vpmulhw	832+qdata_7681(%rip), %ymm7, %ymm6
	vpmullw	2400+qdata_7681(%rip), %ymm7, %ymm7
	vpmulhw	%ymm4, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpmulhw	960+qdata_7681(%rip), %ymm3, %ymm7
	vpmullw	2528+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm4, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm7, %ymm3
	vpmulhw	%ymm15, %ymm1, %ymm7
	vpmullw	%ymm1, %ymm14, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm7, %ymm7
	vpmulhrsw	%ymm10, %ymm0, %ymm1
	vpmullw	%ymm9, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vpaddw	%ymm2, %ymm5, %ymm1
	vpsubw	%ymm5, %ymm2, %ymm2
	vpmulhw	3328+qdata_7681(%rip), %ymm2, %ymm5
	vpmullw	3168+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm5, %ymm5
	vpaddw	%ymm13, %ymm8, %ymm2
	vpsubw	%ymm8, %ymm13, %ymm13
	vpmulhw	3360+qdata_7681(%rip), %ymm13, %ymm8
	vpmullw	3200+qdata_7681(%rip), %ymm13, %ymm13
	vpmulhw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm8, %ymm8
	vpaddw	%ymm3, %ymm6, %ymm13
	vpsubw	%ymm6, %ymm3, %ymm3
	vpmulhw	%ymm11, %ymm3, %ymm6
	vpmullw	%ymm3, %ymm12, %ymm3
	vpmulhw	%ymm4, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm6, %ymm6
	vpaddw	%ymm0, %ymm7, %ymm3
	vpsubw	%ymm7, %ymm0, %ymm0
	vpaddw	%ymm8, %ymm5, %ymm7
	vpsubw	%ymm5, %ymm8, %ymm8
	vpmulhw	%ymm11, %ymm8, %ymm5
	vpmullw	%ymm8, %ymm12, %ymm8
	vpmulhw	%ymm4, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm5, %ymm5
	vpaddw	%ymm2, %ymm1, %ymm8
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmulhw	%ymm11, %ymm2, %ymm1
	vpmullw	%ymm2, %ymm12, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vpaddw	%ymm0, %ymm6, %ymm2
	vpsubw	%ymm6, %ymm0, %ymm0
	vpaddw	%ymm3, %ymm13, %ymm6
	vpsubw	%ymm13, %ymm3, %ymm3
	vpmulhrsw	%ymm10, %ymm8, %ymm13
	vpmullw	%ymm9, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm8, %ymm13
	vpmulhrsw	%ymm10, %ymm6, %ymm8
	vpmullw	%ymm9, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm6, %ymm8
	vpaddw	%ymm8, %ymm13, %ymm6
	vpsubw	%ymm13, %ymm8, %ymm8
	vmovdqu	%ymm6, -960(%rdi)
	vpaddw	%ymm2, %ymm7, %ymm6
	vpsubw	%ymm7, %ymm2, %ymm7
	vmovdqu	%ymm6, -832(%rdi)
	vpaddw	%ymm3, %ymm1, %ymm6
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	%ymm6, -704(%rdi)
	vpaddw	%ymm0, %ymm5, %ymm6
	vpsubw	%ymm5, %ymm0, %ymm0
	vmovdqu	%ymm6, -576(%rdi)
	vmovdqu	%ymm8, -448(%rdi)
	vmovdqu	%ymm7, -320(%rdi)
	vmovdqu	%ymm3, -192(%rdi)
	vmovdqu	%ymm0, -64(%rdi)
	cmpq	%rax, %rdi
	jne	.L41
	addq	%r8, %rdi
	vmovdqa	672+qdata_7681(%rip), %ymm15
	vmovdqa	2240+qdata_7681(%rip), %ymm14
	leaq	(%rdi,%rdx), %rax
	.p2align 4,,10
	.p2align 3
.L43:
	vmovdqu	928(%rdi), %ymm5
	vmovdqu	672(%rdi), %ymm0
	addq	$1024, %rdi
	vmovdqu	-224(%rdi), %ymm8
	vmovdqu	-480(%rdi), %ymm6
	vmovdqu	-736(%rdi), %ymm7
	vmovdqu	-992(%rdi), %ymm1
	vperm2i128	$32, %ymm5, %ymm8, %ymm2
	vperm2i128	$32, %ymm0, %ymm6, %ymm13
	vperm2i128	$49, %ymm5, %ymm8, %ymm8
	vmovdqu	-864(%rdi), %ymm5
	vperm2i128	$49, %ymm0, %ymm6, %ymm6
	vmovdqu	-608(%rdi), %ymm0
	vperm2i128	$32, %ymm0, %ymm7, %ymm3
	vperm2i128	$49, %ymm0, %ymm7, %ymm7
	vperm2i128	$32, %ymm5, %ymm1, %ymm0
	vperm2i128	$49, %ymm5, %ymm1, %ymm1
	vpmulhw	1184+qdata_7681(%rip), %ymm8, %ymm5
	vpmullw	2752+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm4, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm5, %ymm5
	vpmulhw	1056+qdata_7681(%rip), %ymm2, %ymm8
	vpmullw	2624+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vpmulhw	1312+qdata_7681(%rip), %ymm6, %ymm8
	vpmullw	2880+qdata_7681(%rip), %ymm6, %ymm6
	vpmulhw	%ymm4, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm8, %ymm8
	vpmulhw	1440+qdata_7681(%rip), %ymm13, %ymm6
	vpmullw	3008+qdata_7681(%rip), %ymm13, %ymm13
	vpmulhw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm6, %ymm13
	vpmulhw	800+qdata_7681(%rip), %ymm7, %ymm6
	vpmullw	2368+qdata_7681(%rip), %ymm7, %ymm7
	vpmulhw	%ymm4, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpmulhw	928+qdata_7681(%rip), %ymm3, %ymm7
	vpmullw	2496+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm4, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm7, %ymm3
	vpmulhw	%ymm15, %ymm1, %ymm7
	vpmullw	%ymm1, %ymm14, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm7, %ymm7
	vpmulhrsw	%ymm10, %ymm0, %ymm1
	vpmullw	%ymm9, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vpaddw	%ymm2, %ymm5, %ymm1
	vpsubw	%ymm5, %ymm2, %ymm2
	vpmulhw	3328+qdata_7681(%rip), %ymm2, %ymm5
	vpmullw	3168+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm5, %ymm5
	vpaddw	%ymm13, %ymm8, %ymm2
	vpsubw	%ymm8, %ymm13, %ymm13
	vpmulhw	3360+qdata_7681(%rip), %ymm13, %ymm8
	vpmullw	3200+qdata_7681(%rip), %ymm13, %ymm13
	vpmulhw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm8, %ymm8
	vpaddw	%ymm3, %ymm6, %ymm13
	vpsubw	%ymm6, %ymm3, %ymm3
	vpmulhw	%ymm11, %ymm3, %ymm6
	vpmullw	%ymm3, %ymm12, %ymm3
	vpmulhw	%ymm4, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm6, %ymm6
	vpaddw	%ymm0, %ymm7, %ymm3
	vpsubw	%ymm7, %ymm0, %ymm0
	vpaddw	%ymm8, %ymm5, %ymm7
	vpsubw	%ymm5, %ymm8, %ymm8
	vpmulhw	%ymm11, %ymm8, %ymm5
	vpmullw	%ymm8, %ymm12, %ymm8
	vpmulhw	%ymm4, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm5, %ymm5
	vpaddw	%ymm2, %ymm1, %ymm8
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmulhw	%ymm11, %ymm2, %ymm1
	vpmullw	%ymm2, %ymm12, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vpaddw	%ymm0, %ymm6, %ymm2
	vpsubw	%ymm6, %ymm0, %ymm0
	vpaddw	%ymm3, %ymm13, %ymm6
	vpsubw	%ymm13, %ymm3, %ymm3
	vpmulhrsw	%ymm10, %ymm8, %ymm13
	vpmullw	%ymm9, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm8, %ymm13
	vpmulhrsw	%ymm10, %ymm6, %ymm8
	vpmullw	%ymm9, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm6, %ymm8
	vpaddw	%ymm8, %ymm13, %ymm6
	vpsubw	%ymm13, %ymm8, %ymm8
	vmovdqu	%ymm6, -992(%rdi)
	vpaddw	%ymm2, %ymm7, %ymm6
	vpsubw	%ymm7, %ymm2, %ymm7
	vmovdqu	%ymm6, -864(%rdi)
	vpaddw	%ymm3, %ymm1, %ymm6
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	%ymm6, -736(%rdi)
	vpaddw	%ymm0, %ymm5, %ymm6
	vpsubw	%ymm5, %ymm0, %ymm0
	vmovdqu	%ymm6, -608(%rdi)
	vmovdqu	%ymm8, -480(%rdi)
	vmovdqu	%ymm7, -352(%rdi)
	vmovdqu	%ymm3, -224(%rdi)
	vmovdqu	%ymm0, -96(%rdi)
	cmpq	%rax, %rdi
	jne	.L43
	addq	%r8, %rdi
	vmovdqa	640+qdata_7681(%rip), %ymm15
	vmovdqa	2208+qdata_7681(%rip), %ymm14
	addq	%rdi, %rdx
	.p2align 4,,10
	.p2align 3
.L45:
	vmovdqu	896(%rdi), %ymm5
	vmovdqu	640(%rdi), %ymm0
	addq	$1024, %rdi
	vmovdqu	-256(%rdi), %ymm8
	vmovdqu	-512(%rdi), %ymm6
	vmovdqu	-768(%rdi), %ymm7
	vmovdqu	-1024(%rdi), %ymm1
	vperm2i128	$32, %ymm5, %ymm8, %ymm2
	vperm2i128	$32, %ymm0, %ymm6, %ymm13
	vperm2i128	$49, %ymm5, %ymm8, %ymm8
	vmovdqu	-896(%rdi), %ymm5
	vperm2i128	$49, %ymm0, %ymm6, %ymm6
	vmovdqu	-640(%rdi), %ymm0
	vperm2i128	$32, %ymm0, %ymm7, %ymm3
	vperm2i128	$49, %ymm0, %ymm7, %ymm7
	vperm2i128	$32, %ymm5, %ymm1, %ymm0
	vperm2i128	$49, %ymm5, %ymm1, %ymm1
	vpmulhw	1152+qdata_7681(%rip), %ymm8, %ymm5
	vpmullw	2720+qdata_7681(%rip), %ymm8, %ymm8
	vpmulhw	%ymm4, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm5, %ymm5
	vpmulhw	1024+qdata_7681(%rip), %ymm2, %ymm8
	vpmullw	2592+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm8, %ymm2
	vpmulhw	1280+qdata_7681(%rip), %ymm6, %ymm8
	vpmullw	2848+qdata_7681(%rip), %ymm6, %ymm6
	vpmulhw	%ymm4, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm8, %ymm8
	vpmulhw	1408+qdata_7681(%rip), %ymm13, %ymm6
	vpmullw	2976+qdata_7681(%rip), %ymm13, %ymm13
	vpmulhw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm6, %ymm13
	vpmulhw	768+qdata_7681(%rip), %ymm7, %ymm6
	vpmullw	2336+qdata_7681(%rip), %ymm7, %ymm7
	vpmulhw	%ymm4, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpmulhw	896+qdata_7681(%rip), %ymm3, %ymm7
	vpmullw	2464+qdata_7681(%rip), %ymm3, %ymm3
	vpmulhw	%ymm4, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm7, %ymm3
	vpmulhw	%ymm15, %ymm1, %ymm7
	vpmullw	%ymm1, %ymm14, %ymm1
	vpmulhw	%ymm4, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm7, %ymm7
	vpmulhrsw	%ymm10, %ymm0, %ymm1
	vpmullw	%ymm9, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vpaddw	%ymm2, %ymm5, %ymm1
	vpsubw	%ymm5, %ymm2, %ymm2
	vpmulhw	3328+qdata_7681(%rip), %ymm2, %ymm5
	vpmullw	3168+qdata_7681(%rip), %ymm2, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm5, %ymm5
	vpaddw	%ymm13, %ymm8, %ymm2
	vpsubw	%ymm8, %ymm13, %ymm13
	vpmulhw	3360+qdata_7681(%rip), %ymm13, %ymm8
	vpmullw	3200+qdata_7681(%rip), %ymm13, %ymm13
	vpmulhw	%ymm4, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm8, %ymm8
	vpaddw	%ymm3, %ymm6, %ymm13
	vpsubw	%ymm6, %ymm3, %ymm3
	vpmulhw	%ymm11, %ymm3, %ymm6
	vpmullw	%ymm3, %ymm12, %ymm3
	vpmulhw	%ymm4, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm6, %ymm6
	vpaddw	%ymm0, %ymm7, %ymm3
	vpsubw	%ymm7, %ymm0, %ymm0
	vpaddw	%ymm8, %ymm5, %ymm7
	vpsubw	%ymm5, %ymm8, %ymm8
	vpmulhw	%ymm11, %ymm8, %ymm5
	vpmullw	%ymm8, %ymm12, %ymm8
	vpmulhw	%ymm4, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm5, %ymm5
	vpaddw	%ymm2, %ymm1, %ymm8
	vpsubw	%ymm1, %ymm2, %ymm2
	vpmulhw	%ymm11, %ymm2, %ymm1
	vpmullw	%ymm2, %ymm12, %ymm2
	vpmulhw	%ymm4, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vpaddw	%ymm0, %ymm6, %ymm2
	vpsubw	%ymm6, %ymm0, %ymm0
	vpaddw	%ymm3, %ymm13, %ymm6
	vpsubw	%ymm13, %ymm3, %ymm3
	vpmulhrsw	%ymm10, %ymm8, %ymm13
	vpmullw	%ymm9, %ymm13, %ymm13
	vpsubw	%ymm13, %ymm8, %ymm13
	vpmulhrsw	%ymm10, %ymm6, %ymm8
	vpmullw	%ymm9, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm6, %ymm8
	vpaddw	%ymm8, %ymm13, %ymm6
	vpsubw	%ymm13, %ymm8, %ymm8
	vmovdqu	%ymm6, -1024(%rdi)
	vpaddw	%ymm2, %ymm7, %ymm6
	vpsubw	%ymm7, %ymm2, %ymm7
	vmovdqu	%ymm6, -896(%rdi)
	vpaddw	%ymm3, %ymm1, %ymm6
	vpsubw	%ymm1, %ymm3, %ymm3
	vmovdqu	%ymm6, -768(%rdi)
	vpaddw	%ymm0, %ymm5, %ymm6
	vpsubw	%ymm5, %ymm0, %ymm0
	vmovdqu	%ymm6, -640(%rdi)
	vmovdqu	%ymm8, -512(%rdi)
	vmovdqu	%ymm7, -384(%rdi)
	vmovdqu	%ymm3, -256(%rdi)
	vmovdqu	%ymm0, -128(%rdi)
	cmpq	%rdx, %rdi
	jne	.L45
.L57:
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
.L34:
	.cfi_restore_state
	addq	%r9, %rdi
	jmp	.L46
	.cfi_endproc
.LFE4758:
	.size	CRYPTO_NAMESPACE(invntt512_7681), .-CRYPTO_NAMESPACE(invntt512_7681)
	.section	.rodata
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
