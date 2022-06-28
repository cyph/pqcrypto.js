	.file	"rq_rounded.c"
	.text
	.p2align 4,,15
	.globl	rq_roundencode
	.type	rq_roundencode, @function
rq_roundencode:
.LFB5180:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	832(%rdi), %rcx
	movq	%rsi, %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	subq	$1608, %rsp
	movq	%rdi, %rdx
	vmovdqa	.LC0(%rip), %ymm3
	vmovdqa	.LC1(%rip), %ymm4
	vmovdqa	.LC2(%rip), %ymm5
	.p2align 4,,10
	.p2align 3
.L2:
	vmovdqu	(%rax), %xmm0
	vmovdqu	32(%rax), %xmm2
	vmovdqu	16(%rax), %xmm1
	vinserti128	$0x1, 48(%rax), %ymm0, %ymm0
	vinserti128	$0x1, 80(%rax), %ymm2, %ymm2
	vinserti128	$0x1, 64(%rax), %ymm1, %ymm1
	vpmulhrsw	%ymm3, %ymm2, %ymm2
	vpmulhrsw	%ymm3, %ymm0, %ymm0
	vpmulhrsw	%ymm3, %ymm1, %ymm1
	addq	$64, %rdx
	addq	$96, %rax
	vpblendw	$240, %ymm0, %ymm2, %ymm6
	vpshufd	$78, %ymm6, %ymm6
	vpblendw	$240, %ymm1, %ymm0, %ymm0
	vpblendw	$240, %ymm2, %ymm1, %ymm1
	vpblendw	$204, %ymm0, %ymm1, %ymm2
	vpblendw	$204, %ymm6, %ymm0, %ymm0
	vpblendw	$204, %ymm1, %ymm6, %ymm6
	vpblendw	$170, %ymm0, %ymm6, %ymm1
	vpshuflw	$177, %ymm1, %ymm1
	vpshufd	$177, %ymm2, %ymm2
	vpshufhw	$177, %ymm1, %ymm1
	vpblendw	$170, %ymm2, %ymm0, %ymm0
	vpblendw	$170, %ymm6, %ymm2, %ymm2
	vpaddw	%ymm4, %ymm1, %ymm6
	vpsllw	$1, %ymm6, %ymm1
	vpaddw	%ymm4, %ymm0, %ymm0
	vpaddw	%ymm6, %ymm1, %ymm1
	vpaddw	%ymm4, %ymm2, %ymm2
	vpsrlw	$8, %ymm0, %ymm7
	vpsllw	$1, %ymm1, %ymm1
	vpsllw	$3, %ymm2, %ymm6
	vpaddw	%ymm7, %ymm1, %ymm1
	vpaddw	%ymm2, %ymm6, %ymm2
	vpsllw	$8, %ymm1, %ymm7
	vpsllw	$2, %ymm2, %ymm2
	vpsrlw	$8, %ymm1, %ymm1
	vpand	%ymm0, %ymm5, %ymm0
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
	vpmulhrsw	1440(%rsi), %ymm3, %ymm6
	vmovdqa	%ymm6, -24(%rsp)
	vmovdqa	-24(%rsp), %ymm6
	movzwl	-24(%rsp), %eax
	vmovdqa	%ymm6, 200(%rsp)
	vpmulhrsw	1472(%rsi), %ymm3, %ymm6
	vmovdqa	%ymm6, -24(%rsp)
	vmovdqa	-24(%rsp), %ymm6
	vpmulhrsw	1376(%rsi), %ymm3, %ymm4
	vmovdqa	%ymm4, -88(%rsp)
	vmovdqa	.LC3(%rip), %ymm4
	movswl	-24(%rsp), %ecx
	vmovdqa	%ymm6, 232(%rsp)
	vmovdqa	.LC4(%rip), %ymm1
	vpmulhrsw	1504(%rsi), %ymm3, %ymm6
	vmovdqa	%ymm6, -24(%rsp)
	vpmulhrsw	1344(%rsi), %ymm3, %ymm5
	vmovdqa	-24(%rsp), %ymm6
	vmovdqa	%ymm5, -56(%rsp)
	vmovdqa	.LC5(%rip), %ymm13
	vpmulhrsw	1248(%rsi), %ymm3, %ymm5
	vpshufb	%ymm4, %ymm5, %ymm0
	vpermq	$78, %ymm0, %ymm2
	vpshufb	%ymm1, %ymm5, %ymm0
	vmovdqa	.LC7(%rip), %ymm12
	vpmulhrsw	1408(%rsi), %ymm3, %ymm7
	vmovdqa	%ymm6, 264(%rsp)
	vmovdqa	%ymm7, -120(%rsp)
	vpmulhrsw	1280(%rsi), %ymm3, %ymm6
	vpor	%ymm2, %ymm0, %ymm0
	vpmulhrsw	1312(%rsi), %ymm3, %ymm3
	vpshufb	%ymm13, %ymm6, %ymm7
	vpshufb	.LC6(%rip), %ymm3, %ymm2
	vpor	%ymm7, %ymm0, %ymm0
	vpermq	$78, %ymm2, %ymm7
	vpshufb	.LC8(%rip), %ymm3, %ymm2
	vpor	%ymm7, %ymm2, %ymm2
	vpshufb	%ymm12, %ymm0, %ymm0
	vpshufb	.LC9(%rip), %ymm5, %ymm7
	vpor	%ymm2, %ymm0, %ymm0
	vpermq	$78, %ymm7, %ymm7
	vpshufb	.LC10(%rip), %ymm5, %ymm2
	movzwl	-24(%rsp), %edx
	vpor	%ymm7, %ymm2, %ymm7
	vpshufb	.LC11(%rip), %ymm6, %ymm8
	vpor	%ymm8, %ymm7, %ymm7
	vpshufb	.LC12(%rip), %ymm3, %ymm8
	vpshufb	.LC13(%rip), %ymm3, %ymm2
	vpermq	$78, %ymm8, %ymm8
	vpor	%ymm8, %ymm2, %ymm2
	vpshufb	%ymm12, %ymm7, %ymm7
	vpor	%ymm2, %ymm7, %ymm7
	vpshufb	.LC14(%rip), %ymm5, %ymm2
	vmovdqa	.LC19(%rip), %ymm14
	vpermq	$78, %ymm2, %ymm2
	vpshufb	.LC15(%rip), %ymm5, %ymm5
	vpor	%ymm2, %ymm5, %ymm5
	vmovdqa	.LC18(%rip), %ymm15
	vpshufb	.LC17(%rip), %ymm3, %ymm2
	vpshufb	.LC16(%rip), %ymm6, %ymm6
	vpor	%ymm6, %ymm5, %ymm6
	vpermq	$78, %ymm2, %ymm2
	vpshufb	%ymm14, %ymm3, %ymm3
	vmovdqa	.LC20(%rip), %ymm10
	vmovdqa	.LC21(%rip), %ymm11
	vpor	%ymm2, %ymm3, %ymm3
	vpshufb	%ymm15, %ymm6, %ymm6
	vpor	%ymm3, %ymm6, %ymm6
	vpmullw	%ymm6, %ymm11, %ymm8
	vpmullw	%ymm7, %ymm10, %ymm3
	vpmulhw	%ymm10, %ymm7, %ymm2
	vpmulhw	%ymm11, %ymm6, %ymm6
	vmovdqa	.LC22(%rip), %ymm9
	movswl	%ax, %esi
	movswl	202(%rsp), %eax
	vpunpckhwd	%ymm2, %ymm3, %ymm7
	vpunpcklwd	%ymm2, %ymm3, %ymm5
	vpunpcklwd	%ymm6, %ymm8, %ymm2
	vpunpckhwd	%ymm6, %ymm8, %ymm6
	vperm2i128	$32, %ymm7, %ymm5, %ymm3
	vperm2i128	$32, %ymm6, %ymm2, %ymm8
	vperm2i128	$49, %ymm7, %ymm5, %ymm5
	vperm2i128	$49, %ymm6, %ymm2, %ymm2
	vpslld	$18, %ymm8, %ymm8
	vpslld	$18, %ymm2, %ymm2
	vpslld	$9, %ymm3, %ymm3
	vpslld	$9, %ymm5, %ymm5
	vpaddd	%ymm8, %ymm3, %ymm3
	vpaddd	%ymm2, %ymm5, %ymm5
	vpmovsxwd	%xmm0, %ymm8
	vextracti128	$0x1, %ymm0, %xmm2
	vpaddd	%ymm9, %ymm8, %ymm8
	vmovdqa	-56(%rsp), %ymm0
	vpmovsxwd	%xmm2, %ymm2
	vpaddd	%ymm8, %ymm3, %ymm3
	vpaddd	%ymm9, %ymm2, %ymm2
	vmovdqu	%ymm3, 832(%rdi)
	vpaddd	%ymm2, %ymm5, %ymm5
	vmovdqa	-88(%rsp), %ymm3
	vmovdqa	-120(%rsp), %ymm2
	vpshufb	%ymm4, %ymm0, %ymm4
	vpermq	$78, %ymm4, %ymm4
	vpshufb	%ymm1, %ymm0, %ymm1
	vmovdqu	%ymm5, 864(%rdi)
	vpshufb	%ymm13, %ymm3, %ymm13
	vpshufb	.LC6(%rip), %ymm2, %ymm5
	vpor	%ymm4, %ymm1, %ymm1
	vpor	%ymm13, %ymm1, %ymm1
	vpshufb	.LC8(%rip), %ymm2, %ymm4
	vpermq	$78, %ymm5, %ymm5
	vpshufb	%ymm12, %ymm1, %ymm1
	vpor	%ymm5, %ymm4, %ymm4
	vpshufb	.LC9(%rip), %ymm0, %ymm5
	vpor	%ymm4, %ymm1, %ymm4
	vpshufb	.LC12(%rip), %ymm2, %ymm7
	vpshufb	.LC10(%rip), %ymm0, %ymm1
	vpermq	$78, %ymm5, %ymm5
	vpor	%ymm5, %ymm1, %ymm1
	vpermq	$78, %ymm7, %ymm7
	vpshufb	.LC13(%rip), %ymm2, %ymm5
	vpor	%ymm7, %ymm5, %ymm7
	vpshufb	.LC14(%rip), %ymm0, %ymm5
	vpermq	$78, %ymm5, %ymm5
	vpshufb	.LC15(%rip), %ymm0, %ymm0
	vpshufb	.LC11(%rip), %ymm3, %ymm6
	vpor	%ymm5, %ymm0, %ymm0
	vpshufb	.LC16(%rip), %ymm3, %ymm3
	vpor	%ymm3, %ymm0, %ymm3
	vpshufb	.LC17(%rip), %ymm2, %ymm0
	vpor	%ymm6, %ymm1, %ymm1
	vpermq	$78, %ymm0, %ymm0
	vpshufb	%ymm14, %ymm2, %ymm2
	vpor	%ymm0, %ymm2, %ymm2
	vpshufb	%ymm12, %ymm1, %ymm1
	vpshufb	%ymm15, %ymm3, %ymm15
	vpor	%ymm7, %ymm1, %ymm1
	vpor	%ymm2, %ymm15, %ymm15
	vpmullw	%ymm15, %ymm11, %ymm2
	vpmullw	%ymm1, %ymm10, %ymm5
	vpmulhw	%ymm11, %ymm15, %ymm11
	vpmulhw	%ymm10, %ymm1, %ymm10
	movswl	204(%rsp), %r8d
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	vpunpcklwd	%ymm11, %ymm2, %ymm0
	vpunpcklwd	%ymm10, %ymm5, %ymm3
	vpunpckhwd	%ymm11, %ymm2, %ymm11
	vpunpckhwd	%ymm10, %ymm5, %ymm10
	vperm2i128	$32, %ymm11, %ymm0, %ymm2
	vperm2i128	$32, %ymm10, %ymm3, %ymm1
	vpslld	$18, %ymm2, %ymm2
	vpslld	$9, %ymm1, %ymm1
	vpaddd	%ymm1, %ymm2, %ymm1
	vperm2i128	$49, %ymm11, %ymm0, %ymm0
	vpmovsxwd	%xmm4, %ymm2
	vperm2i128	$49, %ymm10, %ymm3, %ymm3
	vextracti128	$0x1, %ymm4, %xmm4
	leal	1806037245(%rsi,%rax), %esi
	vpslld	$9, %ymm3, %ymm3
	vpmovsxwd	%xmm4, %ymm4
	leal	(%r8,%r8,8), %eax
	vpslld	$18, %ymm0, %ymm0
	vpaddd	%ymm9, %ymm2, %ymm2
	vpaddd	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm9, %ymm4, %ymm9
	sall	$18, %eax
	addl	%esi, %eax
	vpaddd	%ymm2, %ymm1, %ymm1
	vpaddd	%ymm9, %ymm0, %ymm9
	vmovdqu	%ymm1, 896(%rdi)
	vmovdqu	%ymm9, 928(%rdi)
	movl	%eax, 960(%rdi)
	movswl	208(%rsp), %eax
	movswl	210(%rsp), %esi
	movswl	206(%rsp), %r8d
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	sall	$18, %eax
	addl	%r8d, %eax
	movl	%eax, 964(%rdi)
	movswl	214(%rsp), %eax
	movswl	216(%rsp), %esi
	movswl	212(%rsp), %r8d
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	sall	$18, %eax
	addl	%r8d, %eax
	movl	%eax, 968(%rdi)
	movswl	220(%rsp), %eax
	movswl	222(%rsp), %esi
	movswl	218(%rsp), %r8d
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	sall	$18, %eax
	addl	%r8d, %eax
	movl	%eax, 972(%rdi)
	movswl	226(%rsp), %eax
	movswl	228(%rsp), %esi
	movswl	224(%rsp), %r8d
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %r8d
	leal	(%rsi,%rsi,8), %eax
	sall	$18, %eax
	addl	%r8d, %eax
	movswl	234(%rsp), %esi
	movl	%eax, 976(%rdi)
	movswl	230(%rsp), %r8d
	leal	(%rcx,%rcx,2), %eax
	sall	$9, %eax
	leal	1806037245(%r8,%rax), %ecx
	leal	(%rsi,%rsi,8), %eax
	sall	$18, %eax
	addl	%ecx, %eax
	movl	%eax, 980(%rdi)
	movswl	238(%rsp), %eax
	movswl	240(%rsp), %ecx
	movswl	236(%rsp), %esi
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	sall	$18, %eax
	addl	%esi, %eax
	movl	%eax, 984(%rdi)
	movswl	244(%rsp), %eax
	movswl	246(%rsp), %ecx
	movswl	242(%rsp), %esi
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	sall	$18, %eax
	addl	%esi, %eax
	movl	%eax, 988(%rdi)
	movswl	250(%rsp), %eax
	movswl	252(%rsp), %ecx
	movswl	248(%rsp), %esi
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	sall	$18, %eax
	addl	%esi, %eax
	movl	%eax, 992(%rdi)
	movswl	254(%rsp), %esi
	movswl	256(%rsp), %eax
	movswl	258(%rsp), %ecx
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rsi,%rax), %esi
	leal	(%rcx,%rcx,8), %eax
	sall	$18, %eax
	addl	%esi, %eax
	movl	%eax, 996(%rdi)
	movswl	262(%rsp), %eax
	movswl	260(%rsp), %ecx
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rcx,%rax), %ecx
	leal	(%rdx,%rdx,8), %eax
	sall	$18, %eax
	addl	%ecx, %eax
	movl	%eax, 1000(%rdi)
	movswl	268(%rsp), %eax
	movswl	270(%rsp), %edx
	movswl	266(%rsp), %ecx
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rcx,%rax), %ecx
	leal	(%rdx,%rdx,8), %eax
	sall	$18, %eax
	addl	%ecx, %eax
	movl	%eax, 1004(%rdi)
	movswl	274(%rsp), %eax
	movswl	276(%rsp), %edx
	movswl	272(%rsp), %ecx
	leal	(%rax,%rax,2), %eax
	sall	$9, %eax
	leal	1806037245(%rcx,%rax), %ecx
	leal	(%rdx,%rdx,8), %eax
	sall	$18, %eax
	addl	%ecx, %eax
	movl	%eax, 1008(%rdi)
	movswl	280(%rsp), %eax
	movswl	278(%rsp), %edx
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
.LFE5180:
	.size	rq_roundencode, .-rq_roundencode
	.p2align 4,,15
	.globl	rq_decoderounded
	.type	rq_decoderounded, @function
