	.file	"small.c"
	.text
	.p2align 4,,15
	.globl	small_encode
	.type	small_encode, @function
small_encode:
.LFB5170:
	.cfi_startproc
	leaq	760(%rsi), %r8
	leaq	190(%rdi), %r9
	cmpq	%r8, %rdi
	jnb	.L8
	cmpq	%r9, %rsi
	jb	.L7
.L8:
	vmovdqa	.LC0(%rip), %ymm4
	vmovdqa	.LC1(%rip), %ymm6
	vmovdqa	.LC2(%rip), %ymm5
	vmovdqa	.LC3(%rip), %ymm7
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L4:
	vmovdqu	(%rsi,%rax,4), %ymm0
	vmovdqu	32(%rsi,%rax,4), %ymm2
	vmovdqu	64(%rsi,%rax,4), %ymm3
	vmovdqu	96(%rsi,%rax,4), %ymm1
	vpand	%ymm2, %ymm4, %ymm8
	vpand	%ymm0, %ymm4, %ymm9
	vpsrlw	$8, %ymm2, %ymm2
	vpsrlw	$8, %ymm0, %ymm0
	vpackuswb	%ymm2, %ymm0, %ymm0
	vpand	%ymm3, %ymm4, %ymm10
	vpand	%ymm1, %ymm4, %ymm2
	vpackuswb	%ymm8, %ymm9, %ymm9
	vpackuswb	%ymm2, %ymm10, %ymm10
	vpsrlw	$8, %ymm1, %ymm1
	vpsrlw	$8, %ymm3, %ymm3
	vpermq	$216, %ymm9, %ymm9
	vpermq	$216, %ymm10, %ymm10
	vpackuswb	%ymm1, %ymm3, %ymm3
	vpermq	$216, %ymm0, %ymm0
	vpsrlw	$8, %ymm10, %ymm1
	vpermq	$216, %ymm3, %ymm3
	vpsrlw	$8, %ymm9, %ymm2
	vpand	%ymm3, %ymm4, %ymm8
	vpackuswb	%ymm1, %ymm2, %ymm2
	vpand	%ymm0, %ymm4, %ymm1
	vpackuswb	%ymm8, %ymm1, %ymm1
	vpermq	$216, %ymm1, %ymm1
	vpsrlw	$8, %ymm3, %ymm3
	vpsrlw	$8, %ymm0, %ymm0
	vpackuswb	%ymm3, %ymm0, %ymm0
	vpmovsxbw	%xmm1, %ymm3
	vpmovsxwd	%xmm3, %ymm12
	vextracti128	$0x1, %ymm3, %xmm3
	vpmovsxwd	%xmm3, %ymm3
	vpaddd	%ymm6, %ymm12, %ymm12
	vpaddd	%ymm6, %ymm3, %ymm3
	vextracti128	$0x1, %ymm1, %xmm1
	vpslld	$2, %ymm12, %ymm12
	vpslld	$2, %ymm3, %ymm3
	vpmovsxbw	%xmm1, %ymm1
	vpand	%ymm3, %ymm5, %ymm3
	vpand	%ymm12, %ymm5, %ymm12
	vpackusdw	%ymm3, %ymm12, %ymm12
	vextracti128	$0x1, %ymm1, %xmm3
	vpmovsxwd	%xmm1, %ymm13
	vpmovsxwd	%xmm3, %ymm3
	vpaddd	%ymm6, %ymm13, %ymm13
	vpaddd	%ymm6, %ymm3, %ymm3
	vpslld	$2, %ymm13, %ymm13
	vpslld	$2, %ymm3, %ymm3
	vpand	%ymm3, %ymm5, %ymm3
	vpand	%ymm13, %ymm5, %ymm1
	vpackusdw	%ymm3, %ymm1, %ymm1
	vpermq	$216, %ymm2, %ymm2
	vpermq	$216, %ymm12, %ymm12
	vpermq	$216, %ymm1, %ymm1
	vpmovsxbw	%xmm2, %ymm11
	vpand	%ymm12, %ymm4, %ymm3
	vpand	%ymm1, %ymm4, %ymm1
	vpackuswb	%ymm1, %ymm3, %ymm3
	vextracti128	$0x1, %ymm11, %xmm1
	vpmovsxwd	%xmm11, %ymm12
	vpmovsxwd	%xmm1, %ymm1
	vextracti128	$0x1, %ymm2, %xmm2
	vpaddd	%ymm6, %ymm12, %ymm12
	vpaddd	%ymm6, %ymm1, %ymm1
	vpmovsxbw	%xmm2, %ymm2
	vpslld	$4, %ymm12, %ymm12
	vpslld	$4, %ymm1, %ymm1
	vpand	%ymm12, %ymm5, %ymm11
	vpand	%ymm1, %ymm5, %ymm1
	vextracti128	$0x1, %ymm2, %xmm12
	vpackusdw	%ymm1, %ymm11, %ymm1
	vpmovsxwd	%xmm12, %ymm12
	vpmovsxwd	%xmm2, %ymm11
	vpaddd	%ymm6, %ymm11, %ymm11
	vpaddd	%ymm6, %ymm12, %ymm12
	vpslld	$4, %ymm11, %ymm11
	vpslld	$4, %ymm12, %ymm12
	vpand	%ymm11, %ymm5, %ymm2
	vpand	%ymm12, %ymm5, %ymm11
	vpackusdw	%ymm11, %ymm2, %ymm2
	vpermq	$216, %ymm1, %ymm1
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm1, %ymm4, %ymm1
	vpand	%ymm2, %ymm4, %ymm2
	vpackuswb	%ymm2, %ymm1, %ymm2
	vpand	%ymm10, %ymm4, %ymm10
	vpand	%ymm9, %ymm4, %ymm1
	vpackuswb	%ymm10, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpermq	$216, %ymm2, %ymm2
	vpermq	$216, %ymm3, %ymm3
	vpermq	$216, %ymm1, %ymm1
	vpmovsxbw	%xmm0, %ymm8
	vpaddb	%ymm3, %ymm2, %ymm3
	vpaddb	%ymm1, %ymm7, %ymm1
	vpaddb	%ymm1, %ymm3, %ymm3
	vextracti128	$0x1, %ymm8, %xmm1
	vpmovsxwd	%xmm8, %ymm2
	vpmovsxwd	%xmm1, %ymm1
	vextracti128	$0x1, %ymm0, %xmm0
	vpaddd	%ymm6, %ymm2, %ymm2
	vpaddd	%ymm6, %ymm1, %ymm1
	vpmovsxbw	%xmm0, %ymm0
	vpslld	$6, %ymm2, %ymm2
	vpslld	$6, %ymm1, %ymm1
	vpand	%ymm2, %ymm5, %ymm2
	vextracti128	$0x1, %ymm0, %xmm8
	vpand	%ymm1, %ymm5, %ymm1
	vpackusdw	%ymm1, %ymm2, %ymm1
	vpmovsxwd	%xmm8, %ymm8
	vpmovsxwd	%xmm0, %ymm2
	vpaddd	%ymm6, %ymm2, %ymm2
	vpaddd	%ymm6, %ymm8, %ymm8
	vpslld	$6, %ymm2, %ymm2
	vpslld	$6, %ymm8, %ymm8
	vpand	%ymm2, %ymm5, %ymm0
	vpand	%ymm8, %ymm5, %ymm2
	vpackusdw	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm1, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm1, %ymm4, %ymm1
	vpand	%ymm0, %ymm4, %ymm0
	vpackuswb	%ymm0, %ymm1, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm3, %ymm3
	vmovdqu	%ymm3, (%rdi,%rax)
	addq	$32, %rax
	cmpq	$160, %rax
	jne	.L4
	leaq	160(%rdi), %r8
	leaq	640(%rsi), %rdx
	.p2align 4,,10
	.p2align 3
