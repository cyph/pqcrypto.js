	.file	"small.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(small_encode)
	.type	CRYPTO_NAMESPACE(small_encode), @function
CRYPTO_NAMESPACE(small_encode):
.LFB5279:
	.cfi_startproc
	leaq	760(%rsi), %r8
	leaq	190(%rdi), %r9
	cmpq	%r8, %rdi
	jnb	.L7
	cmpq	%r9, %rsi
	jb	.L6
.L7:
	vmovdqa	.LC0(%rip), %ymm0
	vpand	32(%rsi), %ymm0, %ymm1
	leaq	160(%rdi), %r8
	leaq	640(%rsi), %rdx
	vpand	(%rsi), %ymm0, %ymm4
	vpand	64(%rsi), %ymm0, %ymm7
	vpackuswb	%ymm1, %ymm4, %ymm4
	vmovdqu	(%rsi), %ymm1
	vpermq	$216, %ymm4, %ymm4
	vpsrlw	$8, %ymm1, %ymm2
	vmovdqu	32(%rsi), %ymm1
	vpsrlw	$8, %ymm4, %ymm5
	vpand	%ymm4, %ymm0, %ymm4
	vpsrlw	$8, %ymm1, %ymm1
	vpackuswb	%ymm1, %ymm2, %ymm2
	vpand	96(%rsi), %ymm0, %ymm1
	vpermq	$216, %ymm2, %ymm2
	vpackuswb	%ymm1, %ymm7, %ymm7
	vmovdqu	64(%rsi), %ymm1
	vpand	%ymm2, %ymm0, %ymm3
	vpermq	$216, %ymm7, %ymm7
	vpsrlw	$8, %ymm2, %ymm2
	vpsrlw	$8, %ymm1, %ymm6
	vmovdqu	96(%rsi), %ymm1
	vpsrlw	$8, %ymm7, %ymm8
	vpand	%ymm7, %ymm0, %ymm7
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpackuswb	%ymm7, %ymm4, %ymm4
	vpsrlw	$8, %ymm1, %ymm1
	vpermq	$216, %ymm5, %ymm5
	vpermq	$216, %ymm4, %ymm4
	vpackuswb	%ymm1, %ymm6, %ymm6
	vpermq	$216, %ymm6, %ymm6
	vpand	%ymm6, %ymm0, %ymm1
	vpsrlw	$8, %ymm6, %ymm6
	vpackuswb	%ymm1, %ymm3, %ymm3
	vpackuswb	%ymm6, %ymm2, %ymm2
	vmovdqa	.LC1(%rip), %ymm1
	vpermq	$216, %ymm2, %ymm2
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm5, %ymm1, %ymm5
	vpaddb	%ymm2, %ymm1, %ymm2
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm5, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm4, %ymm1, %ymm4
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm4, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm3, %ymm2
	vmovdqu	%ymm2, (%rdi)
	vpand	160(%rsi), %ymm0, %ymm2
	vpand	128(%rsi), %ymm0, %ymm4
	vmovdqu	128(%rsi), %ymm7
	vmovdqu	160(%rsi), %ymm6
	vpackuswb	%ymm2, %ymm4, %ymm4
	vmovdqu	224(%rsi), %ymm5
	vpsrlw	$8, %ymm6, %ymm3
	vpsrlw	$8, %ymm7, %ymm2
	vpermq	$216, %ymm4, %ymm4
	vmovdqu	192(%rsi), %ymm6
	vpand	192(%rsi), %ymm0, %ymm7
	vpackuswb	%ymm3, %ymm2, %ymm2
	vpand	224(%rsi), %ymm0, %ymm3
	vpsrlw	$8, %ymm6, %ymm6
	vpermq	$216, %ymm2, %ymm2
	vpackuswb	%ymm3, %ymm7, %ymm7
	vpsrlw	$8, %ymm5, %ymm3
	vpackuswb	%ymm3, %ymm6, %ymm6
	vpermq	$216, %ymm7, %ymm7
	vpand	%ymm2, %ymm0, %ymm3
	vpsrlw	$8, %ymm7, %ymm8
	vpermq	$216, %ymm6, %ymm6
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm6, %ymm0, %ymm5
	vpsrlw	$8, %ymm2, %ymm2
	vpackuswb	%ymm5, %ymm3, %ymm3
	vpsrlw	$8, %ymm6, %ymm6
	vpsrlw	$8, %ymm4, %ymm5
	vpackuswb	%ymm6, %ymm2, %ymm2
	vpermq	$216, %ymm3, %ymm3
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm4, %ymm0, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm1, %ymm2
	vpaddb	%ymm3, %ymm1, %ymm3
	vpaddb	%ymm5, %ymm1, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpackuswb	%ymm7, %ymm4, %ymm4
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpermq	$216, %ymm4, %ymm4
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm5, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm4, %ymm1, %ymm4
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm4, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm3, %ymm2
	vmovdqu	%ymm2, 32(%rdi)
	vpand	288(%rsi), %ymm0, %ymm2
	vpand	256(%rsi), %ymm0, %ymm4
	vmovdqu	256(%rsi), %ymm7
	vmovdqu	352(%rsi), %ymm5
	vpackuswb	%ymm2, %ymm4, %ymm4
	vmovdqu	320(%rsi), %ymm6
	vpsrlw	$8, %ymm7, %ymm2
	vmovdqu	288(%rsi), %ymm7
	vpermq	$216, %ymm4, %ymm4
	vpsrlw	$8, %ymm6, %ymm6
	vpsrlw	$8, %ymm7, %ymm3
	vpand	320(%rsi), %ymm0, %ymm7
	vpackuswb	%ymm3, %ymm2, %ymm2
	vpand	352(%rsi), %ymm0, %ymm3
	vpermq	$216, %ymm2, %ymm2
	vpackuswb	%ymm3, %ymm7, %ymm7
	vpsrlw	$8, %ymm5, %ymm3
	vpackuswb	%ymm3, %ymm6, %ymm6
	vpermq	$216, %ymm7, %ymm7
	vpand	%ymm2, %ymm0, %ymm3
	vpsrlw	$8, %ymm7, %ymm8
	vpermq	$216, %ymm6, %ymm6
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm6, %ymm0, %ymm5
	vpsrlw	$8, %ymm2, %ymm2
	vpackuswb	%ymm5, %ymm3, %ymm3
	vpsrlw	$8, %ymm6, %ymm6
	vpsrlw	$8, %ymm4, %ymm5
	vpackuswb	%ymm6, %ymm2, %ymm2
	vpermq	$216, %ymm3, %ymm3
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm4, %ymm0, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm1, %ymm2
	vpaddb	%ymm3, %ymm1, %ymm3
	vpaddb	%ymm5, %ymm1, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpackuswb	%ymm7, %ymm4, %ymm4
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpermq	$216, %ymm4, %ymm4
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm5, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm4, %ymm1, %ymm4
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm4, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm3, %ymm2
	vmovdqu	%ymm2, 64(%rdi)
	vpand	416(%rsi), %ymm0, %ymm2
	vpand	384(%rsi), %ymm0, %ymm4
	vmovdqu	384(%rsi), %ymm7
	vmovdqu	416(%rsi), %ymm6
	vpackuswb	%ymm2, %ymm4, %ymm4
	vmovdqu	480(%rsi), %ymm5
	vpsrlw	$8, %ymm6, %ymm3
	vpsrlw	$8, %ymm7, %ymm2
	vpermq	$216, %ymm4, %ymm4
	vmovdqu	448(%rsi), %ymm6
	vpand	448(%rsi), %ymm0, %ymm7
	vpackuswb	%ymm3, %ymm2, %ymm2
	vpand	480(%rsi), %ymm0, %ymm3
	vpsrlw	$8, %ymm6, %ymm6
	vpermq	$216, %ymm2, %ymm2
	vpackuswb	%ymm3, %ymm7, %ymm7
	vpsrlw	$8, %ymm5, %ymm3
	vpackuswb	%ymm3, %ymm6, %ymm6
	vpermq	$216, %ymm7, %ymm7
	vpand	%ymm2, %ymm0, %ymm3
	vpsrlw	$8, %ymm7, %ymm8
	vpermq	$216, %ymm6, %ymm6
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm6, %ymm0, %ymm5
	vpsrlw	$8, %ymm2, %ymm2
	vpackuswb	%ymm5, %ymm3, %ymm3
	vpsrlw	$8, %ymm6, %ymm6
	vpsrlw	$8, %ymm4, %ymm5
	vpackuswb	%ymm6, %ymm2, %ymm2
	vpermq	$216, %ymm3, %ymm3
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm4, %ymm0, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm1, %ymm2
	vpaddb	%ymm3, %ymm1, %ymm3
	vpaddb	%ymm5, %ymm1, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpackuswb	%ymm7, %ymm4, %ymm4
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpermq	$216, %ymm4, %ymm4
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm5, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm4, %ymm1, %ymm4
	vpaddb	%ymm3, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm4, %ymm3, %ymm3
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm3, %ymm2
	vmovdqu	%ymm2, 96(%rdi)
	vmovdqu	512(%rsi), %ymm7
	vpand	544(%rsi), %ymm0, %ymm2
	vpand	512(%rsi), %ymm0, %ymm4
	vmovdqu	608(%rsi), %ymm5
	vpsrlw	$8, %ymm7, %ymm3
	vmovdqu	544(%rsi), %ymm7
	vmovdqu	576(%rsi), %ymm6
	vpackuswb	%ymm2, %ymm4, %ymm4
	vpsrlw	$8, %ymm7, %ymm2
	vpsrlw	$8, %ymm6, %ymm6
	vpermq	$216, %ymm4, %ymm4
	vpand	576(%rsi), %ymm0, %ymm7
	vpackuswb	%ymm2, %ymm3, %ymm3
	vpand	608(%rsi), %ymm0, %ymm2
	vpermq	$216, %ymm3, %ymm3
	vpackuswb	%ymm2, %ymm7, %ymm7
	vpsrlw	$8, %ymm5, %ymm2
	vpackuswb	%ymm2, %ymm6, %ymm6
	vpermq	$216, %ymm7, %ymm7
	vpand	%ymm3, %ymm0, %ymm2
	vpermq	$216, %ymm6, %ymm6
	vpsrlw	$8, %ymm7, %ymm8
	vpand	%ymm6, %ymm0, %ymm5
	vpsrlw	$8, %ymm6, %ymm6
	vpackuswb	%ymm5, %ymm2, %ymm2
	vpsrlw	$8, %ymm4, %ymm5
	vpand	%ymm4, %ymm0, %ymm4
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm7, %ymm0, %ymm0
	vpermq	$216, %ymm5, %ymm5
	vpaddb	%ymm2, %ymm1, %ymm2
	vpackuswb	%ymm0, %ymm4, %ymm0
	vpaddb	%ymm5, %ymm1, %ymm5
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm0, %ymm1, %ymm0
	vpaddb	%ymm5, %ymm5, %ymm5
	vpaddb	%ymm5, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm2, %ymm2
	vpaddb	%ymm0, %ymm2, %ymm2
	vpsrlw	$8, %ymm3, %ymm0
	vpackuswb	%ymm6, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm1, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm2, %ymm0
	vmovdqu	%ymm0, 128(%rdi)
	.p2align 4,,10
	.p2align 3
