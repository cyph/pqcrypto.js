	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_1013x2393)
	.type	CRYPTO_NAMESPACE(crypto_encode_1013x2393), @function
CRYPTO_NAMESPACE(crypto_encode_1013x2393):
.LFB5342:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r8
	movl	$32, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$904, %rsp
	.cfi_offset 3, -24
	leaq	-110(%rsp), %rdx
	movq	%rdx, %r11
	vmovdqa	.LC0(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm6
	vmovdqa	.LC2(%rip), %ymm5
	vmovdqa	.LC3(%rip), %ymm1
	vmovdqa	.LC4(%rip), %ymm4
	vmovdqa	.LC5(%rip), %ymm3
.L3:
	decq	%rcx
	jne	.L2
	subq	$24, %rsi
	subq	$12, %rdx
	subq	$12, %rdi
.L2:
	vpaddw	(%rsi), %ymm7, %ymm9
	addq	$32, %rdx
	addq	$64, %rsi
	addq	$32, %rdi
	vpaddw	-32(%rsi), %ymm7, %ymm8
	vpand	%ymm6, %ymm9, %ymm9
	vpand	%ymm6, %ymm8, %ymm8
	vpmulhw	%ymm5, %ymm9, %ymm9
	vpmulhw	%ymm5, %ymm8, %ymm8
	vpsrld	$16, %ymm9, %ymm0
	vpand	%ymm1, %ymm9, %ymm9
	vpsrld	$16, %ymm8, %ymm2
	vpmulld	%ymm4, %ymm0, %ymm0
	vpand	%ymm1, %ymm8, %ymm8
	vpmulld	%ymm4, %ymm2, %ymm2
	vpaddd	%ymm9, %ymm0, %ymm0
	vpaddd	%ymm8, %ymm2, %ymm2
	vpshufb	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpermq	$216, %ymm2, %ymm2
	vperm2i128	$49, %ymm2, %ymm0, %ymm8
	vinserti128	$1, %xmm2, %ymm0, %ymm0
	vmovdqu	%ymm8, -32(%rdx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rcx, %rcx
	jne	.L3
	movswl	2024(%r8), %edx
	addl	$3588, %edx
	andl	$16383, %edx
	imull	$10923, %edx, %edx
	sarl	$15, %edx
	movw	%dx, 902(%rsp)
.L4:
	movzwl	2(%r11,%rcx,4), %esi
	movzwl	(%r11,%rcx,4), %r8d
	imull	$88, %esi, %esi
	addl	%r8d, %esi
	movw	%si, (%r11,%rcx,2)
	incq	%rcx
	cmpq	$253, %rcx
	jne	.L4
	vmovdqa	.LC6(%rip), %ymm5
	movw	%dx, 396(%rsp)
	movq	%r11, %rcx
	movq	%r11, %rdx
	movl	$8, %esi
.L6:
	decq	%rsi
	jne	.L5
	subq	$4, %rdx
	subq	$2, %rcx
	subq	$2, %rdi
.L5:
	vmovdqu	(%rdx), %ymm7
	vpand	(%rdx), %ymm1, %ymm4
	addq	$32, %rcx
	addq	$64, %rdx
	addq	$32, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rdx), %ymm7
	vpmulld	%ymm5, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm2
	vpmulld	%ymm5, %ymm2, %ymm2
	vpaddd	%ymm4, %ymm0, %ymm0
	vpand	-32(%rdx), %ymm1, %ymm4
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vperm2i128	$49, %ymm2, %ymm0, %ymm4
	vinserti128	$1, %xmm2, %ymm0, %ymm0
	vmovdqu	%ymm4, -32(%rcx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rsi, %rsi
	jne	.L6
	vmovdqa	.LC7(%rip), %ymm5
	movq	%r11, %rcx
	movq	%r11, %rsi
	movl	$8, %edx
	vmovdqa	.LC8(%rip), %ymm2
.L8:
	decq	%rdx
	jne	.L7
	subq	$4, %rsi
	subq	$2, %rcx
	decq	%rdi
.L7:
	vmovdqu	(%rsi), %ymm6
	vpand	(%rsi), %ymm1, %ymm4
	addq	$16, %rcx
	addq	$32, %rsi
	addq	$8, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rcx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%rdx, %rdx
	jne	.L8
	movl	$2, %r9d
	movw	142(%rsp), %cx
	movq	%r11, %r8
	vmovdqa	.LC9(%rip), %ymm6
	movl	$2, %r10d
	movw	%cx, 16(%rsp)
	movq	%r11, %rcx
.L10:
	decq	%r10
	movq	%rdi, %rsi
	jne	.L9
	subq	$4, %rcx
	subq	$2, %r8
	subq	$2, %rsi
.L9:
	vmovdqu	(%rcx), %ymm7
	vpand	(%rcx), %ymm1, %ymm5
	addq	$32, %r8
	addq	$64, %rcx
	leaq	32(%rsi), %rdi
	movl	$1, %r10d
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rcx), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rcx), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%r8)
	decl	%r9d
	vmovdqu	%ymm0, (%rsi)
	je	.L25
	movl	$1, %r9d
	jmp	.L10