.L5:
	movzbl	(%rdx), %ecx
	movsbl	1(%rdx), %eax
	addq	$4, %rdx
	leal	5(%rcx,%rax,4), %eax
	movsbl	-2(%rdx), %ecx
	incq	%r8
	incl	%ecx
	sall	$4, %ecx
	addl	%ecx, %eax
	movsbl	-1(%rdx), %ecx
	incl	%ecx
	sall	$6, %ecx
	addl	%ecx, %eax
	movb	%al, -1(%r8)
	cmpq	%r8, %r9
	jne	.L5
	vzeroupper
	movzbl	760(%rsi), %eax
	incl	%eax
	movb	%al, 190(%rdi)
	ret
.L7:
	movq	%rsi, %rdx
	movq	%rdi, %r9
	.p2align 4,,10
	.p2align 3
.L2:
	movzbl	(%rdx), %ecx
	movsbl	1(%rdx), %eax
	addq	$4, %rdx
	leal	5(%rcx,%rax,4), %eax
	movsbl	-2(%rdx), %ecx
	incq	%r9
	incl	%ecx
	sall	$4, %ecx
	addl	%ecx, %eax
	movsbl	-1(%rdx), %ecx
	incl	%ecx
	sall	$6, %ecx
	addl	%ecx, %eax
	movb	%al, -1(%r9)
	cmpq	%rdx, %r8
	jne	.L2
	movzbl	760(%rsi), %eax
	incl	%eax
	movb	%al, 190(%rdi)
	ret
	.cfi_endproc