rq_decoderounded:
.LFB5181:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	vmovapd	.LC29(%rip), %ymm12
	vmovapd	.LC30(%rip), %ymm11
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	vmovapd	.LC31(%rip), %ymm10
	vmovapd	.LC32(%rip), %ymm9
	pushq	%r14
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	movq	%rsi, %r15
	movq	%rdi, %r14
	pushq	%r13
	movq	%rdi, %rcx
	.cfi_offset 13, -40
	leaq	992(%rsi), %r13
	pushq	%rbx
	.cfi_offset 3, -48
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	(%rsi), %ymm4
	addq	$32, %rsi
	vpunpckldq	.LC23(%rip), %ymm4, %ymm1
	vpunpckhdq	.LC23(%rip), %ymm4, %ymm4
	vaddpd	.LC24(%rip), %ymm1, %ymm1
	vaddpd	.LC24(%rip), %ymm4, %ymm4
	addq	$48, %rcx
	vmulpd	.LC25(%rip), %ymm1, %ymm6
	vroundpd	$1, %ymm6, %ymm3
	vfnmadd231pd	.LC26(%rip), %ymm3, %ymm1
	vmulpd	%ymm12, %ymm3, %ymm6
	vmulpd	.LC27(%rip), %ymm1, %ymm0
	vroundpd	$1, %ymm6, %ymm6
	vfnmadd132pd	%ymm11, %ymm3, %ymm6
	vroundpd	$1, %ymm0, %ymm2
	vfnmadd231pd	.LC28(%rip), %ymm2, %ymm1
	vmulpd	%ymm12, %ymm2, %ymm0
	vfmadd132pd	%ymm10, %ymm9, %ymm6
	vmulpd	%ymm12, %ymm1, %ymm5
	vroundpd	$1, %ymm0, %ymm0
	vfnmadd132pd	%ymm11, %ymm2, %ymm0
	vcvtpd2dqy	%ymm6, %xmm6
	vinsertps	$0xe, %xmm6, %xmm6, %xmm8
	vroundpd	$1, %ymm5, %ymm5
	vfnmadd132pd	%ymm11, %ymm1, %ymm5
	vmulpd	.LC25(%rip), %ymm4, %ymm1
	vfmadd132pd	%ymm10, %ymm9, %ymm0
	vpextrd	$2, %xmm6, %r9d
	vfmadd132pd	%ymm10, %ymm9, %ymm5
	vroundpd	$1, %ymm1, %ymm1
	vfnmadd231pd	.LC26(%rip), %ymm1, %ymm4
	vcvtpd2dqy	%ymm0, %xmm0
	vmovd	%xmm0, %ebx
	vcvtpd2dqy	%ymm5, %xmm5
	vinsertps	$0xe, %xmm5, %xmm5, %xmm14
	vmulpd	.LC27(%rip), %ymm4, %ymm3
	vpsrldq	$4, %xmm0, %xmm13
	vpinsrw	$1, %ebx, %xmm14, %xmm14
	vpextrd	$1, %xmm5, %ebx
	vpinsrw	$1, %ebx, %xmm8, %xmm8
	vroundpd	$1, %ymm3, %ymm2
	vmulpd	%ymm12, %ymm1, %ymm3
	vfnmadd231pd	.LC28(%rip), %ymm2, %ymm4
	vinsertps	$0xe, %xmm13, %xmm13, %xmm15
	vpextrd	$1, %xmm6, %ebx
	vpinsrw	$1, %ebx, %xmm15, %xmm13
	vroundpd	$1, %ymm3, %ymm3
	vfnmadd231pd	%ymm11, %ymm3, %ymm1
	vmulpd	%ymm12, %ymm2, %ymm3
	vpextrd	$2, %xmm5, %r10d
	vpunpckldq	%xmm8, %xmm14, %xmm14
	vmovd	%r9d, %xmm8
	vfmadd132pd	%ymm10, %ymm9, %ymm1
	vroundpd	$1, %ymm3, %ymm3
	vfnmadd132pd	%ymm11, %ymm2, %ymm3
	vmulpd	%ymm12, %ymm4, %ymm2
	vpextrd	$3, %xmm5, %r9d
	vcvtpd2dqy	%ymm1, %xmm1
	vpextrd	$3, %xmm0, %eax
	vfmadd132pd	%ymm10, %ymm9, %ymm3
	vroundpd	$1, %ymm2, %ymm2
	vfnmadd231pd	%ymm11, %ymm2, %ymm4
	vpinsrw	$1, %r9d, %xmm8, %xmm8
	vpextrd	$2, %xmm1, %edx
	vcvtpd2dqy	%ymm3, %xmm3
	vmovd	%xmm3, %ebx
	vfmadd132pd	%ymm10, %ymm9, %ymm4
	vpextrd	$1, %xmm3, %r11d
	vpextrd	$3, %xmm3, %edi
	vcvtpd2dqy	%ymm4, %xmm2
	vinsertps	$0xe, %xmm2, %xmm2, %xmm7
	vpinsrw	$1, %ebx, %xmm7, %xmm7
	vpunpckldq	%xmm7, %xmm13, %xmm15
	vinsertps	$0xe, %xmm1, %xmm1, %xmm4
	vpextrd	$1, %xmm2, %ebx
	vmovd	%r11d, %xmm7
	vmovd	%r10d, %xmm13
	vpextrd	$1, %xmm1, %r11d
	vpextrd	$2, %xmm0, %r10d
	vpinsrw	$1, %r10d, %xmm13, %xmm0
	vpinsrw	$1, %ebx, %xmm4, %xmm4
	vpinsrw	$1, %r11d, %xmm7, %xmm7
	vpunpckldq	%xmm7, %xmm4, %xmm4
	vpunpckldq	%xmm8, %xmm0, %xmm8
	vpextrd	$2, %xmm2, %r8d
	vpunpcklqdq	%xmm8, %xmm4, %xmm8
	vmovd	%eax, %xmm4
	vpextrd	$3, %xmm6, %eax
	vpinsrw	$1, %eax, %xmm4, %xmm4
	vmovd	%r8d, %xmm0
	vpextrd	$2, %xmm3, %eax
	vpinsrw	$1, %eax, %xmm0, %xmm3
	vpextrd	$3, %xmm2, %eax
	vmovd	%edx, %xmm0
	vpinsrw	$1, %eax, %xmm0, %xmm0
	vmovd	%edi, %xmm2
	vpextrd	$3, %xmm1, %eax
	vpinsrw	$1, %eax, %xmm2, %xmm2
	vpunpckldq	%xmm3, %xmm4, %xmm1
	vpunpckldq	%xmm2, %xmm0, %xmm0
	vpunpcklqdq	%xmm15, %xmm14, %xmm14
	vpunpcklqdq	%xmm0, %xmm1, %xmm0
	vmovups	%xmm14, -48(%rcx)
	vmovups	%xmm8, -32(%rcx)
	vmovups	%xmm0, -16(%rcx)
	cmpq	%r13, %rsi
	jne	.L7
	leaq	1488(%r14), %rdx
	leaq	1012(%r15), %rcx