.L25:
	movzwl	16(%rsp), %ecx
	movzwl	14(%rsp), %edi
	vmovdqa	.LC10(%rip), %ymm4
	imull	$3278, %ecx, %ecx
	addl	%edi, %ecx
	leaq	33(%rsi), %rdi
	movb	%cl, 32(%rsi)
	shrl	$8, %ecx
	xorl	%esi, %esi
	movw	%cx, -48(%rsp)
.L11:
	vmovdqu	(%r11,%rsi,2), %ymm6
	vpand	(%r11,%rsi,2), %ymm1, %ymm3
	movq	%rdi, %rcx
	addq	$8, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%r11,%rsi)
	vextracti128	$0x1, %ymm0, %xmm0
	addq	$16, %rsi
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	cmpq	$32, %rsi
	jne	.L11
	xorl	%esi, %esi
.L12:
	movzwl	2(%r11,%rsi,4), %r8d
	movzwl	(%r11,%rsi,4), %r9d
	imull	$106, %r8d, %r8d
	addl	%r9d, %r8d
	movw	%r8w, (%r11,%rsi,2)
	incq	%rsi
	cmpq	$7, %rsi
	jne	.L12
	movzwl	-80(%rsp), %esi
	movzwl	-82(%rsp), %r8d
	imull	$106, %esi, %esi
	addl	%r8d, %esi
	movb	%sil, (%rdi)
	shrl	$8, %esi
	movw	%si, -96(%rsp)
.L13:
	movzwl	2(%r11,%rdx,4), %ebx
	movzwl	(%r11,%rdx,4), %esi
	imull	$11236, %ebx, %ebx
	addl	%esi, %ebx
	movb	%bl, 9(%rcx,%rdx,2)
	movb	%bh, 10(%rcx,%rdx,2)
	shrl	$16, %ebx
	movw	%bx, (%r11,%rdx,2)
	incq	%rdx
	cmpq	$4, %rdx
	jne	.L13
	movzwl	-108(%rsp), %edx
	movzwl	-110(%rsp), %eax
	movzwl	-106(%rsp), %esi
	imull	$1927, %edx, %edx
	addl	%eax, %edx
	movzwl	-104(%rsp), %eax
	movb	%dl, 17(%rcx)
	shrl	$8, %edx
	imull	$1927, %eax, %eax
	movzwl	%dx, %edx
	addl	%esi, %eax
	movb	%al, 18(%rcx)
	shrl	$8, %eax
	movzwl	%ax, %eax
	imull	$14506, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 19(%rcx)
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_1013x2393), .-CRYPTO_NAMESPACE(crypto_encode_1013x2393)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.align 32
.LC1:
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.align 32
.LC2:
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.align 32
.LC3:
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.align 32
.LC4:
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.align 32
.LC5:
	.byte	0
	.byte	1
	.byte	4
	.byte	5
	.byte	8
	.byte	9
	.byte	12
	.byte	13
	.byte	2
	.byte	3
	.byte	6
	.byte	7
	.byte	10
	.byte	11
	.byte	14
	.byte	15
	.byte	0
	.byte	1
	.byte	4
	.byte	5
	.byte	8
	.byte	9
	.byte	12
	.byte	13
	.byte	2
	.byte	3
	.byte	6
	.byte	7
	.byte	10
	.byte	11
	.byte	14
	.byte	15
	.align 32
.LC6:
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.align 32
.LC7:
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.align 32
.LC8:
	.byte	1
	.byte	2
	.byte	5
	.byte	6
	.byte	9
	.byte	10
	.byte	13
	.byte	14
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	1
	.byte	2
	.byte	5
	.byte	6
	.byte	9
	.byte	10
	.byte	13
	.byte	14
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.align 32
.LC9:
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.align 32
.LC10:
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