.LFE5170:
	.size	small_encode, .-small_encode
	.p2align 4,,15
	.globl	small_decode
	.type	small_decode, @function
small_decode:
.LFB5171:
	.cfi_startproc
	leaq	190(%rsi), %r8
	leaq	760(%rdi), %r9
	cmpq	%r8, %rdi
	jnb	.L19
	cmpq	%r9, %rsi
	jb	.L18
.L19:
	vmovdqu	(%rsi), %ymm0
	vmovdqa	.LC4(%rip), %ymm2
	vpmovzxbw	%xmm0, %ymm3
	vpand	%ymm2, %ymm0, %ymm6
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovzxbw	%xmm0, %ymm7
	vmovdqa	.LC0(%rip), %ymm0
	vpsrlw	$2, %ymm3, %ymm4
	vpsrlw	$2, %ymm7, %ymm5
	vpand	%ymm5, %ymm0, %ymm5
	vpand	%ymm4, %ymm0, %ymm4
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm7, %ymm8
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm8, %ymm0, %ymm8
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpsrlw	$6, %ymm7, %ymm7
	vpand	%ymm7, %ymm0, %ymm7
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpcmpeqd	%ymm1, %ymm1, %ymm1
	vpand	%ymm4, %ymm2, %ymm4
	vpand	%ymm5, %ymm2, %ymm5
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm6, %ymm6
	vpaddb	%ymm1, %ymm4, %ymm4
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
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
	leaq	640(%rdi), %rdx
	vpmovzxbw	%xmm4, %ymm3
	vpand	%ymm2, %ymm4, %ymm6
	vextracti128	$0x1, %ymm4, %xmm4
	vpmovzxbw	%xmm4, %ymm7
	vpsrlw	$2, %ymm7, %ymm5
	vpsrlw	$2, %ymm3, %ymm4
	vpand	%ymm5, %ymm0, %ymm5
	vpand	%ymm4, %ymm0, %ymm4
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm7, %ymm8
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm8, %ymm0, %ymm8
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpsrlw	$6, %ymm7, %ymm7
	vpand	%ymm7, %ymm0, %ymm7
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpand	%ymm4, %ymm2, %ymm4
	vpand	%ymm5, %ymm2, %ymm5
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm6, %ymm6
	vpaddb	%ymm1, %ymm4, %ymm4
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
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
	leaq	160(%rsi), %r8
	vpmovzxbw	%xmm4, %ymm3
	vpand	%ymm2, %ymm4, %ymm6
	vextracti128	$0x1, %ymm4, %xmm4
	vpmovzxbw	%xmm4, %ymm7
	vpsrlw	$2, %ymm7, %ymm5
	vpsrlw	$2, %ymm3, %ymm4
	vpand	%ymm5, %ymm0, %ymm5
	vpand	%ymm4, %ymm0, %ymm4
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm7, %ymm8
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm8, %ymm0, %ymm8
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpsrlw	$6, %ymm7, %ymm7
	vpand	%ymm7, %ymm0, %ymm7
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpand	%ymm4, %ymm2, %ymm4
	vpand	%ymm5, %ymm2, %ymm5
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm6, %ymm6
	vpaddb	%ymm1, %ymm4, %ymm4
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
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
	vpsrlw	$2, %ymm7, %ymm5
	vpsrlw	$2, %ymm3, %ymm4
	vpand	%ymm5, %ymm0, %ymm5
	vpand	%ymm4, %ymm0, %ymm4
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpsrlw	$4, %ymm7, %ymm8
	vpsrlw	$4, %ymm3, %ymm5
	vpand	%ymm8, %ymm0, %ymm8
	vpand	%ymm5, %ymm0, %ymm5
	vpsrlw	$6, %ymm3, %ymm3
	vpsrlw	$6, %ymm7, %ymm7
	vpand	%ymm7, %ymm0, %ymm7
	vpackuswb	%ymm8, %ymm5, %ymm5
	vpand	%ymm3, %ymm0, %ymm3
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpermq	$216, %ymm4, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpand	%ymm4, %ymm2, %ymm4
	vpand	%ymm5, %ymm2, %ymm5
	vpermq	$216, %ymm3, %ymm3
	vpaddb	%ymm1, %ymm6, %ymm6
	vpaddb	%ymm1, %ymm4, %ymm4
	vpaddb	%ymm1, %ymm5, %ymm5
	vpaddb	%ymm3, %ymm1, %ymm3
	vpunpcklbw	%ymm5, %ymm6, %ymm7
	vpunpckhbw	%ymm5, %ymm6, %ymm5
	vpunpcklbw	%ymm3, %ymm4, %ymm6
	vpunpckhbw	%ymm3, %ymm4, %ymm3
	vperm2i128	$32, %ymm5, %ymm7, %ymm8
	vperm2i128	$32, %ymm3, %ymm6, %ymm4
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vpunpcklbw	%ymm4, %ymm8, %ymm6
	vpunpckhbw	%ymm4, %ymm8, %ymm4
	vperm2i128	$49, %ymm5, %ymm7, %ymm5
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
	vpsrlw	$2, %ymm5, %ymm7
	vpsrlw	$2, %ymm4, %ymm3
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm3, %ymm0, %ymm3
	vpackuswb	%ymm7, %ymm3, %ymm3
	vpsrlw	$4, %ymm5, %ymm8
	vpsrlw	$4, %ymm4, %ymm7
	vpand	%ymm7, %ymm0, %ymm7
	vpand	%ymm8, %ymm0, %ymm8
	vpsrlw	$6, %ymm5, %ymm5
	vpackuswb	%ymm8, %ymm7, %ymm7
	vpsrlw	$6, %ymm4, %ymm4
	vpand	%ymm4, %ymm0, %ymm4
	vpermq	$216, %ymm3, %ymm3
	vpand	%ymm5, %ymm0, %ymm0
	vpermq	$216, %ymm7, %ymm7
	vpand	%ymm3, %ymm2, %ymm3
	vpackuswb	%ymm0, %ymm4, %ymm0
	vpand	%ymm7, %ymm2, %ymm2
	vpaddb	%ymm1, %ymm6, %ymm6
	vpaddb	%ymm1, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm1, %ymm3, %ymm3
	vpaddb	%ymm0, %ymm1, %ymm1
	vpunpcklbw	%ymm2, %ymm6, %ymm0
	vpunpckhbw	%ymm2, %ymm6, %ymm2
	vperm2i128	$32, %ymm2, %ymm0, %ymm4
	vperm2i128	$49, %ymm2, %ymm0, %ymm0
	vpunpcklbw	%ymm1, %ymm3, %ymm2
	vpunpckhbw	%ymm1, %ymm3, %ymm1
	vperm2i128	$32, %ymm1, %ymm2, %ymm5
	vpunpcklbw	%ymm5, %ymm4, %ymm3
	vperm2i128	$49, %ymm1, %ymm2, %ymm1
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
.L16:
	incq	%r8
	movzbl	-1(%r8), %eax
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
	shrb	$4, %cl
	andl	$3, %ecx
	decl	%ecx
	shrb	$6, %al
	movb	%cl, -2(%rdx)
	decl	%eax
	movb	%al, -1(%rdx)
	cmpq	%rdx, %r9
	jne	.L16
	vzeroupper
.L17:
	movzbl	190(%rsi), %eax
	movl	$0, 761(%rdi)
	andl	$3, %eax
	decl	%eax
	movb	%al, 760(%rdi)
	xorl	%eax, %eax
	movw	%ax, 765(%rdi)
	movb	$0, 767(%rdi)
	ret
.L18:
	movq	%rsi, %r9
	movq	%rdi, %rdx
	.p2align 4,,10
	.p2align 3
.L14:
	incq	%r9
	movzbl	-1(%r9), %eax
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
	shrb	$4, %cl
	andl	$3, %ecx
	decl	%ecx
	shrb	$6, %al
	movb	%cl, -2(%rdx)
	decl	%eax
	movb	%al, -1(%rdx)
	cmpq	%r8, %r9
	jne	.L14
	jmp	.L17
	.cfi_endproc
.LFE5171:
	.size	small_decode, .-small_decode
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
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.align 32
.LC2:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
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
	.ident	"GCC: (GNU) 8.2.1 20181105 (Red Hat 8.2.1-5)"
	.section	.note.GNU-stack,"",@progbits