.L8:
	movzbl	1(%r13), %r10d
	movzbl	2(%r13), %r8d
	movzbl	0(%r13), %r9d
	imull	$228, %r10d, %eax
	imull	$58254, %r8d, %edi
	addq	$4, %r13
	movzbl	-1(%r13), %esi
	leal	456(%rdi,%rax), %edi
	imull	$14913081, %esi, %eax
	addl	%eax, %edi
	shrl	$21, %edi
	movl	%esi, %eax
	sall	$8, %eax
	leal	(%rdi,%rdi,8), %esi
	addl	%r8d, %eax
	sall	$2, %esi
	subl	%esi, %eax
	movl	%eax, %esi
	sall	$8, %esi
	leal	1(%r9), %r8d
	addl	%r10d, %esi
	imull	$1365, %r8d, %r8d
	imull	$349525, %r10d, %r10d
	imull	$89478485, %eax, %eax
	addl	%r10d, %r8d
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
	cmpq	%r13, %rcx
	jne	.L8
	movzbl	1014(%r15), %eax
	movzbl	1013(%r15), %ecx
	movl	%eax, %edx
	sall	$8, %edx
	addl	%ecx, %edx
	imull	$89478485, %eax, %eax
	imull	$349525, %ecx, %ecx
	movzbl	1012(%r15), %esi
	movq	$0, 1522(%r14)
	addl	%ecx, %eax
	leal	1(%rsi), %ecx
	imull	$1365, %ecx, %ecx
	movl	$0, 1530(%r14)
	addl	%ecx, %eax
	shrl	$21, %eax
	leal	(%rax,%rax,2), %ecx
	leal	(%rcx,%rcx), %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$8, %eax
	addl	%esi, %eax
	leal	2296(%rax,%rax,2), %edx
	imull	$228, %edx, %eax
	addl	$2296, %ecx
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	imull	$58470, %eax, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	movw	%ax, 1518(%r14)
	imull	$228, %ecx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%ecx, %eax
	imull	$58470, %eax, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	movw	%ax, 1520(%r14)
	xorl	%eax, %eax
	movw	%ax, 1534(%r14)
	vzeroupper
	popq	%rbx
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5181:
	.size	rq_decoderounded, .-rq_decoderounded
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
	.ident	"GCC: (GNU) 8.2.1 20181105 (Red Hat 8.2.1-5)"
	.section	.note.GNU-stack,"",@progbits