.L4:
	movzbl	(%rdx), %ecx
	movsbl	1(%rdx), %eax
	addq	$4, %rdx
	incq	%r8
	leal	5(%rcx,%rax,4), %eax
	movsbl	-2(%rdx), %ecx
	incl	%ecx
	sall	$4, %ecx
	addl	%ecx, %eax
	movsbl	-1(%rdx), %ecx
	incl	%ecx
	sall	$6, %ecx
	addl	%ecx, %eax
	movb	%al, -1(%r8)
	cmpq	%r8, %r9
	jne	.L4
	vzeroupper
	movzbl	760(%rsi), %eax
	incl	%eax
	movb	%al, 190(%rdi)
	ret
.L6:
	movq	%rsi, %rdx
	movq	%rdi, %r9
	.p2align 4,,10
	.p2align 3
.L2:
	movzbl	(%rdx), %ecx
	movsbl	1(%rdx), %eax
	addq	$4, %rdx
	incq	%r9
	leal	5(%rcx,%rax,4), %eax
	movsbl	-2(%rdx), %ecx
	incl	%ecx
	sall	$4, %ecx
	addl	%ecx, %eax
	movsbl	-1(%rdx), %ecx
	incl	%ecx
	sall	$6, %ecx
	addl	%ecx, %eax
	movb	%al, -1(%r9)
	cmpq	%r8, %rdx
	jne	.L2
	movzbl	760(%rsi), %eax
	incl	%eax
	movb	%al, 190(%rdi)
	ret
	.cfi_endproc
