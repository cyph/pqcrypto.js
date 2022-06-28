	.file	"rq.c"
	.text
	.p2align 4,,15
	.globl	rq_encode
	.type	rq_encode, @function
rq_encode:
.LFB5180:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	leaq	1216(%rdi), %r9
	movq	%rsi, %rcx
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rax
	.p2align 4,,10
	.p2align 3
.L2:
	movswl	2(%rcx), %edx
	movswl	(%rcx), %r8d
	leal	6885(%rdx,%rdx,2), %edx
	sall	$11, %edx
	movswl	4(%rcx), %r14d
	movswl	6(%rcx), %r11d
	leal	2295(%r8,%rdx), %edx
	addq	$10, %rcx
	movswl	-2(%rcx), %r10d
	movb	%dl, (%rax)
	movb	%dh, 1(%rax)
	sarl	$16, %edx
	movl	%edx, %r8d
	leal	20655(%r14,%r14,8), %edx
	sall	$6, %edx
	addl	%r8d, %edx
	leal	6885(%r11,%r11,2), %r8d
	movb	%dl, 2(%rax)
	movb	%dh, 3(%rax)
	leal	(%r8,%r8,8), %r8d
	sarl	$16, %edx
	leal	(%rdx,%r8,2), %edx
	movb	%dl, 4(%rax)
	sarl	$8, %edx
	movl	%edx, %r8d
	leal	20655(%r10,%r10,8), %edx
	leal	(%rdx,%rdx,8), %edx
	sall	$4, %edx
	addl	%r8d, %edx
	movb	%dl, 5(%rax)
	movb	%dh, 6(%rax)
	addq	$8, %rax
	sarl	$16, %edx
	movb	%dl, -1(%rax)
	cmpq	%rax, %r9
	jne	.L2
	movswl	1520(%rsi), %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	addl	$2295, %eax
	movw	%ax, 1216(%rdi)
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE5180:
	.size	rq_encode, .-rq_encode
	.p2align 4,,15
	.globl	rq_decode
	.type	rq_decode, @function
rq_decode:
.LFB5181:
	.cfi_startproc
	vmovdqa	.LC0(%rip), %ymm14
	vmovapd	.LC1(%rip), %ymm13
	vmovapd	.LC2(%rip), %ymm15
	vmovapd	.LC11(%rip), %ymm11
	vmovapd	.LC12(%rip), %ymm10
	vmovdqa	.LC13(%rip), %xmm12
	leaq	1520(%rdi), %r8
	movq	%rsi, %rcx
	movq	%rdi, %rax
	.p2align 4,,10
	.p2align 3
.L7:
	vmovdqu	(%rcx), %ymm2
	addq	$40, %rax
	vpunpckldq	%ymm14, %ymm2, %ymm0
	vpunpckhdq	%ymm14, %ymm2, %ymm2
	vpunpckhqdq	%ymm2, %ymm0, %ymm1
	vaddpd	%ymm1, %ymm13, %ymm1
	vpunpcklqdq	%ymm2, %ymm0, %ymm0
	vaddpd	%ymm0, %ymm13, %ymm2
	vmulpd	%ymm15, %ymm1, %ymm7
	addq	$32, %rcx
	vroundpd	$1, %ymm7, %ymm7
	vfnmadd231pd	.LC3(%rip), %ymm7, %ymm1
	vmulpd	%ymm11, %ymm7, %ymm3
	vmulpd	.LC4(%rip), %ymm1, %ymm6
	vroundpd	$1, %ymm3, %ymm3
	vfnmadd132pd	%ymm10, %ymm7, %ymm3
	vroundpd	$1, %ymm6, %ymm6
	vfnmadd231pd	.LC5(%rip), %ymm6, %ymm1
	vmulpd	%ymm11, %ymm6, %ymm5
	vcvtpd2dqy	%ymm3, %xmm3
	vmovapd	%ymm1, %ymm0
	vfmadd132pd	.LC6(%rip), %ymm2, %ymm0
	vroundpd	$1, %ymm5, %ymm5
	vfnmadd132pd	%ymm10, %ymm6, %ymm5
	vmulpd	.LC7(%rip), %ymm0, %ymm9
	vcvtpd2dqy	%ymm5, %xmm5
	vroundpd	$1, %ymm9, %ymm9
	vfnmadd231pd	.LC8(%rip), %ymm9, %ymm0
	vmulpd	%ymm11, %ymm9, %ymm2
	vmulpd	.LC9(%rip), %ymm0, %ymm4
	vroundpd	$1, %ymm2, %ymm2
	vfnmadd132pd	%ymm10, %ymm9, %ymm2
	vroundpd	$1, %ymm4, %ymm8
	vfnmadd231pd	.LC10(%rip), %ymm8, %ymm0
	vmulpd	%ymm11, %ymm8, %ymm4
	vcvtpd2dqy	%ymm2, %xmm2
	vpackssdw	%xmm5, %xmm2, %xmm2
	vmulpd	%ymm11, %ymm0, %ymm1
	vroundpd	$1, %ymm4, %ymm4
	vfnmadd132pd	%ymm10, %ymm8, %ymm4
	vroundpd	$1, %ymm1, %ymm1
	vfnmadd231pd	%ymm10, %ymm1, %ymm0
	vpaddw	%xmm3, %xmm12, %xmm1
	vmovd	%xmm1, %edx
	movw	%dx, -32(%rax)
	vpextrd	$1, %xmm1, %edx
	movw	%dx, -22(%rax)
	vcvtpd2dqy	%ymm4, %xmm4
	vpextrd	$2, %xmm1, %edx
	vcvtpd2dqy	%ymm0, %xmm0
	vpackssdw	%xmm4, %xmm0, %xmm0
	movw	%dx, -12(%rax)
	vpextrd	$3, %xmm1, %edx
	vpunpcklwd	%xmm2, %xmm0, %xmm1
	vpunpckhwd	%xmm2, %xmm0, %xmm0
	vpunpcklwd	%xmm0, %xmm1, %xmm2
	vpunpckhwd	%xmm0, %xmm1, %xmm0
	vpaddw	%xmm12, %xmm0, %xmm0
	vpaddw	%xmm12, %xmm2, %xmm1
	movw	%dx, -2(%rax)
	vmovq	%xmm1, -40(%rax)
	vpextrq	$1, %xmm1, -30(%rax)
	vmovq	%xmm0, -20(%rax)
	vpextrq	$1, %xmm0, -10(%rax)
	cmpq	%r8, %rax
	jne	.L7
	movzbl	1217(%rsi), %eax
	movzbl	1216(%rsi), %edx
	sall	$8, %eax
	leal	2296(%rax,%rdx), %edx
	imull	$228, %edx, %eax
	movq	$0, 1522(%rdi)
	movl	$0, 1530(%rdi)
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
.LFE5181:
	.size	rq_decode, .-rq_decode
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
	.ident	"GCC: (GNU) 8.2.1 20181105 (Red Hat 8.2.1-5)"
	.section	.note.GNU-stack,"",@progbits
