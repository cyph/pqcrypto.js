	.file	"rq.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(rq_encode)
	.type	CRYPTO_NAMESPACE(rq_encode), @function
CRYPTO_NAMESPACE(rq_encode):
.LFB5289:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	leaq	1216(%rdi), %r9
	movq	%rsi, %rcx
	movq	%rdi, %rax
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	.p2align 4,,10
	.p2align 3
.L2:
	movswl	2(%rcx), %edx
	movswl	(%rcx), %r8d
	addq	$10, %rcx
	addq	$8, %rax
	movswl	-6(%rcx), %r14d
	movswl	-4(%rcx), %r11d
	leal	6885(%rdx,%rdx,2), %edx
	movswl	-2(%rcx), %r10d
	sall	$11, %edx
	leal	2295(%r8,%rdx), %edx
	movb	%dl, -8(%rax)
	movb	%dh, -7(%rax)
	sarl	$16, %edx
	movl	%edx, %r8d
	leal	20655(%r14,%r14,8), %edx
	sall	$6, %edx
	addl	%r8d, %edx
	leal	6885(%r11,%r11,2), %r8d
	movb	%dl, -6(%rax)
	leal	(%r8,%r8,8), %r8d
	movb	%dh, -5(%rax)
	sarl	$16, %edx
	leal	(%rdx,%r8,2), %edx
	movb	%dl, -4(%rax)
	sarl	$8, %edx
	movl	%edx, %r8d
	leal	20655(%r10,%r10,8), %edx
	leal	(%rdx,%rdx,8), %edx
	sall	$4, %edx
	addl	%r8d, %edx
	movb	%dl, -3(%rax)
	movb	%dh, -2(%rax)
	sarl	$16, %edx
	movb	%dl, -1(%rax)
	cmpq	%rax, %r9
	jne	.L2
	movswl	1520(%rsi), %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	addl	$2295, %eax
	movw	%ax, 1216(%rdi)
	ret
	.cfi_endproc
.LFE5289:
	.size	CRYPTO_NAMESPACE(rq_encode), .-CRYPTO_NAMESPACE(rq_encode)
	.p2align 4
	.globl	CRYPTO_NAMESPACE(rq_decode)
	.type	CRYPTO_NAMESPACE(rq_decode), @function
CRYPTO_NAMESPACE(rq_decode):
.LFB5290:
	.cfi_startproc
	vmovdqa	.LC0(%rip), %ymm10
	vmovapd	.LC1(%rip), %ymm9
	leaq	1520(%rdi), %r8
	movq	%rsi, %rcx
	vmovapd	.LC2(%rip), %ymm15
	vmovapd	.LC3(%rip), %ymm14
	movq	%rdi, %rax
	vmovapd	.LC4(%rip), %ymm13
	vmovapd	.LC5(%rip), %ymm12
	vmovapd	.LC6(%rip), %ymm11
	vmovapd	.LC11(%rip), %ymm7
	vmovapd	.LC12(%rip), %ymm6
	vmovdqa	.LC13(%rip), %xmm8
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	(%rcx), %ymm0
	addq	$40, %rax
	addq	$32, %rcx
	vpunpckldq	%ymm10, %ymm0, %ymm2
	vpunpckhdq	%ymm10, %ymm0, %ymm0
	vpunpckhqdq	%ymm0, %ymm2, %ymm1
	vpunpcklqdq	%ymm0, %ymm2, %ymm0
	vaddpd	%ymm1, %ymm9, %ymm1
	vaddpd	%ymm0, %ymm9, %ymm4
	vmulpd	%ymm15, %ymm1, %ymm2
	vroundpd	$1, %ymm2, %ymm2
	vmulpd	%ymm7, %ymm2, %ymm5
	vfnmadd231pd	%ymm14, %ymm2, %ymm1
	vmulpd	%ymm13, %ymm1, %ymm3
	vroundpd	$1, %ymm5, %ymm5
	vfnmadd231pd	%ymm6, %ymm5, %ymm2
	vroundpd	$1, %ymm3, %ymm3
	vfnmadd231pd	%ymm12, %ymm3, %ymm1
	vcvtpd2dqy	%ymm2, %xmm2
	vpaddw	%xmm2, %xmm8, %xmm2
	vmulpd	%ymm7, %ymm3, %ymm5
	vmovd	%xmm2, %edx
	movw	%dx, -32(%rax)
	vpextrd	$1, %xmm2, %edx
	movw	%dx, -22(%rax)
	vpextrd	$2, %xmm2, %edx
	movw	%dx, -12(%rax)
	vpextrd	$3, %xmm2, %edx
	vmovapd	%ymm1, %ymm0
	movw	%dx, -2(%rax)
	vfmadd132pd	%ymm11, %ymm4, %ymm0
	vroundpd	$1, %ymm5, %ymm5
	vfnmadd132pd	%ymm6, %ymm3, %ymm5
	vmulpd	.LC7(%rip), %ymm0, %ymm1
	vcvtpd2dqy	%ymm5, %xmm5
	vroundpd	$1, %ymm1, %ymm1
	vmulpd	%ymm7, %ymm1, %ymm3
	vfnmadd231pd	.LC8(%rip), %ymm1, %ymm0
	vmulpd	.LC9(%rip), %ymm0, %ymm4
	vroundpd	$1, %ymm3, %ymm3
	vfnmadd231pd	%ymm6, %ymm3, %ymm1
	vroundpd	$1, %ymm4, %ymm4
	vmulpd	%ymm7, %ymm4, %ymm3
	vfnmadd231pd	.LC10(%rip), %ymm4, %ymm0
	vcvtpd2dqy	%ymm1, %xmm1
	vpackssdw	%xmm5, %xmm1, %xmm1
	vroundpd	$1, %ymm3, %ymm3
	vfnmadd231pd	%ymm6, %ymm3, %ymm4
	vmulpd	%ymm7, %ymm0, %ymm3
	vcvtpd2dqy	%ymm4, %xmm4
	vroundpd	$1, %ymm3, %ymm3
	vfnmadd132pd	%ymm6, %ymm0, %ymm3
	vcvtpd2dqy	%ymm3, %xmm3
	vpackssdw	%xmm4, %xmm3, %xmm3
	vpunpcklwd	%xmm1, %xmm3, %xmm0
	vpunpckhwd	%xmm1, %xmm3, %xmm3
	vpunpcklwd	%xmm3, %xmm0, %xmm1
	vpunpckhwd	%xmm3, %xmm0, %xmm0
	vpaddw	%xmm8, %xmm1, %xmm1
	vpaddw	%xmm8, %xmm0, %xmm0
	vmovq	%xmm1, -40(%rax)
	vpextrq	$1, %xmm1, -30(%rax)
	vpextrq	$1, %xmm0, -10(%rax)
	vmovq	%xmm0, -20(%rax)
	cmpq	%r8, %rax
	jne	.L7
	movzbl	1217(%rsi), %eax
	movzbl	1216(%rsi), %edx
	movq	$0, 1522(%rdi)
	movl	$0, 1530(%rdi)
	sall	$8, %eax
	leal	2296(%rax,%rdx), %edx
	imull	$228, %edx, %eax
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	addl	%edx, %eax
	imull	$58470, %eax, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	movw	%ax, 1520(%rdi)
	xorl	%eax, %eax
	movw	%ax, 1534(%rdi)
	vzeroupper
	ret
	.cfi_endproc