.LFE5279:
	.size	CRYPTO_NAMESPACE(small_encode), .-CRYPTO_NAMESPACE(small_encode)
	.p2align 4
	.globl	CRYPTO_NAMESPACE(small_decode)
	.type	CRYPTO_NAMESPACE(small_decode), @function
CRYPTO_NAMESPACE(small_decode):
.LFB5280:
	.cfi_startproc
	leaq	190(%rsi), %r8
	leaq	760(%rdi), %r9
	cmpq	%r8, %rdi
	jnb	.L17
	cmpq	%r9, %rsi
	jb	.L16
.L17:
	vmovdqu	(%rsi), %ymm0
	vmovdqa	.LC2(%rip), %ymm2
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	leaq	640(%rdi), %rdx
	leaq	160(%rsi), %r8
	vpmovzxbw	%xmm0, %ymm3
	vpand	%ymm2, %ymm0, %ymm6
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovzxbw	%xmm0, %ymm7
	vpsrlw	$2, %ymm3, %ymm4
	vpaddb	%ymm1, %ymm6, %ymm6
	vmovdqa	.LC0(%rip), %ymm0
	vpsrlw	$2, %ymm7, %ymm5
	vpsrlw	$4, %ymm7, %ymm8
	vpand	%ymm5, %ymm0, %ymm5
	vpand	%ymm4, %ymm0, %ymm4
	vpand	%ymm8, %ymm0, %ymm8
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpsrlw	$6, %ymm7, %ymm7
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpand	%ymm7, %ymm0, %ymm7
	vpermq	$216, %ymm5, %ymm5
	vpand	%ymm4, %ymm2, %ymm4
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpand	%ymm5, %ymm2, %ymm5
	vpaddb	%ymm1, %ymm4, %ymm4
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$32, %ymm4, %ymm6, %ymm7
	vperm2i128	$49, %ymm4, %ymm6, %ymm4
	vmovdqu	%ymm4, 32(%rdi)
	vpunpcklbw	%ymm3, %ymm5, %ymm4
	vpunpckhbw	%ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm3, %ymm4, %ymm5
	vperm2i128	$49, %ymm3, %ymm4, %ymm3
	vmovdqu	%ymm7, (%rdi)
	vmovdqu	%ymm5, 64(%rdi)
	vmovdqu	%ymm3, 96(%rdi)
	vmovdqu	32(%rsi), %ymm4
	vpmovzxbw	%xmm4, %ymm3
	vpand	%ymm2, %ymm4, %ymm6
	vextracti128	$0x1, %ymm4, %xmm4
	vpmovzxbw	%xmm4, %ymm7
	vpsrlw	$2, %ymm3, %ymm4
	vpaddb	%ymm1, %ymm6, %ymm6
	vpsrlw	$2, %ymm7, %ymm5
	vpsrlw	$4, %ymm7, %ymm8
	vpand	%ymm4, %ymm0, %ymm4
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm7, %ymm7
	vpand	%ymm8, %ymm0, %ymm8
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpand	%ymm4, %ymm2, %ymm4
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpermq	$216, %ymm5, %ymm5
	vpaddb	%ymm1, %ymm4, %ymm4
	vpand	%ymm5, %ymm2, %ymm5
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$32, %ymm4, %ymm6, %ymm7
	vperm2i128	$49, %ymm4, %ymm6, %ymm4
	vmovdqu	%ymm4, 160(%rdi)
	vpunpcklbw	%ymm3, %ymm5, %ymm4
	vpunpckhbw	%ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm3, %ymm4, %ymm5
	vperm2i128	$49, %ymm3, %ymm4, %ymm3
	vmovdqu	%ymm7, 128(%rdi)
	vmovdqu	%ymm5, 192(%rdi)
	vmovdqu	%ymm3, 224(%rdi)
	vmovdqu	64(%rsi), %ymm4
	vpmovzxbw	%xmm4, %ymm3
	vpand	%ymm2, %ymm4, %ymm6
	vextracti128	$0x1, %ymm4, %xmm4
	vpmovzxbw	%xmm4, %ymm7
	vpsrlw	$2, %ymm3, %ymm4
	vpaddb	%ymm1, %ymm6, %ymm6
	vpsrlw	$2, %ymm7, %ymm5
	vpsrlw	$4, %ymm7, %ymm8
	vpand	%ymm4, %ymm0, %ymm4
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm7, %ymm7
	vpand	%ymm8, %ymm0, %ymm8
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpand	%ymm4, %ymm2, %ymm4
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpermq	$216, %ymm5, %ymm5
	vpaddb	%ymm1, %ymm4, %ymm4
	vpand	%ymm5, %ymm2, %ymm5
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$32, %ymm4, %ymm6, %ymm7
	vperm2i128	$49, %ymm4, %ymm6, %ymm4
	vmovdqu	%ymm4, 288(%rdi)
	vpunpcklbw	%ymm3, %ymm5, %ymm4
	vpunpckhbw	%ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm3, %ymm4, %ymm5
	vperm2i128	$49, %ymm3, %ymm4, %ymm3
	vmovdqu	%ymm7, 256(%rdi)
	vmovdqu	%ymm5, 320(%rdi)
	vmovdqu	%ymm3, 352(%rdi)
	vmovdqu	96(%rsi), %ymm4
	vpmovzxbw	%xmm4, %ymm3
	vpand	%ymm2, %ymm4, %ymm6
	vextracti128	$0x1, %ymm4, %xmm4
	vpmovzxbw	%xmm4, %ymm7
	vpsrlw	$2, %ymm3, %ymm4
	vpaddb	%ymm1, %ymm6, %ymm6
	vpsrlw	$2, %ymm7, %ymm5
	vpsrlw	$4, %ymm7, %ymm8
	vpand	%ymm4, %ymm0, %ymm4
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm7, %ymm7
	vpand	%ymm8, %ymm0, %ymm8
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpand	%ymm4, %ymm2, %ymm4
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpermq	$216, %ymm5, %ymm5
	vpaddb	%ymm1, %ymm4, %ymm4
	vpand	%ymm5, %ymm2, %ymm5
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$32, %ymm4, %ymm6, %ymm7
	vperm2i128	$49, %ymm4, %ymm6, %ymm4
	vmovdqu	%ymm4, 416(%rdi)
	vpunpcklbw	%ymm3, %ymm5, %ymm4
	vpunpckhbw	%ymm3, %ymm5, %ymm3
	vperm2i128	$32, %ymm3, %ymm4, %ymm5
	vperm2i128	$49, %ymm3, %ymm4, %ymm3
	vmovdqu	%ymm7, 384(%rdi)
	vmovdqu	%ymm5, 448(%rdi)
	vmovdqu	%ymm3, 480(%rdi)
	vmovdqu	128(%rsi), %ymm3
	vpmovzxbw	%xmm3, %ymm4
	vpand	%ymm2, %ymm3, %ymm6
	vextracti128	$0x1, %ymm3, %xmm3
	vpmovzxbw	%xmm3, %ymm5
	vpsrlw	$2, %ymm4, %ymm3
	vpaddb	%ymm1, %ymm6, %ymm6
	vpsrlw	$2, %ymm5, %ymm7
	vpsrlw	$4, %ymm5, %ymm8
	vpand	%ymm3, %ymm0, %ymm3
	vpand	%ymm7, %ymm0, %ymm7
	vpsrlw	$6, %ymm5, %ymm5
	vpand	%ymm8, %ymm0, %ymm8
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpsrlw	$4, %ymm4, %ymm7
	vpand	%ymm7, %ymm0, %ymm7
	vpsrlw	$6, %ymm4, %ymm4
	vpermq	$216, %ymm3, %ymm3
	vpackuswb	%ymm8, %ymm7, %ymm7
	vpand	%ymm4, %ymm0, %ymm4
	vpand	%ymm5, %ymm0, %ymm0
	vpermq	$216, %ymm7, %ymm7
	vpand	%ymm3, %ymm2, %ymm3
	vpackuswb	%ymm0, %ymm4, %ymm0
	vpand	%ymm7, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm1, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm2, %ymm2
	vpaddb	%ymm0, %ymm1, %ymm1
	vpunpcklbw	%ymm2, %ymm6, %ymm0
	vpunpckhbw	%ymm2, %ymm6, %ymm2
	vperm2i128	$32, %ymm2, %ymm0, %ymm4
	vperm2i128	$49, %ymm2, %ymm0, %ymm0
	vpunpcklbw	%ymm1, %ymm3, %ymm2
	vpunpckhbw	%ymm1, %ymm3, %ymm1
	vperm2i128	$32, %ymm1, %ymm2, %ymm5
	vperm2i128	$49, %ymm1, %ymm2, %ymm1
	vpunpcklbw	%ymm5, %ymm4, %ymm3
	vpunpckhbw	%ymm5, %ymm4, %ymm2
	vperm2i128	$32, %ymm2, %ymm3, %ymm4
	vperm2i128	$49, %ymm2, %ymm3, %ymm2
	vmovdqu	%ymm2, 544(%rdi)
	vpunpcklbw	%ymm1, %ymm0, %ymm2
	vpunpckhbw	%ymm1, %ymm0, %ymm0
	vperm2i128	$32, %ymm0, %ymm2, %ymm1
	vperm2i128	$49, %ymm0, %ymm2, %ymm0
	vmovdqu	%ymm4, 512(%rdi)
	vmovdqu	%ymm1, 576(%rdi)
	vmovdqu	%ymm0, 608(%rdi)
	.p2align 4,,10
	.p2align 3
