	.file	"rq_rounded.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(rq_roundencode)
	.type	CRYPTO_NAMESPACE(rq_roundencode), @function
CRYPTO_NAMESPACE(rq_roundencode):
.LFB5289:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	832(%rdi), %rcx
	movq	%rsi, %rax
	movq	%rdi, %rdx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	subq	$1608, %rsp
	vmovdqa	.LC0(%rip), %ymm3
	vmovdqa	.LC1(%rip), %ymm4
	vmovdqa	.LC2(%rip), %ymm5
	.p2align 4,,10
	.p2align 3
.L2:
	vmovdqu	(%rax), %xmm0
	vmovdqu	32(%rax), %xmm2
	addq	$64, %rdx
	addq	$96, %rax
	vinserti128	$0x1, -48(%rax), %ymm0, %ymm0
	vinserti128	$0x1, -16(%rax), %ymm2, %ymm2
	vmovdqu	-80(%rax), %xmm1
	vinserti128	$0x1, -32(%rax), %ymm1, %ymm1
	vpmulhrsw	%ymm3, %ymm2, %ymm2
	vpmulhrsw	%ymm3, %ymm0, %ymm0
	vpmulhrsw	%ymm3, %ymm1, %ymm1
	vpblendw	$240, %ymm0, %ymm2, %ymm6
	vpshufd	$78, %ymm6, %ymm6
	vpblendw	$240, %ymm1, %ymm0, %ymm0
	vpblendw	$240, %ymm2, %ymm1, %ymm1
	vpblendw	$204, %ymm0, %ymm1, %ymm2
	vpblendw	$204, %ymm6, %ymm0, %ymm0
	vpblendw	$204, %ymm1, %ymm6, %ymm6
	vpblendw	$170, %ymm0, %ymm6, %ymm1
	vpshufd	$177, %ymm2, %ymm2
	vpshuflw	$177, %ymm1, %ymm1
	vpblendw	$170, %ymm2, %ymm0, %ymm0
	vpblendw	$170, %ymm6, %ymm2, %ymm2
	vpshufhw	$177, %ymm1, %ymm1
	vpaddw	%ymm4, %ymm0, %ymm0
	vpaddw	%ymm4, %ymm2, %ymm2
	vpaddw	%ymm4, %ymm1, %ymm6
	vpsrlw	$8, %ymm0, %ymm7
	vpand	%ymm0, %ymm5, %ymm0
	vpsllw	$1, %ymm6, %ymm1
	vpaddw	%ymm6, %ymm1, %ymm1
	vpsllw	$3, %ymm2, %ymm6
	vpsllw	$1, %ymm1, %ymm1
	vpaddw	%ymm2, %ymm6, %ymm2
	vpaddw	%ymm7, %ymm1, %ymm1
	vpsllw	$2, %ymm2, %ymm2
	vpsllw	$8, %ymm1, %ymm7
	vpsrlw	$8, %ymm1, %ymm1
	vpaddw	%ymm0, %ymm7, %ymm0
	vpaddw	%ymm1, %ymm2, %ymm1
	vpunpcklwd	%ymm1, %ymm0, %ymm2
	vpunpckhwd	%ymm1, %ymm0, %ymm1
	vperm2i128	$32, %ymm1, %ymm2, %ymm0
	vperm2i128	$49, %ymm1, %ymm2, %ymm1
	vmovdqu	%ymm0, -64(%rdx)
	vmovdqu	%ymm1, -32(%rdx)
	cmpq	%rcx, %rdx
	jne	.L2
	vmovdqa	.LC4(%rip), %ymm2
	vmovdqa	.LC5(%rip), %ymm15
	vpmulhrsw	1440(%rsi), %ymm3, %ymm6
	vmovdqa	%ymm6, -24(%rsp)
	vmovdqa	-24(%rsp), %ymm6
	movzwl	-24(%rsp), %eax
	vpmulhrsw	1376(%rsi), %ymm3, %ymm4
	vmovdqa	%ymm4, -88(%rsp)
	vpmulhrsw	1248(%rsi), %ymm3, %ymm4
	vmovdqa	.LC7(%rip), %ymm14
	vpmulhrsw	1344(%rsi), %ymm3, %ymm5
	vpmulhrsw	1280(%rsi), %ymm3, %ymm11
	vpmulhrsw	1408(%rsi), %ymm3, %ymm7
	vmovdqa	%ymm5, -56(%rsp)
	vpshufb	%ymm15, %ymm11, %ymm5
	vmovdqa	%ymm6, 200(%rsp)
	vpmulhrsw	1472(%rsi), %ymm3, %ymm6
	vmovdqa	%ymm6, -24(%rsp)
	vmovdqa	-24(%rsp), %ymm6
	movswl	-24(%rsp), %ecx
	vmovdqa	%ymm7, -120(%rsp)
	vmovdqa	%ymm6, 232(%rsp)
	vpmulhrsw	1504(%rsi), %ymm3, %ymm6
	vpmulhrsw	1312(%rsi), %ymm3, %ymm3
	movswl	%ax, %esi
	vmovdqa	%ymm6, -24(%rsp)
	vmovdqa	-24(%rsp), %ymm6
	movzwl	-24(%rsp), %edx
	vmovdqa	%ymm6, 264(%rsp)
	vmovdqa	.LC3(%rip), %ymm6
	vpshufb	%ymm6, %ymm4, %ymm0
	vpermq	$78, %ymm0, %ymm1
	vpshufb	%ymm2, %ymm4, %ymm0
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqa	.LC8(%rip), %ymm1
	vpor	%ymm5, %ymm0, %ymm0
	vmovdqa	.LC6(%rip), %ymm5
	vpshufb	%ymm14, %ymm0, %ymm0
	vpshufb	%ymm5, %ymm3, %ymm7
	vpermq	$78, %ymm7, %ymm8
	vpshufb	%ymm1, %ymm3, %ymm7
	vpor	%ymm8, %ymm7, %ymm7
	vpshufb	.LC9(%rip), %ymm4, %ymm8
	vpor	%ymm7, %ymm0, %ymm0
	vpermq	$78, %ymm8, %ymm8
	vpshufb	.LC10(%rip), %ymm4, %ymm7
	vpshufb	.LC11(%rip), %ymm11, %ymm9
	vpor	%ymm8, %ymm7, %ymm8
	vpshufb	.LC13(%rip), %ymm3, %ymm7
	vmovdqa	.LC20(%rip), %ymm12
	vpor	%ymm9, %ymm8, %ymm8
	vmovdqa	.LC21(%rip), %ymm13
	vpshufb	.LC12(%rip), %ymm3, %ymm9
	vpshufb	.LC16(%rip), %ymm11, %ymm11
	vpermq	$78, %ymm9, %ymm9
	vpshufb	%ymm14, %ymm8, %ymm8
	movswl	202(%rsp), %eax
	movswl	204(%rsp), %r8d
	vpor	%ymm9, %ymm7, %ymm10
	vpshufb	.LC17(%rip), %ymm3, %ymm9
	vpshufb	.LC14(%rip), %ymm4, %ymm7
	vpor	%ymm10, %ymm8, %ymm10
	vpermq	$78, %ymm9, %ymm9
	vpermq	$78, %ymm7, %ymm7
	vpshufb	.LC19(%rip), %ymm3, %ymm3
	leal	(%rax,%rax,2), %eax
	vpshufb	.LC15(%rip), %ymm4, %ymm4
	vpor	%ymm9, %ymm3, %ymm3
	vpmullw	%ymm10, %ymm12, %ymm9
	vpor	%ymm7, %ymm4, %ymm4
	sall	$9, %eax
	vpmulhw	%ymm12, %ymm10, %ymm10
	vpor	%ymm11, %ymm4, %ymm8
	vmovdqa	.LC22(%rip), %ymm11
	leal	1806037245(%rsi,%rax), %esi
	vpshufb	.LC18(%rip), %ymm8, %ymm8
	leal	(%r8,%r8,8), %eax
	vpor	%ymm3, %ymm8, %ymm8
	sall	$18, %eax
	addl	%esi, %eax
	vpunpcklwd	%ymm10, %ymm9, %ymm7
	vpunpckhwd	%ymm10, %ymm9, %ymm9
	vpmullw	%ymm8, %ymm13, %ymm10
	vpmulhw	%ymm13, %ymm8, %ymm8
	vperm2i128	$32, %ymm9, %ymm7, %ymm4
	vperm2i128	$49, %ymm9, %ymm7, %ymm7
	vpslld	$9, %ymm4, %ymm4
	vpslld	$9, %ymm7, %ymm7
	vpunpcklwd	%ymm8, %ymm10, %ymm3
	vpunpckhwd	%ymm8, %ymm10, %ymm8
	vperm2i128	$32, %ymm8, %ymm3, %ymm10
	vperm2i128	$49, %ymm8, %ymm3, %ymm3
	vpslld	$18, %ymm10, %ymm10
	vpslld	$18, %ymm3, %ymm3
	vpaddd	%ymm10, %ymm4, %ymm4
	vpaddd	%ymm3, %ymm7, %ymm7
	vpmovsxwd	%xmm0, %ymm10
	vextracti128	$0x1, %ymm0, %xmm3
	vpaddd	%ymm11, %ymm10, %ymm10
	vmovdqa	-56(%rsp), %ymm0
	vpmovsxwd	%xmm3, %ymm3
	vpaddd	%ymm10, %ymm4, %ymm4
	vpaddd	%ymm11, %ymm3, %ymm3
	vpshufb	%ymm6, %ymm0, %ymm6
	vpshufb	%ymm2, %ymm0, %ymm2
	vmovdqu	%ymm4, 832(%rdi)
	vpaddd	%ymm3, %ymm7, %ymm7
	vmovdqa	-88(%rsp), %ymm4
	vmovdqa	-120(%rsp), %ymm3
	vpermq	$78, %ymm6, %ymm6
	vpor	%ymm6, %ymm2, %ymm2
	vmovdqu	%ymm7, 864(%rdi)
	vpshufb	%ymm15, %ymm4, %ymm15
	vpshufb	%ymm5, %ymm3, %ymm5
	vpshufb	%ymm1, %ymm3, %ymm1
	vpor	%ymm15, %ymm2, %ymm2
	vpermq	$78, %ymm5, %ymm5
	vpshufb	.LC12(%rip), %ymm3, %ymm7
	vpor	%ymm5, %ymm1, %ymm1
	vpshufb	%ymm14, %ymm2, %ymm2
	vpshufb	.LC10(%rip), %ymm0, %ymm5
	vpor	%ymm1, %ymm2, %ymm2
	vpermq	$78, %ymm7, %ymm7
	vpshufb	.LC9(%rip), %ymm0, %ymm1
	vpshufb	.LC11(%rip), %ymm4, %ymm6
	vpermq	$78, %ymm1, %ymm1
	vpshufb	.LC16(%rip), %ymm4, %ymm4
	vpor	%ymm1, %ymm5, %ymm1
	vpshufb	.LC13(%rip), %ymm3, %ymm5
	vpor	%ymm7, %ymm5, %ymm7
	vpor	%ymm6, %ymm1, %ymm1
	vpshufb	.LC14(%rip), %ymm0, %ymm5
	vpermq	$78, %ymm5, %ymm5
	vpshufb	%ymm14, %ymm1, %ymm1
	vpshufb	.LC15(%rip), %ymm0, %ymm0
	vpor	%ymm5, %ymm0, %ymm0
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm4, %ymm0, %ymm0
	vpmullw	%ymm1, %ymm12, %ymm5
	vpshufb	.LC17(%rip), %ymm3, %ymm4
	vpermq	$78, %ymm4, %ymm4
	vpmulhw	%ymm12, %ymm1, %ymm12
	vpshufb	.LC19(%rip), %ymm3, %ymm3
	vpshufb	.LC18(%rip), %ymm0, %ymm0
	vpor	%ymm4, %ymm3, %ymm3
	vpor	%ymm3, %ymm0, %ymm0
	vpmullw	%ymm0, %ymm13, %ymm3
	vpmulhw	%ymm13, %ymm0, %ymm13
	vpunpcklwd	%ymm12, %ymm5, %ymm4
	vpunpckhwd	%ymm12, %ymm5, %ymm12
	vperm2i128	$32, %ymm12, %ymm4, %ymm1
	vperm2i128	$49, %ymm12, %ymm4, %ymm4
	vpslld	$9, %ymm1, %ymm1
	vpslld	$9, %ymm4, %ymm4
	vpunpcklwd	%ymm13, %ymm3, %ymm0
	vpunpckhwd	%ymm13, %ymm3, %ymm13
	vperm2i128	$32, %ymm13, %ymm0, %ymm3
	vperm2i128	$49, %ymm13, %ymm0, %ymm0
	vpslld	$18, %ymm3, %ymm3
	vpslld	$18, %ymm0, %ymm0
	vpaddd	%ymm1, %ymm3, %ymm1
	vpmovsxwd	%xmm2, %ymm3
	vextracti128	$0x1, %ymm2, %xmm2
	vpaddd	%ymm11, %ymm3, %ymm3
	vpaddd	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm1, %ymm1
	vmovdqu	%ymm1, 896(%rdi)
	vpmovsxwd	%xmm2, %ymm1
	vpaddd	%ymm11, %ymm1, %ymm11
	vpaddd	%ymm11, %ymm0, %ymm11
	vmovdqu	%ymm11, 928(%rdi)
	movl	%eax, 960(%rdi)
	movswl	208(%rsp), %eax
	movswl	210(%rsp), %esi
	movswl	206(%rsp), %r8d
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	movswl	216(%rsp), %esi
	sall	$18, %eax
	addl	%r8d, %eax
	movswl	212(%rsp), %r8d
	movl	%eax, 964(%rdi)
	movswl	214(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	movswl	222(%rsp), %esi
	sall	$18, %eax
	addl	%r8d, %eax
	movswl	218(%rsp), %r8d
	movl	%eax, 968(%rdi)
	movswl	220(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	movswl	228(%rsp), %esi
	sall	$18, %eax
	addl	%r8d, %eax
	movswl	224(%rsp), %r8d
	movl	%eax, 972(%rdi)
	movswl	226(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	movswl	234(%rsp), %esi
	sall	$18, %eax
	addl	%r8d, %eax
	movswl	230(%rsp), %r8d
	movl	%eax, 976(%rdi)
	leal	(%rcx,%rcx,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %ecx
	leal	(%rsi,%rsi,8), %eax
	movswl	236(%rsp), %esi
	sall	$18, %eax
	addl	%ecx, %eax
	movswl	240(%rsp), %ecx
	movl	%eax, 980(%rdi)
	movswl	238(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	movswl	246(%rsp), %ecx
	sall	$18, %eax
	addl	%esi, %eax
	movswl	242(%rsp), %esi
	movl	%eax, 984(%rdi)
	movswl	244(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	movswl	252(%rsp), %ecx
	sall	$18, %eax
	addl	%esi, %eax
	movswl	248(%rsp), %esi
	movl	%eax, 988(%rdi)
	movswl	250(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	sall	$18, %eax
	addl	%esi, %eax
	movswl	254(%rsp), %esi
	movl	%eax, 992(%rdi)
	movswl	256(%rsp), %eax
	movswl	258(%rsp), %ecx
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	movswl	260(%rsp), %ecx
	sall	$18, %eax
	addl	%esi, %eax
	movl	%eax, 996(%rdi)
	movswl	262(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rcx,%rax), %ecx
	leal	(%rdx,%rdx,8), %eax
	movswl	270(%rsp), %edx
	sall	$18, %eax
	addl	%ecx, %eax
	movswl	266(%rsp), %ecx
	movl	%eax, 1000(%rdi)
	movswl	268(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rcx,%rax), %ecx
	leal	(%rdx,%rdx,8), %eax
	movswl	276(%rsp), %edx
	sall	$18, %eax
	addl	%ecx, %eax
	movswl	272(%rsp), %ecx
	movl	%eax, 1004(%rdi)
	movswl	274(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rcx,%rax), %ecx
	leal	(%rdx,%rdx,8), %eax
	movswl	278(%rsp), %edx
	sall	$18, %eax
	addl	%ecx, %eax
	movl	%eax, 1008(%rdi)
	movswl	280(%rsp), %eax
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1175805(%rdx,%rax), %eax
	movw	%ax, 1012(%rdi)
	sarl	$16, %eax
	movb	%al, 1014(%rdi)
	vzeroupper
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5289:
	.size	CRYPTO_NAMESPACE(rq_roundencode), .-CRYPTO_NAMESPACE(rq_roundencode)
	.p2align 4
	.globl	CRYPTO_NAMESPACE(rq_decoderounded)
	.type	CRYPTO_NAMESPACE(rq_decoderounded), @function
CRYPTO_NAMESPACE(rq_decoderounded):
.LFB5290:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	992(%rsi), %r10
	vmovapd	.LC29(%rip), %ymm10
	movq	%rdi, %rcx
	vmovapd	.LC30(%rip), %ymm9
	vmovapd	.LC31(%rip), %ymm8
	vmovapd	.LC32(%rip), %ymm15
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	.cfi_offset 14, -24
	movq	%rsi, %r14
	pushq	%r13
	.cfi_offset 13, -32
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -40
	movq	%r10, %r12
	pushq	%rbx
	.cfi_offset 3, -48
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	(%rsi), %ymm4
	vpunpckldq	.LC23(%rip), %ymm4, %ymm1
	addq	$32, %rsi
	addq	$48, %rcx
	vaddpd	.LC24(%rip), %ymm1, %ymm1
	vmulpd	.LC25(%rip), %ymm1, %ymm6
	vpunpckhdq	.LC23(%rip), %ymm4, %ymm4
	vaddpd	.LC24(%rip), %ymm4, %ymm4
	vroundpd	$1, %ymm6, %ymm3
	vmulpd	%ymm10, %ymm3, %ymm6
	vfnmadd231pd	.LC26(%rip), %ymm3, %ymm1
	vmulpd	.LC27(%rip), %ymm1, %ymm0
	vroundpd	$1, %ymm6, %ymm6
	vfnmadd132pd	%ymm9, %ymm3, %ymm6
	vroundpd	$1, %ymm0, %ymm2
	vmulpd	%ymm10, %ymm2, %ymm0
	vfnmadd231pd	.LC28(%rip), %ymm2, %ymm1
	vfmadd132pd	%ymm8, %ymm15, %ymm6
	vmulpd	%ymm10, %ymm1, %ymm5
	vroundpd	$1, %ymm0, %ymm0
	vfnmadd132pd	%ymm9, %ymm2, %ymm0
	vcvtpd2dqy	%ymm6, %xmm6
	vinsertps	$0xe, %xmm6, %xmm6, %xmm11
	vpextrd	$2, %xmm6, %r9d
	vroundpd	$1, %ymm5, %ymm5
	vfnmadd132pd	%ymm9, %ymm1, %ymm5
	vfmadd132pd	%ymm8, %ymm15, %ymm0
	vmulpd	.LC25(%rip), %ymm4, %ymm1
	vfmadd132pd	%ymm8, %ymm15, %ymm5
	vcvtpd2dqy	%ymm0, %xmm0
	vmovd	%xmm0, %ebx
	vpsrldq	$4, %xmm0, %xmm12
	vroundpd	$1, %ymm1, %ymm1
	vmulpd	%ymm10, %ymm1, %ymm2
	vinsertps	$0xe, %xmm12, %xmm12, %xmm14
	vpextrd	$3, %xmm0, %eax
	vfnmadd231pd	.LC26(%rip), %ymm1, %ymm4
	vmulpd	.LC27(%rip), %ymm4, %ymm3
	vcvtpd2dqy	%ymm5, %xmm5
	vinsertps	$0xe, %xmm5, %xmm5, %xmm13
	vpextrd	$2, %xmm5, %r10d
	vroundpd	$1, %ymm2, %ymm2
	vfnmadd231pd	%ymm9, %ymm2, %ymm1
	vpinsrw	$1, %ebx, %xmm13, %xmm13
	vpextrd	$1, %xmm5, %ebx
	vpinsrw	$1, %ebx, %xmm11, %xmm11
	vpextrd	$1, %xmm6, %ebx
	vroundpd	$1, %ymm3, %ymm3
	vmulpd	%ymm10, %ymm3, %ymm2
	vpinsrw	$1, %ebx, %xmm14, %xmm12
	vpunpckldq	%xmm11, %xmm13, %xmm13
	vfnmadd231pd	.LC28(%rip), %ymm3, %ymm4
	vmovd	%r9d, %xmm11
	vpextrd	$3, %xmm5, %r9d
	vpinsrw	$1, %r9d, %xmm11, %xmm11
	vfmadd132pd	%ymm8, %ymm15, %ymm1
	vroundpd	$1, %ymm2, %ymm2
	vfnmadd231pd	%ymm9, %ymm2, %ymm3
	vmulpd	%ymm10, %ymm4, %ymm2
	vcvtpd2dqy	%ymm1, %xmm1
	vpextrd	$2, %xmm1, %edx
	vfmadd132pd	%ymm8, %ymm15, %ymm3
	vroundpd	$1, %ymm2, %ymm2
	vfnmadd132pd	%ymm9, %ymm4, %ymm2
	vinsertps	$0xe, %xmm1, %xmm1, %xmm4
	vcvtpd2dqy	%ymm3, %xmm3
	vmovd	%xmm3, %ebx
	vpextrd	$1, %xmm3, %r11d
	vpextrd	$3, %xmm3, %edi
	vfmadd132pd	%ymm8, %ymm15, %ymm2
	vcvtpd2dqy	%ymm2, %xmm2
	vinsertps	$0xe, %xmm2, %xmm2, %xmm7
	vpextrd	$2, %xmm2, %r8d
	vpinsrw	$1, %ebx, %xmm7, %xmm7
	vpextrd	$1, %xmm2, %ebx
	vpunpckldq	%xmm7, %xmm12, %xmm14
	vmovd	%r11d, %xmm7
	vmovd	%r10d, %xmm12
	vpextrd	$1, %xmm1, %r11d
	vpextrd	$2, %xmm0, %r10d
	vpinsrw	$1, %ebx, %xmm4, %xmm4
	vpinsrw	$1, %r10d, %xmm12, %xmm0
	vpinsrw	$1, %r11d, %xmm7, %xmm7
	vpunpcklqdq	%xmm14, %xmm13, %xmm13
	vpunpckldq	%xmm7, %xmm4, %xmm4
	vpunpckldq	%xmm11, %xmm0, %xmm11
	vmovd	%r8d, %xmm0
	vmovups	%xmm13, -48(%rcx)
	vpunpcklqdq	%xmm11, %xmm4, %xmm11
	vmovd	%eax, %xmm4
	vpextrd	$3, %xmm6, %eax
	vpinsrw	$1, %eax, %xmm4, %xmm4
	vpextrd	$2, %xmm3, %eax
	vmovups	%xmm11, -32(%rcx)
	vpinsrw	$1, %eax, %xmm0, %xmm3
	vpextrd	$3, %xmm2, %eax
	vmovd	%edx, %xmm0
	vpinsrw	$1, %eax, %xmm0, %xmm0
	vmovd	%edi, %xmm2
	vpextrd	$3, %xmm1, %eax
	vpinsrw	$1, %eax, %xmm2, %xmm2
	vpunpckldq	%xmm3, %xmm4, %xmm1
	vpunpckldq	%xmm2, %xmm0, %xmm0
	vpunpcklqdq	%xmm0, %xmm1, %xmm0
	vmovups	%xmm0, -16(%rcx)
	cmpq	%r12, %rsi
	jne	.L7
	movq	%r12, %r10
	leaq	1488(%r13), %rdx
	leaq	1012(%r14), %rcx
.L8:
	movzbl	1(%r10), %r11d
	movzbl	2(%r10), %r8d
	addq	$4, %r10
	movzbl	-4(%r10), %r9d
	movzbl	-1(%r10), %esi
	imull	$228, %r11d, %eax
	imull	$58254, %r8d, %edi
	leal	456(%rdi,%rax), %edi
	imull	$14913081, %esi, %eax
	addl	%eax, %edi
	movl	%esi, %eax
	shrl	$21, %edi
	sall	$8, %eax
	leal	(%rdi,%rdi,8), %esi
	addl	%r8d, %eax
	leal	1(%r9), %r8d
	sall	$2, %esi
	imull	$1365, %r8d, %r8d
	subl	%esi, %eax
	movl	%eax, %esi
	imull	$89478485, %eax, %eax
	sall	$8, %esi
	addl	%r11d, %esi
	imull	$349525, %r11d, %r11d
	addl	%r11d, %r8d
	addl	%r8d, %eax
	shrl	$21, %eax
	leal	(%rax,%rax,2), %eax
	leal	(%rax,%rax), %r8d
	subl	%r8d, %esi
	sall	$8, %esi
	addl	%esi, %r9d
	leal	2296(%r9,%r9,2), %r8d
	imull	$228, %r8d, %esi
	sarl	$20, %esi
	imull	$-4591, %esi, %esi
	addl	%r8d, %esi
	imull	$58470, %esi, %r8d
	addl	$134217728, %r8d
	sarl	$28, %r8d
	imull	$-4591, %r8d, %r8d
	addl	%r8d, %esi
	movw	%si, (%rdx)
	leal	2296(%rax), %esi
	imull	$228, %esi, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addq	$6, %rdx
	addl	%esi, %eax
	imull	$58470, %eax, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %eax
	leal	2296(%rdi,%rdi,2), %esi
	movw	%ax, -4(%rdx)
	imull	$228, %esi, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%esi, %eax
	imull	$58470, %eax, %esi
	addl	$134217728, %esi
	sarl	$28, %esi
	imull	$-4591, %esi, %esi
	addl	%esi, %eax
	movw	%ax, -2(%rdx)
	cmpq	%r10, %rcx
	jne	.L8
	movzbl	1014(%r14), %eax
	movzbl	1013(%r14), %ecx
	movzbl	1012(%r14), %esi
	movq	$0, 1522(%r13)
	movl	$0, 1530(%r13)
	movl	%eax, %edx
	imull	$89478485, %eax, %eax
	sall	$8, %edx
	addl	%ecx, %edx
	imull	$349525, %ecx, %ecx
	addl	%ecx, %eax
	leal	1(%rsi), %ecx
	imull	$1365, %ecx, %ecx
	addl	%ecx, %eax
	shrl	$21, %eax
	leal	(%rax,%rax,2), %ecx
	leal	(%rcx,%rcx), %eax
	addl	$2296, %ecx
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$8, %eax
	addl	%esi, %eax
	leal	2296(%rax,%rax,2), %edx
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	imull	$58470, %eax, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	movw	%ax, 1518(%r13)
	imull	$228, %ecx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%ecx, %eax
	imull	$58470, %eax, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	movw	%ax, 1520(%r13)
	xorl	%eax, %eax
	movw	%ax, 1534(%r13)
	vzeroupper
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5290:
	.size	CRYPTO_NAMESPACE(rq_decoderounded), .-CRYPTO_NAMESPACE(rq_decoderounded)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.align 32
.LC1:
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.value	765
	.align 32
.LC2:
	.quad	71777214294589695
	.quad	71777214294589695
	.quad	71777214294589695
	.quad	71777214294589695
	.align 32
.LC3:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	2
	.byte	3
	.byte	8
	.byte	9
	.byte	14
	.byte	15
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC4:
	.byte	0
	.byte	1
	.byte	6
	.byte	7
	.byte	12
	.byte	13
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC5:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	4
	.byte	5
	.byte	10
	.byte	11
	.byte	0
	.byte	1
	.byte	6
	.byte	7
	.byte	12
	.byte	13
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC6:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	2
	.byte	3
	.byte	8
	.byte	9
	.byte	14
	.byte	15
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC7:
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	8
	.byte	9
	.byte	10
	.byte	11
	.byte	12
	.byte	13
	.byte	14
	.byte	15
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	5
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC8:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	4
	.byte	5
	.byte	10
	.byte	11
	.align 32
.LC9:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	4
	.byte	5
	.byte	10
	.byte	11
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC10:
	.byte	2
	.byte	3
	.byte	8
	.byte	9
	.byte	14
	.byte	15
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC11:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	0
	.byte	1
	.byte	6
	.byte	7
	.byte	12
	.byte	13
	.byte	2
	.byte	3
	.byte	8
	.byte	9
	.byte	14
	.byte	15
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC12:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	4
	.byte	5
	.byte	10
	.byte	11
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC13:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	0
	.byte	1
	.byte	6
	.byte	7
	.byte	12
	.byte	13
	.align 32
.LC14:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	0
	.byte	1
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	0
	.byte	1
	.byte	6
	.byte	7
	.byte	12
	.byte	13
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC15:
	.byte	4
	.byte	5
	.byte	10
	.byte	11
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC16:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	2
	.byte	3
	.byte	8
	.byte	9
	.byte	14
	.byte	15
	.byte	4
	.byte	5
	.byte	10
	.byte	11
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC17:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	0
	.byte	1
	.byte	6
	.byte	7
	.byte	12
	.byte	13
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC18:
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	8
	.byte	9
	.byte	10
	.byte	11
	.byte	12
	.byte	13
	.byte	14
	.byte	15
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.align 32
.LC19:
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	-128
	.byte	2
	.byte	3
	.byte	8
	.byte	9
	.byte	14
	.byte	15
	.align 32
.LC20:
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
	.align 32
.LC21:
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.value	9
	.align 32
.LC22:
	.long	1806037245
	.long	1806037245
	.long	1806037245
	.long	1806037245
	.long	1806037245
	.long	1806037245
	.long	1806037245
	.long	1806037245
	.align 32
.LC23:
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.align 32
.LC24:
	.long	0
	.long	-1019740160
	.long	0
	.long	-1019740160
	.long	0
	.long	-1019740160
	.long	0
	.long	-1019740160
	.align 32
.LC25:
	.long	477218589
	.long	1050440135
	.long	477218589
	.long	1050440135
	.long	477218589
	.long	1050440135
	.long	477218589
	.long	1050440135
	.align 32
.LC26:
	.long	0
	.long	1094844416
	.long	0
	.long	1094844416
	.long	0
	.long	1094844416
	.long	0
	.long	1094844416
	.align 32
.LC27:
	.long	1431655766
	.long	1061508437
	.long	1431655766
	.long	1061508437
	.long	1431655766
	.long	1061508437
	.long	1431655766
	.long	1061508437
	.align 32
.LC28:
	.long	0
	.long	1083703296
	.long	0
	.long	1083703296
	.long	0
	.long	1083703296
	.long	0
	.long	1083703296
	.align 32
.LC29:
	.long	1321312604
	.long	1061513003
	.long	1321312604
	.long	1061513003
	.long	1321312604
	.long	1061513003
	.long	1321312604
	.long	1061513003
	.align 32
.LC30:
	.long	0
	.long	1083698176
	.long	0
	.long	1083698176
	.long	0
	.long	1083698176
	.long	0
	.long	1083698176
	.align 32
.LC31:
	.long	0
	.long	1074266112
	.long	0
	.long	1074266112
	.long	0
	.long	1074266112
	.long	0
	.long	1074266112
	.align 32
.LC32:
	.long	0
	.long	-1063129600
	.long	0
	.long	-1063129600
	.long	0
	.long	-1063129600
	.long	0
	.long	-1063129600
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