.LFE5290:
	.size	CRYPTO_NAMESPACE(rq_decode), .-CRYPTO_NAMESPACE(rq_decode)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.long	1127743488
	.align 32
.LC1:
	.long	0
	.long	-1019740160
	.long	0
	.long	-1019740160
	.long	0
	.long	-1019740160
	.long	0
	.long	-1019740160
	.align 32
.LC2:
	.long	4241943009
	.long	1053378736
	.long	4241943009
	.long	1053378736
	.long	4241943009
	.long	1053378736
	.long	4241943009
	.long	1053378736
	.align 32
.LC3:
	.long	0
	.long	1091846144
	.long	0
	.long	1091846144
	.long	0
	.long	1091846144
	.long	0
	.long	1091846144
	.align 32
.LC4:
	.long	3181457257
	.long	1066595972
	.long	3181457257
	.long	1066595972
	.long	3181457257
	.long	1066595972
	.long	3181457257
	.long	1066595972
	.align 32
.LC5:
	.long	0
	.long	1078657024
	.long	0
	.long	1078657024
	.long	0
	.long	1078657024
	.long	0
	.long	1078657024
	.align 32
.LC6:
	.long	0
	.long	1106247680
	.long	0
	.long	1106247680
	.long	0
	.long	1106247680
	.long	0
	.long	1106247680
	.align 32
.LC7:
	.long	477218589
	.long	1046245831
	.long	477218589
	.long	1046245831
	.long	477218589
	.long	1046245831
	.long	477218589
	.long	1046245831
	.align 32
.LC8:
	.long	0
	.long	1099038720
	.long	0
	.long	1099038720
	.long	0
	.long	1099038720
	.long	0
	.long	1099038720
	.align 32
.LC9:
	.long	1431655766
	.long	1059411285
	.long	1431655766
	.long	1059411285
	.long	1431655766
	.long	1059411285
	.long	1431655766
	.long	1059411285
	.align 32
.LC10:
	.long	0
	.long	1085800448
	.long	0
	.long	1085800448
	.long	0
	.long	1085800448
	.long	0
	.long	1085800448
	.align 32
.LC11:
	.long	3484807924
	.long	1059884221
	.long	3484807924
	.long	1059884221
	.long	3484807924
	.long	1059884221
	.long	3484807924
	.long	1059884221
	.align 32
.LC12:
	.long	0
	.long	1085402880
	.long	0
	.long	1085402880
	.long	0
	.long	1085402880
	.long	0
	.long	1085402880
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC13:
	.value	-2295
	.value	-2295
	.value	-2295
	.value	-2295
	.value	-2295
	.value	-2295
	.value	-2295
	.value	-2295
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