.L14:
	movzbl	(%r8), %eax
	addq	$4, %rdx
	incq	%r8
	movl	%eax, %ecx
	andl	$3, %ecx
	decl	%ecx
	movb	%cl, -4(%rdx)
	movl	%eax, %ecx
	shrb	$2, %cl
	andl	$3, %ecx
	decl	%ecx
	movb	%cl, -3(%rdx)
	movl	%eax, %ecx
	shrb	$6, %al
	shrb	$4, %cl
	decl	%eax
	andl	$3, %ecx
	decl	%ecx
	movb	%cl, -2(%rdx)
	movb	%al, -1(%rdx)
	cmpq	%rdx, %r9
	jne	.L14
	vzeroupper
.L15:
	movzbl	190(%rsi), %eax
	movb	$0, 767(%rdi)
	movl	$0, 761(%rdi)
	andl	$3, %eax
	decl	%eax
	movb	%al, 760(%rdi)
	xorl	%eax, %eax
	movw	%ax, 765(%rdi)
	ret
.L16:
	movq	%rsi, %r9
	movq	%rdi, %rdx
	.p2align 4,,10
	.p2align 3
.L12:
	movzbl	(%r9), %eax
	incq	%r9
	addq	$4, %rdx
	movl	%eax, %ecx
	andl	$3, %ecx
	decl	%ecx
	movb	%cl, -4(%rdx)
	movl	%eax, %ecx
	shrb	$2, %cl
	andl	$3, %ecx
	decl	%ecx
	movb	%cl, -3(%rdx)
	movl	%eax, %ecx
	shrb	$6, %al
	shrb	$4, %cl
	decl	%eax
	andl	$3, %ecx
	decl	%ecx
	movb	%cl, -2(%rdx)
	movb	%al, -1(%rdx)
	cmpq	%r8, %r9
	jne	.L12
	jmp	.L15
	.cfi_endproc
.LFE5280:
	.size	CRYPTO_NAMESPACE(small_decode), .-CRYPTO_NAMESPACE(small_decode)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
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
.LC1:
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
.LC2:
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
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
